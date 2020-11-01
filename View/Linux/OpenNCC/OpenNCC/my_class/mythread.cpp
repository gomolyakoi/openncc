#include "mythread.h"
#include <QThread>
#include <QDebug>
#include <QDateTime>
#include <QApplication>
#include <sys/time.h>
#include "ncc_sdk/Fp16Convert.h"
#include "ncc_sdk/cameraCtrl.h"
#include "ncc_sdk/sdk.h"
#include "decodevideo.h"
#include <opencv2/highgui/highgui.hpp>

extern void  vehicle_license_plate_detection_barrier(void *data, int w, int h, float scale, char *name, int nn_fov_show, Network1Par *nnParm1,Network2Par *nnParm2, char *nnret, float min_score);
extern float scale;
extern float min_score;


Network1Par cnn1PrmSet =
{
    -1, -1,                     /* 动态获取 */
    -1, -1,                                          /* 根据获取的sensor分辨率设置 */
    -1, -1,                                            /*  根据获取的sensor分辨率设置 */
    -1, -1,            /* 根据获取的模型参数设置 */
    IMG_FORMAT_BGR_PLANAR,      /*  默认为BRG输入 */
    {0,0,0},
    1,
    1,                                               /*打开YUV420输出功能*/
    1,                                             /*打开H26X编码功能*/
    1,                                              /*打开MJPEG编码功能*/
    ENCODE_H264_MODE,                          /* 使用H264编码格式 */
    {0},                                                  /* model多输入，第二个输入参数 */
    0,                                           /* 默认不级联下一级模型 */
    0,
};

/* 第二级模型默认参数 */
Network2Par cnn2PrmSet =
{
    0,
    0,
    0,
    0,
    {0},                                                 /* 需要处理的label掩码，如果对应位置至1才会处理 */
    0.99,                                                /* 检测目标的置信度大于该值才会处理 */
    -1,
    -1,            /* 根据获取的模型参数设置 */
    IMG_FORMAT_BGR_PLANAR,      /*  默认为BRG输入 */
    {0,0,0},
    1,
    {0},                                                  /* model多输入，第二个输入参数 */
    0                                             /* 默认不级联下一级模型 */
};

MyThread::MyThread(QObject *parent) : QObject(parent)
{

}

void MyThread::load_2net_model(int Veido_type,int modeId)
{
        int ret;
        //1. 加载固件
        ret=load_fw("./Configuration/moviUsbBoot","./Configuration/fw/flicRefApp.mvcmd");
        if (ret<0)
        {
            QApplication::restoreOverrideCursor();
            emit send_log_info_to_UI(tr("load firmware to OpenNCC failed! status:%1").arg(ret));
            return;
        }
        else {
            emit send_log_info_to_UI(tr("usb sersion: %1").arg(get_usb_version()));
            //2. 获取camera参数
            SensorModesConfig cameraCfg;
            SensorModesList list;
            camera_control_get_features(&list);
            for(int i=0;i<list.num;i++)
            {
                SensorModesConfig features;
                memcpy(&features, &list.mode[i], sizeof(features));
                emit send_log_info_to_UI("========================");
                emit send_log_info_to_UI(tr("modelIndex: %1,moduleName: %2").arg(i).arg(features.moduleName));
                emit send_log_info_to_UI(tr("camWidth: %1,camHeight: %2").arg(features.camWidth).arg(features.camHeight));
                emit send_log_info_to_UI(tr("camFps: %1").arg(features.camFps));
            }

//            msensorModeId = ui->outflow_mode->currentIndex(); //1080P模式
            int sensorModeId = modeId; //4K模式
            camera_select_sensor(sensorModeId);
            memcpy(&cameraCfg, &list.mode[sensorModeId], sizeof(cameraCfg));//select camera info
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
                     /*
                      *起点向左上方微调（startXAdj，startYAdj ）
                      *底点向右下方微调（endXAdj，endYAdj） */
                     cnn2PrmSet.startXAdj  = -5;
                     cnn2PrmSet.startYAdj  = -5;
                     cnn2PrmSet.endXAdj   = 5;
                     cnn2PrmSet.endYAdj   = 5;


                     cnn2PrmSet.minConf= min_score;  // 置信度设置，第一级目标检测大于这个值，才进入第二级模型处理
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

                 char *blob =   "./Configuration/blob/vehicle-license-plate-detection-barrier-0106.blob";
                 char *blob2 = "./Configuration/blob/license-plate-recognition-barrier-0001.blob";

                 //////////////////////////////////////模型参数初始化 end///////////////////////////////////////////////////
                 //5. sdk初始化
//             struct timeval tpstart,tpend;
//             float timeuse;
//             gettimeofday(&tpstart,NULL);

             ret = sdk_net2_init(0,0,blob,&cnn1PrmSet,sizeof(cnn1PrmSet),blob2,&cnn2PrmSet,sizeof(cnn2PrmSet));

//             gettimeofday(&tpend,NULL);
//             timeuse=(1000000*(tpend.tv_sec-tpstart.tv_sec) + tpend.tv_usec-tpstart.tv_usec)/1000.0;
//             qDebug()<<timeuse<<"ms";

            if (ret<0)
            {
                QApplication::restoreOverrideCursor();
                emit send_log_info_to_UI(tr("failed to init the device! status:%1").arg(ret));
                return;
            }
            else {
                //5. 输出配置
                char *recvImageData     = (char*) malloc(sizeof(frameSpecOut) + cameraCfg.camWidth * cameraCfg.camHeight * 3 / 2);
                int  meteDataSize  = 5*1024*1024;
                char *recvMetaData = (char*) malloc(meteDataSize);
                if ((recvImageData == 0) || (recvMetaData ==0))
                      return;
                NCCVideoCtrl(YUV420p,2);
                bool g_run = true;
                QApplication::restoreOverrideCursor();
                while(g_run)
                {
                    char src[64];
                    frameSpecOut hdr;
//                    scale = 960 * 1.0 / cameraCfg.camWidth;
                    int maxReadSize;
                    char *yuv420p, *metaData;

                    //阻塞读取yuv420数据
                    maxReadSize  = sizeof(frameSpecOut)+ cameraCfg.camWidth * cameraCfg.camHeight * 3 / 2;
                    if (read_yuv_data(recvImageData, &maxReadSize, true) < 0)
                    {
                        QThread::sleep(1);
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
                    sprintf(src, "demo_video_%dx%d@%dfps", cameraCfg.camWidth, cameraCfg.camHeight, cameraCfg.camFps);
                    vehicle_license_plate_detection_barrier(yuv420p , cameraCfg.camWidth, cameraCfg.camHeight, scale,src, 0, &cnn1PrmSet,&cnn2PrmSet, metaData, min_score);
                    if (!cvGetWindowHandle(src))
                    {
                         g_run = false;
                         cv::destroyWindow(src);
                         break;
                    }
                }
                emit send_log_info_to_UI("exit show video");
                sdk_uninit();
        }
    }
}
