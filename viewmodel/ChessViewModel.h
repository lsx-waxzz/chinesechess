#pragma once
#ifndef QT_NO_DEPRECATED_WARNINGS
#define QT_NO_DEPRECATED_WARNINGS
#endif
#include <QObject>
#include "../common/Types.h"
#include "../common/Snapshots.h"
#include "../model/GameEngine.h"
#include "../model/AIPlayer.h"

class UciEngine;

class ChessViewModel : public QObject {
    Q_OBJECT
public:
    enum class GameMode { TwoPlayer, PlayerVsAI };

    explicit ChessViewModel(QObject* parent = nullptr);
    ~ChessViewModel();

    void onCellClicked(int row, int col);
    void resetGame();
    void setGameMode(int mode, int difficulty);
    void undoLastMove();

public slots:
    void executeAIMove();

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
    void moveMade();

private slots:
    void onEngineStarted();
    void onEngineBestMove(const QString& uciMove);
    void onEngineError(const QString& msg);

private:
    void rebuildSnapshots();
    void notifyIfChanged();
    static QString pieceToChinese(const std::optional<Piece>& p);
    void clearHighlight();
    void tryStartEngine();
    void applyAIMove(const Move& move);

    GameEngine engine_;
    GameMode gameMode_ = GameMode::TwoPlayer;
    int aiDifficulty_ = 0;  // 0=Easy, 1=Normal, 2=Hard

    // 双引擎：皮卡鱼优先，内置 AI 回退
    UciEngine* uciEngine_ = nullptr;
    bool useExternalEngine_ = false;

    BoardSnapshot boardSnapshot_;
    GameSnapshot gameSnapshot_;
    BoardSnapshot prevBoardSnapshot_;
    GameSnapshot prevGameSnapshot_;

    bool hasSelection_ = false;
    int selectedRow_ = -1, selectedCol_ = -1;
    std::vector<std::pair<int, int>> validMoves_;

    bool undoUsed_ = false;
    bool aiComputing_ = false;
};
