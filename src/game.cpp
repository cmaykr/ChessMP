#include "game.hpp"

#include <iostream>
#include <SDL2/SDL_image.h>
#include <string.h>
#include <sys/socket.h>
#include <netdb.h>
#include <charconv>
#include <unistd.h>
#include <poll.h>

#include "piece.hpp"
#include "resourceManager.hpp"

Game::Game(std::ostream & output, std::string const& port)
    : board{}, output{output}, _isPlayerWhitesTurn{true}
{
    Piece pawn {"models/pawn.png", PieceType::Pawn, true};
    Piece rook {"models/rook.png", PieceType::Rook, true};
    Piece knight {"models/knight.png", PieceType::Knight, true};
    Piece bishop {"models/bishop.png", PieceType::Bishop, true};
    Piece queen {"models/queen.png", PieceType::Queen, true};
    Piece king {"models/king.png", PieceType::King, true};
    Piece pawnB {"models/pawn.png", PieceType::Pawn, false};
    Piece rookB {"models/rook.png", PieceType::Rook, false};
    Piece knightB {"models/knight.png", PieceType::Knight, false};
    Piece bishopB {"models/bishop.png", PieceType::Bishop, false};
    Piece queenB {"models/queen.png", PieceType::Queen, false};
    Piece kingB {"models/king.png", PieceType::King, false};

    board[0][7] = rook;
    board[1][7] = knight;
    board[2][7] = bishop;
    board[3][7] = queen;
    board[4][7] = king;
    board[5][7] = bishop;
    board[6][7] = knight;
    board[7][7] = rook;

    for (int i{}; i < 8; i++)
    {
        board[i][6] = pawn;
    }

    board[0][0] = rookB;
    board[1][0] = knightB;
    board[2][0] = bishopB;
    board[3][0] = queenB;
    board[4][0] = kingB;
    board[5][0] = bishopB;
    board[6][0] = knightB;
    board[7][0] = rookB;

    for (int i{}; i < 8; i++)
    {
        board[i][1] = pawnB;
    }
}

Game::~Game()
{
    closeSockets();
}

