#include "Chip8.hpp"
#pragma once

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
	for (unsigned int i = 0; i < FONTSET_SIZE; ++i) {
		memory[FONTSET_START_ADDRESS + i] = fontset[i];
	}

	table[0x0] = &Chip8::Table0;
	table[0x1] = &Chip8::OP_1nnn;
	table[0x2] = &Chip8::OP_2nnn;
	table[0x3] = &Chip8::OP_3xkk;
	table[0x4] = &Chip8::OP_4xkk;
	table[0x5] = &Chip8::OP_5xy0;
	table[0x6] = &Chip8::OP_6xkk;
	table[0x7] = &Chip8::OP_7xkk;
	table[0x8] = &Chip8::Table8;
	table[0x9] = &Chip8::OP_9xy0;
	table[0xA] = &Chip8::OP_Annn;
	table[0xB] = &Chip8::OP_Bnnn;
	table[0xC] = &Chip8::OP_Cxkk;
	table[0xD] = &Chip8::OP_Dxyn;
	table[0xE] = &Chip8::TableE;
	table[0xF] = &Chip8::TableF;

	for (size_t i = 0; i <= 0xE; i++) {
		table0[i] = &Chip8::OP_NULL;
		table8[i] = &Chip8::OP_NULL;
		tableE[i] = &Chip8::OP_NULL;
	}

	table0[0x0] = &Chip8::OP_00E0;
	table0[0xE] = &Chip8::OP_00EE;

	table8[0x0] = &Chip8::OP_8xy0;
	table8[0x1] = &Chip8::OP_8xy1;
	table8[0x2] = &Chip8::OP_8xy2;
	table8[0x3] = &Chip8::OP_8xy3;
	table8[0x4] = &Chip8::OP_8xy4;
	table8[0x5] = &Chip8::OP_8xy5;
	table8[0x6] = &Chip8::OP_8xy6;
	table8[0x7] = &Chip8::OP_8xy7;
	table8[0xE] = &Chip8::OP_8xyE;

	tableE[0x1] = &Chip8::OP_ExA1;
	tableE[0xE] = &Chip8::OP_Ex9E;

	for (size_t i = 0; i <= 0x65; ++i) 
	{
		tableF[i] = &Chip8::OP_NULL;
	}

	tableF[0x07] = &Chip8::OP_Fx07;
	tableF[0x0A] = &Chip8::OP_Fx0A;
	tableF[0x15] = &Chip8::OP_Fx15;
	tableF[0x18] = &Chip8::OP_Fx18;
	tableF[0x1E] = &Chip8::OP_Fx1E;
	tableF[0x29] = &Chip8::OP_Fx29;
	tableF[0x33] = &Chip8::OP_Fx33;
	tableF[0x55] = &Chip8::OP_Fx55;
	tableF[0x65] = &Chip8::OP_Fx65;

	randByte = std::uniform_int_distribution<int>(0, 255U);
}

