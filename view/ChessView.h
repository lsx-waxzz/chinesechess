#pragma once
#ifndef QT_NO_DEPRECATED_WARNINGS
#define QT_NO_DEPRECATED_WARNINGS
#endif
#include <QWidget>
#include <QPainter>
#include <QMouseEvent>
#include <functional>
#include <vector>
#include <utility>
#include "../common/Snapshots.h"

class ChessView : public QWidget {
    Q_OBJECT
public:
    explicit ChessView(QWidget* parent = nullptr);

    // 快照提供者（直接使用 std::function）
    void setBoardSnapshotProvider(std::function<const BoardSnapshot& ()> provider);
    // 点击命令（直接使用 std::function）
    void setCellClickCommand(std::function<void(int, int)> command);

    // 高亮数据设置
    void setHighlightData(bool hasSelection, int selRow, int selCol,
        const std::vector<std::pair<int, int>>& validMoves);

protected:
    void paintEvent(QPaintEvent* event) override;
    void mousePressEvent(QMouseEvent* event) override;

private:
    void drawBoard(QPainter& painter);
    void drawPieces(QPainter& painter);
    void drawHighlight(QPainter& painter);

    std::function<const BoardSnapshot& ()> boardProvider_;
    std::function<void(int, int)> cellClickCommand_;

    int cellSize_ = 60;
    int offsetX_ = 40;
    int offsetY_ = 40;

    bool hasHighlight_ = false;
    int selRow_ = -1, selCol_ = -1;
    std::vector<std::pair<int, int>> validMoves_;
};