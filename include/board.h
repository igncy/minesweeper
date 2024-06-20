#ifndef MINESWEEPER_BOARD_H
#define MINESWEEPER_BOARD_H

#include <vector>
#include <ncurses.h>

enum CellState {
    UNOPENED,
    OPENED,
    FLAGGED
};

struct Cell {
    CellState state = UNOPENED;
    // -1 - mine, 0-6 - number of adjacent mines
    int mine_state = 0;
    bool highlighted = false;
};

class Board {
private:
    int rows_, cols_;
    std::vector<std::vector<Cell>> grid_;

    void updateAdjacentCells(int row, int col);

    void openAdjacentCells(int row, int col);

public:
    Board(int rows, int cols);

    [[nodiscard]] int rows() const {
        return rows_;
    }
    [[nodiscard]] int cols() const {
        return cols_;
    }

    int flagCell(int row, int col);

    void highlightCell(int row, int col);

    int clickCell(int row, int col);

    void init(int mines);

    void draw(WINDOW *win) const;

    void reveal_all();

    [[nodiscard]] bool checkIfWon() const;
};

#endif //MINESWEEPER_BOARD_H
