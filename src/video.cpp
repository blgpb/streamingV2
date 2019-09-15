#include "video.hpp"

string getCurrentTime( void )
{
    time_t rawtime;
    struct tm * timeinfo;
    char buffer[80];
    time (&rawtime);
    timeinfo = localtime(&rawtime);
    strftime(buffer,sizeof(buffer),"%m-%d_%H-%M-%S",timeinfo);
    string current_time(buffer);
    return current_time;
}

Video::Video(  int width, int height, string path  ){
    video_recording_file_path = path;
    w = width;
    h = height;
}

void Video::run( bool flag, Mat frame){
    if ( ! write_mutex.try_lock_for( milliseconds( WRITE_VIDEO_WAIT_FOR_MS ) ) ){
        cout << "[LOGGING]: write frame timeout" << endl;
        return;
    }
    write_mutex.unlock();
    if ( flag ){
        if ( video.isOpened() ){
            RECORDING = {false};
            thread t( &Video::tryWriteFrame, this, frame );
            t.detach();
            while ( RECORDING == false) {};
        }
        else if ( ! video.open( video_recording_file_path + getCurrentTime() + ".avi", CV_FOURCC( 'D', 'I', 'V', 'X' ), 30.0, Size( w , h ) ) ){
            cout << "[LOGGING]: fail to open videowriter" << endl;
        }
    }
    else if (  video.isOpened() ){
            video.release();
    }
}

void Video::tryWriteFrame( Mat frame ){
    write_mutex.lock();
    RECORDING = {true};
    video.write( frame );
    write_mutex.unlock();
    return;
}
