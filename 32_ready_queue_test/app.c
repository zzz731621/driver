#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

int main( int argc, char *argv[] ) {
    int fd;
    int value;

    fd = open( "/dev/hello_wq", O_RDWR );
    if ( fd < 0 ) {
        printf( "open device error\n" );
        return fd;
    } // if

    

    while ( 1 ) {
        read( fd, &value, sizeof( value ) );
        // printf( "value is %d\n", value );
    } // while

    return 0;

} // int
