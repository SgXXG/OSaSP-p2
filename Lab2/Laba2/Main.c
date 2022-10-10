#include <windows.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>

#define M             6
#define N             6
#define ID_FIRSTCHILD 1

typedef struct _TABLE {
	HWND window;
	int _windowHeight;
	char* text;
} TABLE;

TABLE Table[M][N];
HINSTANCE hInst;
int windowHeight = 20, windowWidth;
BOOL update = FALSE;

LRESULT CALLBACK WndProc(HWND hWnd, UINT message,
	WPARAM wParam, LPARAM lParam);

void CalcWidth(RECT RCClient);
int CalcCy(int ind);
void FindIndex(HWND hwnd, int* firstInd, int* secondInd);
void UpdateText(HWND hwnd);
//char* GenerateString(int count);

int APIENTRY WinMain(HINSTANCE hInstance,
	HINSTANCE hPrevInstance, LPTSTR lpCmdLine, int nCmdShow)
{
	WNDCLASSEX wcex; HWND hWnd; MSG msg;
	hInst = hInstance;
	wcex.cbSize = sizeof(WNDCLASSEX);
	wcex.style = 0;
	wcex.lpfnWndProc = WndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = hInstance;
	wcex.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wcex.lpszMenuName = NULL;
	wcex.lpszClassName = "MainClass";
	wcex.hIconSm = wcex.hIcon;

	RegisterClassEx(&wcex);

	hWnd = CreateWindow("MainClass", "Laba2",
		WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, 0,
		CW_USEDEFAULT, 0, NULL, NULL, hInstance, NULL);

	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);

	while (GetMessage(&msg, NULL, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	return (int)msg.wParam;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message,
	WPARAM wParam, LPARAM lParam)
{
	WNDCLASSEX wcexChild;
	RECT rcClient;
	RECT rcChild;

	switch (message)
	{
	case WM_GETMINMAXINFO:
	{
		MINMAXINFO* pInfo = (MINMAXINFO*)lParam;
		POINT ptMin = { 600, 600 }, ptMax = { 1920, 1080 };
		pInfo->ptMinTrackSize = ptMin;
		pInfo->ptMaxTrackSize = ptMax;
		return 0;
	}

	case WM_SIZE:
		GetClientRect(hWnd, &rcClient);
		CalcWidth(rcClient);
		for (int i = 0; i < M; i++) {
			for (int j = 0; j < N; j++) {
				UpdateText(Table[i][j].window);
			}
		}
		break;
	case WM_CREATE:
		GetClientRect(hWnd, &rcClient);
		CalcWidth(rcClient);

		for (int i = 0; i < M; i++) {
			for (int j = 0; j < N; j++) {
				Table[i][j].window = CreateWindowEx(0, "Edit", (LPCTSTR)NULL,
					WS_CHILD | WS_BORDER | WS_VISIBLE | ES_MULTILINE, j * windowWidth, i * windowHeight,
					windowWidth, windowHeight, hWnd, (HMENU)(int)(ID_FIRSTCHILD + i*M + j), hInst, NULL);
				Table[i][j]._windowHeight = 50;
				Table[i][j].text = (char*)(malloc(100));
				/*int count = rand() % 200;
				char* str = GenerateString(count);*/
				strcpy_s(Table[i][j].text, 100, "hello");

				SetWindowText(Table[i][j].window, Table[i][j].text);
				ShowWindow(Table[i][j].window, SW_NORMAL);
				UpdateWindow(Table[i][j].window);
			}
		}

		for (int i = 0; i < M; i++) {
			for (int j = 0; j < N; j++) {
				SetWindowText(Table[i][j].window, Table[i][j].text);
			}
		}
		
		break;
	case WM_COMMAND:
		if (HIWORD(wParam) == EN_CHANGE) {
			UpdateText((HWND)lParam);
			update = TRUE;
		}

		if (HIWORD(wParam) == EN_SETFOCUS && update) {
			update = FALSE;
			InvalidateRect(hWnd, NULL, TRUE);
		}
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

char* GenerateString(int count)
{
	char s[201] = { 0 };
	for (int i = 0; i < count; ++i) 
		s[i] = rand() % 26 + 'a';
	return s;
}

void CalcWidth(RECT RCClient) {
	windowWidth = (int)(RCClient.right / N);
}

void FindIndex(HWND hwnd, int* firstInd, int* secondInd) {
	for (int i = 0; i < M; i++) {
		for (int j = 0; j < N; j++) {
			if (hwnd == Table[i][j].window) {
				*firstInd = i;
				*secondInd = j;
				return;
			}
		}
	}
}

void UpdateText(HWND hwnd) {
	SIZE size;
	HDC hdc;
	RECT rcChildRect;
	int newI = 0, newJ = 0;
	int cy = 0;
	int linesCount = 0;
	FindIndex(hwnd, &newI, &newJ);

	hdc = GetDC(hwnd);
	GetWindowText(hwnd, Table[newI][newJ].text, 1000);
	GetClientRect(hwnd, &rcChildRect);
	
	for (int j = 0; j < N; j++) {
		Table[newI][j]._windowHeight = DrawText(hdc, Table[newI][j].text, strlen(Table[newI][j].text),
			&rcChildRect, DT_VCENTER | DT_EDITCONTROL | DT_WORDBREAK | DT_NOCLIP | DT_CALCRECT) * 1.1;
	}
	
	if (Table[newI][newJ].text[strlen(Table[newI][newJ].text) - 1] == '\n') {
		Table[newI][newJ]._windowHeight *= 1.009;
	}
	
	int maxHeight = 0;
	for (int j = 0; j < N; j++) {
		if (Table[newI][j]._windowHeight > maxHeight) {
			maxHeight = Table[newI][j]._windowHeight;
		}
	}

	for (int j = 0; j < N; j++) {
		Table[newI][j]._windowHeight = maxHeight;
	}
	
	for (int i = 0; i < M; i++) {
		if (i != 0) {
			cy = CalcCy(i);
		}
		
		for (int j = 0; j < N; j++) {
			SetWindowPos(Table[i][j].window, NULL, j * windowWidth, cy-1,
				windowWidth, Table[i][j]._windowHeight, SWP_SHOWWINDOW);
		}
	}
	ReleaseDC(hwnd, hdc);
}

int CalcCy(int ind) {
	int cy = 0;
	for (int i = 0; i < ind; i++) {
		cy += Table[i][0]._windowHeight;
	}
	return cy;
}