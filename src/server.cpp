#include <iostream>
#include "game.hpp"

int main(int argc, char** argv)
{
    Game game{std::cout, "8080"};

    game.run();
    return 0;
}