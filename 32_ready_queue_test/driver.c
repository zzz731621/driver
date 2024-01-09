#include <linux/init.h>
#include <linux/module.h>
#include <linux/platform_device.h>
#include <linux/of.h>
#include <linux/of_address.h>
#include <linux/miscdevice.h>
#include <linux/fs.h>
#include <linux/uaccess.h>
#include <linux/io.h>
#include <linux/gpio.h>
#include <linux/of_gpio.h>
#include <linux/interrupt.h>
#include <linux/of_irq.h>
#include <linux/wait.h>
#include <linux/sched.h>


// 接收回傳值的變數
struct device_node * test_device_node;
struct property * test_node_property;
unsigned int * vir_gpio_dr;
// GPIO標號
int gpio_num;
// 中斷號
int irq;

int value = 0; // 用來模擬pin的狀態
int wq_flags = 0;





// 直接使用macro來定義並初始化等待隊列頭
DECLARE_WAIT_QUEUE_HEAD(key_wq);

// 中斷處理函數
irqreturn_t test_key( int irq, void *args ) {

    // 每當收到中斷訊號
    // 將0變成1或將1變成0
    value = !value;

    // 條件設成true
    wq_flags = 1;

    // 將所有process喚醒
    // 但是實際上只有條件為true的process才會被喚醒
    // 不是所有都喚醒
    wake_up(&key_wq);


    // 回傳正常處理的macro即可
    return IRQ_HANDLED;
} // irq_handler_t


// 這個函數是我們自己定義的
// 當進行open操作時，會呼叫此函數來執行
int misc_open( struct inode * inode, struct file * file ) {
    printk( "hello misc open\n" );
    return 0;
} // int

// 這個函數是我們自己定義的
// 當進行close操作時，會呼叫此函數來執行
int misc_release( struct inode * inode, struct file * file ) {
    printk( "hello misc release, and bye bye\n" );
    return 0;
} // int

// 這個函數是我們自己定義的
// 當進行read操作時，會呼叫此函數來執行
ssize_t misc_read( struct file * file, char __user * ubuf, size_t size, loff_t * loff_t ) {
    
    // 1 等待隊列頭的指標
    // 2 判斷條件
    wait_event_interruptible(key_wq, wq_flags);

    if ( copy_to_user( ubuf, &value, sizeof( value ) ) != 0 ) {
        printk( "copy_to_user error\n" );
        return -1;
    } // if


    // 要將條件再次設為false
    // 因為只有在按鍵按下去的時候會將條件設為true
    // 而當條件被設為true之後，value = !value也必定執行過了
    // 而我們的目的是，當按鍵按下觸發中斷改變值後，才讓value印出來
    // 但應用程式是一個單純的while死循環，所以才配合等待隊列來避免CPU資源浪費
    // 若不使用會導致應用程式一直印出value重複的值
    wq_flags = 0;

    return 0;
} // int

// 這個函數是我們自己定義的
// 當進行write操作時，會呼叫此函數來執行
ssize_t misc_write(struct file * file, const char __user * ubuf, size_t size, loff_t * loff_t) {
    // 應用層和內核層進行數據傳輸的方式
    // 將數據從應用層傳到內核層
    char kbuf[64] = {0};
    if ( copy_from_user( kbuf, ubuf, size ) != 0 ) {
        printk( "copy_from_user error\n" );
        return -1;
    } // if
    // 將應用層(app執行檔)傳入的數據印出來
    printk( "kbuf is %s\n", kbuf );


    // 若在應用層接收到數據1
    if ( kbuf[0] == 1 ) {
        // 控制該虛擬地址中的值，給一個高電位即可開啟蜂鳴器
        // *vir_gpio5_dr |= ( 1 << 1 );
        // 相比於之前直接操作暫存器的值，使用gpio子系統可以透過相關函數來完成
        // 比以前來說更通用
        gpio_set_value( gpio_num, 1 );
    } // if
    // 若在應用層接收到數據0
    else if ( kbuf[0] == 0 ) {
        // 控制該虛擬地址中的值，給一個低電位即可開啟蜂鳴器
        // *vir_gpio5_dr &= ~( 1 << 1 );
        // 相比於之前直接操作暫存器的值，使用gpio子系統可以透過相關函數來完成
        // 比以前來說更通用
        gpio_set_value( gpio_num, 0 );
    } // else if

    
    printk( "hello misc write\n" );
    return 0;
} // int

// fs.h中定義的結構體
struct file_operations misc_fops = {
    .owner = THIS_MODULE,
    .open = misc_open,
    .release = misc_release,
    .read = misc_read,
    .write = misc_write
};

