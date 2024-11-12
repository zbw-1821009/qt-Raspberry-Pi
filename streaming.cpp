#include "streaming.h"
#include <QDebug>

void startRTSPServer(QProcess *process, QTextEdit *statusConsole) {
//    QString downloadCmd = "./rtsp-simple-server";
      QString downloadCmd = "/home/zhang/rtsp-simple-server";
    process->start("/bin/sh", QStringList() << "-c" << downloadCmd);

    if (!process->waitForStarted()) {
        statusConsole->append("Failed to start RTSP server");
        return;
    }

    statusConsole->append("RTSP server started");
}

void startFFmpegStream(QProcess *process, QTextEdit *statusConsole) {
    QString ffmpegCmd = "ffmpeg -f v4l2 -i /dev/video0 -vcodec libx264 -preset ultrafast -f rtsp rtsp://localhost:8554/live/test";
//    process->start("ffmpeg", QStringList() << "-f" << "v4l2" << "-i" << "/dev/video0"
//                                           << "-vcodec" << "libx264" << "-preset" << "ultrafast"
//                                           << "-f" << "rtsp" << "rtsp://localhost:8554/live.sdp");

    process->start("/bin/sh", QStringList() << "-c" << ffmpegCmd);

    if (!process->waitForStarted()) {
        statusConsole->append("Failed to start FFmpeg");
        return;
    }

    // 监听标准错误输出，显示到状态控制台中
    QObject::connect(process, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished),
                     [statusConsole](int exitCode, QProcess::ExitStatus exitStatus) {
        statusConsole->append(QString("FFmpeg finished with exit code %1, status %2")
                              .arg(exitCode)
                              .arg(exitStatus == QProcess::NormalExit ? "Normal" : "Crashed"));
    });


    statusConsole->append("FFmpeg streaming started");
}



