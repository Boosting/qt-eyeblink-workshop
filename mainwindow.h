#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QWidget>
#include <cvimagewidget.h>
#include <matcv.h>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    void setFrameNumber(int value);

signals:
    void finished();    //koniec GUI
    void startCaptureSignal();  //chcem zapnut blinkera
    void stopCaptureSignal();   //chcem vypnut blinkera
    void sendPushed();  //chcem zmenit zasielanie/nezasielanie frejmu na obrazovku
    void reinitSignal();    //chcem manualne reinicializovat
    void recordSignal();    //chcem nahravat/prestat nahravat

private slots:  //gui sloty
    void on_pushButton_clicked();
    void on_pushButton_2_clicked();
    void labelPrint(int value);
    //void showMat(Matcv *mat);
    void showMat(cv::Mat *mat);
    void printBlink();
    void on_pushButton_3_clicked();
    void on_pushButton_4_clicked();
    void on_pushButton_5_clicked();
    void on_pushButton_6_clicked();

private:
    Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H
