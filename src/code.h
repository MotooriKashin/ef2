#ifndef CODE_H
#define CODE_H
#include "stdfx.h"
#include "base64.h"

#import "IDM/IDManTypeInfo.tlb"
#include "IDM/IDManTypeInfo.h"
#include "IDM/IDManTypeInfo_i.c"
class Code
{
public:
    static std::string Utf8ToGbk(const char *src_str)
    {
        int len = MultiByteToWideChar(CP_UTF8, 0, src_str, -1, NULL, 0);
        wchar_t *wszGBK = new wchar_t[len + 1];
        memset(wszGBK, 0, len * 2 + 2);
        MultiByteToWideChar(CP_UTF8, 0, src_str, -1, wszGBK, len);
        len = WideCharToMultiByte(CP_ACP, 0, wszGBK, -1, NULL, 0, NULL, NULL);
        char *szGBK = new char[len + 1];
        memset(szGBK, 0, len + 1);
        WideCharToMultiByte(CP_ACP, 0, wszGBK, -1, szGBK, len, NULL, NULL);
        std::string strTemp(szGBK);
        if (wszGBK)
            delete[] wszGBK;
        if (szGBK)
            delete[] szGBK;
        return strTemp;
    };
    static std::string GbkToUtf8(const char *src_str)
    {
        int len = MultiByteToWideChar(CP_ACP, 0, src_str, -1, NULL, 0);
        wchar_t *wstr = new wchar_t[len + 1];
        memset(wstr, 0, len + 1);
        MultiByteToWideChar(CP_ACP, 0, src_str, -1, wstr, len);
        len = WideCharToMultiByte(CP_UTF8, 0, wstr, -1, NULL, 0, NULL, NULL);
        char *str = new char[len + 1];
        memset(str, 0, len + 1);
        WideCharToMultiByte(CP_UTF8, 0, wstr, -1, str, len, NULL, NULL);
        std::string strTemp = str;
        if (wstr)
            delete[] wstr;
        if (str)
            delete[] str;
        return strTemp;
    };
    static std::vector<std::string> StringToArray(std::string str)
    {
        std::string::size_type pos;
        std::vector<std::string> result;
        std::string pattern = " ";
        std::string check = "\"";
        str += pattern;
        int size = str.size();
        for (int i = 0; i < size; i++)
        {
            if (str.substr(i, 1) == check && i + 1 < size)
            {
                i++;
                pos = str.find(check, i + 1);
            }
            else
            {
                pos = str.find(pattern, i);
            }
            if (pos < size)
            {
                std::string s = str.substr(i, pos - i);
                result.push_back(s);
                i = pos + pattern.size() - 1;
            }
        }
        return result;
    };
    static void ef2Protocol(std::string lpLink)
    {
        std::string temp, result;
        lpLink.erase(0, 6);
        Base64::Decode(lpLink, &temp);
        result = Utf8ToGbk(temp.c_str());
        if (result.find("-u ") != std::string::npos)
        {
            SendLinkToIDM(result);
            if (result.find("-q") != std::string::npos)
            {
                MessageBox(NULL, TEXT("已添加到下载队列，需要开始下载时请到IDM点击开始队列！"), TEXT("静默下载"), MB_OK);
            }
        }
        else
        {
            MessageBox(NULL, TEXT("非法ef2链接！请查阅相关文档：https://github.com/MotooriKashin/ef2"), NULL, MB_ICONERROR);
        }
    };
    static void ef2File(std::string path)
    {
        std::ifstream inFile;
        inFile.open(path.c_str());
        std::string temp;
        std::string result = "";
        std::string fin = "";
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
                        fin = Utf8ToGbk(result.c_str());
                        /* ef2文件中不止一条链接，以添加队列方式发送前一条链接给IDM（需要手动开始下载队列） */
                        fin.append("-q");
                        SendLinkToIDM(fin);
                    }
                    result = "";
                    getline(inFile, temp);
                    while (temp != ">" && !inFile.eof())
                    {
                        if (temp.find("referer") != std::string::npos)
                        {
                            result.append("-r ");
                            result.append(temp.substr(9));
                            result.append(" ");
                        }
                        else if (temp.find("User-Agent") != std::string::npos)
                        {
                            result.append("-a \"");
                            result.append(temp.substr(12));
                            result.append("\" ");
                        }
                        else if (temp.find("cookies") != std::string::npos)
                        {
                            result.append("-c \"");
                            result.append(temp.substr(9));
                            result.append("\" ");
                        }
                        else if (temp.find("postdata") != std::string::npos)
                        {
                            result.append("-d \"");
                            result.append(temp.substr(10));
                            result.append("\" ");
                        }
                        else if (temp.find("username") != std::string::npos)
                        {
                            result.append("-U \"");
                            result.append(temp.substr(10));
                            result.append("\" ");
                        }
                        else if (temp.find("password") != std::string::npos)
                        {
                            result.append("-P \"");
                            result.append(temp.substr(10));
                            result.append("\" ");
                        }
                        else if (temp.find("filepath") != std::string::npos)
                        {
                            result.append("-o \"");
                            result.append(temp.substr(10));
                            result.append("\" ");
                        }
                        else if (temp.find("filename") != std::string::npos)
                        {
                            result.append("-s \"");
                            result.append(temp.substr(10));
                            result.append("\" ");
                        }
                        else if (temp.find("://") != std::string::npos)
                        {
                            result.append("-u ");
                            result.append(temp);
                            result.append(" ");
                        }
                        getline(inFile, temp);
                    }
                }
            }
            fin = Utf8ToGbk(result.c_str());
            if (one == 1)
            {
                /* ef2文件中只有一条链接直接发送给IDM */
                SendLinkToIDM(fin);
            }
            else if (one == 0)
            {
                MessageBox(NULL, TEXT("该ef2文件似乎并不包含有效下载链接！？\r\n请检查文件内容或者反馈到：https://github.com/MotooriKashin/ef2/issues"), TEXT("ef2解析"), MB_ICONERROR);
            }
            else
            {
                /* ef2文件中不止一条链接，以添加队列方式发送最后一条链接给IDM（需要手动开始下载队列） */
                fin.append("-q");
                SendLinkToIDM(fin);
                MessageBox(NULL, TEXT("该ef2文件包含复数个下载链接，为避免弹出过多对话框，已全部添加IDM到下载队列，请自行到IDM里开始队列！"), TEXT("ef2解析"), MB_ICONWARNING);
            }
            inFile.close();
        }
    };
    static int CALLBACK SendLinkToIDM(std::string str)
    {
        CComBSTR u; // Url to download
        CComBSTR a; // User-Agent
        CComBSTR c; // cookies
        CComBSTR d; // PostData (if using POST method)
        CComBSTR r; // Referer
        CComBSTR U; // UserName (if server requires authentication)
        CComBSTR P; // Password
        CComBSTR o; // LocalPath (where to save a file on your computer)
        CComBSTR s; // LocalFileName (file name to save with)
        long f = 0; // 0: default; 1: do not show any confirmations dialogs; 2: add to queue only, do not start downloading.

        std::vector<std::string> argv = StringToArray(str);
        int argc = argv.size();
        for (int i = 0; i < argc; i++)
        {
            if (!strcmp(argv[i].c_str(), "-u") && (i + 1 != argc))
            {
                u.Append(argv[i + 1].c_str());
                i++;
            }
            if (!strcmp(argv[i].c_str(), "-a") && (i + 1 != argc))
            {
                a.Append(argv[i + 1].c_str());
                i++;
            }
            if (!strcmp(argv[i].c_str(), "-c") && (i + 1 != argc))
            {
                c.Append(argv[i + 1].c_str());
                i++;
            }
            if (!strcmp(argv[i].c_str(), "-d") && (i + 1 != argc))
            {
                d.Append(argv[i + 1].c_str());
                i++;
            }
            if (!strcmp(argv[i].c_str(), "-r") && (i + 1 != argc))
            {
                r.Append(argv[i + 1].c_str());
                i++;
            }
            if (!strcmp(argv[i].c_str(), "-U") && (i + 1 != argc))
            {
                U.Append(argv[i + 1].c_str());
                i++;
            }
            if (!strcmp(argv[i].c_str(), "-P") && (i + 1 != argc))
            {
                P.Append(argv[i + 1].c_str());
                i++;
            }
            if (!strcmp(argv[i].c_str(), "-o") && (i + 1 != argc))
            {
                o.Append(argv[i + 1].c_str());
                i++;
            }
            if (!strcmp(argv[i].c_str(), "-s") && (i + 1 != argc))
            {
                s.Append(argv[i + 1].c_str());
                i++;
            }
            if (!strcmp(argv[i].c_str(), "-f"))
            {
                f |= 1;
            }
            if (!strcmp(argv[i].c_str(), "-q"))
            {
                f |= 2;
            }
        }
        if (!u)
        {
            MessageBox(NULL, TEXT("未发现有效下载链接！"), TEXT("SendLinkToIDM"), MB_ICONWARNING);
            return -1;
        }

        CoInitialize(NULL);
        ICIDMLinkTransmitter2 *pIDM;
        HRESULT hr = CoCreateInstance(CLSID_CIDMLinkTransmitter, NULL, CLSCTX_LOCAL_SERVER, IID_ICIDMLinkTransmitter2, (void **)&pIDM);
        if (S_OK == hr)
        {
            VARIANT var1, var2;
            VariantInit(&var1);
            VariantInit(&var2);
            if (u)
            {
                var1.vt = VT_BSTR;
                var1.bstrVal = u;
            }
            else
            {
                var1.vt = VT_EMPTY;
                var2.vt = VT_EMPTY;
            }
            hr = pIDM->SendLinkToIDM2(u, r, c, d, U, P, o, s, f, var1, var2);
            if (S_OK != hr)
            {
                MessageBox(NULL, TEXT("拉起IDM失败！IDM返回信息异常！"), TEXT("SendLinkToIDM"), MB_ICONWARNING);
                return -1;
            }
        }
        else
        {
            MessageBox(NULL, TEXT("拉起IDM失败！未找到IDM的COM组件！"), TEXT("SendLinkToIDM"), MB_ICONWARNING);
            return -1;
        }
        CoUninitialize();
        return 0;
    };
};

#endif // CODE_H