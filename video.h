#ifndef VIDEO_H
#define VIDEO_H

#include <mpi_vpss.h>

#ifdef __cplusplus
extern "C" {
#endif 

int video_start();

int video_stop();

int video_get_frame(VIDEO_FRAME_INFO_S *pstVideoFrame, int millisec);

int video_release_frame(VIDEO_FRAME_INFO_S *pstVideoFrame);


#ifdef __cplusplus
}
#endif

#endif 
