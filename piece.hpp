#pragma once

#include <SDL2/SDL_rect.h>
#include <string>

enum PieceType
{
    Default,
    King,
    Queen,
    Rook,
    Bishop,
    Knight,
    Pawn
};

class Piece
{
public:
    Piece(SDL_Rect const& position, std::string const& texturePosition, PieceType const& pieceType);

    SDL_Rect getPosition() const;
    std::string getTexture() const;
private:
    SDL_Rect _posiition;
    std::string texturePosition;
    PieceType pieceType;
};
