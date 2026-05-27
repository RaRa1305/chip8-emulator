#include "chip8.hpp"
#include <cstdlib>

const uint8_t font[80] = {
    0xF0, 0x90, 0x90, 0x90, 0xF0, // 0
    0x20, 0x60, 0x20, 0x20, 0x70, // 1
    0xF0, 0x10, 0xF0, 0x80, 0xF0, // 2
    0xF0, 0x10, 0xF0, 0x10, 0xF0, // 3
    0x90, 0x90, 0xF0, 0x10, 0x10, // 4
    0xF0, 0x80, 0xF0, 0x10, 0xF0, // 5
    0xF0, 0x80, 0xF0, 0x90, 0xF0, // 6
    0xF0, 0x10, 0x20, 0x40, 0x40, // 7
    0xF0, 0x90, 0xF0, 0x90, 0xF0, // 8
    0xF0, 0x90, 0xF0, 0x10, 0xF0, // 9
    0xF0, 0x90, 0xF0, 0x90, 0x90, // A
    0xE0, 0x90, 0xE0, 0x90, 0xE0, // B
    0xF0, 0x80, 0x80, 0x80, 0xF0, // C
    0xE0, 0x90, 0x90, 0x90, 0xE0, // D
    0xF0, 0x80, 0xF0, 0x80, 0xF0, // E
    0xF0, 0x80, 0xF0, 0x80, 0x80  // F
};

Chip8::Chip8()
{
    PC = 0x200;

    for (int i = 0; i < 80; i++)
    {
        memory[0x050 + i] = font[i];
    }
}

