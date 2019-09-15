#ifndef _VIDEO_
#define _VIDEO_

#include <thread>
#include <mutex>
#include <string>
#include <atomic>

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

#include <iostream>

using namespace cv;
using namespace std;
using namespace chrono;

#define WRITE_VIDEO_WAIT_FOR_MS 50

string getCurrentTime( void );

class Video{
    public:
        Video( int width, int height, string path );
        void run( bool flag, Mat frame);
    private:
        atomic<bool> RECORDING = {false};
        VideoWriter video;
        timed_mutex write_mutex;
        int w, h;
        string video_recording_file_path;
        void tryWriteFrame( Mat frame );
};

#endif