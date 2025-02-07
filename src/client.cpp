#include <iostream>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <array>
#include <future>

#include "piece.hpp"
#include "gameClient.hpp"
#include "gameServer.hpp"

int main()
{
    GameServer game {std::cout, "8080"};

    GameClient gameClient{&game, std::cout, "localhost", "8080"};

    gameClient.run();
    return 0;
}