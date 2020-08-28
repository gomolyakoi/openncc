#include "widget.h"
#include "ui_widget.h"
#include <QDesktopServices>
#include <QTextBrowser>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ncc_sdk/win/pthread.h>
#include <QStringList>
#include "cameraCtrl.h"
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/imgproc/types_c.h>
#include <opencv2/highgui/highgui_c.h>
#include "decodevideo.h"
#ifndef _WINDOWS
//#include <sys/time.h>
#else
#include <windows.h>
extern "C" int gettimeofday(struct timeval *tp, void *tzp);
#pragma warning(disable:4996)
#endif

using namespace cv;

//#define CALL_BACK
#define READ_DATA
//#define DL_IMG

/* 1. 物体分类demo程序 */
//#define  DEMO_CLS   1
/* 2. 人脸检测demo程序 */
#define  DEMO_FACE_DETECTION  1

#define  OPENCV_SHOW_SCALE    (0.8)    /* 显示缩放系数 */
extern "C"  void os_sleep(int ms);
static char cvname[64];
frameSpecOut* temp;
QString usb_version;


#define MAX_FRAME_ROWS  2180
#define MAX_FRAME_COLS  3872
#define BYTES_PIXELS    (2)
#define NCC_HEADER_SIZE  sizeof(frameSpecOut)
#define MAX_FRAME_BUF_SIZE  (NCC_HEADER_SIZE + MAX_FRAME_ROWS*MAX_FRAME_COLS*BYTES_PIXELS)
static char max_frame_buffer[MAX_FRAME_BUF_SIZE]; //TODO: This buffer need malloc after call camera_control_get_features
#define MAX_STREAM_BUF_SIZE (10000000/8) // 10MB, for H.264 H.265
static char max_stream_buffer[MAX_STREAM_BUF_SIZE]; //TODO: This buffer need malloc after call camera_control_get_features
static char metadata[1024*1024]; //TODO: depend on model's output format;Could us get size as call camera_control_get_features?

bool g_run = false;
static SensorModesConfig cameraCfg;
static SensorModesList   list;
QStringList valid_data_list;
QString cropx1,cropy1,cropx2,cropy2;

static CameraInfo cam_info =
{
-1,  //imageWidth
-1,  //imageHeight
-1,  //startX
-1,  //startY
-1,   //endX
-1,   //endY
0,  //inputDimWidth
0,   //inputDimHeight                   /* <dim>300</dim>  <dim>300</dim> */
IMG_FORMAT_BGR_PLANAR,      //IMAGE_FORMAT
0,                    //meanValue
0,
0 ,
1,                         //stdValue
1,                           /*打开YUV420输出功能*/
1,                           /*打开H26X编码功能*/
1,                           /*打开MJPEG编码功能*/
ENCODE_H264_MODE,            /* 使用H264编码格式 */
};
extern void  fd_show_img_func(void *data, int w, int h, float scale, char *name, int nn_fov_show, CameraInfo *nnparm, char *nnret,float min_score);
extern void  cls_show_img_func(void *data, int w, int h, float scale, char *name, int nn_fov_show, CameraInfo *nnparm, char *nnret,float min_score);
extern void  obj_show_img_func(void *data, int w, int h, float scale, char *name, int nn_fov_show, CameraInfo *nnparm, char *nnret,float min_score);
typedef void  (*analyzeMetedata)(void *data, int w, int h, float scale, char *name, int nn_fov_show, CameraInfo *nnparm, char *nnret,float min_score);
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
static analyzeMetedata fun;


