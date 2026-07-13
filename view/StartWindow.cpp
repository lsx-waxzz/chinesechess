#include "StartWindow.h"
#include "DifficultyWindow.h"
#include <QFont>
#include <QVBoxLayout>
#include <QLabel>
#include <QPalette>

StartWindow::StartWindow(std::function<void(int mode, int difficulty)> onModeSelected,
                         QWidget* parent)
    : QWidget(parent), onModeSelected_(std::move(onModeSelected))
{
    setWindowTitle(QStringLiteral("中国象棋"));
    setFixedSize(320, 340);

    QPalette pal = palette();
    pal.setColor(QPalette::Window, Qt::white);
    setPalette(pal);
    setAutoFillBackground(true);

    auto* layout = new QVBoxLayout(this);
    layout->setSpacing(40);
    layout->setContentsMargins(20, 20, 20, 20);

    QLabel* title = new QLabel(QStringLiteral("中国象棋"));
    title->setFont(QFont("楷体", 24, QFont::Bold));
    title->setAlignment(Qt::AlignCenter);
    title->setStyleSheet("background-color: transparent; color: black;");
    layout->addWidget(title);

    QPushButton* btnTwoPlayer = new QPushButton(QStringLiteral("双人模式"));
    btnTwoPlayer->setFont(QFont("楷体", 14));
    btnTwoPlayer->setFixedWidth(120);

    QPushButton* btnVsAI = new QPushButton(QStringLiteral("人机模式"));
    btnVsAI->setFont(QFont("楷体", 14));
    btnVsAI->setFixedWidth(120);

    layout->addWidget(btnTwoPlayer, 0, Qt::AlignHCenter);
    layout->addWidget(btnVsAI, 0, Qt::AlignHCenter);
    layout->addStretch();

    QObject::connect(btnTwoPlayer, &QPushButton::clicked,
                     this, &StartWindow::onTwoPlayerClicked);
    QObject::connect(btnVsAI, &QPushButton::clicked,
                     this, &StartWindow::onVsAIClicked);
}

void StartWindow::onTwoPlayerClicked() {
    if (onModeSelected_) {
        onModeSelected_(0, 0);  // 双人模式，难度忽略
    }
}

void StartWindow::onVsAIClicked() {
    auto* diffWin = new DifficultyWindow();
    diffWin->setAttribute(Qt::WA_DeleteOnClose);
    QObject::connect(diffWin, &DifficultyWindow::difficultyChosen,
                     [this](int diff) {
        if (onModeSelected_) {
            onModeSelected_(1, diff);  // 人机模式 + 难度
        }
    });
    diffWin->show();
}
