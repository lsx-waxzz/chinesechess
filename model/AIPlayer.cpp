#include "AIPlayer.h"
#include <chrono>
#include <limits>

Move AIPlayer::getBestMove(const GameEngine& game, AIDifficulty difficulty) {
    return getBestMoveTimed(game, difficulty, 0);
}

Move AIPlayer::getBestMoveTimed(const GameEngine& game, AIDifficulty difficulty, int timeLimitMs) {
    // 中期不实现 — 返回无效移动表示跳过
    return { -1, -1, -1, -1 };
}

int AIPlayer::evaluateBoard(const GameEngine& game, PieceColor aiColor) {
    return 0;
}

int AIPlayer::minimax(GameEngine& game, int depth, int alpha, int beta,
    bool maximizingPlayer, PieceColor aiColor,
    TimePoint startTime, int timeLimitMs) {
    // 时间限制检查
    if (timeLimitMs > 0) {
        auto now = std::chrono::steady_clock::now();
        auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(now - startTime).count();
        if (elapsed >= timeLimitMs) {
            return evaluateBoard(game, aiColor);
        }
    }
    return 0;
}