Widget::Widget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Widget)
{
    ui->setupUi(this);
    //样式初始化
    setWindowIcon(QIcon(":/image/Window_icon.png"));
    setWindowTitle("OpenNCC View");
    ui->scale_spinbox->setValue(50);
    ui->min_score_spinbox->setValue(50);
    ui->outflow_mode->addItem("1080P");
    ui->yuv_stream->hide();

    QStringList mvfile_list;
    mvfile_list << "flicRefApp_SAMSUNG.mvcmd"<<"flicRefApp_MICRON.mvcmd"<<"flicRefApp_2485.mvcmd"<<"flicRefApp_2232.mvcmd" ;
    ui->mvfile->addItems(mvfile_list);

    mydata_thread = new metadata_thread();
    data_thread = new QThread(this);
    mydata_thread->moveToThread(data_thread);

    //模型选项初始化
    ui->model_1st->addItem("None");
    ui->model_2nd->addItem("None");
    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");

    db.setDatabaseName("D:\\NCC\\Configuration\\NCC.db");
    if (!db.open())
    {
        QMessageBox::warning(this,"Loading database error","Failed to load default sqlite database!\n\n"
                                                           "You need reinstall your NCC View or add a modle first.");
    }
    else {
        QSqlQuery query;
        query.exec("SELECT * FROM ncc_model where parentId = 1 or parentId = 3");
        while (query.next()){
            ui->model_1st->addItem(QString(query.value("model_name").toString()));
        }
    }

    add_blob_file = new add_blob();
    if(add_blob_file == NULL)
        return;
    //singals and slots
    connect(ui->roi_setting_area,&roi_label::StartPointSignal,this,&Widget::update_StartPoint);
    connect(ui->roi_setting_area,&roi_label::StopPointSignal,this,&Widget::update_StopPoint);

    //add/del blob
    connect(add_blob_file,&add_blob::update_model_1st_list_signal,this,&Widget::update_model_1st_list);
//    connect(del_blob_file,&del_blob::updatemodel_1st_list_signal,this,&Widget::update_model_1st_list);
    connect(this,&Widget::clear_blob_path,add_blob_file,&add_blob::clear_blob_path_record);

    connect(this,&Widget::close_mydata_thread,this,&Widget::close_data_thread);


    //open browser
    connect(ui->web_label1,&browserlabel::OpenBrowser,[=](){
        QDesktopServices::openUrl(QUrl(QString("https://download.01.org/opencv/2019/open_model_zoo/")));
    });
    connect(ui->web_label2,&browserlabel::OpenBrowser,[=](){
        QDesktopServices::openUrl(QUrl(QString("https://www.baidu.com")));
    });
    connect(ui->web_label3,&browserlabel::OpenBrowser,[=](){
        QDesktopServices::openUrl(QUrl(QString("http://aiyo.eyecloud.tech")));
    });

}

Widget::~Widget()
{
    if (data_thread->isRunning()){
        data_thread->quit();
        data_thread->wait();
        delete data_thread;
    }
    delete ui;
}

void Widget::close_data_thread(){
    if (data_thread->isRunning()){
        data_thread->quit();
        data_thread->wait();
        delete data_thread;
    }

}

void Widget::model_1st_changed()
{
    if (ui->model_1st->currentText() == "None" || ui->model_1st->currentText() == NULL)
    {
        ui->model_2nd->clear();
        ui->model_2nd->addItem("None");
    }
    else{
        QString check_file;
        check_file = QDir::currentPath() +"/Configuration/blob/" +ui->model_1st->currentText() +".blob";
        qDebug()<<check_file;
        if (!QFile::exists(check_file)){
//            QString link =
//            QString msg = tr("No blob file corresponding to the option was found! please visit the link as below to get files:<br><a href='%1'>Intel models</a>").arg(QDesktopServices::openUrl(QUrl(QString("http://aiyo.eyecloud.tech")));
            QMessageBox::critical(this,"option error","No blob file corresponding to the option was found! please visit the link as below to get files");
            ui->model_1st->setCurrentIndex(0);
        }
        else {
            //按选项查询数据库，获取对应模型信息
            QSqlQuery query;
            QString str= QString("SELECT * FROM ncc_model where model_name='%1'").arg(ui->model_1st->currentText());
            query.exec(str);
            query.first();
            //ui->model_info1->setText(query.value("model_info").toString());
            int model_id = query.value("id").toInt();
            //根据查到的模型id，获取其关联的子模型
            QString str2 = QString("select ncc_model.* from test inner join ncc_model on test.child_id = ncc_model.id where test.id=%1;").arg(model_id);
            query.exec(str2);
            ui->model_2nd->clear();
            ui->model_2nd->addItem("None");
            while (query.next())
            {
             ui->model_2nd->addItem(QString(query.value("model_name").toString()));
            }
        }
    }
}

void Widget::download_model_file(QString model_name)
{

}

