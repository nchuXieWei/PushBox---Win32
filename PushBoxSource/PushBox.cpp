#include <stdio.h>
#include "PushBox.h"


//地图存储
static int _iLevelMax = 0;
static short _MapData[20][400];

//当前关卡（地图）
static int _iLevelPresent = 0;
static short _Status[20][20];
static short _xPos, _yPos;

//资源存储
HBITMAP _hBitMap[7];

static void SetStatusByLevel(HWND hWnd, int level);
static bool LoadImgResource();
static bool DrawStatus(HDC hdc);
static bool MoveMan(WPARAM key);
static bool MoveManUp();
static bool MoveManDown();
static bool MoveManLeft();
static bool MoveManRight();
static bool StatusComplete();
static void GameOver();

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	HDC hdc;
	PAINTSTRUCT paintStruct;

	switch (message)
	{
	case WM_CREATE:return 0;
	case WM_SIZE:SetClientSize(hWnd, 600, 600);return 0;

	case WM_KEYUP:
		if (wParam == VK_SHIFT)
		{
			SetStatusByLevel(hWnd, _iLevelPresent);
			return 0;
		}
		if (MoveMan(wParam))
		{
			InvalidateRect(hWnd, NULL, true);
			if (StatusComplete())
			{
				if (_iLevelPresent != _iLevelMax)
				{
					MessageBoxPrintf(NULL, MB_OK, NULL, _TEXT("恭喜通过第%2d关！"), _iLevelPresent+1);
					SetStatusByLevel(hWnd, ++_iLevelPresent);
				}
				else
					GameOver();
			}

		}
		return 0;

	case WM_PAINT:
		hdc = BeginPaint(hWnd, &paintStruct);
		if (!DrawStatus(hdc))
			PostQuitMessage(-1);
		EndPaint(hWnd, &paintStruct);
		return 0;

	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;
	}
	return DefWindowProc(hWnd, message, wParam, lParam);
}


//从文件中加载地图数据、贴图资源，并初始化编辑器状态
bool LoadMapData(HWND hWnd)
{
	//加载地图数据
	FILE *file;
	fopen_s(&file, "PushBoxData.xw", "r");
	if (file == NULL)
	{
		MessageBox(NULL, _TEXT("Open file fail when LoadMapData"), _TEXT("Error"), MB_OK | MB_ICONWARNING);
		return false;
	}
	fread_s(&_iLevelMax, sizeof(_iLevelMax), sizeof(_iLevelMax), 1, file);
	for (int level = 0;level < 20;++level)
		for (int i = 0;i < 400;++i)
			fread_s(&_MapData[level][i], sizeof(short), sizeof(short), 1, file);
	fclose(file);

	//加载贴图资源
	if (!LoadImgResource())
	{
		MessageBox(NULL, _TEXT("Can't load Resource"), _TEXT("Error"), MB_OK | MB_ICONERROR);
		return false;
	}

	//令初始关卡为0
	SetStatusByLevel(hWnd, 0);

	return true;
}



