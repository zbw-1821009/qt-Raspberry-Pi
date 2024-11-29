#include "mainwindow.h"
#include <QDebug>  // 用于调试输出
#include <thread>
#include <atomic>
#include "globals.h"
#include "streaming.h"  

std::atomic<bool> stop_program(false);
std::atomic<bool> stop_joysProgram(false);
//void blue();
void blueMain();
void obstacleAvoidance();

// 主窗口构造函数，初始化界面布局和按钮
MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent) {
    centralWidget = new QWidget(this);  // 创建中心部件
    mainLayout = new QHBoxLayout(centralWidget);  // 创建水平布局，用于整体布局
    buttonLayout = new QVBoxLayout();  // 创建垂直布局，用于按钮排列
    consoleLayout = new QVBoxLayout();  // 创建垂直布局，用于放置两个控制台

    // 创建按钮并添加到布局中
    bluetoothButton = new QPushButton("蓝牙控制", this);  // 蓝牙控制按钮
    joystickButton = new QPushButton("摇杆控制", this);  // 摇杆控制按钮
    startStreamButton = new QPushButton("视频推送", this);  // 开始视频推送按钮
    stopStreamButton = new QPushButton("关闭视频推送", this);  // 关闭视频推送按钮
    cameraPreviewButton = new QPushButton("显示摄像头", this);  // 显示摄像头按钮
    httpVideoButton = new QPushButton("播放 HTTP 视频", this);  // HTTP 视频按钮（添加的新功能）
    rtspVideoButton = new QPushButton("播放 RTSP 视频", this);  // RTSP 视频按钮（添加的新功能）

    // 设置按钮的最小大小，以避免按钮过小
    QSize buttonSize(150, 50);
    bluetoothButton->setMinimumSize(buttonSize);
    joystickButton->setMinimumSize(buttonSize);
    startStreamButton->setMinimumSize(buttonSize);
    stopStreamButton->setMinimumSize(buttonSize);
    cameraPreviewButton->setMinimumSize(buttonSize);
    httpVideoButton->setMinimumSize(buttonSize);
    rtspVideoButton->setMinimumSize(buttonSize);

    // 将按钮添加到垂直布局中
    buttonLayout->addWidget(bluetoothButton);
    buttonLayout->addWidget(joystickButton);
    buttonLayout->addWidget(startStreamButton);
    buttonLayout->addWidget(stopStreamButton);
    buttonLayout->addWidget(cameraPreviewButton);
    buttonLayout->addWidget(httpVideoButton);  // 添加 HTTP 视频按钮（新添加）
    buttonLayout->addWidget(rtspVideoButton);  // 添加 RTSP 视频按钮（新添加）

    // 创建两个控制台窗口
    statusConsole = new QTextEdit(this);  // 用于显示运行状态
    statusConsole->setReadOnly(true);  // 只读
    statusConsole->setPlaceholderText("运行状态...");

    outputConsole = new QTextEdit(this);  // 用于显示运行时输出内容
    outputConsole->setReadOnly(true);  // 只读
    outputConsole->setPlaceholderText("运行时输出...");

    // 将控制台添加到垂直布局中
    consoleLayout->addWidget(statusConsole);
    consoleLayout->addWidget(outputConsole);

    // 将按钮布局添加到主布局的左侧，将控制台布局添加到右侧
    mainLayout->addLayout(buttonLayout);
    mainLayout->addLayout(consoleLayout);

    centralWidget->setLayout(mainLayout);  // 设置中心部件的布局
    setCentralWidget(centralWidget);  // 设置窗口的中心部件

    // 创建QProcess对象用于管理外部进程
    process = new QProcess(this);
    connect(process, &QProcess::readyReadStandardOutput, this, &MainWindow::handleProcessOutput);
    connect(process, &QProcess::readyReadStandardError, this, &MainWindow::handleProcessOutput);
    process2 = new QProcess(this);
    connect(process2, &QProcess::readyReadStandardOutput, this, &MainWindow::handleProcessOutput);
    connect(process2, &QProcess::readyReadStandardError, this, &MainWindow::handleProcessOutput);

    // 连接按钮与槽函数，当按钮被点击时调用对应的槽函数
    connect(bluetoothButton, &QPushButton::clicked, this, &MainWindow::handleBluetoothControl);
    connect(joystickButton, &QPushButton::clicked, this, &MainWindow::handleJoystickControl);
    connect(startStreamButton, &QPushButton::clicked, this, &MainWindow::handleStartStream);
    connect(stopStreamButton, &QPushButton::clicked, this, &MainWindow::handleStopStream);
    connect(cameraPreviewButton, &QPushButton::clicked, this, &MainWindow::handleCameraPreview);
    connect(httpVideoButton, &QPushButton::clicked, this, &MainWindow::handleHttpVideo);  // 连接 HTTP 视频按钮和槽函数（新添加）
    connect(rtspVideoButton, &QPushButton::clicked, this, &MainWindow::handleRtspVideo);  // 连接 RTSP 视频按钮和槽函数（新添加）

}

