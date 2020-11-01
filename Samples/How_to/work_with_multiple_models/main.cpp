
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <unistd.h>
#include <assert.h>
#include "pthread.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
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


//////////////////////////////////////测试用例//////////////////////////////////

#define     TEST_FACE_DETCTION_ADAS_0001                                0
#define     TEST_FACE_DETCTION_RETAIL_0004                              1
#define     TEST_LICENSE_PLATE_RECOGNITION_BARRIER_0001      2
#define     TEST_PERSON_DETECTION_RETAIL_0002                        3
#define     TEST_OBJ_CLS                                                               4

#define    CUR_TEST_CASE     TEST_LICENSE_PLATE_RECOGNITION_BARRIER_0001

////////////////////////////////////////////////////////////////////////

void  cls_show_img_func(void *data, int w, int h, float scale, char *name, int nn_fov_show, NetworkPar0 *nnparm,NetworkPar1 *nnParm2, char *nnret,float conf);
void  obj_show_img_func(void *data, int w, int h, float scale, char *name, int nn_fov_show, NetworkPar0 *nnparm,NetworkPar1 *nnParm2, char *nnret,float conf);
void  vehicle_license_plate_detection_barrier(void *data, int w, int h, float scale, char *name, int nn_fov_show, NetworkPar0 *nnParm1,NetworkPar1 *nnParm2, char *nnret, float conf);
typedef void  (*analyzeMetedata)(void *data, int w, int h, float scale, char *name, int nn_fov_show, NetworkPar0 *nnparm, NetworkPar1 *nnParm2, char *nnret,float conf);

extern "C" void os_sleep(int ms);

/* 第一级模型默认参数 */
static NetworkPar0 cnn1PrmSet =
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
static NetworkPar1 cnn2PrmSet =
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

    //1.加载固件
    ret = load_fw("./fw/flicRefApp.mvcmd");
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
    //////////////////////////////////////模型参数初始化 start///////////////////////////////////////////////////
 #if (CUR_TEST_CASE == TEST_FACE_DETCTION_ADAS_0001)
     //////////单模型demo/////////
     cnn1PrmSet.startX      = 0;
     cnn1PrmSet.startY      = 0;
     cnn1PrmSet.endX        = cameraCfg.camWidth;
     cnn1PrmSet.endY        = cameraCfg.camHeight;
     cnn1PrmSet.inputDimWidth  = 672;
     cnn1PrmSet.inputDimHeight = 384;
     /*URL:      https://docs.openvinotoolkit.org/2019_R1.1/_face_detection_adas_0001_description_face_detection_adas_0001.html
     */
     char *blob  = "./blob/2019.1/face-detection-adas-0001/face-detection-adas-0001.blob";
     char *blob2 = NULL;
     analyzeMetedata fun = obj_show_img_func;
     conf = 0.8;
 #elif  (CUR_TEST_CASE == TEST_FACE_DETCTION_RETAIL_0004)
     //////////单模型demo/////////
      cnn1PrmSet.startX      = 0;
      cnn1PrmSet.startY      = 0;
      cnn1PrmSet.endX        = cameraCfg.camWidth;
      cnn1PrmSet.endY        = cameraCfg.camHeight;
      cnn1PrmSet.inputDimWidth  = 300;
      cnn1PrmSet.inputDimHeight = 300;

      char *blob  = "./blob/2019.1/face-detection-retail-0004/face-detection-retail-0004.blob";
      char *blob2 = NULL;
      analyzeMetedata fun = obj_show_img_func;
      conf = 0.8;

 #elif  (CUR_TEST_CASE == TEST_OBJ_CLS)
     //////////单模型demo/////////
      cnn1PrmSet.startX      = 0;
      cnn1PrmSet.startY      = 0;
      cnn1PrmSet.endX        = cameraCfg.camWidth;
      cnn1PrmSet.endY        = cameraCfg.camHeight;
      cnn1PrmSet.inputDimWidth  = 300;
      cnn1PrmSet.inputDimHeight = 300;

      char *blob  = "./blob/2019.1/object_classification/object_classification.blob";
      char *blob2 = NULL;
      analyzeMetedata fun = cls_show_img_func;
      conf = 0.7;

 #elif  (CUR_TEST_CASE == TEST_LICENSE_PLATE_RECOGNITION_BARRIER_0001)
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
     analyzeMetedata fun = vehicle_license_plate_detection_barrier;
     conf =  cnn2PrmSet.minConf;

 #elif (CUR_TEST_CASE == TEST_PERSON_DETECTION_RETAIL_0002)

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
      char *blob2 = NULL;
      analyzeMetedata fun = obj_show_img_func;
      conf = 0.4;
 #else
 //     "TEST_CASE definition error "
 #endif

     //////////////////////////////////////模型参数初始化 end///////////////////////////////////////////////////
     //5. sdk初始化
     ret = sdk_init(0,0,\
             blob,  &cnn1PrmSet, sizeof(cnn1PrmSet), \
             blob2,  &cnn2PrmSet, sizeof(cnn2PrmSet));

    //6. 输出配置
    camera_yuv420_out(YUV420_OUT_CONTINUOUS);
//    camera_h26x_out(H26X_OUT_ENABLE);
//    camera_mjpeg_out(MJPEG_OUT_CONTINUOUS);

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
         fun(yuv420p , cameraCfg.camWidth, cameraCfg.camHeight, scale,src, 0, &cnn1PrmSet,&cnn2PrmSet, metaData, conf);
    }
    printf("exit test main....\n");
    sdk_uninit();
    return 0;
}
