#include "videoprocessor.h"

VideoProcessor::VideoProcessor()
{
}


bool VideoProcessor::loadVideo(QString fname)
{
    cap = cv::VideoCapture  (fname.toStdString().c_str());
       return cap.isOpened();






} //errors to be handled as exceptions

bool VideoProcessor::play ()
{
    if (!cap.isOpened()) return 0;

}

void VideoProcessor::pause()
{}

void VideoProcessor::seek0(){}

void VideoProcessor::seek (unsigned int position){}


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
        //throw an exception

        return QImage();
    }
}
