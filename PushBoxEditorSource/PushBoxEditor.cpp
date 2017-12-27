#include "PushBoxEditor.h"
#include <stdio.h>

#define STATUSILLEGAL 0
#define LEVELILLEGAL 1
#define SAVESTATUSSUCCEED 2

//关卡存储
static int _iLevelMax = 0;
static short _MapData[20][400];

//当前关卡（关卡）
static int _iLevelPresent = 0;
static short _Status[20][20];

//资源存储
HBITMAP _hBitMap[7];

static void SetStatusByLevel(HWND hWnd, int level);
static bool LoadImgResource();
static int SaveStatusToMapData();
static void ClearStatus();
static void AutoSetWall();
static bool DrawStatus(HDC hdc);
static void ChangeStatus(HWND hWnd, int row, int col);
static bool StatusIsLegal();

/*-----------------------------------------------------------------------------------------------------------------------*/
/*----------------------------------------程序主体使用的函数集合---------------------------------------------------------*/
/*-----------------------------------------------------------------------------------------------------------------------*/


LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	HDC hdc;
	PAINTSTRUCT paintStruct;
	int xPos, yPos;
	int saveStatusResult;

	switch (message)
	{
	case WM_CREATE:return 0;
	case WM_SIZE:SetClientSize(hWnd, 600, 600);return 0;

	case WM_KEYUP:
		switch (wParam)
		{
		case VK_UP:
			SetStatusByLevel(hWnd, _iLevelPresent - 1);
			break;
		case VK_DOWN:
			SetStatusByLevel(hWnd, _iLevelPresent + 1);
			break;
		case VK_SHIFT:
			ClearStatus();
			InvalidateRect(hWnd, NULL, true);
			break;
		case VK_INSERT:
			//将当前编辑关卡数据存入MapData
			saveStatusResult = SaveStatusToMapData();
			switch (saveStatusResult)
			{
			case STATUSILLEGAL:
				MessageBox(NULL, _TEXT("关卡不合法"), _TEXT("WARNING"), MB_OK | MB_ICONWARNING);
				break;
			case LEVELILLEGAL:
				MessageBox(NULL, _TEXT("关卡数超出界限"), _TEXT("WARNING"), MB_OK | MB_ICONWARNING);
				break;
			case SAVESTATUSSUCCEED:
				MessageBox(NULL, _TEXT("已记录当前编辑关卡"), _TEXT(""), MB_OK);
				break;
			}
			break;
		}
		return 0;

	case WM_LBUTTONUP:
		yPos = HIWORD(lParam);
		xPos = LOWORD(lParam);
		if (yPos > 600 || xPos > 600)
			return 0;
		ChangeStatus(hWnd, yPos / 30, xPos / 30);
		return 0;

	case WM_PAINT:
		hdc = BeginPaint(hWnd, &paintStruct);
		if (!DrawStatus(hdc))
			PostQuitMessage(-1);
		EndPaint(hWnd, &paintStruct);
		return 0;

	case WM_DESTROY:
		//存储关卡数据至文件
		if (!StoreMapData())
		{
			MessageBox(NULL, _TEXT("存储关卡数据至文件时失败"), _TEXT("Error"), MB_OK | MB_ICONERROR);
			PostQuitMessage(-1);
		}
		PostQuitMessage(0);
		return 0;
	}
	return DefWindowProc(hWnd, message, wParam, lParam);
}


//从文件中加载关卡数据、贴图资源，并初始化编辑器状态
int LoadMapData(HWND hWnd)
{
	//加载关卡数据
	FILE *file;
	fopen_s(&file, "PushBoxData.xw", "r");
	if (file == NULL)
	{
		MessageBox(NULL, _TEXT("LoadMapData时打开关卡数据文件失败，尝试初始化关卡"), _TEXT("Error"), MB_OK | MB_ICONWARNING);
		return LM_FAIL_FILE;
	}
	fread_s(&_iLevelMax, sizeof(_iLevelMax), sizeof(_iLevelMax), 1, file);
	for (int level = 0;level < 20;++level)
		for (int i = 0;i < 400;++i)
			fread_s(&_MapData[level][i], sizeof(short), sizeof(short), 1, file);
	fclose(file);

	//加载贴图资源
	if (!LoadImgResource())
	{
		MessageBox(NULL, _TEXT("无法加载贴图资源"), _TEXT("Error"), MB_OK | MB_ICONERROR);
		return LM_FAIL_RESOURCE;
	}

	//令初始关卡为0
	SetStatusByLevel(hWnd, 0);

	return LM_SUCCEED;
}





