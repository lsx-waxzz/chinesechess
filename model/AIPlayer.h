#pragma once
#include "GameEngine.h"
#include "../common/Types.h"
#include <chrono>

enum class AIDifficulty { Easy, Normal, Hard };

class AIPlayer {
public:
    using TimePoint = std::chrono::steady_clock::time_point;

    static Move getBestMove(const GameEngine& game, AIDifficulty difficulty);

    /// 带时间限制的搜索
    /// @param timeLimitMs 时间限制（毫秒），0 表示无限制
    static Move getBestMoveTimed(const GameEngine& game, AIDifficulty difficulty, int timeLimitMs);

    static int evaluateBoard(const GameEngine& game, PieceColor aiColor);

private:
    static int minimax(GameEngine& game, int depth, int alpha, int beta,
        bool maximizingPlayer, PieceColor aiColor,
        TimePoint startTime, int timeLimitMs);
};
