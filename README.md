# chipmul8r

C++ Chip-8 emulator skeleton with unit tests.

Structure
- src/         : library and example executable
  - chip8/     : emulator core (headers + implementation)
- tests/       : unit tests (GoogleTest)

Quick build
mkdir build && cd build
cmake ..
cmake --build .
ctest --output-on-failure
