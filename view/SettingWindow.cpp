#include "SettingWindow.h"
#include <QPushButton>
#include <QHBoxLayout>

SettingWindow::SettingWindow(QWidget* parent)
    : QDialog(parent)
{
    setWindowTitle(QStringLiteral("设置"));
    setFixedSize(300, 160);
    setModal(false);

    auto* layout = new QVBoxLayout(this);
    layout->setAlignment(Qt::AlignCenter);

    // 背景音乐
    QHBoxLayout* row1 = new QHBoxLayout();
    QLabel* label1 = new QLabel(QStringLiteral("背景音乐"));
    bgMusicSwitch_ = new Switch();
    bgMusicSwitch_->setChecked(true);
    connect(bgMusicSwitch_, &Switch::toggled, this, &SettingWindow::onBgMusicToggled);
    row1->addWidget(label1);
    row1->addWidget(bgMusicSwitch_);
    layout->addLayout(row1);

    // 走子音效
    QHBoxLayout* row2 = new QHBoxLayout();
    QLabel* label2 = new QLabel(QStringLiteral("走子音效"));
    sfxSwitch_ = new Switch();
    sfxSwitch_->setChecked(true);
    connect(sfxSwitch_, &Switch::toggled, this, &SettingWindow::onSfxToggled);
    row2->addWidget(label2);
    row2->addWidget(sfxSwitch_);
    layout->addLayout(row2);

    // 关闭按钮
    QPushButton* closeBtn = new QPushButton(QStringLiteral("关闭"));
    connect(closeBtn, &QPushButton::clicked, this, &QDialog::accept);
    layout->addWidget(closeBtn, 0, Qt::AlignHCenter);
}

void SettingWindow::setBgMusicEnabled(bool enabled) {
    bgMusicSwitch_->setChecked(enabled);
}

void SettingWindow::setSfxEnabled(bool enabled) {
    sfxSwitch_->setChecked(enabled);
}

void SettingWindow::onBgMusicToggled(bool checked) {
    emit bgMusicToggled(checked);
}

void SettingWindow::onSfxToggled(bool checked) {
    emit sfxToggled(checked);
}