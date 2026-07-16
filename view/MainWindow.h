#pragma once
#ifndef QT_NO_DEPRECATED_WARNINGS
#define QT_NO_DEPRECATED_WARNINGS
#endif

#include <QWidget>
#include <QLabel>
#include <QPushButton>
#include <QMediaPlayer>
#include <QAudioOutput>
#include <functional>
#include <vector>
#include <utility>
#include <QUrl>

#include "../common/Snapshots.h"

class ChessView;
class SettingWindow;

class MainWindow : public QWidget {
    Q_OBJECT
public:
    explicit MainWindow(QWidget* parent = nullptr);
    ~MainWindow();

    // 属性绑定：快照提供者（直接使用 std::function）
    void setBoardSnapshotProvider(std::function<const BoardSnapshot& ()> provider);
    void setGameSnapshotProvider(std::function<const GameSnapshot& ()> provider);

    // 命令绑定（直接使用 std::function）
    void setCellClickCommand(std::function<void(int, int)> command);
    void setResetCommand(std::function<void()> command);
    void setUndoCommand(std::function<void()> command);
    void setBackCommand(std::function<void()> command);

public slots:
    void refreshBoard();
    void refreshStatus();
    void showMessage(const QString& msg);
    void updateHighlight(bool hasSelection, int row, int col,
        const std::vector<std::pair<int, int>>& moves);
    void playMoveSound();

private slots:
    void onSettingClicked();
    void onBgMusicToggled(bool enabled);
    void onSfxToggled(bool enabled);

private:
    void setupUI();
    void initMusic();

    ChessView* chessView_ = nullptr;
    QLabel* statusLabel_ = nullptr;
    QPushButton* resetBtn_ = nullptr;
    QPushButton* undoBtn_ = nullptr;
    QPushButton* backBtn_ = nullptr;
    QPushButton* settingBtn_ = nullptr;
    SettingWindow* settingWindow_ = nullptr;

    // 快照提供者（用具体 std::function 类型）
    std::function<const BoardSnapshot& ()> boardProvider_;
    std::function<const GameSnapshot& ()> gameProvider_;

    // 命令（用具体 std::function 类型）
    std::function<void(int, int)> cellClickCommand_;
    std::function<void()> resetCommand_;
    std::function<void()> undoCommand_;
    std::function<void()> backCommand_;

    bool bgMusicEnabled_ = true;
    bool sfxEnabled_ = true;

    QMediaPlayer* bgPlayer_ = nullptr;
    QAudioOutput* bgAudioOutput_ = nullptr;
    QMediaPlayer* movePlayer_ = nullptr;
    QAudioOutput* moveAudioOutput_ = nullptr;
};