#pragma once

#include <string>
#include <array>
#include <SDL2/SDL.h>
#include <memory>
#include <ostream>

#include "piece.hpp"

class Game
{
public:
    Game(std::ostream & output);

    void run();

    /// @brief Probably temporary solution to access the board before networking/multiplayer is implemented.
    /// @return 
    std::array<std::array<Piece, 8>, 8> &getBoard();

    /// @brief Attempts to move a piece on the board. Use this method to move pieces on the board, because a move needs to be checked for several rules before moving.
    /// @return Returns true if move is valid according to all checked rules, false otherwise.
    bool tryMove(int startX, int startY, int targetX, int targetY, std::array<std::array<Piece, 8>, 8> localBoard);
private:

    std::array<std::array<Piece, 8>, 8> board{};
    std::ostream &output;
};