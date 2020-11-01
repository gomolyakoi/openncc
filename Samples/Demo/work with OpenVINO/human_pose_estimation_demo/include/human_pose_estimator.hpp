// Copyright (C) 2018-2019 Intel Corporation
// SPDX-License-Identifier: Apache-2.0
//

#pragma once

#include <string>
#include <vector>

#include <inference_engine.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/types_c.h>
#include "human_pose.hpp"

namespace human_pose_estimation {
	
class NccDev;
	
class HumanPoseEstimator {
public:
    static const size_t keypointsNumber;

    HumanPoseEstimator(const std::string& modelPath,
                       const std::string& targetDeviceName,
                       bool enablePerformanceReport = false);
    std::vector<HumanPose> estimate(const cv::Mat& image);
    ~HumanPoseEstimator();
	
		bool ReadVideo(cv::Mat& frame);
private:		
    void preprocess(const cv::Mat& image, float* buffer) const;
    std::vector<HumanPose> postprocess(
            const float* heatMapsData, const int heatMapOffset, const int nHeatMaps,
            const float* pafsData, const int pafOffset, const int nPafs,
            const int featureMapWidth, const int featureMapHeight,
            const cv::Size& imageSize) const;
    std::vector<HumanPose> extractPoses(const std::vector<cv::Mat>& heatMaps,
                                        const std::vector<cv::Mat>& pafs) const;
    void resizeFeatureMaps(std::vector<cv::Mat>& featureMaps) const;
    void correctCoordinates(std::vector<HumanPose>& poses,
                            const cv::Size& featureMapsSize,
                            const cv::Size& imageSize) const;
    bool inputWidthIsChanged(const cv::Size& imageSize);

    int minJointsNumber;
    int stride;
    cv::Vec4i pad;
    cv::Vec3f meanPixel;
    float minPeaksDistance;
    float midPointsScoreThreshold;
    float foundMidPointsRatioThreshold;
    float minSubsetScore;
    cv::Size inputLayerSize;
    int upsampleRatio;
    InferenceEngine::InferencePlugin plugin;
    InferenceEngine::CNNNetwork network;
    InferenceEngine::ExecutableNetwork executableNetwork;
    InferenceEngine::InferRequest request;
    InferenceEngine::CNNNetReader netReader;
    std::string pafsBlobName;
    std::string heatmapsBlobName;
    bool enablePerformanceReport;
    std::string modelPath;
    	
    NccDev* ncc;//add by duke 2020.10.12
};

//add by duke 2020.3.17
#include "sdk.h"
#include "cameraCtrl.h"
#include "Fp16Convert.h"
//#include <opencv2/imgproc/types_c.h>

static  std::string fileNameNoExt(const std::string &filepath) {
    auto pos = filepath.rfind('.');
    if (pos == std::string::npos) return filepath;
    return filepath.substr(0, pos);
}

class NccDev{		
    public:	
    NccDev(const std::string &pathToModel)                
    {	
		std::string blobfile = fileNameNoExt(pathToModel) + ".blob";
		std::cout<<blobfile<<std::endl;
			
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
		    1,                           /*��YUV420�������*/
		    0,                           /*��H26X���빦��*/
		    0,                           /*��MJPEG���빦��*/
			ENCODE_H264_MODE,            /* ʹ��H264�����ʽ */
		};

		/*1 load firware*/
		int ret = load_fw("./moviUsbBoot","./fw/flicRefApp.mvcmd");
		if (ret < 0)
		{
			printf("init device error! return \n");
			return;
		}

		/* 2. get camera param */
 		SensorModesConfig mode[MAX_MODE_SIZE];
		int num=camera_control_get_sensorinfo(mode,MAX_MODE_SIZE);
	  for(int i=0;i<num;i++)
    {
        printf("[%d/%d]camera: %s, %dX%d@%dfps, AFmode:%d, maxEXP:%dus,gain[%d, %d]\n",i,num,
                mode[i].moduleName, mode[i].camWidth, mode[i].camHeight, mode[i].camFps,
                mode[i].AFmode, mode[i].maxEXP, mode[i].minGain, mode[i].maxGain);
    }	    

    int sensorModeId = 0; //1080Pģʽ
   // int sensorModeId = 1; //4Kģʽ
    camera_select_sensor(sensorModeId);
   // memcpy(&cameraCfg, &list.mode[sensorModeId], sizeof(cameraCfg));//select camera info

  // 3. �㷨��Ч�����ʼ�� 
  cam_info.imageWidth  = mode[sensorModeId].camWidth;
  cam_info.imageHeight = mode[sensorModeId].camHeight;
  cam_info.startX      = 0;//420;
  cam_info.startY      = 0;
  cam_info.endX        = mode[sensorModeId].camWidth;//1500;
  cam_info.endY        = mode[sensorModeId].camHeight;

		width = cam_info.imageWidth;
		height = cam_info.imageHeight;

		ret = sdk_init(NULL, NULL, (char*)blobfile.c_str(), &cam_info, sizeof(cam_info));
		printf("xlink_init %d  cam %dX%d\n", ret,width,height);
		if (ret < 0) return;
			
 	 //5. �������
 		 camera_video_out(YUV420p,VIDEO_OUT_CONTINUOUS);
  };

	virtual ~NccDev()
	{
		sdk_uninit();
	};       
	
	bool ReadVideo(cv::Mat& frame)
	{
		int size = sizeof(yuv420p);
		if (read_yuv_data(yuv420p, &size, 1) < 0)
			return false ;
		cv::Mat yuvImg;
		yuvImg.create(height * 3 / 2, width, CV_8UC1);	
		yuvImg.data = (unsigned char*)yuv420p + sizeof(frameSpecOut);

		cv::cvtColor(yuvImg, frame, CV_YUV2BGR_I420);
		return true;
	};		
	
	bool ReadMetaData(float* fbuffer,int len)
	{
		int size = sizeof(metadata);
		int ret = read_meta_data(metadata, &size, 0);
		if (ret<0) 
			return false;
		frameSpecOut *out = (frameSpecOut *)metadata;
		uint16_t* cls_ret = (uint16_t*)(metadata+sizeof(frameSpecOut)+OUTPUT_INDEX_SIZE);
		size -= sizeof(frameSpecOut)+OUTPUT_INDEX_SIZE;	
		printf("read read_meta_data size=%d out size=%d success!\n",out->size,size);

		for (int i = 0; i < size/2; i++) 
			fbuffer[i]=f16Tof32(cls_ret[i]);		
		return true;					
	};
	
	private:
		int width;
		int height;
		char metadata[1024*300];
		char yuv420p[sizeof(frameSpecOut) + 1920 * 1080 * 2];		  		    
};
//add end
}  // namespace human_pose_estimation
