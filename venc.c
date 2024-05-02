#include "venc.h"

#if 0
static pthread_t gs_VencPid;
static pthread_t gs_VencQpmapPid;
static SAMPLE_VENC_GETSTREAM_PARA_S gs_stPara;
static SAMPLE_VENC_QPMAP_SENDFRAME_PARA_S stQpMapSendFramePara;
#endif

static HI_S32 gs_s32SnapCnt = 0;
#define TEMP_BUF_LEN 8
#define MAX_THM_SIZE (64 * 1024)

#define MAX_FRAME_LENGTH (2 * 1024 * 1024)

#define BLOCK_FLAG_TYPE_I 0x0002  ///< I帧
#define BLOCK_FLAG_TYPE_P 0x0004  ///< P帧
#define BLOCK_FLAG_TYPE_B 0x0008  ///< B帧
#define BLOCK_FLAG_TYPE_PB 0x0010 ///< P/B帧
#define BLOCK_FLAG_HEADER 0x0020  ///< 含有头部信息


struct stream gStream;

int stream_init()
{
    int i = 0, j = 0, size;
    int channel;

    for (i = 0; i < MAX_VENC_CHANNELS; i++)
    {
        for (j = 0; j < MAX_CALLBACKS; j++)
        {
            gStream.callbacks[i][j].cb = 0;
            gStream.callbacks[i][j].opaque = 0;
        }
    }

    //	gStream.thread = 0;
    gStream.data = (uint8_t *)malloc(MAX_FRAME_LENGTH * sizeof(uint8_t));
    if (!gStream.data)
    {
        printf("malloc failed\n");
        return -1;
    }

    gStream.exit = 0;
    if (pthread_mutex_init(&gStream.mutex, NULL) != 0)
    {
        free(gStream.data);
        return -1;
    }

    return 0;
}

int stream_destory()
{
    int i, j;

    pthread_mutex_lock(&gStream.mutex);
    for (i = 0; i < MAX_VENC_CHANNELS; i++)
    {
        for (j = 0; j < MAX_CALLBACKS; j++)
        {
            gStream.callbacks[i][j].cb = 0;
            gStream.callbacks[i][j].opaque = 0;
        }
    }
    pthread_mutex_unlock(&gStream.mutex);
    pthread_mutex_destroy(&gStream.mutex);
    free(gStream.data);
    return 0;
}

static int find_slot(int channel)
{
    int i = 0;
    for (i = 0; i < MAX_CALLBACKS; i++)
    {
        if (gStream.callbacks[channel][i].cb == 0)
            return i;
    }

    return -1;
}

int stream_add_callback(int channel, StreamCallback cb, void *opaque)
{
    int i;
    if (!cb)
        return -1;

    if (channel < 0 || channel >= MAX_VENC_CHANNELS)
    {
        printf("invaild encoder channel:%d\n", channel);
        return -1;
    }

    pthread_mutex_lock(&gStream.mutex);

    for (i = 0; i < MAX_CALLBACKS; i++)
    {
        if (gStream.callbacks[channel][i].cb == cb &&
            gStream.callbacks[channel][i].opaque == opaque)
        {
            pthread_mutex_unlock(&gStream.mutex);
            return 0;
        }
    }
    i = find_slot(channel);
    if (i == -1)
    {
        printf("Each channel  max support 4 callbacks\n");
        pthread_mutex_unlock(&gStream.mutex);
        return -1;
    }

    gStream.callbacks[channel][i].opaque = opaque;
    gStream.callbacks[channel][i].cb = cb;
    printf("add it in callback[%d]\n", i);
    pthread_mutex_unlock(&gStream.mutex);
    return 0;
}

int stream_del_callback(int channel, StreamCallback cb, void *opaque)
{
    int i;

    if (channel < 0 || channel >= MAX_VENC_CHANNELS)
    {
        printf("invaild encoder channel:%d\n", channel);
        return -1;
    }

    pthread_mutex_lock(&gStream.mutex);
    for (i = 0; i < MAX_CALLBACKS; i++)
    {
        if (gStream.callbacks[channel][i].cb == cb &&
            gStream.callbacks[channel][i].opaque == opaque)
        {
            break;
        }
    }

    if (i < MAX_CALLBACKS)
    {
        gStream.callbacks[channel][i].cb = 0;
        gStream.callbacks[channel][i].opaque = 0;
    }
    pthread_mutex_unlock(&gStream.mutex);
    return 0;
}

static int convertFlags(H264E_NALU_TYPE_E type)
{
    // printf("type=%d\n", type);
    switch (type)
    {
    case H264E_NALU_PSLICE:
        return BLOCK_FLAG_TYPE_P;
    case H264E_NALU_ISLICE:
        return BLOCK_FLAG_TYPE_I;
    case H264E_NALU_SEI:
    case H264E_NALU_SPS:
    case H264E_NALU_PPS:
        return BLOCK_FLAG_TYPE_I;
    default:
        return 0;
    }
}

void doCallback(struct stream *s, int channel, VENC_STREAM_S *pstStream)
{
    int i = 0, j = 0;
    uint32_t len = 0;
    if (channel >= MAX_VENC_CHANNELS)
        return;

    pthread_mutex_lock(&s->mutex);

    //    printf("pstStream->u32PackCount = %d\n", pstStream->u32PackCount);

    XD_StreamBlock block;
    uint8_t *p = s->data;
    s->length = 0;
    memset(&block, 0, sizeof(block));

    /*
            fwrite(pstStream->pstPack[i].pu8Addr + pstStream->pstPack[i].u32Offset,
                   pstStream->pstPack[i].u32Len - pstStream->pstPack[i].u32Offset, 1, pFd);
    */
    if (pstStream->u32PackCount > 1)
    {
        for (i = 0; i < pstStream->u32PackCount; i++)
        {
            len = pstStream->pstPack[i].u32Len - pstStream->pstPack[i].u32Offset;
            if (s->length + len > MAX_FRAME_LENGTH)
            {
                printf("Frame is too big >2MB we drop it\n");
                return;
            }
            memcpy(p + s->length, pstStream->pstPack[i].pu8Addr + pstStream->pstPack[i].u32Offset, len);
            s->length += len;
        }
        block.p_buffer = s->data;
        block.i_buffer = s->length;
    }
    else
    {
        block.p_buffer = pstStream->pstPack[0].pu8Addr + pstStream->pstPack[0].u32Offset;
        block.i_buffer = pstStream->pstPack[0].u32Len - pstStream->pstPack[0].u32Offset;
    }

    block.i_track = channel;
    block.i_pts = pstStream->pstPack[0].u64PTS;
    block.i_dts = pstStream->pstPack[0].u64PTS;
    block.i_codec = AV_FOURCC_H264;
    block.i_flags = convertFlags(pstStream->pstPack[0].DataType.enH264EType);

    for (i = 0; i < MAX_CALLBACKS; i++)
    {
        if (s->callbacks[channel][i].cb)
        {
            // info_msg("do callback chanel:%d incallback[%d]\n",i,channel);
            s->callbacks[channel][i].cb(channel, &block, s->callbacks[channel][i].opaque);
        }
    }
    pthread_mutex_unlock(&s->mutex);
}
