#include <QApplication>
#include "mainwindow.h"

// 程序入口，创建并显示主窗口
int main(int argc, char *argv[]) {
    QApplication app(argc, argv);  // 创建QApplication对象，负责管理应用程序的资源和事件循环

    MainWindow window;  // 创建主窗口对象
    window.setWindowTitle("Raspberry Pi Control Interface");  // 设置窗口标题
    window.resize(1000, 600);  // 设置窗口的分辨率为1000x600，以适应更多的控制台空间
    window.show();  // 显示主窗口

    return app.exec();  // 进入应用程序的事件循环
}
