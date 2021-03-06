QT       += core gui sql network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += console

CONFIG += c++11

# The following define makes your compiler emit warnings if you use
# any Qt feature that has been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS __PC__

# You can also make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

CONFIG+=force_debug_info
CONFIG+=separate_debug_info

QMAKE_CXXFLAGS_RELEASE += -O3

SOURCES += \
    decodevideo.cpp \
    main.cpp \
    my_class/add_blob.cpp \
    my_class/browserlabel.cpp \
    my_class/del_blob.cpp \
    my_class/mythread.cpp \
    my_class/roi_label.cpp \
    ncc_sdk/cls_demo_show.cpp \
    ncc_sdk/face_detection_demo_show.cpp \
    ncc_sdk/mask_detection_demo_show.cpp \
    ncc_sdk/obj_detection_demo_show.cpp \
    ncc_sdk/vehicle_license_plate_detection_barrier.cpp \
    utils.cpp \
    widget.cpp

HEADERS += \
    decodevideo.h \
    helper.h \
    my_class/add_blob.h \
    my_class/browserlabel.h \
    my_class/del_blob.h \
    my_class/mythread.h \
    my_class/roi_label.h \
    ncc_sdk/Fp16Convert.h \
    ncc_sdk/cameraCtrl.h \
    ncc_sdk/includes.h \
    ncc_sdk/sdk.h \
    ncc_sdk/sysinfo.h \
    utils.h \
    widget.h

FORMS += \
    my_class/add_blob.ui \
    my_class/del_blob.ui \
    widget.ui

RESOURCES += \
    openncc_css.qrc \
    res.qrc

INCLUDEPATH += $$PWD/ncc_sdk
DEPENDPATH += $$PWD/ncc_sdk

win32: {

    INCLUDEPATH += \
        D:/opencv/opencv/build/include \
        D:/ffmpeg/dev/include \

    CONFIG(debug, debug|release) {
        LIBS += D:\opencv\opencv\build\x64\vc14\lib\opencv_world3410d.lib
        LIBS += -L$$PWD/ncc_sdk/ -lOpenNCCd
        PRE_TARGETDEPS += $$PWD/ncc_sdk/OpenNCCd.lib
    } else {
        LIBS += D:\opencv\opencv\build\x64\vc14\lib\opencv_world3410.lib
        LIBS += -L$$PWD/ncc_sdk/ -lOpenNCC
        PRE_TARGETDEPS += $$PWD/ncc_sdk/OpenNCC.lib
    }


    LIBS += -LD:/ffmpeg/dev/lib/win64/ -lavcodec -lavdevice -lavfilter -lavformat -lavutil -lpostproc -lswscale

    LIBS += -lAdvapi32

}

unix:{

    CONFIG += link_pkgconfig
    PKGCONFIG += opencv
    PKGCONFIG += libusb-1.0

     LIBS += \
        -L$$PWD/ncc_sdk/ -lOpenNCC -lavutil -lavformat -lavcodec -lswresample -lswscale


}


