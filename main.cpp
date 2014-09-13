#include "mainwindow.h"
#include <cvimagewidget.h>
#include <QApplication>
#include <QThread>
#include <blinker.h>
#include <matcv.h>
#include <iostream>
#include <QMetaType>


int main(int argc, char *argv[]){
    QApplication a(argc, argv);

    //our new cv::Mat type
    // qRegisterMetaType<Matcv>("Matcv");
    qRegisterMetaType<cv::Mat*>("Mat*");

    //objects
    MainWindow *window = new MainWindow();
    Blinker *blinker = new Blinker();
    //blinker->setCap(QString("C:/Users/Tomi/Desktop/bakalarka_temp/video/1/26122013_223310_cam.avi"));


    //threads
    QThread* threadBlinker = new QThread;
    blinker->moveToThread(threadBlinker);


    //worker thread
    QThread::connect(blinker, SIGNAL(finished()), threadBlinker, SLOT(quit()));
    QThread::connect(blinker, SIGNAL(finished()), blinker, SLOT(deleteLater()));
    QThread::connect(threadBlinker, SIGNAL(finished()), threadBlinker, SLOT(deleteLater()));

    //QThread::connect(window, SIGNAL(finished()), window, SLOT(quit()));
    //QThread::connect(window, SIGNAL(finished()), window, SLOT(deleteLater()));


    QThread::connect(blinker, SIGNAL(newFrameSignal(cv::Mat*)), window, SLOT(showMat(cv::Mat*)));
    QThread::connect(blinker, SIGNAL(blinkSignal()), window, SLOT(printBlink()));

    QThread::connect(window, SIGNAL(startCaptureSignal()), blinker, SLOT(startCapture()));
    QThread::connect(window, SIGNAL(stopCaptureSignal()), blinker, SLOT(stopCapture()));
    QThread::connect(window, SIGNAL(sendPushed()), blinker, SLOT(toggleSend()));
    QThread::connect(window, SIGNAL(reinitSignal()), blinker, SLOT(manualReinit()));


    threadBlinker->start();

    window->show();

    return a.exec();
}
