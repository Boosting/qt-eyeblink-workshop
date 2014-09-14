#include "blinker.h"

using namespace std;
using namespace cv;

Blinker::Blinker() {
    frameCount = 0;

    stop = false;
    send = true;
    record = false;

    print = false;

    init = true;
    leftBlink = false;
    rightBlink = false;

    blinkFramesLeft = 0;
    blinkFramesRight = 0;
    stateLeft = 0;
    stateRight = 0;

    blinkCount = 0;
    nonblinkCount = 0;
    reinitCount = 0;
    blinkThresh = 20;

    movesLeftY.assign(9, 0);
    movesLeftX.assign(9, 0);
    movesLeftCount.assign(9, 0);
    movesRightY.assign(9, 0);
    movesRightX.assign(9, 0);
    movesRightCount.assign(9, 0);

    movesLeftYPrev.assign(9, 0);
    movesLeftXPrev.assign(9, 0);
    movesLeftCountPrev.assign(9, 0);
    movesRightYPrev.assign(9, 0);
    movesRightXPrev.assign(9, 0);
    movesRightCountPrev.assign(9, 0);

    srand(time(NULL));
}

Blinker::~Blinker() {
}

void Blinker::startCapture(){
    //if this is the first frame
    if(frameCount == 0){
        if(!initialize()){
            return;
        }

        cap >> previousFrame;
        if(previousFrame.empty()){
            cout << "bad frame" << endl;
            QTimer::singleShot(0, this, SLOT(startCapture()));
            return;
        }
        cvtColor(previousFrame, previousFrame, CV_BGR2GRAY);
        frameCount++;
    }

    //process
    cap >> frame;
    frameCount++;

    if(frame.empty()){
        cout << "bad frame" << endl;
        QTimer::singleShot(0, this, SLOT(startCapture()));
        return;
    }

    cvtColor(frame, frame, CV_BGR2GRAY);

    //reinitialization
    if(init){
        leftFeatures.clear();
        rightFeatures.clear();
        leftFeaturesNext.clear();
        rightFeaturesNext.clear();
        leftFinalPoints.clear();
        rightFinalPoints.clear();
        leftFeaturesNumber = 0;
        rightFeaturesNumber = 0;
        leftLost = 0;
        rightLost = 0;
        nonblinkCount = 0;

        //eye rectangles
        if(!getEyes()){
            frame.copyTo(previousFrame);

            if(send){
                sendFrame();
            }

            QTimer::singleShot(0, this, SLOT(startCapture()));

            emit subjectPresentSignal(false);

            return;
        }

        leftRect = resizeRect(eyes[0], 1.0, 1.5);
        rightRect = resizeRect(eyes[1], 1.0, 1.5);

        //bad rectangles
        if(abs(leftRect.x - rightRect.x) < leftRect.width || abs(leftRect.y - rightRect.y) > leftRect.height){
            frame.copyTo(previousFrame);

            if(send){
                sendFrame();
            }

            QTimer::singleShot(0, this, SLOT(startCapture()));

            emit subjectPresentSignal(true);

            return;
        }

        //left eye features
        placeFeatures(leftFeatures, leftFeaturesNumber, leftRect);
        //right eye features
        placeFeatures(rightFeatures, rightFeaturesNumber, rightRect);

        emit subjectPresentSignal(true);

        init = false;
    }

    //klt tracker
    calcOpticalFlowPyrLK(previousFrame, frame, leftFeatures, leftFeaturesNext, statusLeft, errLeft);
    calcOpticalFlowPyrLK(previousFrame, frame, rightFeatures, rightFeaturesNext, statusRight, errRight);

    //bad points
    removeBadPoints(leftFeatures, leftFeaturesNext, leftLost, statusLeft, leftRect);
    removeBadPoints(rightFeatures, rightFeaturesNext, rightLost, statusRight, rightRect);

    //get eye centers accroding to points
    leftCenter = getCenter(leftFeaturesNext);
    rightCenter = getCenter(rightFeaturesNext);

    centerDist = abs((int) roundf(leftCenter.x - rightCenter.x));

    //get eye rects using the distance histogram
    leftRect = getRect(leftFeaturesNext, leftCenter);
    rightRect = getRect(rightFeaturesNext, rightCenter);

    //3x3 cells
    sortMoves(leftFinalPoints, movesLeftX, movesLeftY, movesLeftCount, leftLost, leftFeatures, leftFeaturesNext, leftRect, leftCenter);
    sortMoves(rightFinalPoints, movesRightX, movesRightY, movesRightCount, rightLost, rightFeatures, rightFeaturesNext, rightRect, rightCenter);

    //final blink status
    leftBlink = getStatus(movesLeftY, movesLeftCount, movesLeftYPrev, movesLeftCountPrev, centerDist, stateLeft, blinkFramesLeft, fpsNum);
    rightBlink = getStatus(movesRightY, movesRightCount, movesRightYPrev, movesRightCountPrev, centerDist, stateRight, blinkFramesRight, fpsNum);

    //blink occured
    if((leftBlink || rightBlink) && blinkCount > blinkThresh){
        emit blinkSignal();

        //minimum number of frames between two blinks
        blinkCount = 0;
        //number of frames without blink
        nonblinkCount = 0;
        //reinit delay
        reinitCount = 0;
    }

    //must be before drawing to the frame
    frame.copyTo(previousFrame);

    //count fps
    start = end;
    end = (float) getTickCount();
    fpsNum = 1.0 / (end - start) * getTickFrequency();

    //send record signal with frame
    if(record){
        sendRecordFrame();
        //cout << "posielam" << endl;
    }

    drawFrame();
    reinitCond();
    resetVariables();

    //send signal with frame
    if(send){
        sendFrame();
    }

    //end capturing
    if(stop){
        frameCount = 0;
        stop = false;
        init = true;

        cap.release();

        //emit finished();
        //cout << "finisujem" << endl;

        return;
    }

    QTimer::singleShot(0, this, SLOT(startCapture()));

    return;
}

