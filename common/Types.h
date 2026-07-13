#pragma once
#include <string>

enum class PieceColor { Red, Black };

enum class PieceType {
    King, Advisor, Elephant, Knight, Rook, Cannon, Pawn
};

struct Piece {
    PieceColor color;
    PieceType type;
};

struct Move {
    int fromRow, fromCol;
    int toRow, toCol;
};
