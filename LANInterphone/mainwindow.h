#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

#include <QtNetwork/QUdpSocket>
#include <QNetworkInterface>

#include "audiothread.h"
#include "audiorecthread.h"
#include <QAudio>//一下这五个是QT处理音频的库
#include <QAudioFormat>
#include <QAudioInput>
#include <QAudioOutput>
#include <QIODevice>




namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    QString GetIpAddress();
    QString AddIp;
    QString MyIp;

private:
    Ui::MainWindow *ui;
    QUdpSocket *udpSocket;//用于音频信号的发送
    AudioThread *myAudio;
    AudioRecThread *myAudioRec;

private slots:
void MicroPhoneonReadyRead();
void RecreadyReadSlot();
void AddIPChange();
void onTimeout();





void on_pushButton_pressed();
void on_pushButton_2_clicked();
};

#endif // MAINWINDOW_H
