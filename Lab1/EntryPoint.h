#include <Windows.h>
#include <WindowsX.h>

#define STEP 5
#define IMAGE_SIZE 100
#define WINDOW_STYLE (WS_VISIBLE | WS_OVERLAPPEDWINDOW)
#define WINDOW_WIDTH 1065
#define WINDOW_HEIGHT 650
#define FIRST_TIMER 1

typedef struct _Image
{
	int xoriginDest;
	int yoriginDest;
	int vx;
	int vy;
} Image;

LRESULT CALLBACK MyWindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
void Init(HWND hwnd);
int IncAccel(int accel);
int DecAccel(int accel);
void Draw();
void Cls_OnDestroy(HWND hwnd);
BOOL Cls_OnCreate(HWND hwnd, LPCREATESTRUCT lpCreateStruct);
void Cls_OnSize(HWND hwnd, UINT state, int cx, int cy);
void Cls_OnPaint(HWND hwnd);
void Cls_OnKey(HWND hwnd, UINT vk, BOOL fDown, int cRepeat, UINT flags);
void Cls_OnMouseWheel(HWND hwnd, int xPos, int yPos, int zDelta, UINT fwKeys);
void Cls_OnTimer(HWND hwnd, UINT id);