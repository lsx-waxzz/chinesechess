#include "AIPlayer.h"
#include <algorithm>
#include <limits>
#include <vector>

using TimePoint = std::chrono::steady_clock::time_point;

// ── 棋子基础价值 ──
static int pieceBaseValue(PieceType type) {
    switch (type) {
    case PieceType::King:    return 10000;
    case PieceType::Rook:    return 900;
    case PieceType::Cannon:  return 450;
    case PieceType::Knight:  return 400;
    case PieceType::Elephant: return 200;
    case PieceType::Advisor:  return 200;
    case PieceType::Pawn:    return 100;
    }
    return 0;
}

// ── 兵/卒过河后位置加分 ──
static int pawnPositionBonus(int row, int col, PieceColor color) {
    bool crossed = (color == PieceColor::Red) ? (row <= 4) : (row >= 5);
    if (!crossed) return 0;
    int bonus = 120;
    if (col >= 3 && col <= 5) bonus += 30;
    if (col == 0 || col == 8) bonus -= 20;
    bonus += (color == PieceColor::Red) ? (4 - row) * 15 : (row - 5) * 15;
    return bonus;
}

static int rookPositionBonus(int row, int col) {
    int bonus = 0;
    if (row <= 2 || row >= 7) bonus += 20;
    if (col >= 2 && col <= 6) bonus += 15;
    return bonus;
}

static int knightPositionBonus(int row, int col) {
    int centerDistRow = std::abs(row - 5);
    int centerDistCol = std::abs(col - 4);
    return (5 - centerDistRow) * 8 + (4 - centerDistCol) * 8;
}

static int cannonPositionBonus(int row, int col) {
    return (col >= 3 && col <= 5) ? 20 : 0;
}

static int evaluatePiece(const Piece& piece, int row, int col) {
    int value = pieceBaseValue(piece.type);
    switch (piece.type) {
    case PieceType::Pawn:    value += pawnPositionBonus(row, col, piece.color); break;
    case PieceType::Rook:    value += rookPositionBonus(row, col); break;
    case PieceType::Knight:  value += knightPositionBonus(row, col); break;
    case PieceType::Cannon:  value += cannonPositionBonus(row, col); break;
    default: break;
    }
    return value;
}

// ── 局面评估 ──
int AIPlayer::evaluateBoard(const GameEngine& game, PieceColor aiColor) {
    if (game.isGameOver()) {
        return (game.currentPlayer() == aiColor) ? -99999 : 99999;
    }

    int score = 0;
    const ChessBoard& board = game.board();

    for (int r = 0; r < 10; ++r) {
        for (int c = 0; c < 9; ++c) {
            const auto& p = board.pieceAt(r, c);
            if (!p) continue;
            int v = evaluatePiece(*p, r, c);
            score += (p->color == aiColor) ? v : -v;
        }
    }

    PieceColor opp = (aiColor == PieceColor::Red) ? PieceColor::Black : PieceColor::Red;
    if (game.isInCheck(aiColor)) score -= 50;
    if (game.isInCheck(opp))     score += 50;

    return score;
}

// ── MVV-LVA 走法排序分数（用于吃子排序） ──
static int mvvLvaScore(const Move& m, const ChessBoard& board) {
    const auto& victim = board.pieceAt(m.toRow, m.toCol);
    const auto& attacker = board.pieceAt(m.fromRow, m.fromCol);
    if (!victim.has_value()) return 0;
    // MVV-LVA: 被吃子价值 * 100 - 攻击子价值
    return pieceBaseValue(victim->type) * 100 - pieceBaseValue(attacker->type);
}

// ── 走法排序（吃子 MVV-LVA 优先） ──
static void orderMoves(std::vector<Move>& moves, const ChessBoard& board) {
    std::sort(moves.begin(), moves.end(), [&](const Move& a, const Move& b) {
        return mvvLvaScore(a, board) > mvvLvaScore(b, board);
    });
}