// miscdevice.h中定義的結構體
struct miscdevice misc_dev = {
    // 次設備號，使用動態分配，macro定義在miscdevice.h
    .minor = MISC_DYNAMIC_MINOR,
    // 設備名稱
    .name = "hello_wq",
    // 文件操作集合，傳送地址的方式來assign
    .fops = &misc_fops
    // 還有很多不常用到的成員，只要先設定上面三個即可
};

int beep_probe( struct platform_device *pdev ) {
    printk( "beep probe\n" );
    int ret;
    

    // 或是使用相關of函數來取得節點資源，也可以直接使用pdev->dev.of_node
    // 查找我們要的節點，為在根節點下的test_key節點
    test_device_node = of_find_node_by_path( "/test_key" );
    if ( test_device_node == NULL ) {
        printk( "of_find_node_by_path error\n" );
        return -1;
    } // if

    // // 申請一個gpio pin
    // ret = gpio_request( gpio_num, "beep");
    // if ( ret < 0 ) {
    //     printk( "gpio_request error\n" );
    //     return -1;
    // } // if



    // 先獲取GPIO編號，也就是把設備樹的test_key節點中的屬性"gpios"轉換成編號
    gpio_num = of_get_named_gpio( test_device_node, "gpios", 0 );
    if ( gpio_num < 0 ) {
        printk( "of_get_named_gpio error\n" );
        return -1;
    } // if

    // 把gpio設置成輸入
    gpio_direction_input( gpio_num );



    // // 獲取gpio對應的中斷號
    // // 只有在使用gpio為中斷節點才能使用
    // irq = gpio_to_irq( gpio_num );


    // 若不使用GPIO為中斷節點，有另一種方式獲取中斷號
    // 1 test_device_node為設備樹節點
    // 2 為索引值，但這次我們只有一種中斷訊息所以設0
    irq = irq_of_parse_and_map( test_device_node, 0 );



    // 印出中斷號
    printk( "irq is %d\n", irq );



    // 申請中斷
    // 1 irq 中斷號，也就是上面取得的
    // 2 test_key，上面自訂的函數
    // 3 macro表示上沿觸發
    // 4 中斷名稱
    // 5 要給test_key傳的參數，test_key的第二個參數，但因為我們沒有傳，所以為NULL
    ret = request_irq( irq, test_key, IRQF_TRIGGER_RISING, "test_key", NULL );
    if ( ret < 0 ) {
        printk( "request_irq error\n" );
        return -1;
    } // if



    // 都配置好之後就可以跟之前一樣註冊雜項/字符設備
    // 註冊雜項設備
    ret = misc_register( &misc_dev );
    if ( ret < 0 ) {
        printk( "misc registeration error" );
        return -1;
    } // if

    printk( "misc registeration sucess\n" );

    return 0;
} // int

int beep_remove( struct platform_device *pdev ) {
    printk( "beep remove\n" );
    return 0;
} // int




// 定義與設備樹節點配對的變數
const struct of_device_id of_match_table_test[] = {
    {.compatible = "keys"},
    {}
};

const struct platform_device_id beep_id_table = {
    .name = "beep_test" // 隨便設定來測試是否優先使用這個NAME來進行配對
};

struct platform_driver beep_driver = {
    .probe = beep_probe,
    .remove = beep_remove,
    .driver = {
        .owner = THIS_MODULE,
        .name = "123", 
        .of_match_table = of_match_table_test // 要使用這個成員來和設備樹節點配對
    },
    .id_table = &beep_id_table // 會優先使用這個成員的name
};


static int beep_driver_init( void ) {
    // kernel當中是使用printk而不是printf，因為在kernel中無法使用C語言Library
    printk( "hello world\n" );


    int ret;

    // 註冊
    ret = platform_driver_register( &beep_driver );
    if ( ret < 0 ) {
        printk( "register driver error\n" );
        return ret;
    } // if
    printk( "register driver success\n" );



    return 0;
} // int

static void beep_driver_exit( void ) {
    // kernel當中是使用printk而不是printf，因為在kernel中無法使用C語言Library
    printk( "bye bye\n" );


    // 釋放用不到的中斷
    // 第二個參數是傳遞給test_key的參數，但因為我們沒有傳，所以為NULL
    free_irq( irq, NULL );

    // 釋放申請的gpio pin
    gpio_free( gpio_num );

    // 取消註冊驅動
    misc_deregister( &misc_dev );

    // 取消映射虛擬地址
    iounmap( vir_gpio_dr );


    // 取消註冊
    platform_driver_unregister( &beep_driver );
} // int


// 驅動module的入口和出口
module_init( beep_driver_init );
module_exit( beep_driver_exit );

// 通知kernel這個module有許可證
MODULE_LICENSE( "GPL" );