void Game::run()
{
    struct addrinfo hints;
    struct addrinfo *rp, *result;

    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;

    int err = getaddrinfo(NULL, "8080", &hints, &result);
    if (err < 0)
    {
        output << "Error getting address " << strerror(errno) << std::endl;
        exit(2);
    }

    for (rp = result; rp != nullptr; rp = rp->ai_next)
    {
        serverFD = socket(rp->ai_family, rp->ai_socktype, rp->ai_protocol);

        if (serverFD == -1)
            continue;
        if (bind(serverFD, rp->ai_addr, rp->ai_addrlen) == 0)
        {
            char host[NI_MAXHOST], service[NI_MAXSERV];
            break;
        }

        closeSockets();
    }

    freeaddrinfo(result);
    if (listen(serverFD, 5) < 0)
    {
        output << "Failed to prepare socket for accepting connections.";
        exit(1);
    }

    output << "Server initalized" << std::endl;
    clientOneFD = accept(serverFD, NULL, NULL);
    clientTwoFD = accept(serverFD, NULL, NULL);

    while (true)
    {
        char buf[1024];
        struct pollfd fds[2];
        fds[0].fd = clientOneFD;
        fds[0].events = POLLIN;
        fds[1].fd = clientTwoFD;
        fds[1].events = POLLIN;
        int N{};
        int polls = poll(fds, 2, -1);
        if (polls > 0)
        {
            output << "Nr of events: " << polls << std::endl;
            for (int i{}; i < 2; i++)
            {
                if (fds[i].revents == POLLIN)
                {
                    N = recv(fds[i].fd, buf, sizeof(buf), 0);
                    if (N == -1)
                    {
                        std::cerr << "Socket failed when receiving message" << std::endl;
                        closeSockets();
                        exit(1);
                    }
                    if (N == 0)
                    {
                        std::cerr << "Peer socket not available, closing connection" << std::endl;
                        closeSockets();
                        exit(2);
                    }
                    std::string message {buf, N};
                    output << message << " From: " << fds[i].fd << std::endl;

                    if (message.substr(0, message.find(':')) == "MOVE")
                    {
                        int startX{}, startY{};
                        int targetX{}, targetY{};
                        std::stringstream ss{message.substr(message.find(':') + 1, message.size())};
                        std::string text;
                        while (getline(ss, text, '}'))
                        {
                            std::stringstream ss2{text};
                            int num;
                            if (text.find("from") != std::string::npos)
                            {
                                std::string from = text.substr(text.find('{') + 2, text.find('}') - text.find('{'));
                                output << from << "." <<  std::endl;
                                std::string x = from.substr(0, from.find(' '));
                                std::string y = from.substr(from.find(' ') + 1, 1);
                                output << x << " " << y << "." << std::endl;
                                startX = std::stoi(x);
                                startY = std::stoi(y);
                            }
                            else if (text.find("to") != std::string::npos)
                            {
                                std::string to = text.substr(text.find('{') + 2, text.find('}') - text.find('{') - 1);
                                output << to << std::endl;
                                std::string x = to.substr(0, to.find(' '));
                                std::string y = to.substr(to.find(' ') + 1, 1);
                                output << x << " " << y << "." << std::endl;
                                targetX = std::stoi(x);
                                targetY = std::stoi(y);
                            }
                            else
                            {
                                output << "Error parsing message" << std::endl;
                            }
                            output << text << std::endl;
                        }
                        std::string test = "From: " + std::to_string(startX) + " " + std::to_string(startY) + " To: " + std::to_string(targetX) + " " + std::to_string(targetY);
                        //output << test << std::endl;
                        std::stringstream response{};
                        response << message << std::endl << "Status: ";
                        if (tryMove(startX, startY, targetX, targetY, board))
                        {
                            response << "1";
                        }
                        else
                        {
                            response << "0";
                        }
                        response << std::endl;
                        message = response.str();
                        output << message << std::endl;
                        send(clientOneFD, message.c_str(), message.size(), 0);
                        send(clientTwoFD, message.c_str(), message.size(), 0);
                    }
                    else if (message.substr(0, message.find(':')) == "Request")
                    {
                        std::string response = "Status: 1\nPlayer: ";
                        if (clientOneFD == fds[i].fd)
                        {
                            response += "White";
                        }
                        else if (clientTwoFD == fds[i].fd)
                        {
                            response += "Black";
                        }
                        response += "\n";
                        send(fds[i].fd, response.c_str(), response.size(), 0);
                    }
                    else
                    {
                        output << "Unknown message" << std::endl;
                    }
                }
                else
                {
                    output << "No events from: " << fds[i].fd << std::endl;
                }

            }


        }
        else if (polls <= 0)
        {
            std::cerr << "Nothing to receive" << std::endl;
            closeSockets();
        }
    }
}

std::array<std::array<Piece, 8>, 8>& Game::getBoard()
{
    return board;
}

