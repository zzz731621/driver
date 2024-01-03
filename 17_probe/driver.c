#include <linux/init.h>
#include <linux/module.h>
#include <linux/platform_device.h>
#include <linux/ioport.h>
#include <linux/miscdevice.h>
#include <linux/fs.h>
#include <linux/uaccess.h>
#include <linux/io.h>
/**************************08_beep_driver中的註冊雜項設備程式碼**************************/
// 定義一個指針來指向GPIO5_DR的虛擬地址
unsigned int *vir_gpio5_dr;
/**************************08_beep_driver中的註冊雜項設備程式碼**************************/


// 接收使用platform_get_resource獲取硬體資源的回傳值
struct resource *beep_mem;
// 接收使用request_mem_region登記後的回傳值
struct resource *beep_mem_tmp;


/**************************08_beep_driver中的註冊雜項設備程式碼**************************/

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
    // 應用層和內核層進行數據傳輸的方式
    // 將數據從內核層傳到應用層
    char kbuf[64] = "hehe";
    if ( copy_to_user( ubuf, kbuf, strlen( kbuf ) ) != 0 ) {
        printk( "copy_to_user error\n" );
        return -1;
    } // if



    printk( "hello misc read\n" );
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
        // 之所以要1 << 1，是因為剛剛查找手冊說明蜂鳴器的控制位是在GPIO5_01針腳
        // 所以要將第2位設置為1
        *vir_gpio5_dr |= ( 1 << 1 );
    } // if
    // 若在應用層接收到數據0
    else if ( kbuf[0] == 0 ) {
        // 控制該虛擬地址中的值，給一個低電位即可開啟蜂鳴器
        // 之所以要1 << 1，是因為剛剛查找手冊說明蜂鳴器的控制位是在GPIO5_01針腳
        // 所以要將第2位設置為0
        *vir_gpio5_dr &= ~( 1 << 1 );
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
    .name = "hello_misc",
    // 文件操作集合，傳送地址的方式來assign
    .fops = &misc_fops
    // 還有很多不常用到的成員，只要先設定上面三個即可
};
/**************************08_beep_driver中的註冊雜項設備程式碼**************************/


int beep_probe( struct platform_device *pdev ) {

    int ret;
    printk( "beep probe\n" );

    // 從device獲得硬體資源的方法
    // 方法1，直接獲得
    // printk( "beep_res name is 0x%s\n", pdev->resource[0].name );

    // 方法2，使用相關函數
    beep_mem = platform_get_resource( pdev, IORESOURCE_MEM, 0 );
    if ( beep_mem == NULL ) {
        printk( "platform_get_resource error\n" );
        return -EBUSY;
    } // if
    printk( "platform_get_resource success\n" );



    // 把device.c中struct resource的成員印出來試試看
    printk("beep_res name is %s\n", beep_mem->name);
    printk("beep_res flags is 0x%x\n", beep_mem->flags);
    printk("beep_res start is 0x%llx\n", beep_mem->start); // 記得要使用llx不能用x，因為start是unsigned 64 bits
    printk("beep_res end is 0x%llx\n", beep_mem->end); // 記得要使用llx不能用x，因為start是unsigned 64 bits

// 正常來說使用的話要向kernel登記，但我們只操作DR暫存器所以先不登記
#if 0
    // 向kernel登記
    beep_mem_tmp = request_mem_region( beep_mem->start, beep_mem->end - beep_mem->start + 1, "beep" );
    if ( beep_mem_tmp == NULL ) {
        printk( "request_mem_region error\n" );
        // 沒有登記成功必須要將資源釋放掉
        goto err_region;
    } // if
#endif

/**************************08_beep_driver中的註冊雜項設備程式碼**************************/
    // 因為不能直接訪問物理地址
    // 所以要將GPIO5_DR的物理地址映射成虛擬地址
    vir_gpio5_dr = ioremap( beep_mem->start, 4 );
    if ( vir_gpio5_dr == NULL ) {
        printk( "GPIO5_DR ioremap error\n" );
        // 系統定義的錯誤訊息
        return -EBUSY;
    } // if

    printk( "GPIO5_DR ioremap ok\n" );
/**************************08_beep_driver中的註冊雜項設備程式碼**************************/

/**************************08_beep_driver中的註冊雜項設備程式碼**************************/

    // 註冊驅動
    // 函數定義在miscdevice.h中，參數為miscdevice結構體
    ret = misc_register( &misc_dev );
    if ( ret < 0 ) {
        printk( "misc registeration error" );
        return -1;
    } // if

    printk( "misc registeration sucess\n" );
/**************************08_beep_driver中的註冊雜項設備程式碼**************************/

    return 0;

err_region:
    release_mem_region( beep_mem->start, beep_mem->end - beep_mem->start + 1 );
    return -EBUSY;
} // int

int beep_remove( struct platform_device *pdev ) {
    printk( "beep remove\n" );
    return 0;
} // int


const struct platform_device_id beep_id_table = {
    .name = "beep_test" // 隨便設定來測試是否優先使用這個NAME來進行配對
};

struct platform_driver beep_driver = {
    .probe = beep_probe,
    .remove = beep_remove,
    .driver = {
        .owner = THIS_MODULE,
        .name = "123" // 要和device.c中的name相同
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

    // 取消註冊
    platform_driver_unregister( &beep_driver );



    // 取消映射虛擬地址
    iounmap( vir_gpio5_dr );
    // 取消註冊驅動
    // 函數定義在miscdevice.h中，參數為miscdevice結構體
    misc_deregister( &misc_dev );
} // int


// 驅動module的入口和出口
module_init( beep_driver_init );
module_exit( beep_driver_exit );

// 通知kernel這個module有許可證
MODULE_LICENSE( "GPL" );