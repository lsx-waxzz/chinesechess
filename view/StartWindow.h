#pragma once
#include <QWidget>
#include <QPushButton>
#include <QVBoxLayout>
#include <QLabel>
#include <functional>

class StartWindow : public QWidget {
    Q_OBJECT
public:
    /// @param onModeSelected 回调：(mode, difficulty)
    ///   mode: 0=双人, 1=人机
    explicit StartWindow(std::function<void(int mode, int difficulty)> onModeSelected,
                         QWidget* parent = nullptr);

private:
    void onTwoPlayerClicked();
    void onVsAIClicked();

    std::function<void(int mode, int difficulty)> onModeSelected_;
};
