#include <ncurses.h>

WINDOW* chatw;
WINDOW* inputw;

WINDOW* create_win( int height, int width, int starty, int startx );
void init_win();
void shutdown_win();
void clean_input();
