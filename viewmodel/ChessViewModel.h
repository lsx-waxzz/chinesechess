#pragma once
#ifndef QT_NO_DEPRECATED_WARNINGS
#define QT_NO_DEPRECATED_WARNINGS
#endif
#include <QObject>
#include "../common/Types.h"
#include "../common/Snapshots.h"
#include "../model/GameEngine.h"
#include "../model/AIPlayer.h"

class ChessViewModel : public QObject {
    Q_OBJECT
public:
    enum class GameMode { TwoPlayer, PlayerVsAI };

    explicit ChessViewModel(QObject* parent = nullptr);

    void onCellClicked(int row, int col);
    void resetGame();
    void setGameMode(int mode, int difficulty);
    void undoLastMove();

    const BoardSnapshot& boardSnapshot() const { return boardSnapshot_; }
    const GameSnapshot& gameSnapshot() const { return gameSnapshot_; }

    bool hasSelection() const { return hasSelection_; }
    int selectedRow() const { return selectedRow_; }
    int selectedCol() const { return selectedCol_; }
    const std::vector<std::pair<int, int>>& validMoves() const { return validMoves_; }

signals:
    void boardChanged();
    void gameStateChanged();
    void statusMessage(const QString& msg);
    void undoRejected(const QString& msg);
    void highlightChanged();
    void moveMade();   // 新增：走子成功时发射，用于触发音效

private:
    void rebuildSnapshots();
    void notifyIfChanged();
    static QString pieceToChinese(const std::optional<Piece>& p);
    void clearHighlight();

    GameEngine engine_;
    GameMode gameMode_ = GameMode::TwoPlayer;
    AIDifficulty aiDifficulty_ = AIDifficulty::Easy;

    BoardSnapshot boardSnapshot_;
    GameSnapshot gameSnapshot_;
    BoardSnapshot prevBoardSnapshot_;
    GameSnapshot prevGameSnapshot_;

    bool hasSelection_ = false;
    int selectedRow_ = -1, selectedCol_ = -1;
    std::vector<std::pair<int, int>> validMoves_;

    bool undoUsed_ = false;
};
