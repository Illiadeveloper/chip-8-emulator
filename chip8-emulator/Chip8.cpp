#include "Chip8.hpp"

const unsigned int START_ADDRESS = 0x200;
const unsigned int FONTSET_SIZE = 80;
const unsigned int FONTSET_START_ADDRESS = 0x50;

uint8_t fontset[FONTSET_SIZE] = {
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

Chip8::Chip8() : randGen(std::chrono::system_clock::now().time_since_epoch().count())
{
	pc = START_ADDRESS;
	for (size_t i = 0; i < FONTSET_SIZE; ++i) {
		memory[FONTSET_START_ADDRESS + i] = fontset[i];
	}

	randByte = std::uniform_int_distribution<int>(0, 255U);
}

void Chip8::LoadROM(const char* filename)
{
	std::ifstream file(filename, std::ios::binary | std::ios::ate);
	if (file.is_open()) {
		std::streampos size = file.tellg();
		char* buffer = new char[size];

		file.seekg(0, std::ios::beg);
		file.read(buffer, size);
		file.close();

		for (size_t i = 0; i < size; ++i) {
			memory[START_ADDRESS + i] = buffer[i];
		}

		delete[] buffer;
	}
}

#include <iostream>

// CLS
void Chip8::OP_OOEO()
{
	memset(video, 0, sizeof(video));

#ifdef DEBUG_CHIP
	std::cout << "[DEBUG] OP_OOEO: Screen cleared." << std::endl;
#endif
}

// RET
void Chip8::OP_00EE()
{
	--sp;
	pc = stack[sp];

#ifdef DEBUG_CHIP
	std::cout << "[DEBUG] OP_00EE: Returning from subroutine." << std::endl;
	std::cout << std::hex << "[DEBUG] Stack pointer (SP): " << sp << std::endl;
	std::cout << std::hex << "[DEBUG] Program counter (PC): " << std::hex << pc << std::endl;
#endif
}

// JP 1nnn addr
void Chip8::OP_1nnn()
{
	pc = opcode & 0x0FFFu;

#ifdef DEBUG_CHIP
	std::cout << "[DEBUG] OP_1nnn: Jumping to address " << std::hex << pc << std::endl;
#endif
}

// CALL 2nnn addr
void Chip8::OP_2nnn()
{
	uint16_t address = opcode & 0x0FFFu;

#ifdef DEBUG_CHIP
	std::cout << "[DEBUG] OP_2nnn: Calling subroutine at address " << std::hex << address << std::endl;
	std::cout << "[DEBUG] Program counter (PC) before CALL: " << std::hex << pc << std::endl;
	std::cout << "[DEBUG] Stack pointer (SP) before CALL: " << sp << std::endl;
#endif

	stack[sp] = pc;
	sp++;
	pc = address;

#ifdef DEBUG_CHIP
	std::cout << "[DEBUG] Stack pointer (SP) after CALL: " << sp << std::endl;
	std::cout << "[DEBUG] Program counter (PC) after CALL: " << std::hex << pc << std::endl;
#endif
}

// SE 3xkk Vx, byte
void Chip8::OP_3xkk()
{
	uint8_t Vx = (opcode & 0x0F00u) >> 8u;
	uint8_t kk = opcode & 0x00FFu;

	if (registers[Vx] == kk) {
		pc += 2;
	}

#ifdef DEBUG_CHIP
	std::cout << "[DEBUG] OP_3xkk: SE Vx" << std::hex << (int)Vx
		<< " == " << std::hex << (int)kk
		<< ", PC = " << std::hex << pc << std::endl;
#endif
}

// SNE 4xkk Vx, byte
void Chip8::OP_4xkk()
{
	uint8_t Vx = (opcode & 0x0F00u) >> 8u;
	uint8_t kk = opcode & 0x00FFu;

	if (registers[Vx] != kk) {
		pc += 2;
	}

#ifdef DEBUG_CHIP
	std::cout << "[DEBUG] OP_4xkk: SNE Vx" << std::hex << (int)Vx
		<< " == " << std::hex << (int)kk
		<< ", PC = " << std::hex << pc << std::endl;
#endif
}

// SE 5xy0 Vx, Vy
void Chip8::OP_5xy0()
{
	uint8_t Vx = (opcode & 0x0F00u) >> 8u;
	uint8_t Vy = (opcode & 0x00F0u) >> 4u;

	if (registers[Vx] == registers[Vy]) {
		pc += 2;
	}

#ifdef DEBUG_CHIP
	std::cout << "[DEBUG] OP_5xy0: SE Vx" << std::hex << (int)Vx
		<< " == " << std::hex << (int)Vy
		<< ", PC = " << std::hex << pc << std::endl;
#endif
}


// LD 6xkk Vx, byte
void Chip8::OP_6xkk()
{
	uint8_t Vx = (opcode & 0x0F00u) >> 8u;
	uint8_t kk = (opcode & 0x00FFu);

#ifdef DEBUG_CHIP
	std::cout << "[DEBUG] OP_6xkk: LD Vx" << std::hex << (int)Vx << " = " << std::hex << (int)kk << std::endl;
#endif 

	registers[Vx] = kk;
}

// ADD 7xkk Vx, byte
void Chip8::OP_7xkk()
{
	uint8_t Vx = (opcode & 0x0F00u) >> 8u;
	uint8_t kk = (opcode & 0x00FFu);

#ifdef DEBUG_CHIP
	std::cout << "[DEBUG] OP_7xkk: ADD Vx" << std::hex << (int)Vx << " += " << std::hex << (int)kk << std::endl;
#endif 

	registers[Vx] += kk;
}

