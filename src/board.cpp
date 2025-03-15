#include <iostream>
#include <algorithm>

#include "../include/board.h"
#include "../include/rng.h"
#include "../include/cli.hpp"

Board::Board(int rows, int cols):
        rows_(rows),
        cols_(cols),
        grid_(rows, std::vector<Cell>(cols)) {
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
            Cell *cell = &grid_[r][c];

            if (0 <= r && r < rows_
                    && 0 <= c && c < cols_
                    && cell->state == UNOPENED
                    && cell->mine_state != -1) {
                cell->state = OPENED;
                if (cell->mine_state == 0) {
                    openAdjacentCells(r, c);
                }
            }
        }
    }
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

void Board::init(int mines) {
    if (rows_*cols_ < mines) {
        throw std::invalid_argument("Error: board too small or too many mines");
    }

    auto rng = RNG_int();

    for (int i=0; i<mines; i++) {
        int row, col;
        do {
            row = rng.generate(0, rows_-1);
            col = rng.generate(0, cols_-1);
        } while (grid_[row][col].mine_state == -1);

        grid_[row][col].mine_state = -1;
        updateAdjacentCells(row, col);
    }
}

void Board::draw(WINDOW *win) const {
    for (int row=0; row<rows_; row++) {
        for (int col=0; col<cols_; col++) {
            Cell cell = grid_[row][col];
            int ch;

            switch (cell.state) {
                case UNOPENED:
                    ch = ' ';
                    break;
                case OPENED:
                    ch = cell.mine_state == -1 ? 'X' : cell.mine_state + '0';
                    if (cell.mine_state == -1) wattron(win, COLOR_PAIR(C_RED));
                    break;
                case FLAGGED:
                    ch = 'F';
                    wattron(win, COLOR_PAIR(C_GREEN));
                    break;
            }

            #ifdef NO_COLORS
                if (cell.highlighted) wattron(win, A_REVERSE);
            #endif
            if (cell.highlighted) wattron(win, A_BOLD | COLOR_PAIR(C_HIGHLIGHT));
            mvwprintw(win, 1+row, 1+col*3, " %c ", ch);
            wattroff(win, A_REVERSE | A_BOLD | COLOR_PAIR(C_HIGHLIGHT) | COLOR_PAIR(C_RED) | COLOR_PAIR(C_GREEN));
        }
    }
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
                return cell.mine_state != -1 && cell.state != OPENED;}))
            return false;
    }
    return true;
}

Board::~Board() {
    std::cout << "destructor" << std::endl;
}