bool Blinker::initialize(){
    if(!loadCascades()){
        return false;
    }

    if(capString.isEmpty()){
        cap.open(0);
    }
    else{
        cap.open(capString.toStdString().c_str());
    }
    //cap.set(CV_CAP_PROP_FPS, 30); //limit fps

    if(!cap.isOpened()){
        cout << "camera is not ready" << endl;
        cap.release();
        return false;
    }

    return true;
}

bool Blinker::loadCascades(){
    //string faceCascadeName = "/home/tomas/source/qt_workspace/eyeblink/haarcascade_frontalface_default.xml";
    //string eyeCascadeName = "/home/tomas/source/qt_workspace/eyeblink/haarcascade_mcs_eyepair_big.xml";

    //string faceCascadeName = "C:/Users/Tomi/Documents/qt_workspace/eyeblink_workshop/haarcascade_frontalface_default.xml";
    //string eyeCascadeName = "C:/Users/Tomi/Documents/qt_workspace/eyeblink_workshop/haarcascade_mcs_eyepair_big.xml";

    string faceCascadeName = "./haarcascade_frontalface_default.xml";
    string eyeCascadeName = "./haarcascade_mcs_eyepair_big.xml";

    if(!faceCascade.load(faceCascadeName)){
        cout << "error loading face cascade!" << endl;
        return false;
    }
    if(!eyeCascade.load(eyeCascadeName)){
        cout << "error loading eye cascade!" << endl;
        return false;
    }

    return true;
}

bool Blinker::getEyes(){
    vector<Rect> faces;
    faceCascade.detectMultiScale(frame, faces, 1.1, 1, 0
                                 |CV_HAAR_FIND_BIGGEST_OBJECT
                                 |CV_HAAR_DO_ROUGH_SEARCH
                                 ,Size(50, 50));

    if(faces.size() == 0){
        //cout << "no face" << endl;
        return false;
    }

    Mat faceROI = frame(faces[0]);

    //detect eyes
    eyeCascade.detectMultiScale(faceROI, eyes, 1.1, 1, 0
                                //|CV_HAAR_FIND_BIGGEST_OBJECT
                                //|CV_HAAR_DO_CANNY_PRUNING
                                |CV_HAAR_DO_ROUGH_SEARCH
                                //|CV_HAAR_SCALE_IMAGE
                                ,Size(8, 8));
    if(eyes.size() != 1){
        //cout << "eyes not detected" << endl;
        return false;
    }

    Rect eye1, eye2;
    eye1.x = eyes[0].x + faces[0].x;
    eye1.y = eyes[0].y + faces[0].y;
    eye1.height = eyes[0].height;
    eye1.width = (int) roundf(eyes[0].width / 2.0);

    eye2.x = eyes[0].x + eye1.width + faces[0].x;;
    eye2.y = eyes[0].y + faces[0].y;;
    eye2.height = eyes[0].height;
    eye2.width = eye1.width;

    eyes.clear();
    eyes.push_back(eye1);
    eyes.push_back(eye2);

    return true;
}

