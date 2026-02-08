#include "chip8/Chip8.h"
#include <algorithm>
#include <cstring>

namespace chipmul8r {

// Standard CHIP-8 font data (5 bytes per character, 16 characters)
const uint8_t CHIP8_FONTS[FONT_SIZE] = {
	0xF0, 0x90, 0x90, 0x90, 0xF0,  // 0
	0x20, 0x60, 0x20, 0x20, 0x70,  // 1
	0xF0, 0x10, 0xF0, 0x80, 0xF0,  // 2
	0xF0, 0x10, 0xF0, 0x10, 0xF0,  // 3
	0x90, 0x90, 0xF0, 0x10, 0x10,  // 4
	0xF0, 0x80, 0xF0, 0x10, 0xF0,  // 5
	0xF0, 0x80, 0xF0, 0x90, 0xF0,  // 6
	0xF0, 0x10, 0x20, 0x40, 0x40,  // 7
	0xF0, 0x90, 0xF0, 0x90, 0xF0,  // 8
	0xF0, 0x90, 0xF0, 0x10, 0xF0,  // 9
	0xF0, 0x90, 0xF0, 0x90, 0x90,  // A
	0xE0, 0x90, 0xE0, 0x90, 0xE0,  // B
	0xF0, 0x80, 0x80, 0x80, 0xF0,  // C
	0xE0, 0x90, 0x90, 0x90, 0xE0,  // D
	0xF0, 0x80, 0xF0, 0x80, 0xF0,  // E
	0xF0, 0x80, 0xF0, 0x80, 0x80   // F
};

Chip8::Chip8() : I_(0), PC_(0x200), SP_(0), delay_timer_(0), sound_timer_(0), display_dirty_(false) {
	reset();
}

void Chip8::reset() {
	memory_.assign(4096, 0);
	std::fill(std::begin(V_), std::end(V_), 0);
	I_ = 0;
	PC_ = 0x200;
	SP_ = 0;
	delay_timer_ = 0;
	sound_timer_ = 0;
	clearDisplay();
	loadFonts();
}

void Chip8::loadFonts() {
	std::copy(CHIP8_FONTS, CHIP8_FONTS + FONT_SIZE, memory_.begin() + FONT_START);
}

void Chip8::loadROM(const std::vector<uint8_t>& data) {
	// load at 0x200 as typical for Chip-8
	size_t load_addr = 0x200;
	if (load_addr + data.size() <= memory_.size()) {
		std::copy(data.begin(), data.end(), memory_.begin() + load_addr);
	}
}

uint16_t Chip8::fetch() {
	if (PC_ + 1 >= memory_.size()) return 0;
	uint16_t opcode = (memory_[PC_] << 8) | memory_[PC_ + 1];
	return opcode;
}

void Chip8::step() {
	uint16_t opcode = fetch();
	PC_ += 2;
	decode_and_execute(opcode);

	// Decrease timers
	if (delay_timer_ > 0) delay_timer_--;
	if (sound_timer_ > 0) sound_timer_--;
}

void Chip8::decode_and_execute(uint16_t opcode) {
	uint8_t X = (opcode >> 8) & 0xF;
	uint8_t Y = (opcode >> 4) & 0xF;
	uint8_t N = opcode & 0xF;
	uint8_t NN = opcode & 0xFF;
	uint16_t NNN = opcode & 0xFFF;

	switch (opcode & 0xF000) {
		case 0x0000:
			if (opcode == 0x00E0) {
				clearDisplay();  // Clear display
				display_dirty_ = true;
			} else if (opcode == 0x00EE) {
				SP_--;
				PC_ = stack_[SP_];  // Return
			}
			break;
		case 0x1000:
			PC_ = NNN;  // Jump to NNN
			break;
		case 0x2000:
			stack_[SP_++] = PC_;
			PC_ = NNN;  // Call subroutine
			break;
		case 0x3000:
			if (V_[X] == NN) PC_ += 2;  // Skip if VX == NN
			break;
		case 0x4000:
			if (V_[X] != NN) PC_ += 2;  // Skip if VX != NN
			break;
		case 0x5000:
			if (V_[X] == V_[Y]) PC_ += 2;  // Skip if VX == VY
			break;
		case 0x6000:
			V_[X] = NN;  // Set VX = NN
			break;
		case 0x7000:
			V_[X] += NN;  // Add NN to VX
			break;
		case 0x8000:
			switch (N) {
				case 0x0:
					V_[X] = V_[Y];  // VX = VY
					break;
				case 0x1:
					V_[X] |= V_[Y];  // VX |= VY
					break;
				case 0x2:
					V_[X] &= V_[Y];  // VX &= VY
					break;
				case 0x3:
					V_[X] ^= V_[Y];  // VX ^= VY
					break;
				case 0x4: {
					uint16_t sum = V_[X] + V_[Y];
					V_[0xF] = (sum > 255) ? 1 : 0;
					V_[X] = sum & 0xFF;
					break;
				}
				case 0x5:
					V_[0xF] = (V_[X] >= V_[Y]) ? 1 : 0;
					V_[X] -= V_[Y];
					break;
				case 0x6:
					V_[0xF] = V_[X] & 1;
					V_[X] >>= 1;
					break;
				case 0x7:
					V_[0xF] = (V_[Y] >= V_[X]) ? 1 : 0;
					V_[X] = V_[Y] - V_[X];
					break;
				case 0xE:
					V_[0xF] = (V_[X] >> 7) & 1;
					V_[X] <<= 1;
					break;
			}
			break;
		case 0x9000:
			if (V_[X] != V_[Y]) PC_ += 2;  // Skip if VX != VY
			break;
		case 0xA000:
			I_ = NNN;  // Set I = NNN
			break;
		case 0xB000:
			PC_ = NNN + V_[0];  // Jump with offset
			break;
		case 0xD000:
			drawSprite(V_[X], V_[Y], N);  // Draw sprite
			break;
		case 0xF000:
			switch (NN) {
				case 0x07:
					V_[X] = delay_timer_;
					break;
				case 0x15:
					delay_timer_ = V_[X];
					break;
				case 0x18:
					sound_timer_ = V_[X];
					break;
				case 0x1E:
					I_ += V_[X];
					break;
				case 0x29:
					I_ = FONT_START + (V_[X] * 5);  // Font address
					break;
				case 0x33: {
					uint8_t val = V_[X];
					memory_[I_] = val / 100;
					memory_[I_ + 1] = (val / 10) % 10;
					memory_[I_ + 2] = val % 10;
					break;
				}
				case 0x55:
					for (int i = 0; i <= X; ++i) {
						memory_[I_ + i] = V_[i];
					}
					break;
				case 0x65:
					for (int i = 0; i <= X; ++i) {
						V_[i] = memory_[I_ + i];
					}
					break;
			}
			break;
	}
}

void Chip8::drawSprite(uint8_t x, uint8_t y, uint8_t height) {
	V_[0xF] = 0;  // Clear collision flag
	for (uint8_t row = 0; row < height; ++row) {
		uint8_t sprite_byte = memory_[I_ + row];
		for (uint8_t col = 0; col < 8; ++col) {
			bool sprite_pixel = (sprite_byte >> (7 - col)) & 1;
			if (!sprite_pixel) continue;

			int px = (x + col) % DISPLAY_WIDTH;
			int py = (y + row) % DISPLAY_HEIGHT;
			size_t idx = py * DISPLAY_WIDTH + px;

			if (display_[idx] && sprite_pixel) {
				V_[0xF] = 1;  // Collision detected
			}
			display_[idx] ^= sprite_pixel;
		}
	}
	display_dirty_ = true;
}

void Chip8::clearDisplay() {
	display_.fill(false);
}

void Chip8::setPixel(size_t x, size_t y, bool state) {
	if (x < DISPLAY_WIDTH && y < DISPLAY_HEIGHT) {
		display_[y * DISPLAY_WIDTH + x] = state;
		display_dirty_ = true;
	}
}

bool Chip8::getPixel(size_t x, size_t y) const {
	if (x < DISPLAY_WIDTH && y < DISPLAY_HEIGHT) {
		return display_[y * DISPLAY_WIDTH + x];
	}
	return false;
}

uint8_t Chip8::getRegister(size_t idx) const {
	if (idx < 16) return V_[idx];
	return 0;
}

} // namespace chipmul8r