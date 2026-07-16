#include "Switch.h"
#include <QPainter>
#include <QMouseEvent>

Switch::Switch(QWidget* parent)
    : QWidget(parent)
{
    setFixedSize(60, 30);
    radius_ = height() / 2;
    margin_ = 2;
}

void Switch::setChecked(bool checked) {
    if (checked_ != checked) {
        checked_ = checked;
        update();
        emit toggled(checked_);
    }
}

void Switch::paintEvent(QPaintEvent*) {
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    // 背景
    QColor bgColor = checked_ ? QColor(76, 175, 80) : QColor(204, 204, 204);
    painter.setBrush(bgColor);
    painter.setPen(Qt::NoPen);
    painter.drawRoundedRect(rect(), radius_, radius_);

    // 滑块
    int sliderWidth = width() / 2 - margin_ * 2;
    int sliderHeight = height() - margin_ * 2;
    int sliderX = checked_ ? width() - margin_ - sliderWidth : margin_;
    sliderRect_ = QRect(sliderX, margin_, sliderWidth, sliderHeight);
    painter.setBrush(Qt::white);
    painter.drawRoundedRect(sliderRect_, radius_, radius_);

    // ON / OFF 文字
    painter.setPen(checked_ ? Qt::white : Qt::gray);
    QFont font = painter.font();
    font.setPointSize(8);
    painter.setFont(font);
    if (checked_) {
        painter.drawText(QRect(margin_, 0, width() / 2 - margin_, height()), Qt::AlignCenter, "ON");
    }
    else {
        painter.drawText(QRect(width() / 2, 0, width() / 2 - margin_, height()), Qt::AlignCenter, "OFF");
    }
}

void Switch::mousePressEvent(QMouseEvent* event) {
    if (event->button() == Qt::LeftButton) {
        setChecked(!checked_);
    }
    QWidget::mousePressEvent(event);
}

void Switch::resizeEvent(QResizeEvent* event) {
    radius_ = height() / 2;
    margin_ = 2;
}