1. Copy the OpenNCC Demo to OpenVINO
1.1. Remove the original "human_pose_estimation_demo" file from OpenVINO.
You can find it in this path: opt/intel/openvino/inference_engine/samples/
1.2. Copy the "human_pose_estimation_demo" file from the OpenNCC file to OpenVINO file.
Its new path in OpenVINO: /opt/intel/openvino/inference_engine/samples/

[Command Refrence] 
sudo cp -a /home/jerry/Desktop/OPENNCC/ncc_cdk/Samples/Demo/work\ with\ OpenVINO/  /opt/intel/openvino/inference_engine/samples/

2. Compile
2.1. Edit the“build_samples.sh”script in OpenVINO file：Edit the“cmake -DCMAKE_BUILD_TYPE=Release”to“cmake -DCMAKE_BUILD_TYPE=Debug”.
2.2. Enter the below command：

cd /opt/intel/openvino/inference_engine/samples/

./build_samples.sh

2.3. Check the compile resut: enter "~/inference_engine_samples_build/intel64/Debug", if there is "human_pose_estimation_demo"，compile succeeded.

3. Setup Run-environment in OpenVINO

3.1. Copy the file named "module" from OpenNCC CDK to OpenVINO.
Its original path in OpenNCC CDK: ncc_cdk/Samples/Demo/work with OpenVINO/Debug
Its new path in OpenVINO: OpenVINO's run category~/inference_engine_samples_build/intel64

3.2. Copy the USB download file named "moviUsbBoot" from OpenNCC CDK to OpenVINO.
Its original path in OpenNCC CDK: ncc_cdk/Samples/bin
Its new path in OpenVINO: OpenVINO's run category~/inference_engine_samples_build/intel64/Debug

4.3. Copy the firmware release file named "fw" from OpenNCC CDK to OpenVINO.
Its original path in OpenNCC CDK: samples/bin
Its new path in OpenVINO: OpenVINO's run category~/inference_engine_samples_build/intel64/Debug

4.4. Enter ncc_cdk/Tools/deployment, type in "sudo ./install_NCC_udev_rules.sh" to get visiting access for the USB device.

4.5 Restart the operation system.

5. Run command
5.1. Enter OpenVINO's run category~/inference_engine_samples_build/intel64/Debug
5.2. Type in the below command to run the Demo:
./human_pose_estimation_demo -i cam -m ../module/human-pose-estimation-0001.xml -d CPU
