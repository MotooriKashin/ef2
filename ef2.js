/**
 * @module ef2
 * @description 将下载链接转化为ef2协议链接，详见底部模块导出部分
 * @author Motoori Kashin
 * @license MIT
 */

const root = window || self;
/**
 * Ef2 class
 * @class Ef2
 * @description ef2编解码模块
 */
class Ef2 {
    /**
     * 引入Base64编解码库，以支持unicode编码
     * @see js-base64 {@link https://github.com/dankogai/js-base64}
     * @returns {function} Base64编解码库，同时挂载为this.Base64
     */
    static async initBase64() {
        await import("https://cdn.jsdelivr.net/npm/js-base64@3.6.0/base64.js");
        if (!root.Base64)
            throw "载入Base64模块失败！";
        this.Base64 = root.Base64;
        return this.Base64
    }
    /**
     * 转化为ef2协议
     * @param {object} obj ef2协议配置对象
     * @param {string} obj.u URL，如：http://www.example.com/example.mp4
     * @param {string} [obj.a] user-agent，如：Bilibili Freedoooooom/MarkII
     * @param {string} [obj.c] cookies
     * @param {string} [obj.d] data(POST方法)
     * @param {string} [obj.r] referer，如：http://www.example.com/
     * @param {string} [obj.U] 账户名称
     * @param {string} [obj.P] 账户密钥
     * @param {string} [obj.o] 下载目录，反斜杠式文件名，以字符串输入时请注意转义反斜杠，如：F:\下载\IDM
     * @param {string} [obj.s] 文件名，包含拓展名，如：example.mp4
     * @param {true} [obj.f] 静默下载，不再弹出IDM下载对话框
     * @param {true} [obj.q] 稍后下载，添加队列而不立即下载，需到IDM点击开始队列
     * @returns {Promise<string>} ef2协议字符串
     * @example
     * // 实例配置URL外user-agent、referer、下载目录和文件名，并设置稍后下载
     * Ef2.btoa({u: "http://www.example.com/example.mp4", a: "Bilibili Freedoooooom/MarkII", r: "http://www.example.com/", o: "F:\\下载\\IDM", s: "example.mp4", q: true });
     * // 返回值：ef2://LXUgaHR0cDovL3d3dy5leGFtcGxlLmNvbS9leGFtcGxlLm1wNCAtYSAiQmlsaWJpbGkgRnJlZWRvb29vb29tL01hcmtJSSIgLXIgaHR0cDovL3d3dy5leGFtcGxlLmNvbS8gLW8gRjpc5LiL6L29XElETSAtcyBleGFtcGxlLm1wNCAtcQ==
     */
    static async btoa(obj) {
        this.data = "";
        for (let key in obj) {
            if (obj[key]) {
                if (typeof (obj[key]) == 'string') {
                    // 处理路径中潜在的空格
                    if (obj[key].includes(" ") && !obj[key].includes("\""))
                        obj[key] = "\"" + obj[key] + "\"";
                    // 处理保存目录可能错将反斜杠写成斜杠的情况
                    if (obj.o && obj.o.includes("/"))
                        obj.o = obj.o.replace(/\//g, "\\");
                    // 处理保存目录时最后一级目录可能带了反斜杠将双引号转义了的情况
                    if (obj.o && obj.o[obj.o.length - 1] == "\"" && obj.o[obj.o.length - 2] == "\\")
                        obj.o = obj.o.substr(0, obj.length - 2) + "\"";
                    // 处理以双斜杠开头的链接（IDM 需要协议头要么完整要么干脆不带）
                    if (obj.u && obj.u.startsWith("//"))
                        obj.u = "https:" + obj.u;
                }
                switch (key) {
                    case "u":
                        this.data = this.data + "-u " + obj[key] + " "; // 下载链接（URL）

                        break;
                    case "a":
                        this.data = this.data + "-a " + obj[key] + " "; // user-agent

                        break;
                    case "c":
                        this.data = this.data + "-c " + obj[key] + " "; // cookies

                        break;
                    case "d":
                        this.data = this.data + "-d " + obj[key] + " "; // post 数据（如果使用 POST 方法）

                        break;
                    case "r":
                        this.data = this.data + "-r " + obj[key] + " "; // referer

                        break;
                    case "U":
                        this.data = this.data + "-U " + obj[key] + " "; // 账户名称（服务器鉴权——基本不可能用到）

                        break;
                    case "P":
                        this.data = this.data + "-P " + obj[key] + " "; // 账户密钥（服务器鉴权——基本不可能用到）

                        break;
                    case "o":
                        this.data = this.data + "-o " + obj[key] + " "; // 保存目录（由于反斜杠也是 JavaScript 的转义符，请使用双反斜杠输入！）

                        break;
                    case "s":
                        this.data = this.data + "-s " + obj[key] + " "; // 文件名（包括推展名）

                        break;
                    case "f":
                        this.data = this.data + "-f "; // 禁用 IDM 对话框，直接后台下载（键值请使用 true 或任何 js 认为的真值）

                        break;
                    case "q":
                        this.data = this.data + "-q "; // 添加到队列而不立即下载（键值请使用 true 或任何 js 认为的真值）

                        break;

                    default:
                        break;
                }
            }
        }
        if (this.data && this.data.endsWith(" "))
            this.data = this.data.substr(0, this.data.length - 1);
        try {
            return "ef2://" + root.btoa(this.data);
        } catch (e) {
            if (!this.Base64)
                await this.initBase64();
            return "ef2://" + this.Base64.encode(this.data);
        }
    }
    /**
     * 分解ef2协议为对象
     * @param {string} str ef2协议字符串
     * @returns {Promise<object>} ef2协议配置对象
     * @example
     * Ef2.atob("ef2://LXUgaHR0cDovL3d3dy5leGFtcGxlLmNvbS9leGFtcGxlLm1wNCAtYSAiQmlsaWJpbGkgRnJlZWRvb29vb29tL01hcmtJSSIgLXIgaHR0cDovL3d3dy5leGFtcGxlLmNvbS8gLW8gRjpc5LiL6L29XElETSAtcyBleGFtcGxlLm1wNCAtcQ==");
     * // 返回值：{u: "http://www.example.com/example.mp4", a: "Bilibili Freedoooooom/MarkII", r: "http://www.example.com/", o: "F:\\下载\\IDM", s: "example.mp4", q: true }
     */
    static async atob(str) {
        this.arr = [];
        this.obj = {};
        str = str.replace("ef2://", "");
        try {
            str = root.atob.decode(str);
        } catch (e) {
            if (!this.Base64)
                await this.initBase64();
            str = this.Base64.decode(str);
        }
        if (!str.startsWith(" "))
            str = " " + str;
        this.arr = str.split(" -");
        this.arr.forEach(d => {
            if (d && d.endsWith(" "))
                d = d.substr(0, d.length - 1);
            if (d[0])
                this.obj[d.substr(0, 1)] = d[2] ? d.substr(2, d.length - 2) : true;
        });
        return this.obj;
    }
    /**
     * 转化为ef2协议
     * @param {object} obj ef2协议配置对象
     * @param {string} obj.u URL，如：http://www.example.com/example.mp4
     * @param {string} [obj.a] user-agent，如：Bilibili Freedoooooom/MarkII
     * @param {string} [obj.c] cookies
     * @param {string} [obj.d] data(POST方法)
     * @param {string} [obj.r] referer，如：http://www.example.com/
     * @param {string} [obj.U] 账户名称
     * @param {string} [obj.P] 账户密钥
     * @param {string} [obj.o] 下载目录，反斜杠式文件名，如：F:\下载\IDM
     * @param {string} [obj.s] 文件名，包含拓展名，如：example.mp4
     * @param {true} [obj.f] 静默下载，不再弹出IDM下载对话框
     * @param {true} [obj.q] 稍后下载，添加队列而不立即下载，需到IDM点击开始队列
     * @returns {Promise<string>} ef2协议字符串
     * @example
     * // 实例配置URL外user-agent、referer、下载目录和文件名，并设置稍后下载
     * let ef2 = new Ef2();
     * ef2.encode({u: "http://www.example.com/example.mp4", a: "Bilibili Freedoooooom/MarkII", r: "http://www.example.com/", o: "F:\\下载\\IDM", s: "example.mp4", q: true });
     * // 返回值：ef2://LXUgaHR0cDovL3d3dy5leGFtcGxlLmNvbS9leGFtcGxlLm1wNCAtYSAiQmlsaWJpbGkgRnJlZWRvb29vb29tL01hcmtJSSIgLXIgaHR0cDovL3d3dy5leGFtcGxlLmNvbS8gLW8gRjpc5LiL6L29XElETSAtcyBleGFtcGxlLm1wNCAtcQ==
     */
    encode(obj) {
        return Ef2.btoa(obj);
    }
    /**
     * 分解ef2协议为对象
     * @param {string} str ef2协议字符串
     * @returns {Promise<object>} ef2协议配置对象
     * @example
     * let ef2 = new Ef2();
     * ef2.decode("ef2://LXUgaHR0cDovL3d3dy5leGFtcGxlLmNvbS9leGFtcGxlLm1wNCAtYSAiQmlsaWJpbGkgRnJlZWRvb29vb29tL01hcmtJSSIgLXIgaHR0cDovL3d3dy5leGFtcGxlLmNvbS8gLW8gRjpc5LiL6L29XElETSAtcyBleGFtcGxlLm1wNCAtcQ==");
     * // 返回值：{u: "http://www.example.com/example.mp4", a: "Bilibili Freedoooooom/MarkII", r: "http://www.example.com/", o: "F:\\下载\\IDM", s: "example.mp4", q: true }
     */
    decode(str) {
        return Ef2.atob(str);
    }
}

/**
 * 导出实例化的ef2对象到页面上下文中，同时以es6默认方法导出
 * 所以有两种方法使用该模块下的方法
 * 方法一：window.ef2
 * 方法二：使用es6标准模块导入方法，这里以动态导入为例
 *    let ef2 = await import(`本模块URL链接`);
 * 由于可能需要异步引入第三方Base64库，编/解码方法encode/decode只能统一返回promise
 * 示例：
 *    let ef2 = (await import(`本模块URL链接`)).default;
 *    return await ef2.encode({u: "http://www.example.com/example.mp4", a: "Bilibili Freedoooooom/MarkII", r: "http://www.example.com/", o: "F:\\下载\\IDM", s: "example.mp4", q: true });
 *    // 本例使用了 await 关键词解析promise，所以上下文应该是个 async 函数
 *    // 当然也可以使用 promise.then 方法
 */
const ef2 = new Ef2();
self.ef2 = ef2;
export default ef2;
