#if defined(UNICODE) && !defined(_UNICODE)
#define _UNICODE
#elif defined(_UNICODE) && !defined(UNICODE)
#define UNICODE
#endif

#include <tchar.h>
#include <windows.h>
#include <stdio.h>

/*  Declare Windows procedure  */
LRESULT CALLBACK WindowProcedure (HWND, UINT, WPARAM, LPARAM);

/*  Make the class name into a global variable  */
TCHAR szClassName[ ] = _T("LazyInputServerApp");

HCURSOR hCurs1, hCurs2;    // cursor handles

POINT pt;                  // cursor location
RECT rc;                   // client area coordinates
static int repeat = 1;     // repeat key counter

HWND activeHWND;

int WINAPI WinMain (HINSTANCE hThisInstance,
                    HINSTANCE hPrevInstance,
                    LPSTR lpszArgument,
                    int nCmdShow)
{
    HWND hwnd;               /* This is the handle for our window */
    MSG messages;            /* Here messages to the application are saved */
    WNDCLASSEX wincl;        /* Data structure for the windowclass */

    /* The Window structure */
    wincl.hInstance = hThisInstance;
    wincl.lpszClassName = szClassName;
    wincl.lpfnWndProc = WindowProcedure;      /* This function is called by windows */
    wincl.style = CS_DBLCLKS;                 /* Catch double-clicks */
    wincl.cbSize = sizeof (WNDCLASSEX);

    /* Use default icon and mouse-pointer */
    wincl.hIcon = LoadIcon (NULL, IDI_APPLICATION);
    wincl.hIconSm = LoadIcon (NULL, IDI_APPLICATION);
    wincl.hCursor = LoadCursor (NULL, IDC_ARROW);
    wincl.lpszMenuName = NULL;                 /* No menu */
    wincl.cbClsExtra = 0;                      /* No extra bytes after the window class */
    wincl.cbWndExtra = 0;                      /* structure or the window instance */
    /* Use Windows's default colour as the background of the window */
    wincl.hbrBackground = (HBRUSH) COLOR_BACKGROUND;

    /* Register the window class, and if it fails quit the program */
    if (!RegisterClassEx (&wincl))
        return 0;

    /* The class is registered, let's create the program*/
    hwnd = CreateWindowEx (
               0,                   /* Extended possibilites for variation */
               szClassName,         /* Classname */
               _T("Lazy Input Server"),       /* Title Text */
               WS_OVERLAPPEDWINDOW, /* default window */
               CW_USEDEFAULT,       /* Windows decides the position */
               CW_USEDEFAULT,       /* where the window ends up on the screen */
               544,                 /* The programs width */
               375,                 /* and height in pixels */
               HWND_DESKTOP,        /* The window is a child-window to desktop */
               NULL,                /* No menu */
               hThisInstance,       /* Program Instance handler */
               NULL                 /* No Window Creation data */
           );

    /* Make the window visible on the screen */
    ShowWindow (hwnd, nCmdShow);

    /* Run the message loop. It will run until GetMessage() returns 0 */
    while (GetMessage (&messages, NULL, 0, 0))
    {
        /* Translate virtual-key messages into character messages */
        TranslateMessage(&messages);
        /* Send message to WindowProcedure */
        DispatchMessage(&messages);
    }

    /* The program return-value is 0 - The value that PostQuitMessage() gave */
    return messages.wParam;
}


/*  This function is called by the Windows function DispatchMessage()  */

LRESULT CALLBACK WindowProcedure (HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)                  /* handle the messages */
    {
    case WM_DESTROY:
        PostQuitMessage (0);       /* send a WM_QUIT to the message queue */
        break;
    case WM_KEYDOWN:
        printf("%c\n",wParam);

//        if (wParam != VK_LEFT && wParam != VK_RIGHT &&
//                wParam != VK_UP && wParam != VK_DOWN)
//        {
//            break;
//        }

        GetCursorPos(&pt);

        // Convert screen coordinates to client coordinates.

        ScreenToClient(hwnd, &pt);

        switch (wParam)
        {
            // mouse clicks
            case 'K':
                ClientToScreen(hwnd, &pt);
                activeHWND =  WindowFromPoint(pt);
                SendMessageCallbackA(activeHWND,WM_KEYDOWN,'Q',NULL,NULL,NULL);
                return 0;
                break;
            // Move the cursor to reflect which
            // arrow keys are pressed.
            case VK_LEFT:               // left arrow
                pt.x -= repeat;
                break;

            case VK_RIGHT:              // right arrow
                pt.x += repeat;
                break;

            case VK_UP:                 // up arrow
                pt.y -= repeat;
                break;

            case VK_DOWN:               // down arrow
                pt.y += repeat;
                break;

            default:
                return 0;
        }

        repeat++;           // Increment repeat count.

        // Convert client coordinates to screen coordinates.
        ClientToScreen(hwnd, &pt);
        SetCursorPos(pt.x, pt.y);
        return 0;


    case WM_KEYUP:

        repeat = 1;            // Clear repeat count.
        return 0;
    default:                      /* for messages that we don't deal with */
        return DefWindowProc (hwnd, message, wParam, lParam);
    }

    return 0;
}
