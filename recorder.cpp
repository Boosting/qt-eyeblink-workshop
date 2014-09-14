#include "recorder.h"

using namespace std;
using namespace cv;

Recorder::Recorder(){
    recording = false;
    folderAddress.clear();
}

Recorder::~Recorder() {
}

void Recorder::init(){
    if(recording){
        return;
    }

    if(folderAddress.isEmpty()){
        emit selectFolderSignal();
        return;
    }

    int max = 0;
    int folderCounter = 0;

    //find next folder number
    QString pathOpen = QString(folderAddress);
    QDir dirOpen(pathOpen);
    //dirOpen.setNameFilters(QStringList() << "*.jpg");
    dirOpen.setFilter(QDir::Dirs);
    foreach(QFileInfo dirFile, dirOpen.entryInfoList()){
        int value = atoi(dirFile.fileName().toStdString().c_str());

        if(max < value){
            max = value;
        }
    }
    //if folder is empty
    if(max == 0){
        //folder name is 1
        folderCounter = 1;
    }
    else{
        //folder name is max value + 1
        folderCounter = max + 1;
    }

    stringstream ss;
    ss << folderCounter;

    //set destination folder
    currentFolderAddress.clear();
    currentFolderAddress.append(folderAddress);
    currentFolderAddress.append(QString("/")).append(QString(ss.str().c_str()));

    //recorder.setTimeLabel(ui->label_3);

    //create folder if it does not exist
    if(!QDir(currentFolderAddress).exists()){
        QDir().mkdir(currentFolderAddress);
    }

    recording = true;
    emit changeRecordButton();

    std::cout << "START RECORDING..." << std::endl;

    generateNewFileName();
    openVideoFile();
    openOutputFile();

    //reference time
    frameCounter = 0;
    startTime = getTickCount();
}

void Recorder::save(){
    if(!recording){
        return;
    }

    recording = false;
    emit changeRecordButton();

    std::cout << "STOP RECORDING!" << std::endl;
    closeVideoFile();
    closeOutputFile();
}

bool Recorder::saveMat(cv::Mat *mat){
    cv::Mat frame = *mat;
    //cout << temp.cols << endl;

    //current time
    currentTime = getTickCount();

    if(frame.empty()){
        cout << "capture error" << endl;
        return false;
    }

    //imshow(videoFileAddress.c_str(), frame);
    output_cap << frame;

    elapsedTime = (currentTime - startTime)/getTickFrequency();
    outputFile << frameCounter << " " << elapsedTime << endl;

    //printTime();

    /*key = waitKey(1);
    if(toupper(key) == 'S'){
        //break;
    }
    if(stopRecording){
        break;
    }*/

    frameCounter++;

    return true;
}

bool Recorder::openVideoFile(){
    Size size;
    size.width = 640;
    size.height = 480;

    cout << "otvaram " << videoFileAddress.toStdString() << endl;

    //output_cap.open(videoFileAddress.c_str(), -1, 30.0, size, true);
    //if x264 does not exist, open -1 dialog to choose manually
    if(output_cap.open(videoFileAddress.toStdString().c_str(), CV_FOURCC('X','2','6','4'), 30.0, size, true) ||
            output_cap.open(videoFileAddress.toStdString().c_str(), CV_FOURCC('x','2','6','4'), 30.0, size, true) ||
            output_cap.open(videoFileAddress.toStdString().c_str(), CV_FOURCC('H','2','6','4'), 30.0, size, true) ||
            output_cap.open(videoFileAddress.toStdString().c_str(), CV_FOURCC('h','2','6','4'), 30.0, size, true)
            ){

    }
    else{
        cout << endl << "Cannot find chosen codec" << endl;
        output_cap.open(videoFileAddress.toStdString().c_str(), -1, 30.0, size, true);
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

bool Recorder::openOutputFile(){
    cout << "otvaram " << outputFileAddress.toStdString().c_str() << endl;

    outputFile.open(outputFileAddress.toStdString().c_str());

    if(!outputFile.is_open()){
        cout << "Output file open failed" << endl;
        return false;
    }
    else{
        //cout << "Output file open success" << endl;
        return true;
    }
}

void Recorder::closeOutputFile(){
    outputFile.close();

    //cout << "Output file closed success" << endl;
}

void Recorder::closeVideoFile(){
    output_cap.release();

    //cout << "Video file closed success" << endl;
}

void Recorder::generateNewFileName(){
    time_t rawtime;
    tm* timeinfo;
    char buffer[80];

    time(&rawtime);
    timeinfo = localtime(&rawtime);

    //file name string format
    strftime(buffer, 80, "/%d%m%Y_%H%M%S_cam", timeinfo);

    videoFileAddress.clear();
    videoFileAddress.append(currentFolderAddress);
    videoFileAddress.append(buffer).append(QString(".avi"));

    outputFileAddress.clear();
    outputFileAddress.append(currentFolderAddress);
    outputFileAddress.append(buffer).append(QString(".txt"));
}

void Recorder::setFolder(QString str){
    folderAddress.clear();
    folderAddress.append(str);
}
