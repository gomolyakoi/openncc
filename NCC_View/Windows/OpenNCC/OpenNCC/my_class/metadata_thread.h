#ifndef METADATA_THREAD_H
#define METADATA_THREAD_H

#include <QObject>
#include <QTimer>
#include "Fp16Convert.h"



class metadata_thread : public QObject
{
    Q_OBJECT
public:
    explicit metadata_thread(QObject *parent = nullptr);

signals:

private:

public slots:
    void add_log(char *data);


};

#endif // METADATA_THREAD_H
