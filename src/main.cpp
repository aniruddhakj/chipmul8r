#include <iostream>
#include <fstream>
#include <vector>
#include "chip8/Chip8.h"

void renderDisplay(const chipmul8r::Chip8& emu) {
	const auto& display = emu.displayBuffer();
	
	std::cout << "\n";
	for (size_t y = 0; y < chipmul8r::DISPLAY_HEIGHT; ++y) {
		for (size_t x = 0; x < chipmul8r::DISPLAY_WIDTH; ++x) {
			bool pixel = display[y * chipmul8r::DISPLAY_WIDTH + x];
			std::cout << (pixel ? "█" : " ");
		}
		std::cout << "\n";
	}
}

int main(int argc, char* argv[]) {
	if (argc < 2) {
		std::cout << "Usage: chip8_bin <rom_file>\n";
		return 1;
	}

	// Read ROM file
	std::ifstream rom_file(argv[1], std::ios::binary);
	if (!rom_file.is_open()) {
		std::cerr << "Error: Could not open ROM file: " << argv[1] << "\n";
		return 1;
	}

	std::vector<uint8_t> rom_data((std::istreambuf_iterator<char>(rom_file)),
	                               std::istreambuf_iterator<char>());
	rom_file.close();

	std::cout << "chipmul8r emulator\n";
	std::cout << "Loaded ROM: " << argv[1] << " (" << rom_data.size() << " bytes)\n";

	// Initialize emulator
	chipmul8r::Chip8 emu;
	emu.reset();
	emu.loadROM(rom_data);

	// Run emulation cycles
	std::cout << "Executing ROM...\n";
	const int cycles = 500;  // Run 500 cycles
	for (int i = 0; i < cycles; ++i) {
		emu.step();
	}

	std::cout << "Execution complete (" << cycles << " cycles executed)\n";
	
	// Render display
	std::cout << "\nDisplay buffer:\n";
	renderDisplay(emu);

	return 0;
}