#include "matcv.h"

using namespace cv;

Matcv::Matcv(){
}

Matcv::~Matcv(){
}

Matcv::Matcv(const Matcv &other){

}

Matcv::Matcv(Mat temp){
    matrix = temp;
}

Mat Matcv::getMatrix(){
    return matrix;
}

void Matcv::setMatrix(Mat temp){
    matrix = temp;
    //temp.copyTo(matrix);
}

void Matcv::print(){
    std::cout << matrix.rows << std::endl;
    std::cout << matrix << std::endl;
}
