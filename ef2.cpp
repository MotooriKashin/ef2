#include <iostream>
#include <fstream>
#include <windows.h>
#include <string.h>
#include "base64.h"

using namespace std;

int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int);
void pushToIDM(string);
void ef2Protocol(string);
void ef2File(string path);
string Utf8ToGbk(const char *);
string GbkToUtf8(const char *);

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
    string lpLink;
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
    MessageBox(NULL, "UI构建中...", "抱歉", MB_ICONEXCLAMATION);
    return 0;
}
// 拉起IDM
void pushToIDM(string lpParameters)
{
    // 获取二进制文件目录
    char szCurPath[MAX_PATH];
    memset(szCurPath, 0, MAX_PATH);
    GetModuleFileName(NULL, szCurPath, MAX_PATH);
    (strrchr(szCurPath, '\\'))[0] = 0;
    strcat(szCurPath, "\\IDMHelper.exe");
    // 拉起 IDM
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
// ef2协议解析
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
// ef2文件解析
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
                    // ef2文件中不止一条链接，以添加队列方式发送前一条链接给IDM（需要手动开始下载队列）
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
            // ef2文件中只有一条链接直接发送给IDM
            pushToIDM(result);
        }
        else if (one == 0)
        {
            MessageBox(NULL, TEXT("该ef2文件似乎并不包含有效下载链接！？\r\n请检查文件内容或者反馈到：https://github.com/MotooriKashin/ef2/issues"), TEXT("ef2解析"), MB_ICONERROR);
        }
        else
        {
            // ef2文件中不止一条链接，以添加队列方式发送最后一条链接给IDM（需要手动开始下载队列）
            result.append("-q");
            pushToIDM(result);
            MessageBox(NULL, TEXT("该ef2文件包含复数个下载链接，为避免弹出过多对话框，已全部添加IDM到下载队列，请自行到IDM里开始队列！"), TEXT("ef2解析"), MB_ICONWARNING);
        }
        inFile.close();
    }
}
// https://blog.csdn.net/u012234115/article/details/83186386
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