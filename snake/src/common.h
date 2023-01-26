#ifndef COMMON_H
#define COMMON_H

#include <stddef.h>

// Let's see if we can keep this as simple as possible, lest we intimidate
// students looking through the provided code.

// Bitflags enable us to store cell data in integers!
#define FLAG_PLAIN_CELL 0b0001  // equals 1
#define FLAG_SNAKE 0b0010       // equals 2
#define FLAG_WALL 0b0100        // equals 4
#define FLAG_FOOD 0b1000        // equals 8

/**
 * Enumerated types, also known as "enums", are a way to create a set of named
 * constants! This enum represents the different possible inputs in our snake
 * game. The type corresponding to this enum is `enum input_key` and variables
 * of this type can take on the following values:
 * INPUT_UP, INPUT_DOWN, INPUT_LEFT, INPUT_RIGHT, and INPUT_NONE.
 */
enum input_key { INPUT_UP, INPUT_DOWN, INPUT_LEFT, INPUT_RIGHT, INPUT_NONE };

// TODO: declare global variables needed for your snake (as `extern`)! (part 1A)

/** Global variables for game status.
 *
 * `g_` prefix used by convention to emphasize that these are global.
 *
 * You may need to add variables here in part 2 of the project!
 *
 * Variables:
 *  - g_game_over: 1 if game is over, 0 otherwise
 *  - g_score: current game score. Starts at 0. 1 point for every food eaten.
 */
extern int g_game_over;  // 1 if game is over, 0 otherwise
extern int g_score;      // game score: 1 point for every food eaten

/** Snake struct. This struct is not needed until part 2!
 * Fields:
 *  - None yet!
 */
typedef struct snake {
    // TODO: Define your snake struct! (in 2A)
    // Store any necessary information about your snake here.
} snake_t;

void set_seed(unsigned seed);
unsigned generate_index(unsigned size);

#endif
