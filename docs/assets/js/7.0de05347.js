(window.webpackJsonp=window.webpackJsonp||[]).push([[7],{357:function(_,t,v){"use strict";v.r(t);var e=v(42),d=Object(e.a)({},(function(){var _=this,t=_.$createElement,v=_._self._c||t;return v("ContentSlotsDistributor",{attrs:{"slot-key":_.$parent.slotKey}},[v("h2",{attrs:{id:"overview-of-the-openncc-hardware"}},[v("a",{staticClass:"header-anchor",attrs:{href:"#overview-of-the-openncc-hardware"}},[_._v("#")]),_._v(" Overview of the OpenNCC Hardware")]),_._v(" "),v("p",[_._v("OpenNCC is an open source programmable AI camera equipped with Intel Movidius Myriad X Visual Processing Unit (VPU). It is a low-power system-on-chip (SoC) used for deep learning and other artificial intelligence vision application acceleration for drones, smart cameras, VR/AR and other devices. Under the same power consumption conditions, the performance of the deep neural network (DNN) provided by Myriad X is ten times that of Myriad 2. Based on the theoretical 4+ TOPS computing power of Myriad X, the DNN peak throughput of more than one trillion times per second (TOPS) is reached.")]),_._v(" "),v("p",[_._v("OpenNCC is composed of SENSOR board and CORE board, equipped with 2MP pixel sensor. OpenNCC CORE board uses the CSI_MIPI_RX 4lane interface to exchange data with the CORE board. The maximum sensor resolution is 20M@30fps, and it also supports 3D modules and infrared modules. CORE board output interface USB2.0/3.0, equipped with 4G/8G/16G LPDDR4, 16M SPI FLASH.。\n"),v("img",{attrs:{src:"/openncc/docimg/zh/HardwareF1.png",alt:"Figure-1"}})]),_._v(" "),v("h2",{attrs:{id:"hardware-spec"}},[v("a",{staticClass:"header-anchor",attrs:{href:"#hardware-spec"}},[_._v("#")]),_._v(" Hardware Spec.")]),_._v(" "),v("ul",[v("li",[v("p",[_._v("Lens"),v("br"),_._v(" "),v("img",{attrs:{src:"/openncc/docimg/zh/HardwareF2.jpg",alt:"Figure-2"}})])]),_._v(" "),v("li",[v("p",[_._v("Camera module board CM2\nsensor：SC2232H"),v("br"),_._v("\nresolution：1920*1080（2MP）or 4K\nFrame rate：30fps"),v("br"),_._v("\nSensor size：1/2.9"),v("br"),_._v(" "),v("img",{attrs:{src:"/openncc/docimg/zh/HardwareF3.jpg",alt:"Figure-3"}})])]),_._v(" "),v("li",[v("p",[_._v("SoM\nSize：38mm*38mm\n"),v("img",{attrs:{src:"/openncc/docimg/zh/HardwareF4.jpg",alt:"Figure-4"}})])]),_._v(" "),v("li",[v("p",[_._v("interface define\nUSB：TYPE C， Image orientation insertion supports USB3.0, and is recognized as USB2.0 when reverse inserting\n"),v("img",{attrs:{src:"/openncc/docimg/zh/HardwareF5.jpg",alt:"Figure-5"}})])])]),_._v(" "),v("p",[_._v("J3 FPC interface definition:")]),_._v(" "),v("table",[v("thead",[v("tr",[v("th",[_._v("ID")]),_._v(" "),v("th",[_._v("PIN")]),_._v(" "),v("th",[_._v("describe")]),_._v(" "),v("th",[_._v("Electrical characteristics")])])]),_._v(" "),v("tbody",[v("tr",[v("td",[_._v("1")]),_._v(" "),v("td",[_._v("VDD_5V")]),_._v(" "),v("td",[_._v("5V Power output")]),_._v(" "),v("td",[_._v("Output current≤1A")])]),_._v(" "),v("tr",[v("td",[_._v("2")]),_._v(" "),v("td",[_._v("VDD_5V")]),_._v(" "),v("td",[_._v("5V Power output")]),_._v(" "),v("td",[_._v("2")])]),_._v(" "),v("tr",[v("td",[_._v("3")]),_._v(" "),v("td",[_._v("VDD_5V")]),_._v(" "),v("td",[_._v("5V Power output")]),_._v(" "),v("td",[_._v("3")])]),_._v(" "),v("tr",[v("td",[_._v("4")]),_._v(" "),v("td",[_._v("GND")]),_._v(" "),v("td",[_._v("Reference ground")]),_._v(" "),v("td",[_._v("/")])]),_._v(" "),v("tr",[v("td",[_._v("5")]),_._v(" "),v("td",[_._v("GND")]),_._v(" "),v("td",[_._v("Reference ground")]),_._v(" "),v("td",[_._v("/")])]),_._v(" "),v("tr",[v("td",[_._v("6")]),_._v(" "),v("td",[_._v("GND")]),_._v(" "),v("td",[_._v("Reference ground")]),_._v(" "),v("td",[_._v("/")])]),_._v(" "),v("tr",[v("td",[_._v("7")]),_._v(" "),v("td",[_._v("CAM_A_AUX")]),_._v(" "),v("td",[_._v("GPIO")]),_._v(" "),v("td",[_._v("1.8V")])]),_._v(" "),v("tr",[v("td",[_._v("8")]),_._v(" "),v("td",[_._v("CAM_A_RST")]),_._v(" "),v("td",[_._v("GPIO/Reset")]),_._v(" "),v("td",[_._v("1.8V")])]),_._v(" "),v("tr",[v("td",[_._v("9")]),_._v(" "),v("td",[_._v("I2C1_SCL")]),_._v(" "),v("td",[_._v("I2C CLK")]),_._v(" "),v("td",[_._v("1.8V")])]),_._v(" "),v("tr",[v("td",[_._v("10")]),_._v(" "),v("td",[_._v("COM_IO1")]),_._v(" "),v("td",[_._v("GPIO")]),_._v(" "),v("td",[_._v("1.8V")])]),_._v(" "),v("tr",[v("td",[_._v("11")]),_._v(" "),v("td",[_._v("CAM_A_CLK")]),_._v(" "),v("td",[_._v("GPIO/CLK")]),_._v(" "),v("td",[_._v("1.8V")])]),_._v(" "),v("tr",[v("td",[_._v("12")]),_._v(" "),v("td",[_._v("CAM_B_AUX")]),_._v(" "),v("td",[_._v("GPIO/Power down")]),_._v(" "),v("td",[_._v("1.8V")])]),_._v(" "),v("tr",[v("td",[_._v("13")]),_._v(" "),v("td",[_._v("COM_IO2")]),_._v(" "),v("td",[_._v("GPIO")]),_._v(" "),v("td",[_._v("1.8V")])]),_._v(" "),v("tr",[v("td",[_._v("14")]),_._v(" "),v("td",[_._v("CAM_A_PWM/RST")]),_._v(" "),v("td",[_._v("GPIO")]),_._v(" "),v("td",[_._v("1.8V")])]),_._v(" "),v("tr",[v("td",[_._v("15")]),_._v(" "),v("td",[_._v("I2C1_SDA")]),_._v(" "),v("td",[_._v("I2C Data")]),_._v(" "),v("td",[_._v("1.8V")])]),_._v(" "),v("tr",[v("td",[_._v("16")]),_._v(" "),v("td",[_._v("GND")]),_._v(" "),v("td",[_._v("Reference ground")]),_._v(" "),v("td",[_._v("/")])]),_._v(" "),v("tr",[v("td",[_._v("17")]),_._v(" "),v("td",[_._v("CAM_A_D1_P")]),_._v(" "),v("td",[_._v("MIPI Data-1 P")]),_._v(" "),v("td",[_._v("/")])]),_._v(" "),v("tr",[v("td",[_._v("18")]),_._v(" "),v("td",[_._v("CAM_A_D1_N")]),_._v(" "),v("td",[_._v("MIPI Data-1 N")]),_._v(" "),v("td",[_._v("/")])]),_._v(" "),v("tr",[v("td",[_._v("19")]),_._v(" "),v("td",[_._v("GND")]),_._v(" "),v("td",[_._v("Reference ground")]),_._v(" "),v("td",[_._v("/")])]),_._v(" "),v("tr",[v("td",[_._v("20")]),_._v(" "),v("td",[_._v("CAM_A_D0_P")]),_._v(" "),v("td",[_._v("MIPI Data-0 P")]),_._v(" "),v("td",[_._v("/")])]),_._v(" "),v("tr",[v("td",[_._v("21")]),_._v(" "),v("td",[_._v("CAM_A_D0_N")]),_._v(" "),v("td",[_._v("MIPI Data-0 N")]),_._v(" "),v("td",[_._v("/")])]),_._v(" "),v("tr",[v("td",[_._v("22")]),_._v(" "),v("td",[_._v("GND")]),_._v(" "),v("td",[_._v("Reference ground")]),_._v(" "),v("td",[_._v("/")])]),_._v(" "),v("tr",[v("td",[_._v("23")]),_._v(" "),v("td",[_._v("CAM_A_L_C_P")]),_._v(" "),v("td",[_._v("MIPI CLK P")]),_._v(" "),v("td",[_._v("/")])]),_._v(" "),v("tr",[v("td",[_._v("24")]),_._v(" "),v("td",[_._v("CAM_A_L_C_N")]),_._v(" "),v("td",[_._v("MIPI CLK N")]),_._v(" "),v("td",[_._v("/")])]),_._v(" "),v("tr",[v("td",[_._v("25")]),_._v(" "),v("td",[_._v("GND")]),_._v(" "),v("td",[_._v("Reference ground")]),_._v(" "),v("td",[_._v("/")])]),_._v(" "),v("tr",[v("td",[_._v("26")]),_._v(" "),v("td",[_._v("CAM_A_D2_P")]),_._v(" "),v("td",[_._v("MIPI Data-2 P")]),_._v(" "),v("td",[_._v("/")])]),_._v(" "),v("tr",[v("td",[_._v("27")]),_._v(" "),v("td",[_._v("CAM_A_D2_N")]),_._v(" "),v("td",[_._v("MIPI Data-2 N")]),_._v(" "),v("td",[_._v("/")])]),_._v(" "),v("tr",[v("td",[_._v("28")]),_._v(" "),v("td",[_._v("GND")]),_._v(" "),v("td",[_._v("Reference ground")]),_._v(" "),v("td",[_._v("/")])]),_._v(" "),v("tr",[v("td",[_._v("29")]),_._v(" "),v("td",[_._v("CAM_A_D3_P")]),_._v(" "),v("td",[_._v("MIPI Data-3 P")]),_._v(" "),v("td",[_._v("/")])]),_._v(" "),v("tr",[v("td",[_._v("30")]),_._v(" "),v("td",[_._v("CAM_A_D3_N")]),_._v(" "),v("td",[_._v("MIPI Data-3 N")]),_._v(" "),v("td",[_._v("/")])])])]),_._v(" "),v("ul",[v("li",[v("p",[_._v("Boot Modes:"),v("br"),_._v(" "),v("img",{attrs:{src:"/openncc/docimg/zh/HardwareF6.jpg",alt:"Figure-6"}})])]),_._v(" "),v("li",[v("p",[_._v("Appendix I:")])])]),_._v(" "),v("table",[v("thead",[v("tr",[v("th",[_._v("Item")]),_._v(" "),v("th",[_._v("openncc")])])]),_._v(" "),v("tbody",[v("tr",[v("td",[_._v("AI")]),_._v(" "),v("td",[_._v("AI")])]),_._v(" "),v("tr",[v("td",[_._v("Computing power")]),_._v(" "),v("td",[_._v("maximum 4tflops")])]),_._v(" "),v("tr",[v("td",[_._v("Support models")]),_._v(" "),v("td",[_._v("all openvino models")])]),_._v(" "),v("tr",[v("td",[_._v("Support framework")]),_._v(" "),v("td",[_._v("onnx, tensorflow, Caffe, mxnet, Kaldi")])]),_._v(" "),v("tr",[v("td",[_._v("Software")]),_._v(" "),v("td",[_._v("software")])]),_._v(" "),v("tr",[v("td",[_._v("Image signal processing")]),_._v(" "),v("td",[_._v("√")])]),_._v(" "),v("tr",[v("td",[_._v("Open source kits")]),_._v(" "),v("td",[_._v("camera development kit openncc CDK, development technical documents, configuration tool openncc view")])]),_._v(" "),v("tr",[v("td",[_._v("Supported development language")]),_._v(" "),v("td",[_._v("C / C + + / Python")])]),_._v(" "),v("tr",[v("td",[_._v("SDK support function")]),_._v(" "),v("td",[_._v("1. Get video stream")])]),_._v(" "),v("tr",[v("td"),_._v(" "),v("td",[_._v("2. AI model download and replacement")])]),_._v(" "),v("tr",[v("td"),_._v(" "),v("td",[_._v("3. Obtain the model operation results")])]),_._v(" "),v("tr",[v("td"),_._v(" "),v("td",[_._v("4. Camera photographing, resetting, etc")])]),_._v(" "),v("tr",[v("td",[_._v("OpenView function")]),_._v(" "),v("td",[_._v("configure camera parameters, configure camera local AI model")])]),_._v(" "),v("tr",[v("td",[_._v("Openncc CDK supported operating systems")]),_._v(" "),v("td",[_._v("Linux and windows")])]),_._v(" "),v("tr",[v("td",[_._v("OpenView supported operating system")]),_._v(" "),v("td",[_._v("Linux / windows")])]),_._v(" "),v("tr",[v("td",[_._v("Hardware")]),_._v(" "),v("td",[_._v("hardware")])]),_._v(" "),v("tr",[v("td",[_._v("Dimensions")]),_._v(" "),v("td",[_._v("38 mm x 38 mm x 45 mm")])]),_._v(" "),v("tr",[v("td",[_._v("Weight")]),_._v(" "),v("td",[_._v("camera net weight 31G")])]),_._v(" "),v("tr",[v("td",[_._v("VPU")]),_._v(" "),v("td",[_._v("Intel Movidius Myriad X MV2085")])]),_._v(" "),v("tr",[v("td",[_._v("Memory")]),_._v(" "),v("td",[_._v("4GB")])]),_._v(" "),v("tr",[v("td",[_._v("Data interface")]),_._v(" "),v("td",[_._v("USB type-C 2.0 / 3.0")])]),_._v(" "),v("tr",[v("td",[_._v("Power supply")]),_._v(" "),v("td",[_._v("5V / 2A")])]),_._v(" "),v("tr",[v("td",[_._v("Camera module")]),_._v(" "),v("td",[_._v("2MP module")])]),_._v(" "),v("tr",[v("td",[_._v("Resolution")]),_._v(" "),v("td",[_._v("1920 x 1080")])]),_._v(" "),v("tr",[v("td",[_._v("Frame rate")]),_._v(" "),v("td",[_._v("30HZ")])]),_._v(" "),v("tr",[v("td",[_._v("Horizontal field angle")]),_._v(" "),v("td",[_._v("50°")])])])])])}),[],!1,null,null,null);t.default=d.exports}}]);