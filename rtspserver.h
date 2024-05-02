#ifndef RTSPSERVER_H
#define	RTSPSERVER_H

#include <XD_Stream.h>

#if __cplusplus
extern "C"{
#endif

int rtsp_start();

int rtsp_push_data(int channel, XD_StreamBlock *block, void* opaque);

void rtsp_stop();

#if __cplusplus
}
#endif

#endif /* RTSPSERVER_H */
