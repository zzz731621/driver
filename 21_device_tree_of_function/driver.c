#include <linux/init.h>
#include <linux/module.h>
#include <linux/of.h>

// 接收回傳值的變數
struct device_node * test_device_node;
struct property * test_node_property;


// 當作參數來接收值，call by address
int size;
const char *str;
u32 out_values[2];

static int hello_init( void ) {
    printk( "hello world\n" );

    // 接收回傳值
    int ret;


    // 查找我們要的節點，為在根節點下的test節點
    test_device_node = of_find_node_by_path( "/test" );
    if ( test_device_node == NULL ) {
        printk( "of_find_node_by_path error\n" );
        return -1;
    } // if

    // 將節點名稱印出來
    printk( "test_device_node is %s\n", test_device_node->name );



    // 獲取compatible屬性值
    test_node_property = of_find_property( test_device_node, "compatible", &size );
    if ( test_node_property == NULL ) {
        printk( "of_find_property error\n" );
        return -1;
    } // if

    // 將compatible屬性名稱印出來
    printk( "test_node_property name is %s\n", test_node_property->name );
    // 將compatible屬性值印出來
    printk( "test_node_property value is %s\n", (char *)test_node_property->value );



    // 獲取reg屬性值
    ret = of_property_read_u32_array( test_device_node, "reg", out_values, 2 );
    if ( ret < 0 ) {
        printk( "of_property_read_u32_array error\n" );
        return -1;
    } // if

    // 將reg屬性值印出來
    printk( "out_values[0] is 0x%08x\n", out_values[0] );
    printk( "out_values[1] is 0x%08x\n", out_values[1] );


    // 獲取status屬性值，也可以使用of_find_property
    ret = of_property_read_string( test_device_node, "status", &str );
    if ( ret < 0 ) {
        printk( "of_property_read_string error\n" );
        return -1;
    } // if

    // 將status屬性值印出來
    printk( "status is %s\n", str );

    return 0;
} // int

static void hello_exit( void ) {
    printk( "bye bye\n" );
} // int






// 驅動module的入口和出口
module_init( hello_init );
module_exit( hello_exit );

// 通知kernel這個module有許可證
MODULE_LICENSE( "GPL" );