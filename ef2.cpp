#define _WIN32_WINNT 0x0600
#include <iostream>
#include <fstream>
#include <windows.h>
#include <windowsx.h>
#include <string.h>
#include "base64.h"
#include "resource.h"

using namespace std;

int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int);
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK BtnProc(HWND, UINT, WPARAM, LPARAM);
void pushToIDM(string);
void ef2Protocol(string);
void ef2File(string path);
string Utf8ToGbk(const char *);
string GbkToUtf8(const char *);
BOOL CALLBACK RegQuery();
BOOL CALLBACK RegCreate();
BOOL QueryPrivilege();
LPTSTR CALLBACK ErrorMessage();

/* 入口函数 */
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
    string lpLink;
    /* 命令行调用，无窗口解析 */
    for (int i = 0; i < __argc; i++)
    {
        if (strstr(__argv[i], "ef2://"))
        {
            lpLink = __argv[i];
            ef2Protocol(lpLink);
            return 0;
        }
        else if (strstr(__argv[i], ".ef2"))
        {
            lpLink = __argv[i];
            ef2File(lpLink);
            return 0;
        }
    }

    /* 非命令行调用，启动窗口 */

    WNDCLASSEX wc, wb = {0};
    HWND hwnd;
    MSG Msg;

    /* 管理员提权 */
    if (!QueryPrivilege())
    {
        TCHAR Path[MAX_PATH];
        SHELLEXECUTEINFO ShExecInfo = {0};

        ZeroMemory(Path, MAX_PATH);
        GetModuleFileName(NULL, Path, MAX_PATH);
        ShExecInfo.cbSize = sizeof(SHELLEXECUTEINFO);
        ShExecInfo.lpVerb = "runas";
        ShExecInfo.lpFile = Path;
        ShExecInfo.nShow = SW_SHOWDEFAULT;
        ShellExecuteEx(&ShExecInfo);
        WaitForSingleObject(ShExecInfo.hProcess, INFINITE);
        return 0;
    }

    /* 注册主窗口类 */
    wc.cbSize = sizeof(WNDCLASSEX);
    wc.style = 0;
    wc.lpfnWndProc = WndProc;
    wc.cbClsExtra = 0;
    wc.cbWndExtra = 0;
    wc.hInstance = hInstance;
    wc.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_MYICON));
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground = CreateSolidBrush(RGB(240, 240, 240));
    wc.lpszMenuName = NULL;
    wc.lpszClassName = "ef2Class";
    wc.hIconSm = (HICON)LoadImage(GetModuleHandle(NULL), MAKEINTRESOURCE(IDI_MYICON), IMAGE_ICON, 16, 16, 0);

    /* 注册自定义控件类 */
    wb.cbSize = sizeof(wb);
    wb.hInstance = hInstance;
    wb.lpfnWndProc = BtnProc;
    wb.lpszClassName = "SuperLink";
    wb.style = CS_PARENTDC | CS_GLOBALCLASS;
    wb.hCursor = LoadCursor(NULL, IDC_HAND);

    RegisterClassEx(&wb);
    if (!RegisterClassEx(&wc))
    {
        MessageBox(NULL, "窗口注册失败！", NULL,
                   MB_ICONEXCLAMATION | MB_OK);
        return 0;
    }

    hwnd = CreateWindowEx(
        WS_EX_CLIENTEDGE,
        "ef2Class",
        "EF2",
        WS_OVERLAPPED | WS_SYSMENU | WS_MINIMIZEBOX,
        CW_USEDEFAULT, CW_USEDEFAULT, 200, 183,
        NULL, NULL, hInstance, NULL);

    if (hwnd == NULL)
    {
        MessageBox(NULL, "窗口创建失败！", NULL,
                   MB_ICONEXCLAMATION | MB_OK);
        return 0;
    }

    ShowWindow(hwnd, nCmdShow);
    UpdateWindow(hwnd);

    /* 消息循环 */
    while (GetMessage(&Msg, NULL, 0, 0) > 0)
    {
        TranslateMessage(&Msg);
        DispatchMessage(&Msg);
    }
    return Msg.wParam;
}
/* 拉起IDM */
void pushToIDM(string lpParameters)
{
    /* 获取二进制文件目录 */
    char szCurPath[MAX_PATH];
    memset(szCurPath, 0, MAX_PATH);
    GetModuleFileName(NULL, szCurPath, MAX_PATH);
    (strrchr(szCurPath, '\\'))[0] = 0;
    strcat(szCurPath, "\\IDMHelper.exe");
    SHELLEXECUTEINFO SendUrlToIDM = {0};
    SendUrlToIDM.cbSize = sizeof(SHELLEXECUTEINFO);
    SendUrlToIDM.fMask = SEE_MASK_NOCLOSEPROCESS;
    SendUrlToIDM.hwnd = NULL;
    SendUrlToIDM.lpVerb = TEXT("open");
    SendUrlToIDM.lpFile = szCurPath;
    SendUrlToIDM.lpParameters = lpParameters.c_str();
    SendUrlToIDM.lpDirectory = NULL;
    SendUrlToIDM.nShow = SW_HIDE;
    SendUrlToIDM.hInstApp = NULL;
    ShellExecuteEx(&SendUrlToIDM);
    WaitForSingleObject(SendUrlToIDM.hProcess, INFINITE);
}
/* ef2协议解析 */
void ef2Protocol(string lpLink)
{
    string temp, result;
    lpLink.erase(0, 6);
    Base64::Decode(lpLink, &temp);
    result = Utf8ToGbk(temp.c_str());
    if (result.find("-u ") != string::npos)
    {
        pushToIDM(result);
    }
    else
    {
        MessageBox(NULL, TEXT("非法ef2链接！请查阅相关文档：https://github.com/MotooriKashin/ef2"), NULL, MB_ICONERROR);
    }
}
/* ef2文件解析 */
void ef2File(string path)
{
    ifstream inFile;
    inFile.open(path.c_str());
    string temp;
    string result = "";
    int one = 0;
    if (inFile)
    {
        while (!inFile.eof())
        {
            getline(inFile, temp);
            if (temp == "<")
            {
                one = one + 1; // 记录ef2文件中下载链接数目
                if (result != "")
                {
                    /* ef2文件中不止一条链接，以添加队列方式发送前一条链接给IDM（需要手动开始下载队列） */
                    result.append("-q");
                    pushToIDM(result);
                }
                result = "";
                getline(inFile, temp);
                while (temp != ">" && !inFile.eof())
                {
                    if (temp.find("referer") != string::npos)
                    {
                        result.append("-r ");
                        result.append(temp.substr(9));
                        result.append(" ");
                    }
                    else if (temp.find("User-Agent") != string::npos)
                    {
                        result.append("-a \"");
                        result.append(temp.substr(12));
                        result.append("\" ");
                    }
                    else if (temp.find("cookies") != string::npos)
                    {
                        result.append("-c \"");
                        result.append(temp.substr(9));
                        result.append("\" ");
                    }
                    else if (temp.find("postdata") != string::npos)
                    {
                        result.append("-d \"");
                        result.append(temp.substr(10));
                        result.append("\" ");
                    }
                    else if (temp.find("username") != string::npos)
                    {
                        result.append("-U \"");
                        result.append(temp.substr(10));
                        result.append("\" ");
                    }
                    else if (temp.find("password") != string::npos)
                    {
                        result.append("-P \"");
                        result.append(temp.substr(10));
                        result.append("\" ");
                    }
                    else if (temp.find("filepath") != string::npos)
                    {
                        result.append("-o \"");
                        result.append(temp.substr(10));
                        result.append("\" ");
                    }
                    else if (temp.find("filename") != string::npos)
                    {
                        result.append("-s \"");
                        result.append(temp.substr(10));
                        result.append("\" ");
                    }
                    else if (temp.find("://") != string::npos)
                    {
                        result.append("-u ");
                        result.append(temp);
                        result.append(" ");
                    }
                    getline(inFile, temp);
                }
            }
        }
        if (one == 1)
        {
            /* ef2文件中只有一条链接直接发送给IDM */
            pushToIDM(result);
        }
        else if (one == 0)
        {
            MessageBox(NULL, TEXT("该ef2文件似乎并不包含有效下载链接！？\r\n请检查文件内容或者反馈到：https://github.com/MotooriKashin/ef2/issues"), TEXT("ef2解析"), MB_ICONERROR);
        }
        else
        {
            /* ef2文件中不止一条链接，以添加队列方式发送最后一条链接给IDM（需要手动开始下载队列） */
            result.append("-q");
            pushToIDM(result);
            MessageBox(NULL, TEXT("该ef2文件包含复数个下载链接，为避免弹出过多对话框，已全部添加IDM到下载队列，请自行到IDM里开始队列！"), TEXT("ef2解析"), MB_ICONWARNING);
        }
        inFile.close();
    }
}
/* 编码转换：utf-8 -> gbk */
string Utf8ToGbk(const char *src_str)
{
    int len = MultiByteToWideChar(CP_UTF8, 0, src_str, -1, NULL, 0);
    wchar_t *wszGBK = new wchar_t[len + 1];
    memset(wszGBK, 0, len * 2 + 2);
    MultiByteToWideChar(CP_UTF8, 0, src_str, -1, wszGBK, len);
    len = WideCharToMultiByte(CP_ACP, 0, wszGBK, -1, NULL, 0, NULL, NULL);
    char *szGBK = new char[len + 1];
    memset(szGBK, 0, len + 1);
    WideCharToMultiByte(CP_ACP, 0, wszGBK, -1, szGBK, len, NULL, NULL);
    string strTemp(szGBK);
    if (wszGBK)
        delete[] wszGBK;
    if (szGBK)
        delete[] szGBK;
    return strTemp;
}
/* 编码转换：gbk -> utf-8 */
string GbkToUtf8(const char *src_str)
{
    int len = MultiByteToWideChar(CP_ACP, 0, src_str, -1, NULL, 0);
    wchar_t *wstr = new wchar_t[len + 1];
    memset(wstr, 0, len + 1);
    MultiByteToWideChar(CP_ACP, 0, src_str, -1, wstr, len);
    len = WideCharToMultiByte(CP_UTF8, 0, wstr, -1, NULL, 0, NULL, NULL);
    char *str = new char[len + 1];
    memset(str, 0, len + 1);
    WideCharToMultiByte(CP_UTF8, 0, wstr, -1, str, len, NULL, NULL);
    string strTemp = str;
    if (wstr)
        delete[] wstr;
    if (str)
        delete[] str;
    return strTemp;
}
/* 主窗口回调 */
LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    static HICON hIcon;
    int scrWidth, scrHeight;
    HWND hButton, hCheckBox1, hCheckBox2, hExit;
    RECT rect;
    BITMAP bm;
    PAINTSTRUCT ps;
    HDC hdc;

    switch (msg)
    {
    case WM_CREATE:
    {
        hIcon = LoadIcon((HINSTANCE)GetWindowLongPtr(hwnd, GWLP_HINSTANCE), MAKEINTRESOURCE(IDI_MYICON));
        /* 窗口居中 */
        scrWidth = GetSystemMetrics(SM_CXSCREEN);
        scrHeight = GetSystemMetrics(SM_CYSCREEN);
        GetWindowRect(hwnd, &rect);
        SetWindowPos(hwnd, HWND_TOP, (scrWidth - rect.right + rect.left) / 2, (scrHeight - rect.bottom + rect.top) / 2, rect.right - rect.left, rect.bottom - rect.top, SWP_SHOWWINDOW);

        /* 界面绘制 */
        CreateWindow("BUTTON", NULL,
                     WS_VISIBLE | WS_CHILD | BS_GROUPBOX,
                     5, 32, 180, 90,
                     hwnd, NULL, (HINSTANCE)GetWindowLongPtr(hwnd, GWLP_HINSTANCE), NULL);
        hButton = CreateWindow("button", "安装",
                               WS_CHILD | WS_VISIBLE | BS_FLAT,
                               25, 47, 140, 30,
                               hwnd, (HMENU)IDC_MAIN_BUTTON, (HINSTANCE)GetWindowLongPtr(hwnd, GWLP_HINSTANCE), NULL);
        hCheckBox1 = CreateWindow("static", "※ 注册ef2协议关联",
                                  WS_CHILD | WS_VISIBLE | SS_LEFT,
                                  25, 84, 140, 15,
                                  hwnd, NULL, (HINSTANCE)GetWindowLongPtr(hwnd, GWLP_HINSTANCE), NULL);
        hCheckBox2 = CreateWindow("static", "※ 注册ef2文件关联",
                                  WS_CHILD | WS_VISIBLE | SS_LEFT,
                                  25, 99, 140, 15,
                                  hwnd, NULL, (HINSTANCE)GetWindowLongPtr(hwnd, GWLP_HINSTANCE), NULL);
        hExit = CreateWindow("button", "退出",
                             WS_CHILD | WS_VISIBLE | BS_FLAT,
                             130, 125, 40, 20,
                             hwnd, (HMENU)IDC_MAIN_EXIT, (HINSTANCE)GetWindowLongPtr(hwnd, GWLP_HINSTANCE), NULL);
        CreateWindow("SuperLink", "帮助",
                     WS_CHILD | WS_VISIBLE | DT_LEFT,
                     15, 125, 50, 20,
                     hwnd, (HMENU)IDC_MAIN_LINK, (HINSTANCE)GetWindowLongPtr(hwnd, GWLP_HINSTANCE), NULL);
        SendMessage(hButton, WM_SETFONT, (WPARAM)GetStockObject(DEFAULT_GUI_FONT), MAKELPARAM(FALSE, 0));
        SendMessage(hCheckBox1, WM_SETFONT, (WPARAM)GetStockObject(DEFAULT_GUI_FONT), MAKELPARAM(FALSE, 0));
        SendMessage(hCheckBox2, WM_SETFONT, (WPARAM)GetStockObject(DEFAULT_GUI_FONT), MAKELPARAM(FALSE, 0));
        SendMessage(hExit, WM_SETFONT, (WPARAM)GetStockObject(DEFAULT_GUI_FONT), MAKELPARAM(FALSE, 0));
        if (RegQuery())
        {
            SendMessage(hButton, WM_SETTEXT, 0, (LPARAM) "卸载");
        }
    }
    break;
    case WM_PAINT:
    {
        /* 绘制图表及标题 */
        rect.left = 60;
        rect.top = 5;
        rect.right = 180;
        rect.bottom = 40;

        hdc = BeginPaint(hwnd, &ps);
        SelectObject(hdc, GetStockObject(DEFAULT_GUI_FONT));                              // 字体
        SetBkMode(hdc, TRANSPARENT);                                                      // 背景色
        SetTextColor(hdc, RGB(10, 132, 255));                                             // 字体颜色
        DrawIconEx(hdc, 15, 5, hIcon, 32, 32, 0, NULL, DI_NORMAL);                        // 图标
        DrawText(hdc, "IDM EF2 Helper", -1, &rect, DT_LEFT | DT_SINGLELINE | DT_VCENTER); // 标题
        EndPaint(hwnd, &ps);
    }
    break;
    case WM_COMMAND:
        switch (LOWORD(wParam))
        {
        case IDC_MAIN_BUTTON:
            /* 主按钮 */
            Button_Enable((HWND)lParam, false);
            if (RegCreate())
            {
                SetTimer(hwnd, 1, 1000, (TIMERPROC)NULL);
            }
            else
            {
                MessageBox(hwnd, ErrorMessage(), "注册失败！", MB_ICONERROR);
            }
            break;
        case IDC_MAIN_EXIT:
            /* 按钮退出 */
            SendMessage(hwnd, WM_CLOSE, wParam, lParam);
            break;
        case IDC_MAIN_LINK:
            /* 自定义控件 */
            ShellExecute(NULL, "open", "https://github.com/MotooriKashin/ef2", NULL, NULL, SW_SHOWMAXIMIZED);
            break;
        default:
            return DefWindowProc(hwnd, msg, wParam, lParam);
        }
        break;
    case WM_TIMER:
        switch (wParam)
        {
        case 1:
            KillTimer(hwnd, 1);
            if (RegQuery())
            {
                Button_SetText(GetDlgItem(hwnd, IDC_MAIN_BUTTON), "卸载");
                Button_Enable(GetDlgItem(hwnd, IDC_MAIN_BUTTON), true);
            }
            else
            {
                Button_SetText(GetDlgItem(hwnd, IDC_MAIN_BUTTON), "安装");
                Button_Enable(GetDlgItem(hwnd, IDC_MAIN_BUTTON), true);
            }
            return 0;
            break;
        }
    case WM_CLOSE:
        DestroyWindow(hwnd);
        break;
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hwnd, msg, wParam, lParam);
    }
    return 0;
}
// 自定义控件回调
LRESULT CALLBACK BtnProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    switch (msg)
    {
    case WM_PAINT:
    {
        /* 自定义控件内容需要自绘 */
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hwnd, &ps);
        SelectObject(hdc, GetStockObject(DEFAULT_GUI_FONT));

        int length = GetWindowTextLength(hwnd);
        TCHAR *title = new TCHAR[length + 1];
        ZeroMemory(title, (length + 1) * sizeof(TCHAR));
        GetWindowText(hwnd, title, length + 1);
        SetTextColor(hdc, RGB(0, 0, 255));
        SetBkMode(hdc, TRANSPARENT);
        RECT rc;
        GetClientRect(hwnd, &rc);
        DrawText(hdc, title, length, &rc, DT_CENTER | DT_SINGLELINE | DT_VCENTER);
        delete[] title;
        EndPaint(hwnd, &ps);
    }
    break;
    case WM_LBUTTONDOWN:
    {
        /* 转发鼠标单击给父窗口 */
        SendMessage(GetParent(hwnd), WM_COMMAND, GetDlgCtrlID(hwnd), (LPARAM)hwnd);
    }
    break;

    default:
        return DefWindowProc(hwnd, msg, wParam, lParam);
    }
    return 0;
}
// 注册检查
BOOL CALLBACK RegQuery()
{
    HKEY hKey;
    LSTATUS nRes;

    nRes = RegOpenKeyEx(HKEY_CURRENT_USER, "Software\\DownloadManager", 0, KEY_READ, &hKey);
    if (nRes != ERROR_SUCCESS)
    {
        MessageBox(NULL, "您似乎未安装IDM！\r\n您必须先安装Internet Download Manager (IDM)才能使用此工具！", NULL, MB_ICONWARNING);
    }
    nRes = RegOpenKeyEx(HKEY_CLASSES_ROOT, "ef2", 0, KEY_READ, &hKey);
    if (nRes != ERROR_SUCCESS)
    {
        return false;
    }

    return true;
}
// 注册关联
BOOL CALLBACK RegCreate()
{
    BOOL state = RegQuery();
    HKEY hKey, hKey1, hKey2, hKey3;
    LPTSTR Error = 0;
    char szCurPath[MAX_PATH];
    memset(szCurPath, 0, MAX_PATH);
    GetModuleFileName(NULL, szCurPath, MAX_PATH);
    strcat(szCurPath, " \"%1\"");

    if (state)
    {
        /* 注销关联 */
        if (RegDeleteTree(HKEY_CLASSES_ROOT, ".ef2") != ERROR_SUCCESS)
        {
            return false;
        }
        if (RegDeleteTree(HKEY_CLASSES_ROOT, "IDM_ef2") != ERROR_SUCCESS)
        {
            return false;
        }
        if (RegDeleteTree(HKEY_CLASSES_ROOT, "ef2") != ERROR_SUCCESS)
        {
            return false;
        }
        return true;
    }
    else
    {
        /* 关联.ef2 */
        if (RegCreateKey(HKEY_CLASSES_ROOT, ".ef2", &hKey) == ERROR_SUCCESS)
        {
            RegSetValue(hKey, NULL, REG_SZ, "IDM_ef2", 0);
            RegCloseKey(hKey);
        }
        else
        {
            return false;
        }
        /* 关联IDM_ef2 */
        if (RegCreateKey(HKEY_CLASSES_ROOT, "IDM_ef2", &hKey) == ERROR_SUCCESS)
        {
            if (RegCreateKey(hKey, "shell", &hKey1) == ERROR_SUCCESS)
            {
                if (RegCreateKey(hKey1, "open", &hKey2) == ERROR_SUCCESS)
                {
                    if (RegCreateKey(hKey2, "command", &hKey3) == ERROR_SUCCESS)
                    {
                        RegSetValue(hKey3, NULL, REG_SZ, szCurPath, 0);
                        RegCloseKey(hKey3);
                    }
                    else
                    {
                        RegCloseKey(hKey2);
                        RegCloseKey(hKey1);
                        RegCloseKey(hKey);
                        return false;
                    }
                    RegCloseKey(hKey2);
                }
                else
                {
                    RegCloseKey(hKey1);
                    RegCloseKey(hKey);
                    return false;
                }
                RegCloseKey(hKey1);
            }
            else
            {
                RegCloseKey(hKey);
                return false;
            }
            RegCloseKey(hKey);
        }
        else
        {
            return false;
        }
        /* 关联ef2 */
        if (RegCreateKey(HKEY_CLASSES_ROOT, "ef2", &hKey) == ERROR_SUCCESS)
        {
            RegSetValue(hKey, NULL, REG_SZ, "URL:IDM", 0);
            RegSetValueEx(hKey, "URL Protocol", 0, REG_SZ, NULL, 0);
            if (RegCreateKey(hKey, "shell", &hKey1) == ERROR_SUCCESS)
            {
                if (RegCreateKey(hKey1, "open", &hKey2) == ERROR_SUCCESS)
                {
                    if (RegCreateKey(hKey2, "command", &hKey3) == ERROR_SUCCESS)
                    {
                        RegSetValue(hKey3, NULL, REG_SZ, szCurPath, 0);
                        RegCloseKey(hKey3);
                    }
                    else
                    {
                        RegCloseKey(hKey2);
                        RegCloseKey(hKey1);
                        RegCloseKey(hKey);
                        return false;
                    }
                    RegCloseKey(hKey2);
                }
                else
                {
                    RegCloseKey(hKey1);
                    RegCloseKey(hKey);
                    return false;
                }
                RegCloseKey(hKey1);
            }
            else
            {
                RegCloseKey(hKey);
                return false;
            }
            RegCloseKey(hKey);
        }
        else
        {
            return false;
        }
        return true;
    }
}
// 查询管理员权限
BOOL QueryPrivilege()
{
    BOOL b;
    SID_IDENTIFIER_AUTHORITY NtAuthority = SECURITY_NT_AUTHORITY;
    PSID AdministratorsGroup;
    b = AllocateAndInitializeSid(
        &NtAuthority,
        2,
        SECURITY_BUILTIN_DOMAIN_RID,
        DOMAIN_ALIAS_RID_ADMINS,
        0, 0, 0, 0, 0, 0,
        &AdministratorsGroup);
    if (b)
    {
        if (!CheckTokenMembership(NULL, AdministratorsGroup, &b))
        {
            b = FALSE;
        }
        FreeSid(AdministratorsGroup);
    }
    return b;
}
// 错误收集
LPTSTR CALLBACK ErrorMessage()
{
    LPTSTR Error = 0;

    DWORD err = GetLastError();
    FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
                  NULL,
                  err,
                  0,
                  (LPTSTR)&Error,
                  0,
                  NULL);
    return Error;
}