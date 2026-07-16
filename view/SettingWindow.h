#pragma once
#ifndef QT_NO_DEPRECATED_WARNINGS
#define QT_NO_DEPRECATED_WARNINGS
#endif
#include <QDialog>
#include <QVBoxLayout>
#include <QLabel>
#include "Switch.h"

class SettingWindow : public QDialog {
    Q_OBJECT
public:
    explicit SettingWindow(QWidget* parent = nullptr);

    bool isBgMusicEnabled() const { return bgMusicSwitch_->isChecked(); }
    bool isSfxEnabled() const { return sfxSwitch_->isChecked(); }

    void setBgMusicEnabled(bool enabled);
    void setSfxEnabled(bool enabled);

signals:
    void bgMusicToggled(bool enabled);
    void sfxToggled(bool enabled);

private slots:
    void onBgMusicToggled(bool checked);
    void onSfxToggled(bool checked);

private:
    Switch* bgMusicSwitch_;
    Switch* sfxSwitch_;
};