//根据_Status绘制地图，限于本文件内使用
static bool DrawStatus(HDC hdc)
{
	static HDC _mDC = CreateCompatibleDC(hdc);
	static TCHAR LevelString[10];
	wsprintf(LevelString, _TEXT("当前关卡：%d"), _iLevelPresent+1);
	SetBkMode(hdc, TRANSPARENT);
	for (int row = 0;row<20;++row)
		for (int col = 0;col < 20;++col)
		{
			switch (_Status[row][col])
			{
			case MAN:
				SelectObject(_mDC, _hBitMap[MAN]);
				BitBlt(hdc, col * 30, row * 30, 30, 30, _mDC, 0, 0, SRCCOPY);
				break;
			case BOX:
				SelectObject(_mDC, _hBitMap[BOX]);
				BitBlt(hdc, col * 30, row * 30, 30, 30, _mDC, 0, 0, SRCCOPY);
				break;
			case BOXONTARGET:
				SelectObject(_mDC, _hBitMap[BOXONTARGET]);
				BitBlt(hdc, col * 30, row * 30, 30, 30, _mDC, 0, 0, SRCCOPY);
				break;
			case BLANK:
				SelectObject(_mDC, _hBitMap[BLANK]);
				BitBlt(hdc, col * 30, row * 30, 30, 30, _mDC, 0, 0, SRCCOPY);
				break;
			case TARGET:
				SelectObject(_mDC, _hBitMap[TARGET]);
				BitBlt(hdc, col * 30, row * 30, 30, 30, _mDC, 0, 0, SRCCOPY);
				break;
			case WALL:
				SelectObject(_mDC, _hBitMap[WALL]);
				BitBlt(hdc, col * 30, row * 30, 30, 30, _mDC, 0, 0, SRCCOPY);
				break;
			case BACKGROUND:
				SelectObject(_mDC, _hBitMap[BACKGROUND]);
				BitBlt(hdc, col * 30, row * 30, 30, 30, _mDC, 0, 0, SRCCOPY);
				break;
			case MANONTARGET:
				SelectObject(_mDC, _hBitMap[MAN]);
				BitBlt(hdc, col * 30, row * 30, 30, 30, _mDC, 0, 0, SRCCOPY);
				break;
			default:
				MessageBoxPrintf(NULL, MB_OK | MB_ICONERROR, _TEXT("Error"), _TEXT("_Status[%d][%d] is %d"), row, col, _Status[row][col]);
				return false;
			}
		}
	TextOut(hdc, 0, 0, LevelString, lstrlen(LevelString));
	return true;
}


//根据所选关卡设置当前地图状态
static void SetStatusByLevel(HWND hWnd, int level)
{
	if (level < 0 || level>_iLevelMax)
		return;
	_iLevelPresent = level;
	int pos = 0;
	for (int row = 0;row<20;++row)
		for (int col = 0;col < 20;++col)
		{
			_Status[row][col] = _MapData[level][pos++];
			if (_Status[row][col] == MAN)
			{
				_xPos = col;_yPos = row;
			}
		}
	InvalidateRect(hWnd, NULL, true);
}


static bool LoadImgResource()
{
	for (int i = BACKGROUND;i <= BOXONTARGET;++i)
		_hBitMap[i] = NULL;

	_hBitMap[BACKGROUND] = (HBITMAP)LoadImage(NULL, _TEXT("Image\\BACKGROUND.bmp"), IMAGE_BITMAP, 30, 30, LR_LOADFROMFILE);
	_hBitMap[WALL] = (HBITMAP)LoadImage(NULL, _TEXT("Image\\WALL.bmp"), IMAGE_BITMAP, 30, 30, LR_LOADFROMFILE);
	_hBitMap[MAN] = (HBITMAP)LoadImage(NULL, _TEXT("Image\\MAN.bmp"), IMAGE_BITMAP, 30, 30, LR_LOADFROMFILE);
	_hBitMap[BOX] = (HBITMAP)LoadImage(NULL, _TEXT("Image\\BOX.bmp"), IMAGE_BITMAP, 30, 30, LR_LOADFROMFILE);
	_hBitMap[BLANK] = (HBITMAP)LoadImage(NULL, _TEXT("Image\\BLANK.bmp"), IMAGE_BITMAP, 30, 30, LR_LOADFROMFILE);
	_hBitMap[TARGET] = (HBITMAP)LoadImage(NULL, _TEXT("Image\\TARGET.bmp"), IMAGE_BITMAP, 30, 30, LR_LOADFROMFILE);
	_hBitMap[BOXONTARGET] = (HBITMAP)LoadImage(NULL, _TEXT("Image\\BOXONTARGET.bmp"), IMAGE_BITMAP, 30, 30, LR_LOADFROMFILE);
	for (int i = BACKGROUND;i <= BOXONTARGET;++i)
		if (_hBitMap[i] == NULL)
			return false;
	return true;
}

