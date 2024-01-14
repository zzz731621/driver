#include <linux/init.h>
#include <linux/module.h>
#include <linux/i2c.h>


int ret;

static struct i2c_client * client;

static int i2c_read_reg( u8 reg_addr );
static void i2c_write_reg( u8 reg_addr, u8 data, u8 len );



static void i2c_write_reg( u8 reg_addr, u8 data, u8 len ) {
    u8 buff[256];

    struct i2c_msg msgs[] = {
        [0] = {
            .addr   = client->addr,
            .flags  = 0,
            .len    = len + 1,
            .buf    = buff
        }
    };

    // 第一個要放暫存器的地址
    buff[0] = reg_addr;
    // 所以意思是資料要從buff[1]開始放，所以上面的len要+1
    memcpy( &buff[1], &data, len );


    // 1. 用到的adapter
    // 2. 要讀或寫的struct i2c_msg結構體
    // 3. 上面結構體的元素數量
    i2c_transfer( client->adapter, msgs, 1 );
} // int

static int i2c_read_reg( u8 reg_addr ) {
    u8 data;
    struct i2c_msg msgs[] = {
        [0] = {
            .addr   = client->addr,
            .flags  = 0,
            .len    = sizeof( reg_addr ),
            .buf    = &reg_addr
        },
        [1] = {
            .addr   = client->addr,
            .flags  = 1,
            .len    = sizeof( data ),
            .buf    = &data
        }
    };

    // 1. 用到的adapter
    // 2. 要讀或寫的struct i2c_msg結構體
    // 3. 上面結構體的元素數量
    i2c_transfer( client->adapter, msgs, 2 );

    return data;
} // int



const struct i2c_device_id myi2c_id[] = {
    {.name = "i2c1_i2c_device_id", 0},
    {},
};
const struct of_device_id myi2c_match[] = {
    {.compatible = "i2c1_test_dev"},
    {},
};

int myi2c_probe( struct i2c_client * i2c_client, const struct i2c_device_id * id ) {
    printk( "This is i2c_probe function\n" );

    

    // 因為要在其他地方使用
    // 所以用一個全域變數來存
    client = i2c_client;

    
    // // 先往地址的0x50暫存器寫入值0x4b的數據
    i2c_write_reg( 0x50, 0x87, 1 );


    // 再去讀數據，看值是不是剛剛我們寫的
    ret = i2c_read_reg( 0x50 );


    printk( "value is %#x\n", ret );
    return 0;
} // int

int myi2c_remove( struct i2c_client * i2c_client ) {
    return 0;
} // int


// i2c_driver結構體
static struct i2c_driver myi2c_driver = {
    .driver = {
        .owner = THIS_MODULE,
        .name = "i2c1_test_dev_name",
        .of_match_table = myi2c_match
    },
    .probe = myi2c_probe,
    .remove = myi2c_remove,
    .id_table = myi2c_id,
};

static int mydriver_init( void ) {

    // 註冊i2c driver
    ret = i2c_add_driver( &myi2c_driver );
    if ( ret < 0 ) {
        printk( "i2c_add_driver error\n" );
        return ret;
    } // if
    printk( "i2c_add_driver success\n" );


    printk( "mydriver_init\n" );
    return 0;
} // int

static void mydriver_exit( void ) {
    // 取消註冊i2c driver
    i2c_del_driver( &myi2c_driver );
    printk( "i2c_del_driver success\n" );


    printk( "mydriver_exit\n" );
} // int


module_init( mydriver_init );
module_exit( mydriver_exit );
MODULE_LICENSE( "GPL" );