#include <windows.h>
#include <gdiplus.h>
#include <commctrl.h>
#include <string>
#include <vector>
#include <sstream>

#pragma comment(lib, "gdiplus.lib")
#pragma comment(lib, "Comctl32.lib")

using namespace Gdiplus;

static ULONG_PTR gdiplusToken;
static HWND gMain = nullptr;

// الألوان المستقبلية (Cyberpunk Dark Green Theme)
#define COLOR_BG        Color(255, 5, 10, 8)
#define COLOR_PANEL     Color(255, 12, 22, 16)
#define COLOR_BORDER    Color(255, 20, 50, 30)
#define COLOR_NEON      Color(255, 0, 255, 100)
#define COLOR_DIM_NEON  Color(255, 0, 150, 60)
#define COLOR_TEXT      Color(255, 0, 230, 90)
#define COLOR_TEXT_DIM  Color(255, 0, 120, 50)

void DrawCyberPanel(Graphics& g, INT x, INT y, INT w, INT h, const wchar_t* title = nullptr) {
    SolidBrush panelBrush(COLOR_PANEL);
    Pen borderPen(COLOR_BORDER, 1.5f);
    g.FillRectangle(&panelBrush, x, y, w, h);
    g.DrawRectangle(&borderPen, x, y, w, h);

    if (title) {
        Font font(L"Consolas", 10, FontStyleBold);
        SolidBrush textBrush(COLOR_TEXT);
        g.DrawString(title, -1, &font, PointF((REAL)(x + 10), (REAL)(y + 8)), &textBrush);
    }
}

void DrawReactorCore(Graphics& g, INT cx, INT cy, INT radius) {
    g.SetSmoothingMode(SmoothingModeAntiAlias);

    Pen neonPen(COLOR_NEON, 3.0f);
    Pen dimPen(COLOR_DIM_NEON, 1.0f);

    g.DrawEllipse(&dimPen, cx - radius, cy - radius, radius * 2, radius * 2);
    g.DrawEllipse(&neonPen, cx - (radius - 15), cy - (radius - 15), (radius - 15) * 2, (radius - 15) * 2);

    Pen dashedPen(COLOR_NEON, 2.0f);
    dashedPen.SetDashStyle(DashStyleDash);
    g.DrawEllipse(&dashedPen, cx - (radius - 30), cy - (radius - 30), (radius - 30) * 2, (radius - 30) * 2);

    Font fontTitle(L"Segoe UI", 22, FontStyleBold);
    SolidBrush neonBrush(COLOR_NEON);
    
    StringFormat sf;
    sf.SetAlignment(StringAlignmentCenter);
    sf.SetLineAlignment(StringAlignmentCenter);

    g.DrawString(L"G-X\nCORE", -1, &fontTitle, PointF((REAL)cx, (REAL)(cy - 10)), &sf, &neonBrush);
}

