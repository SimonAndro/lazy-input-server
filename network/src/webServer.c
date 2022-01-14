

#include "../include/network.h"

#include <tchar.h>

#include <windows.h>

#include "../include/webServer.h"

POINT pt;              // cursor location
static int repeat = 1; // repeat key counter

HWND progressTextHwnd, urlTextHwnd;

const char *get_content_type(const char *path)
{
    const char *last_dot = strrchr(path, '.');
    if (last_dot)
    {
        if (strcmp(last_dot, ".css") == 0)
            return "text/css";
        if (strcmp(last_dot, ".csv") == 0)
            return "text/csv";
        if (strcmp(last_dot, ".gif") == 0)
            return "image/gif";
        if (strcmp(last_dot, ".htm") == 0)
            return "text/html";
        if (strcmp(last_dot, ".html") == 0)
            return "text/html";
        if (strcmp(last_dot, ".ico") == 0)
            return "image/x-icon";
        if (strcmp(last_dot, ".jpeg") == 0)
            return "image/jpeg";
        if (strcmp(last_dot, ".jpg") == 0)
            return "image/jpeg";
        if (strcmp(last_dot, ".js") == 0)
            return "application/javascript";
        if (strcmp(last_dot, ".json") == 0)
            return "application/json";
        if (strcmp(last_dot, ".png") == 0)
            return "image/png";
        if (strcmp(last_dot, ".pdf") == 0)
            return "application/pdf";
        if (strcmp(last_dot, ".svg") == 0)
            return "image/svg+xml";
        if (strcmp(last_dot, ".txt") == 0)
            return "text/plain";
    }

    return "application/octet-stream";
}

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

#define MAX_REQUEST_SIZE 4095

struct client_info
{
    socklen_t address_length;
    struct sockaddr_storage address;
    SOCKET socket;
    char request[MAX_REQUEST_SIZE + 1];
    int received;
    struct client_info *next;
};

static struct client_info *clients = 0;

struct client_info *get_client(SOCKET s)
{
    struct client_info *ci = clients;

    while (ci)
    {
        if (ci->socket == s)
            break;
        ci = ci->next;
    }

    if (ci)
        return ci;
    struct client_info *n =
        (struct client_info *)calloc(1, sizeof(struct client_info));

    if (!n)
    {
        fprintf(stderr, "Out of memory.\n");
        exit(1);
    }

    n->address_length = sizeof(n->address);
    n->next = clients;
    clients = n;
    return n;
}

void drop_client(struct client_info *client)
{
    CLOSESOCKET(client->socket);

    struct client_info **p = &clients;

    while (*p)
    {
        if (*p == client)
        {
            *p = client->next;
            free(client);
            return;
        }
        p = &(*p)->next;
    }

    fprintf(stderr, "drop_client not found.\n");
    exit(1);
}

const char *get_client_address(struct client_info *ci)
{
    static char address_buffer[100];
    getnameinfo((struct sockaddr *)&ci->address,
                ci->address_length,
                address_buffer, sizeof(address_buffer), 0, 0,
                NI_NUMERICHOST);
    return address_buffer;
}

fd_set wait_on_clients(SOCKET server)
{
    fd_set reads;
    FD_ZERO(&reads);
    FD_SET(server, &reads);
    SOCKET max_socket = server;

    struct client_info *ci = clients;

    while (ci)
    {
        FD_SET(ci->socket, &reads);
        if (ci->socket > max_socket)
            max_socket = ci->socket;
        ci = ci->next;
    }

    if (select(max_socket + 1, &reads, 0, 0, 0) < 0)
    {
        fprintf(stderr, "select() failed. (%d)\n", GETSOCKETERRNO());
        exit(1);
    }

    return reads;
}

void send_400(struct client_info *client)
{
    const char *c400 = "HTTP/1.1 400 Bad Request\r\n"
                       "Connection: close\r\n"
                       "Content-Length: 11\r\n\r\nBad Request";
    send(client->socket, c400, strlen(c400), 0);
    drop_client(client);
}

void send_404(struct client_info *client)
{
    const char *c404 = "HTTP/1.1 404 Not Found\r\n"
                       "Connection: close\r\n"
                       "Content-Length: 9\r\n\r\nNot Found";
    send(client->socket, c404, strlen(c404), 0);
    drop_client(client);
}

void send_200(struct client_info *client)
{
    const char *c200 = "HTTP/1.1 200 OK\r\n"
                       "Connection: close\r\n"
                       "Content-Length: 7\r\n\r\nSuccess";
    send(client->socket, c200, strlen(c200), 0);
    drop_client(client);
}

