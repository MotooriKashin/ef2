/**
 * 这是一个es6模块，导出负责编解码ef2自定义协议的构造函数
 */
class Base64 {
    static encode(str) {
        return btoa(encodeURIComponent(str).replace(/%([0-9A-F]{2})/g, function (match, p1) {
            return String.fromCharCode(('0x' + p1));
        }));
    }
    static decode(str) {
        return decodeURIComponent(atob(str).split('').map(function (c) {
            return '%' + ('00' + c.charCodeAt(0).toString(16)).slice(-2);
        }).join(''));
    }
}
class NativeMessaging {
    port;
    constructor(port) {
        this.port = port;
    }
    static websocket(port) {
        return new Promise((resolve, reject) => {
            const socket = new WebSocket(`ws://localhost:${port}`);
            socket.addEventListener("open", e => {
                console.log(`%c连接到ef2辅助工具成功！`, "color: green;");
                resolve(socket);
            }, { once: true });
            socket.addEventListener("error", e => {
                console.log(`%c连接到ef2辅助工具出错！`, "color: red;");
                console.error(e);
                reject(e);
            }, { once: true });
            socket.addEventListener("message", e => {
                console.log("<<", e.data);
            });
        });
    }
    static awaken(port) {
        const a = document.createElement("a");
        a.href = `ef2://websocket:${port}`;
        a.click();
    }
    init() {
        return new Promise((resolve, reject) => {
            NativeMessaging.websocket(this.port).then((ws) => {
                resolve(ws);
            }, e => {
                NativeMessaging.awaken(this.port);
                NativeMessaging.websocket(this.port).then((ws) => {
                    resolve(ws);
                }, e => {
                    reject(e);
                });
            });
        });
    }
}
// class M4SDASH {
//     dash: Partial<DASH> = {
//         videoDetails: { title: "" },
//         streamingData: {
//             adaptiveFormats: [],
//             expiresInSeconds: "21540",
//             formats: []
//         }
//     }
// }
class MsgHost {
    /**
     * 浏览器窗口序号
     */
    static wid = 1;
    /**
     * 与ef2通讯的WebSocket对象
     */
    static ws;
    /**
     * 消息序号
     */
    static index = 0;
    static ha = Function.apply.bind(String.fromCharCode, null);
    static lb = { ftp: 2, ftps: 10, http: 1, https: 9, idmreg: 12, xup: 33 };
    static Ra = false;
    static get i() {
        this.index++;
        return this.index;
    }
    constructor(ws) {
        MsgHost.ws = ws;
        MsgHost.init();
        MsgHost.windowUpdate();
        MsgHost.windowShow();
    }
    /**
     * 构造并发送消息
     * @param type 消息类型
     * @param sort 二级消息类型？
     * @param flag 未知
     * @param ext 消息附加信息？
     * @param obj 消息内容
     * @param tag 未知
     */
    static MSG(type, sort, flag, ext, obj, tag) {
        const arr = ["MSG#", this.i, "#", type, "#", sort, "#", flag];
        if (ext) {
            for (let i = 0; i < ext.length; i++)
                arr.push(":", ext[i] || 0);
        }
        if (obj) {
            for (let key in obj) {
                const value = obj[key];
                if (null != value) {
                    arr.push(",", key, "=");
                    if (value instanceof Array) {
                        let length = arr.length;
                        let b = 0;
                        for (var char of value) {
                            b = char instanceof Uint8Array ? b + (tag ? this.ob(char = this.ha(char)) : char.length) : b + this.ob(char = char.toString());
                            arr.push(char);
                        }
                        arr.splice(length, 0, b, ":");
                    }
                    else {
                        "string" == typeof value ? arr.push(this.ob(value), ":", value) : value instanceof Blob ? arr.push(value.size, ":", value) : arr.push(value);
                    }
                }
            }
        }
        arr.push(";");
        this.send(arr.join(""));
    }
    static ob(a) {
        for (var b = a.length, c = b, d = 0; d < c;) {
            var e = a.charCodeAt(d++);
            128 <= e && (2048 <= e ? (b += 2,
                e - 55296 & 56320 || d++) : b++);
        }
        return b;
    }
    static Yb(a) {
        a = a.split(":", 1).shift().toLowerCase();
        const n = this.lb[a] || 0;
        return n % 8;
    }
    static Fc(a) {
        if (!a || !a.length)
            return null;
        var b = [], c;
        for (c of a)
            b.push(c.name, ": "),
                null != c.value ? b.push(c.value) : c.binaryValue && b.push(new Uint8Array(c.binaryValue)),
                b.push("\r\n");
        return b;
    }
    static en(url) {
        const arr = url.split(".");
        return arr.length > 1 ? arr[arr.length - 1].toUpperCase() : "";
    }
    static send(msg = "") {
        msg = `"${msg}"`;
        console.log(">>", msg);
        this.ws.send(`"${msg}"`);
    }
    /**
     * 初始化连接
     */
    static init() {
        const e = navigator.userAgent;
        const la = e.match(/\bChrome\/(\d+)\.(\d+)\.(\d+)\.(\d+)\b/) ? e.match(/\bChrome\/(\d+)\.(\d+)\.(\d+)\.(\d+)\b/)[0] : "UNKNOWN/0.0";
        const b = [16, 48, this.Ra ? 1028 : 1031, 0];
        const c = {
            112: la,
            113: la,
            114: "Chrome_RenderWidgetHostHWND",
            125: undefined,
            116: "zh-CN"
        };
        this.MSG(2, 1, this.Ra ? 0 : 1024, b, c);
    }
    /**
     * 浏览器窗口刷新消息
     */
    static windowUpdate() {
        const info = [this.wid, null, window.screen.availWidth, window.screen.availHeight, -8, -8, 1];
        this.MSG(6, 2, 2312, info);
    }
    /**
     * 浏览器窗口获得焦点
     */
    static windowShow() {
        const info = [this.wid, 8, 111, window.screen.availWidth, window.screen.availHeight];
        this.MSG(7, 2, 256, info);
    }
    /**
     * 浏览器窗口关闭
     */
    static windowClose() {
        this.MSG(5, 1, 32, [this.wid]);
        this.wid++;
    }
    /**
     * 抓取链接模式
     * @param obj 下载配置数据
     */
    static capture(obj) {
        obj.id || (obj.id = Math.floor(Math.random() * 1e3));
        obj.portId || (obj.portId = 2);
        obj.method || (obj.method = "GET");
        obj.statusCode || (obj.statusCode = 200);
        obj.statusLine || (obj.statusLine = "HTTP/1.1 200");
        obj.extension || (obj.extension = this.en(obj.url));
        let g = obj.url, l = this.Yb(g), k = 1 == (l & 7), m = obj.requestHeaders, z = obj.id, q = obj.portId, r = new Date().getTime(), u = 186, A = 16, B = obj.responseHeaders;
        let v = obj.ip, c = 2048;
        3 == (v ? v.length - v.replaceAll(".", "").length : 0) ? A |= 33554432 : 2 < (v ? v.length - v.replaceAll(":", "").length : 0) && (A |= 4194304);
        "POST" == obj.method && (A |= 512);
        const w = [z, q, r.toFixed(), obj.statusCode, l, 2, 21, 0];
        const p = {};
        w[2] = Math.floor(r / 1E3);
        w[3] = Math.floor(r % 1E3 * 1E3);
        p[6] = g;
        p[12] = obj.statusLine; // 17 100
        p[4] = obj.extension;
        p[8] = A;
        p[129] = u;
        p[122] = "4";
        p[11] = this.Fc(m);
        p[13] = this.Fc(B);
        p[7] = obj.origin;
        p[54] = obj.userAgent || navigator.userAgent;
        p[100] = obj.name && (obj.name + (obj.extension ? "." + obj.extension : ""));
        this.MSG(13, 1, c, w, p);
    }
    /**
     * 直接下载模式
     * @param obj 下载配置数据
     */
    static download(obj) {
        let x = this.Yb(obj.url);
        if (!x)
            return 0;
        const h = [x];
        const k = {};
        k[6] = obj.url; // 100文件名？ 111cookies 112链接文字描述？
        k[50] = obj.referer;
        k[7] = obj.origin;
        k[8] = 4; // 100 102
        k[51] = "";
        k[54] = obj.userAgent || navigator.userAgent;
        k[121] = "4";
        this.MSG(14, 1, 0, h, k);
    }
    /**
     * 批量下载模式
     * @param obj 下载配置数据
     */
    static downloadAll(obj) {
        const d = {
            7: obj.origin,
            8: 4,
            54: obj.userAgent || navigator.userAgent,
            110: obj.window,
            111: "",
            121: "4"
        };
        this.MSG(15, 1, 0, [obj.urls.length], d);
        obj.urls.forEach(d => {
            const m = {
                6: d.url,
                50: d.referer,
                51: "",
                100: null,
                102: d.name
            };
            this.MSG(14, 1, 16, [0], m);
        });
    }
    /**
     * 捕获下载模式
     * @param obj 下载配置数据
     */
    static intercept(obj) {
        const xup = "xup://" + Math.random().toString(36).substring(2);
        const k = this.Yb(xup);
        if (!k)
            return 0;
        const h = [k];
        const b = {};
        b[6] = "idm0x00000002" + xup.slice(3);
        b[54] = obj.userAgent || navigator.userAgent;
        b[121] = "4";
        this.MSG(14, 1, 0, h, b);
        this.capture(obj);
    }
    /**
     * DASH解析模式
     * @param obj json配置
     * @param dash 实际dash数据
     */
    static DASH(obj, dash) {
        obj.id || (obj.id = Math.floor(Math.random() * 1e3));
        this.capture(obj);
        this.MSG(16, 1, 48, [obj.id, 0], JSON.stringify(dash));
        this.MSG(21, 1, 0, [obj.id, 21, 20, 1, 56], {
            110: obj.name,
            128: obj.port || 80
        });
    }
    /**
     * 直接下载
     * @param obj 下载数据
     */
    download(obj) {
        MsgHost.download(obj);
    }
    /**
     * 批量下载
     * @param obj 下载数据
     */
    downloadAll(obj) {
        MsgHost.downloadAll(obj);
    }
    /**
     * 自定义请求头下载
     * @param obj 下载数据
     */
    downloadWithCustomHeaders(obj) {
        MsgHost.intercept(obj);
    }
    /**
     * 直接发送消息给IDM
     * @param msg 消息内容，可以直接发送ef2协议
     */
    send(msg) {
        const str = msg.startsWith("MSG") ? `"${msg}"` : msg;
        return MsgHost.ws.send(str);
    }
    /**
     * 关闭连接并退出ef2辅助工具
     */
    close() {
        MsgHost.windowClose();
        MsgHost.ws.send("fin");
        MsgHost.ws.close();
    }
}
class Ef2 {
    /**
     * 编码下载数据为ef2协议链接，用法同http协议链接
     * @param data 下载配置对象
     * @returns ef2协议链接
     */
    encode(data) {
        let result = "";
        Object.keys(data).forEach((d) => {
            switch (d) {
                case "cookies":
                    result += ` -c "${data.cookies}"`;
                    break;
                case "directory":
                    data.directory = data.directory.replace(/\//, "\\"); // 目录反斜杠可能误输入为了正斜杠
                    data.directory && data.directory[data.directory.length - 1] == "\\" && (data.directory = data.directory.substr(0, data.directory.length - 1)); // 目录最后的反斜杠可能导致引号被转义 
                    result += ` -o "${data.directory}"`;
                    break;
                case "out":
                    result += ` -s "${data.out}"`;
                    break;
                case "password":
                    result += ` -P "${data.password}"`;
                    break;
                case "postDate":
                    result += ` -d "${data.postDate}"`;
                    break;
                case "referer":
                    result += ` -r "${data.referer}"`;
                    break;
                case "sendToList":
                    result += ` -q`;
                    break;
                case "toastDisabled":
                    result += ` -f`;
                    break;
                case "url":
                    data.url.startsWith("//") && (data.url = "https:" + data.url); // 省略协议头时默认添加http/tls头
                    result += ` -u "${data.url}"`;
                    break;
                case "userAgent":
                    result += ` -a "${data.userAgent}"`;
                    break;
                case "userName":
                    result += ` -U "${data.userName}"`;
                    break;
            }
        });
        result && result.startsWith(" ") && (result = result.substr(1, result.length));
        return "ef2://" + Base64.encode(result);
    }
    /**
     * 解码ef2协议为对象
     * @param ef2ptl ef2协议链接
     * @returns 下载配置对象
     */
    decode(ef2ptl) {
        ef2ptl = ef2ptl.replace("ef2://", "");
        ef2ptl = Base64.decode(ef2ptl) + " ";
        const key = ef2ptl.match(/-\w /g);
        const value = ef2ptl.split(/-\w /);
        value.shift();
        return Array.from(key).reduce((s, d, i) => {
            value[i] && value[i].endsWith(" ") && (value[i] = value[i].substr(0, value[i].length - 1));
            value[i] && value[i].endsWith("\"") && (value[i] = value[i].substr(1, value[i].length - 2));
            switch (d) {
                case "-c ":
                    s.cookies = value[i];
                    break;
                case "-o ":
                    s.directory = value[i];
                    break;
                case "-s ":
                    s.out = value[i];
                    break;
                case "-P ":
                    s.password = value[i];
                    break;
                case "-d ":
                    s.postDate = value[i];
                    break;
                case "-r ":
                    s.referer = value[i];
                    break;
                case "-q ":
                    s.sendToList = true;
                    break;
                case "-f ":
                    s.toastDisabled = true;
                    break;
                case "-u ":
                    s.url = value[i];
                    break;
                case "-a ":
                    s.userAgent = value[i];
                    break;
                case "-U ":
                    s.userName = value[i];
                    break;
            }
            return s;
        }, {});
    }
    /**
     * encode的封装，直接发起下载
     * 未安装ef2.exe将没有任何效果
     * @param data 下载配置对象
     */
    sendLinkToIDM(data) {
        const a = document.createElement("a");
        a.href = this.encode(data);
        a.click();
    }
    /**
     * 启动nativeMessaging模式并建立连接
     * @param port websocket端口号
     * @returns nativeMessaging模式提供的方法
     */
    async nativeMessaging(port = 9186) {
        const ws = await new NativeMessaging(port).init();
        return new MsgHost(ws);
    }
}
export default new Ef2();
// interface M4S {
//     video: {
//         url: string,
//         quality: string,
//         mimeType: string,
//         bitrate: number
//     };
//     audio: {
//         url: string,
//         mimeType: string,
//         bitrate: number
//     },
//     title: string
// }
