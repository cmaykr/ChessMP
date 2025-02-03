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
    /// @brief Initializes the game client and connects to the game server using the provided parameters for address and port.
    GameClient(Game *game, std::ostream & output, std::string const& serverAddress, std::string const& serverPort);
    ~GameClient();

    /// @brief Starts the game loop and handles the client-facing game logic, player input and rendering. Also initializes the game and SDL.
    void run();
private:
    /// @brief Sends a message to the game server and waits for a response. Will block until a response from the server is received.
    /// Doesn't currently check the format of the message or what it sends, this is needed to be checked before the message is sent.
    /// @return The response received from the server, if there was nothing to receive it will return an empty string.
    std::string sendAndReceiveToServer(std::string const& message);
    /// @brief Check if there is a message from the server and receive if if there is. 
    /// The checking can be blocking or non-blocking, this is decided by the timeout, -1 to block until a message is received, timeout in milliseconds otherwise.
    /// @param timeout The timeout in milliseconds, valid values are -1 up to 1000.
    /// @return The message received from the server, if no message was in the buffer it will return an empty string.
    std::string receiveMessage(int timeout);
    /// @brief Closes the socket and exits the program.
    void closeSocket();
    /// @brief Attempts to move a piece on the board. Sends a request to the game server to validate the move, the server will respond with a message containing the result and the move.
    /// @return True if the move is valid, false otherwise.
    bool tryMove(int chosenX, int chosenY, int targetX, int targetY);

    std::array<std::array<Piece, 8>, 8> &localBoard;
    bool isPlayerWhitesTurn{true};
    bool clientIsPlayerWhite;
    std::ostream &output;

    Game* game;

    int serverFD;
};