#ifndef _SOCKET_
#define _SOCKET_

#include <unistd.h>
#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <fcntl.h>
#include <arpa/inet.h> 
#include <string.h>
#include <iostream>
#include <errno.h>

#define PEER_SHUTDOWN -1

using namespace std;

bool udpServerInit( int& sock_fd, const char* host, const int port );
bool udpClientInit( int& sock_fd, const char* host, const int port );

bool tcpServerInit( int& server_fd, int& client_fd, const char* host, const int port);
bool tcpClientInit( int& client_fd, const char* host, const int port );

bool sendAll( int socket, void *buffer, size_t length );
bool recvAll( int socket, void *buffer, size_t length );

#endif