// 槽函数：蓝牙控制
// 关闭摇杆程序（如果正在运行），启动蓝牙控制功能
void MainWindow::handleBluetoothControl() {
    qDebug() << "Handling Bluetooth Control";
    statusConsole->append("Handling Bluetooth Control");
    //QProcess::execute("pkill", QStringList() << "-f" << "Joystick8.py");  // 使用pkill命令关闭正在运行的摇杆控制程序

    stop_joysProgram = true;// 停止joys

    // 启动蓝牙控制和避障线程
    stop_program = false;

    std::thread blueThread(blueMain); // 创建蓝牙线程
    std::thread obstacleThread(obstacleAvoidance); // 创建避障线程

    // 分离线程，使其在后台运行
    blueThread.detach();
    obstacleThread.detach();
}

// 槽函数：摇杆控制
// 关闭蓝牙控制程序（如果正在运行），运行C++代码来控制摇杆
void MainWindow::handleJoystickControl() {
    qDebug() << "Handling Joystick Control";
    statusConsole->append("Handling Joystick Control");
    //QProcess::execute("pkill", QStringList() << "-f" << "car.py");  // 使用pkill命令关闭正在运行的蓝牙控制程序
    stop_program = true; // 停止蓝牙控制和避障
    stop_joysProgram = false;
    std::thread joysThread(controlJoystick); // 控制摇杆
    joysThread.detach();
}

// 槽函数：开始视频推送
void MainWindow::handleStartStream() {
    qDebug() << "Starting Video Stream";
    statusConsole->append("Starting Video Stream");

    // 调用新函数启动 RTSP 服务器
    startRTSPServer(process, statusConsole);

    // 调用新函数使用 FFmpeg 推送摄像头视频流
  //  startFFmpegStream(process2, statusConsole);
}

// 槽函数：停止视频推送
// 关闭RTSP服务器和视频推流
void MainWindow::handleStopStream() {
    qDebug() << "Stopping Video Stream";
    statusConsole->append("Stopping Video Stream");
    QProcess::execute("pkill", QStringList() << "-f" << "rtsp-simple-server");  // 使用pkill命令关闭RTSP服务器
    QProcess::execute("pkill", QStringList() << "-f" << "ffmpeg");  // 使用pkill命令关闭ffmpeg视频推流
}

// 槽函数：显示摄像头
// 使用mplayer显示USB摄像头画面
void MainWindow::handleCameraPreview() {
    qDebug() << "Showing Camera Preview";
    statusConsole->append("Showing Camera Preview");
    process->start("libcamera-vid", QStringList()
            << "-t" << "0"
            << "--inline"
            << "--width" << "1280"
            << "--height" << "720"
            << "--framerate" << "30"
            << "--bitrate" << "1000000"
        );
}

// 槽函数：处理进程输出
// 将进程输出内容显示到输出控制台
void MainWindow::handleProcessOutput() {
    QString output = process->readAllStandardOutput();
    QString errorOutput = process->readAllStandardError();
    if (!output.isEmpty()) {
        outputConsole->append(output);
    }
    if (!errorOutput.isEmpty()) {
        outputConsole->append(errorOutput);
    }
}

// 槽函数：处理 HTTP 视频
void MainWindow::handleHttpVideo() {
    qDebug() << "Handling HTTP Video";
    statusConsole->append("Handling HTTP Video");
    playHttpVideo(this);
}

// 槽函数：处理 RTSP 视频
void MainWindow::handleRtspVideo() {
    qDebug() << "Handling RTSP Video";
    statusConsole->append("Handling RTSP Video");
    playRtspVideo(this);
}
