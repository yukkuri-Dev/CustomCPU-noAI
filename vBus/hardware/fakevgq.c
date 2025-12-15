#include <windows.h>
#include <stdint.h>
#include <stdio.h>
#include "fakevgq.h"

// Simple fake VGA: memory is 128x128 cells (16384 words). Each word is a color index.
#define VG_WIDTH_CELLS 64
#define VG_HEIGHT_CELLS 64
#define CELL_PIX 5
#define WIN_WIDTH (VG_WIDTH_CELLS * CELL_PIX)
#define WIN_HEIGHT (VG_HEIGHT_CELLS * CELL_PIX)

static HWND g_hwnd = NULL;
static vbus_device_t *g_dev = NULL;

static COLORREF palette[16] = {
    RGB(0,0,0), RGB(128,0,0), RGB(0,128,0), RGB(128,128,0),
    RGB(0,0,128), RGB(128,0,128), RGB(0,128,128), RGB(192,192,192),
    RGB(128,128,128), RGB(255,0,0), RGB(0,255,0), RGB(255,255,0),
    RGB(0,0,255), RGB(255,0,255), RGB(0,255,255), RGB(255,255,255)
};

static LRESULT CALLBACK FakeVgqWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    switch (msg) {
    case WM_PAINT: {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hwnd, &ps);
        if (g_dev && g_dev->mem) {
            for (int y = 0; y < VG_HEIGHT_CELLS; ++y) {
                for (int x = 0; x < VG_WIDTH_CELLS; ++x) {
                    int idx = y * VG_WIDTH_CELLS + x;
                    uint16_t val = g_dev->mem[idx] & 0xF;
                    COLORREF c = palette[val];
                    HBRUSH br = CreateSolidBrush(c);
                    RECT r = { x * CELL_PIX, y * CELL_PIX, (x+1) * CELL_PIX, (y+1) * CELL_PIX };
                    FillRect(hdc, &r, br);
                    DeleteObject(br);
                }
            }
        }
        EndPaint(hwnd, &ps);
        return 0;
    }
    case WM_DESTROY:
        PostQuitMessage(0);
        return 0;
    }
    return DefWindowProc(hwnd, msg, wParam, lParam);
}

static DWORD WINAPI WindowThread(LPVOID param) {
    HINSTANCE hinst = GetModuleHandle(NULL);
    const char *clsName = "FakeVGQClass";

    WNDCLASS wc = {0};
    wc.lpfnWndProc = FakeVgqWndProc;
    wc.hInstance = hinst;
    wc.lpszClassName = clsName;

    RegisterClass(&wc);

    g_hwnd = CreateWindowA(clsName, "FakeVGQ", WS_OVERLAPPEDWINDOW,
                          CW_USEDEFAULT, CW_USEDEFAULT, WIN_WIDTH+16, WIN_HEIGHT+39,
                          NULL, NULL, hinst, NULL);
    if (!g_hwnd) return 1;

    ShowWindow(g_hwnd, SW_SHOW);

    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0) > 0) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
    return 0;
}

void fakevgq_init(uint16_t dev_id) {
    // If window already created, nothing to do
    if (g_hwnd != NULL) return;

    // If device already exists, use it; otherwise add it
    int idx = vbus_device_fetch(dev_id);
    if (idx == -1) {
        vbus_list_add(dev_id, "fakevgq", VG_WIDTH_CELLS * VG_HEIGHT_CELLS);
        idx = vbus_device_fetch(dev_id);
        if (idx == -1) {
            fprintf(stderr, "[fakevgq] failed to fetch device after add\n");
            return;
        }
    }
    g_dev = &vbus_list.devices[idx];

    // start window thread (only once)
    if (g_hwnd == NULL) {
        HANDLE th = CreateThread(NULL, 0, WindowThread, NULL, 0, NULL);
        if (!th) {
            fprintf(stderr, "[fakevgq] CreateThread failed\n");
        } else {
            CloseHandle(th);
        }
    }
}

void fakevgq_tick(vbus_device_t *dev) {
    // trigger repaint
    if (g_hwnd) {
        InvalidateRect(g_hwnd, NULL, TRUE);
        UpdateWindow(g_hwnd);
    }
}
