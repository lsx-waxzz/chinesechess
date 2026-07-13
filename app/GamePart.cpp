#include "GamePart.h"

GamePart::GamePart(QObject* parent)
    : QObject(parent)
{
}

GamePart::~GamePart() {
    // 确保 View 先析构，因为它持有 ViewModel 数据的只读引用
    delete mainWindow_;
    delete startWindow_;
}

void GamePart::start() {
    // 创建开始窗口
    startWindow_ = new StartWindow([this](int mode, int difficulty) {
        showMainWindow(mode, difficulty);
    });
    startWindow_->show();
}

void GamePart::showMainWindow(int mode, int difficulty) {
    // 销毁旧的主窗口（如有）
    if (mainWindow_) {
        unbindMainWindow();
        delete mainWindow_;
        mainWindow_ = nullptr;
    }

    // 设置游戏模式（这也会重置 Model）
    viewModel_.setGameMode(mode, difficulty);

    // 创建新窗口
    mainWindow_ = new MainWindow();

    // ===== 集中完成三种绑定 =====
    bindMainWindow();

    // 切换显示
    if (startWindow_) startWindow_->hide();
    mainWindow_->show();
}

void GamePart::bindMainWindow() {
    if (!mainWindow_) return;

    // 1) 属性绑定：View 的 provider → ViewModel 的快照
    mainWindow_->setBoardSnapshotProvider([this]() -> const BoardSnapshot& {
        return viewModel_.boardSnapshot();
    });
    mainWindow_->setGameSnapshotProvider([this]() -> const GameSnapshot& {
        return viewModel_.gameSnapshot();
    });

    // 2) 命令绑定：View 的命令 → ViewModel 的方法
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

    // 3) 通知绑定：ViewModel 信号 → View 刷新槽
    QObject::connect(&viewModel_, &ChessViewModel::boardChanged,
                     mainWindow_, &MainWindow::refreshBoard);
    QObject::connect(&viewModel_, &ChessViewModel::gameStateChanged,
                     mainWindow_, &MainWindow::refreshStatus);
    QObject::connect(&viewModel_, &ChessViewModel::undoRejected,
                     mainWindow_, &MainWindow::showMessage);
    QObject::connect(&viewModel_, &ChessViewModel::statusMessage,
                     mainWindow_, &MainWindow::showMessage);
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
