#ifndef VIDEOPROCESSOR_H
#define VIDEOPROCESSOR_H


#include <QObject>

#include "opencv/cv.h"

#include "opencv2/core/core.hpp"

#include "opencv2/highgui/highgui_c.h"

#include <opencv2/highgui/highgui.hpp>

#include <QImage>

#include <QRgb>

#include <stdlib.h>
#include <stdio.h>

#include <QMutex>
#include <QThread>
#include <QImage>
#include <QWaitCondition>

#include <QDebug>

#include <iostream>
#include <fstream>

class VideoProcessor: public QThread
{
    Q_OBJECT

public:
    VideoProcessor(QObject * parent);
    ~VideoProcessor();

    bool loadVideo(QString fname); //errors to be handled as exceptions
    bool play ();
    void pause();

    bool isPlaying();

    void seek0();
    void seek (unsigned int position);

    //setting motion recognition parameters
    void set_frameStep(int step);
    void set_minSizeInPercents (int val);
    void set_threshold(int val);

    bool playFlagRemember;




private:
    cv::VideoCapture cap;
    QImage putImage(const cv::Mat& mat);
    void mainCycle();
    bool playFlag;
    QMutex mutex;
    QWaitCondition condition;
    int frameRate;
    double numberOfFrames;
    cv::Mat frame1, grayImage1, frame;

    double getRealFrCount(QString fname);


    //motion recognition parameters
    unsigned int frameStep; //'base' frame taken every frameStep frames 2-100
    unsigned int minSizeInPercents; //min size of motion area to be examined 2-90%
    unsigned int threshold_val; //theshold on diff image




protected:
    void run();
    void msleep(unsigned long ms);


private slots:
    void play_slot();






signals:

    void showFrame(QImage img);
    void videoEnded();
    void playError(QString err);
    void emitCurrentPosition(int pos);






};

#endif // VIDEOPROCESSOR_H
