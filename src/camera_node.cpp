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
    new_frame = true;
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
    if ( intervalMs( high_resolution_clock::now(), last_grab ) > interval * 0.3 || new_frame == false ){
        cap.grab();
        new_frame = true;
        last_grab = high_resolution_clock::now();
    }
    bool ret = cap.retrieve(frame);
    new_frame = false;
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
    resize(frame, frame, Size(), resize_ratio, resize_ratio );
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
        if ( intervalMs(high_resolution_clock::now(), last_grab) > interval * 0.9 ){
            cap.grab();
            new_frame = true;
            last_grab = high_resolution_clock::now();
        }
        cap_mutex.unlock();
        this_thread::sleep_for(milliseconds( (int) ( interval ) ) );
    }
    return;
}

double intervalMs(high_resolution_clock::time_point end, high_resolution_clock::time_point start)
{
    duration<double> time_span = end - start;
    microseconds d = duration_cast< microseconds >( time_span );
    return d.count() / 1000.0;
}