static bool MoveMan(WPARAM key)
{
	switch (key)
	{
		//向上走
	case VK_UP:
		return MoveManUp();
		//向下走
	case VK_DOWN:
		return MoveManDown();
		//向左走
	case VK_LEFT:
		return MoveManLeft();
		//向右走
	case VK_RIGHT:
		return MoveManRight();
	}
	return false;
}

//将人物上移，若无移动则返回false
static bool MoveManUp()
{
	//JustMan用于判断MAN脚下是BLANK还是TARGET，为true表示脚下是BLANK
	bool JustMan = false;
	if (_Status[_yPos][_xPos] == MAN)
		JustMan = true;

	//如果上方是BLANK
	if (_Status[_yPos - 1][_xPos] == BLANK)
	{
		_Status[_yPos - 1][_xPos] = MAN;
		_Status[_yPos][_xPos] = JustMan ? BLANK : TARGET;
		--_yPos;
		return true;
	}
	//如果上方是TARGET
	else if (_Status[_yPos - 1][_xPos] == TARGET)
	{
		_Status[_yPos - 1][_xPos] = MANONTARGET;
		_Status[_yPos][_xPos] = JustMan ? BLANK : TARGET;
		--_yPos;
		return true;
	}
	//如果上方是BOX
	else if (_Status[_yPos - 1][_xPos] == BOX)
	{
		//上方为BOX，若上上方为BLANK
		if (_Status[_yPos - 2][_xPos] == BLANK)
		{
			_Status[_yPos - 2][_xPos] = BOX;
			_Status[_yPos - 1][_xPos] = MAN;
			_Status[_yPos][_xPos] = JustMan ? BLANK : TARGET;
			--_yPos;
			return true;
		}
		//上方为BOX，若上上方为TARGET
		else if (_Status[_yPos - 2][_xPos] == TARGET)
		{
			_Status[_yPos - 2][_xPos] = BOXONTARGET;
			_Status[_yPos - 1][_xPos] = MAN;
			_Status[_yPos][_xPos] = JustMan ? BLANK : TARGET;
			--_yPos;
			return true;
		}
	}
	//若上方为BOXONTARGET
	else if (_Status[_yPos - 1][_xPos] == BOXONTARGET)
	{
		//上方为BOXONTARGET，若上上方为BLANK
		if (_Status[_yPos - 2][_xPos] == BLANK)
		{
			_Status[_yPos - 2][_xPos] = BOX;
			_Status[_yPos - 1][_xPos] = MANONTARGET;
			_Status[_yPos][_xPos] = JustMan ? BLANK : TARGET;
			--_yPos;
			return true;
		}
		//上方为BOXONTARGET，若上上方为TARGET
		else if (_Status[_yPos - 2][_xPos] == TARGET)
		{
			_Status[_yPos - 2][_xPos] = BOXONTARGET;
			_Status[_yPos - 1][_xPos] = MANONTARGET;
			_Status[_yPos][_xPos] = JustMan ? BLANK : TARGET;
			--_yPos;
			return true;
		}
	}

	return false;
}

