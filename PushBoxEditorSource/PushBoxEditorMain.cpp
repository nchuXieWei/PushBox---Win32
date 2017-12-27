#include "PushBoxEditor.h"


int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR szCmdLine, int iCmdShow)
{
	//注册窗口类
	TCHAR wndClassName[20];
	WNDCLASS wndClass;

	wsprintf(wndClassName, _TEXT("PushBoxWinCls"));
	wndClass.cbClsExtra = 0;
	wndClass.cbWndExtra = 0;
	wndClass.hbrBackground = (HBRUSH)GetStockObject(GRAY_BRUSH);
	wndClass.hCursor = LoadCursor(NULL, IDC_ARROW);
	wndClass.hIcon = LoadIcon(NULL, IDI_WINLOGO);
	wndClass.hInstance = hInstance;
	wndClass.lpfnWndProc = &WndProc;
	wndClass.lpszClassName = wndClassName;
	wndClass.lpszMenuName = NULL;
	wndClass.style = CS_VREDRAW | CS_HREDRAW;
	if (!RegisterClass(&wndClass))
	{
		MessageBoxPrintf(NULL, MB_OK, _TEXT("Error"), _TEXT("无法注册窗口类：%s"), wndClass.lpszClassName);
		return -1;
	}

	//创建窗口
	HWND hWnd = CreateWindow(wndClassName, _TEXT("推箱子编辑器"), WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT,
		CW_USEDEFAULT, CW_USEDEFAULT, NULL, NULL, hInstance, NULL);

	//加载关卡数据
	int loadResult = LoadMapData(hWnd);
	if (loadResult == LM_FAIL_RESOURCE)
		return -2;
	else if (loadResult==LM_FAIL_FILE)
	{
		InitMapData();
		if (!StoreMapData())
		{
			MessageBox(NULL, _TEXT("初始化关卡数据失败，无法将关卡数据存入文件"), _TEXT("Error"), MB_OK|MB_ICONERROR);
			return -3;
		}
		loadResult = LoadMapData(hWnd);
		if (loadResult != LM_SUCCEED)
			return -4;
	}


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


