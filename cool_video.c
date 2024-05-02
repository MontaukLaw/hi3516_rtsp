#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <signal.h>
#include <mpi_vi.h>
#include "utils.h"
#include "video.h"
#include "rtspserver.h"
#include "sample_comm.h"
#include "venc.h"

static int isExit = 0;
static pthread_t vpss_get_frame_thread_id;

HI_VOID *vpss_get_frame_thread(HI_VOID *params)
{
	HI_S32 s32Ret = HI_SUCCESS;

	VPSS_GRP VpssGrp = 0;
	VPSS_CHN VpssChn = VPSS_CHN0;
	VIDEO_FRAME_INFO_S stVFrame;

	HI_S32 s32MilliSec = 1000;
	// Mat mat;

	long counter = 0;

	// HI_S32 HI_MPI_VPSS_GetChnFrame(VPSS_GRP VpssGrp, VPSS_CHN VpssChn,VIDEO_FRAME_INFO_S *pstVideoFrame, HI_S32 s32MilliSec);
	while (isExit == 0)
	{
		s32Ret = HI_MPI_VPSS_GetChnFrame(VpssGrp, VpssChn, &stVFrame, s32MilliSec);
		if (HI_SUCCESS != s32Ret)
		{
			SAMPLE_PRT("HI_MPI_VPSS_GetChnFrame failed with %#x!\n", s32Ret);
			continue;
		}

		// SAMPLE_PRT("HI_MPI_VPSS_GetChnFrame success!\n");
		// frame2Mat(&stVFrame, mat);

		// 写入并查看是否能正常生成jpg文件, 如果可以, YUV转RGB就是成功的. 当然这里也可以直接写入RGB格式.
		if (counter == 10)
		{
			// imwrite("test.jpg", mat);
		}

		counter++;

		s32Ret = HI_MPI_VPSS_ReleaseChnFrame(VpssGrp, VpssChn, &stVFrame);
		if (HI_SUCCESS != s32Ret)
		{
			SAMPLE_PRT("HI_MPI_VPSS_ReleaseChnFrame failed with %#x!\n", s32Ret);
			continue;
		}
	}

	return NULL;
}

static void handler_sig(HI_S32 signo)
{
	if (SIGINT == signo || SIGTERM == signo)
	{
		isExit = 1;
		printf("\033[0;31mprogram termination abnormally!\033[0;39m\n");
	}
}

static int Callback(int channel, XD_StreamBlock *block, void *userData)
{
	rtsp_push_data(channel, block, userData);

	return 0;
}

static int init_param(int argc, char **argv)
{
	int opt = 0;
	while ((opt = getopt(argc, argv, "v")) != -1)
	{
		switch (opt)
		{
		case 'v':
			printf(get_app_version());
			exit(0);
		default:
			break;
		}
	}
	return 0;
}

static int internal_start(int type)
{

	int result;
	result = video_start();
	if (result != 0)
	{
		printf("video_start failed\n");
		return -1;
	}

	stream_add_callback(0, Callback, (void *)0);
	// stream_add_callback(1, Callback, (void *)1);

	return 0;
}

static int internal_stop()
{
	stream_del_callback(0, Callback, (void *)0);
	video_stop();
	return 0;
}

int main(int argc, char *argv[])
{
	int result;
	VIDEO_FRAME_INFO_S vide_frame;

	init_param(argc, argv);
	signal(SIGINT, handler_sig);
	signal(SIGTERM, handler_sig);

	stream_init();

	result = rtsp_start();
	if (result != 0)
	{
		printf("rtsp_start failed\n");
		return -1;
	}

	result = internal_start(0);
	if (result != 0)
	{
		printf("video_start failed\n");
	}

	isExit = 0;

	pthread_create(&vpss_get_frame_thread_id, 0, vpss_get_frame_thread, NULL);

	while (isExit == 0)
	{
		sleep(1);
	}

	internal_stop();
	rtsp_stop();

	stream_destory();
	return result;
}
