[Internet Download Manager (IDM)](http://www.internetdownloadmanager.com/)下载辅助工具：
   1. 支持在Windows资源管理器双击打开IDM导出文件(拓展名`.ef2`)
   2. 支持在使用自定义协议在浏览器中拉起IDM(协议头为`ef2://`)

拓展了IDM能配置的内容，允许自定义`referer`、`User-Agent`、`保存的文件名`及`保存目录`等数据

---
### 安装/卸载
本工具为绿色软件，作为与IDM通讯的载体，您只需要下载[ef2.exe](https://github.com/MotooriKashin/ef2/releases/latest)程序本体后运行并点击“安装”按钮即可。  
之后`ef2.exe`便成为了拉起IDM的“桥梁”一样的东西，在您需要主动拉起IDM时发挥作用。  
如需卸载，请再次运行`ef2.exe`并点击“卸载”按钮，最后删除`ef2.exe`文件即可，保证干净无残留。

安装完成后，您可以有两种方式主动拉起IDM
   1. 在Windows资源管理器双击IDM导出文件(拓展名`.ef2`)
      - 这种导出文件是IDM本身所定义的，一般来自在IDM软件中“任务” -> “导出” -> “导出为"IDM 导出文件"”的操作
      - 本质上是一个纯文本文档，所以我们可以通过记事本等工具自行生成，一些油猴脚本也正是通过这种文件保存批量下载数据到本地然后交给IDM
      - 本工具关联了此类文件，让您双击打开即可以拉起IDM下载，免去“任务” -> “导入” -> “从"IDM 导出文件"导入”的繁琐操作
      - 该文件原本只支持三项：URL、referer和User-Agent，本工具拓展了其支持的内容，详见后文.ef2文件拓展说明
   2. 在浏览器中点击`ef2`协议的链接或将`.ef2`协议的链接粘贴进浏览器的地址栏并回车
      - `ef2`协议是一种不存在的协议，是本工具自定义的一种协议，基本等同于https等协议，只不过是以`ef2://`开头
      - 如果您不需要配置其他内容，那么把下载链接中的`https://`等直接替换为`ef2://`用来拉起IDM是可行的
      - 如果您需要配置URL之外的内容，那么请参看后文自定义ef2协议的说明

---

### 相关说明
####  .ef2文件拓展
ef2文件本质是一个纯文本文件，将其用记事本打开很容易发现其中的规律：
<details>
  <summary>文件示例</summary>
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
  <details>
  <summary>规律说明</summary>
  <ul>
    <li>一对英文尖括号中就是一条下载数据，有几对尖括号就有几条下载数据</li>
    <li>尖括号中第一行就是下载URL，如果有URL之外的内容，换行填写在URL下面</li>
    <li>URL之外的内容是以“内容名称” + “英文冒号” + “英文空格” + “内容数据”的格式</li>
  </ul>
  </details>
  <details>
  <summary>拓展内容</summary>
  <ul>
    <li>除了示例中的referer和User-Agent，本工具添加的内容如下<table border="1">
    <tr>
        <td>cookies</td>
        <td>如果下载需要用到cookies</td>
    </tr>
    <tr>
        <td>postdata</td>
        <td>改用POST方式请求时要提交的数据</td>
    </tr>
    <tr>
        <td>username/password</td>
        <td>这两个搭配使用与HTTP认证<del>从来没用过的说</del></td>
    </tr>
    <tr>
        <td>filepath</td>
        <td>保存路径，双反斜杠形式如<pre>F:\\下载</pre></td>
    </tr>
    <tr>
        <td>filename</td>
        <td>文件名，包含拓展名如<pre>视频.mp4</pre></td>
    </tr>
</table></li>
    <li>尖括号中第一行就是下载URL，如果有URL之外的内容，换行填写在URL下面</li>
    <li>URL之外的内容是以“内容名称” + “英文冒号” + “英文空格” + “内容数据”的格式</li>
  </ul>
  </details>
  <details>
  <summary>拓展示例</summary>
  <div>
    <span>就在上文示例的中添加一个保存目录和文件名作为示例吧</span>
    <pre>
<
https://img2.example.com/data/1102/94/IMG_0001.jpg
referer: https://img2.example.com/data/1102/94/
User-Agent: Mozilla/5.0 (Windows NT 6.1; Trident/7.0; rv:11.0) like Gecko
filepath: F:\\下载
filename: 视频.mp4
>
    </pre>
  </div>
  </details>
  </details>
</details>

---

#### ef2协议
ef2协议是形如
```
ef2://LXUgaHR0cHM6Ly9jYW4uYml1Lm1vZS92MS80LzEyODQ/ZT0xNjE4OTc0MjMwJnRrPTM2MDMzYTA3MDA3MzU5MmJiMTYxMjBmOTc4ODljZjk5Jm1vZWhhc2g9MzcyMzQ3MCAtYSBHby1odHRwLWNsaWVudC8xLjEgLXMgIua4hea1puWkj+WunyAtIOaXheOBrumAlOS4rS5mbGFjIg==
```
的链接  
该链接本质上是`ef2://`头 + 一串Base64字符串，您可以这样使用：
   - 粘贴进浏览器地址栏并回车
   - 写进a标签的href属性里点击该链接
   - js中使用`window.open()`等方法直接打开该链接

<details>
   <summary>那么该如何生成这样的链接呢？</summary>
   <div>
     <span>生成分为三步</span>
     <ol>
      <li>把配置数据以一定格式排列成字符串</li>
      <li>把组合成的字符串进行Base64编码</li>
      <li>给生成的Base64编码添加上 ef2:// 头</li>
     </ol>
   <div>
   <details>
   <summary>所谓的一定格式是？</summary>
   <ul>
     <li>“减号” + “标记” + “英文空格” + “内容(如果存在空格等非法字符请用英文双引号括起来)” + “下一条数据”</li>
     <li>可使用的标记如下，与配置数据内容一一对应<table border="1">
    <tr>
        <th>标记</th>
        <th>内容</th>
        <th>示例</th>
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
      <td>如果需要cookies<pre>-a cookies字符串</pre></td>
    </tr>
    <tr>
      <td>d</td>
      <td>postdata</td>
      <td>如果改用POST方式提交的数据<pre>-d postdata字符串</pre></td>
    </tr>
    <tr>
      <td>U</td>
      <td>username</td>
      <td>与password配对<del>从没用过的说</del><pre>-U username字符串</pre></td>
    </tr>
    <tr>
      <td>P</td>
      <td>password</td>
      <td>与username配对<del>从没用过的说</del><pre>-P password字符串</pre></td>
    </tr>
    <tr>
      <td>o</td>
      <td>保存路径</td>
      <td>注意反斜杠！<pre>-o F:\下载</pre></td>
    </tr>
    <tr>
        <td>s</td>
        <td>文件名</td>
        <td>注意拓展名！<pre>-s 视频.mp4</pre></td>
    </tr>
    <tr>
        <td>f</td>
        <td>特殊标记</td>
        <td>禁用IDM下载对话框，只有标记没有内容！<pre>-f</pre></td>
    </tr>
    <tr>
        <td>q</td>
        <td>特殊标记</td>
        <td>添加到IDM队列而不是立即下载，只有标记没有内容！<pre>-q</pre></td>
    </tr>
</table></li>
     <li>以方才拓展ef2文件的那条数据为例转成的格式是这样<pre>-u https://img2.example.com/data/1102/94/IMG_0001.jpg -r https://img2.example.com/data/1102/94/ -a "Mozilla/5.0 (Windows NT 6.1; Trident/7.0; rv:11.0) like Gecko" -o F:\下载 -s 视频.mp4</pre></li>
     <li>还可以添加上特殊标记“禁用IDM下载对话框”变成这样<pre>-u https://img2.example.com/data/1102/94/IMG_0001.jpg -r https://img2.example.com/data/1102/94/ -a "Mozilla/5.0 (Windows NT 6.1; Trident/7.0; rv:11.0) like Gecko" -o F:\下载 -s 视频.mp4 -f</pre></li>
   </ul>
   </details>
   <details>
  <summary>如何在js中引入ef2.js模块生成ef2协议？</summary>
  <ol>
    <li>本仓库的`ef2.js`是一个标准的es6模块，你可以使用标准的es6模块载入方式载入进你的项目，比如<pre>import("//cdn.jsdelivr.net/gh/MotooriKashin/ef2/ef2.js") // 直接使用jsdelivr从本仓库载入模块</pre></li>
    <li>载入成功后顶层window对象上将挂载一个ef2对象，该对象下的encode方法接受一个对象并以Promise形式返回ef2协议<pre>
    // 仍用上文数据作为例子
    window.ef2.encode({
       u: "https://img2.example.com/data/1102/94/IMG_0001.jpg",
       r: "https://img2.example.com/data/1102/94/",
       a: "Mozilla/5.0 (Windows NT 6.1; Trident/7.0; rv:11.0) like Gecko",
       o: "F:\\下载", // js中反斜杠需要转义，所以也是双反斜杠
       s: "视频.mp4",
       f: true // 任意真值均可
    }).then(d => {
       // d 即为转化后的ef2协议链接，包括`ef2://`头
    })
    </pre></li>
  </ol>
  </details>
</details>

---
### 编译相关
环境：
> Windows 8  
> VSCode 1.55.0  
> C/C++ Extension 1.3.0  
> MSVC 2019

1. 把`msvc`需要配置Windows环境变量
   - path：编译器`cl.exe`所在目录，`Windows Kits`资源编译器`rc.exe`所在目录。
   - include：`VC++`库所在目录，`Windows Kits`中`um`、`shared`、`ucrt`以及`atlmfc`所在目录。
   - lib：`VC++`lib库所在目录，`Windows Kits`中`ucrt`、`atlmfc`lib库所在目录，`Microsoft SDKs`lib库所在目录。
2. 调整`VSCode`设置以使用`MSVC`作为C/C++编译器
   - `C_Cpp.default.intelliSenseMode` 推荐设为`windows-msvc-x64`
   - `C_Cpp.default.compilerPath`     需要精确到`cl.exe`的绝对路径
3. 克隆本项目到本地然后在`VSCode`中打开
4. `idmantypeinfo.tlh`和`idmantypeinfo.tli`两个包含本地信息的项目文件会在第一次编译后自动生成
5. 编译命令`launch.json`和`tasks.json`已配置好，编译任务分为4步
   1. 编译RC资源
   2. 编译代码文件而不链接
   3. 链接生成二进制文件
   4. 清理中间产物
6. 编译任务使用的参数是release版，如需debug请自行调整任务2的参数，代码中管理员提权部分可能会影响debug可临时注释掉

---
### 开放源码
本项目以 MIT 许可开放所有源代码，IDM提供的源码部分请参考[Internet Download Manager COM based Application Programming Interface (API)](http://www.internetdownloadmanager.com/support/idm_api.html)。

---
### 参考致谢
- [unamer](https://github.com/unamer/IDMHelper)：IDM的编程接口使用示例，本项目1.0之前直接使用该项目提供的二进制文件负责拉起IDM。
- [臭咸鱼](https://www.cnblogs.com/chouxianyu/p/11249810.html)：vscode编译c++中文乱码问题。
- [踏莎行hyx](https://blog.csdn.net/u012234115/article/details/83186386)：utf-8转gbk的c++代码。
- [tkislan](https://github.com/tkislan/base64)：base64的c++库。  
- [IDM](http://www.internetdownloadmanager.com/support/idm_api.html)：IDM官方 COM 组件API。
- [js-base64](https://github.com/dankogai/js-base64)：js-base64库。