#include "game.hpp"

#include <iostream>
#include <SDL2/SDL_image.h>

#include "piece.hpp"

Game::Game(std::string const& title, int width, int height)
{
    if (SDL_Init(SDL_INIT_VIDEO) != 0)
        std::cerr << "SDL_init error: " << SDL_GetError() << std::endl;

    SDL_Window *window = SDL_CreateWindow(title.c_str(), SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, width, height, 0);
    if (window == NULL)
        std::cerr << "SDL_CreateWindow error: " << SDL_GetError() << std::endl;

    SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, 0);
    if (renderer == NULL)
        std::cerr << "SDL_CreateRenderer error: " << SDL_GetError() << std::endl;
}

void Game::run()
{
    SDL_Rect box{};
    box.x = 20;
    box.y = 20;

    Piece piece {box, "models/pawn.png", PieceType::Pawn};
    SDL_Surface *surface = IMG_Load(piece.getTexture().c_str());
    SDL_Texture *tex = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_FreeSurface(surface);
    SDL_Rect pos;
    pos.h = 40;
    pos.w = 40;

    int size = 40;

    box.w = size;
    box.h = size;

    int w;
    int h;
    SDL_GetWindowSize(window, &w, &h);
    int xStart{w / 2 - size*4};
    int yStart{h / 2 - size*4};
    bool running = true;
    while (running)
    {
        SDL_Event e;
        while (SDL_PollEvent(&e))
        {
            switch (e.type)
            {
                case SDL_QUIT:
                    running = false;
                    break;
            }
        }

        SDL_SetRenderDrawColor(renderer, 0, 255, 255, 255);
        SDL_RenderClear(renderer);
        box.x = xStart;
        box.y = yStart;

        for (int i{}; i < 8; i++)
        {
            for (int j{}; j < 8; j++)
            {
                if ((i + j) % 2 == 0)
                    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
                else
                    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
                SDL_RenderFillRect(renderer, &box);

                box.x += size;
            }
            box.y += size;
            box.x = xStart;
        }
        SDL_SetTextureColorMod(tex, 255, 255, 0);
        SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
        //pos = piece.getPosition();
        SDL_GetMouseState( &pos.x, &pos.y);
        SDL_RenderCopy(renderer, tex, NULL, &pos);

        SDL_RenderPresent(renderer);
    }
}