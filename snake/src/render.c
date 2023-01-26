#define _XOPEN_SOURCE_EXTENDED 1
#include "render.h"

#include <curses.h>
#include <errno.h>
#include <locale.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "game_setup.h"

#define COLOR_BASE 1
#define COLOR_SNAKE 2
#define COLOR_WALL 3
#define COLOR_FOOD 4
#define COLOR_TEXT 5

#define BOARD_OFFSET_X 0
#define BOARD_OFFSET_Y 1

#define ADD(Y, X, C) mvaddch(Y + BOARD_OFFSET_Y, X + BOARD_OFFSET_X, C)
#define ADDW(Y, X, C) mvadd_wch(Y + BOARD_OFFSET_Y, X + BOARD_OFFSET_X, C)
#define WRITEW(Y, X, ...) \
    mvprintw(Y + BOARD_OFFSET_Y, X + BOARD_OFFSET_X, __VA_ARGS__)

/** Helper function that checks the terminal size against the game board
 * dimensions. Arguments:
 *  - width: width of the board.
 *  - height: height of the board.
 */
void check_terminal_size(size_t width, size_t height) {
    /* DO NOT MODIFY THIS FUNCTION */
    // use ncurses to get terminal dimensions.
    int req_h = (int)height + 2;
    int req_w = (int)width;
    if (LINES < req_h || COLS < req_w) {
        endwin();
        printf(
            "Terminal window must be at least %d by %d characters in size! "
            "Yours is %d by %d.\n",
            req_w, req_h, COLS, LINES);
        exit(1);
    }
    /* DO NOT MODIFY THIS FUNCTION */
}

/** Helper function that initializes the ncurses window and checks the terminal
 * size. Arguments:
 *  - width: width of the board.
 *  - height: height of the board.
 */
void initialize_window(size_t width, size_t height) {
    /* DO NOT MODIFY THIS FUNCTION */
    // Ncurses setup
    setlocale(LC_ALL, "");

    initscr();

    // wait for up to 1/10th of a second for input before returning
    // (ncurses "halfdelay" mode).
    halfdelay(1);

    // set keypad option to true (so getch returns a value representing a
    // pressed function key, instead of an escape sequence representing a
    // function key)
    keypad(stdscr, true);

    check_terminal_size(width, height);

    // Hide cursor
    curs_set(0);

    newwin((int)height, (int)width, 0, 0);
    refresh();

    start_color();
    use_default_colors();
    init_pair(1, COLOR_BLACK, -1);
    init_pair(2, COLOR_YELLOW, -1);
    init_pair(3, COLOR_BLUE, -1);
    init_pair(4, COLOR_RED, -1);
    init_pair(5, COLOR_WHITE, -1);
    /* DO NOT MODIFY THIS FUNCTION */
}

/** Renders the current game's board.
 * Arguments:
 *  - cells: a pointer to the first integer in an array of integers representing
 *    each board cell.
 *  - width: width of the board.
 *  - height: height of the board.
 */
void render_game(int* cells, size_t width, size_t height) {
    /* DO NOT MODIFY THIS FUNCTION */
    for (unsigned i = 0; i < width * height; ++i) {
        if (cells[i] & FLAG_SNAKE) {
            char c = 'S';
            ADD(i / width, i % width, c | COLOR_PAIR(COLOR_SNAKE));
        } else if (cells[i] & FLAG_FOOD) {
            char c = 'O';
            ADD(i / width, i % width, c | COLOR_PAIR(COLOR_FOOD));
        } else if (cells[i] & FLAG_WALL) {
            cchar_t c;
            setcchar(&c, L"\u2588", WA_NORMAL, COLOR_WALL, NULL);
            ADDW(i / width, i % width, &c);
        } else {
            char c = ' ';
            ADD(i / width, i % width, c);
        }
    }

    // Write score
    WRITEW(-1, 0, "SCORE: %d", g_score);
    // right-aligning is very doable, but a tad bit less approachable

    refresh();
    /* DO NOT MODIFY THIS FUNCTION */
}