//将人物下移，若无移动则返回false
static bool MoveManDown()
{
	//JustMan用于判断MAN脚下是BLANK还是TARGET，为true表示脚下是BLANK
	bool JustMan = false;
	if (_Status[_yPos][_xPos] == MAN)
		JustMan = true;

	//若下方是BLANK
	if (_Status[_yPos + 1][_xPos] == BLANK)
	{
		_Status[_yPos + 1][_xPos] = MAN;
		_Status[_yPos][_xPos] = JustMan ? BLANK : TARGET;
		++_yPos;
		return true;
	}
	//若下方是TARGET
	else if (_Status[_yPos + 1][_xPos] == TARGET)
	{
		_Status[_yPos + 1][_xPos] = MANONTARGET;
		_Status[_yPos][_xPos] = JustMan ? BLANK : TARGET;
		++_yPos;
		return true;
	}
	//若下方是BOX
	else if (_Status[_yPos + 1][_xPos] == BOX)
	{
		//下方是BOX，若下下方为BLANK
		if (_Status[_yPos + 2][_xPos] == BLANK)
		{
			_Status[_yPos + 2][_xPos] = BOX;
			_Status[_yPos + 1][_xPos] = MAN;
			_Status[_yPos][_xPos] = JustMan ? BLANK : TARGET;
			++_yPos;
			return true;
		}
		//下方是BOX，若下下方是TARGET
		else if (_Status[_yPos + 2][_xPos] == TARGET)
		{
			_Status[_yPos + 2][_xPos] = BOXONTARGET;
			_Status[_yPos + 1][_xPos] = MAN;
			_Status[_yPos][_xPos] = JustMan ? BLANK : TARGET;
			++_yPos;
			return true;
		}
	}
	//若下方是BOXONTARGET
	else if (_Status[_yPos + 1][_xPos] == BOXONTARGET)
	{
		//下方是BOXONTARGET，若下下方是BLANK
		if (_Status[_yPos + 2][_xPos] == BLANK)
		{
			_Status[_yPos + 2][_xPos] = BOX;
			_Status[_yPos + 1][_xPos] = MANONTARGET;
			_Status[_yPos][_xPos] = JustMan ? BLANK : TARGET;
			++_yPos;
			return true;
		}
		//下方是BOXONTARGET，若下下方是TARGET
		else if (_Status[_yPos + 2][_xPos] == TARGET)
		{
			_Status[_yPos + 2][_xPos] = BOXONTARGET;
			_Status[_yPos + 1][_xPos] = MANONTARGET;
			_Status[_yPos][_xPos] = JustMan ? BLANK : TARGET;
			++_yPos;
			return true;
		}
	}

	return false;
}

//将人物左移，若无移动则返回false
static bool MoveManLeft()
{
	//JustMan用于判断MAN脚下是BLANK还是TARGET，为true表示脚下是BLANK
	bool JustMan = false;
	if (_Status[_yPos][_xPos] == MAN)
		JustMan = true;

	//若左方是BLANK
	if (_Status[_yPos][_xPos - 1] == BLANK)
	{
		_Status[_yPos][_xPos - 1] = MAN;
		_Status[_yPos][_xPos] = JustMan ? BLANK : TARGET;
		--_xPos;
		return true;
	}
	//若左方是TARGET
	else if (_Status[_yPos][_xPos - 1] == TARGET)
	{
		_Status[_yPos][_xPos - 1] = MANONTARGET;
		_Status[_yPos][_xPos] = JustMan ? BLANK : TARGET;
		--_xPos;
		return true;
	}
	//若左方是BOX
	else if (_Status[_yPos][_xPos - 1] == BOX)
	{
		//左方是BOX，左左方是BLANK
		if (_Status[_yPos][_xPos - 2] == BLANK)
		{
			_Status[_yPos][_xPos - 2] = BOX;
			_Status[_yPos][_xPos - 1] = MAN;
			_Status[_yPos][_xPos] = JustMan ? BLANK : TARGET;
			--_xPos;
			return true;
		}
		//左方是BOX，左左方是TARGET
		else if (_Status[_yPos][_xPos - 2] == TARGET)
		{
			_Status[_yPos][_xPos - 2] = BOXONTARGET;
			_Status[_yPos][_xPos - 1] = MAN;
			_Status[_yPos][_xPos] = JustMan ? BLANK : TARGET;
			--_xPos;
			return true;
		}
	}
	//若左方是BOXONTARGET
	else if (_Status[_yPos][_xPos - 1] == BOXONTARGET)
	{
		//左方是BOXONTARGET，若左左方是BLANK
		if (_Status[_yPos][_xPos - 2] == BLANK)
		{
			_Status[_yPos][_xPos - 2] = BOX;
			_Status[_yPos][_xPos - 1] = MANONTARGET;
			_Status[_yPos][_xPos] = JustMan ? BLANK : TARGET;
			--_xPos;
			return true;
		}
		//左方是BOXONTARGET，若左左方是TARGET
		else if (_Status[_yPos][_xPos - 2] == TARGET)
		{
			_Status[_yPos][_xPos - 2] = BOXONTARGET;
			_Status[_yPos][_xPos - 1] = MANONTARGET;
			_Status[_yPos][_xPos] = JustMan ? BLANK : TARGET;
			--_xPos;
			return true;
		}
	}

	return false;
}