void Chip8::Cycle()
{
	opcode = (memory[pc] << 8u) | memory[pc + 1];

#ifdef DEBUG_CHIP
	std::cout << YELLOW << "[DEBUG] Fetched opcode: 0x" << std::hex << opcode << RESET << std::endl;
	std::cout << YELLOW << "[DEBUG] Program counter (PC) before execution: 0x" << std::hex << pc << RESET << std::endl;
#endif

	pc += 2;

#ifdef DEBUG_CHIP
	std::cout << YELLOW << "[DEBUG] Program counter (PC) after increment: 0x" << std::hex << pc << RESET << std::endl;
	std::cout << YELLOW << "[DEBUG] Executing function from table with index: " << std::dec << ((opcode & 0xF000) >> 12u) << RESET << "\n\n";
#endif

	((*this).*(table[(opcode & 0xF000u) >> 12u]))();

#ifdef DEBUG_CHIP
	std::cout << GREEN << "\n[DEBUG] Opcode 0x" << std::hex << opcode << " executed successfully. \n" << RESET << std::endl;
#endif

	if (delayTimer > 0)
	{
		--delayTimer;

#ifdef DEBUG_CHIP
		std::cout << "[DEBUG] Delay timer decreased to: " << std::dec << static_cast<int>(delayTimer) << std::endl;
#endif
	}

	if (soundTimer > 0)
	{
		--soundTimer;

#ifdef DEBUG_CHIP
		std::cout << "[DEBUG] Sound timer decreased to: " << std::dec << static_cast<int>(soundTimer) << std::endl;
#endif
	}
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

void Chip8::Table0()
{
#ifdef DEBUG_CHIP
	std::cout << "[DEBUG] Opcode: 0x" << std::hex << opcode << std::endl;
	std::cout << "[DEBUG] Calling function at table0 index: " << std::dec << (opcode & 0x000F) << std::endl;
#endif

	((*this).*(table0[opcode & 0x000Fu]))();
}

void Chip8::Table8()
{
#ifdef DEBUG_CHIP
	std::cout << "[DEBUG] Opcode: 0x" << std::hex << opcode << std::endl;
	std::cout << "[DEBUG] Calling function at table8 index: " << std::dec << (opcode & 0x000F) << std::endl;
#endif

	((*this).*(table8[opcode & 0x000Fu]))();
}

void Chip8::TableE()
{
#ifdef DEBUG_CHIP
	std::cout << "[DEBUG] Opcode: 0x" << std::hex << opcode << std::endl;
	std::cout << "[DEBUG] Calling function at tableE index: " << std::dec << (opcode & 0x000F) << std::endl;
#endif

	((*this).*(tableE[opcode & 0x000Fu]))();
}

void Chip8::TableF()
{
#ifdef DEBUG_CHIP
	std::cout << "[DEBUG] Opcode: 0x" << std::hex << opcode << std::endl;
	std::cout << "[DEBUG] Calling function at tableF index: " << std::dec << (opcode & 0x000F) << std::endl;
#endif

	((*this).*(tableF[opcode & 0x00FFu]))();
}

void Chip8::OP_NULL() {}

// CLS
void Chip8::OP_00E0()
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
	uint16_t address = opcode & 0x0FFFu;
	pc = address;

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
	++sp;
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
	uint8_t kk = opcode & 0x00FFu;

#ifdef DEBUG_CHIP
	std::cout << "[DEBUG] OP_6xkk: LD Vx" << std::hex << (int)Vx << " = " << std::hex << (int)kk << std::endl;
#endif 

	registers[Vx] = kk;
}

// ADD 7xkk Vx, byte
void Chip8::OP_7xkk()
{
	uint8_t Vx = (opcode & 0x0F00u) >> 8u;
	uint8_t kk = opcode & 0x00FFu;

#ifdef DEBUG_CHIP
	std::cout << "[DEBUG] OP_7xkk: ADD Vx" << std::hex << (int)Vx << " += " << std::hex << (int)kk << std::endl;
#endif 

	registers[Vx] += kk;
}

// LD 8xy0 Vx, Vy
void Chip8::OP_8xy0()
{
	uint8_t Vx = (opcode & 0x0F00u) >> 8u;
	uint8_t Vy = (opcode & 0x00F0u) >> 4u;

#ifdef DEBUG_CHIP
	std::cout << "[DEBUG] OP_8xy0: LD Vx" << std::hex << (int)Vx << " = " << std::hex << (int)Vy << std::endl;
#endif 

	registers[Vx] = registers[Vy];
}

// OR 8xy1 Vx, Vy
void Chip8::OP_8xy1()
{
	uint8_t Vx = (opcode & 0x0F00u) >> 8u;
	uint8_t Vy = (opcode & 0x00F0u) >> 4u;

#ifdef DEBUG_CHIP
	uint8_t prevVx = registers[Vx];
	uint8_t prevVy = registers[Vy]; 
#endif

	registers[Vx] |= registers[Vy];

#ifdef DEBUG_CHIP
	std::cout << "[DEBUG] OP_8xy1:\n";
	std::cout << "    Before OR: Vx(" << std::hex << +Vx << ") = 0x" << std::hex << +prevVx
		<< ", Vy(" << std::hex << +Vy << ") = 0x" << std::hex << +prevVy << "\n";
	std::cout << "    After OR:  Vx(" << std::hex << +Vx << ") = 0x" << std::hex << +registers[Vx]
		<< " (Vx | Vy)\n";
#endif
}


// AND 8xy2 Vx, Vy
void Chip8::OP_8xy2()
{
	uint8_t Vx = (opcode & 0x0F00u) >> 8u;
	uint8_t Vy = (opcode & 0x00F0u) >> 4u;

#ifdef DEBUG_CHIP
	uint8_t prevVx = registers[Vx];
	uint8_t prevVy = registers[Vy];
#endif

	registers[Vx] &= registers[Vy];

#ifdef DEBUG_CHIP
	std::cout << "[DEBUG] OP_8xy2:\n";
	std::cout << "    Before AND: Vx(" << std::hex << +Vx << ") = 0x" << std::hex << +prevVx
		<< ", Vy(" << std::hex << +Vy << ") = 0x" << std::hex << +prevVy << "\n";
	std::cout << "    After AND:  Vx(" << std::hex << +Vx << ") = 0x" << std::hex << +registers[Vx]
		<< " (Vx | Vy)\n";
#endif

}

