#pragma once
#include <QWidget>
#include <QPainter>
#include <QMouseEvent>
#include "../common/Commands.h"
#include "../common/Snapshots.h"

class ChessView : public QWidget {
    Q_OBJECT
public:
    explicit ChessView(QWidget* parent = nullptr);

    /// 设置快照提供者（由 App 层绑定到 ViewModel）
    void setBoardSnapshotProvider(BoardSnapshotProvider provider);

    /// 设置命令（由 App 层绑定到 ViewModel）
    void setCellClickCommand(CellClickCommand command);

protected:
    void paintEvent(QPaintEvent* event) override;
    void mousePressEvent(QMouseEvent* event) override;

private:
    void drawBoard(QPainter& painter);
    void drawPieces(QPainter& painter);

    BoardSnapshotProvider boardProvider_;
    CellClickCommand cellClickCommand_;

    int cellSize_ = 60;
    int offsetX_ = 40;
    int offsetY_ = 40;
};
