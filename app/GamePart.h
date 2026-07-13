#pragma once
#include <QObject>
#include <QWidget>
#include "../viewmodel/ChessViewModel.h"
#include "../view/StartWindow.h"
#include "../view/MainWindow.h"

/// GamePart — MVVM 装配者
///
/// 职责：
///   1. 创建 Model (通过 ViewModel 持有)、ViewModel、View 对象
///   2. 完成三种绑定：属性绑定、命令绑定、通知绑定
///   3. 管理窗口生命周期和页面跳转
///
/// 不负责：游戏规则、绘制、快照生成
class GamePart : public QObject {
    Q_OBJECT
public:
    explicit GamePart(QObject* parent = nullptr);
    ~GamePart();

    void start();

private:
    void showMainWindow(int mode, int difficulty);
    void bindMainWindow();   // 执行三种绑定
    void unbindMainWindow(); // 清理绑定
    void onBackToMenu();

    ChessViewModel viewModel_;

    StartWindow* startWindow_ = nullptr;
    MainWindow* mainWindow_ = nullptr;
};
