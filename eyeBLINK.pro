#-------------------------------------------------
#
# Project created by QtCreator 2014-05-07T11:41:07
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = eyeBLINK
TEMPLATE = app


#LIBS += -LC:/opencv/build/qt/lib \
#-lopencv_core246 \
#-lopencv_imgproc246 \
#-lopencv_highgui246 \
#-lopencv_legacy246 \
#-lopencv_ml246 \
#-lopencv_video246 \
#-lopencv_features2d246 \
#-lopencv_calib3d246 \
#-lopencv_objdetect246 \
#-lopencv_contrib246 \
#-lopencv_flann246 \
#-lopencv_nonfree246

LIBS += -LC:\\opencv\\build\\qt\\bin \
    libopencv_core246 \
    libopencv_highgui246 \
    libopencv_imgproc246 \
    libopencv_features2d246 \
    libopencv_calib3d246 \
    libopencv_legacy246 \
    libopencv_ml246 \
    libopencv_video246 \
    libopencv_objdetect246 \
    libopencv_contrib246 \
    libopencv_flann246 \
    libopencv_nonfree246 \


INCLUDEPATH += "C:/opencv/build/include"

SOURCES +=\
        mainwindow.cpp \
    main.cpp \
    matcv.cpp \
    blinker.cpp \
    recorder.cpp

HEADERS  += mainwindow.h \
    cvimagewidget.h \
    matcv.h \
    blinker.h \
    recorder.h

FORMS    += mainwindow.ui
