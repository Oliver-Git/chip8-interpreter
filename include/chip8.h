#ifndef CHIP8_H_
#define CHIP8_H_

#include <stdint.h>
#include <stdbool.h>

// A structure that stores data that the interpreter will need to access.
struct Chip8 {
	uint8_t memory[4096]; // 4KB of RAM.
	uint8_t V[16]; // 16 general purpose 8-bit registers.
	uint16_t I; // 16-bit register, normally used to store memory addresses.
	uint16_t PC; // Program counter.
	uint8_t SP; // Stack pointer.

	/* 
	 * Used to store the address that the interpreter should return to
	 * when finished with a subroutine.
	 * Chip-8 allows for up to 16 levels of nested subroutines.
	 * -- Cowgod's Chip-8 Technical Reference v1.0
	 */
	uint16_t stack[16];

	uint16_t opcode; // Used to store the current opcode.
	uint8_t delay_timer; // Decreases by 1 at a rate of 60Hz.
	uint8_t sound_timer; // Decreases by 1 at a rate of 60Hz.
	unsigned char display[64 * 32]; // 1D array contatining pixel states.
	bool draw_flag; // If true, the display should be drawn.
};

int initialise_chip_8(struct Chip8 *chip8, char *rom_path);
void execute_opcode(struct Chip8 *chip8);
void update_timers(struct Chip8 *chip8);

#endif // CHIP8_H_
