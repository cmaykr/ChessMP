#include "game.hpp"

#include <iostream>
#include <SDL2/SDL_image.h>

#include "piece.hpp"
#include "resourceManager.hpp"

Game::Game(std::ostream & output)
    : board{}, output{output}
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
    Piece piece = board[startX][startY];
    
    bool validMove = false;

    switch (piece.getType())
    {
        case PieceType::Pawn:
        {
            if (targetX == startX && (targetY) == startY + 1)
            {
                validMove = true;
            }
            break;
        }
        case PieceType::Rook:
        {
            if (targetX == startX || targetY == startY)
            {
                validMove = true;
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
                validMove = true;
            }
            break;
        }
        case PieceType::Bishop:
        {
            int xDiff = abs(startX - targetX);
            int yDiff = abs(startY - targetY);
            if (xDiff == yDiff)
            {
                validMove = true;
            }
            break;
        }
        case PieceType::Queen:
        {
            int xDiff = abs(startX - targetX);
            int yDiff = abs(startY - targetY);
            if (xDiff == yDiff || targetX == startX || targetY == startY)
            {
                validMove = true;
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
        if (board[targetX][targetY].isEmpty())
        {
            board[targetX][targetY] = piece;
            board[startX][startY] = Piece{};
        }
    }

    return validMove;
}