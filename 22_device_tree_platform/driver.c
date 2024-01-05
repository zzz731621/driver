#include <linux/init.h>
#include <linux/module.h>
#include <linux/platform_device.h>
#include <linux/of.h>
#include <linux/of_address.h>

// 接收回傳值的變數
struct device_node * test_device_node;
struct property * test_node_property;
unsigned int * vir_gpio_dr;

// 當作參數來接收值，call by address
int size;
const char *str;
u32 out_values[2];


// 定義與設備樹節點配對的變數
const struct of_device_id of_match_table_test[] = {
    {.compatible = "test1234"},
    {}
};



int beep_probe( struct platform_device *pdev ) {
    printk( "beep probe\n" );
    int ret;

    // 和platform一樣可以直接取得資源
    // 印出節點名稱
    // printk( "node name is %s\n", pdev->dev.of_node->name );





    // 或是使用相關of函數來取得節點資源，也可以直接使用pdev->dev.of_node
    // 查找我們要的節點，為在根節點下的test節點
    test_device_node = of_find_node_by_path( "/test" );
    if ( test_device_node == NULL ) {
        printk( "of_find_node_by_path error\n" );
        return -1;
    } // if

    // 將節點名稱印出來
    printk( "test_device_node is %s\n", test_device_node->name );


    // 獲取reg屬性值
    ret = of_property_read_u32_array( test_device_node, "reg", out_values, 2 );
    if ( ret < 0 ) {
        printk( "of_property_read_u32_array error\n" );
        return -1;
    } // if

    // 將reg屬性值印出來
    printk( "out_values[0] is 0x%08x\n", out_values[0] );
    printk( "out_values[1] is 0x%08x\n", out_values[1] );


    // 物理地址的轉換
    vir_gpio_dr = of_iomap( test_device_node, 0 );
    if ( vir_gpio_dr == NULL ) {
        printk( "of_iomap error\n" );
        return -1;
    } // if
    printk( "of_iomap success\n" );


    /*****************
    註冊雜項或字符設備，可以使用17_的程式碼，流程都一樣
    註冊、完善file_operation等等
    *****************/

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