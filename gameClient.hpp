#pragma once

#include <array>
#include <memory>
#include <vector>
#include <SDL2/SDL.h>

#include "piece.hpp"

class GameClient
{
public:
    /// @brief Probably temporary solution to pass the board to the client before networking is implemented.
    /// @param board 
    GameClient(std::array<std::array<Piece, 8>, 8> &board);
    ~GameClient() = default;

    void run();
private:
    std::array<std::array<Piece, 8>, 8> localBoard;
};