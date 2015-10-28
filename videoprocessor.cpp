#include "videoprocessor.h"


using namespace std;




VideoProcessor::VideoProcessor(QObject * parent) :QThread(parent)
{
    playFlagRemember=playFlag=0;

    numberOfFrames=0;


    frameStep=1;
    minSizeInPercents=1;
    threshold_val=2;

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
/**
 * @brief VideoProcessor::loadVideo Loads video into cap videocapture descriptor
 * @param fname path to file
 * @return
 */
bool VideoProcessor::loadVideo(QString fname)

{
    pause();
    //numberOfFrames = cap.get(CV_CAP_PROP_FRAME_COUNT); //gives inaccurate result
    numberOfFrames=getRealFrCount(fname)*1000;
    cap = cv::VideoCapture  (fname.toStdString().c_str());

    if (!cap.isOpened()) return false;
    if (!cap.read(frame)) return false;

    emit showFrame(putImage(frame));
    frameRate = (int) cap.get(CV_CAP_PROP_FPS);
    emit emitCurrentPosition(0);


    getAnchorFrame();
    emit emitCurrentPosition(0);


    return true;
}


/**
 * @brief VideoProcessor::getRealFrCount
 * As cp.get(CV_CAP_PROP_FRAME_COUNT) gives us inaccurate result,
 * we have to get it with this slow func.
 * @param fname
 * @return
 */
double VideoProcessor::getRealFrCount(QString fname)

{

    cv::VideoCapture  cp (fname.toStdString().c_str());
    double numberOfFrames = cp.get(CV_CAP_PROP_FRAME_COUNT)/1000;
    cap.set(CV_CAP_PROP_POS_FRAMES, numberOfFrames);
    if (!cp.isOpened()) return 0;
    while (cp.grab());
    double fc = cp.get(CV_CAP_PROP_POS_FRAMES);
    cp.release();
    return fc;

}


/**
 * @brief VideoProcessor::putImage
 * cv::Mat to QImage coder
 * @param mat
 * @return QImage
 */
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
        emit playError("Error in conversion to QImage");
        return QImage();
    }

}


/**
 * @brief VideoProcessor::play
 * Strart playing
 * @return
 */
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

/**
 * @brief VideoProcessor::set_frameStep
 * motion detection parameter frame step setter
 * @param step
 */
void VideoProcessor::set_frameStep(int step)
{
    frameStep = step;
}

/**
 * @brief VideoProcessor::set_minSizeInPercents
 * motion detection parameter frame min size setter
 * @param val
 */
void VideoProcessor::set_minSizeInPercents (int val)
{
    minSizeInPercents = val;
}


/**
 * @brief VideoProcessor::set_minSizeInPercents
 * motion detection parameter treshold setter
 * @param val
 */
void VideoProcessor::set_threshold(int val)
{
    threshold_val = val;
}


/**
 * @brief VideoProcessor::run
 * main cycle of the thread: while cycle inside
 */
void VideoProcessor::run()
{
    int delay = (1000/frameRate);
       while(playFlag){
           double currentFramePosition = 1000*cap.get(CV_CAP_PROP_POS_FRAMES);

           if (currentFramePosition>numberOfFrames)
               emit emitCurrentPosition(1000); //1000 is agreed max position
           else
               emit emitCurrentPosition(int(1000*currentFramePosition/numberOfFrames)); //1000 is agreed max position

           //motion detection
           int erosion_size=2;
           cv::Mat grayImage2, differnceImage, thresholdImage;
           cv::RNG rng(12345);
           cv::Mat element = getStructuringElement( cv::MORPH_RECT,cv::Size( 2*erosion_size + 1, 2*erosion_size+1 ),cv::Point( erosion_size, erosion_size ) );
           cv::Mat threshold_output;

           if (!cap.read(frame)) //consider video ended
           {
               pause();
               seek0();
               emit videoEnded();
               return;
           }

            /*Anchor frames taken:
             * +every frameStep frames while playing
             * +on the video loading
             * +if we seek position - on release
             * */
           if ((std::fmod(currentFramePosition/1000,frameStep)==0)||(currentFramePosition<2000)) getAnchorFrame();


           cv::cvtColor(frame,grayImage2,CV_RGB2GRAY);
           cv::blur(grayImage2,grayImage2,cv::Size(21,21));

           cv::absdiff(grayImage1,grayImage2,differnceImage);
           cv::threshold(differnceImage,thresholdImage,threshold_val,255,CV_THRESH_BINARY);

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
               if (boundRect[i].area()>videoArea*minSizeInPercents*.01)
               cv::rectangle( frame, boundRect[i].tl(), boundRect[i].br(), color, 2, 8, 0 );
             }

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


void VideoProcessor::seek0()
{
    seek(0);
    emit emitCurrentPosition(0);
}

/**
 * @brief VideoProcessor::getAnchorFrame
 * anchor frame - is a frame, based on which we detect movement
 */
void VideoProcessor::getAnchorFrame()
{
    frame1=frame.clone();
    cv::cvtColor(frame1,grayImage1,CV_RGB2GRAY);
    cv::blur(grayImage1,grayImage1,cv::Size(50,50));
}


/**
 * @brief VideoProcessor::seek
 * put cap into desired frame index
 * @param position number of frame(index)
 */
void VideoProcessor::seek (unsigned int position)
{
    if (cap.isOpened()) cap.set(CV_CAP_PROP_POS_FRAMES, numberOfFrames*position/1000000); else return;
    playFlag=0;

    if (!cap.read(frame))
    {
        emit playError("No way to read a frame at this position");
        return;
    }
    emit showFrame(putImage(frame));
    getAnchorFrame();

}


