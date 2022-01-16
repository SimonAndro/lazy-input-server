

#include "../include/network.h"

#include "../../server/include/server.h"



SOCKET create_socket(const char *host, const char *port)
{
    printf("Configuring local address...\n");
    struct addrinfo hints;
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;

    struct addrinfo *bind_address;
    getaddrinfo(host, port, &hints, &bind_address);

    printf("Creating socket...\n");
    SetWindowTextA(progressTextHwnd, "Creating socket...");

    SOCKET socket_listen;
    socket_listen = socket(bind_address->ai_family,
                           bind_address->ai_socktype, bind_address->ai_protocol);
    if (!ISVALIDSOCKET(socket_listen))
    {
        fprintf(stderr, "socket() failed. (%d)\n", GETSOCKETERRNO());
        exit(1);
    }

    printf("Binding socket to local address...\n");
    SetWindowTextA(progressTextHwnd, "Binding");
    if (bind(socket_listen,
             bind_address->ai_addr, bind_address->ai_addrlen))
    {
        fprintf(stderr, "bind() failed. (%d)\n", GETSOCKETERRNO());
        exit(1);
    }
    freeaddrinfo(bind_address);

    printf("Listening...\n");
    SetWindowTextA(progressTextHwnd, "Listening...");
    if (listen(socket_listen, 100) < 0)
    {
        fprintf(stderr, "listen() failed. (%d)\n", GETSOCKETERRNO());
        exit(1);
    }

    // To retrieve host information
    struct hostent *localHost;
    char *localIP;
    struct in_addr **addr_list;

    // Get the local host information
    localHost = gethostbyname("");
    localIP = inet_ntoa(*(struct in_addr *)*localHost->h_addr_list);

    addr_list = (struct in_addr **)localHost->h_addr_list;

    char serverUrl[64] = {0};
    char http_str[] = "http://";
    char urlsBuffer[256];

    for (int i = 0; addr_list[i] != NULL; i++)
    {
        localIP = inet_ntoa(*addr_list[i]);
        printf(localIP);

        memset(serverUrl, 0, sizeof(serverUrl));
        strcat(serverUrl, http_str);
        strcat(serverUrl, localIP);
        strcat(serverUrl, ":");
        strcat(serverUrl, port);
        strcat(serverUrl, "\n");

        GetWindowTextA(urlTextHwnd, urlsBuffer, sizeof(urlsBuffer));

        strcat(urlsBuffer, serverUrl);

        SetWindowTextA(urlTextHwnd, urlsBuffer);
    }

    return socket_listen;
}


struct client_info *get_client(struct client_info **client_list,
        SOCKET s) {
    struct client_info *ci = *client_list;

    while(ci) {
        if (ci->socket == s)
            break;
        ci = ci->next;
    }

    if (ci) return ci;
    struct client_info *n =
        (struct client_info*) calloc(1, sizeof(struct client_info));

    if (!n) {
        fprintf(stderr, "Out of memory.\n");
        exit(1);
    }

    n->address_length = sizeof(n->address);
    n->next = *client_list;
    *client_list = n;
    return n;
}


void drop_client(struct client_info **client_list,
        struct client_info *client) {
    CLOSESOCKET(client->socket);

    struct client_info **p = client_list;

    while(*p) {
        if (*p == client) {
            *p = client->next;
            free(client);
            return;
        }
        p = &(*p)->next;
    }

    fprintf(stderr, "drop_client not found.\n");
    exit(1);
}


const char *get_client_address(struct client_info *ci) {
    getnameinfo((struct sockaddr*)&ci->address,
            ci->address_length,
            ci->address_buffer, sizeof(ci->address_buffer), 0, 0,
            NI_NUMERICHOST);
    return ci->address_buffer;
}


fd_set wait_on_clients(struct client_info **client_list, SOCKET server) {
    fd_set reads;
    FD_ZERO(&reads);
    FD_SET(server, &reads);
    SOCKET max_socket = server;

    struct client_info *ci = *client_list;

    while(ci) {
        FD_SET(ci->socket, &reads);
        if (ci->socket > max_socket)
            max_socket = ci->socket;
        ci = ci->next;
    }

    if (select(max_socket+1, &reads, 0, 0, 0) < 0) {
        fprintf(stderr, "select() failed. (%d)\n", GETSOCKETERRNO());
        exit(1);
    }

    return reads;
}


