#include <linux/init.h>
#include <linux/module.h>
// 要多包含註冊雜項設備用到的結構體、函數和macro
#include <linux/miscdevice.h>
#include <linux/fs.h>
#include <linux/uaccess.h>
#include <linux/io.h>

// 定義開發板上蜂鳴器的物理地址，需要查看開發板手冊
// 但我是使用模擬的所以就先跟著做
// 去看蜂鳴器的pin名稱
// 接著搜尋pin的覆用針腳，是GPIO5
// 接著找GPIO_DR暫存器的GPIO5_DR找到對應地址
#define GPIO5_DR 0x020AC000

// 定義一個指針來指向GPIO5_DR的虛擬地址
unsigned int *vir_gpio5_dr;




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




    // 因為不能直接訪問物理地址
    // 所以要將GPIO5_DR的物理地址映射成虛擬地址
    vir_gpio5_dr = ioremap( GPIO5_DR, 4 );
    if ( vir_gpio5_dr == NULL ) {
        printk( "GPIO5_DR ioremap error\n" );
        // 系統定義的錯誤訊息
        return -EBUSY;
    } // if

    printk( "GPIO5_DR ioremap ok\n" );


    return 0;
} // int

// 移除module時，會印出bye bye
static void misc_exit( void ) {

    // 取消映射虛擬地址
    iounmap( vir_gpio5_dr );



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