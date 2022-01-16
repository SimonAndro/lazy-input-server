


#include "../include/simulation.h"


/**
 * @brief moveMouse, moves mouse
 * 
 * @param mouseDirection 
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

/**
 * @brief parseMouseRequest, parses mouse request
 * 
 * @param cmd 
 * @param request 
 * @param mouseDirection 
 * @return int 
 */
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

/**
 * @brief parseKeyboardRequest, parses keyboard request
 * 
 * @param request 
 * @param virtualKey 
 * @param isShift 
 * @return int 
 */
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