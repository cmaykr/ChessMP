#pragma once

#include <string>

#include <SDL2/SDL.h>

class Game
{
public:
    Game(std::string const& title, int width, int height);

    void run();
private:
    int xBoardPos;
    int yBoardPos;

    SDL_Window *window;
    SDL_Renderer *renderer;
};