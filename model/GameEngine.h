#pragma once
#include "ChessBoard.h"
#include "../common/Types.h"
#include <vector>
#include <utility>
#include <optional>

class GameEngine {
public:
    GameEngine();

    void resetGame();
    PieceColor currentPlayer() const;
    void switchPlayer();
    bool isGameOver() const;
    const ChessBoard& board() const;

    // 走法相关
    std::vector<std::pair<int, int>> getValidMoves(int row, int col) const;
    std::vector<Move> getAllValidMoves(PieceColor color) const;
    bool makeMove(const Move& move);
    bool undoLastMove();   // 悔一步，返回是否成功

    // 状态判定
    bool isInCheck(PieceColor color) const;
    bool isCheckmate(PieceColor color) const;
    bool hasAnyLegalMoves(PieceColor color) const;

private:
    // 辅助函数
    std::vector<std::pair<int, int>> getValidMovesForColor(int row, int col, PieceColor color) const;
    bool moveLeavesKingInCheck(const Move& move, PieceColor moverColor) const;
    std::pair<int, int> findKing(PieceColor color) const;

    // 用于悔棋的历史记录（栈结构，支持多步撤销）
    struct UndoInfo {
        Move move;
        PieceColor moverColor;
        std::optional<Piece> captured;  // 被吃掉的棋子（如果有）
    };
    std::vector<UndoInfo> undoStack_;

    ChessBoard board_;
    PieceColor currentTurn_;
    bool gameOver_;
};