//初始化关卡数据
void InitMapData()
{
	_iLevelMax = 4;
	//关卡0
	_iLevelPresent = 0;
	ClearStatus();
	_Status[8][9] = _Status[10][8] = _Status[9][10] = BLANK;
	_Status[7][9] = _Status[9][12] = _Status[10][7] = _Status[12][10] = TARGET;
	_Status[9][11] = _Status[9][9] = _Status[10][9] = _Status[11][10] = BOX;
	_Status[10][10] = MAN;
	AutoSetWall();
	SaveStatusToMapData();

	//关卡1
	_iLevelPresent = 1;
	ClearStatus();
	_Status[7][7] = MAN;
	_Status[9][13] = _Status[10][13] = _Status[11][13] = TARGET;
	_Status[8][8] = _Status[8][9] = _Status[9][8] = BOX;
	_Status[7][8] = _Status[7][9] = _Status[8][7] = _Status[9][7] = _Status[9][9] = BLANK;
	_Status[10][9] = _Status[11][9] = _Status[11][10] = _Status[11][11] = _Status[11][12] = BLANK;
	_Status[12][8] = _Status[12][9] = _Status[12][10] = _Status[12][12] = _Status[12][13] = BLANK;
	_Status[13][8] = _Status[13][9] = _Status[13][10] = BLANK;
	AutoSetWall();
	SaveStatusToMapData();

	//关卡2
	_iLevelPresent = 2;
	ClearStatus();
	_Status[9][7] = MAN;
	_Status[8][7] = _Status[9][9] = _Status[9][12] = _Status[10][11] = BOX;
	_Status[10][7] = _Status[10][8] = _Status[11][7] = _Status[11][8] = TARGET;

	_Status[7][7] = _Status[7][8] = _Status[7][9] = _Status[7][10] = _Status[7][11] = BLANK;
	_Status[8][11] = _Status[8][12] = _Status[8][13] = BLANK;
	_Status[9][6] = _Status[9][8] = _Status[9][10] = _Status[9][11] = _Status[9][13] = BLANK;
	_Status[10][6] = _Status[10][10] = _Status[10][12] = BLANK;
	_Status[11][10] = _Status[11][11] = _Status[11][12] = BLANK;
	AutoSetWall();
	SaveStatusToMapData();

	//关卡3
	_iLevelPresent = 3;
	ClearStatus();
	_Status[8][9] = MAN;
	_Status[8][10] = _Status[9][9] = _Status[10][10] = _Status[11][9] = BOX;
	_Status[12][10] = BOXONTARGET;
	_Status[12][11] = _Status[12][9] = _Status[12][8] = _Status[11][8] = TARGET;
	_Status[7][9] = _Status[7][10] = _Status[8][8] = BLANK;
	_Status[9][10] = _Status[10][9] = _Status[11][10] = BLANK;
	_Status[11][11] = _Status[10][11] = BLANK;
	AutoSetWall();
	SaveStatusToMapData();

	//关卡4
	_iLevelPresent = 4;
	ClearStatus();
	_Status[7][8] = MAN;
	_Status[8][9] = _Status[11][8] = _Status[12][11] = BOX;
	_Status[10][7] = _Status[11][7] = _Status[12][7] = TARGET;
	_Status[7][9] = _Status[8][8] = _Status[8][10] = _Status[8][11] = BLANK;
	_Status[9][9] = _Status[9][11] = _Status[10][9] = _Status[10][11] = _Status[10][12] = BLANK;
	_Status[11][9] = _Status[11][10] = _Status[11][12] = BLANK;
	_Status[12][8] = _Status[12][9] = _Status[12][10] = _Status[12][12] = BLANK;
	AutoSetWall();
	SaveStatusToMapData();
}

//存储关卡数据至文件
bool StoreMapData()
{
	FILE *file;
	fopen_s(&file, "PushBoxData.xw", "w");
	if (file == NULL)
	{
		fclose(file);
		return false;
	}

	fwrite(&_iLevelMax, sizeof(_iLevelMax), 1, file);
	for (int level = 0;level < 20;++level)
		for (int i = 0;i < 400;++i)
			fwrite(&_MapData[level][i], sizeof(short), 1, file);
	fclose(file);
	return true;
}




/*-----------------------------------------------------------------------------------------------------------------------*/
/*----------------------------------------文件内部使用的函数集合---------------------------------------------------------*/
/*-----------------------------------------------------------------------------------------------------------------------*/

