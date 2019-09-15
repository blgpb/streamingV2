#include "client_node.hpp"

ClientNode::ClientNode( string host, int port, ClientParam param )
{
    HOST = host;
    PORT = port;
    client_param = param;
    if ( ! udpClientInit(udp_client, HOST.c_str(), PORT) || ! tcpClientInit(tcp_client, HOST.c_str(), PORT) ){
        OFFLINE = {true};
        cout << strerror(errno) << endl;
        cout << "[LOGGING]: offline mode" << endl;
        return;
    }
    thread recieving_thread( &ClientNode::recvMsgLoop, this);
    recieving_thread.detach();
    start = high_resolution_clock::now();
    return;
}

void ClientNode::sendMsg( void* buffer, uint16_t length, uint8_t msg_type )
{
    if ( OFFLINE )
        return;
    if ( length > MAX_MSG_CONTENT_LENGTH ){
        cout << "[LOGGING]: buffer size overflow" << endl;
        return;
    }
    Msg msg;
    duration<double> time_span = high_resolution_clock::now() - start;
    milliseconds d = duration_cast< milliseconds >( time_span );
    msg.timestamp_ms = d.count(); //cautious maybe overflow
    msg.msg_type = msg_type;
    msg.length = length;
    memcpy( msg.buffer, buffer, length);
    if (  sendAll( udp_client, &msg.head, sizeof msg.head )
        && sendAll( udp_client, &msg.timestamp_ms, sizeof msg.timestamp_ms )
        && sendAll( udp_client, &msg.msg_type, sizeof msg.msg_type )
        && sendAll( udp_client, &msg.length, sizeof msg.length )
        && sendAll( udp_client, msg.buffer, msg.length )
        && sendAll( udp_client, &msg.tail, sizeof msg.tail ) == false )
    {
        cout << strerror(errno) << endl;
    }
    return;
}

void ClientNode::recvMsgLoop(void)
{
    while (true)
    {
        try{
            uint8_t head, tail, msg_type;
            uint16_t length;
            int64_t timestamp_ms;
            if (!recvAll(tcp_client, &head, sizeof head))
            {
                cout << strerror(errno) << endl;
                continue;
            }
            if (head != 0xAA)
                continue;
            if (recvAll(tcp_client, &timestamp_ms, sizeof timestamp_ms) 
                && recvAll(tcp_client, &msg_type, sizeof msg_type) 
                && recvAll(tcp_client, &length, sizeof length) == false)
            {
                cout << strerror(errno) << endl;
                continue;
            }
            char buffer[length];
            if (recvAll(tcp_client, buffer, length) && recvAll(tcp_client, &tail, sizeof tail) == false)
            {
                cout << strerror(errno) << endl;
                continue;
            }
            if (tail != 0xDD)
                continue;
            switch (msg_type)
            {
            case STREAMING_SWITCH:
                bool streaming;
                memcpy(&streaming, buffer, sizeof streaming);
                param_mutex.lock();
                client_param.streaming = streaming;
                param_mutex.unlock();
                break;
            case VIDEO_RECORDING_SWITCH:
                bool video_recording;
                memcpy(&video_recording, buffer, sizeof video_recording);
                param_mutex.lock();
                client_param.video_recording = video_recording;
                param_mutex.unlock();
                break;
            }
        }
        catch (int exception)
        {
            if (exception == PEER_SHUTDOWN)
            {
                shutdown(tcp_client, 2);
                close(tcp_client);
                cout << "[LOGGING]: try reconnect" << endl;
                while ( ! tcpClientInit(tcp_client, HOST.c_str(), PORT) ){
                    close(tcp_client);
                }
                cout << "[LOGGING]: new connection established" << endl;
            }
        }
    }
    return;
}

ClientParam ClientNode::getParam( void )
{
    ClientParam param_copy;
    param_mutex.lock();
    param_copy = client_param;
    param_mutex.unlock();
    return param_copy;
}

void ClientNode::closeClient(void)
{
    close( tcp_client );
    close( udp_client );
}
