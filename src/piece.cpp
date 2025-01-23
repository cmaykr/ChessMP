#include "piece.hpp"
#include <string>
#include <iostream>

Piece::Piece()
    : texturePath(""), pieceType(PieceType::UNKNOWN), isWhite(false)
{

}

Piece::Piece(std::string const& texturePath, PieceType const& pieceType, bool isWhite)
    : texturePath(texturePath), pieceType(pieceType), isWhite(isWhite)
{

}

bool Piece::isEmpty() const
{
    if (pieceType == PieceType::UNKNOWN)
        return true;
    
    return false;
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
    // TODO: Check for correct format?.
    texturePath = newPath;
}

void Piece::setType(PieceType const& newType)
{
    if (newType != PieceType::UNKNOWN)
        pieceType = newType;
}

void Piece::setIsWhite(bool isWhite)
{
    this->isWhite = isWhite;
}