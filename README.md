这是[IDM（Internet Download Manager）](http://www.internetdownloadmanager.com/) 辅助下载工具，用以配合用户脚本使用自定义的`ef2://`链接发送下载数据到IDM，主要用来解决用户脚本无法主动调用IDM且无法自定义`user-agent`、`referer`、“下载目录”、“文件名”等信息的困扰。  
这里自定义了一种类似`http`的名为`ef2`的协议，通过Windows协议关联功能关联到`ef2.exe`，并借助IDM官方COM组件进一步将数据传递到IDM，从而实现了自定义下载数据的功能。  
同时支持关联IDM官方的导出文件（.ef2），可以直接双击打开解析并发送给 IDM。
**一切的前提是已安装[IDM（Internet Download Manager）](http://www.internetdownloadmanager.com/)下载工具**  

---
### 如何使用

#### 安装
绿色命令行程序，无需安装，只是要写入注册表关联
1. 下载所有文件到任意目录（其实只需要`ef2.exe`、`IDMHelper.exe`、`setup.bat`和`uninstall.bat`四个文件在同一目录下，目录最好不要有空格或中文！）
2. 运行`setup.bat`，为`ef2.exe`注册名为`ef2`的链接关联（操作注册表会弹出管理员权限申请，请允许！）
3. 复制下文中的完整`ef2`链接，在浏览器中“粘贴并转到”，如果弹出请求使用外部程序打开的对话框就注册完成了，否则请重试第2步。

#### 卸载
1. 运行`uninstall.bat`，移除名为`ef2`的链接的注册信息（操作注册表会弹出管理员权限申请，请允许！）  
2. 删除整个目录即可，如果不放心可以再在浏览器中点击上面的链接试试，应该已经不会弹出打开外部程序的申请了，否则请重复第1步。

*上述注册/注销操作会在同目录下留下`1.reg`文件，可自行查看，绝对没有写入任何有害信息！也应该没有影响windows稳定的操作。*

---
### 如何使用
使用方式有二：
   1. 双击 IDM导出文件（.ef2）将解析其中的下载链接发送给 IDM。
      - 若`.ef2`文件中只有一条链接，将弹出 IDM 下载对话框。
      - 若不止一条下载链接，将发送所有链接到 IDM 下载队列，需要手动到 IDM 里“开始队列”。
   2. 点击浏览器中的`ef2`自定义协议链接，拉起 IDM 下载。

那网页中怎么才会有`ef2`下载链接？  
正常情况下是不会有的，所以得有用户脚本生成这种链接（比如[Bilibili 旧播放页](https://github.com/MotooriKashin/Bilibili-Old/)在设置中启用“ef2辅助IDM下载”选项）  

下面给出`ef2`链接在页面中如何使用：    
众所周知，HTML的`a`标签是用于超链接跳转，如：
```
<a href="http://www.bilibili.com">IDM</a>
```
其中`href`属性就是点击该链接是跳转去向。  
只须`href`属性中的`http(s)`修改为`ef2`链接的形式，如：
```
<a href="ef2://aHR0cDovL3d3dy5iaWxpYmlsaS5jb20">IDM</a>
```
就可以了，怎么样？简单吧！  
如果之前已成功注册了`ef2.exe`关联，那么此时点击这个`a`标签就可以直接将`ef2`链接传递出去了（可能浏览器会弹窗请求是否打开外部程序）。  
如果这条`ef2`链接是有效的话，IDM就会立即弹出下载对话框。  

所以——怎么构造这样一条有效的`ef2`链接呢？  
base64？那是最后一步，更重要的是数据内容。

---
### 协议构成
**`ef2`协议就是一串以`ef2://`开头的base64编码的字符串，字符串的内容就是要传递给[IDMHelper.exe](https://github.com/unamer/IDMHelper)的参数**  

具体[IDMHelper.exe](https://github.com/unamer/IDMHelper)支持哪些参数可以去对应页面查看，或者看下面的翻译摘要：
   - -u：下载链接（URL）
   - -a：user-agent
   - -c：cookies
   - -d：POST数据（改用post方法请求）
   - -r：referer
   - -U：账户名称（如果服务器需要验证身份——一般都使用cookies替代了）
   - -P：账户密钥（配合账户名称使用）
   - -o：文件保存的本地目录（Windows的反斜杠形式，由于反斜杠也是 JavaScript 的转义符，js中请使用双反斜杠输入！）
   - -s：自定义保存的文件名（包括拓展名）
   - -f：禁用 IDM 对话框（JavaScript 中请赋值`true`或任何其他真值）
   - -q：添加到下载队列而不下载（JavaScript 中请赋值`true`或任何其他真值）

那么问题来了，对于下面这样一个视频：
   - 链接：http://112.48.167.3/upgcxcode/64/98/304509864/304509864_nb2-1-30080.m4s
   - 限制UA：Bilibili Freedoooooom/MarkII
   - 限制referer：https://www.bilibili.com
   - 保存目录：D:\下载\视频
   - 文件名：某某视频.mp4

对应的参数应该是——下面这样！
```
idmhelper.exe -u http://112.48.167.3/upgcxcode/64/98/304509864/304509864_nb2-1-30080.m4s -a "Bilibili Freedoooooom/MarkII" -r https://www.bilibili.com -o D:\下载\视频 -s 某某视频.mp4
```
现在我们去掉`IDMHelper.exe`及`-u`前面的空格。  
再把剩下的字符串用base64编码一下就变成了：
```
LXUgaHR0cDovLzExMi40OC4xNjcuMy91cGdjeGNvZGUvNjQvOTgvMzA0NTA5ODY0LzMwNDUwOTg2NF9uYjItMS0zMDA4MC5tNHMgLWEgIkJpbGliaWxpIEZyZWVkb29vb29vbS9NYXJrSUkiIC1yIGh0dHBzOi8vd3d3LmJpbGliaWxpLmNvbSAtbyBEOlzkuIvovb1c6KeG6aKRIC1zIOafkOafkOinhumikS5tcDQ
```
别忘了还要加上`ef2://`协议头：
```
ef2://LXUgaHR0cDovLzExMi40OC4xNjcuMy91cGdjeGNvZGUvNjQvOTgvMzA0NTA5ODY0LzMwNDUwOTg2NF9uYjItMS0zMDA4MC5tNHMgLWEgIkJpbGliaWxpIEZyZWVkb29vb29vbS9NYXJrSUkiIC1yIGh0dHBzOi8vd3d3LmJpbGliaWxpLmNvbSAtbyBEOlzkuIvovb1c6KeG6aKRIC1zIOafkOafkOinhumikS5tcDQ
```
最后塞进`a`便签里：
```
<a href="ef2://LXUgaHR0cDovLzExMi40OC4xNjcuMy91cGdjeGNvZGUvNjQvOTgvMzA0NTA5ODY0LzMwNDUwOTg2NF9uYjItMS0zMDA4MC5tNHMgLWEgIkJpbGliaWxpIEZyZWVkb29vb29vbS9NYXJrSUkiIC1yIGh0dHBzOi8vd3d3LmJpbGliaWxpLmNvbSAtbyBEOlzkuIvovb1c6KeG6aKRIC1zIOafkOafkOinhumikS5tcDQ">点击下载</a>
```
以上。  
一般来说参数只需要`-u`也就是下载链接就够了，其他参数都是可选的。不过那样直接在浏览器中右键IDM下载就是了，本项目也就没有存在的必要了。  
*不过你要要是直接在http链接前加上`ef2://`也不经过base64加密直接充当ef2链接没什么大问题就是了，只不过缺少referer、user-agent等数据IDM可能下载失败就是了*  
但有时候或者说更多时候我们想传递给IDM的不仅是一条链接！
   - 我们必须设置referer：否则IDM下载会被服务器拦截-403
   - 我们必须设置user-agent：否则一样-403

以上两条就是B站视频下载链接直接复制到IDM里无法下载的原因。（右键下载因为会传递这两项所以没关系）  
又或者视频格式不在IDM捕获列表内（说的就是你`.m4s`）：
   - 我想左键点击而不是右键去调用IDM！
  
也有可能：
   - 我还想自定义下载目录，而不是弹出对话框去选。
   - 我还想重命名文件，而不是IDM自己识别的一串数字或者一串乱码！  

千言万语
   - **我最想的是IDM官方提供从HTML页面配置下载数据的功能，而不是只会被动地捕获！**  
   - 为什么COM接口还是2003年的文档？
   - 为什么 IDM 自己不支持双击代开`.ef2`文件！

---
### 源码相关
初学c++，基于“搜索引擎”编程。代码粗劣，不看入目，如有建议，欢迎指点！  
入口在`ef2.cpp`内，没有任何配置要求，使用vscode生成`ef2.exe`。      
`base64.h`库及`IDMHelper.exe`及其他代码来源见“参考致谢”。  
其实`IDMHelper.exe`功能本想整合进去并添加批量下载的，无奈[IDM官方给的工具](http://www.internetdownloadmanager.com/support/idm_api.html)不知道如何在vscode下使用……

---
### 参考致谢
- [unamer](https://github.com/unamer/IDMHelper)：调用IDM的核心二进制文件
- [臭咸鱼](https://www.cnblogs.com/chouxianyu/p/11249810.html)：vscode编译c++中文乱码问题
- [踏莎行hyx](https://blog.csdn.net/u012234115/article/details/83186386)：utf-8转gbk的c++代码
- [tkislan](https://github.com/tkislan/base64)：base64的c++库  
- [IDM](http://www.internetdownloadmanager.com/support/idm_api.html)：IDM官方 COM 组件API