Openvino 例子使用
1：编译环境
按照intel官方资料安装好openvino_2019.1.144,具体安装说明参考文档(ncc_cdk/Docs/用户使用手册.pdf),安装后进入安装路径下的例子代码目录/opt/intel/openvino/inference_engine/samples/

2:例子拷贝
 把ncc_cdk/Samples/How_to/work with OpenVINO目录下的interactive_face_detection_demo文件夹拷贝到当前samples路径下（之前的备份到其他目录或者直接替换），然后准备下一步编译。

3：编译：
可以配置编译Debug版本，修改build_samples.sh脚本 cmake -DCMAKE_BUILD_TYPE=Debug即可，然后./build_samples.sh,
生成openvino所有例子自动拷贝到程序运行目录如~/inference_engine_samples_build/intel64/Debug目录下，
可以发现当前的interactive_face_detection_demo程序

4：运行环境
1）copy How to/work with OpenVINO/Debug(简称how to目录)下的模型文件夹module到运行目录下~/inference_engine_samples_build/intel64
2）copy How to目录下usb下载程序moviUsbBoot到openvino运行目录~/inference_engine_samples_build/intel64/Debug
3）copy How to目录下固件发布程序fw目录到openvino运行目录~/inference_engine_samples_build/intel64/Debug
4) 进入ncc_cdk/Tools/deployment目录下,sudo ./install_NCC_udev_rules.sh，获取普通用户访问usb设备权限，操作系统重启。

5：运行命令
进入openvino运行目录，如~/inference_engine_samples_build/intel64/Debug目录下，输入下面命令即可
./interactive_face_detection_demo -i cam -m ../module/face-detection-adas-0001.xml -m_ag ../module/age-gender-recognition-retail-0013.xml -m_hp ../module/head-pose-estimation-adas-0001.xml -m_em ../module/emotions-recognition-retail-0003.xml -m_lm ../module/facial-landmarks-35-adas-0002.xml -d CPU

