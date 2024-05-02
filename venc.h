#ifndef __VENC_H_
#define __VENC_H_

#include "sample_comm.h"

#include <XD_Common.h>

#define MAX_VENC_CHANNELS 2
#define MAX_CALLBACKS 2

typedef int (*StreamCallback)(int channel, XD_StreamBlock *block, void *userData);

struct stream
{
    struct pairs
    {
        StreamCallback cb;
        void *opaque;
    } callbacks[MAX_VENC_CHANNELS][MAX_CALLBACKS];
    int vencChannels[MAX_VENC_CHANNELS];
    uint8_t *data;
    int length;
    pthread_mutex_t mutex;
    //	pthread_t	thread;
    int exit;
    HI_U8 videoBuffer[1024 * 1024];
};

int stream_init();
int stream_destory();
int stream_add_callback(int channel, StreamCallback cb, void *userData);
int stream_del_callback(int channel, StreamCallback cb, void *userData);

void doCallback(struct stream *s, int channel, VENC_STREAM_S *pstStream);

#endif
