#ifndef LIBXD_XD_COMMON_H
#define LIBXD_XD_COMMON_H

#include <stdarg.h>
#include <stddef.h>
#if defined(HAVE_STDINT_H) || __GNUC__ >= 4 || _MSC_VER > 1500
#  include <stdint.h>  // for int8_t, uint8_t, int16_t, uint16_t, int32_t, uint32_t, int64_t, uint64_t;
#else
#  if defined(_MSC_VER)
typedef signed __int8       int8_t;
typedef unsigned __int8     uint8_t;
typedef signed __int16      int16_t;
typedef unsigned __int16    uint16_t;
typedef signed __int32      int32_t;
typedef unsigned __int32    uint32_t;
typedef signed __int64      int64_t;
typedef unsigned __int64    uint64_t;
#  else
#  endif
#endif

/// @addtogroup xdcommon-macros 宏定义
/// @{
///

#if defined(_MSC_VER) && !defined(XD_STATIC_LIB)
#  ifdef XD_EXPORTS
#    define XD_API __declspec(dllexport)
#  else
#    define XD_API __declspec(dllimport)
#  endif
#else
#  define XD_API
#endif

#define XD_DECL ///< 函数调用方式

#if defined(_MSC_VER)
#  define XD_DEPRECATED __declspec(deprecated) ///< 已经废弃的接口
#else
#  define XD_DEPRECATED __attribute__((deprecated)) ///< 已经废弃的接口
#endif

#define BLOCK_FLAG_DISCONTINUITY        0x0001      ///< 非连续块
#define BLOCK_FLAG_TYPE_I               0x0002      ///< I帧
#define BLOCK_FLAG_TYPE_P               0x0004      ///< P帧
#define BLOCK_FLAG_TYPE_B               0x0008      ///< B帧
#define BLOCK_FLAG_TYPE_PB              0x0010      ///< P/B帧
#define BLOCK_FLAG_HEADER               0x0020      ///< 含有头部信息
#define BLOCK_FLAG_END_OF_FRAME         0x0040      ///< 帧结束
#define BLOCK_FLAG_NO_KEYFRAME          0x0080      ///< 非关键帧
#define BLOCK_FLAG_END_OF_SEQUENCE      0x0100      ///< 序列结束
#define BLOCK_FLAG_CLOCK                0x0200      ///< 参考时钟
#define BLOCK_FLAG_SCRAMBLED            0x0400      ///< 已加扰或受扰
#define BLOCK_FLAG_PREROLL              0x0800      ///< 已经解码但未显示
#define BLOCK_FLAG_CORRUPTED            0x1000      ///< 数据损坏或丢失
#define BLOCK_FLAG_TOP_FIELD_FIRST      0x2000      ///< 顶场在前
#define BLOCK_FLAG_BOTTOM_FIELD_FIRST   0x4000      ///< 底场在先
#define BLOCK_FLAG_FRAGMENTIZED         0x8000      ///< 碎片化的数据，非完整帧
#define BLOCK_FLAG_BEGIN_OF_FRAME       0x10000     ///< 帧起始
#define BLOCK_FLAG_AUDIO_FRAME          0x20000     ///< 音频帧
#define BLOCK_FLAG_VIDEO_FRAME          0x40000     ///< 视频帧

#define XD_MakeFourCC( a, b, c, d ) \
    ( ((uint32_t)a) | ( ((uint32_t)b) << 8 ) \
    | ( ((uint32_t)c) << 16 ) | ( ((uint32_t)d) << 24 ) )

#define AV_FOURCC_NULL  XD_MakeFourCC('N', 'U', 'L', 'L')
#define AV_FOURCC_AAC   XD_MakeFourCC('m', 'p', '4', 'a')
#define AV_FOURCC_JPEG  XD_MakeFourCC('J', 'P', 'E', 'G')
#define AV_FOURCC_H264  XD_MakeFourCC('a', 'v', 'c', '1')
#define AV_FOURCC_HKMI  XD_MakeFourCC('H', 'K', 'M', 'I')
#define AV_FOURCC_MP2P  XD_MakeFourCC('M', 'P', '2', 'P')
#define AV_FOURCC_MP2T  XD_MakeFourCC('M', 'P', '2', 'T')
#define AV_FOURCC_PCMA  XD_MakeFourCC('P', 'C', 'M', 'A')
#define AV_FOURCC_PCMU  XD_MakeFourCC('P', 'C', 'M', 'U')
#define AV_FOURCC_S16B  XD_MakeFourCC('S', '1', '6', 'B')
#define AV_FOURCC_S16L  XD_MakeFourCC('S', '1', '6', 'L')
#define AV_FOURCC_XDMI  XD_MakeFourCC('X', 'D', 'M', 'I')
#define AV_FOURCC_H265  XD_MakeFourCC('H', 'E', 'V', 'X')

