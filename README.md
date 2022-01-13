[Internet Download Manager (IDM)](http://www.internetdownloadmanager.com/)下载辅助工具，允许主动拉起IDM进行下载，并配置referer、User-Agent等信息。

---
### 安装/卸载
绿色命令行程序，只需要写入注册表关联：  
1. 下载[ef2.exe](https://github.com/MotooriKashin/ef2/releases/latest)到任意目录。
2. 双击打开ef2.exe，点击安装按钮即可。  

如果觉得不好用，需要卸载：  
1. 双击打开ef2.exe，点击卸载按钮。
2. 删除ef2.exe即可。

*ef2.exe最好放到专门的目录，以免移动或丢失，移动后须重新注册。*

---
### 使用方式
#### ※ IDM导出文件
IDM官方定义了专门的文件格式，拓展名.ef2，用于导入导出下载列表，不过用起来步骤较多且不支持双击打开。本工具不仅支持双击打开ef2文件开始下载并拓展了ef2文件的配置项支持。  
ef2文件本质是一个标准的纯文本文件，展开示例如下：

<details>
  <summary>查看ef2文件示例</summary>
  <pre>
<
https://img2.example.com/data/1102/94/IMG_0001.jpg
referer: https://img2.example.com/data/1102/94/
User-Agent: Mozilla/5.0 (Windows NT 6.1; Trident/7.0; rv:11.0) like Gecko
>
<
https://img2.example.com/data/1102/94/IMG_0002.jpg
referer: https://img2.example.com/data/1102/94/
User-Agent: Mozilla/5.0 (Windows NT 6.1; Trident/7.0; rv:11.0) like Gecko
>
<
https://img2.example.com/data/1102/94/IMG_0003.jpg
referer: https://img2.example.com/data/1102/94/
User-Agent: Mozilla/5.0 (Windows NT 6.1; Trident/7.0; rv:11.0) like Gecko
>
  </pre>
</details>

除了IDM官方提供referer、User-Agent关键词，本工具还拓展了一些关键词：

<details>
  <summary>查看拓展的关键词</summary>
  <table border="1">
    <tr>
        <td>cookies</td>
        <td>cookies 字符串</td>
    </tr>
    <tr>
        <td>postdata</td>
        <td>改用POST方法send的字符串</td>
    </tr>
    <tr>
        <td>username</td>
        <td>HTTP认证用户名</td>
    </tr>
    <tr>
        <td>password</td>
        <td>HTTP认证密钥</td>
    </tr>
    <tr>
        <td>filepath</td>
        <td>保存路径（反斜杠形式）</td>
    </tr>
    <tr>
        <td>filename</td>
        <td>文件名（包括拓展名）</td>
    </tr>
  </table>
</details>
值得注意的是如果ef2文件中有多条下载数据（即批量下载），双击打开将直接添加到IDM默认下载列表，需要到IDM中自己开启队列来开始下载。

---
#### ※ ef2协议
为了方便从浏览器中拉起IDM，本工具自定义了一种名为ef2的协议，能想使用http协议一样使用，格式如下：  
| 协议头 | 内容 | 完整形式 |
| ----- | ----- | ----- |
| ef2:// | d3d3LmV4YW1wbGUuY29t | ef2://d3d3LmV4YW1wbGUuY29t |
| http:// | www.example.com | http://www.example.com |

**不难发现所谓的ef2就是将http协议内容部分用Base64编码一下然后将http协议头改为ef2。**  
如此一来在浏览器中点击该链接便可以拉起IDM。但我们还需拓展一下，以让这种协议不仅能传递下载链接，还要能传递referer、User-Agent等关键词以支持ef2文件类似的功能。  
于是我们将协议内容部分由单纯的网址转变为**命令行参数**形式（为方便演示暂时未使用Base64编码）
>
> www.example.com ----参数化----> -u www.example.com
>

<details>
  <summary>查看完整参数说明</summary>
<table border="1">
    <tr>
        <th>参数标记</th>
        <th>参数内容</th>
        <th>说明示例</th>
    </tr>
    <tr>
      <td>u</td>
      <td>URL</td>
      <td><pre>-u https://img2.example.com/data/1102/94/IMG_0001.jpg</pre></td>
    </tr>
    <tr>
      <td>r</td>
      <td>referer</td>
      <td><pre>-r https://img2.example.com/</pre></td>
    </tr>
    <tr>
      <td>a</td>
      <td>User-Agent</td>
      <td><pre>-a Bilibili Freedoooooom/MarkII</pre></td>
    </tr>
    <tr>
      <td>c</td>
      <td>cookies</td>
      <td><pre>-a cookies字符串</pre></td>
    </tr>
    <tr>
      <td>d</td>
      <td>改用POST请求send的数据</td>
      <td><pre>-d send字符串</pre></td>
    </tr>
    <tr>
      <td>U</td>
      <td>HTTP认证用户名</td>
      <td><pre>-U username字符串</pre></td>
    </tr>
    <tr>
      <td>P</td>
      <td>HTTP认证密钥</td>
      <td><pre>-P password字符串</pre></td>
    </tr>
    <tr>
      <td>o</td>
      <td>保存路径（反斜杠）</td>
      <td><pre>-o F:\下载</pre></td>
    </tr>
    <tr>
        <td>s</td>
        <td>文件名（含拓展名）</td>
        <td><pre>-s 视频.mp4</pre></td>
    </tr>
    <tr>
        <td>f</td>
        <td>禁用下载对话框</td>
        <td><pre>-f</pre></td>
    </tr>
    <tr>
        <td>q</td>
        <td>稍后下载（加入默认队列）</td>
        <td><pre>-q</pre></td>
    </tr>
</table>
</details>

注意别忘了用Base64编码一下然后添加上ef2协议头。  
这种ef2协议使用方式基本类似http、ftp等协议，可以直接在浏览器地址栏中输入、添加a标签等href属性并点击、`window.open()`等方法打开，当然也可以直接作为命令行参数传递给ef2.exe，Base64编码千万别忘记。  
**第一次使用本方法浏览器会弹出打开外部程序请求弹窗，需要点击允许，建议勾选不再提示。**

#### ※ webSocket
本工具还支持在后台运行，通过websocket协议接受下载数据并传递给IDM。  
1. 命令行启动ef2.exe同时传递参数`ef2://websocket:${端口号}`，如`ef2://websocket:12345`。（**不需要Base64加密！**）
2. 使用websocket客户端连接`ws:\\ws://localhost:${端口号}`，如`ws://localhost:12345`。
3. 使用websocket.send方法发起下载数据，格式可以是ef2协议（需要Base64编码）字符串。
4. 使用websocket.close关闭websocket连接。
5. 如果要退出后台运行，请在websocket.close前使用websocket.send方法发送字符串`fin`。

注意也可以用使用ef2协议的方式使用`ef2://websocket:${端口号}(不需要Base64加密)`后台运行。

#### ※ ef2.js
本项目自带了一个标准的es6模块ts/js文件，以`default`方式导出了一个对象，支持生成ef2协议字符串。
1. 以标准es6模块加载方式加载该模块，如`const ef2 = (await import("ef2.js")).default`。
2. 使用`encode()`方式返回编码生成ef2协议字符串，`const result = ef2.encode({url: "http://www.example.com/"})`。
3. 使用生成的ef2协议拉起IDM进行下载，如`window.open(result)`。

1.0.2版本，新添了一个异步方法`nativeMessaging`，传递端口号可以启动webSocket模式。  

具体模块定义在ts文件中有详细说明文档，在VSCode中可以获取详细提示。

---
### 编译相关
| 环境 | 版本 |
| --- | --- |
| Windows 8 | 6.2.9200.16442 |
| VSCode | 1.63.2 |
| MSVC | 19.29.30130.2 |
| Windows SDK | 10.0.19041.0 |

说明：
1. 需要添加MSVC和Windows SDK到环境变量，以在VSCode编译及调试。
2. 打开main.cpp文件然后f5启用调试，获取运行任务`C/C++: cl.exe build`直接编译发行版。
3. 任务`tsc: build - typescript/tsconfig.json`用来编译ef2.ts到js文件，输出目录为dist。

---
### 开源许可
[MIT License](https://opensource.org/licenses/MIT)

项目中参考其他源码以文章：
| 项目或文章 | 开源协议 |
| ------- | ------- |
| [idm_api](http://www.internetdownloadmanager.com/support/idm_api.html) | 未知 |
| [IDMHelper](https://github.com/unamer/IDMHelper) | [GPL-3.0 License](https://github.com/unamer/IDMHelper/blob/master/LICENSE) |
| [mingw控制台中文乱码](https://www.cnblogs.com/chouxianyu/p/11249810.html) | 未知 |
| [C++实现utf8和gbk编码字符串互相转换](https://blog.csdn.net/u012234115/article/details/83186386) | 未知 |
| [base64](https://github.com/tkislan/base64) | [MIT License](https://github.com/tkislan/base64/blob/master/LICENSE) |
