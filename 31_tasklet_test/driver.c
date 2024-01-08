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


// 接收回傳值的變數
struct device_node * test_device_node;
struct property * test_node_property;
unsigned int * vir_gpio_dr;
// GPIO標號
int gpio_num;
// 中斷號
int irq;

// 當作參數來接收值，call by address
int size;
const char *str;
u32 out_values[2];


// tasklet用到的結構體
struct tasklet_struct key_test;


// 如果要print 100次
// 會更耗時，所以會影響到中斷實時性
// 放在中斷下部分會比較好
void test( unsigned long data ) {
    int i = 100;

    while ( i-- ) {
        printk( "test_key is %d\n", i );
    } // while
} // void


// 中斷處理函數
irqreturn_t test_key( int irq, void *args ) {
    // 相當於每按一次按鍵就印出下面
    printk( "start\n" );

    // 調用tasklet
    // 因為tasklet會延遲一個短時間再去執行綁定函數
    // 所以實際上會觀察到先印出start > end才印出我們要的
    tasklet_schedule( &key_test );

    printk( "end\n" );

    // 回傳正常處理的macro即可
    return IRQ_HANDLED;
} // irq_handler_t



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

    // 申請一個gpio pin
    ret = gpio_request( gpio_num, "beep");
    if ( ret < 0 ) {
        printk( "gpio_request error\n" );
        return -1;
    } // if



    // 先獲取GPIO編號，也就是把設備樹的test_key節點中的屬性"gpios"轉換成編號
    gpio_num = of_get_named_gpio( test_device_node, "gpios", 0 );
    if ( gpio_num < 0 ) {
        printk( "of_get_named_gpio error\n" );
        return -1;
    } // if


    // 申請一個gpio pin
    ret = gpio_request( gpio_num, "pinctrl_gpio_key1");
    if ( ret < 0 ) {
        printk( "gpio_request error\n" );
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


    // 以下使用tasklet處理中斷下部分

    // 先初始化
    // 1 指向tasklet_struct結構體的指標
    // 2 要執行的函數
    // 3 傳遞給要執行的函數的參數，這邊先設0
    tasklet_init( &key_test, test, 0 );

    // 接著調用test函數的時機在中斷上部分也就是test_key函數


    return 0;
} // int

int beep_remove( struct platform_device *pdev ) {
    printk( "beep remove\n" );
    return 0;
} // int




// 定義與設備樹節點配對的變數
const struct of_device_id of_match_table_test[] = {
    {.compatible = "test1234"},
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


    // 刪除tasklet
    tasklet_kill( &key_test );


    // 釋放申請的gpio pin
    gpio_free( gpio_num );


    

    // // 取消映射虛擬地址
    // iounmap( vir_gpio_dr );


    // 取消註冊
    platform_driver_unregister( &beep_driver );
} // int


// 驅動module的入口和出口
module_init( beep_driver_init );
module_exit( beep_driver_exit );

// 通知kernel這個module有許可證
MODULE_LICENSE( "GPL" );