Rect Blinker::resizeRect(Rect rect, float hor, float ver){
    rect.y = rect.y - (int) roundf((rect.height*ver - rect.height)/2);
    rect.height = (int) roundf(rect.height * ver);

    return rect;
}

void Blinker::placeFeatures(vector<cv::Point2f> &features, int &featuresNumber, Rect rect){
    int horfeatureGap = (int) roundf(rect.width / 15);
    int verfeatureGap = (int) roundf(rect.height / 15);
    //for(int i = 0; i < featureNumber; i++){
    for(int j = 0; j < rect.width; j++){
        for(int k = 0; k < rect.height; k++){
            if((j % (horfeatureGap) == 0) && (k % (verfeatureGap) == 0)){
                features.push_back(Point2f(float(rect.x + j), float(rect.y + k)));
                featuresNumber++;
            }
        }
    }
}

void Blinker::removeBadPoints(vector<cv::Point2f> &features, vector<cv::Point2f> &featuresNext, int &lost, vector<uchar> status, Rect rect){
    //manage status[i] points, if flow was found
    float dx, dy, distance;

    for(int i = 0; i < featuresNext.size(); i++){
        //if point was found
        if(status[i]){
            dx = featuresNext[i].x - features[i].x;
            dy = featuresNext[i].y - features[i].y;

            distance = sqrtf(dx*dx + dy*dy);

            //detect otuliers
            if(distance > rect.height/2){
                features.erase(features.begin() + i);
                featuresNext.erase(featuresNext.begin() + i);
                //leftFeaturesNumber--;
                i--;
                lost++;
                continue;
            }
        }
        //lost points
        else{
            features.erase(features.begin() + i);
            featuresNext.erase(featuresNext.begin() + i);
            //leftFeaturesNumber--;
            lost++;
            i--;
        }
    }
}

Point2f Blinker::getCenter(vector<Point2f> vec){
    float tempX = 0;
    float tempY = 0;

    for(int i = 0; i < vec.size(); i++){
        tempX += vec[i].x;
        tempY += vec[i].y;
    }

    tempX /= vec.size();
    tempY /= vec.size();

    return Point2f(tempX, tempY);
}

Rect Blinker::getRect(vector<Point2f> vec, Point2f center){
    vector<int> histogram(200, 0);

    float radius, xDist, yDist;
    for(int i = 0; i < vec.size(); i++){
        xDist = vec[i].x - center.x;
        yDist = vec[i].y - center.y;

        radius = sqrtf(xDist*xDist + yDist*yDist);
        histogram[(int) roundf(radius)]++;
    }


    int max = 0, maxIndex = 1000;
    int cutIndex = 0;
    for(int i = 1; i < histogram.size() - 1; i++){
        if(histogram[i] > histogram[i+1] && max < histogram[i]){
            max = histogram[i];
            maxIndex = i;
        }
    }

    for(int i = maxIndex; i < histogram.size() - 1; i++){
        if(histogram[i] <= 5 && histogram[i+1] <= 5 && histogram[i+2] <= 5){
            cutIndex = i;
            break;
        }
    }

    /*
    for(int i = 0; i < histogram.size(); i++){
        cout << histogram[i] << " ";
    }
    cout << endl;
    */

    cutIndex = cutIndex*0.8;
    Rect rect;
    rect.x = center.x - ((float) cutIndex) +1;
    rect.y = center.y - ((float) cutIndex) +1;
    rect.width = cutIndex*2;
    rect.height = cutIndex*2;

    return rect;
}

