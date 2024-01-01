// 包含一些常用到的macro和函數
#include <linux/init.h>
#include <linux/module.h>

// 編譯驅動之前要先確保linux kernel已經編譯過
// 並且架構要跟開發版的相同(arm)，不能是x86
// 在目錄下執行make就能夠編譯出.ko文件
// 將該文件複製到開發版下，並使用insmod helloworld.ko來插入module
// 可以使用lsmod來查看我們載入的module
// 使用rmmod helloworld來刪除module，注意不用包括.ko副檔名

// 要傳遞的參數
static int a;

static int count;
static int b[5];

// 若只要傳遞普通參數
// 第一個是參數名稱
// 第二個是類型
// 第三個是權限，可以使用定義好的macro，S_IRUSR是00400表示8進位的0400
module_param( a, int, S_IRUSR );

// 若要傳遞array
// 參數只有第三個不同，為實際傳入的參數個數
module_param_array( b, int, &count, S_IRUSR );


// 加載module時，會印出hello world
static int hello_init( void ) {
    // kernel當中是使用printk而不是printf，因為在kernel中無法使用C語言Library
    printk( "hello world\n" );
    int i;
    for ( i = 0; i < count; i++ ) {
        printk( "b[%d] = %d\n", i, b[i] );
    } // for
    printk( "count = %d\n", count );

    // printk( "a = %d\n", a );
    return 0;
} // int

// 移除module時，會印出bye bye
static void hello_exit( void ) {
    // kernel當中是使用printk而不是printf，因為在kernel中無法使用C語言Library
    printk( "bye bye\n" );

    // printk( "a = %d\n", a );

} // int


// 驅動module的入口和出口
module_init( hello_init );
module_exit( hello_exit );

// 通知kernel這個module有許可證
MODULE_LICENSE( "GPL" );