#include <algorithm>
#include <stdexcept>

#include "../include/board.hpp"
#include "../include/rng.hpp"

Board::Board(int rows, int cols, int mines):
        rows_(rows),
        cols_(cols),
        mines_(mines),
        grid_(rows, std::vector<Cell>(cols)) {
}

Board::Board(Board const &other):
        rows_(other.rows_),
        cols_(other.cols_),
        mines_(other.mines_) {
    grid_ = std::vector<std::vector<Cell>>(rows_, std::vector<Cell>(cols_));
}

void Board::updateAdjacentCells(int row, int col) {
    for (int i=-1; i<=1; i++) {
        for (int j=-1; j<=1; j++) {
            int adj_row = row + i,
                adj_col = col + j;
            if (0 <= adj_row && adj_row < rows_
                    && 0 <= adj_col && adj_col < cols_
                    && grid_[adj_row][adj_col].mine_state != -1) {
                grid_[adj_row][adj_col].mine_state++;
            }
        }
    }
}

void Board::openAdjacentCells(int row, int col) {
    for (int i=-1; i<=1; i++) {
        for (int j=-1; j<=1; j++) {
            int r = row + i,
                c = col + j;

            if (!(i==0 && j==0)
                    && 0 <= r && r < rows_
                    && 0 <= c && c < cols_) {
                Cell &cell = grid_[r][c];

                if (cell.state == UNOPENED && cell.mine_state != -1) {
                    cell.state = OPENED;
                    if (cell.mine_state == 0) openAdjacentCells(r, c);
                }
            }
        }
    }
}

void Board::validateParams() const {
    if (rows_*cols_ < mines_)
        throw std::invalid_argument("Error: board too small or too many mines");
}

int Board::flagCell(int row, int col) {
    if (grid_[row][col].state == OPENED) {
        return 0;
    }
    if (grid_[row][col].state == FLAGGED) {
        grid_[row][col].state = UNOPENED;
        return 1;
    }
    // else UNOPENED
    grid_[row][col].state = FLAGGED;
    return -1;
}

void Board::highlightCell(int row, int col) {
    grid_[row][col].highlighted = !grid_[row][col].highlighted;
}

int Board::clickCell(int row, int col) {
    Cell *cell = &grid_[row][col];
    if (cell->state == UNOPENED) {
        cell->state = OPENED;
        if (cell->mine_state != -1)
            openAdjacentCells(row, col);
    }
    return cell->mine_state;
}

void Board::init() {
    validateParams();

    auto rng = RNG_int();

    for (int i=0; i<mines_; i++) {
        int row, col;
        do {
            row = rng.generate(0, rows_-1);
            col = rng.generate(0, cols_-1);
        } while (grid_[row][col].mine_state == -1);

        grid_[row][col].mine_state = -1;
        updateAdjacentCells(row, col);
    }
}

void Board::init(int mines) {
    mines_ = mines;
    init();
}

void Board::reveal_all() {
    for (auto &row: grid_) {
        for (auto &cell: row) {
            cell.state = OPENED;
            cell.highlighted = false;
        }
    }
}

bool Board::checkIfWon() const {
    for (auto &row_iter: grid_) {
        if (std::any_of(row_iter.begin(), row_iter.end(), [](Cell cell) {
                return cell.mine_state != -1 && cell.state != OPENED;
            }))
            return false;
    }
    return true;
}

bool Board::operator==(Board const &other) const {
    return rows_ == other.rows_
        && cols_ == other.cols_
        && mines_ == other.mines_;
}

bool Board::operator!=(Board const &other) const {
    return !(*this == other);
}

void Board::updateMineCount(int n) {
    mines_ += n;
}

void Board::setMineCount(int n) {
    mines_ = n;
}
