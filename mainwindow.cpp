#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow){
    ui->setupUi(this);

    timer = new QTimer(this);
    timer->start(1000);
    QObject::connect(timer, SIGNAL(timeout()), this, SLOT(updateTime()));
}

MainWindow::~MainWindow(){
    delete ui;
}

void MainWindow::on_pushButton_clicked(){
    if(ui->pushButton->text().compare(QString("START tracking")) == 0){
        ui->pushButton->setText(QString("STOP tracking"));
        emit startCaptureSignal();
    }
    else if(ui->pushButton->text().compare(QString("STOP tracking")) == 0){
        ui->pushButton->setText(QString("START tracking"));
        emit stopCaptureSignal();
    }
}

void MainWindow::on_pushButton_2_clicked(){
    close();
    emit finished();
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
    if(ui->radioButton->isChecked()){
        QApplication::beep();
    }
}

void MainWindow::on_pushButton_3_clicked(){
    emit sendPushed();
}

void MainWindow::on_pushButton_5_clicked(){
    emit reinitSignal();
}

void MainWindow::on_pushButton_6_clicked(){
    if(ui->pushButton_6->text().compare(QString("RECORD")) == 0){
        std::cout << "zacinam" << std::endl;
        emit startRecordSignal();
    }
    else if(ui->pushButton_6->text().compare(QString("STOP recording")) == 0){
        std::cout << "koncim" << std::endl;
        emit stopRecordSignal();
    }
}

void MainWindow::changeRecordLabel(){
    if(ui->pushButton_6->text().compare(QString("RECORD")) == 0){
        ui->pushButton_6->setText(QString("STOP recording"));
    }
    else if(ui->pushButton_6->text().compare(QString("STOP recording")) == 0){
        ui->pushButton_6->setText(QString("RECORD"));
    }
}

void MainWindow::on_pushButton_7_clicked(){
    QFileDialog dialog(this);

    dialog.setFileMode(QFileDialog::Directory);
    dialog.setViewMode(QFileDialog::Detail);
    dialog.setNameFilter("directories");

    //start open file dialog
    QStringList fileNames;
    if(dialog.exec()){
        fileNames = dialog.selectedFiles();
        ui->label_2->setText(fileNames[0]);
        emit folderSelectedSignal(ui->label_2->text());
    }
}

void MainWindow::informationBox1(){
    QMessageBox::information(0, "Select folder", "Please select save folder first.");
}

void MainWindow::updateTime(){
    QString text = QTime::currentTime().toString();
    ui->label->setText(text);
}