void Chip8::cycle()
{
    // FETCH
    uint16_t opcode = (memory[PC] << 8u) | memory[PC + 1];
    PC += 2;

    // DECODE & EXECUTE
    uint8_t X = (opcode & 0x0F00u) >> 8u;
    uint8_t Y = (opcode & 0x00F0u) >> 4u;
    uint8_t N = opcode & 0x000Fu;
    uint8_t NN = opcode & 0x00FFu;
    uint16_t NNN = opcode & 0x0FFFu;

    // Isolate the first digit
    switch (opcode & 0xF000u)
    {

    case 0x0000:
        switch (NN)
        {
        case 0xE0u:
            display = {};
            break;

        case 0xEEu:
            if (sp == 0)
                break;
            sp--;
            PC = stack[sp];
            break;

        default:
            break;
        }
        break;

    case 0x1000:
        // 1NNN: Jump to NNN
        PC = NNN;
        break;

    case 0x2000:
        // 2NNN: Jump to NNN
        if (sp >= 16)
            break;
        stack[sp] = PC;
        sp++;
        PC = NNN;
        break;

    case 0x3000:
        // 3XNN: Skips if VX = NN
        if (V[X] == NN)
        {
            PC += 2;
        }
        break;

    case 0x4000:
        // 4XNN: Skips if VX is not equal to NN
        if (V[X] != NN)
        {
            PC += 2;
        }
        break;

    case 0x5000:
        // 5XY0: Skips if VX = VY
        if (V[X] == V[Y])
        {
            PC += 2;
        }
        break;

    case 0x9000:
        // 9XY0: Skips if VX is not equal to VY
        if (V[X] != V[Y])
        {
            PC += 2;
        }
        break;

    case 0x6000:
        // 6XNN: Set register VX
        V[X] = NN;
        break;

    case 0x7000:
        // 7XNN: Add to register VX
        V[X] += NN;
        break;

    case 0x8000:
    {
        switch (opcode & 0x000Fu)
        {
        case 0x0000:
            V[X] = V[Y];
            break;

        case 0x0001:
            V[X] |= V[Y];
            break;

        case 0x0002:
            V[X] &= V[Y];
            break;

        case 0x0003:
            V[X] ^= V[Y];
            break;

        case 0x0004:
        {
            uint16_t sum = V[X] + V[Y];
            if (sum > 255u)
            {
                V[0xF] = 1;
            }
            else
            {
                V[0xF] = 0;
            }
            V[X] = sum & 0xFFu;
            break;
        }

        case 0x0005:
            if (V[X] >= V[Y])
            {
                V[0xF] = 1;
            }
            else
            {
                V[0xF] = 0;
            }
            V[X] -= V[Y];
            break;

        case 0x0006:
        {
            uint8_t flag = V[X] & 0x1u;
            V[X] >>= 1;
            V[0xF] = flag;
            break;
        }

        case 0x0007:
            if (V[X] <= V[Y])
            {
                V[0xF] = 1;
            }
            else
            {
                V[0xF] = 0;
            }
            V[X] = V[Y] - V[X];
            break;

        case 0x000E:
        {
            uint8_t flag = (V[X] & 0x80u) >> 7u;
            V[X] <<= 1;
            V[0xF] = flag;
            break;
        }
        default:
            break;
        }
        break;
    }

    case 0xA000:
        // ANNN: Set Index
        I = NNN;
        break;

    case 0xB000:
        // BNNN: Jump to NNN + V[0]
        PC = V[0] + NNN;
        break;

    case 0xC000:
        // CXNN: Set V[X] = random byte AND NN
        V[X] = (rand() % 256) & NN;
        break;

    case 0xD000:
    {
        uint8_t x = V[X] & 63;
        uint8_t y = V[Y] & 31;
        V[0xF] = 0;

        for (unsigned int row = 0; row < N; row++)
        {
            if (I + row >= 4096)
                break;
            uint8_t spriteByte = memory[I + row];

            for (unsigned int col = 0; col < 8; col++)
            {
                uint8_t spritePixel = spriteByte & (0x80u >> col);

                uint32_t screenX = x + col;
                uint32_t screenY = y + row;

                if (screenX >= 64)
                {
                    break;
                }

                if (screenY >= 32)
                {
                    break;
                }

                if (spritePixel)
                {
                    uint32_t index = screenX + (screenY * 64);
                    if (display[index])
                    {
                        V[0xF] = 1;
                    }

                    display[index] ^= 1;
                }
            }
        }
        break;
    }

    case 0xE000:
    {
        switch (NN)
        {
        case 0x9E:
            if (keypad[V[X]])
            {
                PC += 2;
            }
            break;

        case 0xA1:
            if (!keypad[V[X]])
            {
                PC += 2;
            }
            break;

        default:
            break;
        }
        break;
    }

    case 0xF000:
    {
        switch (NN)
        {
        case 0x07:
            V[X] = delayTimer;
            break;

        case 0x15:
            delayTimer = V[X];
            break;

        case 0x18:
            soundTimer = V[X];
            break;

        case 0x1E:
            I += V[X];
            if (I >= 0x1000)
            {
                V[0xF] = 1;
            }
            break;

        case 0x0A:
        {
            bool press = false;
            for (int i = 0; i < 16; i++)
            {
                if (keypad[i] != 0)
                {
                    V[X] = i;
                    press = true;
                    break;
                }
            }
            if (!press)
            {
                PC -= 2;
            }
            break;
        }

        case 0x29:
            I = 0x050 + (V[X] * 5);
            break;

        case 0x33:
            memory[I] = V[X] / 100;
            memory[I + 1] = (V[X] / 10) % 10;
            memory[I + 2] = V[X] % 10;
            break;

        case 0x55:
        {
            uint16_t idx = I;
            for (int i = 0; i <= X; i++)
            {
                if (idx + i >= 4096)
                    break;
                memory[idx + i] = V[i];
            }
            break;
        }

        case 0x65:
        {
            uint16_t idx = I;
            for (int i = 0; i <= X; i++)
            {
                if (idx + i >= 4096)
                    break;
                V[i] = memory[idx + i];
            }
            break;
        }

        default:
            break;
        }
    }

    default:
        break;
    }
}

bool Chip8::loadROM(const char *filename)
{
    std::ifstream file(filename, std::ios::binary | std::ios::ate);

    if (file.is_open())
    {
        std::streampos size = file.tellg();
        const std::streampos MAX_AVL_SIZE = 4096 - 0x200;
        if (size > MAX_AVL_SIZE)
        {
            return false;
        }
        file.seekg(0, std::ios::beg);
        file.read(reinterpret_cast<char *>(&memory[0x200]), size);
        file.close();

        return true;
    }
    return false;
}