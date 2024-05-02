/******************************************************************************

  Copyright (C), 2017, Hisilicon Tech. Co., Ltd.

 ******************************************************************************
  File Name     : sample_venc.c
  Version       : Initial Draft
  Author        : Hisilicon multimedia software group
  Created       : 2017
  Description   :
******************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <signal.h>

#include "sample_comm.h"
#include "venc.h"

#ifdef HI_FPGA
#define PIC_SIZE PIC_1080P
#else
#define PIC_SIZE PIC_1080P
#endif

extern struct stream gStream;

VENC_INTRA_REFRESH_MODE_E SAMPLE_VENC_GetIntraRefreshMode(void)
{
    char c;
    VENC_INTRA_REFRESH_MODE_E enIntraRefreshMode = INTRA_REFRESH_ROW;

Begin_Get:

    printf("please input choose IntraRefresh mode!\n");
    printf("\t r) ROW.\n");
    printf("\t c) COLUMN.\n");

    while ((c = getchar()) != '\n' && c != EOF)
        switch (c)
        {
        case 'r':
            enIntraRefreshMode = INTRA_REFRESH_ROW;
            break;
        case 'c':
            enIntraRefreshMode = INTRA_REFRESH_COLUMN;
            break;

        default:
            SAMPLE_PRT("input IntraRefresh Mode: %c, is invaild!\n", c);
            goto Begin_Get;
        }
    return enIntraRefreshMode;
}

HI_S32 SAMPLE_VENC_SYS_Init(HI_U32 u32SupplementConfig, SAMPLE_SNS_TYPE_E enSnsType)
{
    HI_S32 s32Ret;
    HI_U64 u64BlkSize;
    VB_CONFIG_S stVbConf;
    PIC_SIZE_E enSnsSize;
    SIZE_S stSnsSize;

    memset(&stVbConf, 0, sizeof(VB_CONFIG_S));

    s32Ret = SAMPLE_COMM_VI_GetSizeBySensor(enSnsType, &enSnsSize);
    if (HI_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("SAMPLE_COMM_VI_GetSizeBySensor failed!\n");
        return s32Ret;
    }

    s32Ret = SAMPLE_COMM_SYS_GetPicSize(enSnsSize, &stSnsSize);
    if (HI_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("SAMPLE_COMM_SYS_GetPicSize failed!\n");
        return s32Ret;
    }

    u64BlkSize = COMMON_GetPicBufferSize(stSnsSize.u32Width, stSnsSize.u32Height, PIXEL_FORMAT_YVU_SEMIPLANAR_422, DATA_BITWIDTH_8, COMPRESS_MODE_SEG, DEFAULT_ALIGN);
    stVbConf.astCommPool[0].u64BlkSize = u64BlkSize;
    stVbConf.astCommPool[0].u32BlkCnt = 10;

    u64BlkSize = COMMON_GetPicBufferSize(720, 576, PIXEL_FORMAT_YVU_SEMIPLANAR_422, DATA_BITWIDTH_8, COMPRESS_MODE_SEG, DEFAULT_ALIGN);
    stVbConf.astCommPool[1].u64BlkSize = u64BlkSize;
    stVbConf.astCommPool[1].u32BlkCnt = 10;

    stVbConf.u32MaxPoolCnt = 2;

    if (0 == u32SupplementConfig)
    {
        s32Ret = SAMPLE_COMM_SYS_Init(&stVbConf);
    }
    else
    {
        s32Ret = SAMPLE_COMM_SYS_InitWithVbSupplement(&stVbConf, u32SupplementConfig);
    }
    if (HI_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("SAMPLE_COMM_SYS_GetPicSize failed!\n");
        return s32Ret;
    }

    return HI_SUCCESS;
}

HI_VOID SAMPLE_VENC_SetDCFInfo(VI_PIPE ViPipe)
{
    ISP_DCF_INFO_S stIspDCF;

    HI_MPI_ISP_GetDCFInfo(ViPipe, &stIspDCF);

    // description: Thumbnail test
    strncpy((char *)stIspDCF.stIspDCFConstInfo.au8ImageDescription, "Thumbnail test", DCF_DRSCRIPTION_LENGTH);
    // manufacturer: Hisilicon
    strncpy((char *)stIspDCF.stIspDCFConstInfo.au8Make, "Hisilicon", DCF_DRSCRIPTION_LENGTH);
    // model number: Hisilicon IP Camera
    strncpy((char *)stIspDCF.stIspDCFConstInfo.au8Model, "Hisilicon IP Camera", DCF_DRSCRIPTION_LENGTH);
    // firmware version: v.1.1.0
    strncpy((char *)stIspDCF.stIspDCFConstInfo.au8Software, "v.1.1.0", DCF_DRSCRIPTION_LENGTH);

    stIspDCF.stIspDCFConstInfo.u32FocalLength = 0x00640001;
    stIspDCF.stIspDCFConstInfo.u8Contrast = 5;
    stIspDCF.stIspDCFConstInfo.u8CustomRendered = 0;
    stIspDCF.stIspDCFConstInfo.u8FocalLengthIn35mmFilm = 1;
    stIspDCF.stIspDCFConstInfo.u8GainControl = 1;
    stIspDCF.stIspDCFConstInfo.u8LightSource = 1;
    stIspDCF.stIspDCFConstInfo.u8MeteringMode = 1;
    stIspDCF.stIspDCFConstInfo.u8Saturation = 1;
    stIspDCF.stIspDCFConstInfo.u8SceneCaptureType = 1;
    stIspDCF.stIspDCFConstInfo.u8SceneType = 0;
    stIspDCF.stIspDCFConstInfo.u8Sharpness = 5;
    stIspDCF.stIspDCFUpdateInfo.u32ISOSpeedRatings = 500;
    stIspDCF.stIspDCFUpdateInfo.u32ExposureBiasValue = 5;
    stIspDCF.stIspDCFUpdateInfo.u32ExposureTime = 0x00010004;
    stIspDCF.stIspDCFUpdateInfo.u32FNumber = 0x0001000f;
    stIspDCF.stIspDCFUpdateInfo.u8WhiteBalance = 1;
    stIspDCF.stIspDCFUpdateInfo.u8ExposureMode = 0;
    stIspDCF.stIspDCFUpdateInfo.u8ExposureProgram = 1;
    stIspDCF.stIspDCFUpdateInfo.u32MaxApertureValue = 0x00010001;

    HI_MPI_ISP_SetDCFInfo(ViPipe, &stIspDCF);

    return;
}

HI_S32 SAMPLE_VENC_VI_Init(SAMPLE_VI_CONFIG_S *pstViConfig, HI_BOOL bLowDelay, HI_U32 u32SupplementConfig)
{
    HI_S32 s32Ret;
    SAMPLE_SNS_TYPE_E enSnsType;
    ISP_CTRL_PARAM_S stIspCtrlParam;
    HI_U32 u32FrameRate;

    enSnsType = pstViConfig->astViInfo[0].stSnsInfo.enSnsType;

    pstViConfig->as32WorkingViId[0] = 0; // 0;
    // pstViConfig->s32WorkingViNum                              = 1;
    SAMPLE_PRT("device id is :%d \n", SAMPLE_COMM_VI_GetComboDevBySensor(pstViConfig->astViInfo[0].stSnsInfo.enSnsType, 0));

    pstViConfig->astViInfo[0].stSnsInfo.MipiDev = 1; // SAMPLE_COMM_VI_GetComboDevBySensor(pstViConfig->astViInfo[0].stSnsInfo.enSnsType, 0);
    pstViConfig->astViInfo[0].stSnsInfo.s32BusId = 1;

    // pstViConfig->astViInfo[0].stDevInfo.ViDev              = ViDev0;
    pstViConfig->astViInfo[0].stDevInfo.enWDRMode = WDR_MODE_NONE;

    if (HI_TRUE == bLowDelay)
    {
        pstViConfig->astViInfo[0].stPipeInfo.enMastPipeMode = VI_ONLINE_VPSS_ONLINE;
    }
    else
    {
        pstViConfig->astViInfo[0].stPipeInfo.enMastPipeMode = VI_OFFLINE_VPSS_OFFLINE;
    }
    s32Ret = SAMPLE_VENC_SYS_Init(u32SupplementConfig, enSnsType);
    if (s32Ret != HI_SUCCESS)
    {
        SAMPLE_PRT("Init SYS err for %#x!\n", s32Ret);
        return s32Ret;
    }

    // pstViConfig->astViInfo[0].stPipeInfo.aPipe[0]          = ViPipe0;
    pstViConfig->astViInfo[0].stPipeInfo.aPipe[1] = -1;
    pstViConfig->astViInfo[0].stPipeInfo.aPipe[2] = -1;
    pstViConfig->astViInfo[0].stPipeInfo.aPipe[3] = -1;

    // pstViConfig->astViInfo[0].stChnInfo.ViChn              = ViChn;
    // pstViConfig->astViInfo[0].stChnInfo.enPixFormat        = PIXEL_FORMAT_YVU_SEMIPLANAR_420;
    // pstViConfig->astViInfo[0].stChnInfo.enDynamicRange     = enDynamicRange;
    pstViConfig->astViInfo[0].stChnInfo.enVideoFormat = VIDEO_FORMAT_LINEAR;
    pstViConfig->astViInfo[0].stChnInfo.enCompressMode = COMPRESS_MODE_NONE; // COMPRESS_MODE_SEG;
    s32Ret = SAMPLE_COMM_VI_SetParam(pstViConfig);
    if (HI_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("SAMPLE_COMM_VI_SetParam failed with %d!\n", s32Ret);
        return s32Ret;
    }
    SAMPLE_COMM_VI_GetFrameRateBySensor(enSnsType, &u32FrameRate);

    s32Ret = HI_MPI_ISP_GetCtrlParam(pstViConfig->astViInfo[0].stPipeInfo.aPipe[0], &stIspCtrlParam);
    if (HI_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("HI_MPI_ISP_GetCtrlParam failed with %d!\n", s32Ret);
        return s32Ret;
    }
    stIspCtrlParam.u32StatIntvl = u32FrameRate / 30;
    s32Ret = HI_MPI_ISP_SetCtrlParam(pstViConfig->astViInfo[0].stPipeInfo.aPipe[0], &stIspCtrlParam);
    if (HI_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("HI_MPI_ISP_SetCtrlParam failed with %d!\n", s32Ret);
        return s32Ret;
    }

    s32Ret = SAMPLE_COMM_VI_StartVi(pstViConfig);
    if (HI_SUCCESS != s32Ret)
    {

        SAMPLE_COMM_SYS_Exit();
        SAMPLE_PRT("SAMPLE_COMM_VI_StartVi failed with %d!\n", s32Ret);
        return s32Ret;
    }

    return HI_SUCCESS;
}

HI_S32 SAMPLE_VENC_VPSS_Init(VPSS_GRP VpssGrp, HI_BOOL *pabChnEnable, DYNAMIC_RANGE_E enDynamicRange, PIXEL_FORMAT_E enPixelFormat, SIZE_S stSize[], SAMPLE_SNS_TYPE_E enSnsType)
{
    HI_S32 i;
    HI_S32 s32Ret;
    PIC_SIZE_E enSnsSize;
    SIZE_S stSnsSize;
    VPSS_GRP_ATTR_S stVpssGrpAttr = {0};
    VPSS_CHN_ATTR_S stVpssChnAttr[VPSS_MAX_PHY_CHN_NUM];

    s32Ret = SAMPLE_COMM_VI_GetSizeBySensor(enSnsType, &enSnsSize);
    if (HI_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("SAMPLE_COMM_VI_GetSizeBySensor failed!\n");
        return s32Ret;
    }

    s32Ret = SAMPLE_COMM_SYS_GetPicSize(enSnsSize, &stSnsSize);
    if (HI_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("SAMPLE_COMM_SYS_GetPicSize failed!\n");
        return s32Ret;
    }

    stVpssGrpAttr.enDynamicRange = enDynamicRange;
    stVpssGrpAttr.enPixelFormat = enPixelFormat;
    stVpssGrpAttr.u32MaxW = stSnsSize.u32Width;
    stVpssGrpAttr.u32MaxH = stSnsSize.u32Height;
    stVpssGrpAttr.stFrameRate.s32SrcFrameRate = -1;
    stVpssGrpAttr.stFrameRate.s32DstFrameRate = -1;
    stVpssGrpAttr.bNrEn = HI_TRUE;
    stVpssGrpAttr.stNrAttr.enNrType = VPSS_NR_TYPE_VIDEO;
    stVpssGrpAttr.stNrAttr.enNrMotionMode = NR_MOTION_MODE_NORMAL;
    stVpssGrpAttr.stNrAttr.enCompressMode = COMPRESS_MODE_FRAME;

    for (i = 0; i < VPSS_MAX_PHY_CHN_NUM; i++)
    {
        if (HI_TRUE == pabChnEnable[i])
        {
            if (i == 2)
            {
                SAMPLE_PRT("i=2: w:%d h:%d\n", stSize[0].u32Width, stSize[0].u32Height);
                stVpssChnAttr[i].u32Width = stSize[0].u32Width;
                stVpssChnAttr[i].u32Height = stSize[0].u32Height;
            }
            else
            {
                SAMPLE_PRT("i:%d w:%d h:%d\n", i, stSize[i].u32Width, stSize[i].u32Height);
                stVpssChnAttr[i].u32Width = stSize[i].u32Width;
                stVpssChnAttr[i].u32Height = stSize[i].u32Height;
            }
            stVpssChnAttr[i].enChnMode = VPSS_CHN_MODE_USER;
            stVpssChnAttr[i].enCompressMode = COMPRESS_MODE_NONE; // COMPRESS_MODE_SEG;
            stVpssChnAttr[i].enDynamicRange = enDynamicRange;
            stVpssChnAttr[i].enPixelFormat = enPixelFormat;
            stVpssChnAttr[i].stFrameRate.s32SrcFrameRate = -1;
            stVpssChnAttr[i].stFrameRate.s32DstFrameRate = -1;
            if (i == 2)
                stVpssChnAttr[i].u32Depth = 4;
            else
                stVpssChnAttr[i].u32Depth = 0;
            stVpssChnAttr[i].bMirror = HI_FALSE;
            stVpssChnAttr[i].bFlip = HI_FALSE;
            stVpssChnAttr[i].enVideoFormat = VIDEO_FORMAT_LINEAR;
            stVpssChnAttr[i].stAspectRatio.enMode = ASPECT_RATIO_NONE;
        }
    }

    s32Ret = SAMPLE_COMM_VPSS_Start(VpssGrp, pabChnEnable, &stVpssGrpAttr, stVpssChnAttr);
    if (s32Ret != HI_SUCCESS)
    {
        SAMPLE_PRT("start VPSS fail for %#x!\n", s32Ret);
    }

    return s32Ret;
}

HI_S32 SAMPLE_VENC_CheckSensor(SAMPLE_SNS_TYPE_E enSnsType, SIZE_S stSize)
{
    HI_S32 s32Ret;
    SIZE_S stSnsSize;
    PIC_SIZE_E enSnsSize;

    s32Ret = SAMPLE_COMM_VI_GetSizeBySensor(enSnsType, &enSnsSize);
    if (HI_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("SAMPLE_COMM_VI_GetSizeBySensor failed!\n");
        return s32Ret;
    }
    s32Ret = SAMPLE_COMM_SYS_GetPicSize(enSnsSize, &stSnsSize);
    if (HI_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("SAMPLE_COMM_SYS_GetPicSize failed!\n");
        return s32Ret;
    }

    if ((stSnsSize.u32Width < stSize.u32Width) || (stSnsSize.u32Height < stSize.u32Height))
    {
        // SAMPLE_PRT("Sensor size is (%d,%d), but encode chnl is (%d,%d) !\n",
        //     stSnsSize.u32Width,stSnsSize.u32Height,stSize.u32Width,stSize.u32Height);
        return HI_FAILURE;
    }

    return HI_SUCCESS;
}

HI_S32 SAMPLE_VENC_ModifyResolution(SAMPLE_SNS_TYPE_E enSnsType, PIC_SIZE_E *penSize, SIZE_S *pstSize)
{
    HI_S32 s32Ret;
    SIZE_S stSnsSize;
    PIC_SIZE_E enSnsSize;

    s32Ret = SAMPLE_COMM_VI_GetSizeBySensor(enSnsType, &enSnsSize);
    if (HI_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("SAMPLE_COMM_VI_GetSizeBySensor failed!\n");
        return s32Ret;
    }
    s32Ret = SAMPLE_COMM_SYS_GetPicSize(enSnsSize, &stSnsSize);
    if (HI_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("SAMPLE_COMM_SYS_GetPicSize failed!\n");
        return s32Ret;
    }

    *penSize = enSnsSize;
    pstSize->u32Width = stSnsSize.u32Width;
    pstSize->u32Height = stSnsSize.u32Height;

    return HI_SUCCESS;
}

static HI_S32 GetFilePostfix(PAYLOAD_TYPE_E enPayload, char *szFilePostfix)
{
    if (PT_H264 == enPayload)
    {
        strcpy(szFilePostfix, ".h264");
    }
    else if (PT_H265 == enPayload)
    {
        strcpy(szFilePostfix, ".h265");
    }
    else if (PT_JPEG == enPayload)
    {
        strcpy(szFilePostfix, ".jpg");
    }
    else if (PT_MJPEG == enPayload)
    {
        strcpy(szFilePostfix, ".mjp");
    }
    else
    {
        SAMPLE_PRT("payload type err!\n");
        return HI_FAILURE;
    }
    return HI_SUCCESS;
}
static HI_S32 create_and_start_vpss_grp0(HI_BOOL *abChnEnable, VPSS_CHN_ATTR_S *astVpssChnAttr, HI_U32 u32VdecChnNum,
                                         SIZE_S stDispSize, HI_U8 vpssChn)
{
    HI_S32 s32Ret = HI_SUCCESS;

    VPSS_GRP_ATTR_S stVpssGrpAttr;
    memset_s(&stVpssGrpAttr, sizeof(VPSS_GRP_ATTR_S), 0, sizeof(VPSS_GRP_ATTR_S));

    stVpssGrpAttr.stFrameRate.s32SrcFrameRate = -1;
    stVpssGrpAttr.stFrameRate.s32DstFrameRate = -1;

    stVpssGrpAttr.enDynamicRange = DYNAMIC_RANGE_SDR8;
    stVpssGrpAttr.enPixelFormat = PIXEL_FORMAT_YVU_SEMIPLANAR_420;
    stVpssGrpAttr.u32MaxW = 2688;
    stVpssGrpAttr.u32MaxH = 2160;
    stVpssGrpAttr.bNrEn = HI_FALSE;
    stVpssGrpAttr.stNrAttr.enCompressMode = COMPRESS_MODE_FRAME;
    stVpssGrpAttr.stNrAttr.enNrMotionMode = NR_MOTION_MODE_NORMAL;

    // specify vpss chn
    // memset(abChnEnable, 0, sizeof(abChnEnable));
    abChnEnable[vpssChn] = HI_TRUE;

    astVpssChnAttr[vpssChn].u32Width = stDispSize.u32Width;
    astVpssChnAttr[vpssChn].u32Height = stDispSize.u32Height;
    astVpssChnAttr[vpssChn].enChnMode = VPSS_CHN_MODE_USER;
    astVpssChnAttr[vpssChn].enCompressMode = COMPRESS_MODE_NONE;
    astVpssChnAttr[vpssChn].enDynamicRange = DYNAMIC_RANGE_SDR8;
    astVpssChnAttr[vpssChn].enVideoFormat = VIDEO_FORMAT_LINEAR;
    astVpssChnAttr[vpssChn].enPixelFormat = PIXEL_FORMAT_YVU_SEMIPLANAR_420;
    astVpssChnAttr[vpssChn].stFrameRate.s32SrcFrameRate = 30;
    astVpssChnAttr[vpssChn].stFrameRate.s32DstFrameRate = 30;
    astVpssChnAttr[vpssChn].u32Depth = 1;
    // astVpssChnAttr[vpssChn].u32Depth = 0;
    astVpssChnAttr[vpssChn].bMirror = HI_FALSE;
    astVpssChnAttr[vpssChn].bFlip = HI_FALSE;
    astVpssChnAttr[vpssChn].stAspectRatio.enMode = ASPECT_RATIO_NONE;

    s32Ret = SAMPLE_COMM_VPSS_Start(0, abChnEnable, &stVpssGrpAttr, astVpssChnAttr);

    return s32Ret;
}

static SAMPLE_VI_CONFIG_S stViConfig;

static void init_vi_config(void)
{
    VI_PIPE viPipe = 0; // 2;
    VI_CHN viChn = 0;
    HI_S32 s32ViCnt = 1;
    VI_DEV ViDev = 1;
    HI_S32 s32WorkSnsId = 0;
    WDR_MODE_E enWDRMode = WDR_MODE_NONE;
    DYNAMIC_RANGE_E enDynamicRange = DYNAMIC_RANGE_SDR8;
    PIXEL_FORMAT_E enPixFormat = PIXEL_FORMAT_YVU_SEMIPLANAR_420;
    VIDEO_FORMAT_E enVideoFormat = VIDEO_FORMAT_LINEAR;
    COMPRESS_MODE_E enCompressMode = COMPRESS_MODE_NONE;
    VI_VPSS_MODE_E enMastPipeMode = VI_OFFLINE_VPSS_OFFLINE; // VI_OFFLINE_VPSS_OFFLINE; // VI_OFFLINE_VPSS_OFFLINE;

    SAMPLE_COMM_VI_GetSensorInfo(&stViConfig);

    stViConfig.s32WorkingViNum = s32ViCnt;
    stViConfig.as32WorkingViId[0] = 0;
    stViConfig.astViInfo[s32WorkSnsId].stSnsInfo.MipiDev = ViDev;
    stViConfig.astViInfo[s32WorkSnsId].stSnsInfo.s32BusId = 1;
    stViConfig.astViInfo[s32WorkSnsId].stDevInfo.ViDev = ViDev;
    stViConfig.astViInfo[s32WorkSnsId].stDevInfo.enWDRMode = enWDRMode;
    stViConfig.astViInfo[s32WorkSnsId].stPipeInfo.enMastPipeMode = enMastPipeMode;
    stViConfig.astViInfo[s32WorkSnsId].stPipeInfo.aPipe[0] = viPipe;
    stViConfig.astViInfo[s32WorkSnsId].stPipeInfo.aPipe[1] = -1;
    stViConfig.astViInfo[s32WorkSnsId].stPipeInfo.aPipe[2] = -1;
    stViConfig.astViInfo[s32WorkSnsId].stPipeInfo.aPipe[3] = -1;
    // stViConfig.astViInfo[s32WorkSnsId].stPipeInfo.bIspBypass     = HI_TRUE;
    stViConfig.astViInfo[s32WorkSnsId].stPipeInfo.enPixFmt = enPixFormat;
    stViConfig.astViInfo[s32WorkSnsId].stChnInfo.ViChn = viChn;
    stViConfig.astViInfo[s32WorkSnsId].stChnInfo.enPixFormat = enPixFormat;
    stViConfig.astViInfo[s32WorkSnsId].stChnInfo.enDynamicRange = enDynamicRange;
    stViConfig.astViInfo[s32WorkSnsId].stChnInfo.enVideoFormat = enVideoFormat;
    stViConfig.astViInfo[s32WorkSnsId].stChnInfo.enCompressMode = enCompressMode;
}

static HI_S32 MY_VENC_Create(VENC_GOP_ATTR_S *pstGopAttr)
{

    VENC_CHN VencChn = 0;
    HI_S32 s32Ret;
    SIZE_S stPicSize;
    VENC_CHN_ATTR_S stVencChnAttr;
    SAMPLE_VI_CONFIG_S stViConfig;
    HI_U32 u32FrameRate = 30;
    HI_U32 u32StatTime;
    HI_U32 u32Gop = 30;

    // memset(&stVencChnAttr, 0, sizeof(stVencChnAttr));

    stPicSize.u32Width = 1920;
    stPicSize.u32Height = 1080;

    stVencChnAttr.stVencAttr.enType = PT_H264;
    stVencChnAttr.stVencAttr.u32MaxPicWidth = stPicSize.u32Width;
    stVencChnAttr.stVencAttr.u32MaxPicHeight = stPicSize.u32Height;
    stVencChnAttr.stVencAttr.u32PicWidth = stPicSize.u32Width;                          /*the picture width*/
    stVencChnAttr.stVencAttr.u32PicHeight = stPicSize.u32Height;                        /*the picture height*/
    stVencChnAttr.stVencAttr.u32BufSize = stPicSize.u32Width * stPicSize.u32Height * 2; /*stream buffer size*/
    stVencChnAttr.stVencAttr.u32Profile = 0;
    stVencChnAttr.stVencAttr.bByFrame = HI_TRUE;

    u32StatTime = 1;

    VENC_H264_CBR_S stH264Cbr;
    // memset(&stH264Cbr, 0, sizeof(stH264Cbr));

    stVencChnAttr.stRcAttr.enRcMode = VENC_RC_MODE_H264CBR;
    stH264Cbr.u32Gop = u32Gop;                 /*the interval of IFrame*/
    stH264Cbr.u32StatTime = u32StatTime;       /* stream rate statics time(s) */
    stH264Cbr.u32SrcFrameRate = u32FrameRate;  /* input (vi) frame rate */
    stH264Cbr.fr32DstFrameRate = u32FrameRate; /* target frame rate */
    // stH264Cbr.u32BitRate = 1024 * 2 + 2048 * u32FrameRate / 30;
    stH264Cbr.u32BitRate = 1024 * 5;

    memcpy(&stVencChnAttr.stRcAttr.stH264Cbr, &stH264Cbr, sizeof(VENC_H264_CBR_S));

    stVencChnAttr.stVencAttr.stAttrH264e.bRcnRefShareBuf = HI_FALSE;

    memcpy(&stVencChnAttr.stGopAttr, pstGopAttr, sizeof(VENC_GOP_ATTR_S));

    s32Ret = HI_MPI_VENC_CreateChn(VencChn, &stVencChnAttr);
    if (HI_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("HI_MPI_VENC_CreateChn [%d] faild with %#x! ===\n", VencChn, s32Ret);
        return s32Ret;
    }

    s32Ret = SAMPLE_COMM_VENC_CloseReEncode(VencChn);
    if (HI_SUCCESS != s32Ret)
    {
        HI_MPI_VENC_DestroyChn(VencChn);
        return s32Ret;
    }

    return HI_SUCCESS;
}

