#include "camera_node.hpp"

double intervalMs(high_resolution_clock::time_point end, high_resolution_clock::time_point start);

CameraNode::CameraNode( int id, int w, int h ){
    if ( ! cap.open( id ) ){
        cout << "[LOGGING]: fail to open camera" << endl;
        return;
    }
    //cap.set( CV_CAP_PROP_FOURCC, CV_FOURCC('M', 'J', 'P', 'G'));
    cap.set( CV_CAP_PROP_FOURCC, CV_FOURCC('Y', 'U', 'Y', '2'));
    cap.set( CV_CAP_PROP_FRAME_WIDTH, w);
    cap.set( CV_CAP_PROP_FRAME_HEIGHT, h);
    cap.set( CV_CAP_PROP_FPS, 30);
    //cout << cap.get(CV_CAP_PROP_BUFFERSIZE) << endl;
    width = cap.get( CV_CAP_PROP_FRAME_WIDTH );
    height = cap.get( CV_CAP_PROP_FRAME_HEIGHT);
    if ( width != w || height != h ){
        cout << "[LOGGING]: set camera resolution fail" << endl;
    }
    fps = cap.get(CV_CAP_PROP_FPS);
    interval = 1000.0 / fps;
    OPENED = true;
    for ( int i = 5; i > 0; i--)
        cap.grab();
    last_grab = high_resolution_clock::now();
    thread updating_thread( &CameraNode::updating, this );
    updating_thread.detach();
    return;
}

bool CameraNode::isOpened( void ){
    cap_mutex.lock();
    bool ret = cap.isOpened();
    cap_mutex.unlock();
    return ret;
}

bool CameraNode::read( Mat& frame ){
    cap_mutex.lock();
    if ( ! cap.isOpened() ){
        cap_mutex.unlock();
        cout << "cap closed" << endl;
        return false;
    }
    //cap.grab();
    //last_grab = high_resolution_clock::now();
    bool ret = cap.retrieve(frame);
    cap_mutex.unlock();
    return ret;
}

void CameraNode::release( void ){
    cap_mutex.lock();
    cap.release();
    cap_mutex.unlock();
    return;
}
bool CameraNode::compress( vector<uchar>& img_buffer , Mat frame ){
    //Mat gray_frame;
    //resize(frame, frame, Size(), 0.5, 0.5 );
    //cvtColor( frame, gray_frame, COLOR_BGR2GRAY );
    return imencode(".jpeg", frame, img_buffer, quality);
}

void CameraNode::updating( void ){
    while( true ){
        cap_mutex.lock();
        if ( ! cap.isOpened() ){
            cap_mutex.unlock();
            break;
        }
        high_resolution_clock::time_point now = high_resolution_clock::now();
        if ( intervalMs(now, last_grab) > interval ){
            cap.grab();
            last_grab = high_resolution_clock::now();
        }
        cap_mutex.unlock();
        this_thread::sleep_for(milliseconds( (int) ( interval / 5 ) ) );
    }
    return;
}

double intervalMs(high_resolution_clock::time_point end, high_resolution_clock::time_point start)
{
    duration<double> time_span = end - start;
    microseconds d = duration_cast< microseconds >( time_span );
    return d.count() / 1000.0;
}
