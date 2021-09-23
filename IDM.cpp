// IDM.cpp : 负责与IDM通讯的模块

// 此代码模块中包含的函数的前向声明: 
int CALLBACK SendLinkToIDM(std::string);
std::vector<std::string> StringToArray(std::string);

// 拉起 IDM
int CALLBACK SendLinkToIDM(std::string str) {
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
    for (int i = 0; i < argc; i++) {
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
    if (!u) {
        MessageBox(NULL, TEXT("未发现有效下载链接！"), TEXT("SendLinkToIDM"), MB_ICONWARNING);
        return -1;
    }

    CoInitialize(NULL);
    ICIDMLinkTransmitter2* pIDM;
    HRESULT hr = CoCreateInstance(CLSID_CIDMLinkTransmitter, NULL, CLSCTX_LOCAL_SERVER, IID_ICIDMLinkTransmitter2, (void**)&pIDM);
    if (S_OK == hr) {
        VARIANT var1, var2;
        VariantInit(&var1);
        VariantInit(&var2);
        if (u)
        {
            var1.vt = VT_BSTR;
            var1.bstrVal = u;
        }
        else {
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
    else {
        MessageBox(NULL, TEXT("拉起IDM失败！未找到IDM的COM组件！"), TEXT("SendLinkToIDM"), MB_ICONWARNING);
        return -1;
    }
    CoUninitialize();
    return 0;
}

// 转化参数字符串为参数组
std::vector<std::string> StringToArray(std::string str) {
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
        else { pos = str.find(pattern, i); }
        if (pos < size)
        {
            std::string s = str.substr(i, pos - i);
            result.push_back(s);
            i = pos + pattern.size() - 1;
        }
    }
    return result;
}