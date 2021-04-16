这是[Internet Download Manager (IDM)](http://www.internetdownloadmanager.com/)辅助工具，用以拓展调用IDM下载的方式。  
支持解析IDM导出文件(.ef2)并自定义了名为`ef2`的调用协议，用以配合用户脚本(UserScript)主动配置并调用IDM进行下载。
支持配置：
   - 下载链接
   - referer
   - User-Agent
   - cookies
   - post data *如果该链接要求使用 POST 方式下载的话*
   - username/password
   - 下载目录 *使用Windows目录格式——注意反斜杠 如`D:\Download\IDM`*
   - 文件名 *请一并提供文件拓展名 如 `1.mp4`*

### 工具目的
IDM的自带媒体嗅探、下载捕获等功能已经做得足够好，本工具旨在辅助用户脚本(UserScript)以更主动的方式拉起或传递下载数据给IDM，因为IDM本身不支持这些功能：
   1. IDM自己捕获及导出文件导入都不支持配置下载目录和文件名。
   2. IDM自己捕获下载链接时referer或User-Agent可能并不正确而且不能自定义导致下载-403。
   3. IDM导出文件的导入步骤过于繁琐不支持双击打开。
   4. 网页中主动调用IDM下载链接需要右键2步而不是左键一步到位。  


### 安装/卸载
1. 下载[所有文件](https://github.com/MotooriKashin/ef2/archive/master.zip)或[发行版中的ef2.zip压缩包](https://github.com/MotooriKashin/ef2/releases/latest)并解压到任意目录
2. 运行`ef2.exe`点击“安装”按钮即可，也可以通过运行`setup.bat`批处理文件来安装 *注意允许管理员权限！*
3. 如需卸载，可再运行`ef2.exe`并点击“卸载”，或者运行`uninstall.bat` *同样需要管理员权限！*

### 如何使用
用户脚本可自定按格式生成IDM导出文件(.ef2)让用户保存到本地，双击打开即可拉起IDM。
也可将网页中提供的下载链接翻译成本工具自定义的ef2协议链接，左键单击即可拉起IDM。
#### 1. 使用IDM导出文件(.ef2)——适用于批量下载  
这是一个只有一条下载数据的IDM导出文件(.ef2)以纯文本形式打开的样子：
```
<
http://112.48.167.3/upgcxcode/64/98/304509864/304509864_nb2-1-30080.m4s?e=ig8euxZM2rNcNbdlhoNvNC8BqJIzNbfqXBvEuENvNC8aNEVEtEvE9IMvXBvE2ENvNCImNEVEIj0Y2J_aug859IB_&uipk=5&nbs=1&deadline=1614744363&gen=playurlv2&os=bcache&oi=3086813462&trid=380d8edeb31f490da4218197dfa3324du&platform=android_i&upsig=7fa0207c16000f70b55a21fa6918e2b3&uparams=e,uipk,nbs,deadline,gen,os,oi,trid,platform&cdnid=6743&mid=49811844&orderid=0,3&logo=80000000
referer: https://www.bilibili.com/
User-Agent: Bilibili Freedoooooom/MarkII
>

```
   - IDM导出文件(.ef2)导出文件是IDM官方定义的下载文件格式，默认支持配置referer和User-Agent。
   - 本工具支持直接在文件管理器中双击打开.ef2文件推送给IDM，免去IDM->任务->导入->从"IDM 导出文件"导入这些多余的步骤。
      + 若.ef2文件中只有1条下载数据，就直接拉起IDM下载对话框。
      + 若.ef2文件中不止一条下载数据，则将所有下载数据添加到IDM默认下载队列，需要到IDM中自行点击“开始队列”。
   - 本工具拓展了.ef2文件的数据配置用以定制额外的需求，以上面那条下载数据为例，您可以在`User-Agent`和结束符`>`之间键入依行其他数据。
      + 若要设置下载目录为`D:\Download\IDM`，请另起一行输入`filepath: D:\\Download\\IDM`。 **请确保使用双反斜杠**
      + 若要设置文件名为`123.m4v`，请另起一行键入`filename: 123.m4v`。
      + 其他还支持`cookies`、`postdata`、`username`和`password`，即上面列出的所有支持配置，依格式添加即可。  
修改过的文件变成下面这样：
```
<
http://112.48.167.3/upgcxcode/64/98/304509864/304509864_nb2-1-30080.m4s?e=ig8euxZM2rNcNbdlhoNvNC8BqJIzNbfqXBvEuENvNC8aNEVEtEvE9IMvXBvE2ENvNCImNEVEIj0Y2J_aug859IB_&uipk=5&nbs=1&deadline=1614744363&gen=playurlv2&os=bcache&oi=3086813462&trid=380d8edeb31f490da4218197dfa3324du&platform=android_i&upsig=7fa0207c16000f70b55a21fa6918e2b3&uparams=e,uipk,nbs,deadline,gen,os,oi,trid,platform&cdnid=6743&mid=49811844&orderid=0,3&logo=80000000
referer: https://www.bilibili.com/
User-Agent: Bilibili Freedoooooom/MarkII
filepath: D:\\Download\\IDM
filename: 123.m4v
>

```
#### 2. 使用本工具自定义的`ef2`协议直接从浏览器中拉起IDM——适用响应左键单条链接  
这是一个网页中以`a`标签形式存在的标准下载链接的样子：
```
<a href="http://112.48.167.3/upgcxcode/64/98/304509864/304509864_nb2-1-30080.m4s" download="123.m4v">123.m4v</a>
```
ef2协议是本工具自定义的一种链接协议，基于一般超链接添加上面那些参数编码而成。  
比如浏览器中可能存在的类似于上面这个`a`标签的下载链接，可以直接在href属性链接最前方添加上`ef2://`，就变成了`ef2://http://112.48.167.3/upgcxcode/64/98/304509864/304509864_nb2-1-30080.m4s`。  
这样在浏览器中点击该链接就会直接拉起IDM开始下载该http链接。  
但这样只是支持了在浏览器中直接左键拉起IDM，要配置其他数据还需要进一步编码 *为了方便表述这里换一条“短链接”`http://www.x.x/123.m4v`为例：*
   1. 添加链接参数"-u"：即`-u http://www.x.x/123.m4v`  
   2. 如果需要配置referer为`http://www.x.x/`：就变成了：`-u http://www.x.x/123.m4v -r http://www.x.x/`
   3. 如果需要配置User-Agent为`Bilibili Freedoooooom/MarkII`：就变成了`-u http://www.x.x/123.m4v -r http://www.x.x/ -a "Bilibili Freedoooooom/MarkII"` *因为该字符串中存在空格所以得用双引号括起来*
   4. 其他参数需要的话按同样的格式拼接起来即可，支持的全部参数如下： **所有可能存在空格的参数请都用双引号括起来！**
       - `-u` —— 下载链接 如：`-u http://www.x.x/123.m4v`
       - `-r` —— referer 如：`-r http://www.x.x/`
       - `-a` —— User-Agent 如：`-a "Bilibili Freedoooooom/MarkII"`
       - `-c` —— cookies 如：`-c xxxxxxx`
       - `-d` —— post data 如：`-d yyyyyyy`
       - `-U` —— username 如：`-U zzzzzzz`
       - `-P` —— password 如：`-P wwwwwww`
       - `-o` —— 下载目录 如：`-o D:\Download\IDM`
       - `-s` —— 文件名 如：`-s 1.mp4`
       - `-f` —— 禁用IDM下载对话框 *该参数只需添加`-f`即可*
       - `-q` —— 添加到IDM队列而不是立即下载 *该参数只需添加`-q`即可*
   5. 上面所有参数按需添加即可，不用全部配置，`-u`也就是下载链接本身除外 **如果链接都不存在那还下载什么？！**
   6. **参数之间没有排序要求！** 下面展示把所有参数用上最后可能的样子
```
-u http://www.x.x/123.m4v -r http://www.x.x/ -a "Bilibili Freedoooooom/MarkII" -c xxxxxxx -d yyyyyyy -U zzzzzzz -P wwwwwww -o D:\Download\IDM -s 1.mp4 -f -q
```
*再次强调参数请按需添加！*

   7. 这样的最终链接包含“空格”“反斜杠”等**非法符号**，所以得用base64编码一下：
```
LXUgaHR0cDovL3d3dy54LngvMTIzLm00diAtciBodHRwOi8vd3d3LngueC8gLWEgIkJpbGliaWxpIEZyZWVkb29vb29vbS9NYXJrSUkiIC1jIHh4eHh4eHggLWQgeXl5eXl5eSAtVSB6enp6enp6IC1QIHd3d3d3d3cgLW8gRDpcRG93bmxvYWRcSURNIC1zIDEubXA0IC1mIC1x
```

   8. 最后以`ef2`协议的样式返还到网页的`a`标签中即可  
```
<a href="ef2://LXUgaHR0cDovL3d3dy54LngvMTIzLm00diAtciBodHRwOi8vd3d3LngueC8gLWEgIkJpbGliaWxpIEZyZWVkb29vb29vbS9NYXJrSUkiIC1jIHh4eHh4eHggLWQgeXl5eXl5eSAtVSB6enp6enp6IC1QIHd3d3d3d3cgLW8gRDpcRG93bmxvYWRcSURNIC1zIDEubXA0IC1mIC1x">1.mp4</a>
```
   9. 然后用户在网页中点击该链接即可拉起IDM并传递所有数据

*`ef2.js`提供了一个es6标准的ef2协议编解码库，可以引入到项目中将下载链接转化为ef2协议（也可将核心代码直接整合进项目，代码以 MIT 许可开源）* 参看如下示例：  
```
/**
 * ef2编码
 * @param {string} url 下载链接
 * @param {string} [referer] referer
 * @param {string} [userAgent] userAgent
 * @param {string} [path] 下载目录
 * @param {string} [filename] 文件名，包含拓展名
 * @param {Boolean} [f] 禁用IDM下载对话框，默认不禁用
 * @param {Boolean} [q] 稍后下载，添加到下载队列而不立即开始，默认直接下载
 * @returns {Promise<string>} ef2协议字符串
 */
async ef2Encode(url, referer = location.origin, userAgent = navigator.userAgent, path = "", filename = "", f = false, q = false) {
   let ef2 = (await import("/ef2.js")).default; // "/ef2.js"这里使用相对路径，实际情况请使用https绝对路径
   let result = await ef2.encode(
      {
         u: url,
         r: referer,
         a: userAgent,
         o: path,
         s: filename,
         f: f,
         q: q,
      }
   );
   return result;
}

let data = await ef2Encode({
   "http://www.example.com/example.mp4",
   "http://www.example.com/",
   "Bilibili Freedoooooom/MarkII",
   "F:\\下载\\IDM", \\ 反斜杠是js转义符，以字符串输入时自身也要转义所以使用双斜杠
   "example.mp4",
   false, \\ 不取消下载对话框
   true \\ 稍后下载：添加到队列而不立即下载
});
console.log(data);
// 打印结果为：ef2://LXUgaHR0cDovL3d3dy5leGFtcGxlLmNvbS9leGFtcGxlLm1wNCAtYSAiQmlsaWJpbGkgRnJlZWRvb29vb29tL01hcmtJSSIgLXIgaHR0cDovL3d3dy5leGFtcGxlLmNvbS8gLW8gRjpc5LiL6L29XElETSAtcyBleGFtcGxlLm1wNCAtcQ==
```

*[Bilibili Old](https://github.com/MotooriKashin/Bilibili-Old/)设置中开启ef2辅助有使用该自定义协议拉起IDM的示例。*
### 编译相关
环境：
> Windows 8  
> VSCode 1.55.0  
> C/C++ Extension 1.3.0  
> MinGW  8.1.0

- tasks.json、Makefile等配置文件已写好，可以f5编译即可（MinGW需自行配置或者添加系统环境变量）。
- 如需调试，可将Makefile文件中`-static -O3 -DNDEBUG -s`4个参数去掉并添加`-g`参数，还要在`ef2.cpp`的入口函数`WinMain`中注释掉“管理员提权”那段代码，调试功能才能恢复正常。 **读写注册表相关将因权限问题失效！**
- 注册表相关数据可参考`setup.bat`和`uninstall.bat`运行残留的`1.reg`文件，程序安装/卸载只是使用相关API进行同样条目的读写。

### 参考致谢
- [unamer](https://github.com/unamer/IDMHelper)：调用IDM的核心二进制文件，本工具其实是二次拉起该项目提供的二进制文件
- [臭咸鱼](https://www.cnblogs.com/chouxianyu/p/11249810.html)：vscode编译c++中文乱码问题
- [踏莎行hyx](https://blog.csdn.net/u012234115/article/details/83186386)：utf-8转gbk的c++代码
- [tkislan](https://github.com/tkislan/base64)：base64的c++库  
- [IDM](http://www.internetdownloadmanager.com/support/idm_api.html)：IDM官方 COM 组件API
- [js-base64](https://github.com/dankogai/js-base64)：js-base64库