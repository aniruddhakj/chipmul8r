#include <iostream>
#include "chip8/Chip8.h"

int main() {
	chipmul8r::Chip8 emu;
	emu.reset();
	std::cout << "chipmul8r emulator skeleton\n";
	return 0;
}