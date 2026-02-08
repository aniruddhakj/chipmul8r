#pragma once
#include <vector>
#include <cstdint>
#include <array>

namespace chipmul8r {

// CHIP-8 display resolution: 64x32 pixels
constexpr size_t DISPLAY_WIDTH = 64;
constexpr size_t DISPLAY_HEIGHT = 32;
constexpr size_t DISPLAY_SIZE = DISPLAY_WIDTH * DISPLAY_HEIGHT;

// Standard CHIP-8 font (5x4 pixels per character)
constexpr size_t FONT_SIZE = 80;
constexpr size_t FONT_START = 0x50;

class Chip8 {
public:
	Chip8();
	void reset();
	void loadROM(const std::vector<uint8_t>& data);
	void step(); // Execute one CPU cycle

	// Display methods
	void clearDisplay();
	void setPixel(size_t x, size_t y, bool state);
	bool getPixel(size_t x, size_t y) const;
	const std::array<bool, DISPLAY_SIZE>& displayBuffer() const { return display_; }
	bool displayDirty() const { return display_dirty_; }
	void resetDisplayDirty() { display_dirty_ = false; }

	// minimal query method for tests
	uint8_t getRegister(size_t idx) const;

private:
	// CPU state
	std::vector<uint8_t> memory_;
	uint8_t V_[16];              // General purpose registers V0-VF
	uint16_t I_;                 // Index register
	uint16_t PC_;                // Program counter
	std::array<uint16_t, 16> stack_;  // Call stack
	uint8_t SP_;                 // Stack pointer
	uint8_t delay_timer_;        // Delay timer
	uint8_t sound_timer_;        // Sound timer

	// Display
	std::array<bool, DISPLAY_SIZE> display_;
	bool display_dirty_;

	// CPU operations
	void loadFonts();
	uint16_t fetch();
	void decode_and_execute(uint16_t opcode);
	void drawSprite(uint8_t x, uint8_t y, uint8_t height);
};

} // namespace chipmul8r