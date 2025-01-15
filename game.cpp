#include "game.hpp"

#include <iostream>
#include <SDL2/SDL_image.h>

#include "piece.hpp"
#include "resourceManager.hpp"

Game::Game(std::string const& title, int width, int height)
    : xBoardPos{0}, yBoardPos{0}
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
