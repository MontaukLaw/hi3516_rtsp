#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "rtspserver.h"

static RTSP_SVR *gRtspSvr = 0;
#define MAX_CHANNELS 1

struct RtspID
{
    int streamID;
    int trackVideoID;
    int trackAudioID;
};

static struct RtspID rtspIDs[MAX_CHANNELS];

int rtsp_push_data(int channel, XD_StreamBlock *block, void *opaque)
{

    int streamId, trackId;
    int ret = 0;

    // printf("channel = %d\n",channel);
    if (!block || !gRtspSvr)
        return -1;

    if (block->i_codec == AV_FOURCC_H264)
        trackId = rtspIDs[channel].trackVideoID;
    else
        trackId = rtspIDs[channel].trackAudioID;

    streamId = rtspIDs[channel].streamID;
    block->i_track = trackId;

    ret = RTSP_SVR_PushData(gRtspSvr, streamId, trackId, block);
    /*
        if(block->i_codec == AV_FOURCC_H264)
            printf("H264 trackid=%d, ret=%d\n",trackId, ret);
        else
            printf("AAC trackid=%d, ret=%d\n",trackId, ret);
    */
    return ret;
}

int rtsp_start()
{
    int codec = AV_FOURCC_H264;
    int codecAudio = AV_FOURCC_AAC;
    int index = 0, i;
    int ret;
    char buffer[64] = {0};
    if (gRtspSvr)
        return 0;

    ret = libXD_Stream_Init();
    if (ret != 0)
    {
        printf("libXD_Stream_Init failed\n");
        return -1;
    }

    gRtspSvr = RTSP_SVR_New("tcp.svr://0.0.0.0:554");
    if (!gRtspSvr)
    {
        printf("RTSP_SVR_New failed\n");
        return -1;
    }

    for (i = 0; i < MAX_CHANNELS; i++)
    {
        snprintf(buffer, sizeof(buffer), "live/%d", i + 1);
        rtspIDs[i].streamID = RTSP_SVR_NewStream(gRtspSvr, buffer);
        RTSP_SVR_SetStreamParam(gRtspSvr, rtspIDs[i].streamID, RTSP_STM_PARAM_TYPE_CACHE_TIME, (void *)5, 0);

        // video track
        rtspIDs[i].trackVideoID = RTSP_SVR_NewStreamTrack(gRtspSvr, rtspIDs[i].streamID, "track1", 5000000);
        ret = RTSP_SVR_SetStreamTrackParam(gRtspSvr, rtspIDs[i].streamID, rtspIDs[i].trackVideoID, RTSP_STMTR_PARAM_TYPE_FOURCC,
                                           (const void *)codec, 0);
        if (ret != 0)
        {
            printf("RTSP_SVR_SetStreamTrackParam failed! 1\n");
            return -1;
        }
/*
        //audio track
        rtspIDs[i].trackAudioID = RTSP_SVR_NewStreamTrack(gRtspSvr, rtspIDs[i].streamID, "track2", 1000000);
        ret = RTSP_SVR_SetStreamTrackParam(gRtspSvr, rtspIDs[i].streamID, rtspIDs[i].trackAudioID, RTSP_STMTR_PARAM_TYPE_FOURCC,
                                     (const void*)codecAudio, 0);
        if(ret != 0){
            printf("RTSP_SVR_SetStreamTrackParam failed! 1\n");
            return -1;
        }
*/
#if 0
		//  RTSP_STMTR_PARAM_TYPE_SAMPLERATE
	     ret = RTSP_SVR_SetStreamTrackParam(gRtspSvr, rtspIDs[i].streamID, rtspIDs[i].trackAudioID, RTSP_STMTR_PARAM_TYPE_FOURCC,
                                     (void*)44100, 0);
        if(ret != 0){
            printf("RTSP_SVR_SetStreamTrackParam (RTSP_STMTR_PARAM_TYPE_SAMPLERATE) failed! 1\n");
            return -1;
        }
#endif
        RTSP_SVR_SetStreamTrackParam(gRtspSvr, rtspIDs[i].streamID, rtspIDs[i].trackVideoID, RTSP_STMTR_PARAM_TYPE_AUTO_SDP, (const void *)1, 0);
        if (ret != 0)
        {
            printf("RTSP_SVR_SetStreamTrackParam video sdp failed! 2\n");
            return -1;
        }
        /*
                RTSP_SVR_SetStreamTrackParam(gRtspSvr, rtspIDs[i].streamID, rtspIDs[i].trackAudioID, RTSP_STMTR_PARAM_TYPE_AUTO_SDP, (const void*)1, 0);
                if(ret != 0){
                    printf("RTSP_SVR_SetStreamTrackParam audio sdp failed! 2\n");
                    return -1;
                }
        */
        // printf("rtspIDs[i].streamID = %d, video track id = %d\n",  rtspIDs[i].streamID, rtspIDs[i].trackVideoID);
    }

    ret = RTSP_SVR_Start(gRtspSvr);
    if (ret < 0)
    {
        printf("RTSP_SVR_Start failed %d, gRtspSvr=%p\n", ret, gRtspSvr);
        RTSP_SVR_Destroy(gRtspSvr);
        return -1;
    }

    // Limit rtsp connections to 16
    RTSP_SVR_SetParam(gRtspSvr, RTSP_SVR_PARAM_TYPE_MAX_SESSIONS, (const void *)8, 0);

    return 0;
}

void rtsp_stop()
{
    if (gRtspSvr)
    {

        RTSP_SVR_Stop(gRtspSvr);
        RTSP_SVR_Destroy(gRtspSvr);
        gRtspSvr = 0;
    }

    libXD_Stream_Fini();
}
