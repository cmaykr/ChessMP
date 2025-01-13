#include "piece.hpp"

Piece::Piece(std::string const& texturePath, PieceType const& pieceType, bool isWhite)
    : texturePath(texturePath), pieceType(pieceType), isWhite(isWhite)
{

}

std::string Piece::getTexture() const
{
    return texturePath;
}

PieceType Piece::getType() const
{
    return pieceType;
}

bool Piece::isPieceWhite() const
{
    return isWhite;
}

void Piece::setTexture(std::string const& newPath)
{
    
}

void Piece::setType(PieceType const& newType)
{

}

void Piece::setIsWhite(bool isWhite)
{

}