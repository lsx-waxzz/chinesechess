#pragma once
#ifndef QT_NO_DEPRECATED_WARNINGS
#define QT_NO_DEPRECATED_WARNINGS
#endif
#include <QWidget>
#include <QPainter>
#include <QMouseEvent>
#include <functional>
#include "../common/Snapshots.h"

class ChessView : public QWidget {
    Q_OBJECT
public:
    explicit ChessView(QWidget* parent = nullptr);

    // 使用 std::function 直接表达回调类型
    void setBoardSnapshotProvider(std::function<const BoardSnapshot& ()> provider);
    void setCellClickCommand(std::function<void(int, int)> command);

protected:
    void paintEvent(QPaintEvent* event) override;
    void mousePressEvent(QMouseEvent* event) override;

private:
    void drawBoard(QPainter& painter);
    void drawPieces(QPainter& painter);

    std::function<const BoardSnapshot& ()> boardProvider_;
    std::function<void(int, int)> cellClickCommand_;

    int cellSize_ = 60;
    int offsetX_ = 40;
    int offsetY_ = 40;
};
