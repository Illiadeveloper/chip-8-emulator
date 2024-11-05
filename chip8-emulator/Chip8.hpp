#include <cstdint>
#include <fstream>
#include <random>
#include <iostream>
#include <chrono>

#pragma once

//#define DEBUG_CHIP

#define YELLOW "\033[33m"
#define GREEN "\033[32m"
#define RED "\033[31m"
#define RESET "\033[0m"

const unsigned int MEMORY_SIZE = 4096;
const unsigned int STACK_LEVEL = 16;
const unsigned int REGISTRE_LEVEL = 16;
const unsigned int VIDEO_HEIGHT = 32;
const unsigned int VIDEO_WEIGHT = 64;
const unsigned int KEY_COUNT = 16;

class Chip8 {
public:

	Chip8();

	void Cycle();

	void LoadROM(const char* filename);

	uint8_t keypad[KEY_COUNT]{};
	uint32_t video[VIDEO_HEIGHT * VIDEO_WEIGHT]{};
private:

	void Table0();
	void Table8();
	void TableE();
	void TableF();

	// Do nothing
	void OP_NULL();

	// CLS
	void OP_00E0();

	// RET
	void OP_00EE();

	// JP 1nnn adrr
	void OP_1nnn();
	
	// CALL 2nnn adrr
	void OP_2nnn();

	// SE 3xkk Vx, byte
	void OP_3xkk();

	// SNE 4xkk Vx, byte
	void OP_4xkk();

	// Se 5xy0 Vx, Vy
	void OP_5xy0();

	// LD 6xkk Vx, byte
	void OP_6xkk();

	// ADD 7xkk Vx, byte
	void OP_7xkk();

	// LD 8xy0 Vx, Vy
	void OP_8xy0();

	// OR 8xy1 Vx, Vy
	void OP_8xy1();

	// AND 8xy2 Vx, Vy
	void OP_8xy2();

	// XOR 8xy3 Vx, Vy
	void OP_8xy3();

	// ADD 8xy4 Vx, Vy
	void OP_8xy4();

	// SUB 8xy5 Vx, Vy
	void OP_8xy5();

	// SHR 8xy6 Vx {, Vy}
	void OP_8xy6();

	// SUBN Vx Vy
	void OP_8xy7();

	// SHL Vx {, Vy}
	void OP_8xyE();

	// SNE Vx, Vy
	void OP_9xy0();

	// LD I, addr
	void OP_Annn();

	// JP V0, addr
	void OP_Bnnn();

	// RND Vx, byte
	void OP_Cxkk();

	// DRW Vx, Vy, nibble
	void OP_Dxyn();

	// SKP Vx
	void OP_Ex9E();

	// SKNP Vx
	void OP_ExA1();

	// LD Vx, DT
	void OP_Fx07();

	// LD Vx, k
	void OP_Fx0A();
	
	// LD DT, Vx
	void OP_Fx15();

	// LD ST, Vx
	void OP_Fx18();

	// ADD I, Vx
	void OP_Fx1E();

	// LD F, Vx
	void OP_Fx29();

	// LD B, Vx
	void OP_Fx33();

	// LD [I], Vx
	void OP_Fx55();

	// LD Vx, [I]
	void OP_Fx65();



	std::default_random_engine randGen;
	std::uniform_int_distribution<int> randByte;


	uint8_t registers[REGISTRE_LEVEL]{};
	uint8_t memory[MEMORY_SIZE]{};
	uint16_t index{};
	uint16_t pc{};
	uint16_t stack[STACK_LEVEL]{};
	uint8_t sp{};
	uint8_t delayTimer{};
	uint8_t soundTimer{};
	uint16_t opcode{};

	typedef void (Chip8::* ChipFunc)();
	ChipFunc table[0xF + 1];
	ChipFunc table0[0xE + 1];
	ChipFunc table8[0xE + 1];
	ChipFunc tableE[0xE + 1];
	ChipFunc tableF[0x65 + 1];
};