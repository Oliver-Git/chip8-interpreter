#ifndef DISPLAY_H_
#define DISPLAY_H_

#include "chip8.h"

int set_up_sdl(struct Chip8 *chip8);
void quit_sdl();
void draw(struct Chip8 *chip8);

#endif // DISPLAY_H_
