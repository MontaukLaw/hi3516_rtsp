#ifndef LIBXD_MUXER_XD_MUXER_H
#define LIBXD_MUXER_XD_MUXER_H

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "XD_Common.h"


#ifdef __cplusplus
extern "C" {
#endif

    /// @addtogroup xdmuxer-macros 宏定义
    /// @{
    ///

    #define LIBXD_MUXER_VERSION XD_MakeVersion(3,1,10)

    /// @}
    ///

    /// @addtogroup xdmuxer-init 库初始化
    /// @{
    ///

    /// @brief 初始化 XD_Muxer SDK
    /// @retval -1 失败
    /// @retval 0  成功
    /// @note 此接口只需要在应用程序初始化时调用一次即可
    /// @warning 切勿在 DllMain 之类的代码中调用此接口，否则可能引发死锁
    /// @sa libXD_Muxer_Fini()
    XD_API int libXD_Muxer_Init(void);

    /// @brief 释放 XD_Muxer SDK 已分配的资源
    /// @retval -1 失败
    /// @retval 0  成功
    /// @note 此接口只需要在应用程序退出时调用一次即可
    /// @warning 切勿在 DllMain 之类的代码中调用此接口，否则可能引发死锁
    /// @sa libXD_Muxer_Init()
    XD_API void libXD_Muxer_Fini(void);

    /// @}
    ///

    /// @addtogroup xdmuxer-ver 版本信息
    /// @{
    ///

    /// @brief 获取 XD_Muxer 库版本信息
    /// @return 包含版本信息的字符串
    XD_API const char* libXD_Muxer_Version(void);

    /// @brief 获取 XD_Muxer 库版本号
    /// @return 返回整数形式的版本号数字
    /// @par 版本号说明
    ///   数据位 | 描述
    ///   -------|----------
    ///   31~24  | 主版本号
    ///   23~16  | 子版本号
    ///   15~00  | 修正版本号
    ///
    XD_API int libXD_Muxer_VersionNumber(void);

    /// @}
    ///

    /// @addtogroup xdmuxer-inst 打开及关闭
    /// @{
    ///

    /// @brief 打开一个封装器实例
    /// @retval -1                  失败
    /// @retval 0                   成功
    /// @note 字符串编码格式, Windows: ANSI/GBK, Linux: UTF8
    /// @note 在调用此接口前，必须确保已经调用 libXD_Muxer_Init() 对库进行初始化
    /// @sa XD_MuxerClose()
    XD_API int XD_MuxerOpen(const char* uri, int isHaveAudio);



    /// @brief 关闭一个封装器实例
    /// @param [in] token           已创建的实例对象
    /// @retval -1                  失败
    /// @retval 0                   成功
    /// @sa XD_MuxerOpen()
    XD_API int XD_MuxerClose(int token);

    /// @}
    ///

    /// @addtogroup xdmuxer-data 数据输入输出
    /// @{
    ///

    /// @brief 将多媒体数据块推入封装器
    /// @param [in] token           已创建的实例对象
    /// @param [in] block           需要封装的数据，通常是一个多媒体帧
    /// @retval -1                  失败
    /// @retval 0                   成功
    /// @sa XD_MuxerOpen()
    /// @warning 非线程安全，切勿在多个线程中调用此接口对同一个实例进行操作
    XD_API int XD_MuxerPushData(int token, const XD_StreamBlock* block);

    /// @brief 将内存中缓存的数据即时写入磁盘
    /// @param [in] token           已创建的实例对象
    /// @retval -1                  失败
    /// @retval 0                   成功
    /// @sa XD_MuxerOpen(), XD_MuxerPushData()
    /// @warning 非线程安全，切勿在多个线程中调用此接口对同一个实例进行操作
    XD_API int XD_MuxerFlush(int token);

    /// @}
    ///

    /// @addtogroup xdmuxer-prop 状态检测
    /// @{
    ///

    /// @brief 检测是否有文件生成
    /// @param [in] token           已创建的实例对象
    /// @retval 0   未生成
    /// @retval 1   已生成
    /// @sa XD_MuxerOpen(), XD_MuxerPushData()
    /// @warning 非线程安全，切勿在多个线程中调用此接口对同一个实例进行操作
    XD_API int XD_MuxerFileExists(int token);

    /// @brief 获取已经写入磁盘的文件大小
    /// @param [in] token           已创建的实例对象
    /// @retval 0   文件未生成或未有数据写入
    /// @retval >0  已写入磁盘的字节数
    /// @sa XD_MuxerOpen()
    /// @warning 非线程安全，切勿在多个线程中调用此接口对同一个实例进行操作
    XD_API int64_t XD_MuxerFileSize(int token);

    /// @brief 获取已经写入磁盘的文件大小
    /// @param [in] token           已创建的实例对象
    /// @param [in] yes             开关
    /// @retval 0   设置成功
    /// @retval -1  设置失败
    /// @sa XD_MuxerOpen()
    /// @warning 非线程安全，切勿在多个线程中调用此接口对同一个实例进行操作
    XD_API int XD_MuxerSnapshot(int token, int yes);    
    /// @}
    ///

#ifdef __cplusplus
}
#endif

/// @}
///

#endif // LIBXD_MUXER_XD_MUXER_H
