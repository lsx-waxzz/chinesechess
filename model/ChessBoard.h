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

    // 生成不考虑将军过滤的原始走法（目标位置列表）
    std::vector<std::pair<int, int>> rawMoves(int row, int col) const;
    bool inBoard(int r, int c) const;

private:
    // 辅助生成函数
    void addMoveIfValid(int fromRow, int fromCol, int toRow, int toCol,
        std::vector<std::pair<int, int>>& moves) const;
    void addLineMoves(int row, int col, int dr, int dc,
        std::vector<std::pair<int, int>>& moves) const;
    bool hasPieceAt(int row, int col) const;
    bool isSameColor(int row, int col, PieceColor color) const;

    std::vector<std::vector<std::optional<Piece>>> board;
};
