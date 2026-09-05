#include <windows.h>
#include <commctrl.h>
#include <string>
#include <vector>
#include <fstream>
#include <sstream>
#include <ctime>
#include <algorithm>

#pragma comment(lib, "Comctl32.lib")
#pragma comment(lib, "Advapi32.lib")

static HWND gMain=nullptr, gList=nullptr, gStatus=nullptr;
static HBRUSH gBg=nullptr, gPanel=nullptr, gEdit=nullptr;
static HFONT gTitle=nullptr, gFont=nullptr, gSmall=nullptr;
static COLORREF GREEN=RGB(70,255,130), BG=RGB(12,18,16), PANEL=RGB(20,29,24), TEXT=RGB(232,245,235);
static std::wstring baseDir;

std::wstring Now() {
    SYSTEMTIME t; GetLocalTime(&t);
    wchar_t b[64]; swprintf(b,64,L"%04d-%02d-%02d %02d:%02d:%02d",t.wYear,t.wMonth,t.wDay,t.wHour,t.wMinute,t.wSecond);
    return b;
}
void Log(const std::wstring& s){
    std::wofstream f(baseDir+L"\\logs\\gx.log", std::ios::app);
    if(f) f<<L"["<<Now()<<L"] "<<s<<L"\n";
}
void SetStatus(const std::wstring& s){ if(gStatus) SetWindowTextW(gStatus,s.c_str()); Log(s); }

void AddRow(const std::wstring& s){
    int n=(int)SendMessageW(gList,LB_GETCOUNT,0,0);
    SendMessageW(gList,LB_ADDSTRING,0,(LPARAM)s.c_str());
    SendMessageW(gList,LB_SETCURSEL,n,0);
}

std::wstring RunCommand(const std::wstring& cmd){
    std::wstring out;
    SECURITY_ATTRIBUTES sa{sizeof(sa),nullptr,TRUE};
    HANDLE r,w;
    if(!CreatePipe(&r,&w,&sa,0)) return L"";
    SetHandleInformation(r,HANDLE_FLAG_INHERIT,0);
    STARTUPINFOW si{}; si.cb=sizeof(si); si.dwFlags=STARTF_USESTDHANDLES;
    si.hStdOutput=w; si.hStdError=w;
    PROCESS_INFORMATION pi{};
    std::wstring full=L"cmd.exe /c "+cmd;
    std::vector<wchar_t> buf(full.begin(),full.end()); buf.push_back(0);
    if(CreateProcessW(nullptr,buf.data(),nullptr,nullptr,TRUE,CREATE_NO_WINDOW,nullptr,nullptr,&si,&pi)){
        CloseHandle(w);
        char tmp[4096]; DWORD got;
        while(ReadFile(r,tmp,sizeof(tmp)-1,&got,nullptr)&&got){
            tmp[got]=0;
            int n=MultiByteToWideChar(CP_OEMCP,0,tmp,got,nullptr,0);
            if(n > 0){
                std::vector<wchar_t> x(n);
                MultiByteToWideChar(CP_OEMCP,0,tmp,got,x.data(),n);
                out.append(x.data(),n);
            }
        }
        WaitForSingleObject(pi.hProcess,10000);
        CloseHandle(pi.hProcess); CloseHandle(pi.hThread);
    } else CloseHandle(w);
    CloseHandle(r);
    return out;
}

void SecurityAudit(){
    SetStatus(L"G-X Security: running audit...");
    AddRow(L"=== SECURITY AUDIT ===");
    auto a=RunCommand(L"sc query WinDefend & sc query BFE & netsh advfirewall show allprofiles & netstat -ano");
    std::wofstream f(baseDir+L"\\logs\\security-report.txt");
    if(f) f<<a;
    AddRow(L"Security audit completed. Report saved in logs\\security-report.txt");
    SetStatus(L"Security audit complete");
}
void HealthAudit(){
    SetStatus(L"G-X Health: checking system...");
    AddRow(L"=== SYSTEM HEALTH ===");
    auto a=RunCommand(L"systeminfo | findstr /B /C:\"OS Name\" /C:\"OS Version\" /C:\"System Boot Time\" & wmic cpu get loadpercentage & wmic OS get FreePhysicalMemory,TotalVisibleMemorySize /value");
    std::wofstream f(baseDir+L"\\logs\\health-report.txt");
    if(f) f<<a;
    AddRow(L"System health report saved in logs\\health-report.txt");
    SetStatus(L"System health check complete");
}
void NetworkAudit(){
    SetStatus(L"G-X Network: checking...");
    AddRow(L"=== NETWORK AUDIT ===");
    auto a=RunCommand(L"ipconfig /all & netstat -ano");
    std::wofstream f(baseDir+L"\\logs\\network-report.txt");
    if(f) f<<a;
    AddRow(L"Network report saved in logs\\network-report.txt");
    SetStatus(L"Network audit complete");
}
void Processes(){
    SetStatus(L"G-X Processes: collecting...");
    AddRow(L"=== PROCESSES ===");
    auto a=RunCommand(L"tasklist /fo table");
    std::wofstream f(baseDir+L"\\logs\\process-report.txt");
    if(f) f<<a;
    AddRow(L"Process report saved in logs\\process-report.txt");
    SetStatus(L"Process inventory complete");
}
void SystemStatus(){
    AddRow(L"=== G-X STATUS ===");
    auto a=RunCommand(L"ver & whoami & hostname");
    std::wistringstream ss(a); std::wstring line;
    while(std::getline(ss,line)) if(!line.empty()) AddRow(line);
    SetStatus(L"G-X ready");
}

