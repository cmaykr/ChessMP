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
    GameClient(Game *game, std::ostream & output, std::string const& serverAddress, std::string const& serverPort);
    ~GameClient();

    void run();
private:
    std::string sendAndReceiveToServer(std::string const& message);
    std::string receiveMessage(int timeout);
    void closeSocket();
    bool tryMove(int chosenX, int chosenY, int boardX, int boardY);

    std::array<std::array<Piece, 8>, 8> &localBoard;
    bool isPlayerWhitesTurn{true};
    bool clientIsPlayerWhite;
    std::ostream &output;

    Game* game;

    int serverFD;
};