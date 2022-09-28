#include "EntryPoint.h"
#include "stdio.h"

#define SPRITE_STOP_ACCEL 6
#define SPRITE_PULL_ACCEL 10
#define TIMER_UPDATE_TIME 10

HDC hdcBack;
HBITMAP hbmBack;
RECT rcClient;
HDC spriteImageBlack;
HDC spriteImage;
HDC bckImage;
HWND hWnd;
Image image = { 0, 0, 0, 0};

BOOL isUp = FALSE, isLeft = FALSE, isDown = FALSE, isRight = FALSE;
int rightAccel = 0 , leftAccel = 0, upAccel = 0, downAccel = 0;

void InitializeBackBuffer(HWND hWnd, int width, int height)
{
	HDC hdcWindow;
	hdcWindow = GetDC(hWnd);
	hdcBack = CreateCompatibleDC(hdcWindow);
	hbmBack = CreateCompatibleBitmap(hdcWindow, width, height);
	ReleaseDC(hWnd, hdcWindow);

	SaveDC(hdcBack);
	SelectObject(hdcBack, hbmBack);
}

void FinalizeBackBuffer()
{
	if (hdcBack)
	{
		RestoreDC(hdcBack, -1);
		DeleteObject(hbmBack);
		DeleteDC(hdcBack);
		hdcBack = 0;
	}
}

HDC LoadBitmapDC(HWND hwnd, char* fileName)
{
	HANDLE hBitmap = LoadImageW(0, fileName, IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
	HDC hdc = GetDC(hwnd);
	HDC resultDC = CreateCompatibleDC(hdc);
	SelectObject(resultDC, hBitmap);
	ReleaseDC(0, hdc);

	return resultDC;
}

void Init(HWND hwnd)
{
	bckImage = LoadBitmapDC(hwnd, L"Back.bmp");
	spriteImageBlack = LoadBitmapDC(hwnd, L"blackSprite.bmp");
	//spriteImage = LoadBitmapDC(hwnd, L"1.bmp");
	spriteImage = LoadBitmapDC(hwnd, L"cat.bmp\0");
	//spriteImage = LoadBitmapDC(hwnd, L"rectangle.bmp\0");
}

void Draw()
{
	PAINTSTRUCT ps;
	FillRect(hdcBack, &ps.rcPaint, (HBRUSH)(COLOR_WINDOW + 1));
	GdiTransparentBlt(hdcBack, 0, 0, rcClient.right, rcClient.bottom, bckImage, 0, 0, 1338, 770, 1);
	//BitBlt(hdcBack, image.xoriginDest / 10, image.yoriginDest / 10, 100, 100, spriteImageBlack, 0, 0, SRCCOPY);
	//BitBlt(hdcBack, image.xoriginDest / 10, image.yoriginDest / 10, 100, 100, spriteImage, 0, 0, SRCPAINT);
	GdiTransparentBlt(hdcBack, image.xoriginDest / 10, image.yoriginDest / 10, 100, 100, spriteImage, 0, 0, 100, 100, 0);
}

INT WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR lpCmdLine, INT nCmdShow)
{
	WNDCLASSEX wcex =
	{
		sizeof(WNDCLASSEX), 0, MyWindowProc, 0, 0, hInstance, 0, LoadCursor(0, IDC_HAND),
		(HBRUSH)(COLOR_WINDOW + 1), NULL, L"1", 0
	};
	MSG msg;

	RegisterClassEx(&wcex);

	hWnd = CreateWindowEx(WS_EX_ACCEPTFILES, L"1", L"1", WINDOW_STYLE,
		800, 600,
		WINDOW_WIDTH, WINDOW_HEIGHT, 0, 0, hInstance, NULL);

	SetTimer(hWnd, FIRST_TIMER, TIMER_UPDATE_TIME, NULL);

	while (GetMessage(&msg, 0, 0, 0))
	{
		DispatchMessage(&msg);
	}

	KillTimer(hWnd, FIRST_TIMER);

	return msg.wParam;
}

LRESULT CALLBACK MyWindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	PAINTSTRUCT ps;
	HDC hDC; 
	HBRUSH hBmp;
	LPWSTR lpszFile = NULL;

	switch (uMsg)
	{				
		HANDLE_MSG(hWnd, WM_CREATE, Cls_OnCreate);
		HANDLE_MSG(hWnd, WM_SIZE, Cls_OnSize);
		HANDLE_MSG(hWnd, WM_PAINT, Cls_OnPaint);
		HANDLE_MSG(hWnd, WM_DESTROY, Cls_OnDestroy);
		HANDLE_MSG(hWnd, WM_KEYDOWN, Cls_OnKey);
		HANDLE_MSG(hWnd, WM_KEYUP, Cls_OnKey);		
		HANDLE_MSG(hWnd, WM_MOUSEWHEEL, Cls_OnMouseWheel);
		HANDLE_MSG(hWnd, WM_TIMER, Cls_OnTimer);
	}
	return DefWindowProc(hWnd, uMsg, wParam, lParam);
}

