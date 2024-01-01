#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

int main( int argc, char *argv[] ) {
    int fd;
    char buf[64] = {0};

    // 對一個設備節點進行open操作
    // 會呼叫file_operations文件操作集合中對應的open函數
    // 測試自動生成的設備節點是否能用
    fd = open( "/dev/chrdev_test", O_RDWR );
    if ( fd < 0 ) {
        printf( "open device error\n" );
        return fd;
    } // if


    return 0;

} // int
