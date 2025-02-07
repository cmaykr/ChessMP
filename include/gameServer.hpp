#pragma once

#include <string>
#include <array>
#include <SDL2/SDL.h>
#include <memory>
#include <ostream>
#include <sys/socket.h>
#include <netdb.h>

#include "piece.hpp"

class GameServer
{
public:
    /// @brief Initializes the game board with pieces in their starting positions, as they are in a standard chess game.
    /// The port parameter sets the port the server listens on for incoming connections.
    GameServer(std::ostream & output, std::string const& port);
    ~GameServer();

    void run();

    /// @brief Temporary solution to access the board before networking/multiplayer is implemented.
    /// @return 
    std::array<std::array<Piece, 8>, 8> &getBoard();

    /// @brief Method currently to large and does too much to fit a comment. TODO: Refactor this method.
    bool isMoveValid(int startX, int startY, int targetX, int targetY, std::array<std::array<Piece, 8>, 8> localBoard);
    /// @brief Attempts to move a piece on the board. Use this method to move pieces on the board, because a move needs to be checked for several rules before moving.
    /// @return Returns true if move succeeded, false otherwise.
    bool tryMove(int startX, int startY, int targetX, int targetY, std::array<std::array<Piece, 8>, 8> localBoard);
private:
    /// @brief Checks if a move is blocked by another piece that's in the way. 
    /// @return True if there is a piece blocking the move, otherwise false.
    bool isPieceBlockingTarget(int startX, int startY, int targetX, int targetY, std::array<std::array<Piece, 8>, 8> localBoard);

    /// @brief Checks if other players king is in check after a move. This should be called before the players turn has changed but after a valid move has been made.
    bool isCheck(int targetX, int targetY);
    /// @brief Checks if the other players king is in checkmate after a move. This is an expensive operation and should only be called if isCheck returns true. If isCheck() returns false, this method will do the same.
    /// TODO: Maybe refactor to make it more efficient if possible. Refactor isCheck and isCheckMate to merge into one method?
    bool isCheckMate(int targetX, int targetY);
    /// @brief Checks if a move will cause a check (or checkmate) for the players own king. It does this by simulating the move and checking for check.
    bool willMoveCauseCheck(int startX, int startY, int targetX, int targetY, std::array<std::array<Piece, 8>, 8> localBoard);

    void closeSockets();

    std::array<std::array<Piece, 8>, 8> gameBoard{};
    bool _isPlayerWhitesTurn;
    std::ostream &output;

    bool _isGameOver{false};

    int serverFD{-1};
    int clientOneFD{-1};
    int clientTwoFD{-1};
};