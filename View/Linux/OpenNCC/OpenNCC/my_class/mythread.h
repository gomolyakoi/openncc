#ifndef MYTHREAD_H
#define MYTHREAD_H

#include <QObject>

class MyThread : public QObject
{
    Q_OBJECT
public:
    explicit MyThread(QObject *parent = nullptr);

    void load_2net_model(int type,int modeId);

    int Post_ProcessFrame(int type,char* pFrame,int bufsize,char* winhndl);

signals:
    void send_log_info_to_UI(QString msg);
};

#endif // MYTHREAD_H