void Cls_OnDestroy(HWND hwnd)
{
	PostQuitMessage(0);
	FinalizeBackBuffer();
}

BOOL Cls_OnCreate(HWND hwnd, LPCREATESTRUCT lpCreateStruct)
{
	Init(hwnd);
}

void Cls_OnSize(HWND hwnd, UINT state, int cx, int cy)
{
	GetClientRect(hwnd, &rcClient);
	FinalizeBackBuffer();
	InitializeBackBuffer(hwnd, cx, cy);
	InvalidateRect(hwnd, NULL, FALSE);
}

void Cls_OnPaint(HWND hwnd)
{	
	PAINTSTRUCT ps;
	HDC hDC;

	Draw();

	hDC = BeginPaint(hwnd, &ps);
	BitBlt(hDC, 0, 0, rcClient.right, rcClient.bottom, hdcBack, 0, 0, SRCCOPY);
	EndPaint(hwnd, &ps);
}

void Cls_OnKey(HWND hwnd, UINT vk, BOOL fDown, int cRepeat, UINT flags)
{
	switch (vk)
	{
	case VK_UP:
		if (fDown) {
			isUp |= 0x0100;
		}
		else
			isUp &= ~0x0100;
		break;
	case VK_DOWN:
		if (fDown) {
			isDown |= 0x1000;
		}
		else
			isDown &= ~0x1000;
		break;
	case VK_LEFT:
		if (fDown) {
			isLeft |= 0x0010;
		}
		else
			isLeft &= ~0x0010;
		break;
	case VK_RIGHT:
		if (fDown) {
			isRight |= 0x0001;
		}
		else
		{
			isRight &= ~0x0001;
		}
		break;
	}
}

void Cls_OnMouseWheel(HWND hwnd, int xPos, int yPos, int zDelta, UINT fwKeys)
{
	if (isRight || isLeft || isUp || isDown)
		return;

	int wheelDelta = GET_WHEEL_DELTA_WPARAM(zDelta);

	if (fwKeys == MK_SHIFT) {
		image.xoriginDest += zDelta > 0 ? 30 : -30;
	}
	else {
		image.yoriginDest += zDelta > 0 ? -30 : 30;
	}


	InvalidateRect(hwnd, 0, 0);
}

void Cls_OnTimer(HWND hwnd, UINT id) {
	if (id == FIRST_TIMER) {
		if (isRight) {
			rightAccel = IncAccel(rightAccel);
		}
		else
		{
			rightAccel = DecAccel(rightAccel);
		}
		if (isLeft)
		{
			leftAccel = IncAccel(leftAccel);
		}
		else
		{
			leftAccel = DecAccel(leftAccel);
		}
		if (isUp) {
			upAccel = IncAccel(upAccel);
		}
		else {
			upAccel = DecAccel(upAccel);
		}
		if (isDown) {
			downAccel = IncAccel(downAccel);
		}
		else {
			downAccel = DecAccel(downAccel);
		}

		image.xoriginDest += (rightAccel - leftAccel);
		image.yoriginDest += (-upAccel + downAccel);

		if (image.xoriginDest < 0) {
			rightAccel += (int)(leftAccel*0.6);
			leftAccel = 0;
		}
		else if (image.xoriginDest > rcClient.right * 10 - IMAGE_SIZE * 10) {
			leftAccel += (int)(rightAccel * 0.6);
			rightAccel = 0;
		}

		if (image.yoriginDest < 0) {
			downAccel += (int)(upAccel * 0.6);
			upAccel = 0;
		}
		else if (image.yoriginDest > rcClient.bottom * 10 - IMAGE_SIZE * 10) {
			upAccel += (int)(downAccel * 0.6);
			downAccel = 0;
		}

		image.xoriginDest = max(0, min(image.xoriginDest, rcClient.right * 10 - IMAGE_SIZE * 10));
		image.yoriginDest = max(0, min(image.yoriginDest, rcClient.bottom * 10 - IMAGE_SIZE * 10));

		InvalidateRect(hWnd, NULL, FALSE);
	}
}

int IncAccel(int accel) {
	return (min(200, accel + 1));
}

int DecAccel(int accel) {
	int value = abs(accel);
	int sign = accel > 0 ? 1 : -1;

	value = max(0, value - 1);

	return (value * sign);
}