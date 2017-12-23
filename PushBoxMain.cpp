#include "PushBox.h"


int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR szCmdLine, int iCmdShow)
{
	//注册窗口类
	TCHAR wndClassName[20];
	WNDCLASS wndClass;

	wsprintf(wndClassName, _TEXT("PushBoxWinCls"));
	wndClass.cbClsExtra = 0;
	wndClass.cbWndExtra = 0;
	wndClass.hbrBackground = NULL;
	wndClass.hCursor = LoadCursor(NULL, IDC_ARROW);
	wndClass.hIcon = LoadIcon(NULL, IDI_WINLOGO);
	wndClass.hInstance = hInstance;
	wndClass.lpfnWndProc = &WndProc;
	wndClass.lpszClassName = wndClassName;
	wndClass.lpszMenuName = NULL;
	wndClass.style = CS_VREDRAW | CS_HREDRAW;
	if (!RegisterClass(&wndClass))
	{
		MessageBox(NULL, _TEXT("Can't register wndClass"), _TEXT("Error"), MB_OK | MB_ICONERROR);
		return -1;
	}

	//创建窗口
	HWND hWnd = CreateWindow(wndClassName, _TEXT("Push Box"), WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT,
		CW_USEDEFAULT, CW_USEDEFAULT, NULL, NULL, hInstance, NULL);


	//加载地图数据
	if (!LoadMapData(hWnd))
		return -1;


	//显示窗口
	ShowWindow(hWnd, SW_NORMAL);
	UpdateWindow(hWnd);


	MSG message;
	while (GetMessage(&message, NULL, 0, 0))
	{
		TranslateMessage(&message);
		DispatchMessage(&message);
	}



	return message.wParam;
}


