#include "videobuttons.h"
#include <QVideoWidget>
#include <QMediaPlayer>
#include <QDialog>
#include <QVBoxLayout>
#include <QUrl>

QDialog *httpVideoDialog = nullptr;  // 用于跟踪 HTTP 视频对话框
QDialog *rtspVideoDialog = nullptr;  // 用于跟踪 RTSP 视频对话框
QMediaPlayer *httpPlayer = nullptr;  // HTTP 视频播放器
QMediaPlayer *rtspPlayer = nullptr;  // RTSP 视频播放器

void playHttpVideo(QMainWindow* mainWindow)
{
    if (httpVideoDialog) {
        // 在关闭窗口之前先停止播放并删除播放器对象
        if (httpPlayer) {
            httpPlayer->stop();
            delete httpPlayer;
            httpPlayer = nullptr;
        }
        httpVideoDialog->close();
        delete httpVideoDialog;
        httpVideoDialog = nullptr;
    }

    // 设置父窗口为 mainWindow
    httpVideoDialog = new QDialog(mainWindow);
    httpVideoDialog->setWindowTitle("HTTP 视频播放");
    httpVideoDialog->setFixedSize(320, 240);

    // 连接对话框关闭时的信号，以便停止播放器
    QObject::connect(httpVideoDialog, &QDialog::rejected, []() {
        if (httpPlayer) {
            httpPlayer->stop();
            delete httpPlayer;
            httpPlayer = nullptr;
        }
    });

    QVBoxLayout *dialogLayout = new QVBoxLayout(httpVideoDialog);
    QVideoWidget *videoWidget = new QVideoWidget(httpVideoDialog);
    dialogLayout->addWidget(videoWidget);

    if (!httpPlayer) {
        httpPlayer = new QMediaPlayer;
    }

    httpPlayer->setVideoOutput(videoWidget);
    httpPlayer->setMedia(QUrl("http://vjs.zencdn.net/v/oceans.mp4"));
    httpPlayer->play();

    httpVideoDialog->show();
}

void playRtspVideo(QMainWindow* mainWindow)
{
    if (rtspVideoDialog) {
        // 在关闭窗口之前先停止播放并删除播放器对象
        if (rtspPlayer) {
            rtspPlayer->stop();
            delete rtspPlayer;
            rtspPlayer = nullptr;
        }
        rtspVideoDialog->close();
        delete rtspVideoDialog;
        rtspVideoDialog = nullptr;
    }

    // 设置父窗口为 mainWindow
    rtspVideoDialog = new QDialog(mainWindow);
    rtspVideoDialog->setWindowTitle("RTSP 视频播放");
    rtspVideoDialog->setFixedSize(320, 240);

    // 连接对话框关闭时的信号，以便停止播放器
    QObject::connect(rtspVideoDialog, &QDialog::rejected, []() {
        if (rtspPlayer) {
            rtspPlayer->stop();
            delete rtspPlayer;
            rtspPlayer = nullptr;
        }
    });

    QVBoxLayout *dialogLayout = new QVBoxLayout(rtspVideoDialog);
    QVideoWidget *videoWidget = new QVideoWidget(rtspVideoDialog);
    dialogLayout->addWidget(videoWidget);

    if (!rtspPlayer) {
        rtspPlayer = new QMediaPlayer;
    }

    rtspPlayer->setVideoOutput(videoWidget);
    rtspPlayer->setMedia(QUrl("rtsp://dev.internal.xenron.net:10102/stream"));
    rtspPlayer->play();

    rtspVideoDialog->show();
}
