/*
 * sdk.h
 *
 * ============================================================================
 * Copyright (c) eyecloud	Inc	2020
 * Created:	dukezuo@eyecloud.tech								 2020.3.6
 * Description:	C Interface for read/write with OpenNcc device
 * version:1.0.0
 * History: 2020.3.3 created
 *  1.0.1: 
 *  1) add read 3d data 
 *  2) add sdk_uninit() 
 *  3) read usb data timeout or error,return at once in sdk 
 * ============================================================================
 */
 
#ifndef  __PC_SDK_H__
#define  __PC_SDK_H__
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef enum 
{
    IMG_FORMAT_GRAY = 0,
    IMG_FORMAT_I420,        //YUV420 planar
    IMG_FORMAT_I422,        //YUV422 planar
    IMG_FORMAT_I444,        //YUV444 planar
    IMG_FORMAT_YUV444,      //YUV444 packed
    IMG_FORMAT_RGB = 5,     //RGB packed
    IMG_FORMAT_RGB_PLANAR,  //RGB planar
    IMG_FORMAT_BGR,         //BGR packed
    IMG_FORMAT_BGR_PLANAR   //BGR planar
}IMAGE_FORMAT;

typedef enum
{
    ENCODE_H264_MODE,
    ENCODE_H265_MODE,
}encodeMode;
typedef enum frameTypes
{
     YUV422i,   // interleaved 8 bit
     YUV444p,   // planar 4:4:4 format
     YUV420p,   // planar 4:2:0 format
     YUV422p,   // planar 8 bit
     YUV400p,   // 8-bit greyscale
     RGBA8888,  // RGBA interleaved stored in 32 bit word
     RGB888,    // Planar 8 bit RGB data
     LUT2,      // 1 bit  per pixel, Lookup table (used for graphics layers)
     LUT4,      // 2 bits per pixel, Lookup table (used for graphics layers)
     LUT16,     // 4 bits per pixel, Lookup table (used for graphics layers)
     RAW16,     // save any raw type (8, 10, 12bit) on 16 bits
     RAW14,     // 14bit value in 16bit storage
     RAW12,     // 12bit value in 16bit storage
     RAW10,     // 10bit value in 16bit storage
     RAW8,
     PACK10,    // SIPP 10bit packed format
     PACK12,    // SIPP 12bit packed format
     YUV444i,
     NV12,
     NV21,
     BITSTREAM, // used for video encoder bitstream
     HDR,
	   H26X,
	   JPEG,

	   METEDATA,
	   MONO,
	   DEPTH,
     NONE
}frameType;

#define  MAX_LABEL_SIZE            (10000)         /* 目标检测最大支持的label数据 */
#define  MAX_EXTINPUT_SIZE      (64*1024)      /* 多输入的第二级输入最大支持长度 */
#define  MAX_OBJ                        (100)            /* 二级模型最大处理100个目标 */
/* mode process setting */
typedef struct{
    int imageWidth;           /* 图像宽 */
    int imageHeight;          /* 图像高 */
    int startX;               /* 图像处理起点坐标 */
    int startY;
    int endX;                 /* 图像处理终点坐标 */
    int endY;
    int inputDimWidth;        /* 缩放后模型输入宽 */
    int inputDimHeight;       /* 缩放后模型输入高 */
    IMAGE_FORMAT inputFormat; /* 模型输入格式，只支持RGB/RGB_PLANAR/BGR/BGR_PLANAR */
    float meanValue[3];       /* 缩放后的数据二次预处理 如果inputFormat为RGB:
                              　   R = (R-meanValue[0])/stdValue
   　                              G = (G-meanValue[0])/stdValue
          　                       B = (B-meanValue[0])/stdValue */
    float stdValue;
    int   isOutputYUV;
    int   isOutputH26X;
    int   isOutputJPEG;
    encodeMode mode;                 /* H264/H265 */

    char extInputs[MAX_EXTINPUT_SIZE];       /* 模型二级输入     */
    int   modelCascade;                                  /* 级联下一级模型  */
    int   inferenceACC;                                    /* 推理加速，全局开启多个神经网络核心并行计算 */
} CameraInfo;

/* 第一级模型预处理参数 */
typedef   CameraInfo NetworkPar0;
/* 第二级模型预处理参数 */
typedef struct{
    int startXAdj;                                   /* 目标检测起点坐标微调 */
    int startYAdj;
    int endXAdj;                                    /* 目标检测终点坐标微调 */
    int endYAdj;
   char  labelMask[MAX_LABEL_SIZE];  /* 需要处理的label掩码，如果对应位置至1才会处理 */
    float minConf;                                 /* 检测目标的置信度大于该值才会处理 */

    int inputDimWidth;        /* 缩放后模型输入宽 */
    int inputDimHeight;       /* 缩放后模型输入高 */
    IMAGE_FORMAT inputFormat; /* 模型输入格式，只支持RGB/RGB_PLANAR/BGR/BGR_PLANAR */
    float meanValue[3];       /* 缩放后的数据二次预处理 如果inputFormat为RGB:
                              　   R = (R-meanValue[0])/stdValue
   　                              G = (G-meanValue[0])/stdValue
          　                       B = (B-meanValue[0])/stdValue */
    float stdValue;
    char extInputs[MAX_EXTINPUT_SIZE];          /* 模型二级输入  */
    int   modelCascade;                 /* 级联下一级模型  */
} NetworkPar1;

typedef struct
{
     frameType      type;
     unsigned int  seqNo;
     unsigned int  size;
     unsigned int  res[13];
}frameSpecOut;

typedef void (*vscRecvCb)(void* param,void *data, int len);

//load firmware and vsc init
int load_fw(const char* firmware );

//download image data to ai caluate
int down_image_data(const char *data, int len);

//sdk ini
int sdk_init( vscRecvCb cb,void* param, char *blob_path, void *par, int par_Len, char *blob2_path, void *par2, int par2_Len);

void sdk_uninit(void);

//add by duke 2020.2
//return 20:usb2.0  11:usb1.1   30:usb3.0
int get_usb_version(void);

//blocked:1 read until a packet received 0:if no packet ,return at once
int read_yuv_data(char* pbuf,int * size,int blocked);

int read_26x_data(char* pbuf,int * size,int blocked);

int read_jpg_data(char* pbuf,int * size,int blocked);

int read_meta_data(char* pbuf,int * size,int blocked);

int read_ir_data(char* pbuf,int * size,int blocked);

int read_depth_data(char* pbuf,int * size,int blocked);

int down_image_data(const char *data, int len);
//add end 
#ifdef __cplusplus
}
#endif
#endif


