#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>

// 定義命令，使用合成命令的macro
// 第一個參數為magic number，第二個是編號，一般從0開始，兩種命令的參數1和參數2不能都一樣，一定要有一個不同

// 測試沒有數據傳輸的命令
// #define CMD_TEST0 _IO('L',0)
// #define CMD_TEST1 _IO('L',1)

// 測試向kernel寫數據的命令
// #define CMD_TEST2 _IOW('L',2,int)
// #define CMD_TEST3 _IOW('L',3,int)

// 測試從kernel讀數據的命令
#define CMD_TEST4 _IOR('L',4,int)

// #define CMD_TEST1 _IO('A',1)
// #define CMD_TEST2 _IOW('L',2,int)
// #define CMD_TEST3 _IOR('L',3,int)

int main( int argc, char *argv[] ) {
    
    int fd;
    int value;
    fd = open( "/dev/hello_misc", O_RDWR );
    if ( fd < 0 ) {
        printf( "open device error\n" );
        return fd;
    } // if


    while ( 1 ) {
        ioctl( fd, CMD_TEST4, &value );
        printf( "value is %d\n", value );
        sleep( 2 );
    } // while


    

    return 0;
} // int
