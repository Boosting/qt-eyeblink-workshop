#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <iostream>
#include <QString>
#include <opencv/cv.h>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow){
    ui->setupUi(this);
}

MainWindow::~MainWindow(){
    delete ui;
}

void MainWindow::on_pushButton_clicked(){
    //std::cout << "klikol som" << std::endl;
    //toto by chcelo zmenit, netreba tu tento slot
    emit startCaptureSignal();
}

void MainWindow::setFrameNumber(int value){
    QString str = QString::number(value);
    ui->frameNumberLabel->setText(str);
}

void MainWindow::on_pushButton_2_clicked(){
    close();
    emit finished();
}

void MainWindow::labelPrint(int value){
    ui->frameNumberLabel->setText(QString::number(value));
}

/*
void MainWindow::showMat(Matcv *mat){
    //std::cout << "tu som " << std::endl;
    //mat->print();
    //cv::Mat temp = mat->getMatrix();
    //cv::imshow("frame", temp);
    ui->cvWidget->showImage(mat->getMatrix());

    //we need to free memory here - after image is showed
    if(mat){
        delete(mat);
        mat = NULL;
    }
}
*/

void MainWindow::showMat(cv::Mat *mat){
    //std::cout << "tu som " << std::endl;
    //mat->print();
    //cv::Mat temp = mat->getMatrix();
    //cv::imshow("frame", temp);
    ui->cvWidget->showImage(*mat);

    //we need to free memory here - after image is showed
    /*
    if(mat){
        delete(mat);
        mat = NULL;
    }
    */
}

void MainWindow::printBlink(){
    std::cout << "BLINK!" << std::endl;
}

void MainWindow::on_pushButton_3_clicked(){
    emit sendPushed();
}

void MainWindow::on_pushButton_4_clicked(){
    emit stopCaptureSignal();
}

void MainWindow::on_pushButton_5_clicked(){
    emit reinitSignal();
}

void MainWindow::on_pushButton_6_clicked(){
    emit recordSignal();
}
