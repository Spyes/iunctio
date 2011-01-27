#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include "window.h"

int sockfd, portno, n;
struct sockaddr_in serv_addr;
char recv_data[1024], send_data[1024];
int line = 0;
char oname[1024];
int uname_len, oname_len;   // length of my name, and other user's name

void *thread( void *data )
{
    // Keep the server's address as a char so we can print it to screen
    char *saddr = (char *)inet_ntoa( serv_addr.sin_addr );
    int n;
    while( 1 )
    {
        n = recv( sockfd, recv_data, 1024, 0 );
        recv_data[n] = '\0';
        
        if( strcmp( recv_data, "Q" ) == 0 )
        {
            close( sockfd );
            break;
        }
        
        else
        {
            line++;
            wattron( chatw, COLOR_PAIR(2) );
            mvwprintw( chatw, line, 1, "%s:", oname );
            wattroff( chatw, COLOR_PAIR(2) );
            mvwprintw( chatw, line, oname_len+3, "%s", recv_data );
            wmove( inputw, 1, 1 );
            wrefresh( chatw );
            wrefresh( inputw );
        }
    }
    shutdown_win();
    pthread_exit( NULL );
}


void start_thread()
{
    pthread_t hthread;
    void *args = NULL;
    int result = pthread_create( &hthread, NULL, &thread, args );
    if( result )
    {
        printf( "ERORR on pthread_create(), code %d\n", result );
        exit( 1 );
    }
}

int main( int argc, char** argv )
{
    init_win();

    struct hostent *server;
    char *uname;
    
    if( argc < 4 )
    {
        shutdown_win();
        fprintf( stderr, "usage: %s hostname port username\n", argv[0] );
        exit( 0 );
    }
    
    // pretty self-explanatory:
    // third arg - username
    uname = argv[3];

    // second arg - port number
    portno = atoi( argv[2] );

    // first arg - server to connect to
    server = gethostbyname( argv[1] );
    if( server == NULL )
    {
        fprintf( stderr, "ERROR, no such host %s\n", argv[1] );
        exit( 0 );
    }
    
    // Create a socket pointer
    sockfd = socket( AF_INET, SOCK_STREAM, 0 );
    if( sockfd < 0 )
    {
        perror( "ERROR opening socket\n" );
        exit( 1 );
    }
    
    //bzero( (char *) &serv_addr, sizeof( serv_addr ) );
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons( portno );
    serv_addr.sin_addr = *((struct in_addr *)server->h_addr);
    bzero( &(serv_addr.sin_zero), 8 );
    
    // Now connect to the server
    if( connect( sockfd, (struct sockaddr *)&serv_addr, 
                 sizeof( serv_addr ) ) < 0 )
    {
        perror( "ERROR connecting\n" );
        exit( 1 );
    }
    // before we do anything else, send the server our username
    send( sockfd, uname, strlen(uname), 0 );
    recv( sockfd, oname, 1024, 0 );

    uname_len = strlen(uname);
    oname_len = strlen(oname);

    start_thread();
    while(1)
    {
        // Ask for a message from the user, this message
        // will be read by server
        mvwgetstr( inputw, 1, 1, send_data );
        wrefresh( inputw );

        if( strcmp( send_data, "Q" ) != 0 )
        {
            line++;

            wattron( chatw, COLOR_PAIR(1) );
            mvwprintw( chatw, line, 1, "%s:", uname );
            wattroff( chatw, COLOR_PAIR(1) );
            mvwprintw( chatw, line, uname_len+3, send_data );
            wrefresh( chatw );
            clean_input();

            // send that data off to davey jones!
            send( sockfd, send_data, strlen( send_data ), 0 );  
        }
        
        else
        {
            send( sockfd, send_data, strlen( send_data ), 0 );
            close( sockfd );
            break;
        }
    }
    
    pthread_exit( NULL );
    //close( sockfd );
    shutdown_win();
    return 0;
}

