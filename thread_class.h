#ifndef THREAD_CLASS_H
#define THREAD_CLASS_H

#include <QObject>
#include "object_from_file.h"

class Thread_class : public QObject
{
    Q_OBJECT
public:
    Thread_class(QObject *perant = nullptr);

    void makeThread(QVector<Object_from_file>* file_objects, QString type, int num_of);

signals:
    void output(QString);
};

#endif // THREAD_CLASS_H
