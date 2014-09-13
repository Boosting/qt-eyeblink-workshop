#-------------------------------------------------
#
# Project created by QtCreator 2014-05-07T11:41:07
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = eyeBLINK
TEMPLATE = app

INCLUDEPATH += /opt/opencv-2.4.9/include/opencv/
LIBS += -L/opt/opencv-2.4.9/build/lib/ \
-lopencv_core \
-lopencv_imgproc \
-lopencv_highgui \
-lopencv_legacy \
-lopencv_ml \
-lopencv_video \
-lopencv_features2d \
-lopencv_calib3d \
-lopencv_objdetect \
-lopencv_nonfree \
-lopencv_contrib \
-lopencv_legacy \
-lopencv_flann

SOURCES +=\
        mainwindow.cpp \
    main.cpp \
    matcv.cpp \
    blinker.cpp

HEADERS  += mainwindow.h \
    cvimagewidget.h \
    matcv.h \
    blinker.h

FORMS    += mainwindow.ui