LRESULT CALLBACK Proc(HWND h, UINT m, WPARAM wp, LPARAM lp){
    switch(m){
    case WM_CTLCOLORSTATIC:
    case WM_CTLCOLOREDIT:
    case WM_CTLCOLORLISTBOX:{
        HDC dc=(HDC)wp; SetTextColor(dc,TEXT); SetBkColor(dc,(m==WM_CTLCOLORLISTBOX)?RGB(10,15,12):PANEL);
        return (LRESULT)((m==WM_CTLCOLORLISTBOX)?gEdit:gPanel);
    }
    case WM_COMMAND:
        if(HIWORD(wp)==BN_CLICKED){
            switch(LOWORD(wp)){
            case 101: SystemStatus(); break;
            case 102: SecurityAudit(); break;
            case 103: HealthAudit(); break;
            case 104: NetworkAudit(); break;
            case 105: Processes(); break;
            case 106: MessageBoxW(h,L"G-X Core\n\nVoice-First / Text-Optional\nSecurity + Health + Network + Process monitoring\n\nThe UI is intentionally windowed and does not occupy the full screen.",L"G-X",MB_OK); break;
            case 107: DestroyWindow(h); break;
            }
        }
        break;
    case WM_DESTROY: PostQuitMessage(0); break;
    default: return DefWindowProcW(h,m,wp,lp);
    }
    return 0;
}
HWND Btn(HWND p,int id,const wchar_t* t,int x,int y,int w,int h){
    HWND b=CreateWindowW(L"BUTTON",t,WS_CHILD|WS_VISIBLE|BS_OWNERDRAW,x,y,w,h,p,(HMENU)(INT_PTR)id,GetModuleHandleW(nullptr),nullptr);
    SendMessageW(b,WM_SETFONT,(WPARAM)gFont,TRUE); return b;
}
int WINAPI wWinMain(HINSTANCE hi,HINSTANCE, PWSTR, int){
    wchar_t path[MAX_PATH]; GetModuleFileNameW(nullptr,path,MAX_PATH);
    baseDir=path; size_t p=baseDir.find_last_of(L"\\/");
    if(p!=std::wstring::npos) baseDir.resize(p);
    CreateDirectoryW((baseDir+L"\\logs").c_str(),nullptr);

    INITCOMMONCONTROLSEX ic{sizeof(ic),ICC_STANDARD_CLASSES}; InitCommonControlsEx(&ic);
    gFont=CreateFontW(18,0,0,0,FW_SEMIBOLD,0,0,0,DEFAULT_CHARSET,0,0,0,0,L"Segoe UI");
    gTitle=CreateFontW(30,0,0,0,FW_BOLD,0,0,0,DEFAULT_CHARSET,0,0,0,0,L"Segoe UI");
    gSmall=CreateFontW(14,0,0,0,FW_NORMAL,0,0,0,DEFAULT_CHARSET,0,0,0,0,L"Segoe UI");
    gBg=CreateSolidBrush(BG); gPanel=CreateSolidBrush(PANEL); gEdit=CreateSolidBrush(RGB(10,15,12));

    WNDCLASSW wc{}; wc.hInstance=hi; wc.lpfnWndProc=Proc; wc.lpszClassName=L"GXWindow";
    wc.hbrBackground=gBg; wc.hCursor=LoadCursor(nullptr,IDC_ARROW); RegisterClassW(&wc);

    gMain=CreateWindowW(L"GXWindow",L"G-X",WS_OVERLAPPED|WS_CAPTION|WS_SYSMENU|WS_MINIMIZEBOX,
                        CW_USEDEFAULT,CW_USEDEFAULT,1040,700,nullptr,nullptr,hi,nullptr);
    ShowWindow(gMain,SW_SHOW); UpdateWindow(gMain);

    CreateWindowW(L"STATIC",L"G-X",WS_CHILD|WS_VISIBLE,34,25,250,45,gMain,nullptr,hi,nullptr);
    HWND sub=CreateWindowW(L"STATIC",L"G-X Core • Windows Guardian",WS_CHILD|WS_VISIBLE,37,70,500,28,gMain,nullptr,hi,nullptr);
    SendMessageW(sub,WM_SETFONT,(WPARAM)gSmall,TRUE);

    Btn(gMain,101,L"System Status",35,120,190,48);
    Btn(gMain,102,L"Security Audit",235,120,190,48);
    Btn(gMain,103,L"System Health",435,120,190,48);
    Btn(gMain,104,L"Network Audit",35,180,190,48);
    Btn(gMain,105,L"Processes",235,180,190,48);
    Btn(gMain,106,L"About G-X",435,180,190,48);
    Btn(gMain,107,L"Exit",835,590,140,42);

    gList=CreateWindowW(L"LISTBOX",L"",WS_CHILD|WS_VISIBLE|WS_VSCROLL|LBS_NOINTEGRALHEIGHT|WS_BORDER,
                        35,255,940,315,gMain,nullptr,hi,nullptr);
    SendMessageW(gList,WM_SETFONT,(WPARAM)gSmall,TRUE);
    gStatus=CreateWindowW(L"STATIC",L"Ready • G-X is running",WS_CHILD|WS_VISIBLE,
                          35,585,700,42,gMain,nullptr,hi,nullptr);
    SendMessageW(gStatus,WM_SETFONT,(WPARAM)gSmall,TRUE);
    Log(L"G-X started");
    AddRow(L"G-X Core initialized.");
    AddRow(L"Windowed UI: 1040 x 700 (not fullscreen).");
    AddRow(L"Voice-First / Text-Optional architecture ready for voice layer.");
    AddRow(L"Select a module to begin.");
    SetStatus(L"Ready");

    MSG msg; while(GetMessageW(&msg,nullptr,0,0)){ TranslateMessage(&msg); DispatchMessageW(&msg); }
    return 0;
}
  
