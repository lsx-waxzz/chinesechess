#include "GameEngine.h"
#include <algorithm>

GameEngine::GameEngine() : currentTurn_(PieceColor::Red), gameOver_(false) {
    board_.resetBoard();
}

void GameEngine::resetGame() {
    board_.resetBoard();
    currentTurn_ = PieceColor::Red;
    gameOver_ = false;
    undoStack_.clear();  // 清空悔棋历史
}

PieceColor GameEngine::currentPlayer() const { return currentTurn_; }
void GameEngine::switchPlayer() {
    currentTurn_ = (currentTurn_ == PieceColor::Red) ? PieceColor::Black : PieceColor::Red;
}
bool GameEngine::isGameOver() const { return gameOver_; }
const ChessBoard& GameEngine::board() const { return board_; }

std::pair<int, int> GameEngine::findKing(PieceColor color) const {
    for (int r = 0; r < 10; ++r)
        for (int c = 0; c < 9; ++c) {
            const auto& piece = board_.pieceAt(r, c);
            if (piece && piece->color == color && piece->type == PieceType::King)
                return { r, c };
        }
    return { -1, -1 };
}

bool GameEngine::isInCheck(PieceColor color) const {
    auto kingPos = findKing(color);
    if (kingPos.first == -1) return false;

    PieceColor enemy = (color == PieceColor::Red) ? PieceColor::Black : PieceColor::Red;
    for (int r = 0; r < 10; ++r)
        for (int c = 0; c < 9; ++c) {
            const auto& piece = board_.pieceAt(r, c);
            if (piece && piece->color == enemy) {
                auto raw = board_.rawMoves(r, c);
                for (auto [tr, tc] : raw) {
                    if (tr == kingPos.first && tc == kingPos.second)
                        return true;
                }
            }
        }

    // 将帅对面
    PieceColor enemyColor = (color == PieceColor::Red) ? PieceColor::Black : PieceColor::Red;
    auto enemyKing = findKing(enemyColor);
    if (enemyKing.first != -1 && enemyKing.second == kingPos.second) {
        int minR = std::min(kingPos.first, enemyKing.first);
        int maxR = std::max(kingPos.first, enemyKing.first);
        bool blocked = false;
        for (int r = minR + 1; r < maxR; ++r) {
            if (board_.pieceAt(r, kingPos.second).has_value()) {
                blocked = true;
                break;
            }
        }
        if (!blocked) return true;
    }
    return false;
}

bool GameEngine::moveLeavesKingInCheck(const Move& move, PieceColor moverColor) const {
    ChessBoard tempBoard = board_;
    auto piece = tempBoard.pieceAt(move.fromRow, move.fromCol);
    tempBoard.clearPiece(move.fromRow, move.fromCol);
    tempBoard.setPiece(move.toRow, move.toCol, piece);

    auto isKingInCheck = [&](const ChessBoard& board, PieceColor color) -> bool {
        auto kingPos = findKing(color); // 但findKing使用this->board_，不行。我们在tempBoard上找
        for (int r = 0; r < 10; ++r)
            for (int c = 0; c < 9; ++c) {
                const auto& p = board.pieceAt(r, c);
                if (p && p->color == color && p->type == PieceType::King) {
                    kingPos = { r, c };
                    break;
                }
            }
        if (kingPos.first == -1) return false;
        PieceColor enemy = (color == PieceColor::Red) ? PieceColor::Black : PieceColor::Red;
        for (int r = 0; r < 10; ++r)
            for (int c = 0; c < 9; ++c) {
                const auto& p = board.pieceAt(r, c);
                if (p && p->color == enemy) {
                    auto raw = board.rawMoves(r, c);
                    for (auto [tr, tc] : raw) {
                        if (tr == kingPos.first && tc == kingPos.second)
                            return true;
                    }
                }
            }
        // 对面
        auto enemyKing = findKing(enemy); // 同样需在board上找
        for (int r = 0; r < 10; ++r)
            for (int c = 0; c < 9; ++c) {
                const auto& p = board.pieceAt(r, c);
                if (p && p->color == enemy && p->type == PieceType::King) {
                    enemyKing = { r, c };
                    break;
                }
            }
        if (enemyKing.first != -1 && enemyKing.second == kingPos.second) {
            int minR = std::min(kingPos.first, enemyKing.first);
            int maxR = std::max(kingPos.first, enemyKing.first);
            bool blocked = false;
            for (int r = minR + 1; r < maxR; ++r) {
                if (board.pieceAt(r, kingPos.second).has_value()) {
                    blocked = true;
                    break;
                }
            }
            if (!blocked) return true;
        }
        return false;
        };
    return isKingInCheck(tempBoard, moverColor);
}

