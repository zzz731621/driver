#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

int main( int argc, char *argv[] ) {
    int fd;
    char buf[64] = {"12345"};

    // 對一個設備節點進行open操作
    // 會呼叫file_operations文件操作集合中對應的open函數
    fd = open( "/dev/hello_misc", O_RDWR );
    if ( fd < 0 ) {
        printf( "open device error\n" );
        return fd;
    } // if

    // 對一個設備節點進行read操作
    // 會呼叫file_operations文件操作集合中對應的read函數
    // 驅動中的read函數必須要使用copy_to_user函數才能完成真正的數據傳輸
    // read( fd, buf, sizeof( buf ) );
    // printf( "buf is %s\n", buf );


    // 對一個設備節點進行write操作
    // 會呼叫file_operations文件操作集合中對應的write函數
    // 驅動中的write函數必須要使用copy_from_user函數才能完成真正的數據傳輸
    write( fd, buf, sizeof( buf ) );


    // 對一個設備節點進行close操作
    // 會呼叫file_operations文件操作集合中對應的release函數
    close( fd );
    return 0;

} // int
