#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <mutex>
#include "pthread.h"
#include "sdk.h"
#include "cameraCtrl.h"
#include "Fp16Convert.h"

#ifndef _WINDOWS
#include <sys/time.h>
#else
#include <windows.h>	
extern "C" int gettimeofday(struct timeval *tp, void *tzp);
#pragma warning(disable:4996)
#endif

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/imgproc/types_c.h>
#include <opencv2/highgui/highgui_c.h>
using namespace std;
using namespace cv;

std::mutex cv_lock;
	
//#define CALL_BACK 
//#define READ_DATA
//#define DL_IMG
//#define   TWO_NET_MODEL 
#define  TWO_INPUT_MODEL 

/* 1. 物体分类demo程序 */
//#define  DEMO_CLS   1
/* 2. 人脸检测demo程序 */
#define  DEMO_FACE_DETECTION  1

#define  OPENCV_SHOW_SCALE    (0.8)    /* 显示缩放系数 */
extern "C"  void os_sleep(int ms);
static char metadata[1024*1024];
static char yuv420p[sizeof(frameSpecOut)+1024*1024*10];
static volatile int show_update = 0;

extern void  obj_show_img_func(void *data, int w, int h, float scale, char *name, int nn_fov_show, CameraInfo *nnparm, char *nnret,float min_score);
extern void  fd_show_img_func(void *data, int w, int h, float scale, char *name, int nn_fov_show, CameraInfo *nnparm, char *nnret);
extern void  vehicle_license_plate_detection_barrier(void *data, int w, int h, float scale, char *name, int nn_fov_show, Network1Par *nnParm1,Network2Par *nnParm2, char *nnret, float min_score);
///////////////////////////////////////////////////////////////////////////////////
static int fd = 0;
bool g_run = true;

static char cali_file[] = "./blob/calibration.bin";

#ifdef CALL_BACK   //read data by call back
static CameraInfo cam_info =
{
	-1,  //imageWidth
	-1,  //imageHeight                  
    -1,  //startX
	-1,  //startY                 
	-1,   //endX
	-1,   //endY                  
	300,  //inputDimWidth
	300,   //inputDimHeight                   /* <dim>300</dim>  <dim>300</dim> */
	IMG_FORMAT_BGR_PLANAR,      //IMAGE_FORMAT   
	0,                    //meanValue
	0,
	0 ,
	1,                         //stdValue
    1,                           /*打开YUV420输出功能*/
    1,                           /*打开H26X编码功能*/
    1,                           /*打开MJPEG编码功能*/
	ENCODE_H264_MODE,            /* 使用H264编码格式 */
};

static void vscRead(void* param,void* data, int len)
{
    static  struct timeval tv1, tv2;
    static  int res_cnt = 0;

    /* 获取视频流数据头 */
    frameSpecOut *out = (frameSpecOut *)data;
  //  if(JPEG == out->type)
   //     printf("Meta:type:%d,size:%d,seqNo:%d\n", out->type, out->size, out->seqNo);

    switch (out->type)
    {
        case( YUV420p) :
        {
            /* 获取视频流数拿YUV420P */
            char *yuv_data = (char *) data + sizeof(frameSpecOut);
            #if (DEMO_CLS!=0 || DEMO_FACE_DETECTION!=0)
            memcpy(yuv420p, yuv_data, out->size<sizeof(yuv420p)?out->size:sizeof(yuv420p));
            show_update = 1;
            #endif
        }
            break;

        case (H26X ):
        {
            /* 获取H65视频流数懿
            char *h265_data = (char *) data + sizeof(frameSpecOut);
            if(fd ==0)
            {
                if (access(myfifo, F_OK) == -1)
                {
                    int res = mkfifo(myfifo, 0777);
                    if (res != 0)
                    {
                        fprintf(stderr, "Could not create fifo %s\n", myfifo);
                        exit(EXIT_FAILURE);
                    }
                }
                fd = open(myfifo, O_RDWR);
            }
            if(fd)
                write(fd, h265_data, out->size);
				*/
        }
            break;

        case (METEDATA) :
        {
//            printf("seqNo:%d, CNN runtime=%dms\n",out->seqNo, out->res[0]);
            /* 获取NCS计算结果, 数据格式是fp16 */
            char *nn_ret = (char *) data + sizeof(frameSpecOut);
            /* 算法性能统计 */
            if (res_cnt == 0)
            {
                gettimeofday(&tv1, NULL);
            }
            res_cnt++;
            if (res_cnt == 100)
            {
                gettimeofday(&tv2, NULL);
                long int d = tv2.tv_sec * 1000000 + tv2.tv_usec - tv1.tv_sec * 1000000 - tv1.tv_usec;
                printf("NCS2_FPS %.3f param:%s\n", (float) (100 * 1000000) / d,(char*)param);
                res_cnt = 0;
            }

        #if (DEMO_CLS!=0 || DEMO_FACE_DETECTION!=0)
            memcpy(metadata, nn_ret, out->size<sizeof(metadata)?out->size:sizeof(metadata));
        #endif
        }
        break;
        case (JPEG ):
        {
            /* 获取JPEG视频流数*/
            char *jpeg_data = (char *) data + sizeof(frameSpecOut);

          if(out->seqNo %100 ==0)
          {
              char src[64];
              FILE *fp;
              sprintf(src, "seq_%d.jpeg", out->seqNo);
              if((fp=fopen(src,"wb")) != NULL)
              {
                  fwrite(jpeg_data, out->size, 1, fp);
                  fclose(fp);
                  printf("save jpeg file %s \n",src);
              }
          }
        }
        break;

        default:
            break;
    }
}

