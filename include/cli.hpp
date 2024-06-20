#ifndef MINESWEEPER_CLI_HPP
#define MINESWEEPER_CLI_HPP

void init_cli();

void close_cli();

struct Pos {
    int x, y;
};

enum ColorPairs {
    // dummy value bc it has to start from 1
    C_ZERO [[maybe_unused]],
    C_HIGHLIGHT,
    C_BLUE,
    C_RED,
    C_GREEN,
    C_YELLOW
};

#endif //MINESWEEPER_CLI_HPP