void Widget::model_2nd_changed()
{
    if (ui->model_2nd->currentText()=="None")
    {
        //ui->label_2->setText("please select your 2nd model");
    }
    else {
      QSqlQuery query;
      QString str= QString("SELECT * FROM ncc_model where model_name='%1'").arg(ui->model_2nd->currentText());
      query.exec(str);
      query.first();
      //ui->label_2->setText(query.value("model_info").toString());
    }
}

void Widget::compare_coordinate(int a,int b){
    if (a<b){
        int temp = a;
        a = b;
        b = temp;
    }
}

//TODO: Why not make the NCC SDK like this
//OUT_DISABLE,      /* ��ֹ���� */
//OUT_SINGLE,       /* ����һ�� */
//OUT_CONTINUOUS,
int Widget::NCCVideoCtrl(int video_type,int mode)
{
    switch(video_type)
    {
        case YUV420p:
            camera_video_out(video_type,(camera_ctrl_video_out)mode);
        break;
        case H26X:
        {
            if((camera_ctrl_video_out)mode == VIDEO_OUT_CONTINUOUS)
            {
                InitDecoder(AV_CODEC_ID_H264);
            }
            else
                UninitDecoder();
            camera_video_out(video_type,(camera_ctrl_video_out)mode);
        }
            break;
        case JPEG:
            if((camera_ctrl_video_out)mode == VIDEO_OUT_CONTINUOUS)
            {
                InitDecoder(AV_CODEC_ID_MJPEG);
            }
            else
                UninitDecoder();
            camera_video_out(video_type,(camera_ctrl_video_out)mode);
            break;
        default:
            return -1;
            break;
    }
    return 0;
}