int main(void)
{
	int ret;
	 memset(metadata, 0, sizeof(metadata));

    //1. 加载固件
    load_fw("./moviUsbBoot","./fw/flicRefApp.mvcmd");//AUTO_LOAD_FW
		
		printf("usb sersion:%d \n",get_usb_version());
		
 /* 2. 获取camera参数 */
    SensorModesConfig cameraCfg;
    SensorModesList   list;
    camera_control_get_features(&list);
    printf("list num:%d\n",list.num);
    for(int i=0;i<list.num;i++)
    {
        SensorModesConfig features;
        memcpy(&features, &list.mode[i], sizeof(features));
        printf("[%d/%d]camera: %s, %dX%d@%dfps, AFmode:%d, maxEXP:%dus,gain[%d, %d]\n",i,list.num,
                features.moduleName, features.camWidth, features.camHeight, features.camFps,
                features.AFmode, features.maxEXP, features.minGain, features.maxGain);
    }

    int sensorModeId = 1; //1080P模式
//    int sensorModeId = 1; //4K模式
    camera_select_sensor(sensorModeId);
    memcpy(&cameraCfg, &list.mode[sensorModeId], sizeof(cameraCfg));//select camera info

    // 3. 算法有效区域初始冿  cam_info.imageWidth  = cameraCfg.camWidth;
    cam_info.imageWidth  = cameraCfg.camWidth;
    cam_info.imageHeight = cameraCfg.camHeight;
    cam_info.startX      = 0;
    cam_info.startY      = 0;
    cam_info.endX        = cameraCfg.camWidth;
    cam_info.endY        = cameraCfg.camHeight;

	// 4. sdk初始冿 
    //int ret = sdk_init(vscRead, (void*)"12345",cali_file, NULL, NULL);
    ret = sdk_init(vscRead, (void*)"12345", (char*) "./blob/face-detection-retail-0004-fp16.blob", (void*)&cam_info, sizeof(cam_info));
		printf("xlink_init %d\n", ret);

    //5. 输出配置
 //    camera_yuv420_out(YUV420_OUT_CONTINUOUS);
 //   camera_h26x_out(H26X_OUT_ENABLE);
    camera_mjpeg_out(MJPEG_OUT_CONTINUOUS);
	/***************************demo test start********************************/
	while(1)
	{
		char src[64];
		float scale = OPENCV_SHOW_SCALE; /* 显示缩放 */
		if(show_update ==0)
		{
			os_sleep(1);
			continue;
		}

		sprintf(src, "fd_demo_video_%dx%d@%dfps(scale:%d%%)", cameraCfg.camWidth, cameraCfg.camHeight, cameraCfg.camFps,(int)(100*scale));
		fd_show_img_func(yuv420p , cameraCfg.camWidth, cameraCfg.camHeight, scale,src, true, &cam_info, (char*)metadata);
		show_update = 0;
	}
	/***************************test end********************************/
}
#elif defined READ_DATA
int main(void)
{
	int ret;
	memset(metadata, 0, sizeof(metadata));
	CameraInfo cam_info =
	{
		-1,  //imageWidth
		-1,  //imageHeight                  
	    -1,  //startX
		-1,  //startY                 
		-1,   //endX
		-1,   //endY                  
		0,  //inputDimWidth
		0,   //inputDimHeight                   /* <dim>300</dim>  <dim>300</dim> */
		IMG_FORMAT_BGR_PLANAR,      //IMAGE_FORMAT   
		0,                    //meanValue
		0,
		0 ,
		1,                         //stdValue
	    1,                           /*打开YUV420输出功能*/
	    0,                           /*打开H26X编码功能*/
	    0,                           /*打开MJPEG编码功能*/
		ENCODE_H264_MODE,            /* 使用H264编码格式 */
	};

  //1. 加载固件
  // ret= load_fw(AUTO_LOAD_FW);//AUTO_LOAD_FW
   ret=load_fw("./moviUsbBoot","./fw/flicRefApp.mvcmd");
//   ret=load_fw("./moviUsbBoot",NULL);
   if (ret<0)
   {
   	printf("lowd firmware error! return \n");
		return -1;
   }
   
   char version[100];
   get_sdk_version(version);
   printf("sdk sersion:%d %s \n",get_usb_version(),version);
#if 1
 		SensorModesConfig mode[MAX_MODE_SIZE];
		int num=camera_control_get_sensorinfo(mode,MAX_MODE_SIZE);
	  for(int i=0;i<num;i++)
    {
        printf("[%d/%d]camera: %s, %dX%d@%dfps, AFmode:%d, maxEXP:%dus,gain[%d, %d]\n",i,num,
                mode[i].moduleName, mode[i].camWidth, mode[i].camHeight, mode[i].camFps,
                mode[i].AFmode, mode[i].maxEXP, mode[i].minGain, mode[i].maxGain);
    }	    

    int sensorModeId = 0; //1080P模式
   // int sensorModeId = 1; //4K模式
    camera_select_sensor(sensorModeId);
   // memcpy(&cameraCfg, &list.mode[sensorModeId], sizeof(cameraCfg));//select camera info

  // 3. 算法有效区域初始冿 
  cam_info.imageWidth  = mode[sensorModeId].camWidth;
  cam_info.imageHeight = mode[sensorModeId].camHeight;
  cam_info.startX      = 0;
  cam_info.startY      = 0;
  cam_info.endX        = mode[sensorModeId].camWidth;
  cam_info.endY        = mode[sensorModeId].camHeight;
#endif

// 4. sdk初始冿 
  ret = sdk_init(NULL, NULL, "./blob/0100_w277_OV2019R1.blob", &cam_info, sizeof(cam_info));
	
  printf("xlink_init %d,mata size=%d\n", ret,get_meta_size());
	if (ret<0) return -1;
  //5. 输出配置
  camera_video_out(YUV420p,VIDEO_OUT_CONTINUOUS);
  
	pthread_t ctrlThd,irthd,depthd;
//	pthread_create(&ctrlThd, NULL,ControlThread,NULL);  
//	pthread_create(&irthd, NULL,Irthread,NULL);  
//	pthread_create(&depthd, NULL,Depthread,NULL);  
	//FILE* file=fopen("test.h264","w");

	while(g_run)
	{
		char src[64];
		float scale = OPENCV_SHOW_SCALE; /* 显示缩放 */
		int size= sizeof(yuv420p);
		//if (read_yuv_data(yuv420p,&size,1)<0)
		//if (read_jpg_data(yuv420p,&size,1)<0)
		//if ((GetYuvData(yuv420p, size)<0))
		ret=read_yuv_data(yuv420p,&size,1);
		if	(ret==USB_ERROR_NO_DEVICE)
		{
			g_run=false;
			//break;
			exit(-1);
		}
		
		//size = sizeof(metadata);
	//	read_meta_data(metadata,&size,0);	
		ret=GetMetaData(metadata,size);
	//	printf("GetMetaData size=%d \n",ret);
	//	os_sleep(1000);
		//sprintf(src, "fd_demo_video_%dx%d@%dfps(scale:%d%%)", mode[sensorModeId].camWidth, mode[sensorModeId].camHeight, mode[sensorModeId].camFps,(int)(100*scale));
		#if   0
			cv::Mat yuvImg;
		//yuvImg.create(mode[sensorModeId].camHeight * 3/2, mode[sensorModeId].camWidth, CV_8UC1);
		yuvImg.create(cam_info.imageHeight * 3/2, cam_info.imageWidth, CV_8UC1);	
		cv::Mat rgb_img;
	  
		/* YUV420P-->RGB */
		yuvImg.data = (unsigned char*)yuv420p+sizeof(frameSpecOut);
		cv::cvtColor(yuvImg, rgb_img, CV_YUV2BGR_I420);
	//	face_detect_handle(rgb_img,&cam_info, (char*)metadata+sizeof(frameSpecOut));
		
		Mat showImage;
		/* 缩放显示 */
		resize(rgb_img,showImage,Size(rgb_img.cols*scale,rgb_img.rows*scale),0,0,INTER_LINEAR);
		{
			std::lock_guard<std::mutex> lock(cv_lock);
			cv::imshow("rgb", showImage);
			if (cv::waitKey(1)==27) break;
		}
		#endif
		fd_show_img_func(yuv420p+sizeof(frameSpecOut) , cam_info.imageWidth, cam_info.imageHeight, scale,src, true, &cam_info, (char*)metadata+sizeof(frameSpecOut)+OUTPUT_INDEX_SIZE);
	}    		 
	printf("exit test main....\n");
	sdk_uninit();
	//fclose(file);
	return 0;
}
#elif defined DL_IMG
int main(void)
{
	int ret;
	memset(metadata, 0, sizeof(metadata));
	memset(yuv420p, 0x80, sizeof(yuv420p));
	float min_score=0.3;
	CameraInfo cam_info =
	{
		-1,  //imageWidth
		-1,  //imageHeight                  
	  -1,  //startX
		-1,  //startY                 
		-1,   //endX
		-1,   //endY                  
		-1,  //inputDimWidth
		-1,   //inputDimHeight                   /* <dim>300</dim>  <dim>300</dim> */
		IMG_FORMAT_BGR_PLANAR,      //IMAGE_FORMAT   
		0,                    //meanValue
		0,
		0 ,
		1                         //stdValue
	};

    char *imagepath ="1234.jpg";
    char *blob = "./blob/person-vehicle-bike-detection-crossroad-0078-fp16.blob";
    
  //1. 加载固件
   ret=load_fw("./moviUsbBoot","./fw/downImgApp_SAMSUNG.mvcmd");
   if (ret<0)
   {
   	printf("load_fw error! return \n");
		return -1;
   }
  printf("usb sersion:%d \n",get_usb_version());
	
 /* 2. 获取image参数 */
    cv::Mat cv_img = cv::imread(imagepath, 3);
    if (cv_img.empty())
    {
      fprintf(stderr, "cv::imread %s failed\n", imagepath);
      return -1;
    }
    printf("imagepath:%s\n", imagepath);

    printf("cv_img.cols:%d\n", cv_img.cols);
    printf("cv_img.rows:%d\n", cv_img.rows);   
    
    // 3. 算法有效区域初始化
    cam_info.imageWidth  = cv_img.cols;
    cam_info.imageHeight = cv_img.rows;
    cam_info.startX      = 0;
    cam_info.startY      = 0;
    cam_info.endX        = cv_img.cols;
    cam_info.endY        = cv_img.rows;

	// 4. 模型初始化
    cam_info.inputDimWidth  = 1024;
    cam_info.inputDimHeight = 1024;
    
// 4. sdk初始冿 
 // ret = sdk_init(NULL, NULL,cali_file, NULL, NULL);
  ret = sdk_init(NULL, NULL, blob, &cam_info, sizeof(cam_info));
	
  printf("xlink_init %d\n", ret);
	if (ret<0)  return -1;
		
	//5 load a picture for test
    cv::Mat cv_yuv;
    cv::cvtColor(cv_img, cv_yuv, COLOR_BGR2YUV_I420);
    memcpy(&yuv420p[sizeof(frameSpecOut)], cv_yuv.data, (cv_img.rows)*(cv_img.cols)*3/2);
    frameSpecOut *p = (frameSpecOut *)yuv420p;
    //填写数据头信息
    p->type  = YUV420p;
    p->seqNo = 1234;
    p->size  = (cv_img.rows)*(cv_img.cols)*3/2;

    printf("type, seqNo size:%d %d %d  (%dX%d)\n",p->type,p->seqNo ,p->size,cv_img.cols,cv_img.rows);

    //加载测试图片，hdr + yuv420p
   ret=down_image_data(yuv420p, sizeof(frameSpecOut) + (cv_img.rows)*(cv_img.cols)*3/2);
	 if (ret==0)
	 	printf("download image %s ok!",imagepath);
	 else
	 {
	 		printf("download image %s error!",imagepath);
	 		g_run=false;
	 }	 
	while(g_run)
	{
		char src[64];
		float scale = OPENCV_SHOW_SCALE; /* 显示缩放 */

		int size = sizeof(metadata);
		if (read_meta_data(metadata,&size,0)<0) //read ai result
		{
			 os_sleep(200);
			 continue;
		}
		
		 if(cv_img.cols*cv_img.rows < 1000*1000)
	      		  scale = 1;
		//min_score = 0.3;
		sprintf(src, "demo_video_%dx%d(scale:%d%%)", cv_img.cols,cv_img.rows, (int)(100*scale));
		obj_show_img_func(&yuv420p[sizeof(frameSpecOut)] , cv_img.cols,cv_img.rows, scale,src, 0, &cam_info, (char*)metadata+sizeof(frameSpecOut), min_score);
	}    		 
	printf("exit test main....\n");
	sdk_uninit();
	return 0;
}
#elif defined TWO_NET_MODEL
/* 第一级模型默认参数 */
Network1Par cnn1PrmSet =
{
    imageWidth:-1, imageHeight:-1,                     /* 动态获取 */
    startX:-1, startY:-1,                                          /* 根据获取的sensor分辨率设置 */
    endX:-1,endY: -1,                                            /*  根据获取的sensor分辨率设置 */
    inputDimWidth:-1, inputDimHeight:-1,            /* 根据获取的模型参数设置 */
    inputFormat:IMG_FORMAT_BGR_PLANAR,      /*  默认为BRG输入 */
    meanValue:{0,0,0},
    stdValue:1,
    isOutputYUV:1,                                               /*打开YUV420输出功能*/
    isOutputH26X:1,                                             /*打开H26X编码功能*/
    isOutputJPEG:1,                                              /*打开MJPEG编码功能*/
    mode:ENCODE_H264_MODE,                          /* 使用H264编码格式 */
    extInputs:{0},                                                  /* model多输入，第二个输入参数 */
    modelCascade:0 ,                                           /* 默认不级联下一级模型 */
    inferenceACC:0,
};

