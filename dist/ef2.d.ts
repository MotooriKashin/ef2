declare class Ef2 {
    /**
     * 编码下载数据为ef2协议链接，用法同http协议链接
     * @param data 下载配置对象
     * @returns ef2协议链接
     */
    encode(data: EF2Data): string;
    /**
     * 解码ef2协议为对象
     * @param ef2ptl ef2协议链接
     * @returns 下载配置对象
     */
    decode(ef2ptl: string): EF2Data;
    /**
     * encode的封装，直接发起下载
     * 未安装ef2.exe将没有任何效果
     * @param data 下载配置对象
     */
    sendLinkToIDM(data: EF2Data): void;
}
declare const _default: Ef2;
export default _default;
/**
 * 下载配置对象，传递给encode的数据格式
 */
interface EF2Data {
    /**
     * URL
     */
    url: string;
    /**
     * 文件名（含拓展名）
     */
    out?: string;
    /**
     * user-agent
     */
    userAgent?: string;
    /**
     * referer
     */
    referer?: string;
    /**
     * 文件保存目录
     */
    directory?: string;
    /**
     * cookies
     * 一般还是免了吧，有些cookie无法在js环境中获取到
     */
    cookies?: string;
    /**
     * 用于send的参数并改用POST方法下载
     * 一般都是GET，没见过要求POST的
     */
    postDate?: string;
    /**
     * 用于http身份校验的账户，与password配对
     * 从来没见过+1
     * **并非注册的网站账户！**
     */
    userName?: string;
    /**
     * 用于http身份校验的密钥，与userName配对
     * 从来没见过+1
     * **并非注册的网站密码！**
     */
    password?: string;
    /**
     * 禁用IDM下载前的询问弹窗，其中可以选择修改文件名及保存目录等信息
     */
    toastDisabled?: true;
    /**
     * 把下载链接添加到下载列表但是不立即开始下载，需要下载时再手动到IDM里开始
     */
    sendToList?: true;
}
