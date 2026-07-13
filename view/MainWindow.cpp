#include "MainWindow.h"
#include "ChessView.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QMessageBox>
#include <QApplication>

MainWindow::MainWindow(QWidget* parent)
    : QWidget(parent)
{
    setupUI();
}

void MainWindow::setupUI() {
    setWindowTitle(QStringLiteral("中国象棋"));
    auto* layout = new QVBoxLayout(this);

    statusLabel_ = new QLabel(QStringLiteral("当前走棋：红方"));
    statusLabel_->setAlignment(Qt::AlignCenter);
    QFont font = statusLabel_->font();
    font.setPointSize(18);
    font.setBold(true);
    statusLabel_->setFont(font);

    chessView_ = new ChessView();

    resetBtn_ = new QPushButton(QStringLiteral("重新开始"));
    undoBtn_  = new QPushButton(QStringLiteral("悔棋"));
    backBtn_  = new QPushButton(QStringLiteral("返回菜单"));

    auto* buttonRow = new QHBoxLayout();
    buttonRow->addWidget(undoBtn_);
    buttonRow->addWidget(resetBtn_);
    buttonRow->addWidget(backBtn_);

    layout->addWidget(statusLabel_);
    layout->addWidget(chessView_);
    layout->addLayout(buttonRow);

    // 按钮点击 → 调用注入的命令（纯转发，不做逻辑判断）
    QObject::connect(resetBtn_, &QPushButton::clicked, [this]() {
        if (resetCommand_) resetCommand_();
    });
    QObject::connect(undoBtn_, &QPushButton::clicked, [this]() {
        if (undoCommand_) undoCommand_();
    });
    QObject::connect(backBtn_, &QPushButton::clicked, [this]() {
        if (backCommand_) backCommand_();
    });
}

// ===== 属性绑定 =====

void MainWindow::setBoardSnapshotProvider(BoardSnapshotProvider provider) {
    boardProvider_ = std::move(provider);
    chessView_->setBoardSnapshotProvider(boardProvider_);
}

void MainWindow::setGameSnapshotProvider(GameSnapshotProvider provider) {
    gameProvider_ = std::move(provider);
}

// ===== 命令绑定 =====

void MainWindow::setCellClickCommand(CellClickCommand command) {
    cellClickCommand_ = std::move(command);
    chessView_->setCellClickCommand(cellClickCommand_);
}

void MainWindow::setResetCommand(SimpleCommand command) {
    resetCommand_ = std::move(command);
}

void MainWindow::setUndoCommand(SimpleCommand command) {
    undoCommand_ = std::move(command);
}

void MainWindow::setBackCommand(SimpleCommand command) {
    backCommand_ = std::move(command);
}

void MainWindow::showMessage(const QString& msg) {
    QMessageBox::information(this, QStringLiteral("提示"), msg);
}

// ===== 刷新槽 =====

void MainWindow::refreshBoard() {
    chessView_->update();
}

void MainWindow::refreshStatus() {
    if (!gameProvider_) return;
    const GameSnapshot& state = gameProvider_();
    if (state.gameOver) {
        statusLabel_->setText(QStringLiteral("游戏结束！"));
    } else {
        statusLabel_->setText(
            QStringLiteral("当前走棋：") +
            QString::fromStdString(state.currentPlayer));
    }
}
