#include <thread>

#include "../include/timer.hpp"
#include "../include/cli.hpp"

long Timer::getElapsedTime() const {
        if (!running) return 0;
        auto now = std::chrono::steady_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::seconds>(now-start_time);
        return duration.count();
    }

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
