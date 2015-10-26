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


private:
    cv::VideoCapture cap;
    QImage putImage(const cv::Mat& mat);
    void mainCycle();
    bool playFlag;
    QMutex mutex;
    QWaitCondition condition;
    int frameRate;
    double numberOfFrames;

    cv::Mat frame;//debug




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
