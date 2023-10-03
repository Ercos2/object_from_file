#ifndef GROUP_AND_SORT_H
#define GROUP_AND_SORT_H

#include <QDateTime>
#include "object_from_file.h"

class Group : public QObject {
    Q_OBJECT
public:

    Group(QObject *parent = 0);

    QString group_by_distance(QVector<Object_from_file>& object_vec);

    QString group_by_name(QVector<Object_from_file>& object_vec);

    QString group_by_create_time(QVector<Object_from_file>& object_vec);

    QString group_by_type(QVector<Object_from_file>& object_vec, int num_of);

signals:
    void done(QString);
};

template <class X> void obj_sort(std::shared_ptr<X> items, std::shared_ptr<int[]> num_vec, int size);

template <class X> void obj_sort(std::shared_ptr<X> items, std::shared_ptr<int[]> num_vec, int left, int right);

#endif // GROUP_AND_SORT_H
