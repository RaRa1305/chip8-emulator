#include <cstdint>
#include <array>
#include<fstream>

class Chip8 {
public:
    Chip8(); 
    void cycle();
    bool loadROM(const char* filename);

    // Memory and registers
    std::array<uint8_t, 4096> memory{};
    std::array<uint8_t, 16> V{};
    uint16_t I{};
    uint16_t PC{};

    // Stack and stack pointer
    std::array<uint16_t, 16> stack{};
    uint8_t sp{};

    // Timers
    uint8_t delayTimer{};
    uint8_t soundTimer{};

    // Interface
    std::array<uint32_t, 64 * 32> display{};
    std::array<uint8_t, 16> keypad{};
};