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

    [[nodiscard]] long getElapsedTime() const;
};

void draw_timer(const Timer &timer, std::atomic<bool> &running, std::mutex &mutex, WINDOW *win, int game_win_h, int game_win_v);

#endif //MINESWEEPER_TIMER_H
