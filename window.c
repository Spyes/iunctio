#include "window.h"

WINDOW * create_win( int height, int width, int starty, int startx );
void init_win();
void destroy_win( WINDOW* local_win );
void shutdown_win();

//WINDOW* chatw;
//WINDOW* inputw; 

void init_win()
{
    int sx, sy, w, h;
    
    initscr();
    keypad( stdscr, TRUE );

    // chat window
    sx = 0;
    sy = 0;
    h  = (LINES - 6);
    w  = COLS;
    refresh();
    chatw = create_win( h, w, sx, sy );
    wmove( chatw, 1, 1 );
    wrefresh( chatw );
    
    // input window
    sx = 0;
    sy = (LINES - 6);
    h = 6;
    w = COLS;
    inputw = create_win( h, w, sy, sx );
    wmove( inputw, 1, 1 );
    wrefresh( inputw );
}

WINDOW* create_win( int height, int width, int starty, int startx )
{
    WINDOW* local_win;
    local_win = newwin( height, width, starty, startx );
    box( local_win, 0, 0 );
    wrefresh( local_win );
    return local_win;
}

void destroy_win( WINDOW* local_win )
{
    wborder( local_win, ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ' );
    wrefresh( local_win );
    delwin( local_win );
}

void shutdown_win()
{
    destroy_win( chatw );
    destroy_win( inputw );
    endwin();
}

void clean_input()
{
    int xx;
    for( xx = 1; xx < COLS-1; xx++ )
        mvwaddch( inputw, 1, xx, ' ' );
    wmove( inputw, 1, 1 );
    wrefresh( inputw );
}

