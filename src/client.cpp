#include "socket.hpp"
#include "camera_node.hpp"
#include "video.hpp"
#include "client_node.hpp"
#include <chrono>
#include <ctime>
#include <string.h>
#include <iostream>


using namespace std;
using namespace std::chrono;


int main(int argc, char** argv)
{
    /* read from yaml */
    YAML::Node config;
    if ( argc == 1){
        config = YAML::LoadFile("../config/config.yaml");
    }
    else if (argc == 2 && strcmp( argv[1], "local") == 0 ) {

        config = YAML::LoadFile("../config/config_local.yaml");
    }
    else{
        cout << "invalid argv!" << endl;
        return -1;
    }
    
    ServerConfig server_config;
    server_config.host = config["SERVER_CONFIG"]["HOST"].as<string>();
    server_config.port = config["SERVER_CONFIG"]["PORT"].as<int>();
    
    CameraConfig camera_config;
    camera_config.index = config["CAMERA_CONFIG"]["CAMERA_INDEX"].as<int>();
    camera_config.width = config["CAMERA_CONFIG"]["WIDTH"].as<int>();
    camera_config.height = config["CAMERA_CONFIG"]["HEIGHT"].as<int>();
    camera_config.focal_length = config["CAMERA_CONFIG"]["FOCAL_LENGTH"].as<vector<double>>();
    camera_config.principle_point = config["CAMERA_CONFIG"]["PRINCIPLE_POINT"].as<vector<double>>();

    ClientParam client_param;
    client_param.streaming = config["CLIENT_CONFIG"]["STREAMING"].as<bool>();
    double streaming_freq = config["CLIENT_CONFIG"]["STREAMING_FREQ"].as<double>();
    client_param.video_recording = config["CLIENT_CONFIG"]["VIDEO_RECORDING"].as<bool>();
    string recording_path = config["CLIENT_CONFIG"]["VIDEO_RECORDING_FILE_PATH"].as<string>();

    /* init classes */
    ClientNode client_node( server_config.host, server_config.port, client_param );
    CameraNode camera_node( camera_config.index, camera_config.width, camera_config.height );
    if ( ! camera_node.isOpened() )
        return -1;

    Video video( camera_node.width , camera_node.height, recording_path );
    /* start while loop */
    double read_frame_time, img_proc_time, compress_img_time, send_msg_time, write_video_time, total_time;
    high_resolution_clock::time_point t0, start, last_send_msg = high_resolution_clock::now();
    Mat frame, resize_frame;
    while( true ){
        start = high_resolution_clock::now();

        client_param = client_node.getParam();

        t0 = high_resolution_clock::now();
        camera_node.read( frame );
        read_frame_time = intervalMs( high_resolution_clock::now(), t0 );

        /* img proccessing */        
        t0 = high_resolution_clock::now();
        //resize( frame, resize_frame, Size( resize_w, resize_h ) );
        img_proc_time = intervalMs( high_resolution_clock::now(), t0 );

        /* send msg */
        t0 = high_resolution_clock::now();
        if ( client_param.streaming && intervalMs( t0, last_send_msg ) > 1000.0 / streaming_freq){
            last_send_msg = high_resolution_clock::now();
            client_node.sendMsg( &client_param, (uint16_t) sizeof client_param, CLIENT_PARAM);
            vector< uchar > img_buffer;
            if ( camera_node.compress( img_buffer, frame ) ){
                //cout << img_buffer.size() << endl;
                client_node.sendMsg( img_buffer.data(), (uint16_t) img_buffer.size(), IMG );
            }
            else{
                cout << "encode error" << endl;
            }
        }
        send_msg_time = intervalMs( high_resolution_clock::now(), t0 );

        /* write video */
        t0 = high_resolution_clock::now();
        video.run(client_param.video_recording, frame );
        write_video_time = intervalMs( high_resolution_clock::now(), t0 );

        total_time = intervalMs( high_resolution_clock::now(), start );
        
        if ( total_time > 20 ){
            cout << "************************************" << endl;
            cout << "total_time: " << total_time << endl;
            cout << "read_frame_time: " << read_frame_time << endl;
            cout << "img_proc_time: " << img_proc_time << endl;
            cout << "send_msg_time: " << send_msg_time << endl;
            cout << "write_video_time: " << write_video_time << endl;
        }
    }
    client_node.closeClient();
    return 0;
}
