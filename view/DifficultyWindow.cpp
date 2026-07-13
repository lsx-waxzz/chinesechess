#include "DifficultyWindow.h"
#include <QFont>

DifficultyWindow::DifficultyWindow(QWidget* parent) : QWidget(parent) {
    setWindowTitle(QStringLiteral("选择难度"));
    setFixedSize(300, 220);
    auto* layout = new QVBoxLayout(this);
    layout->setAlignment(Qt::AlignCenter);

    QPushButton* btnEasy = new QPushButton(QStringLiteral("菜鸟"));
    btnEasy->setFont(QFont("楷体", 14));
    QPushButton* btnNormal = new QPushButton(QStringLiteral("普通"));
    btnNormal->setFont(QFont("楷体", 14));
    QPushButton* btnHard = new QPushButton(QStringLiteral("大师"));
    btnHard->setFont(QFont("楷体", 14));

    layout->addWidget(btnEasy);
    layout->addSpacing(10);
    layout->addWidget(btnNormal);
    layout->addSpacing(10);
    layout->addWidget(btnHard);

    QObject::connect(btnEasy, &QPushButton::clicked, [this]() {
        emit difficultyChosen(0);
        close();
    });
    QObject::connect(btnNormal, &QPushButton::clicked, [this]() {
        emit difficultyChosen(1);
        close();
    });
    QObject::connect(btnHard, &QPushButton::clicked, [this]() {
        emit difficultyChosen(2);
        close();
    });
}
