#pragma once
#include "piece.hpp"

#include <array>

bool isPieceBlockingTarget(int startX, int startY, int targetX, int targetY, std::array<std::array<Piece, 8>, 8> localBoard);

bool isMoveValid(int startX, int startY, int targetX, int targetY, std::array<std::array<Piece, 8>, 8> localBoard);