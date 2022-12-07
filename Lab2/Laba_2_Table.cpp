#define _CRT_SECURE_NO_WARNINGS
#include "framework.h"
#include "Laba_2_Table.h"
#include "windowsx.h"
#include "math.h"
#include "commctrl.h"

#define MAX_LOADSTRING 100
#define NUM_OF_COLUMNS 5
#define NUM_OF_ROWS 3

HINSTANCE hInst;                              
WCHAR szTitle[MAX_LOADSTRING];                  
WCHAR szWindowClass[MAX_LOADSTRING];            

ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);

const wchar_t SrcFilePath[] = L"D:\\!Studing\\3 Course\\5 Sem\\OSaSP\\Laba_2_Table\\Laba_2_Table\\x64\\Debug\\TableInfo\\text.txt";
char** data;
RECT windowSize;
HWND* cells;
HDC hdcWindow;
HDC HdcBuffer;
HANDLE HandleBuffer;

char** ParseFile(HANDLE);
void DrawTable(HWND, char**, bool);
int CountWidth();
void ResizeCell(HWND, HWND, int, int, RECT);

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    // TODO: Разместите код здесь.

    // Инициализация глобальных строк
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_LABA2TABLE, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    // Выполнить инициализацию приложения:
    if (!InitInstance (hInstance, nCmdShow))
    {
        return FALSE;
    }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_LABA2TABLE));

    MSG msg;

    // Цикл основного сообщения:
    while (GetMessage(&msg, nullptr, 0, 0))
    {
        if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }

    return (int) msg.wParam;
}

//
//  ФУНКЦИЯ: MyRegisterClass()
//
//  ЦЕЛЬ: Регистрирует класс окна.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
    WNDCLASSEXW wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);
    wcex.style = 0;
    wcex.lpfnWndProc    = WndProc;
    wcex.cbClsExtra     = 0;
    wcex.cbWndExtra     = 0;
    wcex.hInstance      = hInstance;
    wcex.hIcon          = LoadIcon(nullptr, IDI_APPLICATION);
    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
    wcex.lpszMenuName   = nullptr;
    wcex.lpszClassName  = szWindowClass;
    wcex.hIconSm        = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassExW(&wcex);
}

//
//   ФУНКЦИЯ: InitInstance(HINSTANCE, int)
//
//   ЦЕЛЬ: Сохраняет маркер экземпляра и создает главное окно
//
//   КОММЕНТАРИИ:
//
//        В этой функции маркер экземпляра сохраняется в глобальной переменной, а также
//        создается и выводится главное окно программы.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   hInst = hInstance;

   HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
      CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, nullptr, nullptr, hInstance, nullptr);

   if (!hWnd)
   {
      return FALSE;
   }

   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

   return TRUE;
}

//
//  ФУНКЦИЯ: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  ЦЕЛЬ: Обрабатывает сообщения в главном окне.
//
//  WM_COMMAND  - обработать меню приложения
//  WM_PAINT    - Отрисовка главного окна
//  WM_DESTROY  - отправить сообщение о выходе и вернуться
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_CREATE:
        {
            GetClientRect(hWnd, &windowSize);
            HANDLE file = CreateFile(SrcFilePath, GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
            if (INVALID_HANDLE_VALUE == file) 
            {
                MessageBox(hWnd, L"File not found!", L"Error", MB_OK);
            }
            else 
            {
                hdcWindow = GetDC(hWnd);
                HdcBuffer = CreateCompatibleDC(hdcWindow);
                HandleBuffer = CreateCompatibleBitmap(hdcWindow, windowSize.right - windowSize.left, windowSize.bottom - windowSize.top);
                SaveDC(HdcBuffer);
                SelectObject(HdcBuffer, HandleBuffer);
                ReleaseDC(hWnd, hdcWindow);

                data = ParseFile(file);
                DrawTable(hWnd, data, true);
                free(data);
                CloseHandle(file);
            }          
        }
        break;
    case WM_SIZE: 
        {
            GetClientRect(hWnd, &windowSize);
            DrawTable(hWnd, nullptr, false);
        }
        break;
    case WM_COMMAND:
        {
            int wmId = LOWORD(wParam);  
            switch (HIWORD(wParam))
            {
                case EN_KILLFOCUS:
                    GetClientRect(hWnd, &windowSize);
                    DrawTable(hWnd, nullptr, false);
                    break;
            }
        }
        break;
    case WM_PAINT:
        {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hWnd, &ps);
            // TODO: Добавьте сюда любой код прорисовки, использующий HDC...
            EndPaint(hWnd, &ps);
        }
        break;
    case WM_DESTROY:
        if (HdcBuffer)
        {
            // Восстанавливает контекст устройства (DC) до указанного состояния
            // -1 - последнее сохранённое
            RestoreDC(HdcBuffer, -1);
            DeleteObject(HandleBuffer);
            DeleteObject(HdcBuffer);
        }
        free(cells);
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

