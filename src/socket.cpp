#include "socket.hpp"

bool udpServerInit( int& server_fd, const char* host, const int port){
    if ( ( server_fd = socket(AF_INET, SOCK_DGRAM, 0 ) ) < 0 )
        return false;

    struct sockaddr_in server_address, client_address;
    if( inet_pton( AF_INET, host, &server_address.sin_addr ) <= 0 ) 
        return false;
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(port);

    if ( bind( server_fd, (struct sockaddr *)& server_address, sizeof( server_address ) ) < 0 )
        return false;
    return true;
}

bool udpClientInit( int& client_fd, const char* host, const int port ){
    //set to UDP
    if ( ( client_fd = socket(AF_INET, SOCK_DGRAM, 0 ) ) < 0 )
        return false;
    
    //set to non-blocking
    if ( fcntl( client_fd, F_SETFL, O_NONBLOCK | O_WRONLY ) < 0 )
        return false;
    //if ( fcntl( client_fd, F_SETFL, O_WRONLY ) < 0 )
    //    return false;

    //set send buffer size
    int send_buffer_size = 32 * 1024 * 1024; //32MB
    if ( setsockopt( client_fd, SOL_SOCKET, SO_SNDBUF, &send_buffer_size, sizeof send_buffer_size ) < 0 )
        return false;
    
    //connect to server
    struct sockaddr_in server_address;
    if( inet_pton( AF_INET, host, &server_address.sin_addr ) <= 0 ) 
        return false;
    server_address.sin_family = AF_INET; 
    server_address.sin_port = htons( port );
    if ( connect( client_fd, (struct sockaddr*)& server_address, sizeof( server_address ) ) < 0 )
        return false;
    return true;
}

bool tcpServerInit( int& server_fd, int& client_fd, const char* host, const int port){
    if ( ( server_fd = socket(AF_INET, SOCK_STREAM, 0 ) ) < 0 )
        return false;
    
    struct sockaddr_in server_address, client_address;
    if( inet_pton( AF_INET, host, &server_address.sin_addr ) <= 0 ) 
        return false;
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(port);

    if ( bind( server_fd, (struct sockaddr *)& server_address, sizeof( server_address ) ) < 0 )
        return false;
    if ( listen( server_fd, 3 ) < 0 )
        return false;
    
    int addrlen = sizeof( server_address );
    if ( ( client_fd = accept( server_fd, (struct sockaddr *)& client_address, (socklen_t *)& addrlen ) ) < 0 )
        return false;
    return true;
}

bool tcpClientInit( int& client_fd, const char* host, const int port ){   
    if ( ( client_fd = socket( AF_INET, SOCK_STREAM, 0 ) ) < 0 ) 
        return false;

    if ( fcntl( client_fd, F_SETFL, O_RDONLY ) < 0 )
        return false;
    
    struct sockaddr_in server_address;
    if( inet_pton( AF_INET, host, &server_address.sin_addr ) <= 0 ) 
        return false;
    server_address.sin_family = AF_INET; 
    server_address.sin_port = htons( port ); 
    
    int send_buffer_size = 32 * 1024 * 1024;
    if ( setsockopt( client_fd, SOL_SOCKET, SO_SNDBUF, &send_buffer_size, sizeof send_buffer_size ) < 0 )
        return false;
    
    if ( connect( client_fd, (struct sockaddr *)& server_address, sizeof( server_address ) ) < 0 ) 
        return false; 
    return true;
}

bool sendAll( int socket, void *buffer, size_t length )
{
    char * ptr = (char *) buffer;
    size_t offset = 0;
    size_t bytes_sent = 0;
    while (offset < length)
    {
        bytes_sent = send(socket, ptr + offset, length - offset, MSG_NOSIGNAL );
        if (bytes_sent < 0){
            return false;
        }
        offset += bytes_sent;
    }
    return true;
}

bool recvAll( int socket, void *buffer, size_t length )
{
    char * ptr = (char *) buffer;
    size_t offset = 0;
    size_t bytes_recv = 0;
    while (offset < length)
    {
        bytes_recv = recv(socket, ptr + offset, length - offset, 0);
        if ( bytes_recv == 0){
            throw PEER_SHUTDOWN;
        }
        else if (bytes_recv < 0){
            return false;
        }
        offset += bytes_recv;
    }
    return true;
}
