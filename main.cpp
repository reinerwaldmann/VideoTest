#include "mainwindow.h"
#include <QApplication>


#include "opencv/cv.h"

#include "opencv2/core/core.hpp"


#include "opencv2/highgui/highgui_c.h"

#include <opencv2/highgui/highgui.hpp>
#include <stdlib.h>
#include <stdio.h>

using namespace cv;


int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    w.show();



    return a.exec();
}



int main1(int argc, char* argv[])
{

        const QString fn="example2.avi";
        const QString fn1="test.avi";

        VideoCapture cap(fn.toStdString().c_str());
        //CvCapture * capture = cvCaptureFromAVI("test.avi");



        int erosion_size=2;


        Mat frame1,frame2;
        Mat grayImage1,grayImage2;
        Mat differnceImage;
        Mat thresholdImage;

        RNG rng(12345);


        //namedWindow("Webcam Video");
        cvNamedWindow("Video"); //Создаем окно с названием image window

        IplImage* image;
        Mat element = getStructuringElement( MORPH_RECT,Size( 2*erosion_size + 1, 2*erosion_size+1 ),Point( erosion_size, erosion_size ) );

        Mat threshold_output;


        int step=20, i=step;
        do
        {
                   if(!cap.isOpened())
                     {
                         std::cout<<"Webcam can't be opened";
                         getchar();
                         return -1;
                     }



                    if (i==step)
                    {
                        cap.read(frame1);

                        cvtColor(frame1,grayImage1,CV_RGB2GRAY);
                        //blur(grayImage1,grayImage1,Size(21,21));
                        blur(grayImage1,grayImage1,Size(50,50));

                        i=0;
                    }
                    i++;

                    if (!cap.read(frame2))
                    {
                        break;

                     }


                    cvtColor(frame2,grayImage2,CV_RGB2GRAY);
                    blur(grayImage2,grayImage2,Size(21,21));

                    absdiff(grayImage1,grayImage2,differnceImage);
                    threshold(differnceImage,thresholdImage,32,255,CV_THRESH_BINARY);


                    //erode(thresholdImage,thresholdImage,element,Point(-1,-1));

                  //  cv::Mat kernel;
                   // cv::getStructuringElement(cv::MORPH_RECT, cv::Size(20,20));
                    //cv::morphologyEx(thresholdImage, thresholdImage, cv::MORPH_CLOSE, kernel, cv::Point(-1,-1));

                   //dilate(thresholdImage,thresholdImage,element,Point(-1,-1), 2);

                   //thresh = cv2.dilate(thresh, None, iterations=2)

                   vector<vector<Point> > contours;
                   vector<Vec4i> hierarchy;

                   //findContours(thresholdImage.clone(), contours, hierarchy, CV_RETR_TREE, CV_CHAIN_APPROX_SIMPLE, Point(0, 0) );
                   findContours(thresholdImage.clone(), contours, hierarchy, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE, Point(0, 0) );


                   vector<vector<Point> > contours_poly( contours.size() );
                   vector<Rect> boundRect( contours.size() );
                   vector<Point2f>center( contours.size() );
                   vector<float>radius( contours.size() );

                   unsigned int videoArea = frame1.rows*frame1.cols;

                   for( int i = 0; i < contours.size(); i++ )
                      {
                       approxPolyDP( Mat(contours[i]), contours_poly[i], 3, true );
                       boundRect[i] = boundingRect( Mat(contours_poly[i]) );

                        //minEnclosingCircle( (Mat)contours_poly[i], center[i], radius[i] );

                        Scalar color = Scalar( 0,255,0);

                        if (boundRect[i].area()>videoArea*.01)

                        rectangle( frame2, boundRect[i].tl(), boundRect[i].br(), color, 2, 8, 0 );


                      }




//                   Mat drawing = Mat::zeros( threshold_output.size(), CV_8UC3 );

//                    for( int i = 0; i< contours.size(); i++ )
//                       {
//                         //Scalar color = Scalar( rng.uniform(0, 255), rng.uniform(0,255), rng.uniform(0,255) );
//                         Scalar color = Scalar( rng.uniform(0, 255), rng.uniform(0,255), rng.uniform(0,255) );

//                         //drawContours( thresholdImage, contours_poly, i, color, 1, 8, vector<Vec4i>(), 0, Point() );
//                         rectangle( thresholdImage, boundRect[i].tl(), boundRect[i].br(), color, 2, 8, 0 );

//                       }
//                    namedWindow( "Contours", CV_WINDOW_AUTOSIZE );
//                      imshow( "Contours", drawing );



                   imshow("Theshold",thresholdImage);

                   imshow("Video",frame2);



        }
        while (cv::waitKey(30)<0);

        cap.release();


        destroyAllWindows();




}



void draw_motion_comp(Mat& img, int x_coordinate, int y_coordinate, int width, int height, double angle,Mat& result)
{
    rectangle(img,Point(x_coordinate,y_coordinate), Point(x_coordinate+width,y_coordinate+width), Scalar(255,0,0), 1, 8, 0);
    int r,cx,cy;
    if(height/2 <= width/2)
        r = height/2;
    else
        r = width/2;
    cx = x_coordinate + width/2;
    cy = y_coordinate + height/2;
    angle = angle*M_PI/180;
    circle(img, Point(cx,cy), r, Scalar(255,0,0),1, 8, 0);
    line(img, Point(cx,cy), Point(int(cx+cos(angle)*r), int(cy+sin(angle)*r)), Scalar(255,0,0), 1, 8, 0);
    result = img.clone();
}





