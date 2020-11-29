### OpenNcc Windows version instructions

#### Table of content
- System Requirements
- Unzip the installation package
- Installing USB driver
- Run the OpenNcc.exe

#####  System Requirements
- win10 64bits

##### Unzip the installation package
-  Copy the `win_openncc_1.2.4.rar` installation package to the non-system disk, unzip `win_openncc_1.2.4.rar` package

##### Installing USB driver
-  Connect the camera to the computer,Open `device manager` on your computer, the device `Movidius MyriadX` can be seen.

- Open `windows command line`,In the `configuration` directory of `win_openncc_1.2.4` package,run `moviUsbBoot.exe fw/flicRefApp.mvcmd` to boot device, `VSC loopback device` appears in `device manager`
- Open `device manager` on your computer,Update driver for `VSC loopback device` to `MovidiusDriver`,==Note:==
The `MovidiusDriver` driver package is provided in our `win_openncc_1.2.4` package

##### Run the OpenNcc.exe
- double-click `OpenNcc.exe` Run the program
- Click `Get device info` button to get device information
- Click `Stream Format` check box to select the video format
- Select the first level model from `1st network model` drop-down list
- The `2st network model` can be selected only if the `1st network model` is `vehicle-license-plate-detection-barrier-0106-FP16`
- Click the `Start Running Model` button to run the model
- Drag `Display Scaler` slider to control the size of the video
- Drag `Model Score` slider to Change the threshold value of target detection