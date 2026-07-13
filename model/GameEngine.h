#pragma once
#include "ChessBoard.h"
#include "../common/Types.h"
#include <vector>
#include <utility>

class GameEngine {
public:
    GameEngine();

    void resetGame();
    PieceColor currentPlayer() const;
    void switchPlayer();
    bool isGameOver() const;
    const ChessBoard& board() const;

    std::vector<std::pair<int, int>> getValidMoves(int row, int col) const;
    std::vector<Move> getAllValidMoves(PieceColor color) const;
    bool makeMove(const Move& move);
    bool isInCheck(PieceColor color) const;
    bool isCheckmate(PieceColor color) const;

private:
    std::vector<std::pair<int, int>> getValidMovesForColor(int row, int col, PieceColor color) const;
    bool hasAnyLegalMoves(PieceColor color) const;
    ChessBoard board_;
    PieceColor currentTurn_;
    bool gameOver_;
    std::pair<int, int> findKing(PieceColor color) const;
    bool moveLeavesKingInCheck(const Move& move, PieceColor moverColor) const;
};
