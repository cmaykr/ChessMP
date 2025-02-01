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

    bool isMoveValid(int startX, int startY, int targetX, int targetY, std::array<std::array<Piece, 8>, 8> localBoard);
    /// @brief Attempts to move a piece on the board. Use this method to move pieces on the board, because a move needs to be checked for several rules before moving.
    /// @return Returns true if move is valid according to all checked rules, false otherwise.
    bool tryMove(int startX, int startY, int targetX, int targetY, std::array<std::array<Piece, 8>, 8> localBoard);

    bool isPlayerWhitesTurn() const;
    bool isGameOver() const;
private:
    /// @brief Checks if a move is blocked by another piece that's in the way. 
    /// @return True if there is a piece blocking the move, otherwise false.
    bool isPieceBlockingTarget(int startX, int startY, int targetX, int targetY, std::array<std::array<Piece, 8>, 8> localBoard);

    bool isCheck(int targetX, int targetY);
    bool isCheckMate(int targetX, int targetY);
    bool willMoveCauseCheck(int startX, int startY, int targetX, int targetY, std::array<std::array<Piece, 8>, 8> localBoard);

    std::array<std::array<Piece, 8>, 8> board{};
    bool _isPlayerWhitesTurn;
    std::ostream &output;

    bool _isGameOver{false};
};