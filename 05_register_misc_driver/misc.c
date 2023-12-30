#include <linux/init.h>
#include <linux/module.h>
// 要多包含註冊雜項設備用到的結構體、函數和macro
#include <linux/miscdevice.h>
#include <linux/fs.h>


// fs.h中定義的結構體
struct file_operations misc_fops = {
    .owner = THIS_MODULE
    // 還有很多成員，這次只是簡單的操作如何註冊雜項驅動
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


// 加載module時，會印出hello world
static int misc_init( void ) {
    int ret;

    // 註冊驅動
    // 函數定義在miscdevice.h中，參數為miscdevice結構體
    ret = misc_register( &misc_dev );
    if ( ret < 0 ) {
        printk( "misc registeration error" );
        return -1;
    } // if

    printk( "misc registeration sucess\n" );
    return 0;
} // int

// 移除module時，會印出bye bye
static void misc_exit( void ) {
    // 取消註冊驅動
    // 函數定義在miscdevice.h中，參數為miscdevice結構體
    misc_deregister( &misc_dev );

    printk( "bye bye\n" );
} // int


// 驅動module的入口和出口
module_init( misc_init );
module_exit( misc_exit );

// 通知kernel這個module有許可證
MODULE_LICENSE( "GPL" );