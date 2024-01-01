// 包含一些常用到的macro和函數
#include <linux/init.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/kdev_t.h>
#include <linux/cdev.h>

// 設備個數
#define DEVICE_NUMBER 1

// 靜態分配的設備名稱
#define DEVICE_SNAME "schrdev"

// 動態分配的設備名稱
#define DEVICE_ANAME "achrdev"

// 我們申請的第一個次設備號，通常是0
#define DEVICE_MINOR_NUMBER 0

// 要傳遞的參數
static int major_num, minor_num;

// 定義一個cdev結構體
struct cdev cdev;

// 簡單寫一個function來測試驅動
int chrdev_open( struct inode * inode, struct file * file ) {
    printk( "chrdev_open success\n" );
    return 0;
} // int

// 定義一個file operations結構體
struct file_operations chrdev_ops = {
    .owner = THIS_MODULE,
    .open = chrdev_open
};


// 若只要傳遞普通參數
// 第一個是參數名稱
// 第二個是類型
// 第三個是權限，可以使用定義好的macro，S_IRUSR是00400表示8進位的0400
module_param( major_num, int, S_IRUSR );
module_param( minor_num, int, S_IRUSR );



// 加載module時，會印出hello world
static int hello_init( void ) {
    printk( "hello init\n" );
    

    int ret;

    // 用來儲存設備號
    dev_t dev_num;

    // 用參數的值來判斷
    // 若不為0，代表有傳參數，使用靜態分配
    if ( major_num ) {

        // 將主設備號和次設備號存到dev_t中
        dev_num = MKDEV(major_num,minor_num);

        // macro定義在上面
        ret = register_chrdev_region( dev_num, DEVICE_NUMBER, DEVICE_SNAME );
        if ( ret < 0 ) {
            printk( "static register chrdev error\n" );
            return -1;
        } // if

        printk( "static register chrdev success\n" );
    } // if
    // 若為0，代表沒有傳參數，使用動態分配
    else {
        // macro定義在上面
        // 記得第一個參數要用&取址，因為要讓系統自動分配，傳地址才能收到更改後的值
        ret = alloc_chrdev_region( &dev_num, DEVICE_MINOR_NUMBER, DEVICE_NUMBER, DEVICE_ANAME );
        if ( ret < 0 ) {
            printk( "dynamic register chrdev error\n" );
            return -2;
        } // if

        // 因為是使用動態分配的
        // 所以要將動態分配的主次設備號取出來
        major_num = MAJOR(dev_num);
        minor_num = MINOR(dev_num);


        printk( "dynamic register chrdev success\n" );
    } // else

    printk( "major_num = %d\n", major_num );
    printk( "minor_num = %d\n", minor_num );



    // 先聲明cdev是屬於哪個module
    cdev.owner = THIS_MODULE;
    // 初始化cdev
    cdev_init( &cdev, &chrdev_ops );

    // 註冊到kernel當中
    cdev_add( &cdev, dev_num, DEVICE_NUMBER );



    return 0;
} // int

// 移除module時，會印出bye bye
static void hello_exit( void ) {
    // 取消註冊設備號
    unregister_chrdev_region( MKDEV(major_num,minor_num), DEVICE_NUMBER );

    // 取消註冊設備，必須在unregister_chrdev_region下方
    cdev_del( &cdev );
    printk( "unregister chrdev success\n" );

    printk( "bye bye exit\n" );
} // int


// 驅動module的入口和出口
module_init( hello_init );
module_exit( hello_exit );

// 通知kernel這個module有許可證
MODULE_LICENSE( "GPL" );