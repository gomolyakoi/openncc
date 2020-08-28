#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <QSqlDatabase>
#include <QtSql>
#include <QMessageBox>
#include <QFile>
#include <QTextBrowser>
#include <QPushButton>
#include <QComboBox>
#include <QDebug>
#include <QString>
#include <QThread>
#include <QTimer>
#include "my_class/roi_label.h"
#include "my_class/add_blob.h"
#include "my_class/del_blob.h"
#include "my_class/metadata_thread.h"
#include "sdk.h"

QT_BEGIN_NAMESPACE
namespace Ui { class Widget; }
QT_END_NAMESPACE

class Widget : public QWidget
{
    Q_OBJECT

public:
    Widget(QWidget *parent = nullptr);
    ~Widget();

signals:
    void clear_blob_path();
    void get_valid_algo_data(int w,int h,char *data,float score);
    void close_mydata_thread();
    void stop_mydata_thread_timer();

public slots:
    void update_model_1st_list();

    void download_model_file(QString model_name);


private slots:
    void on_load_fw_btn_clicked();
    //model type
    void model_1st_changed();
    void model_2nd_changed();
    //stream encoding
    void yuv_selected();
    void h264_selected();
    void h265_selected();
    void mjpeg_selected();
    //update coordinate
    void compare_coordinate(int a,int b);
    void update_StartPoint();
    void update_StopPoint();
    //creat thread to load fw and preview
    void show_valid_data(QString msg);

    void on_Add_widget_clicked();

    void on_Del_widget_clicked();

    void close_data_thread();

    void on_mvfile_currentTextChanged(const QString &arg1);

    void on_check_device_clicked();

    QString current_time();

private:
    int NCCVideoCtrl(int video_type,int en);
    int PostProcessFrame(char* pFrame,int bufsize,float min_score,float scale,char* winhndl);

private:
    Ui::Widget *ui;
    metadata_thread *mydata_thread;
    QThread *data_thread;
    add_blob *add_blob_file;
    del_blob *del_blob_file;

    int mvideo_type;
    int mh265en;
    int msensorModeId;

};
#endif // WIDGET_H
