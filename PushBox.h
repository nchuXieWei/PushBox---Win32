#pragma once
#include "MyWin32Util.h"

#define BACKGROUND 0
#define WALL 1
#define MAN 2
#define BOX 3
#define BLANK 4
#define TARGET 5
#define BOXONTARGET 6
#define MANONTARGET 7


LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
bool LoadMapData(HWND hWnd);