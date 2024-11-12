#ifndef STREAMING_H
#define STREAMING_H

#include <QString>
#include <QProcess>
#include <QTextEdit>

void startRTSPServer(QProcess *process, QTextEdit *statusConsole);
void startFFmpegStream(QProcess *process, QTextEdit *statusConsole);

#endif // STREAMING_H
