#include "object_from_file.h"

Object_from_file::Object_from_file()
{

}

bool Object_from_file::set_object_from_line(QString line) {
    int param_num = 0;
    QString temp_x_pos = "", temp_y_pos = "", temp_time = "";
    for(auto a = 0; a < line.size(); a++) {
        if (line[a] != ' ' && param_num == 0) name += line[a];
        else if (line[a] != ' ' && param_num == 1) temp_x_pos += line[a];
        else if (line[a] != ' ' && param_num == 2) temp_y_pos += line[a];
        else if (line[a] != ' ' && param_num == 3) type += line[a];
        else if (line[a] != ' ' && param_num == 4) temp_time += line[a];

        if(line[a] == ' ') ++param_num;
    }
    x_pos = temp_x_pos.toDouble();
    y_pos = temp_y_pos.toDouble();
    create_time = temp_time.toDouble();

    if (name == "" || temp_x_pos == "" || temp_y_pos == "" || type == "" || temp_time == "") return 1;
    else {
        //qDebug("Object been inicieted");
        return 0;

    }
}

QString Object_from_file::get_name() {
    return name;
}
double Object_from_file::get_x_pos() {
    return x_pos;
}
double Object_from_file::get_y_pos() {
    return y_pos;
}
QString Object_from_file::get_type() {
    return type;
}
double Object_from_file::get_create_time() {
    return create_time;
}
