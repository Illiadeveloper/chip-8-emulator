#include <cstdint>
#include <fstream>

const unsigned int MEMORY_SIZE = 4096;
const unsigned int STACK_LEVEL = 16;
const unsigned int REGISTRE_LEVEL = 16;
const unsigned int VIDEO_HEIGHT = 32;
const unsigned int VIDEO_WEIGHT = 64;
const unsigned int KEY_COUNT = 16;

const unsigned int START_ADDRESS = 0x200;

class Chip8 {
public:

	Chip8();

	void LoadROM(const char* filename);
	
	uint8_t registers[REGISTRE_LEVEL]{};
	uint8_t memory[MEMORY_SIZE]{};
	uint16_t index{};
	uint16_t pc{};
	uint16_t stack[STACK_LEVEL]{};
	uint8_t sp{};
	uint8_t delayTimer{};
	uint8_t soundTimer{};
	uint8_t keypad[KEY_COUNT]{};
	uint32_t video[VIDEO_HEIGHT * VIDEO_WEIGHT]{};
	uint16_t opcode;
};