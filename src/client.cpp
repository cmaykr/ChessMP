#include <iostream>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <array>
#include <future>

#include "piece.hpp"
#include "gameClient.hpp"

int main()
{
    GameClient gameClient{std::cout, "localhost", "8080"};

    gameClient.run();
    return 0;
}