void Widget::on_load_fw_btn_clicked()
{
    QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
    if(!ui->mjpeg_stream->isChecked()&&!ui->yuv_stream->isChecked()&&!ui->h264_stream->isChecked()){
        QMessageBox::warning(this,"please select your Stream Format","Please select Stream Format first.");
        QApplication::restoreOverrideCursor();
    }
    else {
        data_thread->start();
        if (g_run){
            g_run = 0;
            cv::destroyWindow(cvname);
            ui->log_area->append(tr("exit show video"));
            sdk_uninit();
            UninitDecoder();
        }
        if (ui->yuv_stream->isChecked()){
            mvideo_type = YUV420p;
        }
        else if (ui->h264_stream->isChecked()) {
            mvideo_type = H26X;
        }
        else if (ui->mjpeg_stream->isChecked()){
            mvideo_type = JPEG;
        }
        int ret;
        memset(metadata, 0, sizeof(metadata));


        //1. 加载固件
        switch (ui->mvfile->currentIndex()) {
        case(0):
            ret=load_fw("D:\\NCC\\Configuration\\moviUsbBoot.exe","./Configuration/fw/flicRefApp_SAMSUNG.mvcmd");
            break;
        case(1):
            ret=load_fw("./Configuration/moviUsbBoot","./Configuration/fw/flicRefApp_MICRON.mvcmd");
            break;
        case(2):
            ret=load_fw("./Configuration/moviUsbBoot","./Configuration/fw/flicRefApp_2485.mvcmd");
            break;
        case(3):
            ret=load_fw("./Configuration/moviUsbBoot","./Configuration/fw/flicRefApp_2232.mvcmd");
            break;
        }
        //flicRefApp_SAMSUNG.mvcmd
        //flicRefApp_MICRON.mvcmd
        //flicRefApp_2485.mvcmd
        if (ret<0)
        {
            QApplication::restoreOverrideCursor();
            ui->log_area->append("load firmware to OpenNCC failed!");
        }
        else {
            ui->log_area->append(tr("usb sersion: %1").arg(get_usb_version()));

            //2. 获取camera参数
            camera_control_get_features(&list);
            for(int i=0;i<list.num;i++)
            {
                SensorModesConfig features;
                memcpy(&features, &list.mode[i], sizeof(features));
                ui->log_area->append("========================");
                ui->log_area->append(tr("modelIndex: %1,moduleName: %2").arg(i).arg(features.moduleName));
                ui->log_area->append(tr("camWidth: %1,camHeight: %2").arg(features.camWidth).arg(features.camHeight));
                ui->log_area->append(tr("camFps: %1").arg(features.camFps));
            }

            msensorModeId = ui->outflow_mode->currentIndex(); //1080P模式
        //    int sensorModeId = 1; //4K模式
            camera_select_sensor(msensorModeId);
            memcpy(&cameraCfg, &list.mode[msensorModeId], sizeof(cameraCfg));//select camera info

            // 3. 算法有效区域初始冿
            cam_info.imageWidth  = cameraCfg.camWidth;
            cam_info.imageHeight = cameraCfg.camHeight;
            if (cropx1.isEmpty()||cropx2.isEmpty()||cropy1.isEmpty()||cropy2.isEmpty()||cropx1==cropx2||cropy1==cropy2)
            {
                cam_info.startX      = 0;
                cam_info.startY      = 0;
                cam_info.endX        = cameraCfg.camWidth;
                cam_info.endY        = cameraCfg.camHeight;
            }
            else {
                int x1,y1,x2,y2;
                switch (ui->outflow_mode->currentIndex()) {
                case(0):
                    x1 = cropx1.toInt()*5;
                    x2 = cropx2.toInt()*5;
                    y1 = cropy1.toInt()*5;
                    y2 = cropy2.toInt()*5;
                    break;
                case(1):
                    x1 = cropx1.toInt()*10;
                    x2 = cropx2.toInt()*10;
                    y1 = cropy1.toInt()*10;
                    y2 = cropy2.toInt()*10;
                    break;
                }
                compare_coordinate(x1,x2);
                compare_coordinate(y1,y2);
                cam_info.startX      = x1;
                cam_info.startY      = y1;
                cam_info.endX        = x2;
                cam_info.endY        = y2;
            }
            ui->log_area->append(tr("ROI Region(Right Top:(%1,%2) Left Bottom:(%3,%4))").arg(cam_info.startX).arg(cam_info.startY).arg(cam_info.endX).arg(cam_info.endY));

            // 4. sdk初始冿
            char blob[256];
            switch (ui->model_1st->currentIndex()) {
            case(1):
                ui->log_area->append("model 01");
                strcpy(blob, "./Configuration/blob/classification-fp16.blob");
                ret = sdk_init(NULL,NULL, blob, &cam_info, sizeof(cam_info));
                fun = cls_show_img_func;
                break;
            case(2):
                ui->log_area->append("model 02");
                strcpy(blob, "./Configuration/blob/face-detection-retail-0004-fp16.blob");
                ret = sdk_init(NULL,NULL, blob, &cam_info, sizeof(cam_info));
                fun = fd_show_img_func;
                break;
            case(3):
                ui->log_area->append("model 03");
                strcpy(blob, "./Configuration/blob/face-detection-adas-0001-fp16.blob");
                ret = sdk_init(NULL,NULL, blob, &cam_info, sizeof(cam_info));
                fun = obj_show_img_func;
                break;
            case(4):
                ui->log_area->append("model 04");
                strcpy(blob, "./Configuration/blob/person-detection-retail-0013-fp16.blob");
                ret = sdk_init(NULL,NULL, blob, &cam_info, sizeof(cam_info));
                fun = obj_show_img_func;
                break;
            case(5):
                ui->log_area->append("model 05");
                strcpy(blob, "./Configuration/blob/person-vehicle-bike-detection-crossroad-0078-fp16.blob");
                ret = sdk_init(NULL,NULL, blob, &cam_info, sizeof(cam_info));
                fun = obj_show_img_func;
                break;
            case(6):
                ui->log_area->append("model 06");
                strcpy(blob, "./Configuration/blob/face-person-detection-retail-0002-fp16.blob");
                ret = sdk_init(NULL,NULL, blob, &cam_info, sizeof(cam_info));
                fun = obj_show_img_func;
                break;
            case(7):
                ui->log_area->append("model 07");
                strcpy(blob, "./Configuration/blob/pedestrian-detection-adas-0002-fp16.blob");
                ret = sdk_init(NULL,NULL, blob, &cam_info, sizeof(cam_info));
                fun = obj_show_img_func;
                break;
            case(8):
                ui->log_area->append("model 08");
                strcpy(blob, "./Configuration/blob/vehicle-detection-adas-0002-fp16.blob");
                ret = sdk_init(NULL,NULL, blob, &cam_info, sizeof(cam_info));
                fun = obj_show_img_func;
                break;
            case(9):
                ui->log_area->append("model 09");
                strcpy(blob, "./Configuration/blob/vehicle-license-plate-detection-barrier-0106-fp16.blob");
                ret = sdk_init(NULL,NULL, blob, &cam_info, sizeof(cam_info));
                fun = obj_show_img_func;
                break;
            case(10):
                ui->log_area->append("model 10");
                strcpy(blob, "./Configuration/blob/pedestrian-and-vehicle-detector-adas-0001-fp16.blob");
                ret = sdk_init(NULL,NULL, blob, &cam_info, sizeof(cam_info));
                fun = obj_show_img_func;
                break;
            default:
                ui->log_area->append("No blob has been selected");
                ret = sdk_init(NULL,NULL, NULL, &cam_info, sizeof(cam_info));
                fun = obj_show_img_func;
                break;
            }

            if (ret<0){
                QApplication::restoreOverrideCursor();
                ui->log_area->append("failed to init the device");
            }
            else {
                QApplication::restoreOverrideCursor();
                //5. 输出配置
                int get_log = 0;
                NCCVideoCtrl(mvideo_type,2);
                g_run = true;
                float min_score;
                float scale;
                //TODO:jonzhao 数据获取放到子线程去，UI这里不应该死循环
                while(g_run)
                {
                    get_log++;
                    if (ui->min_score_spinbox->value()==0){
                        min_score = 0.5;
                    }
                    else{
                        min_score = ui->min_score_spinbox->value()*0.01;
                    }
                    if (ui->scale_spinbox->value()==0){
                        scale = OPENCV_SHOW_SCALE;
                    }
                    else {
                        scale = ui->scale_spinbox->value()*0.01;
                    }

                    ret = PostProcessFrame(max_frame_buffer,MAX_FRAME_BUF_SIZE,min_score,scale,cvname);
                    switch (ret) {
                    case(2):
                        os_sleep(1);
                    case(3):
                        g_run = 0;
                        cv::destroyWindow(cvname);
                    }
                    if (valid_data_list.size()>0&&get_log%20==0){
                        ui->algo_area->append(valid_data_list.at(valid_data_list.size()-1));
                        valid_data_list.clear();
                    }
                    if (ui->mvfile->currentIndex() == 0 || ui->mvfile->currentIndex() == 3 )
                    {
                        if (get_log%30==0)
                        {
                        char *ch = (char *)temp->temps;
                        QString temp_info = tr("Temperature CSS = %1 MSS = %2 UPA0 = %3 UPA1 =%4 ID:%5")
                                .arg(temp->res[1]).arg(temp->res[2]).arg(temp->res[3]).arg(temp->res[4])
                                .arg(QString(ch));
                        ui->log_area->append(temp_info);
                        }

                    }
                    if (!cvGetWindowHandle(cvname))
                    {
                     g_run = 0;
                     cv::destroyWindow(cvname);
                     emit stop_mydata_thread_timer();
                     break;
                    }
                }
                ui->log_area->append(tr("exit show video"));
                sdk_uninit();
                UninitDecoder();
            }
        }
    }

}

