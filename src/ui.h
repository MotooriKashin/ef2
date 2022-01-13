#include "register.h"

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
        CreateWindow(TEXT("BUTTON"), NULL, WS_VISIBLE | WS_CHILD | BS_GROUPBOX, 5, 32, 170, 90, hwnd, NULL, (HINSTANCE)GetWindowLongPtr(hwnd, GWLP_HINSTANCE), NULL);
        hButton = CreateWindow(TEXT("button"), TEXT("安装"), WS_CHILD | WS_VISIBLE | BS_FLAT, 25, 47, 130, 30, hwnd, (HMENU)IDC_MAIN_BUTTON, (HINSTANCE)GetWindowLongPtr(hwnd, GWLP_HINSTANCE), NULL);
        hCheckBox1 = CreateWindow(TEXT("static"), TEXT("※ 注册ef2协议关联"), WS_CHILD | WS_VISIBLE | SS_LEFT, 25, 84, 140, 15, hwnd, NULL, (HINSTANCE)GetWindowLongPtr(hwnd, GWLP_HINSTANCE), NULL);
        hCheckBox2 = CreateWindow(TEXT("static"), TEXT("※ 注册ef2文件关联"), WS_CHILD | WS_VISIBLE | SS_LEFT, 25, 99, 140, 15, hwnd, NULL, (HINSTANCE)GetWindowLongPtr(hwnd, GWLP_HINSTANCE), NULL);
        hExit = CreateWindow(TEXT("button"), TEXT("退出"), WS_CHILD | WS_VISIBLE | BS_FLAT, 130, 125, 40, 20, hwnd, (HMENU)IDC_MAIN_EXIT, (HINSTANCE)GetWindowLongPtr(hwnd, GWLP_HINSTANCE), NULL);
        CreateWindow(TEXT("SuperLink"), TEXT("帮助"), WS_CHILD | WS_VISIBLE | DT_LEFT, 5, 125, 50, 20, hwnd, (HMENU)IDC_MAIN_LINK, (HINSTANCE)GetWindowLongPtr(hwnd, GWLP_HINSTANCE), NULL);
        SendMessage(hButton, WM_SETFONT, (WPARAM)GetStockObject(DEFAULT_GUI_FONT), MAKELPARAM(FALSE, 0));
        SendMessage(hCheckBox1, WM_SETFONT, (WPARAM)GetStockObject(DEFAULT_GUI_FONT), MAKELPARAM(FALSE, 0));
        SendMessage(hCheckBox2, WM_SETFONT, (WPARAM)GetStockObject(DEFAULT_GUI_FONT), MAKELPARAM(FALSE, 0));
        SendMessage(hExit, WM_SETFONT, (WPARAM)GetStockObject(DEFAULT_GUI_FONT), MAKELPARAM(FALSE, 0));
        if (Register::RegQuery())
        {
            SendMessage(hButton, WM_SETTEXT, 0, (LPARAM)TEXT("卸载"));
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
        SelectObject(hdc, GetStockObject(DEFAULT_GUI_FONT));                                    // 字体
        SetBkMode(hdc, TRANSPARENT);                                                            // 背景色
        SetTextColor(hdc, RGB(10, 132, 255));                                                   // 字体颜色
        DrawIconEx(hdc, 15, 5, hIcon, 32, 32, 0, NULL, DI_NORMAL);                              // 图标
        DrawText(hdc, TEXT("IDM EF2 Helper"), -1, &rect, DT_LEFT | DT_SINGLELINE | DT_VCENTER); // 标题
        EndPaint(hwnd, &ps);
    }
    break;
    case WM_COMMAND:
        switch (LOWORD(wParam))
        {
        case IDC_MAIN_BUTTON:
            /* 主按钮 */
            Button_Enable((HWND)lParam, false);
            if (Register::RegCreate())
            {
                SetTimer(hwnd, 1, 1000, (TIMERPROC)NULL);
            }
            else
            {
                MessageBox(hwnd, Register::ErrorMessage(), TEXT("注册失败！"), MB_ICONERROR);
            }
            break;
        case IDC_MAIN_EXIT:
            /* 按钮退出 */
            SendMessage(hwnd, WM_CLOSE, wParam, lParam);
            break;
        case IDC_MAIN_LINK:
            /* 自定义控件 */
            ShellExecute(NULL, TEXT("open"), TEXT("https://github.com/MotooriKashin/ef2"), NULL, NULL, SW_SHOWMAXIMIZED);
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
            if (Register::RegQuery())
            {
                Button_SetText(GetDlgItem(hwnd, IDC_MAIN_BUTTON), TEXT("卸载"));
                Button_Enable(GetDlgItem(hwnd, IDC_MAIN_BUTTON), true);
            }
            else
            {
                Button_SetText(GetDlgItem(hwnd, IDC_MAIN_BUTTON), TEXT("安装"));
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
class UI
{
public:
    WNDCLASSEX wc, wb = {0};
    HWND hwnd;
    MSG Msg;
    UI(HINSTANCE hInstance, int nCmdShow)
    {
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
        wc.lpszClassName = TEXT("ef2Class");
        wc.hIconSm = (HICON)LoadImage(GetModuleHandle(NULL), MAKEINTRESOURCE(IDI_MYICON), IMAGE_ICON, 16, 16, 0);

        /* 注册自定义控件类 */
        wb.cbSize = sizeof(wb);
        wb.hInstance = hInstance;
        wb.lpfnWndProc = BtnProc;
        wb.lpszClassName = TEXT("SuperLink");
        wb.style = CS_PARENTDC | CS_GLOBALCLASS;
        wb.hCursor = LoadCursor(NULL, IDC_HAND);

        RegisterClassEx(&wb);
        if (!RegisterClassEx(&wc))
        {
            MessageBox(NULL, TEXT("窗口注册失败！"), NULL,
                       MB_ICONEXCLAMATION | MB_OK);
            exit(-1);
        }

        hwnd = CreateWindowEx(
            WS_EX_CLIENTEDGE,
            TEXT("ef2Class"),
            TEXT("EF2"),
            WS_OVERLAPPED | WS_SYSMENU | WS_MINIMIZEBOX,
            CW_USEDEFAULT, CW_USEDEFAULT, 200, 193,
            NULL, NULL, hInstance, NULL);

        if (hwnd == NULL)
        {
            MessageBox(NULL, TEXT("窗口创建失败！"), NULL,
                       MB_ICONEXCLAMATION | MB_OK);
            exit(-1);
        }

        ShowWindow(hwnd, nCmdShow);
        UpdateWindow(hwnd);

        /* 消息循环 */
        while (GetMessage(&Msg, NULL, 0, 0) > 0)
        {
            TranslateMessage(&Msg);
            DispatchMessage(&Msg);
        }
        exit(Msg.wParam);
    };
};