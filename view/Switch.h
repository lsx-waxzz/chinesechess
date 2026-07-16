#pragma once
#ifndef QT_NO_DEPRECATED_WARNINGS
#define QT_NO_DEPRECATED_WARNINGS
#endif
#include <QWidget>

class Switch : public QWidget {
    Q_OBJECT
public:
    explicit Switch(QWidget* parent = nullptr);

    bool isChecked() const { return checked_; }
    void setChecked(bool checked);

signals:
    void toggled(bool checked);

protected:
    void paintEvent(QPaintEvent* event) override;
    void mousePressEvent(QMouseEvent* event) override;
    void resizeEvent(QResizeEvent* event) override;

private:
    bool checked_ = false;
    int radius_;
    int margin_;
    QRect sliderRect_;
};