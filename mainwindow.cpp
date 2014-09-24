#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow){
    ui->setupUi(this);

    timer = new QTimer(this);
    timer->start(1000);
    QObject::connect(timer, SIGNAL(timeout()), this, SLOT(updateTime()));

    recTimer = new QTimer(this);
    QObject::connect(recTimer, SIGNAL(timeout()), this, SLOT(updateRecImage()));

    blinkTimer = new QTimer(this);

    ui->label_4->setPixmap(QPixmap("./rec.png"));
    ui->label_4->hide();
    //ui->label_4->show();

    ui->label_5->hide();
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
        recTimer->start(1000);
    }
    else if(ui->pushButton_6->text().compare(QString("STOP recording")) == 0){
        ui->pushButton_6->setText(QString("RECORD"));
        recTimer->stop();
        ui->label_4->hide();
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

void MainWindow::updateRecImage(){
    if(ui->label_4->isHidden()){
        ui->label_4->show();
    }
    else{
        ui->label_4->hide();
    }
}

void MainWindow::eraseTable(){
    ui->tableWidget->setRowCount(0);
}

void MainWindow::updateTable(int minute){
    std::cout << minute << std::endl;

    if(ui->tableWidget->rowCount() != minute || ui->tableWidget->rowCount() == 0){
        ui->tableWidget->setRowCount(minute);
        ui->tableWidget->setItem(minute - 1, 0, new QTableWidgetItem(QString(QString::number(minute).append(". minute"))));
        ui->tableWidget->setItem(minute - 1, 1, new QTableWidgetItem(QString(QString::number(1  ))));
    }
    else{
        int number = ui->tableWidget->item(minute - 1, 1)->text().toInt();
        number++;
        ui->tableWidget->setItem(minute - 1, 1, new QTableWidgetItem(QString(QString::number(number))));
    }
}

void MainWindow::showBlinkExecute(){
    ui->label_5->show();
    blinkTimer->singleShot(1000, this, SLOT(showBlinkText()));
}

void MainWindow::showBlinkText(){
    ui->label_5->hide();
}
