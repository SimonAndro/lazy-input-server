

#ifndef _SIMULATION_HEADER_
#define _SIMULATION_HEADER_

#include <Windows.h>
#include <stdio.h>

// mouse move command structure
struct mouse_move
{
    int xDirection;
    int yDirection;
};

void resetMouse(); // resets mouse speed
void moveMouse(struct mouse_move mouseDirection); // moving the mouse
void rightClick(); // mouse right click button 
void leftClick(); // mouse left click button
void mouseScrollUp(); // mouse scroll button upwards
void mouseScrollDown(); // mouse scroll button downwards
void middleClick(); // mouse midddle button
void keyboardInput(DWORD virtualKey, int isShift); // keyboard input
int parseMouseRequest(char *cmd, char *request, struct mouse_move *mouseDirection); // parsing mouse request url
int parseKeyboardRequest(char *request, DWORD *virtualKey, int *isShift); // parsing keyboard request url

#endif  //_SIMULATION_HEADER_