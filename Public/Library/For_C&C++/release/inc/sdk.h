/*
 * sdk.h
 *
 * ============================================================================
 * Copyright (c) eyecloud	Inc	2020
 * Created:	dukezuo@eyecloud.tech								 2020.3.6
 * Description:	C Interface for read/write with OpenNcc device
 * version:1.0.0
 * History: 2020.3.3 created
 * ============================================================================
 */
 
#ifndef  __PC_SDK_H__
#define  __PC_SDK_H__
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

//defien video frame type  for camera_video_out
#define   YUV420p  2
#define   H26X     22
#define   JPEG     23
#define   METEDATA 24
#define   MONO     25
#define   DEPTH    26

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

/* mode process setting */
typedef struct{
	int imageWidth;           
	int imageHeight;        
	int startX;              
	int startY;
	int endX;                
	int endY;
	int inputDimWidth;        /* 缩放后模型输入宽 */
	int inputDimHeight;       /* 缩放后模型输入高 */
	IMAGE_FORMAT inputFormat; /* 模型输入格式，只支持RGB/RGB_PLANAR/BGR/BGR_PLANAR */
	float meanValue[3];       /* 缩放后的数据二次预处玿如果inputFormat为RGB:
	                          　   R = (R-meanValue[0])/stdValue
   　                              G = (G-meanValue[0])/stdValue
          　                       B = (B-meanValue[0])/stdValue */
	float stdValue;
	int   isOutputYUV;
	int   isOutputH26X;
	int   isOutputJPEG;
 	encodeMode mode;            /* H264/H265 */
} CameraInfo;

typedef struct
{
     int      type;
     unsigned int  seqNo;
     unsigned int  size;
     unsigned int  res[13];
}frameSpecOut;

int GetYuvData(char *pbuf, int slen);
int GetMetaData(char *pbuf, int slen);
int GetH26xData(char *pbuf, int slen);
int GetJpegData(char *buf, int slen);
void SetMeanValue(CameraInfo* cam_info,float val1,float val2,float val3);
#ifdef __cplusplus
extern "C" {
#endif
typedef void (*vscRecvCb)(void* param,void *data, int len);

void get_sdk_version( char* version);

//load firmware and vsc init
int load_fw(const char* bootExe, const char* firmware );

//sdk ini
int sdk_init( vscRecvCb cb,void* param, char *blob_path, CameraInfo * cam, int cam_Len);

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

//download image data to ai caluate
int down_image_data(const char *data, int len);
//add end 

int get_meta_size();

int get_err_no();
#ifdef __cplusplus
}
#endif
#endif


