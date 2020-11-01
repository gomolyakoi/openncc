
已测试环境：ubuntu16.04/ubuntu18.04, usb3.0.

软件依赖和安装：
1. sudo apt-get install libusb-dev  libusb-1.0-0-dev 
2. sudo apt-get install ffmpeg
3. sudo apt-get install libopencv-dev

////////////////////////////////////////////////////////////////////////

测试程序流程和说明：
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

5. ncc配置相关配置
 5.1 算法有效区配置，左顶点、右底点
    cam_info.startX      = 0;
    cam_info.startY      = 0;
    cam_info.endX        = cameraCfg.camWidth;
    cam_info.endY        = cameraCfg.camHeight;
 5.2 图像预处理初始化
    cam_info.inputDimWidth  = xxx;
    cam_info.inputDimHeight = xxx;
    cam_info.inputFormat    = IMG_FORMAT_BGR_PLANAR;
    cam_info.meanValue[0]   = 0;
    cam_info.meanValue[1]   = 0;
    cam_info.meanValue[2]   = 0;
    cam_info.stdValue       = 1;
5.3 算法blob文件保存路径

6. 测试图像显示和metedata解析测试
obj_show_img_func
////////////////////////////////////////////////////////////////////////
7. 测试运行
7.1 编译: make clean;make -j
7.2 运行: 进入bin目录，　./Openncc