void serve_resource(struct client_info *client, const char *path)
{

    printf("serve_resource %s %s\n", get_client_address(client), path);

    if (strcmp(path, "/") == 0)
        path = "/index.html";

    if (strlen(path) > 100)
    {
        send_400(client);
        return;
    }

    if (strstr(path, ".."))
    {
        send_404(client);
        return;
    }

    char full_path[128];
    sprintf(full_path, "public%s", path);

#if defined(_WIN32)
    char *p = full_path;
    while (*p)
    {
        if (*p == '/')
            *p = '\\';
        ++p;
    }
#endif

    FILE *fp = fopen(full_path, "rb");

    if (!fp)
    {
        send_404(client);
        return;
    }

    fseek(fp, 0L, SEEK_END);
    size_t cl = ftell(fp);
    rewind(fp);

    const char *ct = get_content_type(full_path);

#define BSIZE 1024
    char buffer[BSIZE];

    sprintf(buffer, "HTTP/1.1 200 OK\r\n");
    send(client->socket, buffer, strlen(buffer), 0);

    sprintf(buffer, "Connection: close\r\n");
    send(client->socket, buffer, strlen(buffer), 0);

    sprintf(buffer, "Content-Length: %u\r\n", cl);
    send(client->socket, buffer, strlen(buffer), 0);

    sprintf(buffer, "Content-Type: %s\r\n", ct);
    send(client->socket, buffer, strlen(buffer), 0);

    sprintf(buffer, "\r\n");
    send(client->socket, buffer, strlen(buffer), 0);

    int r = fread(buffer, 1, BSIZE, fp);
    while (r)
    {
        send(client->socket, buffer, r, 0);
        r = fread(buffer, 1, BSIZE, fp);
    }

    fclose(fp);
    drop_client(client);
}

// mouse move command
struct mouse_move
{
    int xDirection;
    int yDirection;
};

int parseMouseRequest(char *cmd, char *request, struct mouse_move *mouseDirection)
{
    if (!strcmp(cmd, "mouse"))
    {

        char delim_1[] = "=";
        char delim_2[] = "&";

        char *ptr_x, *ptr_y, *ptr;
        ptr_x = strtok(request, delim_1);
        ptr_x = strtok(NULL, delim_1);
        ptr_y = strtok(NULL, delim_1);

        int xDir = 0, yDir = 0;

        //printf("%s, %s\n", ptr_x, ptr_y);

        // parse x
        if (ptr_x != NULL)
        {
            ptr = strtok(ptr_x, delim_2);
            xDir = atoi(ptr);
            printf("x = '%d'\n", xDir);
        }

        // parse y
        if (ptr_y != NULL)
        {
            ptr = strtok(ptr_y, delim_2);
            yDir = atoi(ptr);
            printf("y = '%d'\n", yDir);
        }

        mouseDirection->xDirection = xDir;
        mouseDirection->yDirection = yDir;
    }
    else
    {
        return -1; // unknown command
    }

    return 0; // success return
}

int parseKeyboardRequest(char *request, DWORD *virtualKey, int *isShift)
{
    char delim_1[] = "=";
    char delim_2[] = "&";

    *isShift = 0;

    int keyHex = 0;

    char *ptr_x, *ptr_y, *ptr;
    ptr_x = strtok(request, delim_1);
    ptr_x = strtok(NULL, delim_1);
    ptr_y = strtok(NULL, delim_1);

    // parse virtual key
    if (ptr_x != NULL)
    {
        ptr = strtok(ptr_x, delim_2);
        *virtualKey = (DWORD)strtol(ptr, NULL, 16);
        printf("virtualKey = '%0x'\n", *virtualKey);
    }

    // parse shift status
    if (ptr_y != NULL)
    {
        ptr = strtok(ptr_y, delim_2);
        *isShift = atoi(ptr);
        printf("isShift = '%d'\n", *isShift);
    }

    return 0; // success return
}

/**
 * @brief  moveMouse
 * 
 */
void moveMouse(struct mouse_move mouseDirection)
{
    GetCursorPos(&pt);

    // printf("%d, %d\n", mouseDirection.xDirection, mouseDirection.yDirection);

    if (mouseDirection.xDirection < 0) // left arrow
        pt.x -= repeat;

    if (mouseDirection.xDirection > 0) // right arrow
        pt.x += repeat;

    if (mouseDirection.yDirection < 0) // up arrow
        pt.y -= repeat;

    if (mouseDirection.yDirection > 0) // down arrow
        pt.y += repeat;

    SetCursorPos(pt.x, pt.y);

    if (repeat < 10)
        repeat += 2; // Increment repeat count.
}

/**
 * @brief rightClick, right click mouse button
 * 
 */
void rightClick()
{
    INPUT Input = {0};
    ZeroMemory(&Input, sizeof(INPUT));
    // right down
    Input.type = INPUT_MOUSE;
    Input.mi.dwFlags = MOUSEEVENTF_RIGHTDOWN;
    SendInput(1, &Input, sizeof(INPUT));

    // right up
    ZeroMemory(&Input, sizeof(INPUT));
    Input.type = INPUT_MOUSE;
    Input.mi.dwFlags = MOUSEEVENTF_RIGHTUP;
    SendInput(1, &Input, sizeof(INPUT));
}