void ResizeCell(HWND hWnd, HWND parenthWnd, int width, int height, RECT sizeCell)
{
    wchar_t text[1024];
    GetWindowText(hWnd, text, 1024);

    MoveWindow(hWnd, sizeCell.left, sizeCell.top, width, height,true);
    SetWindowText(hWnd, text);
    memset(text, 0, sizeof(wchar_t) * 1024);
}

int CountWidth()
{
    return round((windowSize.right - windowSize.left) / NUM_OF_COLUMNS);
}

char** ParseFile(HANDLE file)
{
    short cellsAmount = NUM_OF_COLUMNS * NUM_OF_ROWS;

    char** result = (char**)calloc(cellsAmount, sizeof(char*));
    for (int i = 0; i < cellsAmount; i++)
        result[i] = (char*)calloc(255, sizeof(char));

    DWORD bytesToReadAmount = 50;
    for (int i = 0; i < cellsAmount; i++)
    {
        DWORD dwByte;
        DWORD bytesToReadAmount = rand() % 254 + 4;
        
        ReadFile(file, result[i], bytesToReadAmount, &dwByte, NULL);
        result[i][dwByte - 2] = '\0';
    }

    return result;
}

void DrawTable(HWND hWnd, char** data, bool isCreate)
{
    int widthCell = CountWidth();
    
    RECT sizeCell = {0,0,widthCell,0};

    short cellsAmount = NUM_OF_COLUMNS * NUM_OF_ROWS;

    if (isCreate)
        cells = (HWND*)calloc(cellsAmount, sizeof(HWND));
    
    int counterCell = 0;
    for(int i = 0; i < NUM_OF_ROWS; i++)
    {
        int maxHeightInRow = 0;
        int counterLines = counterCell;
        for (int j = 0; j < NUM_OF_COLUMNS; j++)
        {
            int currHeight = 0;
            if (isCreate) {
                wchar_t* textBuf = (wchar_t*)calloc(strlen(data[counterLines]) + 1, sizeof(wchar_t));
                mbstowcs(textBuf, data[counterLines], strlen(data[counterLines]) + 1);
                currHeight = DrawText(HdcBuffer, textBuf, wcslen(textBuf), &sizeCell, DT_LEFT | DT_EDITCONTROL | DT_WORDBREAK |
                    DT_NOCLIP) + 10;
                free(textBuf);
            }
            else
            {
                wchar_t textBuff[1024];
                GetWindowText(cells[counterLines], textBuff, 1024);
                currHeight = DrawText(HdcBuffer, textBuff, wcslen(textBuff), &sizeCell, DT_LEFT | DT_EDITCONTROL | DT_WORDBREAK |
                    DT_NOCLIP) + 10;
                memset(textBuff, 0, sizeof(wchar_t) * 1024);
            }
          
            if (maxHeightInRow < currHeight)
                maxHeightInRow = currHeight;
            counterLines++;
        }

        for (int j = 0; j < NUM_OF_COLUMNS; j++)
        {
            if (isCreate) {
                wchar_t* text = (wchar_t*)calloc(strlen(data[counterCell]) + 1, sizeof(wchar_t));
                mbstowcs(text, data[counterCell], strlen(data[counterCell]) + 1);

                cells[counterCell] = CreateWindowEx(0, L"edit", text, WS_BORDER | WS_CHILD | WS_VISIBLE | ES_MULTILINE | ES_AUTOVSCROLL,
                    sizeCell.left, sizeCell.top, widthCell, maxHeightInRow,
                    hWnd, NULL, NULL, NULL);
                free(text);
            }
            else
            {
                ResizeCell(cells[counterCell], hWnd, widthCell,maxHeightInRow, sizeCell);
            }
            counterCell++;

            sizeCell.left = sizeCell.right;
            sizeCell.right = sizeCell.left + widthCell;
        }       

        sizeCell.left = 0;
        sizeCell.right = sizeCell.left + widthCell;
        sizeCell.top += maxHeightInRow;   
    }
}