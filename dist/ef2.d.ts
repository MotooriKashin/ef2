declare class MsgHost {
    /**
     * 浏览器窗口序号
     */
    static wid: number;
    /**
     * 与ef2通讯的WebSocket对象
     */
    static ws: WebSocket;
    /**
     * 消息序号
     */
    static index: number;
    static ha: any;
    static lb: {
        ftp: number;
        ftps: number;
        http: number;
        https: number;
        idmreg: number;
        xup: number;
    };
    static Ra: boolean;
    static get i(): number;
    constructor(ws: WebSocket);
    /**
     * 构造并发送消息
     * @param type 消息类型
     * @param sort 二级消息类型？
     * @param flag 未知
     * @param ext 消息附加信息？
     * @param obj 消息内容
     * @param tag 未知
     */
    static MSG(type: number, sort: number, flag: number, ext: (string | number)[], obj?: Record<number, any>, tag?: boolean): void;
    static ob(a: string): number;
    static Yb(a: string): number;
    static Fc(a: Capture["requestHeaders"]): any[];
    static en(url: string): string;
    static send(msg?: string): void;
    /**
     * 初始化连接
     */
    static init(): void;
    /**
     * 浏览器窗口刷新消息
     */
    static windowUpdate(): void;
    /**
     * 浏览器窗口获得焦点
     */
    static windowShow(): void;
    /**
     * 浏览器窗口关闭
     */
    static windowClose(): void;
    /**
     * 抓取链接模式
     * @param obj 下载配置数据
     */
    static capture(obj: Partial<Capture>): void;
    /**
     * 直接下载模式
     * @param obj 下载配置数据
     */
    static download(obj: Partial<RightKey>): number;
    /**
     * 批量下载模式
     * @param obj 下载配置数据
     */
    static downloadAll(obj: Partial<RightAll>): void;
    /**
     * 捕获下载模式
     * @param obj 下载配置数据
     */
    static intercept(obj: Partial<Capture>): number;
    /**
     * DASH解析模式
     * @param obj json配置
     * @param dash 实际dash数据
     */
    static DASH(obj: Partial<Capture>, dash: Partial<DASH>): void;
    /**
     * 直接下载
     * @param obj 下载数据
     */
    download(obj: Partial<RightKey>): void;
    /**
     * 批量下载
     * @param obj 下载数据
     */
    downloadAll(obj: Partial<RightAll>): void;
    /**
     * 自定义请求头下载
     * @param obj 下载数据
     */
    downloadWithCustomHeaders(obj: Partial<Capture>): void;
    /**
     * 直接发送消息给IDM
     * @param msg 消息内容，可以直接发送ef2协议
     */
    send(msg: string): void;
    /**
     * 关闭连接并退出ef2辅助工具
     */
    close(): void;
}
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
    /**
     * 启动nativeMessaging模式并建立连接
     * @returns nativeMessaging模式提供的方法
     */
    nativeMessaging(): Promise<MsgHost>;
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
interface Capture {
    userAgent: string;
    url: string;
    /**
     * 自定义请求头
     */
    requestHeaders: Record<"name" & "value", string>[];
    responseHeaders: Record<"name" & "value", string>[];
    /**
     * 下载任务id
     */
    id: string | number;
    /**
     * 通道id
     */
    portId: number;
    /**
     * ip地址
     */
    ip: string;
    method: "POST" | "GET";
    statusCode: number;
    statusLine: string;
    /**
     * 文件拓展名（大写）
     */
    extension: string;
    origin: string;
    /**
     * 文件名
     */
    name: string;
    /**
     * HTTP端口号
     */
    port: number;
}
interface RightKey {
    url: string;
    referer: string;
    origin: string;
    userAgent: string;
}
interface RightAll {
    origin: string;
    userAgent: string;
    /**
     * 网页标题
     */
    window: string;
    urls: {
        url: string;
        referer: string;
        /**
         * 链接描述文字
         */
        name: string;
    }[];
}
interface DASH {
    attestation: Partial<{
        playerAttestationRenderer: {
            botguardData: {};
            challenge: string;
        };
    }>;
    frameworkUpdates: Partial<{
        entityBatchUpdate: {};
    }>;
    messages: [{
        mealbarPromoRenderer: {};
    }];
    microformat: Partial<{
        playerMicroformatRenderer: {};
    }>;
    playabilityStatus: Partial<{
        contextParams: string;
        miniplayer: {
            miniplayerRenderer: {};
        };
        playableInEmbed: boolean;
        status: "OK";
    }>;
    playbackTracking: Record<string, Record<"baseUrl", string>>;
    playerConfig: Partial<{
        audioConfig: {};
        mediaCommonConfig: {};
        streamSelectionConfig: {};
        webPlayerConfig: {};
    }>;
    responseContext: Partial<{
        mainAppWebResponseContext: {};
        maxAgeSeconds: number;
        serviceTrackingParams: {}[];
        webResponseContextExtensionData: {};
    }>;
    storyboards: Partial<{
        playerStoryboardSpecRenderer: {
            spec: string;
        };
    }>;
    streamingData: Partial<{
        adaptiveFormats: {
            approxDurationMs: number;
            averageBitrate: number;
            bitrate: number;
            contentLength: number;
            fps: number;
            height: number;
            indexRange: {
                start: number;
                end: number;
            };
            initRange: {
                start: number;
                end: number;
            };
            itag: number;
            lastModified: string;
            mimeType: string;
            projectionType: string;
            quality: string;
            qualityLabel: string;
            url: string;
            width: number;
            audioChannels?: number;
            audioQuality?: "AUDIO_QUALITY_LOW" | "AUDIO_QUALITY_MEDIUM";
            audioSampleRate?: number;
        }[];
        expiresInSeconds: "21540";
        formats: DASH["streamingData"]["adaptiveFormats"];
    }>;
    trackingParams: string;
    videoDetails: Partial<{
        allowRatings: true;
        author: string;
        channelId: string;
        isCrawlable: true;
        isLiveContent: false;
        isLowLatencyLiveStream: false;
        isOwnerViewing: false;
        isPrivate: false;
        isUnpluggedCorpus: false;
        keywords: string[];
        latencyClass: string;
        lengthSeconds: number;
        shortDescription: string;
        thumbnail: {
            thumbnails: {}[];
        };
        title: string;
        videoId: string;
        viewCount: number;
    }>;
    videoQualityPromoSupportedRenderers: Partial<{
        videoQualityPromoRenderer: {};
    }>;
}
