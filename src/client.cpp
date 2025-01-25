#include <iostream>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <array>
#include <future>

#include "piece.hpp"
#include "gameClient.hpp"
#include "game.hpp"

int main()
{
    Game game {std::cout};

    GameClient gameClient{&game, std::cout};
    gameClient.run();

    return 0;
}