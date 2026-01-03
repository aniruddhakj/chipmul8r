#include "chip8/Chip8.h"
#include <algorithm>

namespace chipmul8r {

Chip8::Chip8() {
	reset();
}

void Chip8::reset() {
	memory_.assign(4096, 0);
	std::fill(std::begin(V_), std::end(V_), 0);
}

void Chip8::loadROM(const std::vector<uint8_t>& data) {
	// load at 0x200 as typical for Chip-8
	size_t load_addr = 0x200;
	if (load_addr + data.size() <= memory_.size()) {
		std::copy(data.begin(), data.end(), memory_.begin() + load_addr);
	}
}

void Chip8::step() {
	// placeholder: no-op
}

uint8_t Chip8::getRegister(size_t idx) const {
	if (idx < 16) return V_[idx];
	return 0;
}

} // namespace chipmul8r