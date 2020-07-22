#include <stdio.h>
#include "../include//chip8.h"

int initialise_chip_8(struct Chip8 *chip8, char *rom_path)
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
	 * memset is not used because calls to memset can be removed by 
	 * the compiler. memset_s is not used due to poor support for the
	 * 'safe' functions from the ISO C Committee's TR 24731-1.
	 */
	for (size_t i = 0; i < sizeof(chip8->memory); i++) {
		*(volatile uint8_t *)(chip8->memory + i) = 0;
	}

	for (size_t i = 0; i < sizeof(chip8->V); i++) {
		*(volatile uint8_t *)(chip8->V + i) = 0;
	}

	for (size_t i = 0; i < sizeof(chip8->display); i++) {
		*(volatile unsigned char *)(chip8->display + i) = 0;
	}

	for (size_t i = 0; i < sizeof(chip8->stack); i++) {
		*(volatile uint16_t *)(chip8->stack + i) = 0;
	}

	chip8->I = 0;
	chip8->PC = PROGRAM_START;
	chip8->SP = 0;
	chip8->opcode = 0;
	chip8->delay_timer = 0;
	chip8->sound_timer = 0;
	chip8->draw_flag = false;

	// Load font into memory.
	for (size_t i = 0; i < sizeof(font); i++) {
		chip8->memory[i] = font[i];
	}

	FILE *rom = fopen(rom_path, "rb");

	if (rom == NULL) {
		perror("Failed loading ROM");
		return -1;
	}

	// Load ROM into memory.
	fread(chip8->memory + PROGRAM_START, 1, sizeof(chip8->memory) - PROGRAM_START, rom);
	fclose(rom);

	// debug_print_values(chip8);
	return 0;
}

static void debug_print_values(struct Chip8 *chip8)
{
	for (int i = 0; i < sizeof(chip8->memory); i++) {
		printf("%x ", chip8->memory[i]);
	}

	printf("\n\n");

	for (int j = 0; j < sizeof(chip8->V); j++) {
		printf("%x ", chip8->V[j]);
	}

	printf("\n\n%x\n\n", chip8->I);
}

void execute_opcode(struct Chip8 *chip8)
{
	// Get the first byte of the opcode from memory, shift it 8 bits to
	// the left, concatenate with 2nd byte using bitwise OR.
	chip8->opcode = chip8->memory[chip8->PC] << 8 | chip8->memory[chip8->PC + 1];

	switch (chip8->opcode & 0xF000) {
	case 0x0000:
		switch (chip8->opcode & 0x000F) {
		case 0x0000: // 00E0: Clear the display.
			for (size_t i = 0; i < sizeof(chip8->display); i++) {
				*(volatile unsigned char *)(chip8->display + i) = 0;
			}

			chip8->draw_flag = true;
			chip8->PC += 2;
			break;

		case 0x000E: // 00EE: Return from a subroutine.
			break;

		default:
			printf("Unknown opcode: %x\n", chip8->opcode);
			break;
		}
		break;

	case 0x1000: // 1nnn: Jump to location nnn.
		chip8->PC = chip8->opcode & 0x0FFF;
		break;

	case 0x2000: // 2nnn: Call subroutine at nnn.
		chip8->SP++;
		chip8->stack[chip8->SP] = chip8->PC;
		chip8->PC = chip8->opcode & 0x0FFF;
		break;

	case 0x3000: // 3xkk: Skip next instruction if Vx = kk.
		if (chip8->V[chip8->opcode & 0x0F00] == (chip8->opcode & 0x00FF)) {
			chip8->PC += 2;
		}
		break;

	case 0x4000: // 4xkk: Skip next instruction if Vx != kk.
		if (chip8->V[chip8->opcode & 0x0F00] != (chip8->opcode & 0x00FF)) {
			chip8->PC += 2;
		}
		break;

	case 0x5000: // 5xy0: Skip next instruction if Vx = Vy.
		if (chip8->V[chip8->opcode & 0x0F00] == chip8->V[chip8->opcode & 0x00F0]) {
			chip8->PC += 2;
		}
		break;

	case 0x6000: // 6xkk: Set Vx = kk.
		chip8->V[chip8->opcode & 0x0F00] = chip8->opcode & 0x00FF;
		chip8->PC += 2;
		break;

	case 0x7000: // 7xkk: Set Vx = Vx + kk.
		chip8->V[chip8->opcode & 0x0F00] += chip8->opcode & 0x00FF;
		chip8->PC += 2;
		break;

	case 0x8000:
		switch (chip8->opcode & 0x000F) {
		case 0x0000: // 8xy0: Set Vx = Vy.
			chip8->V[chip8->opcode & 0x0F00] = chip8->V[chip8->opcode & 0x00F0];
			chip8->PC += 2;
			break;

		case 0x0001: // 8xy1: Set Vx = Vx OR Vy.
			chip8->V[chip8->opcode & 0x0F00] |= chip8->V[chip8->opcode & 0x00F0];
			chip8->PC += 2;
			break;

		case 0x0002: // 8xy2: Set Vx = Vx AND Vy.
			chip8->V[chip8->opcode & 0x0F00] &= chip8->V[chip8->opcode & 0x00F0];
			chip8->PC += 2;
			break;

		case 0x0003: // 8xy3: Set Vx = Vx XOR Vy.
			chip8->V[chip8->opcode & 0x0F00] ^= chip8->V[chip8->opcode & 0x00F0];
			chip8->PC += 2;
			break;

		case 0x0004: // 8xy4: Set Vx = Vx + Vy, set VF = carry.
			// Unfinished.
			chip8->PC += 2;
			break;

		default:
			printf("Unknown opcode: %x\n", chip8->opcode);
			break;
		}
		break;

	case 0xA000: // Annn: Set I = nnn.
		chip8->I = chip8->opcode & 0x0FFF;
		chip8->PC += 2;
		break;

	// Dxyn: Display n-byte sprite starting at memory location I
	// at (Vx, Vy), set VF = collision.
	case 0xD000: 
		// Unfinished.
		chip8->PC += 2;
		chip8->draw_flag = true;
		break;

	default:
		printf("Unknown opcode: %x\n", chip8->opcode);
		break;
	}
}

// Timers decrease at a rate of 60Hz.
// This is independent from the speed of opcode execution.
void update_timers(struct Chip8 *chip8)
{
	if (chip8->delay_timer > 0)
		--chip8->delay_timer;

	if (chip8->sound_timer > 0) {
		if (chip8->sound_timer == 1)
			printf("Sound buzzer\n");
		--chip8->sound_timer;
	}
}