// XOR 8xy3 Vx, Vy
void Chip8::OP_8xy3()
{
	uint8_t Vx = (opcode & 0x0F00u) >> 8u;
	uint8_t Vy = (opcode & 0x00F0u) >> 4u;

#ifdef DEBUG_CHIP
	uint8_t prevVx = registers[Vx];
	uint8_t prevVy = registers[Vy];
#endif

	registers[Vx] ^= registers[Vy];

#ifdef DEBUG_CHIP
	std::cout << "[DEBUG] OP_8xy3:\n";
	std::cout << "    Before XOR: Vx(" << std::hex << +Vx << ") = 0x" << std::hex << +prevVx
		<< ", Vy(" << std::hex << +Vy << ") = 0x" << std::hex << +prevVy << "\n";
	std::cout << "    After XOR:  Vx(" << std::hex << +Vx << ") = 0x" << std::hex << +registers[Vx]
		<< " (Vx | Vy)\n";
#endif
}

// ADD 8xy4 Vx, Vy
void Chip8::OP_8xy4()
{
	uint8_t Vx = (opcode & 0x0F00u) >> 8u;
	uint8_t Vy = (opcode & 0x00F0u) >> 4u;

#ifdef DEBUG_CHIP
	uint8_t prevVx = registers[Vx];
	uint8_t prevVy = registers[Vy];
#endif

	uint16_t sum = registers[Vx] + registers[Vy];

	if (sum > 255U) {
		registers[0xF] = 1;
	}
	else {
		registers[0xF] = 0;
	}

	registers[Vx] = sum & 0xFFu;

#ifdef DEBUG_CHIP
	std::cout << "[DEBUG] OP_8xy4:\n";
	std::cout << "    Before ADD: Vx(" << std::hex << +Vx << ") = 0x" << std::hex << +prevVx
		<< ", Vy(" << std::hex << +Vy << ") = 0x" << std::hex << +prevVy << "\n";
	std::cout << "    After ADD:  Vx(" << std::hex << +Vx << ") = 0x" << std::hex << +registers[Vx]
		<< " (Vx | Vy)\n";
#endif
}

// SUB 8xy5 Vx, Vy
void Chip8::OP_8xy5()
{
	uint8_t Vx = (opcode & 0x0F00u) >> 8u;
	uint8_t Vy = (opcode & 0x00F0u) >> 4u;

#ifdef DEBUG_CHIP
	uint8_t prevVx = registers[Vx];
	uint8_t prevVy = registers[Vy];
#endif

	if (registers[Vx] > registers[Vy]) {
		registers[0xF] = 1;
	}
	else 
	{
		registers[0xF] = 0;
	}

	registers[Vx] -= registers[Vy];

#ifdef DEBUG_CHIP
	std::cout << "[DEBUG] OP_8xy5:\n";
	std::cout << "    Before SUB: Vx(" << std::hex << +Vx << ") = 0x" << std::hex << +prevVx
		<< ", Vy(" << std::hex << +Vy << ") = 0x" << std::hex << +prevVy << "\n";
	std::cout << "    After SUB:  Vx(" << std::hex << +Vx << ") = 0x" << std::hex << +registers[Vx]
		<< " (Vx | Vy)\n";
#endif
}

// SHR 8xy6 Vx {, Vy}
void Chip8::OP_8xy6()
{
	uint8_t Vx = (opcode & 0x0F00u) >> 8u;

#ifdef DEBUG_CHIP
	uint8_t prevVx = registers[Vx];
#endif

	registers[0xF] = (registers[Vx] & 0x1u);

	registers[Vx] >>= 1;

#ifdef DEBUG_CHIP
	std::cout << "[DEBUG] OP_8xy6:\n";
	std::cout << "    Before SHR: Vx(" << std::hex << +Vx << ") = 0x" << std::hex << +prevVx << std::endl;
	std::cout << "    After SHR:  Vx(" << std::hex << +Vx << ") = 0x" << std::hex << +registers[Vx] << "\n";
#endif
}

