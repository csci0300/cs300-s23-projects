#define _XOPEN_SOURCE_EXTENDED 1
#include "game_over.h"

#include <curses.h>
#include <errno.h>
#include <locale.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "game_setup.h"

#define BOARD_OFFSET_X 0
#define BOARD_OFFSET_Y 2

#define ADD(Y, X, C) mvaddch(Y + BOARD_OFFSET_Y, X + BOARD_OFFSET_X, C)
#define ADDW(Y, X, C) mvadd_wch(Y + BOARD_OFFSET_Y, X + BOARD_OFFSET_X, C)
#define WRITEW(Y, X, ...) \
    mvprintw(Y + BOARD_OFFSET_Y, X + BOARD_OFFSET_X, __VA_ARGS__)

/** Renders the Game Over screen.
 * Arguments:
 *  - width: width of the board
 *  - height: height of the board
 */
void render_game_over(size_t width, size_t height) {
    /* DO NOT MODIFY THIS FUNCTION */

    int y_center = ((int)height / 2);
    int x_center = ((int)width / 2);

    WRITEW(y_center - 4, x_center - 4, "GAME OVER");
    WRITEW(y_center - 2, x_center - (g_name_len / 2), "%s", g_name);
    int number_of_digits_in_score =
        g_score ? (int)(ceil(log10((double)g_score))) : 1;
    // (note that log10(0) is undefined, so we have to catch it)
    WRITEW(y_center - 1, x_center - ((7 + number_of_digits_in_score) / 2),
           "SCORE: %d", g_score);

    WRITEW(y_center + 2, x_center - 10, "PRESS ANY KEY TO EXIT");

    refresh();
    /* DO NOT MODIFY THIS FUNCTION */
}
