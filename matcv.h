#ifndef MATCV_H
#define MATCV_H

#include"opencv/cv.h"
#include"opencv2/opencv.hpp"
#include<opencv2/imgproc/imgproc.hpp>
#include<opencv2/highgui/highgui.hpp>
#include<opencv2/core/core.hpp>
#include<qmetatype.h>
#include<iostream>

class Matcv
{
public:
    Matcv();
    Matcv(const Matcv &other);
    ~Matcv();

    Matcv(cv::Mat temp);

    cv::Mat getMatrix();
    void setMatrix(cv::Mat temp);
    void print();

private:
    cv::Mat matrix;
};

Q_DECLARE_METATYPE(Matcv)

#endif // MATCV_H