/// 将数值 v 向上对齐到 x
#define XD_AlignedUpTo(v, x)  (((v)+((x)-1)) & (~((x)-1)))

/// 打印当前源文件名、行号及其他调试信息
#ifdef DEBUG
#  define XD_DbgPrintf(fmt, ...) fprintf(stderr, "%s:%d " fmt, __FILE__, __LINE__, __VA_ARGS__)
#else
#  define XD_DbgPrintf(fmt, ...) do {} while (0)
#endif

/// 生成 32 位版本号
#define XD_MakeVersion(a, b, c) (a<<24|b<<16|c)

/// 当前定义版本号
#define LIBXD_COMMON_VERSION XD_MakeVersion(3,1,18)

/// 定义流数据块最大尺寸
#define XD_STREAM_BLOCK_MAX_SIZE 16777216

/// @}
///

#ifdef __cplusplus
extern "C" {
#endif

    /// @addtogroup xdcommon-enums 枚举类型
    /// @{
    ///

    /// @brief 日志等级
    typedef enum {
        XD_LogNullLevel,        ///< 不记录任何内容
        XD_LogErrorLevel,       ///< 记录错误消息
        XD_LogCriticalLevel,    ///< 记录危急消息
        XD_LogWarningLevel,     ///< 记录警告消息
        XD_LogNoticeLevel,      ///< 记录通知消息
        XD_LogInformationLevel, ///< 记录信息消息
        XD_LogDebugLevel,       ///< 记录调式信息
        XD_LogDefaultLevel = XD_LogWarningLevel ///< 库初始化时默认等级
    } XD_LogLevel;

    /// @brief 日志时间格式
    typedef enum {
        XD_LogNullTime,             ///< 不记录时间
        XD_LogHighPrecisionTime,    ///< 高精度时间，精确到1微秒, 格式为： 000000.000000
        XD_LogOnlyTime,             ///< 只有时间部分(UTC), 格式为： hh:mm:ss UTC
        XD_LogOnlyLocalTime,        ///< 只有时间部分(本地), 格式为： hh:mm:ss
        XD_LogDateTime,             ///< 有日期及时间(UTC), 格式为： YYYY-MM-DD hh:mm:ss UTC
        XD_LogLocalDateTime,        ///< 有日期及时间(本地), 格式为： YYYY-MM-DD hh:mm:ss
        XD_LogDefaultTimeFormat = XD_LogHighPrecisionTime, ///< 库初始化时默认格式
    } XD_LogTimeFormat;

    /// @brief 日志输出格式
    typedef enum {
        XD_LogOutputPlainText,      ///< 输出纯文本格式
        XD_LogOutputTTYText,        ///< 输出TTY类型文本格式， 通过不同的颜色代表不同的日志等级
        XD_LogDefaultOutputFormat = XD_LogOutputPlainText, ///< 库初始化时默认格式
    } XD_LogOutputFormat;

    /// @}
    ///

    /// @addtogroup xdcommon-structs 数据结构
    /// @{
    ///
    // 前置声明
    /// 数据流输出的结构体
    struct XD_StreamBlock;
    typedef struct XD_StreamBlock XD_StreamBlock;

    /// 流数据块资源释放回调函数
    /// @param [in] block       已分配的流数据块指针
    /// @return 无返回值.
    typedef void (*XD_StreamBlockDeallocator)(XD_StreamBlock* block);

    /// 流数据块
    #pragma pack(8)
    struct XD_StreamBlock {
        XD_StreamBlock* p_next;         ///< 后续数据块

        uint8_t*        p_buffer;       ///< 负载数据起始位置
        uint32_t        i_buffer;       ///< 负载数据长度

        uint8_t*        p_start;        ///< 缓冲区起始位置
        uint32_t        i_size;         ///< 缓冲区长度

        uint32_t        i_flags;        ///< 数据包类型，是I帧，P帧
        uint32_t        i_nb_samples;   ///< 音频样本数量
        uint32_t        i_track;        ///< 数据轨道
        uint32_t        i_codec;        ///< 编码类型FOURCC('a','b','c','d')

        int64_t         i_pts;          ///< 数据包时戳
        int64_t         i_dts;          ///< 解码时戳
        int64_t		    i_length;       ///< 持续时长
        XD_StreamBlockDeallocator pf_release;   ///< 数据块释放回调
    };
    #pragma pack()


#ifdef __cplusplus 
}
#endif

/// @}
///

#endif // LIBXD_XD_COMMON_H
