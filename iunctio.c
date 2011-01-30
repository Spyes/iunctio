#include "window.h"
#include "server.h"


int main( int argc, char** argv )
{
    char err[1024];
    init_win();
    int s = start_server(&err);
    if( s > 0 )
    {
        printf( "%s", err );
        shutdown_win();
        return 1;
    }   

    shutdown_win();
    return 0;
}
