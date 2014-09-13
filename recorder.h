#ifndef RECORDER_H
#define RECORDER_H

#include <QObject>
#include <QThread>
#include <QString>

#include <iostream>

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

public slots:
    void init();    //inicializacia video suboru
    void saveMat(cv::Mat *mat);

private:
    cv::VideoWriter output_cap;

    bool openVideoFile();
};

#endif // RECORDER_H
