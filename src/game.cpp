#include "game.hpp"

#include <iostream>
#include <SDL2/SDL_image.h>

#include "piece.hpp"
#include "resourceManager.hpp"

Game::Game(std::ostream & output)
    : board{}, output{output}, isPlayerWhitesTurn{true}
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

void Game::run()
{
}

std::array<std::array<Piece, 8>, 8>& Game::getBoard()
{
    return board;
}

bool Game::tryMove(int startX, int startY, int targetX, int targetY, std::array<std::array<Piece, 8>, 8> localBoard)
{
    if (isPlayerWhitesTurn != localBoard[startX][startY].isPieceWhite())
    {
        return false;
    }

    Piece piece = board[startX][startY];
    
    bool validMove = false;

    switch (piece.getType())
    {
        case PieceType::Pawn:
        {
            if (targetX == startX)
            {
                if (board[targetX][targetY].isEmpty() && ((piece.isPieceWhite() && targetY == startY - 1) || (!piece.isPieceWhite() && targetY == startY + 1)))
                {
                    validMove = isPieceBlockingTarget(startX, startY, targetX, targetY);
                }
                else if ((piece.isPieceWhite() && targetY == startY - 2 && startY == 6) || (!piece.isPieceWhite() && targetY == startY + 2 && startY == 1))
                {
                    validMove = isPieceBlockingTarget(startX, startY, targetX, targetY);
                }
            }
            else if (!board[targetX][targetY].isEmpty() && abs(startX - targetX) == 1 && piece.isPieceWhite() != board[targetX][targetY].isPieceWhite())
            {
                if ((piece.isPieceWhite() && targetY == startY - 1) || (!piece.isPieceWhite() && targetY == startY + 1))
                    validMove = isPieceBlockingTarget(startX, startY, targetX, targetY);
            }
            break;
        }
        case PieceType::Rook:
        {
            if (targetX == startX || targetY == startY)
            {
                validMove = isPieceBlockingTarget(startX, startY, targetX, targetY);
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
                validMove = isPieceBlockingTarget(startX, startY, targetX, targetY);
            }
            break;
        }
        case PieceType::Bishop:
        {
            int xDiff = abs(startX - targetX);
            int yDiff = abs(startY - targetY);
            if (xDiff == yDiff)
            {
                validMove = isPieceBlockingTarget(startX, startY, targetX, targetY);
            }
            break;
        }
        case PieceType::Queen:
        {
            int xDiff = abs(startX - targetX);
            int yDiff = abs(startY - targetY);
            if (xDiff == yDiff || targetX == startX || targetY == startY)
            {
                validMove = isPieceBlockingTarget(startX, startY, targetX, targetY);
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

    if (validMove)
    {
        Piece piece = board[startX][startY];
        if (board[targetX][targetY].isEmpty() || board[targetX][targetY].isPieceWhite() != piece.isPieceWhite())
        {
            board[targetX][targetY] = piece;
            board[startX][startY] = Piece{};
            isPlayerWhitesTurn = !isPlayerWhitesTurn;
        }
    }

    return validMove;
}

bool Game::isPieceBlockingTarget(int startX, int startY, int targetX, int targetY)
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
                if (!board[startX][y].isEmpty())
                {
                    return false;
                }
            }
        }
        else
        {
            for (y; y < yEnd; y++)
            {
                if (!board[startX][y].isEmpty())
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
                if (!board[x][startY].isEmpty())
                {
                    return false;
                }
            }
        }
        else
        {
            for (x; x < xEnd; x++)
            {
                if (!board[x][startY].isEmpty())
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

            if (!board[x][y].isEmpty())
            {
                return false;
            }
        }
    }

    return true;
}