#include "socket.hpp"
#include "client_node.hpp"
#include <fstream>

using namespace std;
using namespace cv;

void print_uchar( uchar* buffer, int length);

int main(int argc, char**argv )
{
    string host;
    if ( argc == 1){
        host = "192.168.11.27";
    }
    else if (argc == 2 && strcmp( argv[1], "local") == 0 ) {

        host = "127.0.0.1";
    }
    else{
        cout << "invalid argv!" << endl;
        return -1;
    }
    int server, port = 8080;
    if ( ! udpServerInit( server, host.c_str(), port) ){
        cout << strerror(errno) << endl;
        return -1;
    }
    while ( true ){
        uint8_t head, tail, msg_type;
        uint16_t length;
        int64_t timestamp_ms;
        if (!recvAll(server, &head, sizeof head))
        {
            cout << strerror(errno) << endl;
            return -1;
        }
        if (head != 0xAA)
            continue;
        if (recvAll(server, &timestamp_ms, sizeof timestamp_ms) && recvAll(server, &msg_type, sizeof msg_type) && recvAll(server, &length, sizeof length) == false)
        {
            cout << strerror(errno) << endl;
            return -1;
        }
        //cout << unsigned(msg_type) << endl;
        //cout << length << endl;
        uchar buffer[length];
        if (recvAll(server, buffer, length) && recvAll(server, &tail, sizeof tail) == false)
        {
            cout << strerror(errno) << endl;
            return -1;
        }
        if (tail != 0xDD)
            continue;
        switch (msg_type)
        {
        case IMG:
            vector<uchar> decode_img(buffer, buffer + sizeof buffer);
            //print_uchar(buffer, length);
            Mat frame = imdecode(decode_img, -1);
            if ( ! frame.empty()){
                imshow("camera", frame);
                waitKey(10);
            }
            break;
        }
    }
    return 0;
}

void print_uchar( uchar* buffer, int length){
    int i = 0;
    while( i < length ){
        uint8_t k;
        k = buffer[i];
        cout << unsigned(k) << " ";
        i++;
    }
}
