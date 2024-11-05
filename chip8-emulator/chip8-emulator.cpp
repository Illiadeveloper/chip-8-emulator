#include <iostream>
#include "Chip8.hpp"
#include "Platform.hpp"
#include <SFML/Graphics.hpp>


int main(int argc, char* argv[]) {
	if (argc != 4) {
		std::cerr << "Usage: " << argv[0] << " <Scale> <Delay> <ROM>\n";
		std::exit(EXIT_FAILURE);
	}

	int videoScale = std::stoi(argv[1]);
	int cycleDelay = std::stoi(argv[2]);
	const char* romFileName = argv[3];

	Platform platform = Platform("Chip8 Emulator", VIDEO_WEIGHT * videoScale, VIDEO_HEIGHT * videoScale, videoScale, VIDEO_WEIGHT, VIDEO_HEIGHT);


	Chip8 chip8;

	chip8.LoadROM(romFileName);

	auto lastCycleTime = std::chrono::high_resolution_clock::now();

	bool quit = false;

	while (!quit) {
		quit = platform.ProcessInput(chip8.keypad);

		auto currentTime = std::chrono::high_resolution_clock::now();
		float dt = std::chrono::duration<float, std::chrono::milliseconds::period>(currentTime - lastCycleTime).count();
		if (dt > cycleDelay) {
			lastCycleTime = currentTime;

			chip8.Cycle();

			platform.Update(chip8.video);

		}
	}

	return 0;
}