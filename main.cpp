#include "mainwindow.h"
#include <QApplication>
#include <cvimagewidget.h>
#include <QThread>
#include <blinker.h>
#include <recorder.h>
#include <matcv.h>
#include <iostream>
#include <QMetaType>
#include <iostream>

int main(int argc, char *argv[]){
    QApplication a(argc, argv);

    //our new cv::Mat type
    // qRegisterMetaType<Matcv>("Matcv");
    qRegisterMetaType<cv::Mat*>("Mat*");

    //objects
    MainWindow *window = new MainWindow();
    Blinker *blinker = new Blinker();
    Recorder *recorder = new Recorder();
    //blinker->setCap(QString("C:/Users/Tomi/Desktop/bakalarka_temp/video/1/26122013_223310_cam.avi"));

    //BLINKER thread
    QThread* threadBlinker = new QThread;
    blinker->moveToThread(threadBlinker);

    //RECORDER thread
    QThread* threadRecorder = new QThread;
    recorder->moveToThread(threadRecorder);

    //worker thread
    QThread::connect(blinker, SIGNAL(finished()), threadBlinker, SLOT(quit()));
    QThread::connect(blinker, SIGNAL(finished()), blinker, SLOT(deleteLater()));
    QThread::connect(threadBlinker, SIGNAL(finished()), threadBlinker, SLOT(deleteLater()));

    QThread::connect(recorder, SIGNAL(finished()), threadRecorder, SLOT(quit()));
    QThread::connect(recorder, SIGNAL(finished()), recorder, SLOT(deleteLater()));
    QThread::connect(threadRecorder, SIGNAL(finished()), threadRecorder, SLOT(deleteLater()));

    //QThread::connect(window, SIGNAL(finished()), window, SLOT(quit()));
    //QThread::connect(window, SIGNAL(finished()), window, SLOT(deleteLater()));


    QThread::connect(blinker, SIGNAL(newFrameSignal(cv::Mat*)), window, SLOT(showMat(cv::Mat*)));
    QThread::connect(blinker, SIGNAL(blinkSignal()), window, SLOT(printBlink()));

    QThread::connect(blinker, SIGNAL(blinkSignal()), window, SLOT(showBlinkExecute()));
    QThread::connect(blinker, SIGNAL(minuteSignal(int)), window, SLOT(updateTable(int)));


    QThread::connect(window, SIGNAL(startCaptureSignal()), blinker, SLOT(startCapture()));
    QThread::connect(window, SIGNAL(stopCaptureSignal()), blinker, SLOT(stopCapture()));
    QThread::connect(window, SIGNAL(finished()), blinker, SLOT(stopCapture()));
    QThread::connect(window, SIGNAL(sendPushed()), blinker, SLOT(toggleSend()));
    QThread::connect(window, SIGNAL(reinitSignal()), blinker, SLOT(manualReinit()));
    QThread::connect(window, SIGNAL(reinitSignal()), window, SLOT(eraseTable()));
    QThread::connect(window, SIGNAL(startRecordSignal()), window, SLOT(eraseTable()));
    QThread::connect(window, SIGNAL(startRecordSignal()), blinker, SLOT(manualReinit()));

    QThread::connect(window, SIGNAL(startRecordSignal()), recorder, SLOT(init()));
    QThread::connect(window, SIGNAL(stopRecordSignal()), recorder, SLOT(save()));
    QThread::connect(window, SIGNAL(startRecordSignal()), blinker, SLOT(toggleRecord()));
    QThread::connect(window, SIGNAL(stopRecordSignal()), blinker, SLOT(toggleRecord()));

    QThread::connect(window, SIGNAL(stopCaptureSignal()), recorder, SLOT(save()));
    QThread::connect(window, SIGNAL(finished()), recorder, SLOT(save()));


    QThread::connect(blinker, SIGNAL(newRecordFrameSignal(cv::Mat*)), recorder, SLOT(saveMat(cv::Mat*)));


    QThread::connect(recorder, SIGNAL(selectFolderSignal()), window, SLOT(informationBox1()));
    QThread::connect(recorder, SIGNAL(changeRecordButton()), window, SLOT(changeRecordLabel()));

    QThread::connect(window, SIGNAL(folderSelectedSignal(QString)), recorder, SLOT(setFolder(QString)));


    QTimer::singleShot(1000, window, SLOT(updateTime()));
    //QTimer::


    threadBlinker->start();
    threadRecorder->start();

    window->show();

    return a.exec();
}
