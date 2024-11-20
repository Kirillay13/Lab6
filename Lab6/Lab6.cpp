// Lab6.cpp : Определяет точку входа для приложения.

#include "framework.h"
#include "Lab6.h"
#include <stdio.h>
#include <stdlib.h>

#define MAX_LOADSTRING 100

// Глобальные переменные:
HINSTANCE hInst;                                // текущий экземпляр
WCHAR szTitle[MAX_LOADSTRING];                  // Текст строки заголовка
WCHAR szWindowClass[MAX_LOADSTRING];            // имя класса главного окна

// Отправить объявления функций, включенных в этот модуль кода:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);

int* readArrayFromFile(const char* filename, int* size) {
    FILE* file;
    errno_t err = fopen_s(&file, filename, "r");
    if (err != 0) {
        MessageBox(NULL, L"Не удалось открыть файл", L"Ошибка", MB_OK);
        return NULL;
    }

    if (fscanf_s(file, "%d", size) != 1) {
        MessageBox(NULL, L"Ошибка чтения размера массива", L"Ошибка", MB_OK);
        fclose(file);
        return NULL;
    }

    int* array = (int*)malloc(*size * sizeof(int));
    if (array == NULL) {
        MessageBox(NULL, L"Ошибка выделения памяти", L"Ошибка", MB_OK);
        fclose(file);
        return NULL;
    }

    for (int i = 0; i < *size; i++) {
        if (fscanf_s(file, "%d", &array[i]) != 1) {
            MessageBox(NULL, L"Ошибка чтения элемента массива", L"Ошибка", MB_OK);
            free(array);
            fclose(file);
            return NULL;
        }
    }

    fclose(file);
    return array;
}

void findMaxMin(int* mas, int n, int* max_index, int* min_index) {
    *min_index = 0;
    *max_index = 0;
    for (int i = 0; i < n; i++) {
        if (mas[i] < mas[*min_index]) {
            *min_index = i;
        }
        if (mas[i] > mas[*max_index]) {
            *max_index = i;
        }
    }
}


void MaxMinDel(int* mas, int* n)
{
    int min_index = 0, max_index = 0;
    for (int i = 0; i < *n; i++) {
        if (mas[i] < mas[min_index]) {
            min_index = i;
        }
        if (mas[i] > mas[max_index]) {
            max_index = i;
        }
    }

    int start = (min_index < max_index) ? min_index : max_index;
    int end = (min_index < max_index) ? max_index : min_index;

    int j = 0;
    for (int i = 0; i < *n; i++) {
        if (i >= start && i <= end && mas[i] % 2 != 0) {
            mas[j] = mas[i];
            j++;
        }
    }

    *n = j;
}

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
    _In_opt_ HINSTANCE hPrevInstance,
    _In_ LPWSTR    lpCmdLine,
    _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    // Инициализация глобальных строк
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_LAB6, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    // Выполнить инициализацию приложения:
    if (!InitInstance(hInstance, nCmdShow))
    {
        return FALSE;
    }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_LAB6));

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

    return (int)msg.wParam;
}

ATOM MyRegisterClass(HINSTANCE hInstance)
{
    WNDCLASSEXW wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc = WndProc;
    wcex.cbClsExtra = 0;
    wcex.cbWndExtra = 0;
    wcex.hInstance = hInstance;
    wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_LAB6));
    wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wcex.lpszMenuName = MAKEINTRESOURCEW(IDC_LAB6);
    wcex.lpszClassName = szWindowClass;
    wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassExW(&wcex);
}

BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
    hInst = hInstance; // Сохранить маркер экземпляра в глобальной переменной

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

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_COMMAND:
    {
        int wmId = LOWORD(wParam);
        // Разобрать выбор в меню:
        switch (wmId)
        {
        case IDM_ABOUT:
            DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
            break;
        case IDM_EXIT:
            DestroyWindow(hWnd);
            break;
        default:
            return DefWindowProc(hWnd, message, wParam, lParam);
        }
    }
    break;
    case WM_PAINT:
    {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hWnd, &ps);

        int size;
        int* array = readArrayFromFile("D:\\PROGRAMS\\Lab6\\Lab6\\Lab6\\input.txt", &size);

        if (array != NULL) {
            // Копия массива
            int newSize = size;
            int* transformedArray = (int*)malloc(size * sizeof(int));
            memcpy(transformedArray, array, size * sizeof(int));

            // Параметры отрисовки
            int x = 100;          
            int baseY1 = 200;     
            int baseY2 = 400;     
            int width = 40;       
            int gap = 10;         

            
            int max_index, min_index;
            findMaxMin(array, size, &max_index, &min_index);

            // Базовые линии
            MoveToEx(hdc, 50, baseY1, NULL);
            LineTo(hdc, 600, baseY1);
            MoveToEx(hdc, 50, baseY2, NULL);
            LineTo(hdc, 600, baseY2);

            // Подписи для массивов
//            TextOut(hdc, 50, baseY1 - 150, L"Исходный массив:", 16);
//            TextOut(hdc, 50, baseY2 - 150, L"Преобразованный массив:", 22);

            // Исходный массив
            x = 100;
            for (int i = 0; i < size; i++) {
                HBRUSH hBrush;
                if (i == max_index) {
                    hBrush = CreateSolidBrush(RGB(255, 0, 0));
                }
                else if (i == min_index) {
                    hBrush = CreateSolidBrush(RGB(0, 255, 0));
                }
                else {
                    hBrush = CreateSolidBrush(RGB(0, 0, 255 - i * 40));
                }

                SelectObject(hdc, hBrush);

                Rectangle(hdc,
                    x, baseY1 - array[i] * 20,
                    x + width, baseY1
                );

                WCHAR number[10];
                wsprintf(number, L"%d", array[i]);
                TextOut(hdc, x + width / 4, baseY1 - array[i] * 20 - 20, number, wcslen(number));

                DeleteObject(hBrush);
                x += width + gap;
            }

            // Преобразование к копии массива
            MaxMinDel(transformedArray, &newSize);

            // Преобразованный массив
            x = 100;
            for (int i = 0; i < newSize; i++) {
                HBRUSH hBrush = CreateSolidBrush(RGB(0, 0, 255 - i * 40));
                SelectObject(hdc, hBrush);

                Rectangle(hdc,
                    x, baseY2 - transformedArray[i] * 20,
                    x + width, baseY2
                );

                WCHAR number[10];
                wsprintf(number, L"%d", transformedArray[i]);
                TextOut(hdc, x + width / 4, baseY2 - transformedArray[i] * 20 - 20, number, wcslen(number));

                DeleteObject(hBrush);
                x += width + gap;
            }

            free(array);
            free(transformedArray);
        }

        EndPaint(hWnd, &ps);
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

// Обработчик сообщений для окна "О программе".
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(lParam);
    switch (message)
    {
    case WM_INITDIALOG:
        return (INT_PTR)TRUE;

    case WM_COMMAND:
        if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
        {
            EndDialog(hDlg, LOWORD(wParam));
            return (INT_PTR)TRUE;
        }
        break;
    }
    return (INT_PTR)FALSE;
}
