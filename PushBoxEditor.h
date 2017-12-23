#pragma once
#include "MyWin32Util.h"

#define BACKGROUND 0
#define WALL 1
#define MAN 2
#define BOX 3
#define BLANK 4
#define TARGET 5
#define BOXONTARGET 6

#define LM_FAIL_RESOURCE 0
#define LM_FAIL_FILE 1
#define LM_SUCCEED 2


LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
int LoadMapData(HWND hWnd);
void InitMapData();
bool StoreMapData();