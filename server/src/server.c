
#include "../include/server.h"

#include "../../simulation/include/simulation.h"

const char mouse_move_req[] = "GET /mouse-move";
const char mouse_stop_req[] = "GET /mouse-stop";
const char mouse_left_req[] = "GET /mouse-left";
const char mouse_right_req[] = "GET /mouse-right";
const char mouse_doubleTap_req[] = "GET /double-tap";
const char mouse_scrollUp_req[] = "GET /scroll-up";
const char mouse_scrollDown_req[] = "GET /scroll-down";
const char mouse_scrollWheelDown_req[] = "GET /wheel-down";
const char keyboard_key[] = "GET /keyboard";

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

void send_400(struct client_info **client_list,
              struct client_info *client)
{
    const char *c400 = "HTTP/1.1 400 Bad Request\r\n"
                       "Connection: close\r\n"
                       "Content-Length: 11\r\n\r\nBad Request";
    send(client->socket, c400, strlen(c400), 0);
    drop_client(client_list, client);
}

void send_404(struct client_info **client_list,
              struct client_info *client)
{
    const char *c404 = "HTTP/1.1 404 Not Found\r\n"
                       "Connection: close\r\n"
                       "Content-Length: 9\r\n\r\nNot Found";
    send(client->socket, c404, strlen(c404), 0);
    drop_client(client_list, client);
}

void send_200(struct client_info **client_list,
              struct client_info *client)
{
    const char *c200 = "HTTP/1.1 200 OK\r\n"
                       "Connection: close\r\n"
                       "Content-Length: 7\r\n\r\nSuccess";
    send(client->socket, c200, strlen(c200), 0);
    drop_client(client_list, client);
}

void serve_resource(struct client_info **client_list,
                    struct client_info *client, const char *path)
{

    printf("serve_resource %s %s\n", get_client_address(client), path);

    if (strcmp(path, "/") == 0)
        path = "/index.html";

    if (strlen(path) > 100)
    {
        send_400(client_list, client);
        return;
    }

    if (strstr(path, ".."))
    {
        send_404(client_list, client);
        return;
    }

    char full_path[128];
    sprintf(full_path, "../public%s", path);

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
        send_404(client_list, client);
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
    drop_client(client_list, client);
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

    struct client_info *client_list = 0; //    pointer to clients

    char progressMsg[256] = {0};

    while (1)
    {

        fd_set reads;
        reads = wait_on_clients(&client_list, server);

        printf("ready to read from count: %d\n", reads.fd_count);

        if (FD_ISSET(server, &reads))
        {
            struct client_info *client = get_client(&client_list, -1);

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

        struct client_info *client = client_list;
        while (client)
        {
            struct client_info *next = client->next;

            if (FD_ISSET(client->socket, &reads))
            {

                if (MAX_REQUEST_SIZE == client->received)
                {
                    send_400(&client_list, client);
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
                    drop_client(&client_list, client);

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

                        printf("request length: %d", strlen(client->request));
                        printf("\n");
                        printf("client request : %s", client->request);
                        printf("\n");

                        if (strncmp("GET /", client->request, 5))
                        {
                            send_400(&client_list, client);
                        }
                        else if (!strncmp(mouse_move_req, client->request, strlen(mouse_move_req)))
                        { //mouse move command

                            char request_buffer[255] = {0};
                            struct mouse_move mouseDirection = {0};
                            int res = parseMouseRequest("mouse", client->request, &mouseDirection);

                            if (res) // invalid result from request parser
                            {
                                send_400(&client_list, client);
                            }
                            else
                            {
                                moveMouse(mouseDirection);
                                send_200(&client_list, client);
                            }
                        }
                        else if (!strncmp(mouse_left_req, client->request, strlen(mouse_left_req)))
                        { //mouse left click command
                            leftClick();
                            send_200(&client_list, client);
                        }
                        else if (!strncmp(mouse_right_req, client->request, strlen(mouse_right_req)))
                        { //mouse right click command
                            rightClick();
                            send_200(&client_list, client);
                        }
                        else if (!strncmp(mouse_doubleTap_req, client->request, strlen(mouse_doubleTap_req)))
                        { //mouse double click command
                            leftClick();
                            leftClick();
                            send_200(&client_list, client);
                        }
                        else if (!strncmp(mouse_stop_req, client->request, strlen(mouse_stop_req)))
                        { //mouse stop move command
                            resetMouse();
                            send_200(&client_list, client);
                        }
                        else if (!strncmp(mouse_scrollUp_req, client->request, strlen(mouse_scrollUp_req)))
                        { //mouse scroll upwards command
                            mouseScrollUp();
                            send_200(&client_list, client);
                        }
                        else if (!strncmp(mouse_scrollDown_req, client->request, strlen(mouse_scrollDown_req)))
                        { //mouse scroll downwards command
                            mouseScrollDown();
                            send_200(&client_list, client);
                        }
                        else if (!strncmp(mouse_scrollWheelDown_req, client->request, strlen(mouse_scrollWheelDown_req)))
                        { //mouse middle button down command
                            middleClick();
                            send_200(&client_list, client);
                        }
                        else if (!strncmp(keyboard_key, client->request, strlen(keyboard_key)))
                        { //keyboard input

                            DWORD virtualKey;
                            int isShift = 0;

                            parseKeyboardRequest(client->request, &virtualKey, &isShift);

                            keyboardInput(virtualKey, isShift);

                            send_200(&client_list, client);
                        }
                        else
                        {
                            char *path = client->request + 4;
                            char *end_path = strstr(path, " ");
                            if (!end_path)
                            {
                                send_400(&client_list, client);
                            }
                            else
                            {
                                *end_path = 0;
                                serve_resource(&client_list, client, path);
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