static HI_S32 create_venc_normal(void)
{
    VENC_CHN VencChn = 0;
    static HI_S32 s32Ret = HI_SUCCESS;
    static VENC_GOP_ATTR_S stGopAttr;
    // static VENC_GOP_MODE_E enGopMode;
    // PAYLOAD_TYPE_E enType = PT_H264;
    // PIC_SIZE_E enPicSize = PIC_1080P;
    // SAMPLE_RC_E enRcMode = SAMPLE_RC_CBR;
    // HI_U32 u32Profile = 0;
    VENC_RECV_PIC_PARAM_S stRecvParam;

    // set rc mode gop mode
    // enGopMode = VENC_GOPMODE_NORMALP;
    stGopAttr.enGopMode = VENC_GOPMODE_NORMALP;
    stGopAttr.stNormalP.s32IPQpDelta = 2;

    /******************************************
     step 1:  Creat Encode Chnl
    ******************************************/
    s32Ret = MY_VENC_Create(&stGopAttr);
    if (HI_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("SAMPLE_COMM_VENC_Creat faild with%#x! \n", s32Ret);
        return HI_FAILURE;
    }

    /******************************************
     step 2:  Start Recv Venc Pictures
    ******************************************/
    stRecvParam.s32RecvPicNum = -1;
    s32Ret = HI_MPI_VENC_StartRecvFrame(VencChn, &stRecvParam);
    if (HI_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("HI_MPI_VENC_StartRecvPic faild with%#x! \n", s32Ret);
        return HI_FAILURE;
    }
    return HI_SUCCESS;
}