void Blinker::sortMoves(vector<Point2f> &finalPoints, vector<float> &movesX, vector<float> &movesY, vector<int> &movesCount, int &lost, vector<Point2f> &features, vector<Point2f> &featuresNext, Rect rect, Point2f center){
    for(int i = 0; i < featuresNext.size(); i++){
        Point point = featuresNext[i];

        float dx = point.x - center.x;
        float dy = point.y - center.y;

        float distance = sqrtf(dx*dx + dy*dy);

        //detect points far from center
        if(distance > rect.width){
            features.erase(features.begin() + i);
            featuresNext.erase(featuresNext.begin() + i);
            //featuresNumber--;
            i--;
            lost++;
            continue;
        }

        //consider only points in the rectangle
        if((point.x > rect.x) && (point.y > rect.y) && (point.x < rect.x + rect.width) && (point.y < rect.y + rect.height)){
            finalPoints.push_back(point);

            if(featuresNext[i].y < (rect.y + rect.height/3)){
                if(featuresNext[i].x < (rect.x + rect.width/3)){
                    movesY[0] += (featuresNext[i].y - features[i].y);
                    movesX[0] += (featuresNext[i].x - features[i].x);
                    movesCount[0]++;

                }
                else if(featuresNext[i].x < (rect.x + 2*rect.width/3)){
                    movesY[1] += (featuresNext[i].y - features[i].y);
                    movesX[1] += (featuresNext[i].x - features[i].x);
                    movesCount[1]++;
                }
                else if(featuresNext[i].x < (rect.x + 3*rect.width/3)){
                    movesY[2] += (featuresNext[i].y - features[i].y);
                    movesX[2] += (featuresNext[i].x - features[i].x);
                    movesCount[2]++;
                }
            }
            else if(featuresNext[i].y < (rect.y + 2*rect.height/3)){
                if(featuresNext[i].x < (rect.x + rect.width/3)){
                    movesY[3] += (featuresNext[i].y - features[i].y);
                    movesX[3] += (featuresNext[i].x - features[i].x);
                    movesCount[3]++;
                }
                else if(featuresNext[i].x < (rect.x + 2*rect.width/3)){
                    movesY[4] += (featuresNext[i].y - features[i].y);
                    movesX[4] += (featuresNext[i].x - features[i].x);
                    movesCount[4]++;
                }
                else if(featuresNext[i].x < (rect.x + 3*rect.width/3)){
                    movesY[5] += (featuresNext[i].y - features[i].y);
                    movesX[5] += (featuresNext[i].x - features[i].x);
                    movesCount[5]++;
                }
            }
            else if(featuresNext[i].y < (rect.y + 3*rect.height/3)){
                if(featuresNext[i].x < (rect.x + rect.width/3)){
                    movesY[6] += (featuresNext[i].y - features[i].y);
                    movesX[6] += (featuresNext[i].x - features[i].x);
                    movesCount[6]++;
                }
                else if(featuresNext[i].x < (rect.x + 2*rect.width/3)){
                    movesY[7] += (featuresNext[i].y - features[i].y);
                    movesX[7] += (featuresNext[i].x - features[i].x);
                    movesCount[7]++;
                }
                else if(featuresNext[i].x < (rect.x + 3*rect.width/3)){
                    movesY[8] += (featuresNext[i].y - features[i].y);
                    movesX[8] += (featuresNext[i].x - features[i].x);
                    movesCount[8]++;
                }
            }
        }
    }
}

float Blinker::getVariance(vector<float> vec){
    float average = 0;
    float sum = 0;

    for(int i = 0; i < vec.size(); i++){
        average += vec[i];
    }

    average /= vec.size();

    for(int i = 0; i < vec.size(); i++){
        sum += ((average - vec[i])*(average - vec[i]));
    }

    return sum / vec.size();
}

bool Blinker::getStatus(vector<float> current, vector<int> currentCount, vector<float> previous, vector<int> previousCount, int distance, int &state, int &blinkFrame, float fpsN){
    vector<float> currentMoves(9, 0);
    vector<float> previousMoves(9, 0);
    vector<float> change(6, 0);

    //for(int i = 0; i < currentMoves.size(); i++){
    for(int i = 0; i < 6; i++){
        currentMoves[i] = current[i] / currentCount[i];
        previousMoves[i] = previous[i] / previousCount[i];
        change[i] = currentMoves[i] - previousMoves[i];
        //cout << current[i] << endl;
        //cout << previous[i] << endl;
    }

    float threshold;

    threshold = distance * 0.00068 * (30/fpsN); //otestuje toto este!!!

    /*
    if(print){
        for(int i = 0; i < change.size(); i++){
            cout << (int) roundf(change[i] * 1000) << " ";
            //cout << change[i] << " ";
            if(i % 3 == 2){
                cout << endl;
            }
        }
    }
    */

    //cout << "num " << blinkFrame << endl;

    float upper = change[1];
    float lower = change[4];

    if(state == 0){
        //if(change[4] - change[1] > 0 && (change[4] > 0 || change[1] > 0) && getVariance(change) > threshold){
        if(lower - upper > 0 && lower > 0 && getVariance(change) > threshold){
            //if(change[4] - change[1] > 0 && change[4] > 0 && (change[4] > change[1] + distance*0.002) && getVariance(change) > threshold){
            if(print){cout << "--------------------------DOLE" << endl;}
            state = 1;
            blinkFrame++;
        }
    }
    else if(state == 1){
        blinkFrame++;

        if(blinkFrame < 3){
            state = 1;
            if(print){cout << "-------skoro" << endl;}
        }
        else if(blinkFrame > 4){
            state = 0;
            blinkFrame = 0;
            if(print){
                cout << "-------fail" << endl;
            }
        }
        else if(lower - upper < 0 && lower < 0 && getVariance(change) > threshold){
            //else if(lower - upper < 0 && lower < 0 && (fabs(upper) < fabs(lower)) && getVariance(change) > threshold){
            //else if(change[4] - change[1] < 0 && change[4] < 0 && (change[4] < change[1] - distance*0.002) && getVariance(change) > threshold){
            if(print){cout << "--------------------------HORE" << endl;}
            state = 2;
        }
    }

    /*
    if(print){
        cout << getVariance(change) << " " << threshold << " | " << distance << " v " << state << endl;
        cout << "------------------ " << endl;
    }
    */

    if(state == 2){
        state = 0;
        blinkFrame = 0;
        if(print){cout << "-----------------------------------------------BLINK" << endl;}
        //makeSound();
        return true;
    }

    return false;
}

