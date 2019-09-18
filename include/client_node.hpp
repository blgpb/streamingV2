#ifndef _CLIENT_NODE_
#define _CLIENT_NODE_

#include "socket.hpp"
#include <thread>
#include <mutex>
#include "yaml-cpp/yaml.h"
#include <chrono>
#include <errno.h>
#include <atomic>

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>

using namespace cv;
using namespace std;
using namespace std::chrono;

#define MAX_MSG_CONTENT_LENGTH 0xFFFF
#define SEND_MSG_FREQ 10
#define HEAD 0xAA
#define TAIL 0xDD

enum MsgType{
    CLIENT_PARAM = 0,
    IMG = 1,
    STREAMING_SWITCH = 4,
    VIDEO_RECORDING_SWITCH = 5,
};

typedef struct{
    string host;
    int port;
} ServerConfig;

typedef struct{
    bool streaming = true;
    bool video_recording = false;
} ClientParam;

typedef struct{
    uint8_t head = HEAD;
    int64_t timestamp_ms;
    uint8_t msg_type;
    uint16_t length;
    char buffer[MAX_MSG_CONTENT_LENGTH];
    uint8_t tail = TAIL;
} Msg;

class ClientNode{
    public:
        ClientNode( string host, int port, ClientParam param );
        void sendMsg( void* buffer, uint16_t length, uint8_t msg_type );
        ClientParam getParam( void );
        void closeClient( void );
    private:
        int udp_client = 0;
        int tcp_client = 0;
        string HOST;
        int PORT;
        atomic<bool> OFFLINE = {false};
        high_resolution_clock::time_point start;
        mutex param_mutex;
        ClientParam client_param;
        void recvMsgLoop( void );
};

#endif