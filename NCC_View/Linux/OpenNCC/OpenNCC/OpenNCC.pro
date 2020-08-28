QT       += core gui sql network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

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

SOURCES += \
    decodevideo.cpp \
    main.cpp \
    my_class/add_blob.cpp \
    my_class/browserlabel.cpp \
    my_class/del_blob.cpp \
    my_class/metadata_thread.cpp \
    my_class/roi_label.cpp \
    ncc_sdk/cls_demo_show.cpp \
    ncc_sdk/face_detection_demo_show.cpp \
    ncc_sdk/obj_detection_demo_show.cpp \
    widget.cpp

HEADERS += \
    decodevideo.h \
    my_class/add_blob.h \
    my_class/browserlabel.h \
    my_class/del_blob.h \
    my_class/metadata_thread.h \
    my_class/roi_label.h \
    ncc_sdk/Fp16Convert.h \
    ncc_sdk/cameraCtrl.h \
    ncc_sdk/includes.h \
    ncc_sdk/sdk.h \
    ncc_sdk/sysinfo.h \
    widget.h

FORMS += \
    my_class/add_blob.ui \
    my_class/del_blob.ui \
    widget.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/ncc_sdk/release/ -lOpenNCC
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/ncc_sdk/debug/ -lOpenNCC
else:unix: LIBS += -L$$PWD/ncc_sdk/ -lOpenNCC -lavutil -lavformat -lavcodec -lswresample -lswscale

#-lavdevice -lavfilter -lpostproc
INCLUDEPATH += $$PWD/ncc_sdk
DEPENDPATH += $$PWD/ncc_sdk

unix: CONFIG += link_pkgconfig
unix: PKGCONFIG += opencv

unix: CONFIG += link_pkgconfig
unix: PKGCONFIG += libusb-1.0

RESOURCES += \
    res.qrc

unix:!macx: LIBS += -L$$PWD/ncc_sdk/ -lOpenNCC

INCLUDEPATH += $$PWD/ncc_sdk
DEPENDPATH += $$PWD/ncc_sdk


