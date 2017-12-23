#include "MyWin32Util.h"
#define MAXSTRSIZE 1024

int MessageBoxPrintf(HWND hWnd, UINT uShowMode, TCHAR * szCaption, TCHAR * szFormat, ...)
{
	//创建一个字符串缓冲区，用于存储去格式后的字符串
	TCHAR szBuffer[MAXSTRSIZE];

	//将格式化字符串转换为无格式字符串并存储在szBuffer
	va_list pva;
	va_start(pva, szFormat);
	_vsntprintf_s(szBuffer, sizeof(szBuffer)/sizeof(TCHAR), szFormat, pva);
	va_end(pva);

	//将szBuffer中的无格式字符串通过MessageBox输出，并返回
	return MessageBox(hWnd, szBuffer, szCaption, uShowMode);
}

void SetClientSize(HWND hWnd, UINT uWidth, UINT uHeight)
{
	if (!hWnd)
	{
		MessageBox(NULL, _TEXT("Can't set client size because HWND is NULL!"), _TEXT("Error"), MB_OK);
		return;
	}
	UINT uOldWinWidth, uOldWinHeight, uOldClnWidth, uOldClnHeight;
	RECT WinRect, ClnRect;
	GetWindowRect(hWnd, &WinRect);
	GetClientRect(hWnd, &ClnRect);
	uOldWinWidth = WinRect.right - WinRect.left;
	uOldWinHeight = WinRect.bottom - WinRect.top;
	uOldClnWidth = ClnRect.right - ClnRect.left;
	uOldClnHeight = ClnRect.bottom - ClnRect.top;

	UINT uNewWinWidth, uNewWinHeight;
	uNewWinWidth = uOldWinWidth - uOldClnWidth + uWidth;
	uNewWinHeight = uOldWinHeight - uOldClnHeight + uHeight;

	UINT uShowPosX, uShowPosY;
	uShowPosX = (GetSystemMetrics(SM_CXSCREEN) - uNewWinWidth) / 2;
	uShowPosY = (GetSystemMetrics(SM_CYSCREEN) - uNewWinHeight) / 2;

	MoveWindow(hWnd, uShowPosX, uShowPosY, uNewWinWidth, uNewWinHeight, false);
}


