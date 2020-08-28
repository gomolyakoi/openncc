
#ifndef _CAMERA_CONTROL_H
#define _CAMERA_CONTROL_H

#include <stdint.h>

#define MAX_MODE_SIZE   5

typedef enum {
    CAMERA_CONTROL__AE_AUTO__FLASH_MODE__OFF,
    CAMERA_CONTROL__AE_AUTO__FLASH_MODE__AUTO,
    CAMERA_CONTROL__AE_AUTO__FLASH_MODE__ALWAYS,
    CAMERA_CONTROL__AE_AUTO__FLASH_MODE__AUTO_REDEYE,
}camera_ctrl_ae_mode;

typedef enum {
    CAMERA_CONTROL__AWB_MODE__OFF, //0
    CAMERA_CONTROL__AWB_MODE__AUTO, //1
    CAMERA_CONTROL__AWB_MODE__INCANDESCENT, //2
    CAMERA_CONTROL__AWB_MODE__FLUORESCENT, //3
    CAMERA_CONTROL__AWB_MODE__WARM_FLUORESCENT, //4
    CAMERA_CONTROL__AWB_MODE__DAYLIGHT, //5
    CAMERA_CONTROL__AWB_MODE__CLOUDY_DAYLIGHT, //6
    CAMERA_CONTROL__AWB_MODE__TWILIGHT, //7
    CAMERA_CONTROL__AWB_MODE__SHADE, //8
}camera_ctrl_awb_mode;

typedef enum {
    CAMERA_CONTROL__AF_MODE_OFF, //0
    CAMERA_CONTROL__AF_MODE_AUTO, //1
    CAMERA_CONTROL__AF_MODE_MACRO, //2
    CAMERA_CONTROL__AF_MODE_CONTINUOUS_VIDEO, //3
    CAMERA_CONTROL__AF_MODE_CONTINUOUS_PICTURE, //4
    CAMERA_CONTROL__AF_MODE_EDOF, //5
}camera_ctrl_af_mode;


typedef enum
{
    VIDEO_OUT_DISABLE,       /* 禁止输出 */
    VIDEO_OUT_SINGLE,        /* 输出一次 */
    VIDEO_OUT_CONTINUOUS,    /* 连续输出 */
}camera_ctrl_video_out;


typedef struct
{
  char     moduleName[16];
  int      camWidth;   // width
  int      camHeight;  // heigth
  int      camFps;     // fps
  int      AFmode;     //　是否支持AF
  int      maxEXP;     // 最大曝光时间，单位us
  int      minGain;    // 最小增益倍数
  int      maxGain;    // 最大增益倍数
} SensorModesConfig; // config structure

typedef struct
{
  int              num;
  SensorModesConfig mode[MAX_MODE_SIZE];
} SensorModesList; // config structure

typedef struct
{
  int      ret;
  char     status[200];
} BlobInitStatus;

//caete this class for easy used by python 2020.3.26
class  CameraSensor  
{
	public :
		CameraSensor();
		~CameraSensor();
		int GetFirstSensor(SensorModesConfig* sensor);
		int GetNextSensor(SensorModesConfig* sensor);
	private:
		SensorModesList list;
		int nreaded;	
};

#ifdef __cplusplus
extern "C" {
#endif
//获取camera属性
int camera_control_get_features(SensorModesList* list);

//get cam sensor info returne array of sensors
int camera_control_get_sensorinfo(SensorModesConfig info[],int num);

//获取设备初始化状态
int camera_control_get_device_init_status(BlobInitStatus *status);

//设置af工作模式
int camera_control_af_mode(camera_ctrl_af_mode af_mode);

//设置lens位移
int camera_control_lens_move(uint32_t lens_position);

//触发af对焦
int camera_control_focus_trigger(void);

//设置camera曝光模式
int camera_control_ae_mode(camera_ctrl_ae_mode flash_mode);

//设置曝光时间，单位为us, 值不大于 1/fps 。
int camera_control_ae_set_exp( uint32_t exp_compensation);

//设置增益
int camera_control_ae_set_gain( uint32_t iso_val);

//设置camera白平衡模式
int camera_control_awb_mode(camera_ctrl_awb_mode awb_mode);

//激活看门狗和复位
int camera_wd_active(void);

//设备重启
int  camera_wd_reset(void);

//视频输出控制
int camera_video_out(int video_type,camera_ctrl_video_out mode);

//select camera sernsor
int camera_select_sensor(int sensorid);

#ifdef __cplusplus
}
#endif
#endif /* _CAMERA_CONTROL_H */

