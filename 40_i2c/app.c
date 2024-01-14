#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <linux/input.h>
#include <linux/i2c.h>
#include <linux/i2c-dev.h>
#include <sys/ioctl.h>
#include <string.h>

int fd;

int i2c_read_data( __u8 slave_addr, __u8 reg_addr, int len ) {
    struct i2c_rdwr_ioctl_data i2c_read_lcd;
    unsigned char data;
    int ret;

    __u8 buff[256];
    struct i2c_msg msg = {
        .addr = slave_addr,
        .flags = 0, // 0是寫，1是讀
        .buf = buff, // 根據flags的值來決定是要發送buf內的數據還是要接收數據到buf內
        .len = len + 1
    };

    // 設定結構體成員
    // 因為array類似於指標，直接指過去就好
    i2c_read_lcd.msgs = &msg;
    // 只有1個元素
    i2c_read_lcd.nmsgs = 1;


    buff[0] = slave_addr;
    memcpy( &buff[1], &reg_addr, len );

    // 使用ioctl傳送/接收數據
    ret = ioctl( fd, I2C_RDWR, &i2c_read_lcd );
    if ( ret < 0 ) {
        perror( "ioctl error\n" );
        return ret;
    } // if

    return data;
} // int


int main( int argc, char *argv[] ) {
    int data;
    fd = open( "/dev/i2c-0", O_RDWR );
    if ( fd < 0 ) {
        perror( "open error\n" );
        return fd;
    } // if

    while ( 1 ) {
        data = i2c_read_data( 0x50, 0x87, 1 );
        printf( "data is %d\n", data );
    } // while






    close( fd );
    return 0;
} // int