static HI_S32 start_venc(VENC_CHN VencChn, HI_U32 u32Profile, HI_BOOL bRcnRefShareBuf, VENC_GOP_ATTR_S *pstGopAttr)
{
    HI_S32 s32Ret;
    VENC_RECV_PIC_PARAM_S stRecvParam;

    /******************************************
     step 1:  Creat Encode Chnl
    ******************************************/
    // s32Ret = create_venc_simple();
    s32Ret = create_venc_normal();
    if (HI_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("SAMPLE_COMM_VENC_Creat faild with%#x! \n", s32Ret);
        return HI_FAILURE;
    }

    return HI_SUCCESS;
}
/******************************************************************************
 * funciton : get stream from each channels and save them
 * 这里不能擅自修改， 容易内存泄漏
 ******************************************************************************/
static HI_VOID *GetVencStreamProc(HI_VOID *p)
{
    HI_S32 i;
    HI_S32 s32ChnTotal;
    VENC_CHN_ATTR_S stVencChnAttr;
    SAMPLE_VENC_GETSTREAM_PARA_S *pstPara;
    HI_S32 maxfd = 0;
    struct timeval TimeoutVal;
    fd_set read_fds;
    HI_U32 u32PictureCnt[VENC_MAX_CHN_NUM] = {0};
    HI_S32 VencFd[VENC_MAX_CHN_NUM];
    HI_CHAR aszFileName[VENC_MAX_CHN_NUM][64];
    FILE *pFile[VENC_MAX_CHN_NUM];
    char szFilePostfix[10];
    VENC_CHN_STATUS_S stStat;
    VENC_STREAM_S stStream;
    HI_S32 s32Ret;
    VENC_CHN VencChn;
    PAYLOAD_TYPE_E enPayLoadType[VENC_MAX_CHN_NUM];
    VENC_STREAM_BUF_INFO_S stStreamBufInfo[VENC_MAX_CHN_NUM];

    // prctl(PR_SET_NAME, "GetVencStream", 0, 0, 0);

    pstPara = (SAMPLE_VENC_GETSTREAM_PARA_S *)p;
    s32ChnTotal = 1;

    SAMPLE_PRT("s32ChnTotal:%d\n", s32ChnTotal);

    for (i = 0; i < s32ChnTotal; i++)
    {
        /* decide the stream file name, and open file to save stream */
        VencChn = pstPara->VeChn[i];
        s32Ret = HI_MPI_VENC_GetChnAttr(VencChn, &stVencChnAttr);
        if (s32Ret != HI_SUCCESS)
        {
            SAMPLE_PRT("HI_MPI_VENC_GetChnAttr chn[%d] failed with %#x!\n",
                       VencChn, s32Ret);
            return NULL;
        }
        enPayLoadType[i] = stVencChnAttr.stVencAttr.enType;

        s32Ret = GetFilePostfix(enPayLoadType[i], szFilePostfix);
        if (s32Ret != HI_SUCCESS)
        {
            SAMPLE_PRT("GetFilePostfix [%d] failed with %#x!\n",
                       stVencChnAttr.stVencAttr.enType, s32Ret);
            return NULL;
        }
        if (PT_JPEG != enPayLoadType[i])
        {
            snprintf(aszFileName[i], 32, "stream_chn%d%s", i, szFilePostfix);

            pFile[i] = fopen(aszFileName[i], "wb");
            if (!pFile[i])
            {
                SAMPLE_PRT("open file[%s] failed!\n",
                           aszFileName[i]);
                return NULL;
            }
        }
        /* Set Venc Fd. */
        VencFd[i] = HI_MPI_VENC_GetFd(i);
        if (VencFd[i] < 0)
        {
            SAMPLE_PRT("HI_MPI_VENC_GetFd failed with %#x!\n",
                       VencFd[i]);
            return NULL;
        }
        if (maxfd <= VencFd[i])
        {
            maxfd = VencFd[i];
        }

        SAMPLE_PRT("VencFd i:%d fd:%d\n", i, VencFd[i]);

        s32Ret = HI_MPI_VENC_GetStreamBufInfo(i, &stStreamBufInfo[i]);
        if (HI_SUCCESS != s32Ret)
        {
            SAMPLE_PRT("HI_MPI_VENC_GetStreamBufInfo failed with %#x!\n", s32Ret);
            return (void *)HI_FAILURE;
        }
    }

    /******************************************
     step 2:  Start to get streams of each channel.
    ******************************************/
    while (HI_TRUE == pstPara->bThreadStart)
    {
        FD_ZERO(&read_fds);
        for (i = 0; i < s32ChnTotal; i++)
        {
            FD_SET(VencFd[i], &read_fds);
        }

        TimeoutVal.tv_sec = 2;
        TimeoutVal.tv_usec = 0;
        s32Ret = select(maxfd + 1, &read_fds, NULL, NULL, &TimeoutVal);
        if (s32Ret < 0)
        {
            SAMPLE_PRT("select failed!\n");
            break;
        }
        else if (s32Ret == 0)
        {
            SAMPLE_PRT("get venc stream time out, exit thread\n");
            continue;
        }
        else
        {
            for (i = 0; i < s32ChnTotal; i++)
            {
                if (FD_ISSET(VencFd[i], &read_fds))
                {
                    /*******************************************************
                     step 2.1 : query how many packs in one-frame stream.
                    *******************************************************/
                    memset(&stStream, 0, sizeof(stStream));

                    s32Ret = HI_MPI_VENC_QueryStatus(i, &stStat);
                    if (HI_SUCCESS != s32Ret)
                    {
                        SAMPLE_PRT("HI_MPI_VENC_QueryStatus chn[%d] failed with %#x!\n", i, s32Ret);
                        break;
                    }

                    /*******************************************************
                    step 2.2 :suggest to check both u32CurPacks and u32LeftStreamFrames at the same time,for example:
                     if(0 == stStat.u32CurPacks || 0 == stStat.u32LeftStreamFrames)
                     {
                        SAMPLE_PRT("NOTE: Current  frame is NULL!\n");
                        continue;
                     }
                    *******************************************************/
                    if (0 == stStat.u32CurPacks)
                    {
                        SAMPLE_PRT("NOTE: Current  frame is NULL!\n");
                        continue;
                    }

                    /*******************************************************
                     step 2.3 : malloc corresponding number of pack nodes.
                    *******************************************************/
                    stStream.pstPack = (VENC_PACK_S *)malloc(sizeof(VENC_PACK_S) * stStat.u32CurPacks);
                    if (NULL == stStream.pstPack)
                    {
                        SAMPLE_PRT("malloc stream pack failed!\n");
                        break;
                    }

                    /*******************************************************
                     step 2.4 : call mpi to get one-frame stream
                    *******************************************************/
                    stStream.u32PackCount = stStat.u32CurPacks;
                    s32Ret = HI_MPI_VENC_GetStream(i, &stStream, HI_TRUE);
                    if (HI_SUCCESS != s32Ret)
                    {
                        free(stStream.pstPack);
                        stStream.pstPack = NULL;
                        SAMPLE_PRT("HI_MPI_VENC_GetStream failed with %#x!\n",
                                   s32Ret);
                        break;
                    }

                    /*******************************************************
                     step 2.5 : save frame to file
                    *******************************************************/

                    if (i == 0)
                    {
                        doCallback(&gStream, i, &stStream);
                    }

                    // save_stream_to_rtsp_list(&stStream);
                    for (HI_S32 packIdx = 0; packIdx < stStream.u32PackCount; packIdx++)
                    {
                        HI_U32 packLength = stStream.pstPack[packIdx].u32Len - stStream.pstPack[packIdx].u32Offset;
                        // SAMPLE_PRT("packLength :%d\n", packLength);
                        // send_h264_raw_udp_package(stStream.pstPack[packIdx].pu8Addr + stStream.pstPack[packIdx].u32Offset, packLength);
                    }

                    /*******************************************************
                     step 2.6 : release stream
                     *******************************************************/
                    s32Ret = HI_MPI_VENC_ReleaseStream(i, &stStream);
                    if (HI_SUCCESS != s32Ret)
                    {
                        SAMPLE_PRT("HI_MPI_VENC_ReleaseStream failed!\n");
                        free(stStream.pstPack);
                        stStream.pstPack = NULL;
                        break;
                    }

                    /*******************************************************
                     step 2.7 : free pack nodes
                    *******************************************************/
                    free(stStream.pstPack);
                    stStream.pstPack = NULL;
                    u32PictureCnt[i]++;
                    if (PT_JPEG == enPayLoadType[i])
                    {
                        fclose(pFile[i]);
                    }
                }
            }
        }
    }
    /*******************************************************
     * step 3 : close save-file
     *******************************************************/
    if (pFile[0] != NULL)
    {
        fclose(pFile[0]);
    }
    return NULL;
}

