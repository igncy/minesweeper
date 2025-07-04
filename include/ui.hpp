#ifndef MINESWEEPER_CLI_HPP
#define MINESWEEPER_CLI_HPP

#include <ncurses.h>

#include "board.hpp"

struct Cursor {
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

class UI {
private:
    virtual void drawBoard(const Board &board) const = 0;

public:
    virtual void init() = 0;

    virtual void close() = 0;

    virtual void draw(const Board &board) const = 0;

    virtual void drawEndScreen(bool game_lost) const = 0;

    virtual int getKey() = 0;

    virtual ~UI() = default;
};

class TUI final: public UI {
private:
    int game_win_h, game_win_v;
    WINDOW *win, *game_win;

    void drawBoard(const Board &board) const override;

    void drawMineCount(const Board &board) const;

public:
    TUI(int game_win_h, int game_win_v);

    [[nodiscard]] WINDOW *get_win() const {
        return win;
    }
    [[nodiscard]] WINDOW *get_game_win() const {
        return game_win;
    }

    void init() override;

    void close() override;

    void on_resize() const;

    void draw(const Board &board) const override;

    void drawEndScreen(bool game_lost) const override;

    void refresh() const;

    int getKey() override;

    ~TUI() override;
};

class CLI final: public UI {
private:
    void drawBoard(const Board &board) const override;

public:
    void init() override;

    void close() override;

    void on_resize() const;

    void draw(const Board &board) const override;

    void drawEndScreen(bool game_lost) const override;

    void refresh() const;

    int getKey() override;
};

#endif //MINESWEEPER_CLI_HPP
