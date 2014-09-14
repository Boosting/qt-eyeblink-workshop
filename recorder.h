#ifndef RECORDER_H
#define RECORDER_H

#include <QObject>
#include <QThread>
#include <QString>
#include <string>
#include <QDir>

#include <iostream>
#include <fstream>
#include <ctime>

#include "opencv2/opencv.hpp"
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/core/core.hpp>

#include <matcv.h>

class Recorder : public QObject {
    Q_OBJECT

public:
    Recorder();
    ~Recorder();

signals:
    void finished();
    void selectFolderSignal();
    void changeRecordButton();

public slots:
    void init();    //inicializacia video suboru
    void save();    //ulozenie a ukoncenie recordera
    bool saveMat(cv::Mat *mat);
    void setFolder(QString str);

private:
    cv::VideoWriter output_cap;
    std::ofstream outputFile;

    QString folderAddress;
    QString currentFolderAddress;
    QString videoFileAddress;
    QString outputFileAddress;

    int64 startTime, currentTime;
    float elapsedTime;
    int frameCounter;

    bool recording;


    bool openVideoFile();
    bool openOutputFile();
    void closeOutputFile();
    void closeVideoFile();
    void generateNewFileName();
};

#endif // RECORDER_H
