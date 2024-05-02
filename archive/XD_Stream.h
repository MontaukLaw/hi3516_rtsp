#ifndef LIBXD_XD_STREAM_H
#define LIBXD_XD_STREAM_H

#include <stddef.h>
#include "XD_Common.h"


#ifdef __cplusplus
extern "C"
{
#endif

    /// @addtogroup xdstream-macros 宏定义
    /// @{
    ///

    #define LIBXD_STREAM_VERSION XD_MakeVersion(4,2,12)    ///< 当前定义版本号

    #ifdef _MSC_VER
    #define LIBXD_STREAM_CALLCVT __cdecl
    #else
    #define LIBXD_STREAM_CALLCVT
    #endif

    /// @}
    ///

    /// @addtogroup xdstream-enums 枚举类型
    /// @{

    /// 库参数配置类型
    enum libXD_StreamConfigType {
        LIBXD_StreamCoreTicker,             ///< 运行核心调度器循序间隔
        LIBXD_StreamBindWorkerToPerCPU,     ///< 绑定每个工作线程到特定的CPU上, 数据类型为： int, 有效值为: 0/1
        LIBXD_StreamAdjustWorkerPriority,   ///< 调整工作线程优先级, 数据类型为: int, 有效值为: -2 ~ +2
        LIBXD_StreamEnableSourceEvent,      ///< 启用或禁用数据源事件通知, 数据类型为: int, 有效值为: 0/1
    };

    /// 数据流事件类型
    typedef enum XD_StreamEventType {
        XD_StreamRTSPTMO,               ///< RTSP 命令超时
        XD_StreamClosing,               ///< 数据流实例即将关闭
        XD_Stream404 = 404,             ///< 媒体不存在
        XD_Stream408 = 408,             ///< 请求超时
        XD_Stream462 = 462,             ///< 目标不可到达
        XD_Stream500 = 500,             ///< 服务器内部错误
        XD_StreamRTCPBYE = 0x110,       ///< RTSP 服务端自动离线
        XD_StreamStopped = 0x106,       ///< 数据流传输停止
        XD_StreamSourceStopped = 0x111, ///< 数据源自动停止
        XD_StreamSourceStarted = 0x112, ///< 数据源已经启动传输
    } XD_StreamEventType;

    /// 数据流传输模式
    enum XD_StreamTransferMode {
        XD_StreamOverUDP,       ///< 使用 UDP 方式传输数据
        XD_StreamOverTCP,       ///< 使用 TCP 方式传输数据 
        XD_StreamOverMCAST,     ///< 使用多播方式传输数据
    };

    /// @}
    ///

    /// @addtogroup xdstream-cfg 库全局参数配置
    /// @{
    ///

    /// 配置 XD_Stream 库初始化前参数
    /// @param [in] request     参数类型, 参见 #libXD_StreamConfigType
    /// @param [in] optVal      参数值数据
    /// @param [in] optLen      参数值数据长度
    /// @return 无返回值
    /// @note 此接口必须在 libXD_Stream_Init() 前调用
    void LIBXD_STREAM_CALLCVT libXD_Stream_Config(int request, const char* optVal, size_t optLen);

    /// @}
    ///

    /// @addtogroup xdstream-init 库初始化
    /// @{
    ///

    /// @brief 初始化 XD_Stream SDK
    /// @retval -1 失败
    /// @retval 0  成功
    /// @note 此接口只需要在应用程序初始化时调用一次即可
    /// @warning 切勿在 DllMain 之类的代码中调用此接口，否则可能引发死锁
    /// @sa libXD_Stream_Fini()
    int LIBXD_STREAM_CALLCVT libXD_Stream_Init(void);

    /// @brief 释放 XD_Stream SDK 已分配的资源
    /// @retval -1 失败
    /// @retval 0  成功
    /// @note 此接口只需要在应用程序退出时调用一次即可
    /// @warning 切勿在 DllMain 之类的代码中调用此接口，否则可能引发死锁
    /// @sa libXD_Stream_Init()
    void LIBXD_STREAM_CALLCVT libXD_Stream_Fini(void);

    /// @}
    ///

    /// @addtogroup xdstream-ver 版本信息
    /// @{
    ///

    /// @brief 获取 XD_Stream 库版本信息
    /// @return 包含版本信息的字符串
    const char* LIBXD_STREAM_CALLCVT libXD_Stream_Version(void);

    /// @brief 获取 XD_Stream 库版本号
    /// @return 返回整数形式的版本号数字
    /// @par 版本号说明
    ///   数据位 | 描述
    ///   -------|----------
    ///   31~24  | 主版本号
    ///   23~16  | 子版本号
    ///   15~00  | 修正版本号
    ///
    int LIBXD_STREAM_CALLCVT libXD_Stream_VersionNumber(void);

    /// @}
    ///

    /// @addtogroup xdstream-instance 打开及关闭
    /// @{
    ///

    /// @brief 打开一个数据流传输实例
    /// @param [in] url		        数据流资源定位
    /// @retval <1 失败
    /// @retval >0 流传输实例句柄
    /// @note 在调用此接口前须确保已经调用 libXD_Stream_Init() 对库进行初始化
    /// @sa XD_StreamClose()
    int LIBXD_STREAM_CALLCVT XD_StreamOpen(const char *url);

    /// @brief 关闭一个数据流传输实例
    /// @param [in] token		    已创建的实例对象
    /// @retval -1  失败
    /// @retval 0   成功
    /// @sa XD_StreamOpen()
    int LIBXD_STREAM_CALLCVT XD_StreamClose(int token);

    /// @}
    ///

    /// @addtogroup xdstream-control 流传输控制
    /// @{

    /// @brief 启动数据流传输
    /// @param [in] token		    已创建的实例对象
    /// @retval -1	失败
    /// @retval >=0	成功
    /// @sa XD_StreamOpen(), XD_StreamStop()
    int LIBXD_STREAM_CALLCVT XD_StreamStart(int token);

    /// @brief 停止数据流传输
    /// @param [in] token		    已创建的实例对象
    /// @retval -1	失败
    /// @retval 0	成功
    /// @sa XD_StreamOpen(), XD_StreamStart()
    int LIBXD_STREAM_CALLCVT XD_StreamStop(int token);

    /// @}
    ///

    /// @addtogroup xdstream-callback 回调管理
    /// @{
    ///

    /// @brief 流数据数据回调函数定义
    /// @param [in] block		    数据块信息
    /// @param [in] opaque	        用户关联数据
    /// @retval -1	失败
    /// @retval 0	成功
    /// @note 当 block 不再使用时，请调用 XD_StreamBlockRelease() 释放已分配的资源
    /// @warning 切勿在此回调函数中调用任何带有 XD_Stream 前缀的接口
    typedef int (LIBXD_STREAM_CALLCVT *XD_StreamDataCallback)(XD_StreamBlock* block, void* opaque);

    /// @brief 添加数据流数据处理回调函数
    /// @param [in] token		    已创建的实例对象
    /// @param [in] callback        数据处理函数
    /// @param [in] opaque          用户关联数据
    /// @retval -1	失败
    /// @retval 0	成功
    /// @sa XD_StreamOpen(), XD_StreamDelDataCallback(), XD_StreamDataCallback
    int LIBXD_STREAM_CALLCVT XD_StreamAddDataCallback(int token, XD_StreamDataCallback callback, void* opaque);

    /// @brief 删除数据流数据处理回调函数
    /// @param [in] token		    已创建的实例对象
    /// @param [in] callback        数据处理函数
    /// @param [in] opaque          用户关联数据
    /// @retval -1	失败
    /// @retval 0	成功
    /// @sa XD_StreamOpen(), XD_StreamDelDataCallback(), XD_StreamDataCallback
    /// @warning callback 及 opaque 两个参数必须与在调用 XD_StreamAddDataCallback() 时传进去的参数相一致，否则将不会执行任何操作或引发不可预知的错误
    int LIBXD_STREAM_CALLCVT XD_StreamDelDataCallback(int token, XD_StreamDataCallback callback, void* opaque);

    /// @brief 流事件通知回调函数
    /// @param [in] event		    消息类型
    /// @param [in] opaque		    用户关联数据
    /// @retval -1	失败
    /// @retval 0	成功
    /// @warning 切勿在此回调函数中调用任何带有 XD_Stream 前缀的接口
    typedef int (LIBXD_STREAM_CALLCVT *XD_StreamEventCallback)(XD_StreamEventType event, void* opaque);

    /// @brief 添加消息处理回调函数
    /// @param [in] token			已创建的实例对象
    /// @param [in] callback	    消息处理函数
    /// @param [in] opaque		    用户关联数据
    /// @retval -1	失败
    /// @retval 0	成功
    /// @sa XD_StreamOpen(), XD_StreamDelEventCallback(), XD_StreamEventCallback
    int LIBXD_STREAM_CALLCVT XD_StreamAddEventCallback(int token, XD_StreamEventCallback callback, void* opaque);

    /// @brief 删除消息处理回调函数
    /// @param [in] token			已创建的实例对象
    /// @param [in] callback	    消息处理函数
    /// @param [in] opaque		    用户关联数据
    /// @retval -1	失败
    /// @retval 0	成功
    /// @sa XD_StreamOpen(), XD_StreamAddEventCallback(), XD_StreamEventCallback
    /// @note callback 及 opaque 两个参数必须与在调用 XD_StreamAddEventCallback() 时传进去的参数相一致，否则将不会执行任何操作或引发不可预知的错误
    int LIBXD_STREAM_CALLCVT XD_StreamDelEventCallback(int token, XD_StreamEventCallback callback, void* opaque);

    /// @}
    ///

    /// @addtogroup xdstream-settings 参数配置
    /// @{
    ///

    /// @brief 允许输出 RTP 数据包
    /// @param [in] token			已创建的实例对象
    /// @param [in] yes             是否启用，有效值: 0 = 否，1 = 是
    /// @retval -1  失败
    /// @retval 0   成功
    int LIBXD_STREAM_CALLCVT XD_StreamEnableRTPPacket(int token, int yes);

    /// @brief 设置认证信息
    /// @param [in] token			已创建的实例对象
    /// @param [in] username        认证所需的用户名
    /// @param [in] password        认证所需的用户密码
    /// @param [in] pwdIsMD5        指示 password 是否已经为 MD5 值, 有效值: 0 = 否，1 = 是
    /// @retval -1  失败
    /// @retval 0   成功
    /// @note 本接口必须在 XD_StreamStart() 之前调用方可生效
    int LIBXD_STREAM_CALLCVT XD_StreamSetAuth(int token, const char* username, const char* password, int pwdIsMD5);

    /// @brief 设置超时属性
    /// @param [in] token			已创建的实例对象
    /// @param [in] msecs		    通信及数据收发超时, 以毫秒为单位
    /// @retval -1	失败
    /// @retval 0	成功
    int LIBXD_STREAM_CALLCVT XD_StreamSetTimeout(int token, int msecs);

    /// @brief 设置数据流传输模式
    /// @param [in] token           已创建的实例对象
    /// @param [in] mode            数据流传输模式
    /// @retval -1	失败
    /// @retval 0	成功
    /// @note 只能在 XD_StreamStart() 未调用之前有效
    /// @sa XD_StreamOpen(), XD_StreamTransferMode
    int LIBXD_STREAM_CALLCVT XD_StreamSetTransferMode(int token, int mode);

    /// @brief 设置输出数据格式
    /// @param [in] token           已创建的实例对象
    /// @param [in] fourcc          FourCC 格式编码
    /// @retval -1	失败
    /// @retval 0	成功
    int LIBXD_STREAM_CALLCVT XD_StreamSetOutputFormat(int token, int fourcc);

    /// @brief 用于设置输出帧片段模式
    ///        此模式可以降低 CPU 消耗及内存使用，库内默认开启此功能，如果数据播放出现问题可以关闭此功能
    /// @param [in] token           已创建的实例对象
    /// @param [in] enable          是否启用, 0 = 禁用, 1 = 启用
    /// @retval -1	失败
    /// @retval 0	成功
    /// @note 此模式仅对 ES 格式输出有效，PS 格式会强制开启
    int LIBXD_STREAM_CALLCVT XD_StreamSetOFPM(int token, int enable);

    /// @}
    ///

    // RTSP 服务器相关数据类型
    //=========================================================================
    #define RTSP_SVR_MAX_STREAMS        128

    #ifndef RTSP_SVR_TYPE_DEFINED
    #define RTSP_SVR_TYPE_DEFINED
    struct RTSP_SVR;
    typedef struct RTSP_SVR RTSP_SVR;
    #endif // RTSP_SVR_TYPE_DEFINED

    #ifndef RTSP_SVRCS_TYPE_DEFINED
    #define RTSP_SVRCS_TYPE_DEFINED
    struct RTSP_SVRCS;
    typedef struct RTSP_SVRCS RTSP_SVRCS;
    #endif // RTSP_SVRCS_TYPE_DEFINED

    #ifndef RTSP_SVR_PARAMTYPE_TYPE_DEFINED
    #define RTSP_SVR_PARAMTYPE_TYPE_DEFINED
    enum RTSP_SVR_ParamType {
        RTSP_SVR_PARAM_TYPE_UNKNOWN,
        RTSP_SVR_PARAM_TYPE_CACHE_TIME,
        RTSP_SVR_PARAM_TYPE_MAX_SESSIONS,
        RTSP_SVR_PARAM_TYPE_HOST,
        RTSP_SVR_PARAM_TYPE_MAX,
    };
    typedef enum RTSP_SVR_ParamType RTSP_SVR_ParamType;
    #endif // RTSP_SVR_PARAMTYPE_TYPE_DEFINED

    #ifndef RTSP_SVR_STAGE_TYPE_DEFINED
    #define RTSP_SVR_STAGE_TYPE_DEFINED
    enum RTSP_SVR_Stage {
        RTSP_SVR_STAGE_UNKNOWN,
        RTSP_SVR_STAGE_PENDING,
        RTSP_SVR_STAGE_STARTED,
        RTSP_SVR_STAGE_RUNNING,
        RTSP_SVR_STAGE_STOPED,
        RTSP_SVR_STAGE_FAULT,
        RTSP_SVR_STAGE_MAX,
    };
    typedef enum RTSP_SVR_Stage RTSP_SVR_Stage;
    #endif // RTSP_SVR_STAGE_TYPE_DEFINED

    // RTSP 服务器数据流相关数据类型
    //=========================================================================
    #define RTSP_STM_CACHE_TIME     2
    #define RTSP_STM_MAX_TRACKS     8

    #ifndef RTSP_STM_TYPE_DEFINED
    #define RTSP_STM_TYPE_DEFINED
    struct RTSP_STM;
    typedef struct RTSP_STM RTSP_STM;
    #endif // RTSP_STM_TYPE_DEFINED

    #ifndef RTSP_STM_PARAMTYPE_TYPE_DEFINED
    #define RTSP_STM_PARAMTYPE_TYPE_DEFINED
    enum RTSP_STM_ParamType {
        RTSP_STM_PARAM_TYPE_UNKNOWN,
        RTSP_STM_PARAM_TYPE_CACHE_TIME,
        RTSP_STM_PARAM_TYPE_MAX,
    };
    typedef enum RTSP_STM_ParamType RTSP_STM_ParamType;
    #endif // RTSP_STM_PARAMTYPE_TYPE_DEFINED

    #ifndef RTSP_STM_STAGE_TYPE_DEFINED
    #define RTSP_STM_STAGE_TYPE_DEFINED
    enum RTSP_STM_Stage {
        RTSP_STM_STAGE_UNKNOWN,
    };
    typedef enum RTSP_STM_Stage RTSP_STM_Stage;
    #endif // RTSP_STM_STAGE_TYPE_DEFINED

    // RTSP 服务器数据流轨道相关数据类型
    //=========================================================================
    #define RTSP_STMTR_CACHE_TIME   2

    #ifndef RTSP_STM_TYPE_DEFINED
    #define RTSP_STM_TYPE_DEFINED
    struct RTSP_STM;
    typedef struct RTSP_STM RTSP_STM;
    #endif // RTSP_STM_TYPE_DEFINED

    #ifndef RTSP_STMTR_TYPE_DEFINED
    #define RTSP_STMTR_TYPE_DEFINED
    struct RTSP_STMTR;
    typedef struct RTSP_STMTR RTSP_STMTR;
    #endif // RTSP_STMTR_TYPE_DEFINED

    #ifndef RTSP_STMTR_MEDIA_TYPE_TYPE_DEFINED
    #define RTSP_STMTR_MEDIA_TYPE_TYPE_DEFINED
    enum RTSP_STMTR_MediaType {
        RTSP_STMTR_MEDIA_TYPE_UNKNOWN,
        RTSP_STMTR_MEDIA_TYPE_AUDIO,
        RTSP_STMTR_MEDIA_TYPE_VIDEO,
        RTSP_STMTR_MEDIA_TYPE_TEXT,
        RTSP_STMTR_MEDIA_TYPE_APPLICATION,
        RTSP_STMTR_MEDIA_TYPE_MESSAGE,
        RTSP_STMTR_MEDIA_TYPE_MAX,
    };
    typedef enum RTSP_STMTR_MediaType RTSP_STMTR_MediaType;
    #endif // RTSP_STMTR_MEDIA_TYPE_TYPE_DEFINED

    #ifndef RTSP_STMTR_PARAMTYPE_TYPE_DEFINED
    #define RTSP_STMTR_PARAMTYPE_TYPE_DEFINED
    enum RTSP_STMTR_ParamType {
        RTSP_STMTR_PARAM_TYPE_UNKNOWN,
        RTSP_STMTR_PARAM_TYPE_CACHE_TIME,       ///< Max caching time in seconds
        RTSP_STMTR_PARAM_TYPE_FOURCC,
        RTSP_STMTR_PARAM_TYPE_CHANNELS,
        RTSP_STMTR_PARAM_TYPE_SAMPLERATE,
        RTSP_STMTR_PARAM_TYPE_PPS_DATA,
        RTSP_STMTR_PARAM_TYPE_SPS_DATA,
        RTSP_STMTR_PARAM_TYPE_VPS_DATA,
        RTSP_STMTR_PARAM_TYPE_AUTO_SDP,
        RTSP_STMTR_PARAM_TYPE_JPEG_QFACTOR,
        RTSP_STMTR_PARAM_TYPE_MEDIA_CHANGED,
        RTSP_STMTR_PARAM_TYPE_MAX,
    };
    typedef enum RTSP_STMTR_ParamType RTSP_STMTR_ParamType;
    #endif // RTSP_STMTR_PARAMTYPE_TYPE_DEFINED

    // RTSP 服务器相关接口
    //=========================================================================

    /// @brief Create a new rtsp server
    /// @return the rtsp server instance
    RTSP_SVR* LIBXD_STREAM_CALLCVT RTSP_SVR_New(const char* url);

    /// @brief Destroy a rtsp server instance
    /// @return no value returns
    void LIBXD_STREAM_CALLCVT RTSP_SVR_Destroy(RTSP_SVR* svr);

    ///
    int LIBXD_STREAM_CALLCVT RTSP_SVR_Reset(RTSP_SVR* svr, const char* url);

    /// @brief Start the rtsp server
    int LIBXD_STREAM_CALLCVT RTSP_SVR_Start(RTSP_SVR* svr);

    /// @brief Stop the rtsp server
    int LIBXD_STREAM_CALLCVT RTSP_SVR_Stop(RTSP_SVR* svr);

    int LIBXD_STREAM_CALLCVT RTSP_SVR_Retain(RTSP_SVR* svr);

    RTSP_SVR* LIBXD_STREAM_CALLCVT RTSP_SVR_Release(RTSP_SVR* svr);

    int LIBXD_STREAM_CALLCVT RTSP_SVR_GetRefCount(const RTSP_SVR* svr);

    int LIBXD_STREAM_CALLCVT RTSP_SVR_GetStage(const RTSP_SVR* svr);

    int LIBXD_STREAM_CALLCVT RTSP_SVR_GetParam(RTSP_SVR* svr, int param,
            void* buffer, int* len);

    int LIBXD_STREAM_CALLCVT RTSP_SVR_SetParam(RTSP_SVR* svr, int param,
            const void* data, int len);

    // RTSP_SVR_NewCS() is private

    int LIBXD_STREAM_CALLCVT RTSP_SVR_DestroyCS(RTSP_SVR* svr, RTSP_SVRCS* svrcs);

    int LIBXD_STREAM_CALLCVT RTSP_SVR_TeardownAllCS(RTSP_SVR* svr);

    int LIBXD_STREAM_CALLCVT RTSP_SVR_NewStream(RTSP_SVR* svr,
            const char* streamName);

    int LIBXD_STREAM_CALLCVT RTSP_SVR_DestroyStream(RTSP_SVR* svr, int streamId);

    int LIBXD_STREAM_CALLCVT RTSP_SVR_NewStreamTrack(RTSP_SVR* svr,
            int streamId, const char* trackName, int bandWidth);

    int LIBXD_STREAM_CALLCVT RTSP_SVR_DestroyStreamTrack(RTSP_SVR* svr,
            int streamId, int trackId);

    /// @brief Find stream id by stream name
    /// @return the stream id
    int LIBXD_STREAM_CALLCVT RTSP_SVR_FindStream(RTSP_SVR* svr,
            const char* streamName);

    int LIBXD_STREAM_CALLCVT RTSP_SVR_FindStreamTrack(RTSP_SVR* svr,
            int streamId, const char* trackName);

    /// @brief Get stream sdp string
    /// @return the sdp string
    const char* LIBXD_STREAM_CALLCVT RTSP_SVR_GetStreamSDP(RTSP_SVR* svr,
            int streamId);

    /// @brief Get stream sdp content size
    /// @return the sdp content size
    int LIBXD_STREAM_CALLCVT RTSP_SVR_GetStreamSDPSize(RTSP_SVR* svr,
            int streamId);

    const char* LIBXD_STREAM_CALLCVT RTSP_SVR_GetStreamName(RTSP_SVR* svr,
            int streamId);

    int LIBXD_STREAM_CALLCVT RTSP_SVR_GetStreamParam(RTSP_SVR* svr,
            int streamId, int param, void* buffer, int* len);

    int LIBXD_STREAM_CALLCVT RTSP_SVR_SetStreamParam(RTSP_SVR* svr,
            int streamId, int param, const void* data, int len);

    int LIBXD_STREAM_CALLCVT RTSP_SVR_GetStreamTrackParam(RTSP_SVR* svr,
            int streamId, int trackId, int param, void* buffer, int* len);

    int LIBXD_STREAM_CALLCVT RTSP_SVR_SetStreamTrackParam(RTSP_SVR* svr,
            int streamId, int trackId, int param, const void* data, int len);

    int LIBXD_STREAM_CALLCVT RTSP_SVR_PushData(RTSP_SVR* svr,
            int streamId, int trackId, XD_StreamBlock* block);

#ifdef __cplusplus
}
#endif

/// @}
///

#endif // LIBXD_XD_STREAM_H
