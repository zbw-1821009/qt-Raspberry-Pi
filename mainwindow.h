#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QTextEdit>
#include <QProcess>
#include "joystick_control.h"
#include "bluetooth_control.h"
#include "videobuttons.h"
// 主窗口类，继承自QMainWindow
class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);  // 构造函数

private slots:
    // 按钮对应的槽函数，用于处理按钮的点击事件
    void handleBluetoothControl();
    void handleJoystickControl();
    void handleStartStream();
    void handleStopStream();
    void handleCameraPreview();
    void handleProcessOutput();  // 处理进程输出
    void handleHttpVideo();  // 处理 HTTP 视频
    void handleRtspVideo();  // 处理 RTSP 视频

private:
    // 界面布局和按钮
    QWidget *centralWidget;  // 中心部件，包含所有控件
    QHBoxLayout *mainLayout;  // 水平布局，用于左侧按钮和右侧控制台区域
    QVBoxLayout *buttonLayout;  // 垂直布局，用于排列按钮
    QVBoxLayout *consoleLayout;  // 垂直布局，用于放置两个控制台
    QPushButton *bluetoothButton;  // 蓝牙控制按钮
    QPushButton *joystickButton;  // 摇杆控制按钮
    QPushButton *startStreamButton;  // 开始视频推送按钮
    QPushButton *stopStreamButton;  // 关闭视频推送按钮
    QPushButton *cameraPreviewButton;  // 显示摄像头按钮
    QPushButton *httpVideoButton;  // HTTP 视频按钮
    QPushButton *rtspVideoButton;  // RTSP 视频按钮
    QTextEdit *statusConsole;  // 显示运行状态的控制台
    QTextEdit *outputConsole;  // 显示运行时输出内容的控制台
    QProcess *process;  // 进程，用于执行外部命令并捕获输出
    QProcess *process2;  // 进程，用于执行外部命令并捕获输出

};

#endif // MAINWINDOW_H