bool Game::isMoveValid(int startX, int startY, int targetX, int targetY, std::array<std::array<Piece, 8>, 8> localBoard)
{
    Piece piece = localBoard[startX][startY];
    
    bool validMove = false;

    switch (piece.getType())
    {
        case PieceType::Pawn:
        {
            if (targetX == startX)
            {
                if (localBoard[targetX][targetY].isEmpty() && ((piece.isPieceWhite() && targetY == startY - 1) || (!piece.isPieceWhite() && targetY == startY + 1)))
                {
                    validMove = isPieceBlockingTarget(startX, startY, targetX, targetY, localBoard);
                }
                else if (localBoard[targetX][targetY].isEmpty() && ((piece.isPieceWhite() && targetY == startY - 2 && startY == 6) || (!piece.isPieceWhite() && targetY == startY + 2 && startY == 1)))
                {
                    validMove = isPieceBlockingTarget(startX, startY, targetX, targetY, localBoard);
                }
            }
            else if (!localBoard[targetX][targetY].isEmpty() && abs(startX - targetX) == 1 && piece.isPieceWhite() != localBoard[targetX][targetY].isPieceWhite())
            {
                if ((piece.isPieceWhite() && targetY == startY - 1) || (!piece.isPieceWhite() && targetY == startY + 1))
                    validMove = isPieceBlockingTarget(startX, startY, targetX, targetY, localBoard);
            }
            break;
        }
        case PieceType::Rook:
        {
            if (targetX == startX || targetY == startY)
            {
                validMove = isPieceBlockingTarget(startX, startY, targetX, targetY, localBoard);
            }
            break;
        }
        case PieceType::King:
        {
            int xDiff = abs(startX - targetX);
            int yDiff = abs(startY - targetY);

            bool validMoveXAxis = xDiff == 1 || xDiff == 0;
            bool validMoveYAxis = yDiff == 1 || yDiff == 0;
            if (validMoveXAxis && validMoveYAxis)
            {
                validMove = isPieceBlockingTarget(startX, startY, targetX, targetY, localBoard);
            }
            break;
        }
        case PieceType::Bishop:
        {
            int xDiff = abs(startX - targetX);
            int yDiff = abs(startY - targetY);
            if (xDiff == yDiff)
            {
                validMove = isPieceBlockingTarget(startX, startY, targetX, targetY, localBoard);
            }
            break;
        }
        case PieceType::Queen:
        {
            int xDiff = abs(startX - targetX);
            int yDiff = abs(startY - targetY);
            if (xDiff == yDiff || targetX == startX || targetY == startY)
            {
                validMove = isPieceBlockingTarget(startX, startY, targetX, targetY, localBoard);
            }
            break;
        }
        case PieceType::Knight:
        {
            int xDiff = abs(startX - targetX);
            int yDiff = abs(startY - targetY);
            if (xDiff == 2 && yDiff == 1)
            {
                validMove = true;
            }
            else if (yDiff == 2 && xDiff == 1)
            {
                
                validMove = true;
            }
            break;
        }
        default:
            break;
    }

    if ((localBoard[targetX][targetY].isEmpty() || localBoard[targetX][targetY].isPieceWhite() != piece.isPieceWhite()) == false)
    {
        validMove = false;
    }
    
    return validMove;
}

bool Game::tryMove(int startX, int startY, int targetX, int targetY, std::array<std::array<Piece, 8>, 8> localBoard)
{
    board = localBoard;

    if (isMoveValid(startX, startY, targetX, targetY, localBoard) && _isPlayerWhitesTurn == localBoard[startX][startY].isPieceWhite())
    {
        if (willMoveCauseCheck(startX, startY, targetX, targetY, localBoard))
        {
            output << "Move will cause a check" << std::endl;
            return false;
        }
        Piece piece = board[startX][startY];
        board[targetX][targetY] = piece;
        board[startX][startY] = Piece{};
        if (isCheck(targetX, targetY))
        {
            output << "King is in check" << std::endl;
        }
        if (isCheckMate(targetX, targetY))
        {
            output << "Checkmate" << std::endl;
        }
        _isPlayerWhitesTurn = !_isPlayerWhitesTurn;

        return true;
    }

    return false;
}

bool Game::isPieceBlockingTarget(int startX, int startY, int targetX, int targetY, std::array<std::array<Piece, 8>, 8> localBoard)
{
    // This can most likely be done in less code, or at least more readable.
    if (startX == targetX)
    {
        int y = ((startY < targetY) ? startY : targetY) + 1;
        int yEnd = ((startY < targetY) ? targetY : startY);
        if (startY < targetY)
        {
            for (y; y < yEnd; y++)
            {
                if (!localBoard[startX][y].isEmpty())
                {
                    return false;
                }
            }
        }
        else
        {
            for (y; y < yEnd; y++)
            {
                if (!localBoard[startX][y].isEmpty())
                {
                    return false;
                }
            }
        }
    }
    else if (startY == targetY)
    {
        int x = ((startX < targetX) ? startX : targetX) + 1;
        int xEnd = ((startX < targetX) ? targetX : startX);
        if (startX < targetX)
        {
            for (x; x < xEnd; x++)
            {
                if (!localBoard[x][startY].isEmpty())
                {
                    return false;
                }
            }
        }
        else
        {
            for (x; x < xEnd; x++)
            {
                if (!localBoard[x][startY].isEmpty())
                {
                    return false;
                }
            }
        }
    }
    else
    {
        int x = startX;
        int y = startY;
        int dist = abs(startX - targetX);

        int xMod = (startX < targetX) ? 1 : -1;
        int yMod = (startY < targetY) ? 1 : -1;

        for (int i{}; i < dist - 1; i++)
        {
            x += 1*xMod;
            y += 1*yMod;

            if (!localBoard[x][y].isEmpty())
            {
                return false;
            }
        }
    }

    return true;
}

