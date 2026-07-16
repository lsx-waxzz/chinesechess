#pragma once
#ifndef QT_NO_DEPRECATED_WARNINGS
#define QT_NO_DEPRECATED_WARNINGS
#endif
#include <QWidget>
#include <QPushButton>
#include <QVBoxLayout>

class DifficultyWindow : public QWidget {
    Q_OBJECT
public:
    explicit DifficultyWindow(QWidget* parent = nullptr);

signals:
    void difficultyChosen(int difficulty);
};
