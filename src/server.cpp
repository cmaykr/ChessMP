#include <iostream>
#include "gameServer.hpp"

int main(int argc, char** argv)
{
    GameServer game{std::cout, "8080"};

    game.run();
    return 0;
}