/******************************************************************************
 * function: H.265e + H264e@720P, H.265 Channel resolution adaptable with sensor
 ******************************************************************************/
int video_start(void)
{
    SIZE_S stSize = {1920, 1080};
    VI_DEV ViDev = 1;
    VI_PIPE viPipe = 0; // 2;
    VI_CHN viChn = 0;
    VPSS_CHN vpssChn = 0;
    VPSS_GRP vpssGrp = 0;
    VENC_CHN vencChn = 0;

    HI_S32 s32Ret = HI_SUCCESS;
    VENC_GOP_MODE_E enGopMode;
    HI_BOOL abChnEnable[VPSS_MAX_CHN_NUM];
    VPSS_CHN_ATTR_S astVpssChnAttr[VPSS_MAX_CHN_NUM];

    VENC_GOP_ATTR_S stGopAttr;

    VB_CONFIG_S stVbConf;
    // PIC_SIZE_E enPicSize = PIC_1080P;
    HI_U32 u32BlkSize;

    // SAMPLE_VI_FRAME_INFO_S stViFrameInfo;
    // VI_USERPIC_ATTR_S stUsrPic;

    pthread_t gs_myVencPid;

    // enDispPicSize = PIC_1080P;
    // enIntfSync = VO_OUTPUT_1080P60;
    // enVoIntfType = VO_INTF_HDMI;

    /*config vi*/
    init_vi_config();

    /*get picture size*/
    SAMPLE_PRT("pic w:%d h:%d \n", stSize.u32Width, stSize.u32Height);

    /*config vb*/
    memset_s(&stVbConf, sizeof(VB_CONFIG_S), 0, sizeof(VB_CONFIG_S));
    stVbConf.u32MaxPoolCnt = 2;

    u32BlkSize = COMMON_GetPicBufferSize(stSize.u32Width, stSize.u32Height, SAMPLE_PIXEL_FORMAT, DATA_BITWIDTH_8, COMPRESS_MODE_SEG, DEFAULT_ALIGN);
    stVbConf.astCommPool[0].u64BlkSize = u32BlkSize;
    stVbConf.astCommPool[0].u32BlkCnt = 10;

    u32BlkSize = VI_GetRawBufferSize(stSize.u32Width, stSize.u32Height, PIXEL_FORMAT_RGB_BAYER_16BPP, COMPRESS_MODE_NONE, DEFAULT_ALIGN);
    stVbConf.astCommPool[1].u64BlkSize = u32BlkSize;
    stVbConf.astCommPool[1].u32BlkCnt = 4;

    SAMPLE_PRT("SAMPLE_COMM_SYS_Init\n");

    s32Ret = SAMPLE_COMM_SYS_Init(&stVbConf);
    if (HI_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("system init failed with %d!\n", s32Ret);
        return s32Ret;
    }

    SAMPLE_PRT("SAMPLE_COMM_VI_StartVi\n");
    /*start vi*/
    s32Ret = SAMPLE_COMM_VI_StartVi(&stViConfig);
    if (HI_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("start vi failed.s32Ret:0x%x !\n", s32Ret);
        return s32Ret;
    }

    SAMPLE_PRT("create_and_start_vpss_grp0\n");
    s32Ret = create_and_start_vpss_grp0(abChnEnable, astVpssChnAttr, 1, stSize, 0);
    if (s32Ret != HI_SUCCESS)
    {
        SAMPLE_PRT("start VPSS fail for %#x!\n", s32Ret);
        return s32Ret;
    }

    // set rc mode gop mode
    enGopMode = VENC_GOPMODE_NORMALP;
    SAMPLE_PRT("get gop attr\n");
    s32Ret = SAMPLE_COMM_VENC_GetGopAttr(enGopMode, &stGopAttr);
    if (HI_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("Venc Get GopAttr for %#x!\n", s32Ret);
        return s32Ret;
    }

    // encode to h.264
    SAMPLE_PRT("start venc\n");
    s32Ret = start_venc(vencChn, HI_TRUE, HI_TRUE, &stGopAttr);
    if (HI_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("start_venc faild with%#x! \n", s32Ret);
        return s32Ret;
    }

    // from vpss chn0 bind to vencChn[0]
    // VPSS_GRP VpssGrp, VPSS_CHN VpssChn, VENC_CHN VencChn
    s32Ret = SAMPLE_COMM_VPSS_Bind_VENC(vpssGrp, vpssChn, vencChn);
    if (HI_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("Venc Get GopAttr failed for %#x!\n", s32Ret);
        return s32Ret;
    }

    SAMPLE_VENC_GETSTREAM_PARA_S s_stMyPara;
    memset(&s_stMyPara, 0, sizeof(s_stMyPara));

    s_stMyPara.bThreadStart = HI_TRUE;
    s_stMyPara.s32Cnt = 1;
    s_stMyPara.VeChn[0] = 0;

    // add_low_delay_mode();
    // pthread_create(&gs_myVencPid, 0, stream_process, (HI_VOID *)&s_stMyPara);
    // 使用子线程, 从编码后的缓存中拿取压缩后的数据帧, 进行写入或者放入rtsp队列
    pthread_create(&gs_myVencPid, 0, GetVencStreamProc, (HI_VOID *)&s_stMyPara);

    /*vi bind vpss VI_PIPE viPipe, VI_CHN viChn, VPSS_GRP VpssGrp */
    s32Ret = SAMPLE_COMM_VI_Bind_VPSS(viPipe, viChn, vpssGrp);
    if (HI_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("vo bind vpss failed. s32Ret: 0x%x !\n", s32Ret);
        return s32Ret;
    }

    SAMPLE_PRT("VI->VPSS->VENC init finished\n");

    return s32Ret;
}

