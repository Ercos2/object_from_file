#include "thread_class.h"
#include "processes_pool.h"

Thread_class::Thread_class(QObject* parent) : QObject(parent)
{

}

void Thread_class::makeThread(QVector<Object_from_file>* file_objects, QString type, int num_of) {

    Processes_pool* pool = new Processes_pool;

    pool->group_type = type;
    pool->pool_object = *file_objects;
    pool->num_of = num_of;

    connect(pool, &Processes_pool::process_finished, [this] (QString text) {
        //qDebug() << "Seccuse";
        emit output(text);
        qDebug() << "over " << QThreadPool::globalInstance()->activeThreadCount();
    });

    QThreadPool::globalInstance()->start(pool);
    qDebug() << QThreadPool::globalInstance()->activeThreadCount();
}
