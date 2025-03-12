#include <ncurses.h>

#include "../include/cli.hpp"

void init_cli() {
    initscr();
	clear();
	noecho();
	cbreak();
    curs_set(0);
    keypad(stdscr, TRUE);

    #ifndef NO_COLORS
        start_color();
    #endif

    init_pair(C_HIGHLIGHT, COLOR_YELLOW, COLOR_BLUE);
    init_pair(C_BLUE, COLOR_BLUE, COLOR_BLACK);
    init_pair(C_RED, COLOR_RED, COLOR_BLACK);
    init_pair(C_GREEN, COLOR_GREEN, COLOR_BLACK);
    init_pair(C_YELLOW, COLOR_YELLOW, COLOR_BLACK);
}

void close_cli() {
    endwin();
    curs_set(1);
}