// SUBN Vx Vy
void Chip8::OP_8xy7()
{
	uint8_t Vx = (opcode & 0x0F00u) >> 8u;
	uint8_t Vy = (opcode & 0x00F0u) >> 4u;

#ifdef DEBUG_CHIP
	uint8_t prevVx = registers[Vx];
	uint8_t prevVy = registers[Vy];
#endif

	if (registers[Vy] > registers[Vx]) {
		registers[0xF] = 1;
	}
	else {
		registers[0xF] = 0;
	}

	registers[Vx] = registers[Vy] - registers[Vx];

#ifdef DEBUG_CHIP
	std::cout << "[DEBUG] OP_8xy7:\n";
	std::cout << "    Before SUBN: Vx(" << std::hex << +Vx << ") = 0x" << std::hex << +prevVx
		<< ", Vy(" << std::hex << +Vy << ") = 0x" << std::hex << +prevVy << "\n";
	std::cout << "    After SUBN:  Vx(" << std::hex << +Vx << ") = 0x" << std::hex << +registers[Vx]
		<< " (Vx | Vy)\n";
#endif

}

// SHL Vx {, Vy}
void Chip8::OP_8xyE()
{
	uint8_t Vx = (opcode & 0x0F00u) >> 8u;

#ifdef DEBUG_CHIP
	uint8_t prevVx = registers[Vx];
#endif

	registers[0xF] = (registers[Vx] & 0x80u) >> 7u;

	registers[Vx] <<= 1;

#ifdef DEBUG_CHIP
	std::cout << "[DEBUG] OP_8xy9:\n";
	std::cout << "    Before SHL: Vx(" << std::hex << +Vx << ") = 0x" << std::hex << +prevVx << std::endl;
	std::cout << "    After SHL:  Vx(" << std::hex << +Vx << ") = 0x" << std::hex << +registers[Vx] << "\n";
#endif
}

// SNE Vx, Vy
void Chip8::OP_9xy0()
{
	uint8_t Vx = (opcode & 0x0F00u) >> 8u;
	uint8_t Vy = (opcode & 0x00F0u) >> 4u;

#ifdef DEBUG_CHIP
	std::cout << "[DEBUG] OP_9xy0 (SNE):\n";
	std::cout << "    Vx(" << std::hex << +Vx << ") = 0x" << std::hex << +registers[Vx]
		<< ", Vy(" << std::hex << +Vy << ") = 0x" << std::hex << +registers[Vy] << "\n";
#endif

	if (registers[Vx] != registers[Vy]) {
		pc += 2;

#ifdef DEBUG_CHIP
		std::cout << "[DEBUG] Condition met (Vx != Vy). Program counter (PC) increased to: 0x"
			<< std::hex << pc << std::endl;
#endif
	}
}

// LD I, addr
void Chip8::OP_Annn()
{
	uint16_t address = opcode & 0x0FFFu;

#ifdef DEBUG_CHIP
	std::cout << "[DEBUG] OP_Annn (LD I, addr):\n";
	std::cout << "    Loading address: 0x" << std::hex << address << " into index register (I)\n";
#endif

	index = address;

#ifdef DEBUG_CHIP
	std::cout << "[DEBUG] Index register (I) set to: 0x" << std::hex << index << std::endl;
#endif
}

// JP V0, addr
void Chip8::OP_Bnnn()
{
	uint16_t address = opcode & 0x0FFFu;

#ifdef DEBUG_CHIP
	std::cout << "[DEBUG] OP_Bnnn (JP V0, addr):\n";
	std::cout << "    Jumping to address: 0x" << std::hex << address
		<< " + V0 (0x" << std::hex << +registers[0] << ")\n";
#endif

	pc = registers[0] + address;

#ifdef DEBUG_CHIP
	std::cout << "[DEBUG] Program counter (PC) set to: 0x" << std::hex << pc << std::endl;
#endif
}

// RND Vx, byte
void Chip8::OP_Cxkk()
{
	uint8_t Vx = (opcode & 0x0F00u) >> 8u;
	uint8_t byte = opcode & 0x00FFu;

#ifdef DEBUG_CHIP
	std::cout << "[DEBUG] OP_Cxkk (RND Vx, byte):\n";
	std::cout << "    Byte mask applied: 0x" << std::hex << +byte << "\n";
	std::cout << "    Storing result into Vx(" << std::hex << +Vx << ")\n";
#endif

	registers[Vx] = randByte(randGen) & byte;

#ifdef DEBUG_CHIP
	std::cout << "[DEBUG] Vx(" << std::hex << +Vx << ") set to: 0x" << std::hex << +registers[Vx] << std::endl;
#endif
}

