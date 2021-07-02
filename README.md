[Internet Download Manager (IDM)](http://www.internetdownloadmanager.com/)下载辅助工具：支持解析IDM导出文件(.ef2)，支持使用自定义协议从浏览器直接拉起IDM。  
- 使IDM导出文件(.ef2)支持双击打开。
- 拓展IDM导出文件(.ef2)配置内容，支持重命名文件、保存目录等。
- 支持使用自定义协议(ef2://)直接从浏览器拉起IDM并配置Referer、User-Agent、Cookies、保存目录、重命名文件等。

---
### 安装/卸载
1. 下载[ef2.exe](https://github.com/MotooriKashin/ef2/releases/latest)到任意目录。 *作为安装目录*
2. 运行`ef2.exe`点击“安装”。 *注意允许管理员权限！*
3. 如需卸载，再次运行`ef2.exe`并点击“卸载”。 *同样需要管理员权限！*
4. 如果移动了`ef2.exe`，需要重新“卸载”再“安装”。

*项目中的`setup.bat`和`uninstall.bat`批处理文件如果放在`ef2.exe`同目录中也可以代理安装/卸载操作。*  
*`ef2.exe`文件请尽量放在一个单独的目录，不要放在桌面，因为“安装”后除非要卸载不然你基本不需要主动用到它。*  
成功“安装”后，就可以直接在Windows资源管理器里双击打开任意IDM导出文件(.ef2)，同时在浏览器中左键点击自定义的(ef2://)协议链接也能直接拉起IDM了。  
配合用户脚本(UserScript)，您可以将下载链接导出为ef2文件来批量下载，或者翻译为ef2协议来直接拉起IDM。  
当然IDM本身就支持捕获和右键拉起，本工具真正目的在于自定义URL之外的数据。

---
### 如何使用
#### 使用IDM导出文件(.ef2)——适用于批量下载  
这是一个只有一条下载数据的IDM导出文件(.ef2)以纯文本形式打开的样子：
```
<
http://112.48.167.3/upgcxcode/64/98/304509864/304509864_nb2-1-30080.m4s?e=ig8euxZM2rNcNbdlhoNvNC8BqJIzNbfqXBvEuENvNC8aNEVEtEvE9IMvXBvE2ENvNCImNEVEIj0Y2J_aug859IB_&uipk=5&nbs=1&deadline=1614744363&gen=playurlv2&os=bcache&oi=3086813462&trid=380d8edeb31f490da4218197dfa3324du&platform=android_i&upsig=7fa0207c16000f70b55a21fa6918e2b3&uparams=e,uipk,nbs,deadline,gen,os,oi,trid,platform&cdnid=6743&mid=49811844&orderid=0,3&logo=80000000
referer: https://www.bilibili.com/
User-Agent: Bilibili Freedoooooom/MarkII
>
```
该文件可由IDM导出而来，本质上是一个纯文本文件(类似于.txt)，所以完全也可以按格式自己生成并保存为.ef2文件。
我们来分析一下该文件格式：
   1. 首先一对尖括号定义一条下载数据，所以一个.ef2文件可包括复数条下载数据。  
   2. 尖括号内部第一行是下载链接URL。
   3. 再往下是下载该URL的所需配置，格式为`名称: 内容`。这些数据可以不存在，它们可能用途是发送给服务器鉴别用户是否有权下载该URL。
      - referer 代表下载链接来源，一般为网站顶级域名，服务器可能据此拒绝来自其他站点的下载请求（即禁止外链）。
      - User-Agent 是下载工具识别码，服务器可能据此限制只允许特定客户端下载（禁止第三方下载工具）。
   4. IDM能识别的内容最多只有上面两种，这意味着我们通过编辑ef2文件能自定义也就是这些。但本工具通过调用IDM编程接口实现了自定义更多：
      - cookies 即浏览器识别用户身份的数据，服务器可能据此限制登录或者会员才能下载。
      - postdata 如果服务器要求使用`POST`方式请求下载的话。
      - username 如果服务器要求用户身份认证的用户名。
      - password 和上者匹配的密钥。
      - filepath 本地保存目录。（需要转义反斜杠，如`F:\\下载`）
      - filename 重命名文件。（需要带上拓展名，如`视频.mp4`）
   5. 这些数据一般真正常用的也就是“保存目录”和“重命名”了，直接以`名称: 内容`添加进.ef2文件的数据中即可。

这是一个添加了自定义数据的ef2文件以纯文本打开的例子：
```
<
http://112.48.167.3/upgcxcode/64/98/304509864/304509864_nb2-1-30080.m4s?e=ig8euxZM2rNcNbdlhoNvNC8BqJIzNbfqXBvEuENvNC8aNEVEtEvE9IMvXBvE2ENvNCImNEVEIj0Y2J_aug859IB_&uipk=5&nbs=1&deadline=1614744363&gen=playurlv2&os=bcache&oi=3086813462&trid=380d8edeb31f490da4218197dfa3324du&platform=android_i&upsig=7fa0207c16000f70b55a21fa6918e2b3&uparams=e,uipk,nbs,deadline,gen,os,oi,trid,platform&cdnid=6743&mid=49811844&orderid=0,3&logo=80000000
referer: https://www.bilibili.com/
User-Agent: Bilibili Freedoooooom/MarkII
filepath: F:\\下载
filename: 视频.mp4
>
```

#### 2. 使用本工具自定义的`ef2`协议直接从浏览器中拉起IDM
网页中的下载链接本质可能是一个`a`标签，如：
```
<a href="http://112.48.167.3/upgcxcode/64/98/304509864/304509864_nb2-1-30080.m4s" download="123.m4v">123.m4v</a>
```
简要解析一下其中的含义：
   - 123.m4v 网页中真正显示的文字。
   - href 下载URL，鼠标点击跳转去的链接。
   - download 指示这是一个下载链接并重命名文件

现在我们改造一下`href`中的链接，变成鼠标单击能直接拉起本工具发送给IDM的样子。如果不需要配置其他自定义数据的，直接在`http`前面加上`ef2://`最简单。即`ef2://http://112.48.167.3/upgcxcode/64/98/304509864/304509864_nb2-1-30080.m4s`。  
但这样简单处理意义并不大，为了能自定义更多数据，需要换一种处理方式，通用格式为`-类型 数据`
   - `-u http://112.48.167.3/upgcxcode/64/98/304509864/304509864_nb2-1-30080.m4s` （URL）
   - `-r https://www.bilibili.com/` (referer)
   - `-a "Bilibili Freedoooooom/MarkII"` （User-Agent）
   - `-c xxxxxxx` （cookies）
   - `-d yyyyyyy` （postdata）
   - `-U zzzzzzz` （username）
   - `-P wwwwwww` （password）
   - `-o F:\\下载` （保存目录）
   - `-s 视频.mp4` （重命名）
   - `-f` （禁用IDM下载对话框）
   - `-q` （添加到IDM队列而不是立即下载）

*注意数据中带空格的话请用双引号括起来，如上面的User-Agent*  
*所有参数除了URL都可是按需添加且没有顺序，不过只有URL的话也用不着本工具，直接右键IDM下载就是。*  
*保存目录由于用到了反斜杠所以务必注意转义问题*  
*最后两个设置只有类型无需数据，并且最多选一个*  
第一步：将需要的数据用空格连起来，如
 ```
 -u http://112.48.167.3/upgcxcode/64/98/304509864/304509864_nb2-1-30080.m4s -r https://www.bilibili.com/ -a "Bilibili Freedoooooom/MarkII" -o F:\\下载 -s 视频.mp4 -q
 ```
*这个例子配置了URL、referer、User-Agent、保存目录、重命名并取消了立即下载*  
第二步：将链起来的数据用Base64编码一下：
 ```
LXUgaHR0cDovLzExMi40OC4xNjcuMy91cGdjeGNvZGUvNjQvOTgvMzA0NTA5ODY0LzMwNDUwOTg2NF9uYjItMS0zMDA4MC5tNHMgLXIgaHR0cHM6Ly93d3cuYmlsaWJpbGkuY29tLyAtYSAiQmlsaWJpbGkgRnJlZWRvb29vb29tL01hcmtJSSIgLW8gRjpcXOS4i+i9vSAtcyDop4bpopEubXA0
 ```
第三步：添加上`ef2://`头替换进`a`标签的`href`属性中
 ```
<a href="ef2://LXUgaHR0cDovLzExMi40OC4xNjcuMy91cGdjeGNvZGUvNjQvOTgvMzA0NTA5ODY0LzMwNDUwOTg2NF9uYjItMS0zMDA4MC5tNHMgLXIgaHR0cHM6Ly93d3cuYmlsaWJpbGkuY29tLyAtYSAiQmlsaWJpbGkgRnJlZWRvb29vb29tL01hcmtJSSIgLW8gRjpcXOS4i+i9vSAtcyDop4bpopEubXA0" download="123.m4v">123.m4v</a>
 ```
最后：用户点击该`a`标签即可拉起IDM，自定义的referer、User-Agent、保存目录和重命名一并传递给了IDM，并且直接添加到下载列表而不立即可开始下载(`-q`参数的效果)。    

项目中的`ef2.js`文件是一个生成`ef2`协议的es6模块，可以通过es6载入模块命令载入，以`default`导出了一个`ef2`对象同时挂载到了`window`下。其中`ef2.encode()`方法接收一个对象并将其转化为`ef2`协议，如：
```
ef2.encode({
   u: "http://www.example.com/example.mp4",
   a: "Bilibili Freedoooooom/MarkII",
   r: "http://www.example.com/",
   o: "F:\\下载\\IDM",
   s: "example.mp4",
   q: true });
```
*`-f`和`-q`由于无数据所以务必传入“真值”，js中任意真值皆可。*  
`ef2.dncode()`方法是`ef2.encode()`方法的反过程。  
注意二者都返回`promise`，可以链式调用，也可以使用`await`关键词获取真正的返回值。

---
### 编译相关
环境：
> Windows 8  
> VSCode 1.55.0  
> C/C++ Extension 1.3.0  
> MSVC 2015

- 由于IDM是Windows平台的下载工具，而官方又以COM组件形式暴露的编程接口，所以项目从1.0版本起正式弃用`MinGW`而使用`vs2015`自带的`msvc`作为编译器，以获得最佳效果并取消对于第三方二进制文件的依赖。  
- 编译平台仍使用`VSCode`，通过配置`launch.json`和`tasks.json`以命令行形式调用`cl.exe`、`rc.exe`以及`link.exe`编译并链接，所以需要自行配置`msvc`进环境变量。  
- 调用COM组件不可避免调用了`ATL`和`Windows SDK`，请一并添加进环境变量`include`。这些包括上面的环境变量对于vs的不同版本有不同的目录，所以无法给出一个统一的配置样例。以下所举全为`vs2015`在`Windows8`上默认目录，其他Windows版本或VS版本请自行对照修改。  
   + path：编译器`cl.exe`所在目录，`Windows Kits`资源编译器`rc.exe`所在目录。
      - 例如：`C:\Program Files (x86)\Microsoft Visual Studio 14.0\VC\bin\amd64;C:\Program Files (x86)\Windows Kits\8.1\bin\x64`
   + include：`VC++`库所在目录，`Windows Kits`中`um`、`shared`、`ucrt`以及`atlmfc`所在目录。
      - 例如：`C:\Program Files (x86)\Microsoft Visual Studio 14.0\VC\include;C:\Program Files (x86)\Windows Kits\8.1\Include\um;C:\Program Files (x86)\Windows Kits\8.1\Include\shared;C:\Program Files (x86)\Windows Kits\10\Include\10.0.10150.0\ucrt;C:\Program Files (x86)\Microsoft Visual Studio 14.0\VC\atlmfc\include`
   + lib：`VC++`lib库所在目录，`Windows Kits`中`ucrt`、`atlmfc`lib库所在目录，`Microsoft SDKs`lib库所在目录。
      - 例如：`C:\Program Files (x86)\Windows Kits\10\Lib\10.0.10150.0\ucrt\x64;C:\Program Files (x86)\Microsoft Visual Studio 14.0\VC\lib\amd64;C:\Program Files (x86)\Microsoft SDKs\Windows\v7.1A\Lib\x64;C:\Program Files (x86)\Microsoft Visual Studio 14.0\VC\atlmfc\lib\amd64`  
- 配置环境变量后可能出现编译调试正常但`VSCode`的C/C++拓展语法检查提示无法找不到某些源文件问题。请在`c_cpp_properties.json`（位于.vscode目录下，本人使用全局设定没有用该文件，如需要请自行创建——在VSCode打开cpp文件时点击右下角【win32】即可快速创建）中指定编译器为对应的`MSVC`并添加`include`。或者直接在`VSCode`的`settings.json`中添加如下项目：
   + `C_Cpp.default.intelliSenseMode`：`windows-msvc-x64`，即`MSVC`的64位桌面程序
   + `C_Cpp.default.compilerPath`：这里请精确到编译器`cl.exe`的绝对路径，如：`C:\\MSVC\\14.29.30130\\bin\\Hostx64\\x64\\cl.exe`
   + `C_Cpp.default.includePath`：这是一个数组，数组中请填入`Windows SDK`中所有`include`目录，对于`MSVC 2019`应该包括：winrt、shared、um三项  
- `ef2.cpp`是程序入口，打开该文件进行编译会在根目录生成`ef2.exe`，即是对外发布的二进制程序。  
- 直接在`VSCode`中打开`ef2.cpp`文件可能报错`无法打开 源 文件 ".\IDManTypeInfo.tlh"`，这是因为缺少`tlb`库对应的解析文件，会在编译过程中自动生成。因为该文件包含设备本地环境数据不适合对外发布，所以将这些文件添加进了`.gitignore`，只需编译一次就会在本地生成，然后就不会报错了。  
- 对外发布的`tasks.json`是release版本任务，如需调试请将`cl`任务的`args`参数替换成debug版本，在终端输入`cl.exe /?`有详细的配置说明。值得注意的是本项目编译和链接是分开的，因为要链接RC资源，所以请务必保留`/c`参数以禁用自动链接。  
- 本地调试时如果`launch.json`中没有传入参数，将进入UI绘制部分，那部分申请了管理员权限以至于无法调试，需要的话请注释掉`ef2.cpp`中“管理员提权”那部分代码，以便进行调试，只是这样会导致注册表相关操作失败。

### 参考致谢
- [unamer](https://github.com/unamer/IDMHelper)：IDM的编程接口使用示例，本项目1.0之前直接使用该项目提供的二进制文件负责拉起IDM。
- [臭咸鱼](https://www.cnblogs.com/chouxianyu/p/11249810.html)：vscode编译c++中文乱码问题。
- [踏莎行hyx](https://blog.csdn.net/u012234115/article/details/83186386)：utf-8转gbk的c++代码。
- [tkislan](https://github.com/tkislan/base64)：base64的c++库。  
- [IDM](http://www.internetdownloadmanager.com/support/idm_api.html)：IDM官方 COM 组件API。
- [js-base64](https://github.com/dankogai/js-base64)：js-base64库。