//将人物右移，若无移动则返回false
static bool MoveManRight()
{
	//JustMan用于判断MAN脚下是BLANK还是TARGET，为true表示脚下是BLANK
	bool JustMan = false;
	if (_Status[_yPos][_xPos] == MAN)
		JustMan = true;

	//若右方是BLANK
	if (_Status[_yPos][_xPos + 1] == BLANK)
	{
		_Status[_yPos][_xPos + 1] = MAN;
		_Status[_yPos][_xPos] = JustMan ? BLANK : TARGET;
		++_xPos;
		return true;
	}
	//若右方是TARGET
	else if (_Status[_yPos][_xPos + 1] == TARGET)
	{
		_Status[_yPos][_xPos + 1] = MANONTARGET;
		_Status[_yPos][_xPos] = JustMan ? BLANK : TARGET;
		++_xPos;
		return true;
	}
	//若右方是BOX
	else if (_Status[_yPos][_xPos + 1] == BOX)
	{
		//右方是BOX，若右右方是BLANK
		if (_Status[_yPos][_xPos + 2] == BLANK)
		{
			_Status[_yPos][_xPos + 2] = BOX;
			_Status[_yPos][_xPos + 1] = MAN;
			_Status[_yPos][_xPos] = JustMan ? BLANK : TARGET;
			++_xPos;
			return true;
		}
		//右方是BOX，若右右方是TARGET
		else if (_Status[_yPos][_xPos + 2] == TARGET)
		{
			_Status[_yPos][_xPos + 2] = BOXONTARGET;
			_Status[_yPos][_xPos + 1] = MAN;
			_Status[_yPos][_xPos] = JustMan ? BLANK : TARGET;
			++_xPos;
			return true;
		}
	}
	//若右方是BOXONTARGET
	else if (_Status[_yPos][_xPos + 1] == BOXONTARGET)
	{
		//右方是BOXONTARGET，若右右方是BLANK
		if (_Status[_yPos][_xPos + 2] == BLANK)
		{
			_Status[_yPos][_xPos + 2] = BOX;
			_Status[_yPos][_xPos + 1] = MANONTARGET;
			_Status[_yPos][_xPos] = JustMan ? BLANK : TARGET;
			++_xPos;
			return true;
		}
		//右方是BOXONTARGET，若右右方是TARGET
		else if (_Status[_yPos][_xPos + 2] == TARGET)
		{
			_Status[_yPos][_xPos + 2] = BOXONTARGET;
			_Status[_yPos][_xPos + 1] = MANONTARGET;
			_Status[_yPos][_xPos] = JustMan ? BLANK : TARGET;
			++_xPos;
			return true;
		}
	}

	return false;
}

bool StatusComplete()
{
	for (int row = 0;row < 20;++row)
		for (int col = 0;col < 20;++col)
			if (_Status[row][col] == BOX || _Status[row][col] == TARGET)
				return false;
	return true;
}

void GameOver()
{
	MessageBox(NULL, _TEXT("您已通关，恭喜！"), _TEXT("Game Over"), MB_OK);
	PostQuitMessage(0);
}