QString Widget::current_time()
{
    QDateTime current_date_time =QDateTime::currentDateTime();
    QString current_date =current_date_time.toString("yyyy.MM.dd hh:mm:ss.zzz ddd");
    return current_date;
}



int Widget::PostProcessFrame(char* pFrame,int bufsize,float min_score,float scale,char* winhndl)
{
    int w,h,pixfmt,ret;
    char* Postframe = NULL;
    int size = bufsize;
    if(pFrame == NULL || bufsize <= 0 || winhndl == NULL)
        return 1;
    static int frametick = 0;
    frametick++;
    switch(mvideo_type)
    {
    case YUV420p:
        //如何先确定开的缓存区够大？如何保证数据不溢出？错误返回值？
        ret = read_yuv_data(pFrame,&size,1);
        if (ret<0){
            if (ret == USB_ERROR_NO_DEVICE){
                return 3;
            }
            else {
                return 2;
            }
        }
        temp = (frameSpecOut *)pFrame;
        Postframe = pFrame+sizeof(frameSpecOut);
        break;
    case H26X:
    {
        ret = read_26x_data(max_stream_buffer,&size,1);
        if (ret<0){
            if (ret == USB_ERROR_NO_DEVICE){
                return 3;
            }
            else {
                return 2;
            }
        }
        temp = (frameSpecOut *)max_stream_buffer;


//        struct timeval tpstart,tpend;
//        float timeuse;
//        gettimeofday(&tpstart,NULL);
          DecodeFrame((unsigned char *)(max_stream_buffer+sizeof(frameSpecOut)),size,(unsigned char *)pFrame,&w,&h,&pixfmt);
          Postframe = pFrame;

//        gettimeofday(&tpend,NULL);
//        timeuse=(1000000*(tpend.tv_sec-tpstart.tv_sec) + tpend.tv_usec-tpstart.tv_usec)/1000.0;
//        qDebug()<<timeuse<<"ms";
    }
        break;
    case JPEG:
    {
        ret = read_jpg_data(max_stream_buffer,&size,1);
        if (ret<0){
            if (ret == USB_ERROR_NO_DEVICE){
                return 3;
            }
            else {
                return 2;
            }
        }
        temp = (frameSpecOut *)max_stream_buffer;
//        frameSpecOut* spec =(frameSpecOut*) max_stream_buffer;
        DecodeFrame((unsigned char *)(max_stream_buffer+sizeof(frameSpecOut)),size,(unsigned char *)pFrame,&w,&h,&pixfmt);
        Postframe = pFrame;

    }
        break;
    default:
        break;
    }

    size = sizeof(metadata);
    read_meta_data(metadata,&size,0);
    //Only for debug,print and save the meta data
    //emit get_valid_algo_data(cameraCfg.camWidth,cameraCfg.camHeight,metadata+sizeof(frameSpecOut),min_score);
    sprintf(winhndl, "NCC_View_%dx%d@%dfps", cameraCfg.camWidth, cameraCfg.camHeight, cameraCfg.camFps);
    fun(Postframe, cameraCfg.camWidth, cameraCfg.camHeight, scale,winhndl, true, &cam_info, metadata+sizeof(frameSpecOut),min_score);
    return 0; 
}


