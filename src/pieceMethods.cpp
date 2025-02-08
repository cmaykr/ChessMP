#include "pieceMethods.hpp"

#include <array>

/// @brief Checks if a move is blocked by another piece that's in the way. 
/// @return True if there is a piece blocking the move, otherwise false.
bool isPieceBlockingTarget(int startX, int startY, int targetX, int targetY, std::array<std::array<Piece, 8>, 8> localBoard)
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

/// @brief Method currently to large and does too much to fit a comment. TODO: Refactor this method.
bool isMoveValid(int startX, int startY, int targetX, int targetY, std::array<std::array<Piece, 8>, 8> localBoard)
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