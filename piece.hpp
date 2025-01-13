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
    Piece() = default;
    Piece(std::string const& texturePath, PieceType const& pieceType, bool isWhite);

    std::string getTexture() const;
    PieceType getType() const;
    bool isPieceWhite() const;

    void setTexture(std::string const& newPath);
    void setType(PieceType const& newType);
    void setIsWhite(bool isWhite);
private:
    std::string texturePath;
    PieceType pieceType;

    bool isWhite{};
};
