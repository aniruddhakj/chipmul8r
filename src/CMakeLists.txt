add_library(chip8_lib
	chip8/Chip8.cpp
)

target_include_directories(chip8_lib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

add_executable(chip8_bin main.cpp)
target_link_libraries(chip8_bin PRIVATE chip8_lib)