//根据所选关卡设置当前关卡状态
static void SetStatusByLevel(HWND hWnd, int level)
{
	if (level < 0 || level>19)
		return;
	_iLevelPresent = level;
	int pos = 0;
	for (int row = 0;row<20;++row)
		for (int col = 0;col < 20;++col)
			_Status[row][col] = _MapData[level][pos++];
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


//将当前编辑关卡存进_MapData，level说明关卡，此函数只在本文件内使用
static int SaveStatusToMapData()
{
	if (_iLevelPresent < 0 || _iLevelPresent>_iLevelMax+1)
		return LEVELILLEGAL;

	if (!StatusIsLegal())
		return STATUSILLEGAL;
	int pos = 0;
	for(int row=0;row<20;++row)
		for (int col = 0;col < 20;++col)
			_MapData[_iLevelPresent][pos++] = _Status[row][col];
	if (_iLevelPresent == _iLevelMax + 1)
		_iLevelMax = _iLevelPresent;
	return SAVESTATUSSUCCEED;
}

//对未设置墙的关卡自动设置墙的位置，限于本文件内使用
static void AutoSetWall()
{
	for(int row=0;row<20;++row)
		for (int col = 0;col < 20;++col)
			if (_Status[row][col] != WALL&&_Status[row][col] != BACKGROUND)
			{
				if (row > 0 && _Status[row - 1][col] == BACKGROUND)
					_Status[row - 1][col] = WALL;
				if (row < 19 && _Status[row + 1][col] == BACKGROUND)
					_Status[row + 1][col] = WALL;
				if (col > 0 && _Status[row][col - 1] == BACKGROUND)
					_Status[row][col - 1] = WALL;
				if (col < 19 && _Status[row][col + 1] == BACKGROUND)
					_Status[row][col + 1] = WALL;
			}
			
	for(int row=0;row<20;++row)
		for(int col=0;col<20;++col)
			if (_Status[row][col] == WALL)
			{
				//左侧为背景，左上为墙
				if (col > 0 && row > 0)
				{
					if (_Status[row - 1][col] != WALL&&_Status[row - 1][col] != BACKGROUND&&_Status[row][col - 1] == BACKGROUND&&_Status[row - 1][col - 1] == WALL)
						_Status[row][col - 1] = WALL;
				}
				//左侧为背景，左下为墙
				if (col > 0 && row < 19)
				{
					if (_Status[row + 1][col] != WALL&&_Status[row + 1][col] != BACKGROUND&&_Status[row][col - 1] == BACKGROUND&&_Status[row + 1][col - 1] == WALL)
						_Status[row][col - 1] = WALL;
				}
				//右侧为背景，右上为墙
				if (col < 19 && row > 0)
				{
					if (_Status[row - 1][col] != WALL&&_Status[row - 1][col] != BACKGROUND&&_Status[row][col + 1] == BACKGROUND&&_Status[row - 1][col + 1] == WALL)
						_Status[row][col + 1] = WALL;
				}
				//右侧为背景，右下为墙
				if (col < 19 && row < 19)
				{
					if (_Status[row + 1][col] != WALL&&_Status[row + 1][col] != BACKGROUND&&_Status[row][col + 1] == BACKGROUND&&_Status[row + 1][col + 1] == WALL)
						_Status[row][col + 1] = WALL;
				}
			}
}

//清除当前关卡，限于本文件内使用
static void ClearStatus()
{
	for (int row = 0;row < 20;++row)
		for (int col = 0;col < 20;++col)
			_Status[row][col] = BACKGROUND;
}

//根据_Status绘制关卡，限于本文件内使用
static bool DrawStatus(HDC hdc)
{
	static HDC _mDC = CreateCompatibleDC(hdc);
	static TCHAR LevelString[10];
	wsprintf(LevelString, _TEXT("当前关卡：%d"), _iLevelPresent+1);
	SetBkMode(hdc, TRANSPARENT);
	for(int row=0;row<20;++row)
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
			default:
				MessageBoxPrintf(NULL, MB_OK | MB_ICONERROR, _TEXT("Error"), _TEXT("_Status[%d][%d] is %d"), row, col, _Status[row][col]);
				return false;
			}
		}
	TextOut(hdc, 0, 0, LevelString, lstrlen(LevelString));
	return true;
}

//改变当前关卡状态
static void ChangeStatus(HWND hWnd, int row, int col)
{
	_Status[row][col] = (_Status[row][col] + 1) % 7;
	InvalidateRect(hWnd, NULL, true);
}

//判断当前编辑关卡是否合法
static bool StatusIsLegal()
{
	int manNum = 0, targetNum = 0, boxNum = 0;

	for(int row=0;row<20;++row)
		for (int col = 0;col < 20;++col)
		{
			if (_Status[row][col] == MAN)
				++manNum;
			else if (_Status[row][col] == TARGET)
				++targetNum;
			else if (_Status[row][col] == BOX)
				++boxNum;
			else if (_Status[row][col] == BOXONTARGET)
			{	++boxNum;++targetNum;	}

			if (_Status[row][col] != WALL&&_Status[row][col] != BACKGROUND)
			{
				if (row < 1 || row>18 || col < 1 || col>18)
					return false;
				if (_Status[row - 1][col] == BACKGROUND || _Status[row + 1][col] == BACKGROUND
					|| _Status[row][col - 1] == BACKGROUND || _Status[row][col + 1] == BACKGROUND)
					return false;
				if (_Status[row - 1][col] == WALL && _Status[row + 1][col] == WALL
					&& _Status[row][col - 1] == WALL && _Status[row][col + 1] == WALL)
					return false;
			}
		}
	if (manNum != 1)
		return false;
	if (targetNum != boxNum)
		return false;

	return true;
}