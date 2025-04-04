
#include <windows.h>
#include "resource.h"

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
HBITMAP hBitmap;

HWND hTextBox;

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE, LPSTR, int nCmdShow) {
    const wchar_t CLASS_NAME[] = L"MyWideWindowClass";

    WNDCLASS wc = {};
    wc.lpfnWndProc = WndProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = CLASS_NAME;
    wc.hCursor = LoadCursor(nullptr, IDC_ARROW);
    wc.hbrBackground = nullptr; // No default background
    RegisterClass(&wc);

    HWND hwnd = CreateWindow(
        CLASS_NAME, L"My Wide Window",
        WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX,
        CW_USEDEFAULT, CW_USEDEFAULT,
        1000, 500,
        nullptr, nullptr, hInstance, nullptr
    );

    hTextBox = CreateWindowW(
        L"EDIT", L"",
        WS_CHILD | WS_VISIBLE | WS_VSCROLL | ES_MULTILINE | ES_AUTOVSCROLL | ES_LEFT | ES_WANTRETURN | WS_BORDER | ES_READONLY,
        0, 0, 0, 0, // We'll resize it in WM_SIZE
        hwnd, nullptr, hInstance, nullptr
    );

    // Set text
    SetWindowTextW(hTextBox, L"What Happend To This Computer?\r\n\r\nAll your company data, including your operating system has been encrypted and can be accessed again if you a ransome. You can't access anything on this machine but this screen.\r\n\r\nWhat Can You Do?\r\n\r\nNothing, just pay!\r\n\r\nHow Did This Happen?\r\n\r\nWe are expert hackers, we can break in any system which is vulnerable! Don't think you can hide from us!");

    if (!hwnd) return 0;

    ShowWindow(hwnd, nCmdShow);
    UpdateWindow(hwnd);

    // Load the bitmap from resource
    hBitmap = (HBITMAP)LoadImage(hInstance, MAKEINTRESOURCE(IDB_BITMAP1), IMAGE_BITMAP, 0, 0, LR_CREATEDIBSECTION);

    MSG msg = {};
    while (GetMessage(&msg, nullptr, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
    return 0;
}

void DrawCenteredText(HDC hdc, LPCWSTR text, RECT rect, int fontSize) {
    HFONT hFont = CreateFontW(fontSize, 0, 0, 0, FW_BOLD, FALSE, FALSE, FALSE,
        DEFAULT_CHARSET, OUT_OUTLINE_PRECIS, CLIP_DEFAULT_PRECIS,
        CLEARTYPE_QUALITY, VARIABLE_PITCH, L"Consolas");

    HFONT oldFont = (HFONT)SelectObject(hdc, hFont);
    SetBkMode(hdc, TRANSPARENT);
    SetTextColor(hdc, RGB(255, 255, 255)); // White text
    DrawTextW(hdc, text, -1, &rect, DT_CENTER | DT_TOP | DT_SINGLELINE);
    SelectObject(hdc, oldFont);
    DeleteObject(hFont);
}


LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    switch (msg) {
    case WM_PAINT: {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hwnd, &ps);

        RECT clientRect;
        GetClientRect(hwnd, &clientRect);

        // Fill custom background color: #7A2A1E
        HBRUSH hCustomBrush = CreateSolidBrush(RGB(122, 42, 30)); // #7A2A1E
        FillRect(hdc, &clientRect, hCustomBrush);
        DeleteObject(hCustomBrush);

        // Draw top title
        RECT topTitleRect = { 0, 10, clientRect.right, 60 };
        DrawCenteredText(hdc, L"Whoops, your computer has been infected with fake ransomware!", topTitleRect, 32);

        // Draw image in square with white border
        if (hBitmap) {
            HDC hMemDC = CreateCompatibleDC(hdc);
            HBITMAP oldBmp = (HBITMAP)SelectObject(hMemDC, hBitmap);

            BITMAP bmp;
            GetObject(hBitmap, sizeof(BITMAP), &bmp);

            int imageSize = 300;
            int imageX = 20;
            int imageY = (clientRect.bottom - imageSize) / 2;

            // Draw white border
            HBRUSH hWhite = CreateSolidBrush(RGB(255, 255, 255));
            RECT borderRect = { imageX - 2, imageY - 2, imageX + imageSize + 2, imageY + imageSize + 2 };
            FillRect(hdc, &borderRect, hWhite);
            DeleteObject(hWhite);

            // Stretch and draw image into square
            StretchBlt(hdc, imageX, imageY, imageSize, imageSize, hMemDC, 0, 0, bmp.bmWidth, bmp.bmHeight, SRCCOPY);

            SelectObject(hMemDC, oldBmp);
            DeleteDC(hMemDC);
        }

        // Draw bottom title
        RECT bottomTitleRect = { 0, clientRect.bottom - 60, clientRect.right, clientRect.bottom - 20 };
        DrawCenteredText(hdc, L"To decrypt your files, please transfer 100k in euro's to NL03 INGB05 0001 9429 21", bottomTitleRect, 24);

        EndPaint(hwnd, &ps);
        break;
    }
    case WM_SIZE: {
        RECT rc;
        GetClientRect(hwnd, &rc);

        int padding = 20;
        int imageSize = 300;
        int imageX = padding;
        int imageY = (rc.bottom - imageSize) / 2;

        int textX = imageX + imageSize + padding;
        int textWidth = rc.right - textX - padding;
        int textHeight = imageSize;

        MoveWindow(hTextBox, textX, imageY, textWidth, textHeight, TRUE);
        break;
    }
    case WM_DESTROY:
        if (hBitmap) DeleteObject(hBitmap);
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hwnd, msg, wParam, lParam);
    }
    return 0;
}