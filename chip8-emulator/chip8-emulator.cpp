#include <iostream>
#include "Chip8.hpp"


int main(int argc, char* argv[]) {
	if (argc != 4) {
		std::cerr << "Usage: " << argv[0] << " <Scale> <Delay> <ROM>\n";
		std::exit(EXIT_FAILURE);
	}

	int videoScale = 8;
	int cycleDelay = 1;
	int soundDelay = 1;
	const char* romFileName = "test_opcode.ch8";

	Chip8 chip8;

	chip8.LoadROM(romFileName);

	auto lastCycleTime = std::chrono::high_resolution_clock::now();
	

	while (true) {
		auto currentTime = std::chrono::high_resolution_clock::now();
		float dt = std::chrono::duration<float, std::chrono::milliseconds::period>(currentTime - lastCycleTime).count();
		if (dt > cycleDelay) {
			lastCycleTime = currentTime;

			chip8.Cycle();
		}
	}

	return 0;
}