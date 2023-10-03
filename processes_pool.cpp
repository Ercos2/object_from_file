#include "processes_pool.h"
//#include "mainwindow.h"
#include "group_and_sort.h"

Processes_pool::Processes_pool(QObject *parent) : QObject(parent)
{

}

Processes_pool::~Processes_pool()
{
    delete group;
}

void Processes_pool::run() {
    group = new Group();

    if (group == nullptr) {
        emit process_finished("errore");
        return;
    }
    //qDebug() << "send signal";
    if (group_type == "По расстоянию")
        emit process_finished(group->group_by_distance(pool_object));
    else if (group_type == "По имени")
        emit process_finished(group->group_by_name(pool_object));
    else if (group_type == "По времени создания")
        emit process_finished(group->group_by_create_time(pool_object));
    else if (group_type == "По типу")
        emit process_finished(group->group_by_type(pool_object, num_of));
}
