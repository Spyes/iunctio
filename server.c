/* vim: set ts=4 sw=4 : */
/* emacs: better than vim */
// :P
#include <netinet/in.h>
#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>

#include "server.h"
#include "window.h"


// sockfd - socket descriptor returned from socket()
int sockfd, newsockfd, portno, clilen;
char send_data[1024], recv_data[1024];
char oname[1024], *ipa;    // other user's name
struct sockaddr_in serv_addr, cli_addr;
int line = 0;
int uname_len, oname_len;   // length of my name, and other user's name
bool quit = false;

void *thread( void *data )
{
    int n;
//    char *ipa;
//    ipa = (char *)inet_ntoa( cli_addr.sin_addr );       // address of the client
    
    while( 1 )
    {
        if( quit )
        {
            pthread_exit(NULL);
        }
        // recieve data
        n = recv( newsockfd, recv_data, 1024, 0 );
        recv_data[n] = '\0';    // add a new-line char at the end of the message
        
        if( strcmp( recv_data, "Q" ) == 0 )
        {
            close( newsockfd );
            break;
        }
        else
        {
            line++;
            wattron( chatw, COLOR_PAIR(2) );
            mvwprintw( chatw, line, 1, "%s:", oname );      // print the message
            wattroff( chatw, COLOR_PAIR(2) );
            mvwprintw( chatw, line, oname_len+3, "%s", recv_data );
            wmove( inputw, 1, 1 );              // move the cursor back to the input window
            wrefresh( chatw );
            wrefresh( inputw );
        }
    }
    //  shutdown_win();
    // close( sockfd );
    pthread_exit( NULL );
}

void start_thread()
{
    pthread_t hthread;
    void *args = NULL;
    int result = pthread_create( &hthread, NULL, &thread, args );
    // TAKE CARE OF THIS!!
/*    if( result )
    {
       sprintf( err, "ERORR on pthread_create(), code %d\n", result );
       return 1;
       }   */
}

int start_server( char *err )
{
    // our username
    char *uname = "server";

    // First call to socket() function
    sockfd = socket( AF_INET, SOCK_STREAM, 0 );
    if( sockfd < 0 )
    {
        sprintf( err, "ERROR opening socket" );
        return 1;
    }

    // Initialize socket structure
    bzero((char*) &serv_addr, sizeof( serv_addr ) );
    portno = SERVER_PORT;
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons( portno );

    // Now bind the host address using bind() call
    if( bind( sockfd, (struct sockaddr *) &serv_addr,
              sizeof( serv_addr ) ) < 0 )
    {
        sprintf( err, "ERROR on binding" );
        return 1;
    }

    // Now start listening for the clients, here process will
    // go into sleep mode and will wait for the incoming connection
    listen( sockfd, 5 );
    while( 1 )
    {
        clilen = sizeof( cli_addr );

        // Accept actual connection from the client
        newsockfd = accept( sockfd, (struct sockaddr *)&cli_addr,
                            &clilen );
        if( newsockfd < 0 )
        {
            sprintf( err, "ERROR on accept" );
            return 1;
        }
        ipa = (char *)inet_ntoa( cli_addr.sin_addr );

        // get the name of the user that connected, and send ours
        send( newsockfd, uname, strlen(uname), 0 );
        int n = recv( newsockfd, oname, 1024, 0 );

        // store length of my name and other user's name
        oname_len = strlen(oname);
        uname_len = strlen( uname );

        //init_win();
        start_thread();

        line++;
        mvwprintw( chatw, line, 1, "Connection from %s [%s]\n", oname, ipa );
        wrefresh(chatw);

        while( 1 )
        {
            mvwgetstr( inputw, 1, 1, send_data );
            wrefresh( inputw );

            if( strcmp( send_data, "Q" ) == 0 )
            {
                quit = true;
                send( newsockfd, send_data, 
                      strlen( send_data ), 0 );
                close( newsockfd );
                break;
            }

            else
            {
                line++;
                wattron( chatw, COLOR_PAIR(1) );
                mvwprintw( chatw, line, 1, "%s: ", uname );
                wattroff( chatw, COLOR_PAIR(1) );                       
                mvwprintw( chatw, line, uname_len+3, "%s", send_data );
                wrefresh( chatw );
                refresh();                
                clean_input();
                send( newsockfd, send_data, strlen( send_data ), 0 );
            }

        }
        //shutdown_win();
    }

    pthread_exit( NULL );
    close( sockfd );
    return 0;
}