void Widget::yuv_selected()
{
    if (ui->yuv_stream->isChecked()){
        ui->h264_stream->setEnabled(false);
        ui->h265_stream->setEnabled(false);
        ui->mjpeg_stream->setEnabled(false);
    }
    else {
        ui->h264_stream->setEnabled(true);
//        ui->h265_stream->setEnabled(true);
        ui->mjpeg_stream->setEnabled(true);
    }
}

void Widget::h264_selected()
{
    if (ui->h264_stream->isChecked()){
        ui->yuv_stream->setEnabled(false);
        ui->h265_stream->setEnabled(false);
        ui->mjpeg_stream->setEnabled(false);
    }
    else {
        ui->yuv_stream->setEnabled(true);
//        ui->h265_stream->setEnabled(true);
        ui->mjpeg_stream->setEnabled(true);
    }
}

void Widget::h265_selected()
{
    if (ui->h265_stream->isChecked()){
        ui->h264_stream->setEnabled(false);
        ui->yuv_stream->setEnabled(false);
        ui->mjpeg_stream->setEnabled(false);
    }
    else {
        ui->h264_stream->setEnabled(true);
        ui->yuv_stream->setEnabled(true);
        ui->mjpeg_stream->setEnabled(true);
    }
}

void Widget::mjpeg_selected()
{
    if (ui->mjpeg_stream->isChecked()){
        ui->h264_stream->setEnabled(false);
        ui->h265_stream->setEnabled(false);
        ui->yuv_stream->setEnabled(false);
    }
    else {
        ui->h264_stream->setEnabled(true);
//        ui->h265_stream->setEnabled(true);
        ui->yuv_stream->setEnabled(true);
    }
}

void Widget::update_StartPoint(){
    int satrt_point_x = ui->roi_setting_area->getx1();
    int satrt_point_y = ui->roi_setting_area->gety1();
    cropx1 = tr("%1").arg(satrt_point_x);
    cropy1 = tr("%1").arg(satrt_point_y);
}

