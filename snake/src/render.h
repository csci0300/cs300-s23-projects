#ifndef RENDER_H
#define RENDER_H

#include "game.h"

void check_terminal_size(size_t width, size_t height);
void initialize_window(size_t width, size_t height);
void end_game(int* cells, size_t width, size_t height, snake_t* snake_p);
void render_game(int* cells, size_t width, size_t height);

#endif
