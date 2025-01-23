#pragma once

#include <SDL2/SDL_rect.h>
#include <string>

enum PieceType
{
    UNKNOWN = 0,
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
    Piece();
    Piece(std::string const& texturePath, PieceType const& pieceType, bool isWhite);


    bool isEmpty() const;
    std::string getTexture() const;
    PieceType getType() const;
    bool isPieceWhite() const;

    void setTexture(std::string const& newPath);
    void setType(PieceType const& newType);
    void setIsWhite(bool isWhite);
private:
    std::string texturePath;
    PieceType pieceType;
    bool isWhite;
    bool _hasMoved {false};
};