void Widget::update_StopPoint(){
    int end_point_x = ui->roi_setting_area->getx2();
    int end_point_y = ui->roi_setting_area->gety2();
    cropx2 = tr("%1").arg(end_point_x);
    cropy2 = tr("%1").arg(end_point_y);

}

void Widget::update_model_1st_list(){
    QSqlQuery query;
    ui->model_1st->clear();
    ui->model_1st->addItem("None");
    query.exec("SELECT * FROM ncc_model where parentId = 1 or parentId = 3");
    while (query.next()){
        ui->model_1st->addItem(QString(query.value("model_name").toString()));
    }
}

void Widget::on_Add_widget_clicked()
{
//    add_blob_file = new add_blob();
    add_blob_file->show();
    emit clear_blob_path();
}

void Widget::show_valid_data(QString msg){

    ui->algo_area->append(msg);

}

void Widget::on_Del_widget_clicked()
{
    del_blob_file = new del_blob();
    del_blob_file->show();
}

void Widget::on_mvfile_currentTextChanged(const QString &arg1)
{
    if (arg1 == "flicRefApp_2232.mvcmd")
    {
        ui->outflow_mode->clear();
        ui->outflow_mode->addItem("1080P");
    }
    else {
        ui->outflow_mode->clear();
        ui->outflow_mode->addItem("1080P");
//        ui->outflow_mode->addItem("4K");
    }
}

void Widget::on_check_device_clicked()
{
    QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
        if (g_run){
            g_run = 0;
            cv::destroyWindow(cvname);
            ui->log_area->append(tr("exit show video"));
            sdk_uninit();
            UninitDecoder();
        }

        int ret;
        //1. 加载固件
        switch (ui->mvfile->currentIndex()) {
        case(0):
            ret=load_fw("./Configuration/moviUsbBoot","./Configuration/fw/flicRefApp_SAMSUNG.mvcmd");
            break;
        case(1):
            ret=load_fw("./Configuration/moviUsbBoot","./Configuration/fw/flicRefApp_MICRON.mvcmd");
            break;
        case(2):
            ret=load_fw("./Configuration/moviUsbBoot","./Configuration/fw/flicRefApp_2485.mvcmd");
            break;
        }
        //flicRefApp_SAMSUNG.mvcmd
        //flicRefApp_MICRON.mvcmd
        //flicRefApp_2485.mvcmd
        if (ret<0)
        {
            QApplication::restoreOverrideCursor();
            ui->log_area->append("load firmware to OpenNCC failed!");
        }
        else {
            usb_version = tr("%1").arg(get_usb_version());
            if (usb_version.toInt()<30){
                ui->yuv_stream->setCheckState(Qt::CheckState(false));
                ui->h264_stream->setCheckState(Qt::CheckState(false));
                ui->mjpeg_stream->setCheckState(Qt::CheckState(false));
                ui->h264_stream->setEnabled(true);
                ui->mjpeg_stream->setEnabled(true);
                ui->yuv_stream->hide();
                ui->log_area->append("USB interface is not 3.0, yuv outflow will be disabled");
            }
            else {
                ui->yuv_stream->setCheckState(Qt::CheckState(false));
                ui->h264_stream->setCheckState(Qt::CheckState(false));
                ui->mjpeg_stream->setCheckState(Qt::CheckState(false));
                ui->h264_stream->setEnabled(true);
                ui->mjpeg_stream->setEnabled(true);
                ui->yuv_stream->setEnabled(true);
                ui->yuv_stream->show();
                ui->log_area->append("USB interface is 3.0, yuv outflow is currently available");
            }
            //2. 获取camera参数
            camera_control_get_features(&list);
            SensorModesConfig features;
            memcpy(&features, &list.mode[0], sizeof(features));
            QString model_name = tr("%1").arg(features.moduleName);
            if (model_name == "sc8238"){
                ui->outflow_mode->clear();
                ui->outflow_mode->addItem("1080P");
                ui->outflow_mode->addItem("4K");
                ui->log_area->append("your device support 1080P and 4K mode.");
            }
            else if(model_name == "sc2232"){
                ui->outflow_mode->clear();
                ui->outflow_mode->addItem("1080P");
                ui->log_area->append("your device support 1080P mode only.");
            }
            QApplication::restoreOverrideCursor();
            sdk_uninit();
}
}
