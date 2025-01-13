#include "piece.hpp"

Piece::Piece(SDL_Rect const& position, std::string const& texturePosition, PieceType const& pieceType)
    : _posiition(position), texturePosition(texturePosition), pieceType(pieceType)
{

}

SDL_Rect Piece::getPosition() const
{
    return _posiition;
}

std::string Piece::getTexture() const
{
    return texturePosition;
}