bool Game::isCheck(int targetX, int targetY)
{
    Piece movedPiece = board[targetX][targetY];

    Piece enemyKing{};
    int xKing{-1};
    int yKing{-1};

    for (int x {}; x < 8; x++)
    {
        for (int y {}; y < 8; y++)
        {
            if (board[x][y].getType() == PieceType::King && board[x][y].isPieceWhite() != _isPlayerWhitesTurn)
            {
                enemyKing = board[x][y];
                xKing = x;
                yKing = y;
                break;
            }
        }
    }

    if (enemyKing.isEmpty() || xKing == -1 || yKing == -1)
    {
        output << "There is no enemy king, no valid game, exiting." << std::endl;
        exit(1);
    }

    return isMoveValid(targetX, targetY, xKing, yKing, board);
}

bool Game::willMoveCauseCheck(int startX, int startY, int targetX, int targetY, std::array<std::array<Piece, 8>, 8> localBoard)
{
    std::array<std::array<Piece, 8>, 8> futureBoard = localBoard;

    futureBoard[targetX][targetY] = board[startX][startY];
    futureBoard[startX][startY] = Piece{};
    int kingX{};
    int kingY{};

    for (int x {}; x < 8; x++)
    {
        for (int y {}; y < 8; y++)
        {
            if (futureBoard[x][y].getType() == PieceType::King && futureBoard[x][y].isPieceWhite() == futureBoard[targetX][targetY].isPieceWhite())
            {
                kingX = x;
                kingY = y;

                break;
            }
        }
    }

    for (int x{}; x < 8; x++)
    {
        for (int y{}; y < 8; y++)
        {
            if (!futureBoard[x][y].isEmpty() && futureBoard[x][y].isPieceWhite() != futureBoard[targetX][targetY].isPieceWhite())
            {
                if (isMoveValid(x, y, kingX, kingY, futureBoard))
                {
                    return true;
                }
            }
        }
    }

    return false;
}

bool Game::isCheckMate(int targetX, int targetY)
{
    std::array<std::array<Piece, 8>, 8> futureBoard = board;

    if (isCheck(targetX, targetY))
    {
        int kingX{};
        int kingY{};

        for (int x {}; x < 8; x++)
        {
            for (int y {}; y < 8; y++)
            {
                if (futureBoard[x][y].getType() == PieceType::King && futureBoard[x][y].isPieceWhite() != _isPlayerWhitesTurn)
                {
                    kingX = x;
                    kingY = y;

                    break;
                }
            }
        }
        Piece checkedKing = futureBoard[kingX][kingY];

        for (int dX{-1}; dX <= 1; dX++)
        {
            for (int dY{-1}; dY <= 1; dY++)
            {
                if (kingX + dX < 0 || kingX + dX > 7 || kingY + dY < 0 || kingY + dY > 7)
                {
                    continue;
                }
                if (dX == 0 && dY == 0)
                    continue;

                if (!willMoveCauseCheck(kingX, kingY, kingX + dX, kingY + dY, futureBoard))
                {
                    if (isMoveValid(kingX, kingY, kingX + dX, kingY + dY, futureBoard) )
                    {
                        return false;
                    }
                }
            }
        }
        _isGameOver = true;
        return true;
    }

    return false;
}

bool Game::isPlayerWhitesTurn() const
{
    return _isPlayerWhitesTurn;
}

bool Game::isGameOver() const
{
    return _isGameOver;
}


void Game::closeSockets()
{
    while (recv(serverFD, nullptr, 0, 0) > 0)
        continue;
    close(serverFD);

    while (recv(clientOneFD, nullptr, 0, 0) > 0)
        continue;
    close(clientOneFD);

    while (recv(clientTwoFD, nullptr, 0, 0) > 0)
        continue;
    close(clientTwoFD);
}