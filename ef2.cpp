#include <shlobj.h>
#include <shlwapi.h>
#include <objbase.h>
#include <tchar.h>
#include <iostream>
#include <fstream>
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
// 拉起 IDM
// 感谢 IDMHelper.exe https://github.com/unamer/IDMHelper
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
    SendUrlToIDM.lpVerb = _T("open");
    SendUrlToIDM.lpFile = szCurPath;
    SendUrlToIDM.lpParameters = lpParameters.c_str();
    SendUrlToIDM.lpDirectory = NULL;
    SendUrlToIDM.nShow = SW_HIDE;
    SendUrlToIDM.hInstApp = NULL;
    ShellExecuteEx(&SendUrlToIDM);
    WaitForSingleObject(SendUrlToIDM.hProcess, INFINITE);
}
// 处理ef2协议
void ef2Protocol(string str)
{
    string para = "";
    string fin = "";
    // 去除 ef2:// 头
    str.erase(0, 6);
    // 从浏览器打开不知道为什么末尾会多斜杠
    if (str[str.length() - 1] == '/')
    {
        str.erase(str.end() - 1);
    }
    if (str.find("://") != string::npos)
    {
        // 带协议头可能直接传入的下载链接
        para = "-u " + str;
    }
    else
    {
        Base64::Decode(str, &para); // base64 解码
    }
    // 处理编码问题（utf-8 -> gbk）
    fin = Utf8ToGbk(para.c_str());
    if (fin.find("-u ") != string::npos)
    {
        pushToIDM(fin);
    }
    else
    {
        MessageBox(NULL, _T("非法ef2链接！请查阅相关文档：https://github.com/MotooriKashin/ef2"), NULL, MB_OK);
    }
}
// 处理.ef2文件
void ef2File(string path)
{
    ifstream inFile;
    inFile.open(path.c_str());
    string str;
    string para = "";
    int one = 0;
    if (inFile)
    {
        while (!inFile.eof())
        {
            getline(inFile, str);
            if (str == "<")
            {
                one = one + 1; // 记录ef2文件中下载链接数目
                if (para != "")
                {
                    // ef2文件中不止一条链接，以添加队列方式发送前一条链接给IDM（需要手动开始下载队列）
                    para.append("-q");
                    pushToIDM(para);
                }
                para = "";
                getline(inFile, str);
                while (str != ">" && !inFile.eof())
                {
                    if (str.find("referer") != string::npos)
                    {
                        para.append("-r ");
                        para.append(str.substr(9));
                        para.append(" ");
                    }
                    else if (str.find("User-Agent") != string::npos)
                    {
                        para.append("-a \"");
                        para.append(str.substr(12));
                        para.append("\" ");
                    }
                    else if (str.find("://") != string::npos)
                    {
                        para.append("-u ");
                        para.append(str);
                        para.append(" ");
                    }
                    getline(inFile, str);
                }
            }
        }
        if (one == 1)
        {
            // ef2文件中只有一条链接直接发送给IDM
            pushToIDM(para);
        }
        else
        {
            // ef2文件中不止一条链接，以添加队列方式发送最后一条链接给IDM（需要手动开始下载队列）
            para.append("-q");
            pushToIDM(para);
            MessageBox(NULL, _T("该ef2文件包含复数个下载链接，为避免弹出过多对话框，已全部添加IDM到下载队列，请自行到IDM里开始队列！"), _T("ef2解析"), MB_OK);
        }
        inFile.close();
    }
}

int main(int argc, char *argv[])
{
    string str = "";
    string type = "";
    for (int i = 0; i < argc; i++)
    {
        if (strstr(argv[i], "ef2://"))
        {
            type = "protocol";
            str.append(argv[i]);
        }
        if (strstr(argv[i], ".ef2"))
        {
            type = "file";
            str.append(argv[i]);
        }
    }
    if (type == "protocol")
    {
        if (!str.empty())
        {
            ef2Protocol(str);
        }
    }
    if (type == "file")
    {
        if (!str.empty())
        {
            ef2File(str);
        }
    }
    return 0;
}