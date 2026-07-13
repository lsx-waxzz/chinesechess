#include <QApplication>
#include "app/GamePart.h"

int main(int argc, char* argv[]) {
    QApplication a(argc, argv);

    // 全局样式表
    a.setStyleSheet(
        "QPushButton {"
        "   background-color: #4CAF50;"
        "   border: none;"
        "   color: white;"
        "   padding: 10px 20px;"
        "   font-size: 16px;"
        "   border-radius: 8px;"
        "   font-family: '楷体';"
        "}"
        "QPushButton:hover {"
        "   background-color: #45a049;"
        "}"
        "QPushButton:pressed {"
        "   background-color: #3d8b40;"
        "}"
        "QPushButton:disabled {"
        "   background-color: #cccccc;"
        "   color: #666666;"
        "}"
    );

    // App 层：创建装配者，启动程序
    GamePart gamePart;
    gamePart.start();

    return a.exec();
}