/**
 * @brief leftClick, left click mouse button
 * 
 */
void leftClick()
{
    INPUT Input = {0};
    ZeroMemory(&Input, sizeof(INPUT));
    // left down
    Input.type = INPUT_MOUSE;
    Input.mi.dwFlags = MOUSEEVENTF_LEFTDOWN;
    SendInput(1, &Input, sizeof(INPUT));

    // left up
    ZeroMemory(&Input, sizeof(INPUT));
    Input.type = INPUT_MOUSE;
    Input.mi.dwFlags = MOUSEEVENTF_LEFTUP;
    SendInput(1, &Input, sizeof(INPUT));
}

/**
 * @brief mouseScrollUp, scroll wheel scrolling upwards
 * 
 */
void mouseScrollUp()
{
    INPUT Input = {0};
    ZeroMemory(&Input, sizeof(INPUT));
    // scroll up
    Input.type = INPUT_MOUSE;
    Input.mi.dwFlags = MOUSEEVENTF_WHEEL;
    Input.mi.mouseData = repeat++;
    SendInput(1, &Input, sizeof(INPUT));
}

/**
 * @brief mouseScrollDown, scroll wheel scrolling downwards
 * 
 */
void mouseScrollDown()
{
    INPUT Input = {0};
    ZeroMemory(&Input, sizeof(INPUT));
    // scroll down
    ZeroMemory(&Input, sizeof(INPUT));
    Input.type = INPUT_MOUSE;
    Input.mi.dwFlags = MOUSEEVENTF_WHEEL;
    Input.mi.mouseData = -repeat++;
    SendInput(1, &Input, sizeof(INPUT));
}

/**
 * @brief middle button, middle mouse button dowm
 * 
 */
void middleClick()
{
    INPUT Input = {0};
    ZeroMemory(&Input, sizeof(INPUT));
    // left down
    Input.type = INPUT_MOUSE;
    Input.mi.dwFlags = MOUSEEVENTF_MIDDLEDOWN;
    SendInput(1, &Input, sizeof(INPUT));

    // left up
    ZeroMemory(&Input, sizeof(INPUT));
    Input.type = INPUT_MOUSE;
    Input.mi.dwFlags = MOUSEEVENTF_MIDDLEUP;
    SendInput(1, &Input, sizeof(INPUT));
}

/**
 * @brief keyboardInput, sends keyboard input messages
 * 
 * @return void 
 */
void keyboardInput(DWORD virtualKey, int isShift)
{
    INPUT Input = {0};

    if (isShift)
    { //simulate shift key down
        ZeroMemory(&Input, sizeof(INPUT));
        Input.type = INPUT_KEYBOARD;
        Input.ki.wVk = VK_LSHIFT;
        SendInput(1, &Input, sizeof(INPUT));
    }

    // simulate pressing the key
    ZeroMemory(&Input, sizeof(INPUT));
    Input.type = INPUT_KEYBOARD;
    Input.ki.wVk = virtualKey;
    SendInput(1, &Input, sizeof(INPUT));

    // simulate releasing the key
    ZeroMemory(&Input, sizeof(INPUT));
    Input.type = INPUT_KEYBOARD;
    Input.ki.wVk = virtualKey;
    Input.ki.dwFlags = KEYEVENTF_KEYUP;
    SendInput(1, &Input, sizeof(INPUT));

    if (isShift)
    { //simulate release of the shift key
        ZeroMemory(&Input, sizeof(INPUT));
        Input.type = INPUT_KEYBOARD;
        Input.ki.wVk = VK_LSHIFT;
        Input.ki.dwFlags = KEYEVENTF_KEYUP;
        SendInput(1, &Input, sizeof(INPUT));
    }
}

