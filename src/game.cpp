#include "game.hpp"

#include <iostream>
#include <SDL2/SDL_image.h>

#include "piece.hpp"
#include "resourceManager.hpp"

Game::Game(std::ostream & output)
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

void Game::run()
{
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
    // Very long method.
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
        return true;
    }

    return false;
}

bool Game::isPlayerWhitesTurn() const
{
    return _isPlayerWhitesTurn;
}