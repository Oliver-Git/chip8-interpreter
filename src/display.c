#include <stdio.h>
#include <SDL2/SDL.h>
#include "../include//display.h"
#include "../include//chip8.h"

// Display
const int DISPLAY_WIDTH = 64;
const int DISPLAY_HEIGHT = 32;
const int DISPLAY_SIZE_MULTIPLIER = 10;

const uint32_t WHITE = 0xFFFFFF;
const uint32_t BG_COLOR = 0x442342;

SDL_Window *window = NULL;
SDL_Surface *window_surface = NULL;

// Initialise SDL, create the window, get the window surface.
int set_up_sdl(struct Chip8  *chip)
{
	if (SDL_Init(SDL_INIT_VIDEO) < 0) {
		printf("SDL failed to initialise. SDL_Error: %s\n", SDL_GetError());
		return -1;
	}

	window = SDL_CreateWindow(
		"Test",
		SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
		DISPLAY_WIDTH * DISPLAY_SIZE_MULTIPLIER,
		DISPLAY_HEIGHT * DISPLAY_SIZE_MULTIPLIER, 0
	);

	if (window == NULL) {
		printf("Window is NULL. SDL_Error: %s\n", SDL_GetError());
		return -1;
	}

	window_surface = SDL_GetWindowSurface(window);

	if (window_surface == NULL) {
		printf("Window surface is NULL. SDL_Error: %s\n", SDL_GetError());
		return -1;
	}

	return 0;
}

void quit_sdl()
{
	SDL_DestroyWindow(window);

	window = NULL;
	window_surface = NULL;

	SDL_Quit();
}

void draw(struct Chip8 *chip)
{
	SDL_Rect pixel;
	pixel.w = DISPLAY_SIZE_MULTIPLIER;
	pixel.h = DISPLAY_SIZE_MULTIPLIER;

	SDL_FillRect(window_surface, NULL, BG_COLOR);

	for (int i = 0; i < DISPLAY_HEIGHT; i++) {
		for (int j = 0; j < DISPLAY_WIDTH; j++) {
			pixel.x = j * DISPLAY_SIZE_MULTIPLIER;
			pixel.y = i * DISPLAY_SIZE_MULTIPLIER;
			
			//  == 1 is present because for some reason there are
			// are always a few 8s in chip->display ... why??
			if (chip->display[j + i * DISPLAY_WIDTH] == 1) {
				SDL_FillRect(window_surface, &pixel, WHITE);
			}
		}
	}

	SDL_UpdateWindowSurface(window);
}