#if 0
int video_start_bak(void)
{
    HI_S32 i;
    HI_S32 s32Ret;
    SIZE_S stSize[2];
    PIC_SIZE_E enSize[2] = {PIC_720P, PIC_D1_PAL};
    HI_S32 s32ChnNum = 2;
    VENC_CHN VencChn[2] = {0, 1};
    HI_U32 u32Profile[2] = {0, 0};
    PAYLOAD_TYPE_E enPayLoad[2] = {PT_H264, PT_H264};
    VENC_GOP_MODE_E enGopMode;
    VENC_GOP_ATTR_S stGopAttr;
    SAMPLE_RC_E enRcMode;
    HI_BOOL bRcnRefShareBuf = HI_TRUE;

    VI_DEV ViDev = 1;
    VI_PIPE ViPipe = 0; // 2;
    VI_CHN ViChn = 0;
    SAMPLE_VI_CONFIG_S stViConfig;

    VPSS_GRP VpssGrp = 0;
    VPSS_CHN VpssChn[3] = {0, 1, 2};
    HI_BOOL abChnEnable[VPSS_MAX_PHY_CHN_NUM] = {1, 1, 1};

    HI_U32 u32SupplementConfig = HI_FALSE;
    for (i = 0; i < s32ChnNum; i++)
    {
        s32Ret = SAMPLE_COMM_SYS_GetPicSize(enSize[i], &stSize[i]);
        if (HI_SUCCESS != s32Ret)
        {
            SAMPLE_PRT("SAMPLE_COMM_SYS_GetPicSize failed!\n");
            return s32Ret;
        }
    }
    for (i = 0; i < s32ChnNum; i++)
    {
        SAMPLE_PRT("w:%dh:%d\n", stSize[i].u32Width, stSize[i].u32Height);
    }

    SAMPLE_COMM_VI_GetSensorInfo(&stViConfig);
    if (SAMPLE_SNS_TYPE_BUTT == stViConfig.astViInfo[0].stSnsInfo.enSnsType)
    {
        SAMPLE_PRT("Not set SENSOR%d_TYPE !\n", 0);
        return HI_FAILURE;
    }

    s32Ret = SAMPLE_VENC_CheckSensor(stViConfig.astViInfo[0].stSnsInfo.enSnsType, stSize[0]);
    if (s32Ret != HI_SUCCESS)
    {
        SAMPLE_PRT("SAMPLE_VENC_ModifyResolution !\n");
        s32Ret = SAMPLE_VENC_ModifyResolution(stViConfig.astViInfo[0].stSnsInfo.enSnsType, &enSize[0], &stSize[0]);
        if (s32Ret != HI_SUCCESS)
        {
            return HI_FAILURE;
        }
    }

    stViConfig.s32WorkingViNum = 1;
    stViConfig.astViInfo[0].stDevInfo.ViDev = ViDev;
    stViConfig.astViInfo[0].stPipeInfo.aPipe[0] = ViPipe;
    stViConfig.astViInfo[0].stChnInfo.ViChn = ViChn;
    stViConfig.astViInfo[0].stChnInfo.enDynamicRange = DYNAMIC_RANGE_SDR8;
    stViConfig.astViInfo[0].stChnInfo.enPixFormat = PIXEL_FORMAT_YVU_SEMIPLANAR_420;
    s32Ret = SAMPLE_VENC_VI_Init(&stViConfig, HI_FALSE, u32SupplementConfig);
    if (s32Ret != HI_SUCCESS)
    {
        SAMPLE_PRT("Init VI err for %#x!\n", s32Ret);
        return HI_FAILURE;
    }

    s32Ret = SAMPLE_VENC_VPSS_Init(VpssGrp, abChnEnable, DYNAMIC_RANGE_SDR8, PIXEL_FORMAT_YVU_SEMIPLANAR_420, stSize, stViConfig.astViInfo[0].stSnsInfo.enSnsType);
    if (HI_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("Init VPSS err for %#x!\n", s32Ret);
        goto EXIT_VI_STOP;
    }

    s32Ret = SAMPLE_COMM_VI_Bind_VPSS(ViPipe, ViChn, VpssGrp);
    if (s32Ret != HI_SUCCESS)
    {
        SAMPLE_PRT("VI Bind VPSS err for %#x!\n", s32Ret);
        goto EXIT_VPSS_STOP;
    }

    /******************************************
     start stream venc
     ******************************************/
    enRcMode = SAMPLE_RC_CBR;         // SAMPLE_VENC_GetRcMode();
    enGopMode = VENC_GOPMODE_NORMALP; // SAMPLE_VENC_GetGopMode();

    s32Ret = SAMPLE_COMM_VENC_GetGopAttr(enGopMode, &stGopAttr);
    if (HI_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("Venc Get GopAttr for %#x!\n", s32Ret);
        goto EXIT_VI_VPSS_UNBIND;
    }

    /***encode h.265 **/
    s32Ret = SAMPLE_COMM_VENC_Start(VencChn[0], enPayLoad[0], enSize[0], enRcMode, u32Profile[0], bRcnRefShareBuf, &stGopAttr);
    if (HI_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("Venc Start failed for %#x!\n", s32Ret);
        goto EXIT_VI_VPSS_UNBIND;
    }

    s32Ret = SAMPLE_COMM_VPSS_Bind_VENC(VpssGrp, VpssChn[0], VencChn[0]);
    if (HI_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("Venc Get GopAttr failed for %#x!\n", s32Ret);
        goto EXIT_VENC_H265_STOP;
    }

    /***encode h.264 **/
    s32Ret = SAMPLE_COMM_VENC_Start(VencChn[1], enPayLoad[1], enSize[1], enRcMode, u32Profile[1], bRcnRefShareBuf, &stGopAttr);
    if (HI_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("Venc Start failed for %#x!\n", s32Ret);
        goto EXIT_VENC_H265_UnBind;
    }

    s32Ret = SAMPLE_COMM_VPSS_Bind_VENC(VpssGrp, VpssChn[1], VencChn[1]);
    if (HI_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("Venc bind Vpss failed for %#x!\n", s32Ret);
        goto EXIT_VENC_H264_STOP;
    }

    /******************************************
     stream save process
    ******************************************/
    s32Ret = SAMPLE_COMM_VENC_StartGetStream(VencChn, s32ChnNum);
    if (HI_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("Start Venc failed!\n");
        goto EXIT_VENC_H264_UnBind;
    }

    // printf("please press twice ENTER to exit this sample\n");
    // getchar();
    // getchar();

    // printf("please press twice ENTER to exit this sample\n");
    return 0;

    /******************************************
     exit process
    ******************************************/
    SAMPLE_COMM_VENC_StopGetStream();

EXIT_VENC_H264_UnBind:
    SAMPLE_COMM_VPSS_UnBind_VENC(VpssGrp, VpssChn[1], VencChn[1]);
EXIT_VENC_H264_STOP:
    SAMPLE_COMM_VENC_Stop(VencChn[1]);
EXIT_VENC_H265_UnBind:
    SAMPLE_COMM_VPSS_UnBind_VENC(VpssGrp, VpssChn[0], VencChn[0]);
EXIT_VENC_H265_STOP:
    SAMPLE_COMM_VENC_Stop(VencChn[0]);
EXIT_VI_VPSS_UNBIND:
    SAMPLE_COMM_VI_UnBind_VPSS(ViPipe, ViChn, VpssGrp);
EXIT_VPSS_STOP:
    SAMPLE_COMM_VPSS_Stop(VpssGrp, abChnEnable);
EXIT_VI_STOP:
    SAMPLE_COMM_VI_StopVi(&stViConfig);
    SAMPLE_COMM_SYS_Exit();

    return s32Ret;
}
#endif

