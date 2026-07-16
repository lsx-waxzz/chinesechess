#include "MainWindow.h"
#include "ChessView.h"
#include "SettingWindow.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QMessageBox>
#include <QApplication>
#include <QDir>
#include <QCoreApplication>
#include <QFile>
#include <QDebug>

MainWindow::MainWindow(QWidget* parent)
    : QWidget(parent)
{
    setupUI();
    initMusic();
}

MainWindow::~MainWindow() {
    // 父对象析构自动释放子对象
}

void MainWindow::initMusic() {
    // ----- 背景音乐 -----
    bgPlayer_ = new QMediaPlayer(this);
    bgAudioOutput_ = new QAudioOutput(this);
    bgPlayer_->setAudioOutput(bgAudioOutput_);

    QString exeDir = QCoreApplication::applicationDirPath();
    QString musicPath = exeDir + "/music.mp3";
    if (!QFile::exists(musicPath)) {
        musicPath = "D:/chessfinal/chessfinal/music.mp3";
    }
    if (!QFile::exists(musicPath)) {
        musicPath = QDir::currentPath() + "/music.mp3";
    }

    if (QFile::exists(musicPath)) {
        bgPlayer_->setSource(QUrl::fromLocalFile(musicPath));
        bgPlayer_->setLoops(QMediaPlayer::Infinite);
    }
    else {
        qDebug() << "背景音乐文件未找到：" << musicPath;
    }

    if (bgMusicEnabled_ && bgPlayer_->source() != QUrl()) {
        bgPlayer_->play();
    }

    // ----- 走子音效 -----
    movePlayer_ = new QMediaPlayer(this);
    moveAudioOutput_ = new QAudioOutput(this);
    movePlayer_->setAudioOutput(moveAudioOutput_);

    QString movePath = exeDir + "/move.m4a";
    if (!QFile::exists(movePath)) {
        movePath = "D:/chessfinal/chessfinal/move.m4a";
    }
    if (!QFile::exists(movePath)) {
        movePath = QDir::currentPath() + "/move.m4a";
    }

    if (QFile::exists(movePath)) {
        movePlayer_->setSource(QUrl::fromLocalFile(movePath));
        qDebug() << "走子音效已加载：" << movePath;
    }
    else {
        qDebug() << "走子音效文件未找到：" << movePath;
    }
}

void MainWindow::playMoveSound() {
    if (sfxEnabled_ && movePlayer_ && movePlayer_->source() != QUrl()) {
        movePlayer_->play();
    }
}

void MainWindow::setupUI() {
    setWindowTitle(QStringLiteral("中国象棋"));
    auto* layout = new QVBoxLayout(this);

    QHBoxLayout* topLayout = new QHBoxLayout();
    statusLabel_ = new QLabel(QStringLiteral("当前走棋：红方"));
    statusLabel_->setAlignment(Qt::AlignCenter);
    QFont font = statusLabel_->font();
    font.setPointSize(18);
    font.setBold(true);
    statusLabel_->setFont(font);

    settingBtn_ = new QPushButton(QStringLiteral("设置"));
    settingBtn_->setFixedSize(80, 35);
    connect(settingBtn_, &QPushButton::clicked, this, &MainWindow::onSettingClicked);

    topLayout->addWidget(statusLabel_, 1);
    topLayout->addWidget(settingBtn_, 0);
    layout->addLayout(topLayout);

    chessView_ = new ChessView();
    layout->addWidget(chessView_);

    resetBtn_ = new QPushButton(QStringLiteral("重新开始"));
    undoBtn_ = new QPushButton(QStringLiteral("悔棋"));
    backBtn_ = new QPushButton(QStringLiteral("返回菜单"));

    auto* buttonRow = new QHBoxLayout();
    buttonRow->addWidget(undoBtn_);
    buttonRow->addWidget(resetBtn_);
    buttonRow->addWidget(backBtn_);

    layout->addLayout(buttonRow);

    connect(resetBtn_, &QPushButton::clicked, [this]() {
        if (resetCommand_) resetCommand_();
        });
    connect(undoBtn_, &QPushButton::clicked, [this]() {
        if (undoCommand_) undoCommand_();
        });
    connect(backBtn_, &QPushButton::clicked, [this]() {
        if (backCommand_) backCommand_();
        });

    settingWindow_ = nullptr;
}

void MainWindow::onSettingClicked() {
    if (!settingWindow_) {
        settingWindow_ = new SettingWindow(this);
        connect(settingWindow_, &SettingWindow::bgMusicToggled,
            this, &MainWindow::onBgMusicToggled);
        connect(settingWindow_, &SettingWindow::sfxToggled,
            this, &MainWindow::onSfxToggled);
        settingWindow_->setAttribute(Qt::WA_DeleteOnClose);
        connect(settingWindow_, &SettingWindow::destroyed, this, [this]() {
            settingWindow_ = nullptr;
            });
    }
    settingWindow_->setBgMusicEnabled(bgMusicEnabled_);
    settingWindow_->setSfxEnabled(sfxEnabled_);
    settingWindow_->show();
    settingWindow_->raise();
    settingWindow_->activateWindow();
}

void MainWindow::onBgMusicToggled(bool enabled) {
    bgMusicEnabled_ = enabled;
    if (bgPlayer_) {
        if (enabled) bgPlayer_->play();
        else bgPlayer_->pause();
    }
}

void MainWindow::onSfxToggled(bool enabled) {
    sfxEnabled_ = enabled;
}

// ---- setter 实现（使用 std::function） ----
void MainWindow::setBoardSnapshotProvider(std::function<const BoardSnapshot& ()> provider) {
    boardProvider_ = std::move(provider);
    chessView_->setBoardSnapshotProvider(boardProvider_);
}

void MainWindow::setGameSnapshotProvider(std::function<const GameSnapshot& ()> provider) {
    gameProvider_ = std::move(provider);
}

void MainWindow::setCellClickCommand(std::function<void(int, int)> command) {
    cellClickCommand_ = std::move(command);
    chessView_->setCellClickCommand(cellClickCommand_);
}

void MainWindow::setResetCommand(std::function<void()> command) {
    resetCommand_ = std::move(command);
}

void MainWindow::setUndoCommand(std::function<void()> command) {
    undoCommand_ = std::move(command);
}

void MainWindow::setBackCommand(std::function<void()> command) {
    backCommand_ = std::move(command);
}

void MainWindow::showMessage(const QString& msg) {
    QMessageBox::information(this, QStringLiteral("提示"), msg);
}

void MainWindow::refreshBoard() {
    chessView_->update();
}

void MainWindow::refreshStatus() {
    if (!gameProvider_) return;
    const GameSnapshot& state = gameProvider_();
    if (state.gameOver) {
        statusLabel_->setText(QStringLiteral("游戏结束！"));
    }
    else {
        statusLabel_->setText(
            QStringLiteral("当前走棋：") +
            QString::fromStdString(state.currentPlayer));
    }
}

void MainWindow::updateHighlight(bool hasSelection, int row, int col,
    const std::vector<std::pair<int, int>>& moves) {
    chessView_->setHighlightData(hasSelection, row, col, moves);
}