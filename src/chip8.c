#include <stdio.h>
#include "../include//chip8.h"

int initialise_chip_8(struct Chip8 *chip, char *rom_path)
{
	// Start of most Chip-8 programs.
	const int PROGRAM_START = 0x200;

	// Built-in font.
	uint8_t font[80] = {
		0xF0, 0x90, 0x90, 0x90, 0xF0, // 0
		0x20, 0x60, 0x20, 0x20, 0x70, // 1
		0xF0, 0x10, 0xF0, 0x80, 0xF0, // 2
		0xF0, 0x10, 0xF0, 0x10, 0xF0, // 3
		0x90, 0x90, 0xF0, 0x10, 0x10, // 4
		0xF0, 0x80, 0xF0, 0x10, 0xF0, // 5
		0xF0, 0x80, 0xF0, 0x90, 0xF0, // 6
		0xF0, 0x10, 0x20, 0x40, 0x40, // 7
		0xF0, 0x90, 0xF0, 0x90, 0xF0, // 8
		0xF0, 0x90, 0xF0, 0x10, 0xF0, // 9
		0xF0, 0x90, 0xF0, 0x90, 0x90, // A
		0xE0, 0x90, 0xE0, 0x90, 0xE0, // B
		0xF0, 0x80, 0x80, 0x80, 0xF0, // C
		0xE0, 0x90, 0x90, 0x90, 0xE0, // D
		0xF0, 0x80, 0xF0, 0x80, 0xF0, // E
		0xF0, 0x80, 0xF0, 0x80, 0x80  // F
	};
	
	/*
	 * Clear memory, V registers, display, and stack.
	 * 
	 * memset is not used because calls to memset can be removed
	 * by the compiler. memset_s is not used because it was causing
	 * linker errors and I was unable to find a solution.
	 * 
	 * Yes, I was defining __STDC_WANT_LIB_EXT1__ to be 1 before including
	 * string.h
	 */
	for (size_t i = 0; i < sizeof(chip->memory); i++) {
		*(volatile uint8_t *)(chip->memory + i) = 0;
	}

	for (size_t i = 0; i < sizeof(chip->V); i++) {
		*(volatile uint8_t *)(chip->V + i) = 0;
	}

	for (size_t i = 0; i < sizeof(chip->display); i++) {
		*(volatile unsigned char *)(chip->display + i) = 0;
	}

	for (size_t i = 0; i < sizeof(chip->stack); i++) {
		*(volatile uint16_t *)(chip->stack + i) = 0;
	}

	chip->I = 0;
	chip->PC = PROGRAM_START;
	chip->SP = 0;
	chip->opcode = 0;
	chip->delay_timer = 0;
	chip->sound_timer = 0;
	chip->draw_flag = false;

	// Load font into memory.
	for (int i = 0; i < sizeof(font); i++) {
		chip->memory[i] = font[i];
	}

	FILE *rom = fopen(rom_path, "rb");

	if (rom == NULL) {
		perror("Failed loading ROM");
		return -1;
	}

	// Load ROM into memory.
	fread(chip->memory + PROGRAM_START, 1, sizeof(chip->memory) - PROGRAM_START, rom);
	fclose(rom);

	// debug_print_values(chip);
	return 0;
}

static void debug_print_values(struct Chip8 *chip)
{
	for (int i = 0; i < sizeof(chip->memory); i++) {
		printf("%x ", chip->memory[i]);
	}

	printf("\n\n");

	for (int j = 0; j < sizeof(chip->V); j++) {
		printf("%x ", chip->V[j]);
	}

	printf("\n\n%x\n\n", chip->I);
}

void execute_opcode(struct Chip8 *chip)
{
	// Get the first byte of the opcode from memory, shift it 8 bits to
	// the left, concatenate with 2nd byte using bitwise OR.
	chip->opcode = chip->memory[chip->PC] << 8 | chip->memory[chip->PC + 1];

	switch (chip->opcode & 0xF000) {
	case 0x0000:
		switch (chip->opcode & 0x000F) {
		case 0x0000: // 00E0: Clear the display.
			for (int i = 0; i < sizeof(chip->display); i++) {
				*(volatile unsigned char *)(chip->display + i) = 0;
			}

			chip->draw_flag = true;
			chip->PC += 2;
			break;

		case 0x000E: // 00EE: Return from a subroutine.
			break;
		}
		break;

	case 0x1000: // 1nnn: Jump to location nnn.
		chip->PC = chip->opcode & 0x0FFF;
		break;

	case 0x2000: // 2nnn: Call subroutine at nnn.
		chip->SP++;
		chip->stack[chip->SP] = chip->PC;
		chip->PC = chip->opcode & 0x0FFF;
		break;

	case 0x3000: // 3xkk: Skip next instruction if Vx = kk.
		if (chip->V[chip->opcode & 0x0F00] == (chip->opcode & 0x00FF)) {
			chip->PC += 2;
		}
		break;

	case 0x4000: // 4xkk: Skip next instruction if Vx != kk.
		if (chip->V[chip->opcode & 0x0F00] != (chip->opcode & 0x00FF)) {
			chip->PC += 2;
		}
		break;

	case 0x5000: // 5xy0: Skip next instruction if Vx = Vy.
		if (chip->V[chip->opcode & 0x0F00] == chip->V[chip->opcode & 0x00F0]) {
			chip->PC += 2;
		}
		break;

	case 0x6000: // 6xkk: Set Vx = kk.
		chip->V[chip->opcode & 0x0F00] = chip->opcode & 0x00FF;
		chip->PC += 2;
		break;

	case 0x7000: // 7xkk: Set Vx = Vx + kk.
		chip->V[chip->opcode & 0x0F00] += chip->opcode & 0x00FF;
		chip->PC += 2;
		break;

	case 0x8000:
		switch (chip->opcode & 0x000F) {
		case 0x0000: // 8xy0: Set Vx = Vy.
			chip->V[chip->opcode & 0x0F00] = chip->V[chip->opcode & 0x00F0];
			chip->PC += 2;
			break;

		case 0x0001: // 8xy1: Set Vx = Vx OR Vy.
			chip->V[chip->opcode & 0x0F00] |= chip->V[chip->opcode & 0x00F0];
			chip->PC += 2;
			break;

		case 0x0002: // 8xy2: Set Vx = Vx AND Vy.
			chip->V[chip->opcode & 0x0F00] &= chip->V[chip->opcode & 0x00F0];
			chip->PC += 2;
			break;

		case 0x0003: // 8xy3: Set Vx = Vx XOR Vy.
			chip->V[chip->opcode & 0x0F00] ^= chip->V[chip->opcode & 0x00F0];
			chip->PC += 2;
			break;

		case 0x0004: // 8xy4: Set Vx = Vx + Vy, set VF = carry.
			// Unfinished.
			chip->PC += 2;
			break;
		}
		break;

	case 0xA000: // Annn: Set I = nnn.
		chip->I = chip->opcode & 0x0FFF;
		chip->PC += 2;
		break;

	// Dxyn: Display n-byte sprite starting at memory location I
	// at (Vx, Vy), set VF = collision.
	case 0xD000: 
		// Unfinished.
		chip->PC += 2;
		chip->draw_flag = true;
		break;
	}
}

// Timers decrease at a rate of 60Hz.
// This is independent from the speed of opcode execution.
void update_timers(struct Chip8 *chip)
{
	if (chip->delay_timer > 0)
		--chip->delay_timer;

	if (chip->sound_timer > 0) {
		if (chip->sound_timer == 1)
			printf("Sound buzzer\n");
		--chip->sound_timer;
	}
}