int video_stop()
{
    SAMPLE_VI_CONFIG_S stViConfig;
    VI_DEV ViDev = 1;
    VI_PIPE ViPipe = 2;
    VI_CHN ViChn = 0;
    VENC_CHN VencChn[2] = {0};
    HI_U32 u32Profile[2] = {0, 0};

    VPSS_GRP VpssGrp = 0;
    VPSS_CHN VpssChn[3] = {0, 1, 2};
    HI_BOOL abChnEnable[VPSS_MAX_PHY_CHN_NUM] = {1, 1, 1};

    SAMPLE_COMM_VI_GetSensorInfo(&stViConfig);

    stViConfig.s32WorkingViNum = 1;
    stViConfig.astViInfo[0].stDevInfo.ViDev = ViDev;
    stViConfig.astViInfo[0].stPipeInfo.aPipe[0] = ViPipe;
    stViConfig.astViInfo[0].stChnInfo.ViChn = ViChn;
    stViConfig.astViInfo[0].stChnInfo.enDynamicRange = DYNAMIC_RANGE_SDR8;
    stViConfig.astViInfo[0].stChnInfo.enPixFormat = PIXEL_FORMAT_YVU_SEMIPLANAR_420;

    SAMPLE_COMM_VENC_StopGetStream();
    SAMPLE_COMM_VPSS_UnBind_VENC(VpssGrp, VpssChn[1], VencChn[1]);
    SAMPLE_COMM_VENC_Stop(VencChn[1]);
    SAMPLE_COMM_VPSS_UnBind_VENC(VpssGrp, VpssChn[0], VencChn[0]);
    SAMPLE_COMM_VENC_Stop(VencChn[0]);
    SAMPLE_COMM_VI_UnBind_VPSS(ViPipe, ViChn, VpssGrp);
    SAMPLE_COMM_VPSS_Stop(VpssGrp, abChnEnable);
    SAMPLE_COMM_VI_StopVi(&stViConfig);
    SAMPLE_COMM_SYS_Exit();
    return 0;
}

int video_get_frame(VIDEO_FRAME_INFO_S *pstVideoFrame, int millisec)
{
    HI_S32 ret = HI_MPI_VPSS_GetChnFrame(0, 2, pstVideoFrame, millisec);
    if (ret != HI_SUCCESS)
    {
        printf("ERROR: HI_MPI_VPSS_GetChnFrame failed:%x\n", ret);
        return -1;
    }
    return 0;
}

int video_release_frame(VIDEO_FRAME_INFO_S *pstVideoFrame)
{
    HI_S32 ret = HI_MPI_VPSS_ReleaseChnFrame(0, 2, pstVideoFrame);
    if (ret != HI_SUCCESS)
    {
        printf("HI_MPI_VPSS_ReleaseChnFrame failed:%x\n", ret);
        return -1;
    }
    return 0;
}
