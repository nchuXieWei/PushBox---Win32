#pragma once
#include "..\Util\MyWin32Util.h"

#define BACKGROUND 0
#define WALL 1
#define BOX 2
#define TARGET 3
#define MAN 4
#define BOXONTARGET 5
#define BLANK 6

#define LM_FAIL_RESOURCE 0
#define LM_FAIL_FILE 1
#define LM_SUCCEED 2


LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
int LoadMapData(HWND hWnd);
void InitMapData();
bool StoreMapData();