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

    uint32_t videoBuffer[64 * 32];
    bool quit = false;
    SDL_Event e;


    while (!quit)
    {
        while (SDL_PollEvent(&e))
        {
            if (e.type == SDL_EVENT_QUIT)
                quit = true;
        }

        for (int i = 0; i < 10; i++)
        {
            chip8.cycle();
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