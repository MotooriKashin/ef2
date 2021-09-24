// code.cpp : 协议及编码处理模块

// 此代码模块中包含的函数的前向声明: 
string Utf8ToGbk(const char*);
string GbkToUtf8(const char*);
void ef2Protocol(string);
void ef2File(string);

// Utf8 -> Gbk
string Utf8ToGbk(const char* src_str)
{
    int len = MultiByteToWideChar(CP_UTF8, 0, src_str, -1, NULL, 0);
    wchar_t* wszGBK = new wchar_t[len + 1];
    memset(wszGBK, 0, len * 2 + 2);
    MultiByteToWideChar(CP_UTF8, 0, src_str, -1, wszGBK, len);
    len = WideCharToMultiByte(CP_ACP, 0, wszGBK, -1, NULL, 0, NULL, NULL);
    char* szGBK = new char[len + 1];
    memset(szGBK, 0, len + 1);
    WideCharToMultiByte(CP_ACP, 0, wszGBK, -1, szGBK, len, NULL, NULL);
    string strTemp(szGBK);
    if (wszGBK)
        delete[] wszGBK;
    if (szGBK)
        delete[] szGBK;
    return strTemp;
}

// Gbk -> Utf8
string GbkToUtf8(const char* src_str)
{
    int len = MultiByteToWideChar(CP_ACP, 0, src_str, -1, NULL, 0);
    wchar_t* wstr = new wchar_t[len + 1];
    memset(wstr, 0, len + 1);
    MultiByteToWideChar(CP_ACP, 0, src_str, -1, wstr, len);
    len = WideCharToMultiByte(CP_UTF8, 0, wstr, -1, NULL, 0, NULL, NULL);
    char* str = new char[len + 1];
    memset(str, 0, len + 1);
    WideCharToMultiByte(CP_UTF8, 0, wstr, -1, str, len, NULL, NULL);
    string strTemp = str;
    if (wstr)
        delete[] wstr;
    if (str)
        delete[] str;
    return strTemp;
}

// 处理ef2协议
void ef2Protocol(string lpLink)
{
    string temp, result;
    lpLink.erase(0, 6);
    Base64::Decode(lpLink, &temp);
    result = Utf8ToGbk(temp.c_str());
    if (result.find("-u ") != string::npos)
    {
        SendLinkToIDM(result);
        if (result.find("-q") != string::npos)
        {
            MessageBox(NULL, TEXT("已添加到下载队列，需要开始下载时请到IDM点击开始队列！"), TEXT("静默下载"), MB_OK);
        }
    }
    else
    {
        MessageBox(NULL, TEXT("非法ef2链接！请查阅相关文档：https://github.com/MotooriKashin/ef2"), NULL, MB_ICONERROR);
    }
}

// 处理ef2文件
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
                    SendLinkToIDM(result);
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
            SendLinkToIDM(result);
        }
        else if (one == 0)
        {
            MessageBox(NULL, TEXT("该ef2文件似乎并不包含有效下载链接！？\r\n请检查文件内容或者反馈到：https://github.com/MotooriKashin/ef2/issues"), TEXT("ef2解析"), MB_ICONERROR);
        }
        else
        {
            /* ef2文件中不止一条链接，以添加队列方式发送最后一条链接给IDM（需要手动开始下载队列） */
            result.append("-q");
            SendLinkToIDM(result);
            MessageBox(NULL, TEXT("该ef2文件包含复数个下载链接，为避免弹出过多对话框，已全部添加IDM到下载队列，请自行到IDM里开始队列！"), TEXT("ef2解析"), MB_ICONWARNING);
        }
        inFile.close();
    }
}