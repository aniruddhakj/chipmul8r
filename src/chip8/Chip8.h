#pragma once
#include <vector>
#include <cstdint>

namespace chipmul8r {

class Chip8 {
public:
	Chip8();
	void reset();
	void loadROM(const std::vector<uint8_t>& data);
	void step(); // single emulation step (placeholder)

	// minimal query method for tests
	uint8_t getRegister(size_t idx) const;

private:
	std::vector<uint8_t> memory_;
	uint8_t V_[16];
};

} // namespace chipmul8r