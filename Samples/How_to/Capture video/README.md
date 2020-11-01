
已测试环境：ubuntu16.04, usb3.0.

软件依赖和安装：
1. sudo apt-get install libusb-dev  libusb-1.0-0-dev 
2. sudo apt-get install ffmpeg
3. sudo apt-get install libopencv-dev
////////////////////////////////////////////////////////////////////////

测试程序流程和说明：
测试初始化创建了３个线程分别处理YUV/H26X和mjpeg，yuv直接使用opencv显示，Mjpeg保存一张图片到本地，
h26x保存到本地，使用ffplay播放。支持1080p和4K两种模式，参考"选择摄像头工作模式".

1. USB加载固件
load_fw();

2. 获取当前摄像头模式支持列表，包含分辨率和帧率
camera_control_get_features();
　
3. 选择摄像头工作模式
camera_select_sensor();
         
4. 配置设备资源
cam_info.imageWidth   = cameraCfg.camWidth;　
cam_info.imageHeight  = cameraCfg.camHeight;　　
cam_info.isOutputYUV  = 1;　　　　　　　　　　//初始化YUV输出
cam_info.isOutputH26X = 1;　　　　　　　　　　//初始化H26X输出
cam_info.isOutputJPEG = 1;                   //初始化MJPEG输出
cam_info.mode         = ENCODE_H264_MODE;    //选择H26X编码模式， 264/265

5. 创建接收线程
5.1 打开YUV数据接收开关(isOutputYUV 必须设置成1)
    camera_yuv420_out(YUV420_OUT_SINGLE);　　　//单次触发
    camera_yuv420_out(YUV420_OUT_CONTINUOUS);　//连续输出 
	camera_yuv420_out(YUV420_OUT_DISABLE);　   //关闭输出

5.2  打开MJEP数据接收开关(isOutputJPEG 必须设置成1)
   camera_mjpeg_out(MJPEG_OUT_SINGLE); 　　　　//单次触发
   camera_mjpeg_out(MJPEG_OUT_CONTINUOUS); 　　//连续输出
   camera_mjpeg_out(MJPEG_OUT_DISABLE); 　　   //关闭输出

5.3  打开MJEP数据接收开关(isOutputJPEG 必须设置成1)
   camera_h26x_out(H26X_OUT_ENABLE); 　　　　   //打开输出
   camera_h26x_out(H26X_OUT_DISABLE); 　　　　　//关闭输出　

////////////////////////////////////////////////////////////////////////

6. 测试运行
6.1 编译: make clean;make -j
6.2 运行: 进入上层Samples/bin目录，　sudo ./Capturevideo

mjepg:    demo自动保存MJPEG图片
h26x预览: ffplay x_x.h26x