/* 第二级模型默认参数 */
Network2Par cnn2PrmSet =
{
    startXAdj:0,
    startYAdj:0,
    endXAdj:0,
    endYAdj:0,
    labelMask:{0},                                                 /* 需要处理的label掩码，如果对应位置至1才会处理 */
    minConf: 0.99,                                                /* 检测目标的置信度大于该值才会处理 */
    inputDimWidth:-1, inputDimHeight:-1,            /* 根据获取的模型参数设置 */
    inputFormat:IMG_FORMAT_BGR_PLANAR,      /*  默认为BRG输入 */
    meanValue:{0,0,0},
    stdValue:1,
    extInputs:{0},                                                  /* model多输入，第二个输入参数 */
    modelCascade:0                                             /* 默认不级联下一级模型 */
};

///////////////////////////////////////////////////////////////////////////////////
int main(void)
{
    int ret;
    float conf=0.5;
    CameraInfo cam_info;
    memset(&cam_info, 0, sizeof(cam_info));
    
   char version[100];
   get_sdk_version(version);
   printf("sdk sersion:%d %s \n",get_usb_version(),version);
   
    //1.加载固件
    ret = load_fw("./moviUsbBoot","./fw/flicRefApp.mvcmd");
    if (ret < 0)
    {
        printf("lowd firmware error! return \n");
        return -1;
    }
    printf("usb sersion:%d \n", get_usb_version());

    //2. 获取camera模式支持列表
    SensorModesConfig cameraCfg;
    SensorModesList list;
    camera_control_get_features(&list);
    printf("list num:%d\n", list.num);
    for (int i = 0; i < list.num; i++)
    {
        SensorModesConfig features;
        memcpy(&features, &list.mode[i], sizeof(features));
        printf("[%d/%d]camera: %s, %dX%d@%dfps, AFmode:%d, maxEXP:%dus,gain[%d, %d]\n",
                i, list.num, features.moduleName, features.camWidth,
                features.camHeight, features.camFps, features.AFmode,
                features.maxEXP, features.minGain, features.maxGain);
    }

    //3. 选择camera工作模式
    int sensorModeId = 0; //0:1080P, 1:4K
    camera_select_sensor(sensorModeId);
    memcpy(&cameraCfg, &list.mode[sensorModeId], sizeof(cameraCfg)); //select camera info
    cnn1PrmSet.imageWidth  = cameraCfg.camWidth;
    cnn1PrmSet.imageHeight = cameraCfg.camHeight;

    //4. 配置模型参数

     //////////2级模型demo/////////
     // 4. 1 第一级模型初始化初始化
     cnn1PrmSet.startX      = 0;
     cnn1PrmSet.startY      = 0;
     cnn1PrmSet.endX        = cameraCfg.camWidth;
     cnn1PrmSet.endY        = cameraCfg.camHeight;
     cnn1PrmSet.inputDimWidth  = 300;
     cnn1PrmSet.inputDimHeight = 300;
     /* 级联下一级模型 */
     cnn1PrmSet.modelCascade = 1;

     //4.2 第二级模型初始化
     if(cnn1PrmSet.modelCascade !=0)
     {
         /* 基于第一级的检测结果，适当微调第一级的检测坐标，有利于识别 */
         if(1)
         {
             /*
              *起点向左上方微调（startXAdj，startYAdj ）
              *底点向右下方微调（endXAdj，endYAdj） */
             cnn2PrmSet.startXAdj  = -5;
             cnn2PrmSet.startYAdj  = -5;
             cnn2PrmSet.endXAdj   = 5;
             cnn2PrmSet.endYAdj   = 5;
         }

         cnn2PrmSet.minConf        = 0.7;  // 置信度设置，第一级目标检测大于这个值，才进入第二级模型处理
         cnn2PrmSet.labelMask[2] = 1;     // 车牌 label id = 2，车辆 label id = 1, 只处理id=2

         /*name1: "data" , shape: [1x3x24x94] - An input image in following format [1xCxHxW]. Expected color order is BGR.*/
         cnn2PrmSet.inputDimWidth  = 94;
         cnn2PrmSet.inputDimHeight = 24;
         /*
          *URL:  https://docs.openvinotoolkit.org/2019_R1.1/_license_plate_recognition_barrier_0001_description_license_plate_recognition_barrier_0001.html
          * name2: "seq_ind" , shape: [88,1] - An auxiliary blob that is needed for correct decoding. Set this to [0, 1, 1, ..., 1]. */
         //多输入初始化，转换成FP16
         uint16_t *p = (uint16_t *)cnn2PrmSet.extInputs;
         p[0] = (uint16_t)f32Tof16(0);
         for(int i=1;i<88;i++)
         {
             p[i] = (uint16_t)f32Tof16(1.0);
         }
     }

     char *blob =   "./blob/2019.1/vehicle-license-plate-detection-barrier-0106/vehicle-license-plate-detection-barrier-0106.blob";
     char *blob2 = "./blob/2019.1/license-plate-recognition-barrier-0001/license-plate-recognition-barrier-0001.blob";

     conf =  cnn2PrmSet.minConf;

     //////////////////////////////////////模型参数初始化 end///////////////////////////////////////////////////
     //5. sdk初始化
     ret = sdk_net2_init(0,0,\
             blob,  &cnn1PrmSet, sizeof(cnn1PrmSet), \
             blob2,  &cnn2PrmSet, sizeof(cnn2PrmSet));

    //6. 输出配置
    camera_video_out(YUV420p,VIDEO_OUT_CONTINUOUS);

    //////////////////////////////////////////测试读取/////////////////////////////////////////////////
    char *recvImageData     = (char*) malloc(sizeof(frameSpecOut) + cameraCfg.camWidth * cameraCfg.camHeight * 3 / 2);
    int  meteDataSize  = 5*1024*1024;
    char *recvMetaData = (char*) malloc(meteDataSize);
    if ((recvImageData == 0) || (recvMetaData ==0))
          return -1;
    while (1)
    {
        char src[64];
        frameSpecOut hdr;
        float scale = 960 * 1.0 / cameraCfg.camWidth;
        int maxReadSize;
        char *yuv420p, *metaData;

        //阻塞读取yuv420数据
        maxReadSize  = sizeof(frameSpecOut)+ cameraCfg.camWidth * cameraCfg.camHeight * 3 / 2;
        if (read_yuv_data(recvImageData, &maxReadSize, true) < 0)
        {
            os_sleep(1);
            break;
        }
        memcpy(&hdr, recvImageData, sizeof(frameSpecOut));

        //非阻塞读取metedata数据
        maxReadSize = meteDataSize;
        if (read_meta_data(recvMetaData, &maxReadSize, false) == 0)
        {
            memcpy(&hdr, recvMetaData, sizeof(frameSpecOut));
        }

        //提取YUV数据和metedata数据
        yuv420p  = (char*) recvImageData + sizeof(frameSpecOut);
        metaData = (char*) recvMetaData + sizeof(frameSpecOut);

        //测试显示
        sprintf(src, "demo_video_%dx%d@%dfps(scale:%d%%)", cameraCfg.camWidth, cameraCfg.camHeight, cameraCfg.camFps,(int)(100*scale));
        vehicle_license_plate_detection_barrier(yuv420p , cameraCfg.camWidth, cameraCfg.camHeight, scale,src, 0, &cnn1PrmSet,&cnn2PrmSet, metaData, conf);
    }
    printf("exit test main....\n");
    sdk_uninit();
    return 0;
}
#else defined TWO_INPUT_MODEL
Network1Par cnn1PrmSet =
{
    imageWidth:-1, imageHeight:-1,                     /* 动态获取 */
    startX:-1, startY:-1,                                          /* 根据获取的sensor分辨率设置 */
    endX:-1,endY: -1,                                            /*  根据获取的sensor分辨率设置 */
    inputDimWidth:-1, inputDimHeight:-1,            /* 根据获取的模型参数设置 */
    inputFormat:IMG_FORMAT_BGR_PLANAR,      /*  默认为BRG输入 */
    meanValue:{0,0,0},
    stdValue:1,
    isOutputYUV:1,                                               /*打开YUV420输出功能*/
    isOutputH26X:1,                                             /*打开H26X编码功能*/
    isOutputJPEG:1,                                              /*打开MJPEG编码功能*/
    mode:ENCODE_H264_MODE,                          /* 使用H264编码格式 */
    extInputs:{0},                                                  /* model多输入，第二个输入参数 */
    modelCascade:0 ,                                           /* 默认不级联下一级模型 */
    inferenceACC:0,
};
int main(void)
{
    int ret;
    float conf=0.5;
    
   char version[100];
   get_sdk_version(version);
   printf("sdk sersion:%d %s \n",get_usb_version(),version);
   
    //1.加载固件
    ret = load_fw("./moviUsbBoot","./fw/flicRefApp.mvcmd");
    if (ret < 0)
    {
        printf("lowd firmware error! return \n");
        return -1;
    }
    printf("usb sersion:%d \n", get_usb_version());

    //2. 获取camera模式支持列表
    SensorModesConfig cameraCfg;
    SensorModesList list;
    camera_control_get_features(&list);
    printf("list num:%d\n", list.num);
    for (int i = 0; i < list.num; i++)
    {
        SensorModesConfig features;
        memcpy(&features, &list.mode[i], sizeof(features));
        printf("[%d/%d]camera: %s, %dX%d@%dfps, AFmode:%d, maxEXP:%dus,gain[%d, %d]\n",
                i, list.num, features.moduleName, features.camWidth,
                features.camHeight, features.camFps, features.AFmode,
                features.maxEXP, features.minGain, features.maxGain);
    }

    //3. 选择camera工作模式
    int sensorModeId = 0; //0:1080P, 1:4K
    camera_select_sensor(sensorModeId);
    memcpy(&cameraCfg, &list.mode[sensorModeId], sizeof(cameraCfg)); //select camera info

    //4. 配置模型参数
     cnn1PrmSet.imageWidth  = cameraCfg.camWidth;
     cnn1PrmSet.imageHeight = cameraCfg.camHeight;

     cnn1PrmSet.startX      = 0;
     cnn1PrmSet.startY      = 0;
     cnn1PrmSet.endX        = cameraCfg.camWidth;
     cnn1PrmSet.endY        = cameraCfg.camHeight;
     cnn1PrmSet.inputDimWidth  = 992;
     cnn1PrmSet.inputDimHeight = 544;

      /* https://docs.openvinotoolkit.org/2019_R1.1/person-detection-retail-0002.html */
      /*input1 name: data , shape: [1x3x544x992] - An input image in following format [1xCxHxW]. The expected channel order is BGR.
        input2  name: im_info, shape: [1x6] - An image information [544, 992, 992/frame_width, 544/frame_height, 992/frame_width, 544/frame_height] */
     uint16_t *p = (uint16_t *)cnn1PrmSet.extInputs;
      int oft=0;
      p[oft++] = f32Tof16(554);
      p[oft++] = f32Tof16(992);
      p[oft++] = f32Tof16(1.0*992/(cnn1PrmSet.endX -cnn1PrmSet.startX ));
      p[oft++] = f32Tof16(1.0*544/(cnn1PrmSet.endY -cnn1PrmSet.startY ));
      p[oft++] = f32Tof16(1.0*992/(cnn1PrmSet.endX -cnn1PrmSet.startX ));
      p[oft++] = f32Tof16(1.0*544/(cnn1PrmSet.endY -cnn1PrmSet.startY ));

      char *blob  = "./blob/2019.1/person-detection-retail-0002/person-detection-retail-0002.blob";
    
      conf = 0.4;

     //////////////////////////////////////模型参数初始化 end///////////////////////////////////////////////////
     //5. sdk初始化
     ret = sdk_net2_init(0,0,\
             blob,  &cnn1PrmSet, sizeof(cnn1PrmSet), \
             NULL,  NULL, 0);

    //6. 输出配置
    camera_video_out(YUV420p,VIDEO_OUT_CONTINUOUS);

    //////////////////////////////////////////测试读取/////////////////////////////////////////////////
    char *recvImageData     = (char*) malloc(sizeof(frameSpecOut) + cameraCfg.camWidth * cameraCfg.camHeight * 3 / 2);
    int  meteDataSize  = 5*1024*1024;
    char *recvMetaData = (char*) malloc(meteDataSize);
    if ((recvImageData == 0) || (recvMetaData ==0))
          return -1;
    while (1)
    {
        char src[64];
        frameSpecOut hdr;
        float scale = 960 * 1.0 / cameraCfg.camWidth;
        int maxReadSize;
        char *yuv420p, *metaData;

        //阻塞读取yuv420数据
        maxReadSize  = sizeof(frameSpecOut)+ cameraCfg.camWidth * cameraCfg.camHeight * 3 / 2;
        if (read_yuv_data(recvImageData, &maxReadSize, true) < 0)
        {
            os_sleep(1);
            break;
        }
        memcpy(&hdr, recvImageData, sizeof(frameSpecOut));

        //非阻塞读取metedata数据
        maxReadSize = meteDataSize;
        if (read_meta_data(recvMetaData, &maxReadSize, false) == 0)
        {
            memcpy(&hdr, recvMetaData, sizeof(frameSpecOut));
        }

        //提取YUV数据和metedata数据
        yuv420p  = (char*) recvImageData + sizeof(frameSpecOut);
        metaData = (char*) recvMetaData + sizeof(frameSpecOut)+OUTPUT_INDEX_SIZE;

        //测试显示
        sprintf(src, "demo_video_%dx%d@%dfps(scale:%d%%)", cameraCfg.camWidth, cameraCfg.camHeight, cameraCfg.camFps,(int)(100*scale));
      //  void  obj_show_img_func(void *data, int w, int h, float scale, char *name, int nn_fov_show, CameraInfo *nnparm, char *nnret,float min_score)
        obj_show_img_func(yuv420p , cameraCfg.camWidth, cameraCfg.camHeight, scale,src, 0, (CameraInfo*)&cnn1PrmSet, metaData, conf);
    }
    printf("exit test main....\n");
    sdk_uninit();
    return 0;
}
#endif
