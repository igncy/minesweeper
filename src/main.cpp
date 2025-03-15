#include <csignal>
#include <iostream>
#include <argparse/argparse.hpp>
#include <thread>
#include <mutex>
#include <atomic>

#include "../include/ui.hpp"
#include "../include/board.hpp"
#include "../include/timer.hpp"

std::atomic<bool> running(true);

void sigint_handler([[maybe_unused]] int sig) {
    running.store(false);
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
        return EXIT_FAILURE;
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

    int game_win_h = grid_rows+2,
        game_win_v = grid_cols*3+2;
    auto ui = TUI(game_win_h, game_win_v);
    if (game_win_h > LINES || game_win_v > COLS) {
        std::cerr << "Error: board too big" << std::endl;
        return EXIT_FAILURE;
    }

    Board board(grid_rows, grid_cols, mines);
    try {
        board.init();
    }
    catch (const std::exception &err) {
        ui.close();
        std::cerr << err.what() << std::endl;
        return EXIT_FAILURE;
    }

    signal(SIGINT, sigint_handler);

    Cursor cursor = {0, 0};

    Timer timer;
    std::mutex mutex;
    std::thread timer_thread(draw_timer, std::ref(timer), std::ref(running), std::ref(mutex), ui.get_win(), game_win_h, game_win_v);
    timer.start();

    bool game_over = false;
	while (running.load()) {
        board.highlightCell(cursor.x, cursor.y);
        mutex.lock();
	    ui.drawBoard(board);
	    ui.drawMineCount(board);
	    ui.refresh();
        mutex.unlock();

        board.highlightCell(cursor.x, cursor.y);

        int ch = wgetch(ui.get_win());
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
		        ui.on_resize();
            default:
                break;
		}

        if (game_over || board.checkIfWon()) {
            timer.stop();
            running.store(false);
            board.reveal_all();
            ui.drawBoard(board);
            ui.drawEndScreen(game_over);
            ui.refresh();

            do {
                ch = wgetch(ui.get_win());
            } while (ch != ' ' && ch != 'q' && ch != 'Q');
            break;
        }
	}

    timer_thread.join();
    ui.close();
	return EXIT_SUCCESS;
}
