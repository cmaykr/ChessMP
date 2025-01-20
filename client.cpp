#include <iostream>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <array>

#include "piece.hpp"
#include "gameClient.hpp"
#include "game.hpp"

int main()
{
    Game game {"ChessMP", 640, 480};

    GameClient gameClient{&game};
    gameClient.run();
    
    return 0;
}