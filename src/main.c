#include <SDL2/SDL.h>
#include "../include//chip8.h"
#include "../include//display.h"

#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__)
	#define WINDOWS 1
	#include <windows.h>
	#include "../include//windows_sleep.h"
#elif __linux__
	#include <time.h>
	#include <sched.h>
#endif

int main(int argc, char *argv[])
{
	struct Chip8 chip;
	struct Chip8 *chip_ptr = &chip;

	// Initialise interpreter.
	if (initialise_chip_8(chip_ptr, "../roms/IBM Logo.ch8") == -1) {
		quit_sdl();
		return -1;
	}

	// Initialise SDL, create the window, get the window surface.
	if (set_up_sdl(chip_ptr) == -1) {
		quit_sdl();
		return -1;
	}

	const long delay_ns = 2000000;

	#ifdef WINDOWS
		HANDLE timer;
		LONGLONG delay_windows;

		// Create timer.
		if(!(timer = CreateWaitableTimer(NULL, TRUE, NULL))) {
			perror("Failed to create timer");
			return -1;
		}

		delay_windows = (LONGLONG) delay_ns / 100;
	#elif __linux__
		// Set to real time priority. Necessary for accurate timing.
		// Max priority is 99, min is 0.
		struct sched_param sp = {.sched_priority = 50};

		// int sched_setscheduler(pid_t pid, int policy,
		// const struct sched_param *param);

		// If pid equals zero, the scheduling policy and
		// parameters of the calling thread will be set. (man)
		sched_setscheduler(0, SCHED_FIFO, &sp);

		struct timespec delay_linux = {.tv_sec=1, .tv_nsec=0};
	#endif

	SDL_Event event;

	int quit = 0;

	while (!quit) {
		while (SDL_PollEvent(&event) != 0) {
			if (event.type == SDL_QUIT) {
				quit = 1;
			}
		}

		// The number 8 was obtained by dividing the clock speed
		// of the interpreter by the refresh rate, which also
		// happens to be the rate at which the timers decrement.
		// The calculation was: 500Hz / 60Hz = 8.33...
		for (int cycles = 0; cycles < 8; cycles++) {
			execute_opcode(chip_ptr);
			// printf("%x ", chip.opcode);
		}

		if (chip_ptr->draw_flag) {
			draw(chip_ptr);
			chip_ptr->draw_flag = false;
		}

		update_timers(chip_ptr);

		// Sleep for 1/500 seconds.
		#ifdef WINDOWS
			nanosleep_windows(delay_windows, timer);
		#elif __linux__
			nanosleep(&delay_linux, NULL);
		#endif
	}

	#ifdef WINDOWS
		CloseHandle(timer);
	#endif

	quit_sdl();
	return 0;
}
