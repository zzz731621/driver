#include <linux/init.h>
#include <linux/module.h>
#include <linux/platform_device.h>


int beep_probe( struct platform_device *pdev ) {
    printk( "beep probe\n" );
    return 0;
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
} // int


// 驅動module的入口和出口
module_init( beep_driver_init );
module_exit( beep_driver_exit );

// 通知kernel這個module有許可證
MODULE_LICENSE( "GPL" );