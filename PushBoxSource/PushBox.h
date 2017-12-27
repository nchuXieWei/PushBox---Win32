#pragma once
#include "..\Util\MyWin32Util.h"

#define BACKGROUND 0
#define WALL 1
#define BOX 2
#define TARGET 3
#define MAN 4
#define BOXONTARGET 5
#define BLANK 6
#define MANONTARGET 7


LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
bool LoadMapData(HWND hWnd);