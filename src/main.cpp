#include <ncurses.h>
#include <csignal>
#include <iostream>
#include <argparse/argparse.hpp>
#include <thread>
#include <mutex>
#include <atomic>

#include "../include/cli.hpp"
#include "../include/board.h"
#include "../include/timer.h"

std::atomic<bool> running(true);

void sigint_handler([[maybe_unused]] int sig) {
    running.store(false);
}

void at_exit() {
    close_cli();
}

int main(int argc, char *argv[]) {
    argparse::ArgumentParser program("minesweeper");
    program.set_usage_max_line_width(80);
    program.add_description("Play minesweeper");
    program.add_epilog("Controls:\n"
        "  Arrow keys / WASD   move the cursor\n"
        "  Space               reveal a cell\n"
        "  F                   flag a cell\n"
        "  Q                   quit");
    program.add_argument("-r", "--rows")
        .help("board height")
        .default_value(9)
        .metavar("ROWS")
        .nargs(1)
        .scan<'i', int>();
    program.add_argument("-c", "--cols")
        .help("set board width")
        .default_value(9)
        .metavar("COLS")
        .nargs(1)
        .scan<'i', int>();
    program.add_argument("-m", "--mines")
        .help("set mine count")
        .default_value(10)
        .metavar("MINES")
        .nargs(1)
        .scan<'i', int>();
    program.add_argument("-d", "--difficulty")
        .help("set difficulty level (1-3), overrides other options")
        .default_value(1)
        .choices(1, 2, 3)
        .metavar("D")
        .nargs(1)
        .scan<'i', int>();

    try {
        program.parse_args(argc, argv);
    }
    catch (const std::exception &err) {
        std::cerr << err.what() << std::endl << program;
        return 1;
    }

    int grid_rows = program.get<int>("--rows");
    int grid_cols = program.get<int>("--cols");
    int mines = program.get<int>("--mines");

    if (program.is_used("--difficulty")) {
        int preset = program.get<int>("--difficulty");
        if (preset == 1) {
            grid_rows = 9;
            grid_cols = 9;
            mines = 10;
        } else if (preset == 2) {
            grid_rows = 16;
            grid_cols = 16;
            mines = 40;
        } else if (preset == 3) {
            grid_rows = 16;
            grid_cols = 30;
            mines = 99;
        }
    }

    signal(SIGINT, sigint_handler);
    std::atexit(at_exit);
    init_cli();

    WINDOW *win = newwin(0, 0, 0, 0);
	keypad(win, TRUE);

    int game_win_h = grid_rows+2,
        game_win_v = grid_cols*3+2;
    if (game_win_h > LINES || game_win_v > COLS) {
        close_cli();
        std::cerr << "Error: board too big" << std::endl;
        return 1;
    }

    WINDOW *game_win = newwin(game_win_h, game_win_v, LINES/2-game_win_h/2, COLS/2-game_win_v/2);
	keypad(game_win, TRUE);
    wattron(game_win, COLOR_PAIR(C_BLUE));
    box(game_win, 0, 0);
    wattroff(game_win, COLOR_PAIR(C_BLUE));

    Board board(grid_rows, grid_cols);
    try {
        board.init(mines);
    }
    catch (const std::exception &err) {
        close_cli();
        std::cerr << err.what() << std::endl;
        return 1;
    }

    Cursor cursor = {0, 0};

    Timer timer;
    std::mutex mutex;
    std::thread timer_thread(draw_timer, std::ref(timer), std::ref(running), std::ref(mutex), win, game_win_h, game_win_v);
    timer.start();

    bool game_over = false;
	while (running.load()) {
        board.highlightCell(cursor.x, cursor.y);
        mutex.lock();
        board.draw(game_win);

        wattron(win, COLOR_PAIR(C_YELLOW));
        mvwprintw(win, LINES/2-game_win_h/2-1, COLS/2-game_win_v/2, "   ");
        mvwprintw(win, LINES/2-game_win_h/2-1, COLS/2-game_win_v/2, "%d", mines);
        wattroff(win, COLOR_PAIR(C_YELLOW));

        wrefresh(win);
        wrefresh(game_win);
        mutex.unlock();

        board.highlightCell(cursor.x, cursor.y);

        int ch = wgetch(win);
		switch (ch) {
            case KEY_UP: case 'w': case 'W':
                cursor.x > 0 ? cursor.x-- : cursor.x = board.rows()-1;
				break;
			case KEY_DOWN: case 's': case 'S':
                cursor.x < board.rows()-1 ? cursor.x++ : cursor.x = 0;
				break;
            case KEY_LEFT: case 'a': case 'A':
                cursor.y > 0 ? cursor.y-- : cursor.y = board.cols()-1;
                break;
            case KEY_RIGHT: case 'd': case 'D':
                cursor.y < board.cols()-1 ? cursor.y++ : cursor.y = 0;
                break;
            case 'f': case 'F':
                mines += board.flagCell(cursor.x, cursor.y);
                break;
            case ' ': {
                game_over = board.clickCell(cursor.x, cursor.y) == -1;
                break;
            }
            case 'q': case 'Q':
                running.store(false);
                break;
            case KEY_RESIZE:
                wclear(stdscr);
                wclear(win);
                wclear(game_win);
                mvwin(game_win, LINES/2-game_win_h/2, COLS/2-game_win_v/2);
                wattron(game_win, COLOR_PAIR(C_BLUE));
                box(game_win, 0, 0);
                wattroff(game_win, COLOR_PAIR(C_BLUE));
            default:
                break;
		}

        if (game_over || board.checkIfWon()) {
            timer.stop();
            running.store(false);
            board.reveal_all();
            board.draw(game_win);

            wmove(win, LINES/2+game_win_h/2+1, COLS/2-game_win_v/2);
            wclrtoeol(win);
            if (game_over) wattron(win, A_BOLD | COLOR_PAIR(C_RED));
            else wattron(win, A_BOLD | COLOR_PAIR(C_GREEN));
            mvwprintw(win, LINES/2+game_win_h/2+1, COLS/2-game_win_v/2, "you %s", game_over ? "lost" : "won");
            wattroff(win, A_BOLD | COLOR_PAIR(C_RED) | COLOR_PAIR(C_GREEN));

            wattron(game_win, COLOR_PAIR(game_over ? C_RED : C_GREEN));
            box(game_win, 0, 0);
            wattroff(game_win, COLOR_PAIR(game_over ? C_RED : C_GREEN));

            wrefresh(win);
            wrefresh(game_win);

            do {
                ch = wgetch(win);
            } while (ch != ' ' && ch != 'q' && ch != 'Q');
            break;
        }
	}

    timer_thread.join();
    close_cli();
	return 0;
}