void Blinker::drawFrame(){
    //cout << leftFeaturesNext.size() << " " << rightFeaturesNext.size() << " - " << leftFinalPoints.size() << " " << rightFinalPoints.size() << endl;

    if(leftFinalPoints.size() != 0 && rightFinalPoints.size() != 0){
        //final thigns---------------------------------------------------------
        for(int i = 0; i < leftFeaturesNext.size(); i++){
            circle(frame, leftFeaturesNext[i], 1, Scalar(0, 0, 0), 1, 8);
        }

        for(int i = 0; i < rightFeaturesNext.size(); i++){
            circle(frame, rightFeaturesNext[i], 1, Scalar(0, 0, 0), 1, 8);
        }

        for(int i = 0; i < leftFinalPoints.size(); i++){
            circle(frame, leftFinalPoints[i], 1, Scalar(255, 255, 255), 1, 8);
        }

        for(int i = 0; i < rightFinalPoints.size(); i++){
            circle(frame, rightFinalPoints[i], 1, Scalar(255, 255, 255), 1, 8);
        }

        //circle(frame, leftCenter, 5, Scalar(0, 0, 0), 1, 8);
        //circle(frame, rightCenter, 5, Scalar(0, 0, 0), 1, 8);

        rectangle(frame, leftRect, Scalar(0, 0, 0), 1, 8);
        rectangle(frame, rightRect, Scalar(0, 0, 0), 1, 8);
    }
}

void Blinker::reinitCond(){
    //reinitialization
    if(leftLost > leftFeaturesNumber/2 || rightLost > rightFeaturesNumber/2 || leftFinalPoints.size() < 20 || rightFinalPoints.size() < 20){
        init = true;
        return;
    }

    nonblinkCount++;
    blinkCount++;
    reinitCount++;

    if(reinitCount == 3){
        init = true;
        blinkFramesLeft = 0;
        blinkFramesRight = 0;
        stateLeft = 0;
        stateRight = 0;
        return;
    }

    if(nonblinkCount % 100 == 0){
        init = true;
        nonblinkCount = 0;
        return;
    }
}

void Blinker::resetVariables(){
    leftFinalPoints.clear();
    rightFinalPoints.clear();

    swap(leftFeatures, leftFeaturesNext);
    swap(rightFeatures, rightFeaturesNext);

    leftLost = 0;
    rightLost = 0;

    swap(movesLeftX, movesLeftXPrev);
    swap(movesLeftY, movesLeftYPrev);
    swap(movesRightX, movesRightXPrev);
    swap(movesRightY, movesRightYPrev);
    swap(movesLeftCount, movesLeftCountPrev);
    swap(movesRightCount, movesRightCountPrev);

    movesLeftY.assign(9, 0.0);
    movesLeftX.assign(9, 0.0);
    movesLeftCount.assign(9, 0);
    movesRightY.assign(9, 0.0);
    movesRightX.assign(9, 0.0);
    movesRightCount.assign(9, 0);
}

void Blinker::sendFrame(){
    emit newFrameSignal(&frame);
}

void Blinker::sendRecordFrame(){
    emit newRecordFrameSignal(&frame);
}

void Blinker::setCap(QString string){
    capString = string;
}

void Blinker::stopCapture(){
    stop = true;
}

void Blinker::toggleSend(){
    if(send){
        send = false;

        frame = imread("./background.jpg", CV_LOAD_IMAGE_GRAYSCALE);
        sendFrame();
    }
    else{
        send = true;
    }
}

void Blinker::toggleRecord(){
    if(record){
        record = false;
    }
    else{
        record = true;
    }
}

void Blinker::manualReinit(){
    init = true;
}