// ── Minimax + Alpha-Beta 剪枝 ──
static int minimax(GameEngine& game, int depth, int alpha, int beta,
                   bool maximizingPlayer, PieceColor aiColor,
                   TimePoint startTime, int timeLimitMs)
{
    if (depth == 0 || game.isGameOver())
        return AIPlayer::evaluateBoard(game, aiColor);

    // 超时直接返回当前估值
    if (timeLimitMs > 0) {
        auto now = std::chrono::steady_clock::now();
        auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(now - startTime).count();
        if (elapsed >= timeLimitMs)
            return AIPlayer::evaluateBoard(game, aiColor);
    }

    PieceColor mover = maximizingPlayer ? aiColor
        : ((aiColor == PieceColor::Red) ? PieceColor::Black : PieceColor::Red);

    auto moves = game.getAllValidMoves(mover);

    // MVV-LVA 排序：提升剪枝效率
    orderMoves(moves, game.board());

    if (maximizingPlayer) {
        int maxEval = std::numeric_limits<int>::min() + 1;
        for (const auto& m : moves) {
            game.makeMove(m);
            int eval = minimax(game, depth - 1, alpha, beta, false, aiColor,
                               startTime, timeLimitMs);
            game.undoLastMove();
            if (eval > maxEval) maxEval = eval;
            if (maxEval > alpha) alpha = maxEval;
            if (beta <= alpha) break;
        }
        return (maxEval == std::numeric_limits<int>::min() + 1)
                   ? AIPlayer::evaluateBoard(game, aiColor) : maxEval;
    } else {
        int minEval = std::numeric_limits<int>::max() - 1;
        for (const auto& m : moves) {
            game.makeMove(m);
            int eval = minimax(game, depth - 1, alpha, beta, true, aiColor,
                               startTime, timeLimitMs);
            game.undoLastMove();
            if (eval < minEval) minEval = eval;
            if (minEval < beta) beta = minEval;
            if (beta <= alpha) break;
        }
        return (minEval == std::numeric_limits<int>::max() - 1)
                   ? AIPlayer::evaluateBoard(game, aiColor) : minEval;
    }
}

// ── 对外接口 ──
Move AIPlayer::getBestMove(const GameEngine& game, AIDifficulty difficulty) {
    int timeLimit;
    int maxDepth;
    switch (difficulty) {
    case AIDifficulty::Easy:   timeLimit = 500;  maxDepth = 2;  break;
    case AIDifficulty::Normal: timeLimit = 2000; maxDepth = 5;  break;
    case AIDifficulty::Hard:   timeLimit = 5000; maxDepth = 10; break;
    default:                   timeLimit = 500;  maxDepth = 2;  break;
    }
    return getBestMoveTimed(game, difficulty, timeLimit);
}

Move AIPlayer::getBestMoveTimed(const GameEngine& game, AIDifficulty difficulty,
                                int timeLimitMs)
{
    PieceColor aiColor = game.currentPlayer();
    auto allMoves = game.getAllValidMoves(aiColor);
    if (allMoves.empty()) return { -1, -1, -1, -1 };

    // 根节点走法排序（MVV-LVA）
    orderMoves(allMoves, game.board());

    auto startTime = std::chrono::steady_clock::now();
    Move bestMove = allMoves[0];

    // ── 迭代加深：从深度 1 开始逐步加深 ──
    int maxDepth;
    switch (difficulty) {
    case AIDifficulty::Easy:   maxDepth = 2;  break;
    case AIDifficulty::Normal: maxDepth = 5;  break;
    case AIDifficulty::Hard:   maxDepth = 10; break;
    default:                   maxDepth = 2;  break;
    }

    for (int depth = 1; depth <= maxDepth; ++depth) {
        int bestScore = std::numeric_limits<int>::min();
        Move currentBest = allMoves[0];
        bool completed = true;  // 本轮是否完整搜完

        for (const auto& m : allMoves) {
            // 每步走法前检查超时
            auto now = std::chrono::steady_clock::now();
            auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(now - startTime).count();
            if (elapsed >= timeLimitMs) {
                completed = false;
                break;
            }

            GameEngine sim = game;
            sim.makeMove(m);
            int score = minimax(sim, depth - 1,
                                std::numeric_limits<int>::min(),
                                std::numeric_limits<int>::max(),
                                false, aiColor, startTime, timeLimitMs);
            // 注意：minimax 内部也会检查超时

            if (score > bestScore) {
                bestScore = score;
                currentBest = m;
            }
        }

        // 超时中断 → 返回上轮结果
        if (!completed) break;
        // 完整搜完 → 更新最优走法，继续加深
        bestMove = currentBest;
    }

    return bestMove;
}
