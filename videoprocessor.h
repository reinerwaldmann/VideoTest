#ifndef VIDEOPROCESSOR_H
#define VIDEOPROCESSOR_H


#include <QObject>

#include "opencv/cv.h"

#include "opencv2/core/core.hpp"

#include "opencv2/highgui/highgui_c.h"

#include <opencv2/highgui/highgui.hpp>

#include <QImage>

#include <QRgb>




class VideoProcessor: public QObject
{
    Q_OBJECT

public:
    VideoProcessor();

    bool loadVideo(QString fname); //errors to be handled as exceptions
    bool play ();
    void pause();
    void seek0();
    void seek (unsigned int position);

private:
    cv::VideoCapture cap;

    QImage putImage(const cv::Mat& mat);





};

#endif // VIDEOPROCESSOR_H
