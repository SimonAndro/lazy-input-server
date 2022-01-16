
#ifndef _SERVER_HEADER_
#define _SERVER_HEADER_

#include "../../network/include/network.h"

HWND progressTextHwnd; // server progress handle
HWND urlTextHwnd;      // urls handle

void runServer(); // main server function
void send_400(struct client_info **client_list,
              struct client_info *client); // 400 response
void send_404(struct client_info **client_list,
              struct client_info *client); // 404 response
void send_200(struct client_info **client_list,
              struct client_info *client); // 200 response

#endif //_SERVER_HEADER_