std::vector<std::pair<int, int>> GameEngine::getValidMoves(int row, int col) const {
    if (!board_.inBoard(row, col)) return {};
    const auto& piece = board_.pieceAt(row, col);
    if (!piece) return {};
    auto raw = board_.rawMoves(row, col);
    std::vector<std::pair<int, int>> valid;
    for (auto [tr, tc] : raw) {
        Move move{ row, col, tr, tc };
        if (!moveLeavesKingInCheck(move, piece->color))
            valid.push_back({ tr, tc });
    }
    return valid;
}

std::vector<Move> GameEngine::getAllValidMoves(PieceColor color) const {
    std::vector<Move> moves;
    for (int r = 0; r < 10; ++r)
        for (int c = 0; c < 9; ++c) {
            const auto& piece = board_.pieceAt(r, c);
            if (piece && piece->color == color) {
                auto valid = getValidMoves(r, c);
                for (auto [tr, tc] : valid)
                    moves.push_back({ r, c, tr, tc });
            }
        }
    return moves;
}

bool GameEngine::hasAnyLegalMoves(PieceColor color) const {
    return !getAllValidMoves(color).empty();
}

bool GameEngine::isCheckmate(PieceColor color) const {
    return isInCheck(color) && !hasAnyLegalMoves(color);
}

bool GameEngine::makeMove(const Move& move) {
    if (gameOver_) return false;
    const auto& piece = board_.pieceAt(move.fromRow, move.fromCol);
    if (!piece) return false;
    if (piece->color != currentTurn_) return false;
    auto valid = getValidMoves(move.fromRow, move.fromCol);
    if (std::find(valid.begin(), valid.end(), std::make_pair(move.toRow, move.toCol)) == valid.end())
        return false;

    // 保存历史（走子前）
    UndoInfo info;
    info.move = move;
    info.moverColor = currentTurn_;
    const auto& target = board_.pieceAt(move.toRow, move.toCol);
    info.captured = target.has_value() ? target.value() : std::optional<Piece>{};
    undoStack_.push_back(info);

    // 执行走子
    board_.setPiece(move.toRow, move.toCol, piece);
    board_.clearPiece(move.fromRow, move.fromCol);

    switchPlayer();
    if (!hasAnyLegalMoves(currentTurn_)) {
        gameOver_ = true;
    }
    return true;
}

bool GameEngine::undoLastMove() {
    if (undoStack_.empty()) return false;

    const UndoInfo& info = undoStack_.back();

    // 还原棋子
    auto piece = board_.pieceAt(info.move.toRow, info.move.toCol);
    if (!piece.has_value()) {
        undoStack_.pop_back();
        return false;
    }
    board_.clearPiece(info.move.toRow, info.move.toCol);
    board_.setPiece(info.move.fromRow, info.move.fromCol, piece);

    // 恢复被吃棋子
    if (info.captured.has_value()) {
        board_.setPiece(info.move.toRow, info.move.toCol, info.captured);
    }

    // 切换回走子方（因为走子后切换了玩家，现在反向切换一次）
    switchPlayer();
    gameOver_ = false;
    undoStack_.pop_back();
    return true;
}
