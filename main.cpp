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

// الدعم التلقائي للرسم المستقبلي
void DrawCyberPanel(Graphics& g, int x, int y, int w, int h, const wchar_t* title = nullptr) {
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

// رسم المفاعل المركزي (G-X Reactor Core)
void DrawReactorCore(Graphics& g, int cx, int cy, int radius) {
    g.SetSmoothingMode(SmoothingModeAntiAlias);

    // الحلقات الخارجية والداخلية
    Pen neonPen(COLOR_NEON, 3);
    Pen dimPen(COLOR_DIM_NEON, 1);

    g.DrawEllipse(&dimPen, cx - radius, cy - radius, radius * 2, radius * 2);
    g.DrawEllipse(&neonPen, cx - (radius - 15), cy - (radius - 15), (radius - 15) * 2, (radius - 15) * 2);

    Pen dashedPen(COLOR_NEON, 2);
    dashedPen.SetDashStyle(DashStyleDash);
    g.DrawEllipse(&dashedPen, cx - (radius - 30), cy - (radius - 30), (radius - 30) * 2, (radius - 30) * 2);

    // النص المركزي المضيء
    Font fontTitle(L"Segoe UI", 22, FontStyleBold);
    Font fontSub(L"Segoe UI", 10, FontStyleRegular);
    SolidBrush neonBrush(COLOR_NEON);
    
    StringFormat sf;
    sf.SetAlignment(StringAlignmentCenter);
    sf.SetLineAlignment(StringAlignmentCenter);

    g.DrawString(L"G-X\nCORE", -1, &fontTitle, PointF((REAL)cx, (REAL)(cy - 10)), &sf, &neonBrush);
}

// رسم الواجهة الكاملة
void OnPaint(HDC hdc, RECT& rc) {
    Bitmap buffer(rc.right, rc.bottom);
    Graphics g(&buffer);
    g.SetSmoothingMode(SmoothingModeAntiAlias);

    // الخلفية العامة
    SolidBrush bgBrush(COLOR_BG);
    g.FillRectangle(&bgBrush, 0, 0, rc.right, rc.bottom);

    // 1. الشريط العلوي (Header Bar)
    DrawCyberPanel(g, 10, 10, rc.right - 20, 50);
    Font headerFont(L"Consolas", 14, FontStyleBold);
    SolidBrush neonBrush(COLOR_NEON);
    g.DrawString(L"G-X CORE  v3.0.0 | G-X Reactor Core System", -1, &headerFont, PointF(25, 22), &neonBrush);

    // 2. شاشة التيرمينال اليسرى (Terminal Screen)
    DrawCyberPanel(g, 10, 70, 320, 480, L"> G-X TERMINAL");
    Font termFont(L"Consolas", 9, FontStyleRegular);
    SolidBrush textBrush(COLOR_TEXT);
    SolidBrush textDimBrush(COLOR_TEXT_DIM);

    int ty = 100;
    g.DrawString(L"[23:45:18] G-X CORE INITIALIZING...", -1, &termFont, PointF(20, (REAL)ty), &textDimBrush); ty += 20;
    g.DrawString(L"[23:45:18] SYSTEM SECURE [ OK ]", -1, &termFont, PointF(20, (REAL)ty), &textBrush); ty += 20;
    g.DrawString(L"[23:45:19] CHECKING INTEGRITY...", -1, &termFont, PointF(20, (REAL)ty), &textDimBrush); ty += 20;
    g.DrawString(L"[23:45:20] SCANNING CRITICAL AREAS...", -1, &termFont, PointF(20, (REAL)ty), &textBrush); ty += 20;
    g.DrawString(L"  > SYSTEM FILES         [ OK ]", -1, &termFont, PointF(20, (REAL)ty), &textBrush); ty += 20;
    g.DrawString(L"  > REGISTRY             [ OK ]", -1, &termFont, PointF(20, (REAL)ty), &textBrush); ty += 20;
    g.DrawString(L"  > DRIVERS              [ OK ]", -1, &termFont, PointF(20, (REAL)ty), &textBrush); ty += 20;
    g.DrawString(L"[23:45:24] 0 THREATS FOUND", -1, &termFont, PointF(20, (REAL)ty), &textBrush); ty += 20;
    g.DrawString(L"[23:45:29] MODULE EXECUTED SUCCESS", -1, &termFont, PointF(20, (REAL)ty), &textBrush);

    // 3. المفاعل المركزي (Center Reactor Core)
    DrawCyberPanel(g, 340, 70, 380, 480);
    Font statusFont(L"Consolas", 12, FontStyleBold);
    StringFormat sfCenter; sfCenter.SetAlignment(StringAlignmentCenter);
    g.DrawString(L"REACTOR STATUS\nEXECUTING", -1, &statusFont, PointF(530, 90), &sfCenter, &neonBrush);
    
    DrawReactorCore(g, 530, 290, 120);

    // شريط التقدم في المفاعل
    g.DrawString(L"PROCESS PROGRESS: 67%", -1, &termFont, PointF(360, 470), &neonBrush);
    SolidBrush progBg(COLOR_BORDER);
    SolidBrush progFill(COLOR_NEON);
    g.FillRectangle(&progBg, 360, 495, 340, 15);
    g.FillRectangle(&progFill, 360, 495, 228, 15); // 67%

    // 4. الشاشة اليمين (System Monitor)
    DrawCyberPanel(g, 730, 70, 280, 480, L"SYSTEM MONITOR");
    int my = 100;
    g.DrawString(L"CPU USAGE: 32%", -1, &termFont, PointF(745, (REAL)my), &neonBrush); my += 20;
    g.FillRectangle(&progBg, 745, (REAL)my, 250, 10);
    g.FillRectangle(&progFill, 745, (REAL)my, 80, 10); my += 25;

    g.DrawString(L"MEMORY: 61% (9.7 GB / 15.9 GB)", -1, &termFont, PointF(745, (REAL)my), &neonBrush); my += 20;
    g.FillRectangle(&progBg, 745, (REAL)my, 250, 10);
    g.FillRectangle(&progFill, 745, (REAL)my, 152, 10); my += 25;

    g.DrawString(L"DISK ACTIVITY: 27%", -1, &termFont, PointF(745, (REAL)my), &neonBrush); my += 20;
    g.FillRectangle(&progBg, 745, (REAL)my, 250, 10);
    g.FillRectangle(&progFill, 745, (REAL)my, 67, 10); my += 35;

    g.DrawString(L"NETWORK SPEED", -1, &termFont, PointF(745, (REAL)my), &neonBrush); my += 20;
    g.DrawString(L"↑ 12.4 MB/s   ↓ 3.7 MB/s", -1, &termFont, PointF(745, (REAL)my), &textBrush); my += 35;

    g.DrawString(L"TEMPERATURES", -1, &termFont, PointF(745, (REAL)my), &neonBrush); my += 20;
    g.DrawString(L"CPU: 44°C   GPU: 44°C   MAIN: 37°C", -1, &termFont, PointF(745, (REAL)my), &textBrush);

    // 5. الشريط السفلي (Footer)
    DrawCyberPanel(g, 10, 560, rc.right - 20, 40);
    g.DrawString(L"G-X CORE v3.0.0 | ALL SYSTEMS OPERATIONAL | ENCRYPTION: AES-256 | BUILT BY G-X ENGINE", -1, &termFont, PointF(25, 572), &textDimBrush);

    // نقل الرسم إلى الشاشة مباشرة لتفادي الـ Flicker
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
