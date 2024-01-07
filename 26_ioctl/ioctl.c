#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>

// 定義命令，使用合成命令的macro
// 第一個參數為magic number，第二個是編號，一般從0開始，兩種命令的參數1和參數2不能都一樣，一定要有一個不同
#define CMD_TEST0 _IO('L',0)
#define CMD_TEST1 _IO('A',1)
#define CMD_TEST2 _IOW('L',2,int)
#define CMD_TEST3 _IOR('L',3,int)

int main( int argc, char *argv[] ) {
    
    // 印出命令的方向，使用分解macro來提取
    printf( "30-31 is %d\n", _IOC_DIR(CMD_TEST0) );
    printf( "30-31 is %d\n", _IOC_DIR(CMD_TEST3) );


    // 印出命令的magic number
    printf( "8-15 is %c\n", _IOC_TYPE(CMD_TEST0) );
    printf( "8-15 is %c\n", _IOC_TYPE(CMD_TEST1) );


    // 印出命令的編號
    printf( "0-7 is %d\n", _IOC_NR(CMD_TEST0) );
    printf( "0-7 is %d\n", _IOC_NR(CMD_TEST1) );

    return 0;
} // int
