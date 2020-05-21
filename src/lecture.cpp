#include <opencv2/opencv.hpp>
#include <vector>
#include <ctime>
#include <time.h>
#include <stdio.h>
#include <iostream>


using namespace std;
using namespace cv;

void draw_rect(Mat& img, vector<Rect>& v_rect);

int main()
{

    int th = 90;
    const int th_max = 255;
    int th_area;
    float detection_area = 0.001;
    const float alpha = 0.01; // back ground udate speed
    const float beta = (1.0 - alpha);



    // Make mask
    Mat element = getStructuringElement(MORPH_RECT, Size(5,5), Point(2,2));

    Mat frame, frame_binary;
    Mat old_frame, bg_binary_frame;
    Mat sub_frame, diff_img;

    VideoCapture stream1(0);

    if(!stream1.isOpened())
    {
        cout << "please open camera" << endl;
    }

    namedWindow("frame",0);
    namedWindow("sub_frame",0);
    
    int count = 0;
    const int skip = 50;

    while(1)
    {
        if(!(stream1.read(frame)))
        {
            break;
        }
        

        if(old_frame.empty())
        {
            old_frame = frame.clone();
            cvtColor(old_frame,bg_binary_frame,COLOR_BGR2GRAY);
            th_area = int(old_frame.cols*old_frame.rows) * detection_area;
            continue;
        }
        cvtColor(frame, frame_binary,COLOR_BGR2GRAY);
        addWeighted(frame_binary, alpha, bg_binary_frame, beta, 0.0, bg_binary_frame);

        count++;
        if(count < skip)
        {
            cout << count << endl;
            continue;
        }
 
        //subtract(bg_binary_frame, frame_binary,sub_frame);
        absdiff(bg_binary_frame,frame_binary,sub_frame);
        threshold(sub_frame,sub_frame,th,th_max,THRESH_BINARY);
        //Morphology
        morphologyEx(sub_frame, sub_frame, MORPH_CLOSE, element);

        //contour
        vector<vector<Point>> contours;
        vector<Vec4i> hierarchy;
        findContours(sub_frame.clone(), contours, hierarchy, RETR_CCOMP, CHAIN_APPROX_SIMPLE);
        //drawContours(frame, contours, -1, Scalar(255,0,0), 5, 8, hierarchy);

        //blob
        vector<Rect> v_rect;
        for(auto it : contours)
        {   
            Rect mr = boundingRect(Mat(it));
            double area = contourArea(it,false);
            if(area > th_area)
            {
                v_rect.push_back(mr);
            }
            
        }
        draw_rect(frame,v_rect);

        //save write file
        if(v_rect.size()>0)
        {
            time_t     now = time(0); //현재 시간을 time_t 타입으로 저장
            struct tm  tstruct;
            char       buf[80];
            tstruct = *localtime(&now);
            strftime(buf, sizeof(buf), "%Y/%m/%d.%X.jpg", &tstruct); // yyyy/mm/dd.HH:mm:ss
            cout << buf << endl;
            //imwrite(buf,frame);
        }


        imshow("frame",frame);
        imshow("sub_frame",sub_frame);
        //imshow("absdiff_frame",absdiff_frame);

        //old_frame - frame.clone();

        if(waitKey(5) >= 0)
        {
            break;
        }
    }
    return 0;
}


void draw_rect(Mat& img, vector<Rect>& v_rect)
{
    for(auto it : v_rect)
    {
        rectangle(img,it,Scalar(255,0,0),2);
    }
}