void OnPaint(HDC hdc, RECT& rc) {
    Bitmap buffer(rc.right, rc.bottom);
    Graphics g(&buffer);
    g.SetSmoothingMode(SmoothingModeAntiAlias);

    SolidBrush bgBrush(COLOR_BG);
    g.FillRectangle(&bgBrush, 0, 0, (INT)rc.right, (INT)rc.bottom);

    // 1. Header Bar
    DrawCyberPanel(g, 10, 10, rc.right - 20, 50);
    Font headerFont(L"Consolas", 14, FontStyleBold);
    SolidBrush neonBrush(COLOR_NEON);
    g.DrawString(L"G-X CORE  v3.0.0 | G-X Reactor Core System", -1, &headerFont, PointF(25.0f, 22.0f), &neonBrush);

    // 2. Terminal Screen
    DrawCyberPanel(g, 10, 70, 320, 480, L"> G-X TERMINAL");
    Font termFont(L"Consolas", 9, FontStyleRegular);
    SolidBrush textBrush(COLOR_TEXT);
    SolidBrush textDimBrush(COLOR_TEXT_DIM);

    REAL ty = 100.0f;
    g.DrawString(L"[23:45:18] G-X CORE INITIALIZING...", -1, &termFont, PointF(20.0f, ty), &textDimBrush); ty += 20.0f;
    g.DrawString(L"[23:45:18] SYSTEM SECURE [ OK ]", -1, &termFont, PointF(20.0f, ty), &textBrush); ty += 20.0f;
    g.DrawString(L"[23:45:19] CHECKING INTEGRITY...", -1, &termFont, PointF(20.0f, ty), &textDimBrush); ty += 20.0f;
    g.DrawString(L"[23:45:20] SCANNING CRITICAL AREAS...", -1, &termFont, PointF(20.0f, ty), &textBrush); ty += 20.0f;
    g.DrawString(L"  > SYSTEM FILES         [ OK ]", -1, &termFont, PointF(20.0f, ty), &textBrush); ty += 20.0f;
    g.DrawString(L"  > REGISTRY             [ OK ]", -1, &termFont, PointF(20.0f, ty), &textBrush); ty += 20.0f;
    g.DrawString(L"  > DRIVERS              [ OK ]", -1, &termFont, PointF(20.0f, ty), &textBrush); ty += 20.0f;
    g.DrawString(L"[23:45:24] 0 THREATS FOUND", -1, &termFont, PointF(20.0f, ty), &textBrush); ty += 20.0f;
    g.DrawString(L"[23:45:29] MODULE EXECUTED SUCCESS", -1, &termFont, PointF(20.0f, ty), &textBrush);

    // 3. Reactor Core
    DrawCyberPanel(g, 340, 70, 380, 480);
    Font statusFont(L"Consolas", 12, FontStyleBold);
    StringFormat sfCenter; sfCenter.SetAlignment(StringAlignmentCenter);
    g.DrawString(L"REACTOR STATUS\nEXECUTING", -1, &statusFont, PointF(530.0f, 90.0f), &sfCenter, &neonBrush);
    
    DrawReactorCore(g, 530, 290, 120);

    g.DrawString(L"PROCESS PROGRESS: 67%", -1, &termFont, PointF(360.0f, 470.0f), &neonBrush);
    SolidBrush progBg(COLOR_BORDER);
    SolidBrush progFill(COLOR_NEON);
    g.FillRectangle(&progBg, 360, 495, 340, 15);
    g.FillRectangle(&progFill, 360, 495, 228, 15);

    // 4. System Monitor
    DrawCyberPanel(g, 730, 70, 280, 480, L"SYSTEM MONITOR");
    INT my = 100;
    g.DrawString(L"CPU USAGE: 32%", -1, &termFont, PointF(745.0f, (REAL)my), &neonBrush); my += 20;
    g.FillRectangle(&progBg, 745, my, 250, 10);
    g.FillRectangle(&progFill, 745, my, 80, 10); my += 25;

    g.DrawString(L"MEMORY: 61% (9.7 GB / 15.9 GB)", -1, &termFont, PointF(745.0f, (REAL)my), &neonBrush); my += 20;
    g.FillRectangle(&progBg, 745, my, 250, 10);
    g.FillRectangle(&progFill, 745, my, 152, 10); my += 25;

    g.DrawString(L"DISK ACTIVITY: 27%", -1, &termFont, PointF(745.0f, (REAL)my), &neonBrush); my += 20;
    g.FillRectangle(&progBg, 745, my, 250, 10);
    g.FillRectangle(&progFill, 745, my, 67, 10); my += 35;

    g.DrawString(L"NETWORK SPEED", -1, &termFont, PointF(745.0f, (REAL)my), &neonBrush); my += 20;
    g.DrawString(L"↑ 12.4 MB/s   ↓ 3.7 MB/s", -1, &termFont, PointF(745.0f, (REAL)my), &textBrush); my += 35;

    g.DrawString(L"TEMPERATURES", -1, &termFont, PointF(745.0f, (REAL)my), &neonBrush); my += 20;
    g.DrawString(L"CPU: 44°C   GPU: 44°C   MAIN: 37°C", -1, &termFont, PointF(745.0f, (REAL)my), &textBrush);

    // 5. Footer Bar
    DrawCyberPanel(g, 10, 560, rc.right - 20, 40);
    g.DrawString(L"G-X CORE v3.0.0 | ALL SYSTEMS OPERATIONAL | ENCRYPTION: AES-256 | BUILT BY G-X ENGINE", -1, &termFont, PointF(25.0f, 572.0f), &textDimBrush);

    Graphics screenGraphics(hdc);
    screenGraphics.DrawImage(&buffer, 0, 0);
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    switch (msg) {
    case WM_PAINT: {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hwnd, &ps);
        RECT rc; GetClientRect(hwnd, &rc);
        OnPaint(hdc, rc);
        EndPaint(hwnd, &ps);
        break;
    }
    case WM_ERASEBKGND:
        return 1;
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hwnd, msg, wParam, lParam);
    }
    return 0;
}

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE, PWSTR, int nCmdShow) {
    GdiplusStartupInput gdiplusStartupInput;
    GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);

    WNDCLASSW wc = {};
    wc.lpfnWndProc = WndProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = L"GXCyberCoreClass";
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);

    RegisterClassW(&wc);

    gMain = CreateWindowExW(
        0, L"GXCyberCoreClass", L"G-X CORE - Cybernetic Interface",
        WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX,
        CW_USEDEFAULT, CW_USEDEFAULT, 1040, 650,
        NULL, NULL, hInstance, NULL
    );

    ShowWindow(gMain, nCmdShow);
    UpdateWindow(gMain);

    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    GdiplusShutdown(gdiplusToken);
    return 0;
}
