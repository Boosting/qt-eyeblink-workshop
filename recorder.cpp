#include "recorder.h"

using namespace std;
using namespace cv;

Recorder::Recorder(){
}

Recorder::~Recorder() {
}

void Recorder::init(){
    openVideoFile();
}

void Recorder::saveMat(cv::Mat *mat){
    //cout << "tu som " << endl;
    //cout << (*mat)->cols << endl;
    //mat->print();
    cv::Mat temp = *mat;
    cout << temp.cols << endl;
    //cv::imshow("frame", temp);
    //ui->cvWidget->showImage(*mat);

    //we need to free memory here - after image is showed
    /*
    if(mat){
        delete(mat);
        mat = NULL;
    }
    */
}

bool Recorder::openVideoFile(){
    Size size;
    size.width = 640;
    size.height = 480;

    std::string videoFileAddress = "./videoSave.avi";

    //output_cap.open(videoFileAddress.c_str(), -1, 30.0, size, true);
    //if x264 does not exist, open -1 dialog to choose manually
    if(output_cap.open(videoFileAddress.c_str(), CV_FOURCC('X','2','6','4'), 30.0, size, true) ||
            output_cap.open(videoFileAddress.c_str(), CV_FOURCC('x','2','6','4'), 30.0, size, true) ||
            output_cap.open(videoFileAddress.c_str(), CV_FOURCC('H','2','6','4'), 30.0, size, true) ||
            output_cap.open(videoFileAddress.c_str(), CV_FOURCC('h','2','6','4'), 30.0, size, true)
            ){

    }
    else{
        cout << endl << "Cannot find chosen codec" << endl;
        output_cap.open(videoFileAddress.c_str(), -1, 30.0, size, true);
    }

    if(!output_cap.isOpened()){
        cout << "Video file open failed" << endl;
        return false;
    }
    else{
        cout << "Video file open success" << endl;
        return true;
    }
}
