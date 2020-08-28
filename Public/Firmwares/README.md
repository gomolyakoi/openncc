
固件和硬件匹配：
flicRefApp_2485.mvcmd     ----> MV0274
flicRefApp_MICRON.mvcmd   ----> MV0275
flicRefApp_SAMSUNG.mvcmd  ----> OPENNCC

v0.1
sensor驱动支持列表，ISP tunning只匹配SC8238。

{"imx378",  1920, 1080, 30, 1, 33333, 100, 1600},
{"sc8238",  1920, 1080, 30, 0, 33333, 100, 1600},
{"ov12895", 4096, 3072, 30, 0, 33333, 100, 1600},

v0.2 
1. 模型输入大小修改，缩放从640修改到1024。
2. CNN 核心只开启１个，优化ISP掉帧的问题。
3. 模型大小不做硬性限制，有内部DDR资源匹配。
4. 模型最大支持8核。
./myriad_compile -m cls.xml -o cls.blob  -VPU_PLATFORM VPU_2480 -VPU_NUMBER_OF_SHAVES 8 -VPU_NUMBER_OF_CMX_SLICES 8
５. 已测试模型：
pedestrian-and-vehicle-detector-adas-0001-fp16
vehicle-license-plate-detection-barrier-0106-fp16
vehicle-detection-adas-0002-fp16
pedestrian-detection-adas-0002-fp16
face-person-detection-retail-0002-fp16
person-vehicle-bike-detection-crossroad-0078-fp16
person-detection-retail-0013-fp16
face-detection-adas-0001-fp16
face-detection-retail-0004-fp16
cls

v0.21
1. 增加sensor分辨率模式的选择。
2. 修复了bug,提高系统稳定性。






