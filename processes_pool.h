#ifndef PROCESSES_POOL_H
#define PROCESSES_POOL_H

#include <QObject>
#include <QThreadPool>
#include "object_from_file.h"
#include "group_and_sort.h"
#include <QThreadPool>

class Processes_pool : public QObject, public QRunnable
{
    Q_OBJECT
public:
    explicit Processes_pool(QObject *parent = 0);
    ~Processes_pool();

    QVector<Object_from_file> pool_object;

    QString group_type;

    Group* group;

    int num_of;

    void run();

signals:
    void process_finished(QString);
};

#endif // PROCESSES_POOL_H
