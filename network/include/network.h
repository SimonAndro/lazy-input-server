
#ifndef _NETWORK_HEADER_
#define _NETWORK_HEADER_

#if defined(_WIN32)
#ifndef _WIN32_WINNT
#define _WIN32_WINNT 0x0600
#endif
#include <winsock2.h>
#include <ws2tcpip.h>
#pragma comment(lib, "ws2_32.lib")

#else
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <unistd.h>
#include <errno.h>

#endif

#if defined(_WIN32)
#define ISVALIDSOCKET(s) ((s) != INVALID_SOCKET)
#define CLOSESOCKET(s) closesocket(s)
#define GETSOCKETERRNO() (WSAGetLastError())

#else
#define ISVALIDSOCKET(s) ((s) >= 0)
#define CLOSESOCKET(s) close(s)
#define SOCKET int
#define GETSOCKETERRNO() (errno)
#endif

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <windows.h>

#define MAX_REQUEST_SIZE 4095 //size of request body

struct client_info
{ // client info struct
    socklen_t address_length;
    struct sockaddr_storage address;
    char address_buffer[128];
    SOCKET socket;
    char request[MAX_REQUEST_SIZE + 1];
    int received;
    struct client_info *next;
};

SOCKET create_socket(const char *host, const char *port); // created TCP socket

struct client_info *get_client(struct client_info **client_list,
                               SOCKET s); // get TCP client

void drop_client(struct client_info **client_list,
                 struct client_info *client);                            // drop TCP client

const char *get_client_address(struct client_info *ci);                  // gets client address
fd_set wait_on_clients(struct client_info **client_list, SOCKET server); // wait a client to connect

#endif //_NETWORK_HEADER_