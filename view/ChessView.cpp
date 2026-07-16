#include "ChessView.h"
#include <QFont>

ChessView::ChessView(QWidget* parent)
    : QWidget(parent)
{
    setFixedSize(offsetX_ * 2 + cellSize_ * 8, offsetY_ * 2 + cellSize_ * 9);
    setMouseTracking(false);
}

void ChessView::setBoardSnapshotProvider(std::function<const BoardSnapshot& ()> provider) {
    boardProvider_ = std::move(provider);
}

void ChessView::setCellClickCommand(std::function<void(int, int)> command) {
    cellClickCommand_ = std::move(command);
}

void ChessView::setHighlightData(bool hasSelection, int selRow, int selCol,
    const std::vector<std::pair<int, int>>& validMoves) {
    hasHighlight_ = hasSelection;
    selRow_ = selRow;
    selCol_ = selCol;
    validMoves_ = validMoves;
    update();
}

void ChessView::paintEvent(QPaintEvent*) {
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    drawBoard(painter);
    drawHighlight(painter);
    drawPieces(painter);
}

void ChessView::drawBoard(QPainter& painter) {
    painter.fillRect(rect(), QColor("#EDC98D"));
    QPen pen(Qt::black, 2);
    painter.setPen(pen);

    for (int r = 0; r <= 9; ++r)
        painter.drawLine(offsetX_, offsetY_ + r * cellSize_,
            offsetX_ + 8 * cellSize_, offsetY_ + r * cellSize_);

    for (int c = 0; c <= 8; ++c) {
        if (c == 0 || c == 8)
            painter.drawLine(offsetX_ + c * cellSize_, offsetY_,
                offsetX_ + c * cellSize_, offsetY_ + 9 * cellSize_);
        else {
            painter.drawLine(offsetX_ + c * cellSize_, offsetY_,
                offsetX_ + c * cellSize_, offsetY_ + 4 * cellSize_);
            painter.drawLine(offsetX_ + c * cellSize_, offsetY_ + 5 * cellSize_,
                offsetX_ + c * cellSize_, offsetY_ + 9 * cellSize_);
        }
    }

    QFont font("楷体", 20, QFont::Bold);
    painter.setFont(font);
    painter.setPen(Qt::black);
    painter.drawText(offsetX_ + 80, offsetY_ + 4 * cellSize_ + 45,
        QStringLiteral("楚  河          汉  界"));

    painter.drawLine(offsetX_ + 3 * cellSize_, offsetY_,
        offsetX_ + 5 * cellSize_, offsetY_ + 2 * cellSize_);
    painter.drawLine(offsetX_ + 5 * cellSize_, offsetY_,
        offsetX_ + 3 * cellSize_, offsetY_ + 2 * cellSize_);
    painter.drawLine(offsetX_ + 3 * cellSize_, offsetY_ + 7 * cellSize_,
        offsetX_ + 5 * cellSize_, offsetY_ + 9 * cellSize_);
    painter.drawLine(offsetX_ + 5 * cellSize_, offsetY_ + 7 * cellSize_,
        offsetX_ + 3 * cellSize_, offsetY_ + 9 * cellSize_);
}

void ChessView::drawHighlight(QPainter& painter) {
    if (!hasHighlight_) return;

    // 绘制选中棋子高亮（半透明矩形或圆框）
    int x = offsetX_ + selCol_ * cellSize_;
    int y = offsetY_ + selRow_ * cellSize_;
    painter.setPen(QPen(Qt::yellow, 4));
    painter.setBrush(Qt::NoBrush);
    painter.drawEllipse(x - cellSize_ / 2 + 4, y - cellSize_ / 2 + 4,
        cellSize_ - 8, cellSize_ - 8);

    // 绘制合法走法标记（小圆点）
    painter.setBrush(QBrush(QColor(0, 255, 0, 150)));
    painter.setPen(Qt::NoPen);
    for (auto [tr, tc] : validMoves_) {
        int tx = offsetX_ + tc * cellSize_;
        int ty = offsetY_ + tr * cellSize_;
        // 判断目标是否有棋子，有棋子则画圆圈框，否则画小圆点
        if (boardProvider_) {
            const BoardSnapshot& snap = boardProvider_();
            const auto& cell = snap.cells[tr][tc];
            if (cell.has_value() && !cell->text.empty()) {
                // 可吃子位置：画红色圆圈
                painter.setBrush(Qt::NoBrush);
                painter.setPen(QPen(Qt::red, 3));
                painter.drawEllipse(tx - cellSize_ / 2 + 4, ty - cellSize_ / 2 + 4,
                    cellSize_ - 8, cellSize_ - 8);
                painter.setPen(Qt::NoPen);
                painter.setBrush(QBrush(QColor(0, 255, 0, 150)));
            }
            else {
                // 空位：绿色小圆点
                painter.drawEllipse(tx - 8, ty - 8, 16, 16);
            }
        }
    }
}

void ChessView::drawPieces(QPainter& painter) {
    if (!boardProvider_) return;

    const BoardSnapshot& snapshot = boardProvider_();

    QFont font("楷体", 24, QFont::Bold);
    painter.setFont(font);

    for (int r = 0; r < 10; ++r) {
        for (int c = 0; c < 9; ++c) {
            const auto& cell = snapshot.cells[r][c];
            if (!cell.has_value() || cell->text.empty()) continue;

            int x = offsetX_ + c * cellSize_;
            int y = offsetY_ + r * cellSize_;
            QRect rect(x - cellSize_ / 2, y - cellSize_ / 2, cellSize_, cellSize_);

            bool isRed = cell->isRed;
            painter.setPen(Qt::black);
            painter.setBrush(isRed ? QColor("#FF9999") : QColor("#CCCCCC"));
            painter.drawEllipse(rect.adjusted(4, 4, -4, -4));

            painter.setPen(isRed ? Qt::red : Qt::black);
            painter.drawText(rect, Qt::AlignCenter,
                QString::fromStdString(cell->text));
        }
    }
}

void ChessView::mousePressEvent(QMouseEvent* event) {
    int col = static_cast<int>(event->position().x() - offsetX_ + cellSize_ / 2) / cellSize_;
    int row = static_cast<int>(event->position().y() - offsetY_ + cellSize_ / 2) / cellSize_;

    if (col >= 0 && col < 9 && row >= 0 && row < 10) {
        if (cellClickCommand_) {
            cellClickCommand_(row, col);
        }
    }
}