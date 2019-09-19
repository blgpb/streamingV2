#ifndef _CAMERA_NODE_
#define _CAMERA_NODE_

#include <iostream>
#include <unistd.h>
#include <string.h>
#include <ctime>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <chrono>

#include <thread>
#include <mutex>

using namespace cv;
using namespace std;
using namespace std::chrono;

double intervalMs(high_resolution_clock::time_point end, high_resolution_clock::time_point start);

typedef struct{
    int index;
    int width;
    int height;
    vector<double> focal_length;
    vector<double> principle_point;
} CameraConfig;

class CameraNode{
    public:
        int width, height;
        double fps;
        CameraNode( int id, int w, int h );
        bool isOpened( void );
        bool read( Mat& frame );
        bool compress( vector<uchar>& img_buffer, Mat frame );
        void release( void );
    private:
        vector< int > quality{ CV_IMWRITE_JPEG_QUALITY, 50};
        double interval;
        bool OPENED;
        VideoCapture cap;
        mutex cap_mutex;
        high_resolution_clock::time_point last_grab;
        void updating( void );
};

#endif