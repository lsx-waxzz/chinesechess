#include "GamePart.h"

GamePart::GamePart(QObject* parent)
    : QObject(parent)
{}

GamePart::~GamePart() {
    delete mainWindow_;
    delete startWindow_;
}

void GamePart::start() {
    startWindow_ = new StartWindow([this](int mode, int difficulty) {
        showMainWindow(mode, difficulty);
        });
    startWindow_->show();
}

void GamePart::showMainWindow(int mode, int difficulty) {
    if (mainWindow_) {
        unbindMainWindow();
        delete mainWindow_;
        mainWindow_ = nullptr;
    }

    viewModel_.setGameMode(mode, difficulty);
    mainWindow_ = new MainWindow();

    bindMainWindow();

    if (startWindow_) startWindow_->hide();
    mainWindow_->show();
}

void GamePart::bindMainWindow() {
    if (!mainWindow_) return;

    // 属性绑定
    mainWindow_->setBoardSnapshotProvider([this]() -> const BoardSnapshot& {
        return viewModel_.boardSnapshot();
        });
    mainWindow_->setGameSnapshotProvider([this]() -> const GameSnapshot& {
        return viewModel_.gameSnapshot();
        });

    // 命令绑定
    mainWindow_->setCellClickCommand([this](int row, int col) {
        viewModel_.onCellClicked(row, col);
        });
    mainWindow_->setResetCommand([this]() {
        viewModel_.resetGame();
        });
    mainWindow_->setUndoCommand([this]() {
        viewModel_.undoLastMove();
        });
    mainWindow_->setBackCommand([this]() {
        onBackToMenu();
        });

    // 通知绑定
    QObject::connect(&viewModel_, &ChessViewModel::boardChanged,
        mainWindow_, &MainWindow::refreshBoard);
    QObject::connect(&viewModel_, &ChessViewModel::gameStateChanged,
        mainWindow_, &MainWindow::refreshStatus);
    QObject::connect(&viewModel_, &ChessViewModel::undoRejected,
        mainWindow_, &MainWindow::showMessage);
    QObject::connect(&viewModel_, &ChessViewModel::statusMessage,
        mainWindow_, &MainWindow::showMessage);

    // 高亮绑定
    QObject::connect(&viewModel_, &ChessViewModel::highlightChanged,
        mainWindow_, [this]() {
            mainWindow_->updateHighlight(
                viewModel_.hasSelection(),
                viewModel_.selectedRow(),
                viewModel_.selectedCol(),
                viewModel_.validMoves()
            );
        });
}

void GamePart::unbindMainWindow() {
    if (!mainWindow_) return;
    QObject::disconnect(&viewModel_, nullptr, mainWindow_, nullptr);
}

void GamePart::onBackToMenu() {
    if (mainWindow_) {
        unbindMainWindow();
        delete mainWindow_;
        mainWindow_ = nullptr;
    }

    viewModel_.resetGame();

    if (startWindow_) {
        startWindow_->show();
    }
}
