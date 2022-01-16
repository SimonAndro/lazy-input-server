
#include "../server/include/server.h"

#include "../include/main.h"

const char g_szClassName[] = "myWindowClass";
const char g_appName[] = "Lazy Input";
const int g_windowWidth = 512;
const int g_windowHeight = 512 - 512 / 4;

HBITMAP g_hbmBackgroundImage = NULL;

/*  Declare Windows procedure  */
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

HWND g_updateTextHwnd,g_urlTextHwnd;


int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
				   LPSTR lpCmdLine, int nCmdShow)
{
	WNDCLASSEX wc;
	MSG Msg;
	HWND hwnd;

	//Step 1: Registering the Window Class
	wc.cbSize = sizeof(WNDCLASSEX);
	wc.style = 0;
	wc.lpfnWndProc = WndProc;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = hInstance;
	wc.hIcon = LoadIcon(GetModuleHandle(NULL), MAKEINTRESOURCE(IDI_MYICON));
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wc.lpszMenuName = NULL;
	wc.lpszClassName = g_szClassName;
	wc.hIconSm = LoadIcon(GetModuleHandle(NULL), MAKEINTRESOURCE(IDI_MYICON));

	if (!RegisterClassEx(&wc))
	{
		MessageBox(NULL, "Window Registration Failed!", "Error!",
				   MB_ICONEXCLAMATION | MB_OK);
		return 0;
	}

	// Step 2: Creating the Window
	hwnd = CreateWindowEx(
		WS_EX_CLIENTEDGE,
		g_szClassName,
		g_appName,
		WS_OVERLAPPED | WS_CAPTION | WS_MINIMIZEBOX | WS_SYSMENU,
		CW_USEDEFAULT, CW_USEDEFAULT, g_windowWidth, g_windowHeight,
		NULL, NULL, hInstance, NULL);

	if (hwnd == NULL)
	{
		MessageBox(NULL, "Window Creation Failed!", "Error!",
				   MB_ICONEXCLAMATION | MB_OK);
		return 0;
	}

	ShowWindow(hwnd, nCmdShow);
	UpdateWindow(hwnd);

	HWND handlesEx[2] = {0};
	handlesEx[0] = g_updateTextHwnd;
	handlesEx[1] = g_urlTextHwnd;

	// create server thread
	HANDLE  serverThread = (HANDLE)_beginthread(runServer, 0, &handlesEx);

	// Step 3: The Message Loop
	while (GetMessage(&Msg, NULL, 0, 0) > 0)
	{
		TranslateMessage(&Msg);
		DispatchMessage(&Msg);
	}

	//close server thread
 	if (serverThread) CloseHandle(serverThread);

	/* The program return-value is 0 - The value that PostQuitMessage() gave */
	return Msg.wParam;
}

// Step 4: the Window Procedure
/*  This function is called by the Windows function DispatchMessage()  */
LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg)
	{
	case WM_CREATE:
	{
		//background image
		g_hbmBackgroundImage = LoadBitmap(GetModuleHandle(NULL), MAKEINTRESOURCE(IDB_BACKGROUND));
		if (g_hbmBackgroundImage == NULL)
			MessageBox(hwnd, "Could not load IDB_BACKGROUND!", "Error", MB_OK | MB_ICONEXCLAMATION);

		// static text and edit text
		HFONT hfDefault;
		HWND hEdit, hTitle,hUrlTitle;

		// hfDefault = (HFONT)GetStockObject(DEFAULT_GUI_FONT);
		hfDefault=CreateFont (15, 0, 0, 0, FW_DONTCARE, FALSE, FALSE, FALSE, ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_SWISS, "Arial");

		// how to use
		char titleText[] =
			"	Lazy Input, the world is this simple!\n\n"
			"	How to Use:\n"
			"	- Connect Computer and mobile device onto the same WIFI.\n"
			"	- Type the url below into the url bar of any browser on your mobile device.\n"
			"	- Thats it!";
		hTitle = CreateWindowEx(WS_EX_CLIENTEDGE, "STATIC", titleText,
								WS_CHILD | WS_VISIBLE,
								0, 0, g_windowWidth, 100, hwnd, (HMENU)ID_NONE, GetModuleHandle(NULL), NULL);
		if (hTitle == NULL)
			MessageBox(hwnd, "Could not create hTitle box.", "Error", MB_OK | MB_ICONERROR);
		SendMessage(hTitle, WM_SETFONT, (WPARAM)hfDefault, MAKELPARAM(FALSE, 0));

		// url text
		g_updateTextHwnd = CreateWindowEx(WS_EX_CLIENTEDGE, "STATIC", "",
							   WS_CHILD | WS_VISIBLE,
							   (g_windowWidth-205)/2, 105, 205, 25, hwnd, (HMENU)IDC_MAIN_EDIT, GetModuleHandle(NULL), NULL);
		if (g_updateTextHwnd == NULL)
			MessageBox(hwnd, "Could not create edit box.", "Error", MB_OK | MB_ICONERROR);
		SendMessage(g_updateTextHwnd, WM_SETFONT, (WPARAM)hfDefault, MAKELPARAM(FALSE, 0));
		

		// url title
		hUrlTitle = CreateWindowEx(WS_EX_CLIENTEDGE, "STATIC", "Copy/Type one url to browser url bar:",
								WS_CHILD | WS_VISIBLE,
								227, 135, 210, 25, hwnd, (HMENU)ID_NONE, GetModuleHandle(NULL), NULL);
		if (hUrlTitle == NULL)
			MessageBox(hwnd, "Could not create hUrlTitle box.", "Error", MB_OK | MB_ICONERROR);
		SendMessage(hUrlTitle, WM_SETFONT, (WPARAM)hfDefault, MAKELPARAM(FALSE, 0));


		g_urlTextHwnd = CreateWindowEx(WS_EX_CLIENTEDGE, "EDIT", "",
							   WS_CHILD | WS_VISIBLE|ES_MULTILINE,
							   227, 160, 210, g_windowHeight-160, hwnd, (HMENU)ID_NONE, GetModuleHandle(NULL), NULL);
		if (g_urlTextHwnd == NULL)
			MessageBox(hwnd, "Could not create edit box.", "Error", MB_OK | MB_ICONERROR);
		SendMessage(g_urlTextHwnd, WM_SETFONT, (WPARAM)hfDefault, MAKELPARAM(FALSE, 0));

	
	}
	break;
	case WM_PAINT:
	{
		BITMAP bm;
		PAINTSTRUCT ps;
		HDC hdc = BeginPaint(hwnd, &ps);
		HDC hdcMem = CreateCompatibleDC(hdc);
		HBITMAP hbmOld = SelectObject(hdcMem, g_hbmBackgroundImage);
		GetObject(g_hbmBackgroundImage, sizeof(bm), &bm);
		SetStretchBltMode(hdc, STRETCH_HALFTONE);
		int bgWidth = bm.bmWidth / 3;
		int bgHeight = bm.bmHeight / 3;
		StretchBlt(hdc, 75, 135, bgWidth, bgHeight, hdcMem, 0, 0, bm.bmWidth, bm.bmHeight, SRCCOPY);
		SelectObject(hdcMem, hbmOld);
		DeleteDC(hdcMem);
		EndPaint(hwnd, &ps);
	}
	break;
	case WM_CLOSE:
		DestroyWindow(hwnd);
		break;
	case WM_DESTROY:
		DeleteObject(g_hbmBackgroundImage);
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hwnd, msg, wParam, lParam);
	}
	return 0;
}