// DRW Vx, Vy, nibble
void Chip8::OP_Dxyn()
{
	uint8_t Vx = (opcode & 0x0F00u) >> 8u;
	uint8_t Vy = (opcode & 0x00F0u) >> 4u;
	uint8_t height = opcode & 0x000Fu;

	uint8_t xPos = registers[Vx] % VIDEO_WEIGHT;
	uint8_t yPos = registers[Vy] % VIDEO_HEIGHT;

#ifdef DEBUG_CHIP
	std::cout << "[DEBUG] OP_Dxyn (DRW Vx, Vy, nibble): Drawing sprite at (X: "
		<< (int)xPos << ", Y: " << (int)yPos << "), Height: " << (int)height << "\n";
#endif

	registers[0xF] = 0;

	for (unsigned int row = 0; row < height; ++row) {
		uint8_t spriteByte = memory[index + row];
		for (unsigned int col = 0; col < 8; ++col) {
			uint8_t spritePixel = spriteByte & (0x80u >> col);
			uint32_t* screenPixel = &video[(yPos + row) * VIDEO_WEIGHT + (xPos + col)];

			if (spritePixel) {
				if (*screenPixel == 0xFFFFFFFF) {
					registers[0xF] = 1;
				}

				*screenPixel ^= 0xFFFFFFFF;
			}
		}
	}

#ifdef DEBUG_CHIP
	std::cout << "[DEBUG] Finished drawing sprite. Collision flag: "
		<< (int)registers[0xF] << "\n";
#endif
}

// SKP Vx
void Chip8::OP_Ex9E()
{
	uint8_t Vx = (opcode & 0x0F00u) >> 8u;

	uint8_t key = registers[Vx];

#ifdef DEBUG_CHIP
	std::cout << "[DEBUG] OP_Ex9E (SKP Vx): Checking if key " << (int)key << " is pressed\n";
#endif

	if (keypad[key]) {
		pc += 2;

#ifdef DEBUG_CHIP
		std::cout << "[DEBUG] Key pressed. PC incremented by 2.\n";
#endif
	}
}

// SKNP Vx
void Chip8::OP_ExA1()
{
	uint8_t Vx = (opcode & 0x0F00u) >> 8u;
	uint8_t key = registers[Vx];

#ifdef DEBUG_CHIP
	std::cout << "[DEBUG] Key pressed. PC incremented by 2.\n";
#endif

	if (!keypad[key]) {
		pc += 2;

#ifdef DEBUG_CHIP
		std::cout << "[DEBUG] Key not pressed. PC incremented by 2.\n";
#endif
	}
}

// LD Vx, DT
void Chip8::OP_Fx07()
{
	uint8_t Vx = (opcode & 0x0F00u) >> 8u;
	registers[Vx] = delayTimer;
}

// LD Vx, k
void Chip8::OP_Fx0A()
{
	uint8_t Vx = (opcode & 0x0F00u) >> 8u;

#ifdef DEBUG_CHIP
	std::cout << "[DEBUG] OP_Fx0A (LD Vx, k): Waiting for key press to store in Vx(" << (int)Vx << ")\n";
#endif

	if (keypad[0]) {
		registers[Vx] = 0;
	}
	else if (keypad[1]) {
		registers[Vx] = 1;
	}
	else if (keypad[2]) {
		registers[Vx] = 2;
	}
	else if (keypad[3]) {
		registers[Vx] = 3;
	}
	else if (keypad[4]) {
		registers[Vx] = 4;
	}
	else if (keypad[5]) {
		registers[Vx] = 5;
	}
	else if (keypad[6]) {
		registers[Vx] = 6;
	}
	else if (keypad[7]) {
		registers[Vx] = 7;
	}
	else if (keypad[8]) {
		registers[Vx] = 8;
	}
	else if (keypad[9]) {
		registers[Vx] = 9;
	}
	else if (keypad[10]) {
		registers[Vx] = 10;
	}
	else if (keypad[11]) {
		registers[Vx] = 11;
	}
	else if (keypad[12]) {
		registers[Vx] = 12;
	}
	else if (keypad[13]) {
		registers[Vx] = 13;
	}
	else if (keypad[14]) {
		registers[Vx] = 14;
	}
	else if (keypad[15]) {
		registers[Vx] = 15;
	}
	else {
		pc -= 2;
	}
}