void runServer(void *param)
{

    HWND *handlesEx = (HWND *)param;
    progressTextHwnd = handlesEx[0];
    urlTextHwnd = handlesEx[1];

#if defined(_WIN32)
    WSADATA d;
    if (WSAStartup(MAKEWORD(2, 2), &d))
    {
        fprintf(stderr, "Failed to initialize.\n");
        return;
    }
#endif

    SOCKET server = create_socket("0.0.0.0", "2023");

    char progressMsg[256] = {0};

    while (1)
    {

        fd_set reads;
        reads = wait_on_clients(server);

        printf("%d\n", reads.fd_count);

        if (FD_ISSET(server, &reads))
        {
            struct client_info *client = get_client(-1);

            client->socket = accept(server,
                                    (struct sockaddr *)&(client->address),
                                    &(client->address_length));

            if (!ISVALIDSOCKET(client->socket))
            {
                fprintf(stderr, "accept() failed. (%d)\n",
                        GETSOCKETERRNO());
                return;
            }

            printf("New connection from %s.\n",
                   get_client_address(client));

            memset(progressMsg, 0, sizeof(progressMsg));
            strcat(progressMsg, get_client_address(client));
            strcat(progressMsg, " connected");

            SetWindowTextA(progressTextHwnd, progressMsg);
        }

        struct client_info *client = clients;
        while (client)
        {
            struct client_info *next = client->next;

            if (FD_ISSET(client->socket, &reads))
            {

                if (MAX_REQUEST_SIZE == client->received)
                {
                    send_400(client);
                    client = next;
                    continue;
                }

                int r = recv(client->socket,
                             client->request + client->received,
                             MAX_REQUEST_SIZE - client->received, 0);

                if (r < 1)
                {
                    printf("Unexpected disconnect from %s.\n",
                           get_client_address(client));
                    drop_client(client);

                    memset(progressMsg, 0, sizeof(progressMsg));
                    strcat(progressMsg, get_client_address(client));
                    strcat(progressMsg, " disconnected");
                }
                else
                {
                    client->received += r;
                    client->request[client->received] = 0;

                    char *q = strstr(client->request, "\r\n\r\n");
                    if (q)
                    {
                        *q = 0;

                        printf("%d", strlen(client->request));
                        printf("\n");
                        printf(client->request);
                        printf("\n");

                        char mouse_move_req[] = "GET /mouse-move";
                        char mouse_stop_req[] = "GET /mouse-stop";
                        char mouse_left_req[] = "GET /mouse-left";
                        char mouse_right_req[] = "GET /mouse-right";
                        char mouse_doubleTap_req[] = "GET /double-tap";
                        char mouse_scrollUp_req[] = "GET /scroll-up";
                        char mouse_scrollDown_req[] = "GET /scroll-down";
                        char mouse_scrollWheelDown_req[] = "GET /wheel-down";
                        char keyboard_key[] = "GET /keyboard";

                        if (strncmp("GET /", client->request, 5))
                        {
                            send_400(client);
                        }
                        else if (!strncmp(mouse_move_req, client->request, strlen(mouse_move_req)))
                        { //mouse move command

                            char request_buffer[255] = {0};
                            struct mouse_move mouseDirection = {0};
                            int res = parseMouseRequest("mouse", client->request, &mouseDirection);

                            if (res) // invalid result from request parser
                            {
                                send_400(client);
                            }
                            else
                            {
                                moveMouse(mouseDirection);
                                send_200(client);
                            }
                        }
                        else if (!strncmp(mouse_left_req, client->request, strlen(mouse_left_req)))
                        { //mouse left click command
                            leftClick();
                            send_200(client);
                        }
                        else if (!strncmp(mouse_right_req, client->request, strlen(mouse_right_req)))
                        { //mouse right click command
                            rightClick();
                            send_200(client);
                        }
                        else if (!strncmp(mouse_doubleTap_req, client->request, strlen(mouse_doubleTap_req)))
                        { //mouse double click command
                            leftClick();
                            leftClick();
                            send_200(client);
                        }
                        else if (!strncmp(mouse_stop_req, client->request, strlen(mouse_stop_req)))
                        { //mouse stop move command
                            repeat = 1;
                            send_200(client);
                        }
                        else if (!strncmp(mouse_scrollUp_req, client->request, strlen(mouse_scrollUp_req)))
                        { //mouse scroll upwards command
                            mouseScrollUp();
                            send_200(client);
                        }
                        else if (!strncmp(mouse_scrollDown_req, client->request, strlen(mouse_scrollDown_req)))
                        { //mouse scroll downwards command
                            mouseScrollDown();
                            send_200(client);
                        }
                        else if (!strncmp(mouse_scrollWheelDown_req, client->request, strlen(mouse_scrollWheelDown_req)))
                        { //mouse middle button down command
                            middleClick();
                            send_200(client);
                        }
                        else if (!strncmp(keyboard_key, client->request, strlen(keyboard_key)))
                        { //keyboard input

                            DWORD virtualKey;
                            int isShift = 0;

                            parseKeyboardRequest(client->request, &virtualKey, &isShift);

                            keyboardInput(virtualKey, isShift);

                            send_200(client);
                        }
                        else
                        {
                            char *path = client->request + 4;
                            char *end_path = strstr(path, " ");
                            if (!end_path)
                            {
                                send_400(client);
                            }
                            else
                            {
                                *end_path = 0;
                                serve_resource(client, path);
                            }
                        }
                    } //if (q)
                }
            }

            client = next;
        }

    } //while(1)

    printf("\nClosing socket...\n");
    CLOSESOCKET(server);

#if defined(_WIN32)
    WSACleanup();
#endif

    printf("Finished.\n");
}
