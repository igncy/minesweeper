#ifndef MINESWEEPER_BOARD_H
#define MINESWEEPER_BOARD_H

#include <vector>

enum CellState {
    UNOPENED,
    OPENED,
    FLAGGED
};

struct Cell {
    CellState state = UNOPENED;
    // -1 - mine, 0-8 - number of adjacent mines
    int mine_state = 0;
    bool highlighted = false;
};

class Board {
private:
    int rows_, cols_, mines_;
    std::vector<std::vector<Cell>> grid_;

    void updateAdjacentCells(int row, int col);

    void openAdjacentCells(int row, int col);

    void validateParams() const;

public:
    Board(int rows, int cols, int mines);
    Board(Board const &other);

    [[nodiscard]] int rows() const {
        return rows_;
    }
    [[nodiscard]] int cols() const {
        return cols_;
    }
    [[nodiscard]] int mines() const {
        return mines_;
    }

    int flagCell(int row, int col);

    void highlightCell(int row, int col);

    int clickCell(int row, int col);

    void init();
    void init(int mines);

    void reveal_all();

    [[nodiscard]] bool checkIfWon() const;

    ~Board();

    bool operator==(Board const &other) const;

    bool operator!=(Board const &other) const;

    friend class TUI;
    friend class CLI;
};

#endif //MINESWEEPER_BOARD_H