// LD DT, Vx
void Chip8::OP_Fx15()
{
	uint8_t Vx = (opcode & 0x0F00u) >> 8u;

#ifdef DEBUG_CHIP
	std::cout << "[DEBUG] OP_Fx15 (LD DT, Vx): Setting delayTimer from Vx(" << (int)Vx << ")\n";
#endif

	delayTimer = registers[Vx];

#ifdef DEBUG_CHIP
	std::cout << "[DEBUG] delayTimer set to: " << (int)delayTimer << "\n";
#endif
}

// LD ST, Vx
void Chip8::OP_Fx18()
{
	uint8_t Vx = (opcode & 0x0F00u) >> 8u;

#ifdef DEBUG_CHIP
	std::cout << "[DEBUG] OP_Fx18 (LD ST, Vx): Setting soundTimer from Vx(" << (int)Vx << ")\n";
#endif

	soundTimer = registers[Vx];

#ifdef DEBUG_CHIP
	std::cout << "[DEBUG] soundTimer set to: " << (int)soundTimer << "\n";
#endif
}

// ADD I, Vx
void Chip8::OP_Fx1E()
{
	uint8_t Vx = (opcode & 0x0F00u) >> 8u;

#ifdef DEBUG_CHIP
	std::cout << "[DEBUG] OP_Fx1E (ADD I, Vx): Adding Vx(" << (int)Vx << ") to index\n";
#endif

	index += registers[Vx];

#ifdef DEBUG_CHIP
	std::cout << "[DEBUG] Index set to: 0x" << std::hex << +index << "\n";
#endif
}

// LD F, Vx	
void Chip8::OP_Fx29()
{
	uint8_t Vx = (opcode & 0x0F00u) >> 8u;
	uint8_t digit = registers[Vx];

#ifdef DEBUG_CHIP
	std::cout << "[DEBUG] OP_Fx29 (LD F, Vx): Setting index to font location for digit " << (int)digit << "\n";
#endif
	
	index = FONTSET_START_ADDRESS + (5 * digit);

#ifdef DEBUG_CHIP
	std::cout << "[DEBUG] Index set to: 0x" << std::hex << +index << "\n";
#endif
}

// LD B, Vx
void Chip8::OP_Fx33()
{
	uint8_t Vx = (opcode & 0x0F00u) >> 8u;
	uint8_t value = registers[Vx];
	
#ifdef DEBUG_CHIP
	std::cout << "[DEBUG] OP_Fx33 (LD B, Vx): Storing BCD of Vx(" << (int)Vx << ") into memory at index\n";
#endif

	memory[index + 2] = value % 10;
	value /= 10;

	memory[index + 1] = value % 10;
	value /= 10;

	memory[index] = value % 10;

#ifdef DEBUG_CHIP
	std::cout << "[DEBUG] Stored BCD: [" << (int)memory[index] << ", " << (int)memory[index + 1] << ", " << (int)memory[index + 2] << "]\n";
#endif
}

// LD [I], Vx
void Chip8::OP_Fx55()
{
	uint8_t Vx = (opcode & 0x0F00u) >> 8u;

#ifdef DEBUG_CHIP
	std::cout << "[DEBUG] OP_Fx55 (LD [I], Vx): Storing registers V0 to V" << (int)Vx << " into memory starting at index 0x" << std::hex << +index << "\n";
#endif

	for (uint8_t i = 0; i <= Vx; ++i) {
		memory[index + i] = registers[i];
	}

#ifdef DEBUG_CHIP
	std::cout << "[DEBUG] Memory from index: ";
	for (uint8_t i = 0; i <= Vx; ++i) {
		std::cout << "0x" << std::hex << +memory[index + i] << " ";
	}
	std::cout << "\n";
#endif
}

// LD Vx, [I]
void Chip8::OP_Fx65()
{
	uint8_t Vx = (opcode & 0x0F00u) >> 8u;

#ifdef DEBUG_CHIP
	std::cout << "[DEBUG] OP_Fx65 (LD Vx, [I]): Loading registers V0 to V" << (int)Vx << " from memory starting at index 0x" << std::hex << +index << "\n";
#endif

	for (uint8_t i = 0; i <= Vx; ++i) {
		 registers[i] = memory[index + i];
	}

#ifdef DEBUG_CHIP
	std::cout << "[DEBUG] Registers after loading: ";
	for (uint8_t i = 0; i <= Vx; ++i) {
		std::cout << "V" << (int)i << ": 0x" << std::hex << +registers[i] << " ";
	}
	std::cout << "\n";
#endif
}