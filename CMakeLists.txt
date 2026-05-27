cmake_minimum_required(VERSION 3.10)
project(Chip8Emulator)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include_directories(include)

find_package(SDL3 REQUIRED)

add_executable(chip8 src/main.cpp src/chip8.cpp)

target_link_libraries(chip8 PRIVATE SDL3::SDL3)