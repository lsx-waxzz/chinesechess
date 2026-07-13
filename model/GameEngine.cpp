#include "GameEngine.h"
#include <algorithm>

GameEngine::GameEngine() : currentTurn_(PieceColor::Red), gameOver_(false) {
    board_.resetBoard();
}

void GameEngine::resetGame() {
    board_.resetBoard();
    currentTurn_ = PieceColor::Red;
    gameOver_ = false;
}

PieceColor GameEngine::currentPlayer() const { return currentTurn_; }
void GameEngine::switchPlayer() {
    currentTurn_ = (currentTurn_ == PieceColor::Red) ? PieceColor::Black : PieceColor::Red;
}
bool GameEngine::isGameOver() const { return gameOver_; }
const ChessBoard& GameEngine::board() const { return board_; }

std::vector<std::pair<int, int>> GameEngine::getValidMoves(int row, int col) const { return {}; }
std::vector<Move> GameEngine::getAllValidMoves(PieceColor color) const { return {}; }
bool GameEngine::makeMove(const Move& move) { return false; }
bool GameEngine::isInCheck(PieceColor color) const { return false; }
bool GameEngine::isCheckmate(PieceColor color) const { return false; }

std::vector<std::pair<int, int>> GameEngine::getValidMovesForColor(int row, int col, PieceColor color) const { return {}; }
bool GameEngine::hasAnyLegalMoves(PieceColor color) const { return false; }
std::pair<int, int> GameEngine::findKing(PieceColor color) const { return { -1, -1 }; }
bool GameEngine::moveLeavesKingInCheck(const Move& move, PieceColor moverColor) const { return false; }
