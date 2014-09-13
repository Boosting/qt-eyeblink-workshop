#ifndef BLINKER_H
#define BLINKER_H

#include <QObject>
#include <QThread>
#include <QString>
#include <QTimer>

#include <iostream>
#include <time.h>
#include <vector>
#include <math.h>

#include "opencv2/opencv.hpp"
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/core/core.hpp>

#include<matcv.h>

class Blinker : public QObject {
    Q_OBJECT

public:
    Blinker();
    ~Blinker();
    void setCap(QString string);

public slots:
    void startCapture();    //spusti detekciu zmurknutia
    void stopCapture();     //zastavi detekciu zmurknutia
    void toggleSend();      //prepina medzi zasielanim frejmu a nezasielanim frejmu
    void manualReinit();    //manualna reinicializacia

signals:
    void finished();
    void newFrameSignal(cv::Mat *matrix);   //mam novy frejm a mozem ho poslat
    void blinkSignal();     //nastalo zmurknutie
    void subjectPresentSignal(bool isVisible);  //signal s atributom ci je subjekt viditelny

private:
    bool stop;  //ovladanie capture
    bool send;  //ovladanie zasielania frejmu

    //capture variables
    QString capString;
    cv::VideoCapture cap;
    cv::Mat frame, previousFrame;
    int frameCount;
    cv::CascadeClassifier faceCascade, eyeCascade;
    float fpsNum = 30.0;
    float start, end;

    //algorithm variables
    bool init;
    bool print;
    bool leftBlink, rightBlink;
    std::vector<cv::Rect> eyes;
    cv::Rect leftRect, rightRect;
    std::vector<cv::Point2f> leftFeatures, rightFeatures;
    std::vector<cv::Point2f> leftFeaturesNext, rightFeaturesNext;
    std::vector<cv::Point2f> leftFinalPoints, rightFinalPoints;
    std::vector<uchar> statusLeft, statusRight;
    std::vector<float> errLeft, errRight;
    int leftFeaturesNumber, rightFeaturesNumber;
    int leftLost, rightLost;

    cv::Point2f leftCenter, rightCenter;
    int centerDist;

    std::vector<float> movesLeftY;
    std::vector<float> movesLeftX;
    std::vector<int> movesLeftCount;
    std::vector<float> movesRightY;
    std::vector<float> movesRightX;
    std::vector<int> movesRightCount;

    std::vector<float> movesLeftYPrev;
    std::vector<float> movesLeftXPrev;
    std::vector<int> movesLeftCountPrev;
    std::vector<float> movesRightYPrev;
    std::vector<float> movesRightXPrev;
    std::vector<int> movesRightCountPrev;

    int blinkFramesLeft, blinkFramesRight;
    int stateLeft, stateRight;
    int blinkCount, nonblinkCount, reinitCount;
    int blinkThresh;

    //help variables

    //methods
    bool loadCascades();
    bool initialize();
    bool getEyes();
    cv::Rect resizeRect(cv::Rect rect, float hor, float ver);
    void placeFeatures(std::vector<cv::Point2f> &features, int &featuresNumber, cv::Rect rect);
    void removeBadPoints(std::vector<cv::Point2f> &features, std::vector<cv::Point2f> &featuresNext, int &lost, std::vector<uchar> status, cv::Rect rect);
    cv::Point2f getCenter(std::vector<cv::Point2f> vec);
    cv::Rect getRect(std::vector<cv::Point2f> vec, cv::Point2f center);
    void sortMoves(std::vector<cv::Point2f> &finalPoints, std::vector<float> &movesX, std::vector<float> &movesY, std::vector<int> &movesCount, int &lost, std::vector<cv::Point2f> &features, std::vector<cv::Point2f> &featuresNext, cv::Rect rect, cv::Point2f center);
    float getVariance(std::vector<float> vec);
    bool getStatus(std::vector<float> current, std::vector<int> currentCount, std::vector<float> previous, std::vector<int> previousCount, int distance, int &state, int &blinkFrame, float fpsN);
    void drawFrame();
    void resetVariables();
    void reinitCond();

    void sendFrame();

};

#endif // BLINKER_H
