#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QWidget>
#include <cvimagewidget.h>
#include <matcv.h>
#include <qfiledialog.h>
#include <QMessageBox>
#include <iostream>
#include <QString>
#include <opencv/cv.h>
#include <QTime>
#include <QTimer>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

public slots:
    void informationBox1();
    void changeRecordLabel();
    void updateTime();
    void updateRecImage();

signals:
    void finished();    //koniec GUI
    void startCaptureSignal();  //chcem zapnut blinkera
    void stopCaptureSignal();  //chcem vypnut blinkera
    void sendPushed();  //chcem zmenit zasielanie/nezasielanie frejmu na obrazovku
    void reinitSignal();    //chcem manualne reinicializovat
    void startRecordSignal();    //chcem nahravat
    void stopRecordSignal();    //chcem prestat nahravat
    void folderSelectedSignal(QString);

private slots:  //gui sloty
    void on_pushButton_clicked();
    void on_pushButton_2_clicked();
    //void showMat(Matcv *mat);
    void showMat(cv::Mat *mat);
    void printBlink();
    void on_pushButton_3_clicked();
    void on_pushButton_5_clicked();
    void on_pushButton_6_clicked();
    void on_pushButton_7_clicked();

private:
    Ui::MainWindow *ui;
    QTimer *timer;
    QTimer *recTimer;
};

#endif // MAINWINDOW_H
