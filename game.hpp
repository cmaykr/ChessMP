#pragma once

#include <string>
#include <array>
#include <SDL2/SDL.h>
#include <memory>

#include "piece.hpp"

class Game
{
public:
    Game(std::string const& title, int width, int height);

    void run();

    /// @brief Probably temporary solution to access the board before networking/multiplayer is implemented.
    /// @return 
    std::array<std::array<Piece, 8>, 8>& getBoard();
private:
    int xBoardPos;
    int yBoardPos;

    std::array<std::array<Piece, 8>, 8> board{};
};