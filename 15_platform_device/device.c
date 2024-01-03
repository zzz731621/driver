// 包含一些常用到的macro和函數
#include <linux/init.h>
#include <linux/module.h>
#include <linux/platform_device.h>


void beep_release( struct device *dev ) {
    printk( "beep_release\n" );
} // void

// 描述beep使用的資源
struct resource beep_res[] = {
    [0] = {
        .start  = 0x20AC000,        // 查開發板手冊找到蜂鳴器的針腳是GPIO5_DR並且起始地址是20A_C000
        .end    = 0x20AC003,        // 查開發板手冊找到GPIO5_DR下個硬體是的起始地址是20A_C004，所以只要到20A_C003即可
        .flags   = IORESOURCE_MEM,   // 因為這邊描述的是一段物理地址，所以用這個macro
        .name   = "GPIO5_DR"
    }
};

// 描述beep的設備
struct platform_device beep_deivce = {
    .name = "beep_test",
    .id = -1,
    .resource = beep_res,
    .num_resources = ARRAY_SIZE(beep_res),
    .dev = {
        .release = beep_release // 至少要描述一個release，不定義會warning，當使用rmmod命令卸載驅動時會呼叫device_exit並且會執行該函數
    }
};


static int device_init( void ) {
    // kernel當中是使用printk而不是printf，因為在kernel中無法使用C語言Library
    printk( "hello device\n" );
    // 註冊
    return platform_device_register( &beep_deivce );
} // int

static void device_exit( void ) {
    // kernel當中是使用printk而不是printf，因為在kernel中無法使用C語言Library
    printk( "bye bye\n" );
    
    // 卸載
    platform_device_unregister( &beep_deivce );
} // int


// 驅動module的入口和出口
module_init( device_init );
module_exit( device_exit );

// 通知kernel這個module有許可證
MODULE_LICENSE( "GPL" );