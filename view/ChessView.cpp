#include "ChessView.h"
#include <QFont>

ChessView::ChessView(QWidget* parent)
    : QWidget(parent)
{
    setFixedSize(offsetX_ * 2 + cellSize_ * 8, offsetY_ * 2 + cellSize_ * 9);
    setMouseTracking(false);
}

void ChessView::setBoardSnapshotProvider(BoardSnapshotProvider provider) {
    boardProvider_ = std::move(provider);
}

void ChessView::setCellClickCommand(CellClickCommand command) {
    cellClickCommand_ = std::move(command);
}

void ChessView::paintEvent(QPaintEvent*) {
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    drawBoard(painter);
    drawPieces(painter);
}

void ChessView::drawBoard(QPainter& painter) {
    painter.fillRect(rect(), QColor("#EDC98D"));
    QPen pen(Qt::black, 2);
    painter.setPen(pen);

    // 横线
    for (int r = 0; r <= 9; ++r)
        painter.drawLine(offsetX_, offsetY_ + r * cellSize_,
                         offsetX_ + 8 * cellSize_, offsetY_ + r * cellSize_);
    // 竖线
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

    // 楚河汉界
    QFont font("楷体", 20, QFont::Bold);
    painter.setFont(font);
    painter.setPen(Qt::black);
    painter.drawText(offsetX_ + 80, offsetY_ + 4 * cellSize_ + 45,
                     QStringLiteral("楚  河          汉  界"));

    // 九宫格斜线
    painter.drawLine(offsetX_ + 3 * cellSize_, offsetY_,
                     offsetX_ + 5 * cellSize_, offsetY_ + 2 * cellSize_);
    painter.drawLine(offsetX_ + 5 * cellSize_, offsetY_,
                     offsetX_ + 3 * cellSize_, offsetY_ + 2 * cellSize_);
    painter.drawLine(offsetX_ + 3 * cellSize_, offsetY_ + 7 * cellSize_,
                     offsetX_ + 5 * cellSize_, offsetY_ + 9 * cellSize_);
    painter.drawLine(offsetX_ + 5 * cellSize_, offsetY_ + 7 * cellSize_,
                     offsetX_ + 3 * cellSize_, offsetY_ + 9 * cellSize_);
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
