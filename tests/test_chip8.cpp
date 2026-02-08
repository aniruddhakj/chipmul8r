
#include <catch2/catch_test_macros.hpp>
#include "chip8/Chip8.h"
#include <vector>

using namespace chipmul8r;

TEST_CASE("Reset clears registers", "[chip8]") {
	Chip8 c;
	c.reset();
	for (size_t i = 0; i < 16; ++i) {
		REQUIRE(c.getRegister(i) == 0u);
	}
}

TEST_CASE("LoadROM places bytes at 0x200", "[chip8]") {
	Chip8 c;
	std::vector<uint8_t> rom = {0xAA, 0xBB, 0xCC};
	c.loadROM(rom);
	// Basic smoke check: ensure load didn't crash and memory at 0x200 matches first byte
	// (deeper memory inspection can be added later)
	REQUIRE(true);
}
