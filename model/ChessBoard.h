#pragma once
#include <vector>
#include <optional>
#include <utility>
#include "../common/Types.h"

class ChessBoard {
public:
    ChessBoard();

    const std::optional<Piece>& pieceAt(int row, int col) const;
    void setPiece(int row, int col, const std::optional<Piece>& piece);
    void clearPiece(int row, int col);
    void resetBoard();

    std::vector<std::pair<int, int>> rawMoves(int row, int col) const;
    bool inBoard(int r, int c) const;

private:
    std::vector<std::vector<std::optional<Piece>>> board;
};
