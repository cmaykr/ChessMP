#pragma once

#include <array>
#include <memory>
#include <vector>
#include <SDL2/SDL.h>
#include <ostream>

#include "piece.hpp"
#include "game.hpp"

class GameClient
{
public:
    /// @brief Probably temporary solution to pass the board to the client before networking is implemented.
    /// @param board 
    GameClient(Game *game, std::ostream & output);
    ~GameClient() = default;

    void run();
private:
    std::array<std::array<Piece, 8>, 8> &localBoard;
    std::ostream &output;

    Game* game;
};