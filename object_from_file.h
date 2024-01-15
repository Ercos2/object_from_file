#ifndef OBJECT_FROM_FILE_H
#define OBJECT_FROM_FILE_H

#include <QMainWindow>

class Object_from_file
{
public:
    Object_from_file();

    bool set_object_from_line(QString line);

    QString get_name() const;
    double get_x_pos() const;
    double get_y_pos() const;
    QString get_type() const;
    double get_create_time() const;
    QString get_output() const;
private:
    QString name = "";
    double x_pos;
    double y_pos;
    QString type = "";
    double create_time;
};

#endif // OBJECT_FROM_FILE_H
