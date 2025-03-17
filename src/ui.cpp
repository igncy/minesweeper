#include <ncurses.h>
#include <iostream>

#include "../include/ui.hpp"

TUI::TUI(int game_win_h, int game_win_v): game_win_h(game_win_h), game_win_v(game_win_v) {
    initscr();
    win = newwin(0, 0, 0, 0);
    game_win = newwin(game_win_h, game_win_v, LINES/2-game_win_h/2, COLS/2-game_win_v/2);
}

void TUI::init() {
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

    if (game_win_h > LINES || game_win_v > COLS) {
        throw std::invalid_argument("Error: board too big");
    }

    keypad(win, TRUE);
    keypad(game_win, TRUE);

    wattron(game_win, COLOR_PAIR(C_BLUE));
    box(game_win, 0, 0);
    wattroff(game_win, COLOR_PAIR(C_BLUE));
}

void TUI::close() {
    endwin();
    curs_set(1);
}

void TUI::on_resize() const {
    wclear(stdscr);
    wclear(win);
    wclear(game_win);
    mvwin(game_win, LINES/2-game_win_h/2, COLS/2-game_win_v/2);
    wattron(game_win, COLOR_PAIR(C_BLUE));
    box(game_win, 0, 0);
    wattroff(game_win, COLOR_PAIR(C_BLUE));
}

void TUI::draw(const Board &board) const {
    drawBoard(board);
    drawMineCount(board);
}

void TUI::drawBoard(const Board &board) const {
    for (int row=0; row<board.rows(); row++) {
        for (int col=0; col<board.cols(); col++) {
            Cell cell = board.grid_[row][col];
            unsigned char ch;

            switch (cell.state) {
                case UNOPENED:
                    ch = ' ';
                    break;
                case OPENED:
                    if (cell.mine_state == -1) {
                        ch = 'X';
                        wattron(game_win, COLOR_PAIR(C_RED));
                    }
                    else ch = cell.mine_state + '0';
                    break;
                case FLAGGED:
                    ch = 'F';
                    wattron(game_win, COLOR_PAIR(C_GREEN));
                    break;
                default:
                    ch = '?';
            }

            #ifdef NO_COLORS
                if (cell.highlighted) wattron(game_win, A_REVERSE);
            #endif
            if (cell.highlighted) wattron(game_win, A_BOLD | COLOR_PAIR(C_HIGHLIGHT));
            mvwprintw(game_win, 1+row, 1+col*3, " %c ", ch);
            wattroff(game_win, A_REVERSE | A_BOLD | COLOR_PAIR(C_HIGHLIGHT) | COLOR_PAIR(C_RED) | COLOR_PAIR(C_GREEN));
        }
    }
}

void TUI::drawMineCount(const Board &board) const {
    wattron(win, COLOR_PAIR(C_YELLOW));
    mvwprintw(win, LINES/2-game_win_h/2-1, COLS/2-game_win_v/2, "   ");
    mvwprintw(win, LINES/2-game_win_h/2-1, COLS/2-game_win_v/2, "%d", board.mines());
    wattroff(win, COLOR_PAIR(C_YELLOW));
}

void TUI::drawEndScreen(bool game_lost) const {
    wmove(win, LINES/2+game_win_h/2+1, COLS/2-game_win_v/2);
    wclrtoeol(win);
    if (game_lost) wattron(win, A_BOLD | COLOR_PAIR(C_RED));
    else wattron(win, A_BOLD | COLOR_PAIR(C_GREEN));
    mvwprintw(win, LINES/2+game_win_h/2+1, COLS/2-game_win_v/2, "you %s", game_lost ? "lost" : "won");
    wattroff(win, A_BOLD | COLOR_PAIR(C_RED) | COLOR_PAIR(C_GREEN));

    wattron(game_win, COLOR_PAIR(game_lost ? C_RED : C_GREEN));
    box(game_win, 0, 0);
    wattroff(game_win, COLOR_PAIR(game_lost ? C_RED : C_GREEN));
}

void TUI::refresh() const {
    wrefresh(win);
    wrefresh(game_win);
}

int TUI::getKey() {
    return wgetch(win);
}

TUI::~TUI() {
    delwin(win);
    delwin(game_win);
}

void CLI::init() {
}

void CLI::close() {
}

void CLI::on_resize() const {
}

void CLI::draw(const Board &board) const {
    drawBoard(board);
}

void CLI::drawBoard(const Board &board) const {
    std::cout << "+";
    for (int col=0; col<board.cols(); col++)
        std::cout << "--";
    std::cout << "-+\n";

    for (auto &row: board.grid_) {
        std::cout << "|";
        for (auto &cell: row) {
            unsigned char ch;

            switch (cell.state) {
                case UNOPENED:
                    ch = ' ';
                    break;
                case OPENED:
                    if (cell.mine_state == -1) {
                        ch = 'X';
                        #ifndef NO_COLORS
                            std::cout << "\033[91m";
                        #endif
                    }
                    else ch = cell.mine_state + '0';
                    break;
                case FLAGGED:
                    ch = 'F';
                    #ifndef NO_COLORS
                        std::cout << "\033[92m";
                    #endif
                    break;
                default:
                    ch = '?';
            }

            if (cell.highlighted) std::cout << "\033[7m";
            std::cout << " " << ch << "\033[0m";
        }
        std::cout << " |\n";
    }

    std::cout << "+";
    for (int col=0; col<board.cols(); col++)
        std::cout << "--";
    std::cout << "-+" << std::endl;
}

void CLI::drawEndScreen(bool game_lost) const {
    std::cout << (game_lost ? "\033[91m" : "\033[92m")
        << "you " << (game_lost ? "lost" : "won")
        << "\033[0m" << std::endl;
}

void CLI::refresh() const {
}

int CLI::getKey() {
    return getchar();
}
