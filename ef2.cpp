#include <shlobj.h>
#include <shlwapi.h>
#include <objbase.h>
#include <tchar.h>
#include <iostream>
#include "base64.h"
#include <string>
#include <string.h>
#include <stdlib.h>
#include <windows.h>
using namespace std;

/**
 * @link 踏莎行hyx {@see https://blog.csdn.net/u012234115/article/details/83186386}
 */
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

int main(int argc, char *argv[])
{
    string str = "";
    string para = "";
    string fin = "";
    for (int i = 0; i < argc; i++)
    {
        if (strstr(argv[i], "ef2://"))
        {
            str.append(argv[i]);
        }
    }

    // 去除 ef2:// 头
    str.erase(0, 6);
    // 从浏览器打开不知道为什么末尾会多斜杠
    if (str[str.length() - 1] == '/')
        str.erase(str.end() - 1);
    // base64 解码
    Base64::Decode(str, &para);
    // 处理编码问题（utf-8 -> gbk）
    fin = Utf8ToGbk(para.c_str());

    // 打印调试信息：str为原本的base64编码数据；fin为最终转换发送给 IDMHelper 的参数
    // printf("[+] links: %s\n", str.c_str());
    // MessageBox(NULL, fin.c_str(), NULL, MB_OK);
    // 调用 IDMHelper
    char szCurPath[MAX_PATH];
    memset(szCurPath, 0, MAX_PATH);
    GetModuleFileName(NULL, szCurPath, MAX_PATH);
    (strrchr(szCurPath, '\\'))[0] = 0;
    strcat(szCurPath, "\\IDMHelper.exe");
    SHELLEXECUTEINFO SendUrlToIDM = {0};
    SendUrlToIDM.cbSize = sizeof(SHELLEXECUTEINFO);
    SendUrlToIDM.fMask = SEE_MASK_NOCLOSEPROCESS;
    SendUrlToIDM.hwnd = NULL;
    SendUrlToIDM.lpVerb = _T("open");
    SendUrlToIDM.lpFile = szCurPath;
    SendUrlToIDM.lpParameters = fin.c_str();
    SendUrlToIDM.lpDirectory = NULL;
    SendUrlToIDM.nShow = SW_HIDE;
    SendUrlToIDM.hInstApp = NULL;

    ShellExecuteEx(&SendUrlToIDM);
    WaitForSingleObject(SendUrlToIDM.hProcess, INFINITE);
    return 0;
}