#pragma once
#include <vector>
#include <optional>
#include <string>

/// 单个棋子的显示快照（View 可直接用于绘制）
struct PieceSnapshot {
    std::string text;   // 棋子中文文字，空串表示无棋子
    bool isRed = false; // 是否红方
};

/// 棋盘快照：10行 × 9列
struct BoardSnapshot {
    std::vector<std::vector<std::optional<PieceSnapshot>>> cells; // 10x9

    BoardSnapshot()
        : cells(10, std::vector<std::optional<PieceSnapshot>>(9)) {}
};

/// 游戏状态快照（HUD / 状态栏使用）
struct GameSnapshot {
    std::string currentPlayer;  // "红方" 或 "黑方"
    bool gameOver = false;
    std::string statusMessage;
};
