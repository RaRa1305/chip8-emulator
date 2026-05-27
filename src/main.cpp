#include <iostream>
#include <SDL3/SDL.h>
#include "chip8.hpp"

const int SCALE = 15;

int main(int argc, char *argv[])
{
    Chip8 chip8;
    if (argc < 2)
    {
        std::cerr << "ROM not specified\n";
        std::cerr << "Usage: .\\chip8_emu.exe <ROM_FILE>\n";
        return -1;
    }

    if (!chip8.loadROM(argv[1]))
    {
        std::cerr << "Failed to load ROM\n";
        return -1;
    }

    SDL_Init(SDL_INIT_VIDEO);

    SDL_Window *window = SDL_CreateWindow("CHIP-8 Emulator", 64 * SCALE, 32 * SCALE, 0);
    SDL_Renderer *renderer = SDL_CreateRenderer(window, nullptr);
    SDL_Texture *texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STREAMING, 64, 32);
    SDL_SetTextureScaleMode(texture, SDL_SCALEMODE_NEAREST);

    uint32_t videoBuffer[64 * 32];
    bool quit = false;
    SDL_Event e;

    while (!quit)
    {
        while (SDL_PollEvent(&e))
        {
            if (e.type == SDL_EVENT_QUIT)
                quit = true;

            else if (e.type == SDL_EVENT_KEY_DOWN)
            {
                switch (e.key.key)
                {
                case SDLK_X:
                    chip8.keypad[0] = 1;
                    break;

                case SDLK_1:
                    chip8.keypad[1] = 1;
                    break;

                case SDLK_2:
                    chip8.keypad[2] = 1;
                    break;

                case SDLK_3:
                    chip8.keypad[3] = 1;
                    break;

                case SDLK_Q:
                    chip8.keypad[4] = 1;
                    break;

                case SDLK_W:
                    chip8.keypad[5] = 1;
                    break;

                case SDLK_E:
                    chip8.keypad[6] = 1;
                    break;

                case SDLK_A:
                    chip8.keypad[7] = 1;
                    break;

                case SDLK_S:
                    chip8.keypad[8] = 1;
                    break;

                case SDLK_D:
                    chip8.keypad[9] = 1;
                    break;

                case SDLK_Z:
                    chip8.keypad[0xA] = 1;
                    break;

                case SDLK_C:
                    chip8.keypad[0xB] = 1;
                    break;

                case SDLK_4:
                    chip8.keypad[0xC] = 1;
                    break;

                case SDLK_R:
                    chip8.keypad[0xD] = 1;
                    break;

                case SDLK_F:
                    chip8.keypad[0xE] = 1;
                    break;

                case SDLK_V:
                    chip8.keypad[0xF] = 1;
                    break;
                }
            }
            else if (e.type == SDL_EVENT_KEY_UP)
            {
                switch (e.key.key)
                {
                case SDLK_X:
                    chip8.keypad[0] = 0;
                    break;
                case SDLK_1:
                    chip8.keypad[1] = 0;
                    break;
                case SDLK_2:
                    chip8.keypad[2] = 0;
                    break;
                case SDLK_3:
                    chip8.keypad[3] = 0;
                    break;
                case SDLK_Q:
                    chip8.keypad[4] = 0;
                    break;
                case SDLK_W:
                    chip8.keypad[5] = 0;
                    break;
                case SDLK_E:
                    chip8.keypad[6] = 0;
                    break;
                case SDLK_A:
                    chip8.keypad[7] = 0;
                    break;
                case SDLK_S:
                    chip8.keypad[8] = 0;
                    break;
                case SDLK_D:
                    chip8.keypad[9] = 0;
                    break;
                case SDLK_Z:
                    chip8.keypad[0xA] = 0;
                    break;
                case SDLK_C:
                    chip8.keypad[0xB] = 0;
                    break;
                case SDLK_4:
                    chip8.keypad[0xC] = 0;
                    break;
                case SDLK_R:
                    chip8.keypad[0xD] = 0;
                    break;
                case SDLK_F:
                    chip8.keypad[0xE] = 0;
                    break;
                case SDLK_V:
                    chip8.keypad[0xF] = 0;
                    break;
                }
            }
        }

        for (int i = 0; i < 10; i++)
        {
            chip8.cycle();
        }

        if (chip8.delayTimer > 0)
        {
            --chip8.delayTimer;
        }

        if (chip8.soundTimer > 0)
        {
            std::cout << "BEEP" << '\n';
            --chip8.soundTimer;
        }

        for (int i = 0; i < 2048; i++)
        {
            videoBuffer[i] = chip8.display[i] ? 0xFFFFFFFF : 0xFF000000;
        }

        SDL_UpdateTexture(texture, nullptr, videoBuffer, 64 * sizeof(uint32_t));
        SDL_RenderClear(renderer);
        SDL_RenderTexture(renderer, texture, nullptr, nullptr);
        SDL_RenderPresent(renderer);

        SDL_Delay(16);
    }

    SDL_DestroyTexture(texture);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}