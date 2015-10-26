#include "videoprocessor.h"
/*

TODO
*errors to be handled as exceptions
*/

using namespace std;

VideoProcessor::VideoProcessor(QObject * parent) :QThread(parent)
{
    playFlag=0;
    numberOfFrames=0;

}

VideoProcessor::~VideoProcessor()
{
       mutex.lock();
       playFlag=0;
       cap.release();
       condition.wakeOne();
       mutex.unlock();
       wait();

}

bool VideoProcessor::loadVideo(QString fname)
{
    pause();
    cap = cv::VideoCapture  (fname.toStdString().c_str());

    if (!cap.isOpened()) return false;

    cv::Mat  frame1;

    if (!cap.read(frame1)) return false;

    emit showFrame(putImage(frame1));
    frameRate = (int) cap.get(CV_CAP_PROP_FPS);

    emit emitCurrentPosition(0);

    numberOfFrames = (unsigned long) cap.get(CV_CAP_PROP_FRAME_COUNT);

    return true;


} //errors to be handled as exceptions


QImage VideoProcessor::putImage(const cv::Mat& mat)
{
    // 8-bits unsigned, NO. OF CHANNELS=1
    if(mat.type()==CV_8UC1)
    {
        // Set the color table (used to translate colour indexes to qRgb values)
        QVector<QRgb> colorTable;
        for (int i=0; i<256; i++)
            colorTable.push_back(qRgb(i,i,i));
        // Copy input Mat
        const uchar *qImageBuffer = (const uchar*)mat.data;
        // Create QImage with same dimensions as input Mat
        QImage img(qImageBuffer, mat.cols, mat.rows, mat.step, QImage::Format_Indexed8);
        img.setColorTable(colorTable);
        return img;
    }
    // 8-bits unsigned, NO. OF CHANNELS=3
    if(mat.type()==CV_8UC3)
    {
        // Copy input Mat
        const uchar *qImageBuffer = (const uchar*)mat.data;
        // Create QImage with same dimensions as input Mat
        QImage img(qImageBuffer, mat.cols, mat.rows, mat.step, QImage::Format_RGB888);
        return img.rgbSwapped();
    }
    else
    {
        //qDebug() << "ERROR: Mat could not be converted to QImage.";
        //TODO: throw an exception

        return QImage();
    }

}


void VideoProcessor::mainCycle()
{
    int erosion_size=2;
    cv::Mat frame1,frame2,
            grayImage1,grayImage2,
            differnceImage, thresholdImage, threshold_output,
            element = cv::getStructuringElement( cv::MORPH_RECT,cv::Size( 2*erosion_size + 1, 2*erosion_size+1 ),cv::Point( erosion_size, erosion_size ) );

    cv::RNG rng(12345);



    int step=20, i=step;
    do
    {
               if(!cap.isOpened())
                 {
                   emit playError("Video source not opened");
                 }


                if (i==step)
                {
                    cap.read(frame1);

                    cv::cvtColor(frame1,grayImage1,CV_RGB2GRAY);
                    //blur(grayImage1,grayImage1,Size(21,21));
                    cv::blur(grayImage1,grayImage1,cv::Size(50,50));

                    i=0;
                }
                i++;

                if (!cap.read(frame2))
                {
                    emit videoEnded();
                    break;

                }


                cvtColor(frame2,grayImage2,CV_RGB2GRAY);
                blur(grayImage2,grayImage2,cv::Size(21,21));

                absdiff(grayImage1,grayImage2,differnceImage);
                threshold(differnceImage,thresholdImage,32,255,CV_THRESH_BINARY);


                //erode(thresholdImage,thresholdImage,element,Point(-1,-1));

              //  cv::Mat kernel;
               // cv::getStructuringElement(cv::MORPH_RECT, cv::Size(20,20));
                //cv::morphologyEx(thresholdImage, thresholdImage, cv::MORPH_CLOSE, kernel, cv::Point(-1,-1));

               //dilate(thresholdImage,thresholdImage,element,Point(-1,-1), 2);

               //thresh = cv2.dilate(thresh, None, iterations=2)

               vector<vector<cv::Point> > contours;
               vector<cv::Vec4i> hierarchy;

               //findContours(thresholdImage.clone(), contours, hierarchy, CV_RETR_TREE, CV_CHAIN_APPROX_SIMPLE, Point(0, 0) );
               cv::findContours(thresholdImage.clone(), contours, hierarchy, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE, cv::Point(0, 0) );


               vector<vector<cv::Point> > contours_poly( contours.size() );
               vector<cv::Rect> boundRect( contours.size() );
               vector<cv::Point2f>center( contours.size() );
               vector<float>radius( contours.size() );

               unsigned int videoArea = frame1.rows*frame1.cols;

               for( int i = 0; i < contours.size(); i++ )
                  {
                   cv::approxPolyDP( cv::Mat(contours[i]), contours_poly[i], 3, true );
                   boundRect[i] = cv::boundingRect( cv::Mat(contours_poly[i]) );

                    //minEnclosingCircle( (Mat)contours_poly[i], center[i], radius[i] );

                    cv::Scalar color = cv::Scalar( 0,255,0);

                    if (boundRect[i].area()>videoArea*.01)

                    cv::rectangle( frame2, boundRect[i].tl(), boundRect[i].br(), color, 2, 8, 0 );


                  }

               emit showFrame(putImage(frame2));
               //imshow("Theshold",thresholdImage);


               //imshow("Video",frame2);



    }
    while (playFlag);

//cap.release();

}


bool VideoProcessor::play ()

{
    if (!cap.isOpened()) return false;

    if (!isRunning())
    {
        if (!isPlaying())
        {
            playFlag=true;
        }
           start(LowPriority);
           return 1;
    }
    return 0;
}

void VideoProcessor::run()
{

    int delay = (1000/frameRate);
       while(playFlag){
           if (!cap.read(frame))
           {
               playFlag=0;
               emit videoEnded();

               cap.set(CV_CAP_PROP_POS_FRAMES, 0);
               if (!cap.read(frame))
               {
                   //some error
               }





           }
           double currentFramePosition = 1000*cap.get(CV_CAP_PROP_POS_FRAMES);

           if (currentFramePosition>numberOfFrames)
               emit emitCurrentPosition(1000); //1000 is agreed max position
           else
               emit emitCurrentPosition(int(1000*currentFramePosition/numberOfFrames)); //1000 is agreed max position

//           if (frame.channels()== 3){
//               cv::cvtColor(frame, RGBframe, CV_BGR2RGB);
//               img = QImage((const unsigned char*)(RGBframe.data),
//                                 RGBframe.cols,RGBframe.rows,QImage::Format_RGB888);
//           }
//           else
//           {
//               img = QImage((const unsigned char*)(frame.data),
//                                    frame.cols,frame.rows,QImage::Format_Indexed8);
//           }
           emit showFrame(putImage(frame));
           this->msleep(delay);
       }
}

bool VideoProcessor::isPlaying()
{
    return playFlag;

}


void VideoProcessor::pause()
{
    playFlag=0;
}

void VideoProcessor::msleep(unsigned long ms){
    struct timespec ts = { ms / 1000, (ms % 1000) * 1000 * 1000 };
    nanosleep(&ts, NULL);
}


void VideoProcessor::seek0(){}

void VideoProcessor::seek (unsigned int position){}

void VideoProcessor::play_slot()
{}
