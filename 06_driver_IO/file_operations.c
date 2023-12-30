#include <linux/init.h>
#include <linux/module.h>
// 要多包含註冊雜項設備用到的結構體、函數和macro
#include <linux/miscdevice.h>
#include <linux/fs.h>
#include <linux/uaccess.h>

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