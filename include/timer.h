#ifndef MINESWEEPER_TIMER_H
#define MINESWEEPER_TIMER_H

#include <chrono>
#include <ncurses.h>
#include <thread>
#include <mutex>
#include <atomic>

#include "cli.hpp"

class Timer {
private:
    std::chrono::time_point<std::chrono::steady_clock> start_time;
    bool running;

public:
    Timer(): running(false) {}

    void start() {
        start_time = std::chrono::steady_clock::now();
        running = true;
    }

    void stop() {
        running = false;
    }

    [[nodiscard]] long getElapsedTime() const {
        if (!running) return 0;
        auto now = std::chrono::steady_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::seconds>(now-start_time);
        return duration.count();
    }
};

void draw_timer(const Timer &timer, std::atomic<bool> &running, std::mutex &mutex, WINDOW *win, int game_win_h, int game_win_v) {
    while (running.load()) {
        mutex.lock();

        wmove(win, LINES/2-game_win_h/2-1, COLS/2+game_win_v/2-4);
        wclrtoeol(win);
        wattron(win, COLOR_PAIR(C_YELLOW));
        long elapsed = timer.getElapsedTime();
        mvwprintw(win, LINES/2-game_win_h/2-1, COLS/2+game_win_v/2-4, "%02ld:%02ld",
                  elapsed/60, elapsed%60);
        wattroff(win, COLOR_PAIR(C_YELLOW));

        wrefresh(win);
        mutex.unlock();
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }
}

#endif //MINESWEEPER_TIMER_H
