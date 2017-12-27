#pragma once
#include <Windows.h>
#include <tchar.h>

int MessageBoxPrintf(HWND hWnd, UINT uShowMode, TCHAR *szCaption, TCHAR *szFormat, ...);
void SetClientSize(HWND hWnd, UINT uWidth, UINT uHeight);