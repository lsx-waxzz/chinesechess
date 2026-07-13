#pragma once
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

    // ===== 命令（View 调用） =====
    void onCellClicked(int row, int col);
    void resetGame();
    void setGameMode(int mode, int difficulty);
    void undoLastMove();

    // ===== 只读属性（View 读取） =====
    const BoardSnapshot& boardSnapshot() const { return boardSnapshot_; }
    const GameSnapshot& gameSnapshot() const { return gameSnapshot_; }

signals:
    // ===== 通知（ViewModel → View） =====
    void boardChanged();
    void gameStateChanged();
    void statusMessage(const QString& msg);
    void undoRejected(const QString& msg);

private:
    void rebuildSnapshots();
    void notifyIfChanged();

    // 将 Model 的 Piece 转换为显示文字
    static QString pieceToChinese(const std::optional<Piece>& p);

    // Model
    GameEngine engine_;
    GameMode gameMode_ = GameMode::TwoPlayer;
    AIDifficulty aiDifficulty_ = AIDifficulty::Easy;

    // 缓存快照（用于比较变化）
    BoardSnapshot boardSnapshot_;
    GameSnapshot gameSnapshot_;
    BoardSnapshot prevBoardSnapshot_;
    GameSnapshot prevGameSnapshot_;
};
