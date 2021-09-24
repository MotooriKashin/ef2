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
}
export default new Ef2();
