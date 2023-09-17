#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include <QDateTime>

QString file_path;

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    QString type_mass[]{" ", "Человек", "Машина", "Здание", "Дерево"};
    QString group_by_mass[]{" ", "По расстоянию", "По имени", "По времени создания", "По типу"};

    for(QString type : type_mass)
        ui->obj_type_box->addItem(type);

    for(QString type : group_by_mass)
        ui->group_by_type_box->addItem(type);

}

MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::on_read_button_clicked(){
    file_path = QFileDialog::getOpenFileName(this, "Выбрать файл", "C:\\", "TXT (*.txt);");
    //file_path = "C:/Users/";
    read_file();
}

void MainWindow::read_file() {
    QString from_file = "";

    QFile File(file_path);
    if (File.open(QIODevice::ReadOnly)) {
        QTextStream stream(&File);
        file_objects.clear();
        while (!stream.atEnd()) {
            Object_from_file temp_object;
            if (!temp_object.set_object_from_line(stream.readLine())) {
                file_objects.push_back(temp_object);
                from_file += temp_object.get_name() + " " +
                        QString::number(temp_object.get_x_pos(),'f',3) + " " +
                        QString::number(temp_object.get_y_pos(),'f',3) + " " +
                        temp_object.get_type() + " " +
                        QString::number(temp_object.get_create_time(),'f',3) + "\n";
            }
        }
        ui->textEdit->setText(from_file);
    }

    File.close();
}


void MainWindow::on_add_button_clicked() {
    QString s_name = ui->name_edit->text();
    QString s_x = ui->x_edit->text();
    QString s_y = ui->y_edit->text();
    QString s_type = ui->obj_type_box->currentText();

    if (s_name.size() < 1) {
        ui->label_name->setText("Name (ERROR)");
        return;
    }
    else ui->label_name->setText("Name");

    if(!s_x.toDouble()) {
        ui->label_x->setText("X (ERROR)");
        return;
    }
    else ui->label_x->setText("X");

    if(!s_y.toDouble()) {
        ui->label_y->setText("Y (ERROR)");
        return;
    }
    else ui->label_y->setText("Y");

    if (s_type == " ") {
        ui->label_type->setText("Type (ERROR)");
        return;
    }
    else ui->label_type->setText("Type");

    if (file_path == "") {
        ui->read_label->setText("ERROR: select a file");
        qDebug("ERROR");
        return;
    }
    else ui->read_label->setText(" ");

    QString time = QString::number((QDateTime::currentMSecsSinceEpoch()/1000.0), 'f', 3);
    //qDebug() << time;

    QString data = s_name + " " + s_x + " " + s_y + " " + s_type + " " + time + "\n";

    Object_from_file temp_obj;
    temp_obj.set_object_from_line(data);
    file_objects.push_back(temp_obj);
    QString output = "";
    for(int a = 0; a < file_objects.size(); ++a) {
        output += file_objects[a].get_name() + " " +
                QString::number(file_objects[a].get_x_pos(),'f',3) + " " +
                QString::number(file_objects[a].get_y_pos(),'f',3) + " " +
                file_objects[a].get_type() + " " +
                QString::number(file_objects[a].get_create_time(),'f',3) + "\n";
    }
    ui->textEdit->setText(output);
    QTextCursor cursor = ui->textEdit->textCursor();
    cursor.movePosition(QTextCursor::End);
    ui->textEdit->setTextCursor(cursor);

}


void MainWindow::on_group_button_clicked() {

    if(ui->group_by_type_box->currentText() == " ") {
        ui->group_label->setText("ERROR");
        return;
    } else ui->group_label->setText(" ");

    int mass_size = file_objects.size();

    int number_mass[mass_size];
    for(int a = 0; a < mass_size; ++a)
        number_mass[a] = a;

    if (ui->group_by_type_box->currentText() == "По расстоянию") ui->textEdit->setText(group_by_distance(file_objects));
    else if (ui->group_by_type_box->currentText() == "По имени") ui->textEdit->setText(group_by_name(file_objects));
    else if (ui->group_by_type_box->currentText() == "По времени создания") ui->textEdit->setText(group_by_create_time(file_objects));
    else if (ui->group_by_type_box->currentText() == "По типу") ui->textEdit->setText(group_by_type(file_objects));

}

QString MainWindow::group_by_distance(QVector<Object_from_file>& object_vec) {
    const int obj_size = object_vec.size();
    int num_mass[obj_size];
    double dist_mass[obj_size];

    for (int a = 0; a < object_vec.size(); ++a) {
        dist_mass[a] = sqrt(object_vec[a].get_x_pos()*object_vec[a].get_x_pos() + object_vec[a].get_y_pos() * object_vec[a].get_y_pos());
        num_mass[a] = a;
    }

    obj_sort(dist_mass, num_mass, obj_size);

    QString output = "";
    const int flag_mass_size = 4;
    bool far_flag[flag_mass_size] = {false};
    int far_numbers[flag_mass_size] = {0, 100, 1000, 10000};
    QString far_messages[flag_mass_size] = {"До 100 единиц\n", "До 1000 единиц\n", "До 10000 единиц\n", "Слишком далеко\n"};

    int a = 0;
    for(int b = 0; b < flag_mass_size; ++b) {
        for (a; a < obj_size; ++a) {
            if (((b < (flag_mass_size - 1)) ? (dist_mass[a] > far_numbers[b] && dist_mass[a] < far_numbers[b + 1])
                        :  (dist_mass[a] > far_numbers[b]))) {
                if (!far_flag[b]) {
                    output += far_messages[b];
                    far_flag[b] = true;
                }
                output += object_vec[num_mass[a]].get_name() + " " +
                    QString::number(object_vec[num_mass[a]].get_x_pos(),'f',3) + " " +
                    QString::number(object_vec[num_mass[a]].get_y_pos(),'f',3) + " " +
                    object_vec[num_mass[a]].get_type() + " " +
                    QString::number(object_vec[num_mass[a]].get_create_time(),'f',3) + "\n";
            }
            else break;
        }
    }
    return output;
}

QString MainWindow::group_by_name(QVector<Object_from_file>& object_vec) {
    const int obj_size = object_vec.size();
    QString name_mass[obj_size];
    int num_mass[obj_size];

    for (int a = 0; a < obj_size; ++a) {
        name_mass[a] = object_vec[a].get_name().toUpper();
        num_mass[a] = a;
    }
    obj_sort(name_mass, num_mass, obj_size);

    QString output = "";
    bool A_flag = false;
    bool not_A_flag = false;

    for (int a = 0; a < obj_size; ++a) {
        QString name = object_vec[num_mass[a]].get_name();
        if (name[0].toUpper() == L'А' && !A_flag)
            A_flag = true;
        if (name[0] != L'А' && !not_A_flag) {
            output += "#\n";
            not_A_flag = true;
        }
        else if (name[0].toUpper() != object_vec[num_mass[a - 1]].get_name()[0].toUpper() && A_flag)
            output += "Буква " + (QString)name[0].toUpper() + "\n";

        output += object_vec[num_mass[a]].get_name() + " " +
            QString::number(object_vec[num_mass[a]].get_x_pos(),'f',3) + " " +
            QString::number(object_vec[num_mass[a]].get_y_pos(),'f',3) + " " +
            object_vec[num_mass[a]].get_type() + " " +
            QString::number(object_vec[num_mass[a]].get_create_time(),'f',3) + "\n";

    }
    return output;
}

QString MainWindow::group_by_create_time(QVector<Object_from_file>& object_vec) {
    const int obj_size = object_vec.size();
    double create_time_mass[obj_size];
    int num_mass[obj_size];

    for (int a = 0; a < obj_size; ++a) {
        create_time_mass[a] = object_vec[a].get_create_time();
        num_mass[a] = a;
    }

    obj_sort(create_time_mass, num_mass, obj_size);

    QString output = "";
    const int flag_mass_size = 6;
    bool time_flag[flag_mass_size] = {false};
    QString time_messages[flag_mass_size] = {"Сегодня\n", "Вчера\n", "На этой неделе\n", "В этом месяце\n", "В этом году\n", "Ранее\n"};
    double time_mass[flag_mass_size];
    time_mass[0] = 0;
    time_mass[1] = (QDateTime::currentDateTime().time().hour() * 60 * 60) + (QDateTime::currentDateTime().time().minute() * 60); //today
    time_mass[2] = time_mass[0] + 24 * 60 * 60; //yesterday
    time_mass[3] = (QDateTime::currentDateTime().date().dayOfWeek() - 1) * 24 * 60 * 60; // this week
    time_mass[4] = (QDateTime::currentDateTime().date().day() - 1) * 24 * 60 * 60; // this month
    time_mass[5] = (QDateTime::currentDateTime().date().dayOfYear() - 1) * 24 * 60 * 60; // this year
    double current_time = QDateTime::currentDateTime().currentSecsSinceEpoch();

    int a =  obj_size - 1;
    for(int b = 0; b < flag_mass_size; ++b) {

        for (a; a >= 0; --a) {
            double difference_time = (current_time - create_time_mass[a]);
            if (((b < (flag_mass_size - 1)) ? (difference_time > time_mass[b] && difference_time < time_mass[b + 1])
                                    :  (difference_time > time_mass[b]))) {
                if (!time_flag[b]) {
                    output += time_messages[b];
                    time_flag[b] = true;
                }
                output += object_vec[num_mass[a]].get_name() + " " +
                    QString::number(object_vec[num_mass[a]].get_x_pos(),'f',3) + " " +
                    QString::number(object_vec[num_mass[a]].get_y_pos(),'f',3) + " " +
                    object_vec[num_mass[a]].get_type() + " " +
                    QString::number(object_vec[num_mass[a]].get_create_time(),'f',3) + "\n";
            }
            else break;
        }
    }
    return output;
}

QString MainWindow::group_by_type(QVector<Object_from_file>& object_vec) {
    const int obj_size = object_vec.size();
    QString type_mass[obj_size];
    int num_mass[obj_size];

    for (int a = 0; a < obj_size; ++a) {
        type_mass[a] = object_vec[a].get_type();
        num_mass[a] = a;
    }

    obj_sort(type_mass, num_mass, obj_size);

    int num_of = ui->spinBox_for_type->value();
    if (num_of < 1) {
        ui->group_label->setText("ERROR");
        return " ";
    }
    else ui->group_label->setText(" ");
    QString output = "";
    std::vector<Object_from_file> differents, one_type;
    QString type;

    for (int a = 0; a < obj_size; ) {
        int actual_num = 1;
        type = object_vec[num_mass[a]].get_type();
        for (int b = a + 1; b < obj_size; ++b) {
            if (type == object_vec[num_mass[b]].get_type()) ++actual_num;
            else break;
        }
        if (actual_num >= num_of) {
            for (int b = a; b < obj_size; ++b) {
                if (type == object_vec[num_mass[b]].get_type()) one_type.push_back(object_vec[num_mass[b]]);
                else break;
            }
        }else {
            for (int b = a; b < obj_size; ++b) {
                if (type == object_vec[num_mass[b]].get_type()) differents.push_back(object_vec[num_mass[b]]);
                else break;
            }
        }
        a += actual_num;
        if(!one_type.empty()) {
            output += one_type[0].get_type() + "\n";

            const int type_mass_size = one_type.size();
            QString type_mass[type_mass_size];
            int type_num_mass[type_mass_size];
            for (int с = 0; с < type_mass_size; ++с) {
                type_mass[с] = one_type[с].get_name().toUpper();
                type_num_mass[с] = с;
            }
            obj_sort(type_mass, type_num_mass, type_mass_size);

            for(int b = 0; b < type_mass_size; ++b)
                output += one_type[type_num_mass[b]].get_name() + " " +
                    QString::number(one_type[type_num_mass[b]].get_x_pos(),'f',3) + " " +
                    QString::number(one_type[type_num_mass[b]].get_y_pos(),'f',3) + " " +
                    one_type[type_num_mass[b]].get_type() + " " +
                    QString::number(one_type[type_num_mass[b]].get_create_time(),'f',3) + "\n";
            one_type.clear();
        }
    }
    if(!differents.empty()){
        output += "Разное\n";

        const int diff_mass_size = differents.size();
        QString diff_mass[diff_mass_size];
        int diff_num_mass[diff_mass_size];
        for (int a = 0; a < diff_mass_size; ++a) {
            diff_mass[a] = differents[a].get_name().toUpper();
            diff_num_mass[a] = a;
        }

        obj_sort(diff_mass, diff_num_mass, diff_mass_size);

        for(int a = 0; a < diff_mass_size; ++a ) {
            output += differents[diff_num_mass[a]].get_name() + " " +
                QString::number(differents[diff_num_mass[a]].get_x_pos(),'f',3) + " " +
                QString::number(differents[diff_num_mass[a]].get_y_pos(),'f',3) + " " +
                differents[diff_num_mass[a]].get_type() + " " +
                QString::number(differents[diff_num_mass[a]].get_create_time(),'f',3) + "\n";
        }
    }

    return output;
}

template <class X> void MainWindow::obj_sort(X* items, int* num_vec, int size) {
    obj_sort(items, num_vec, 0, size - 1);
}

template <class X> void MainWindow::obj_sort(X* items, int* num_vec, int left, int right) {
    int i, j, l;
    X x, y;

    i = left; j = right;
    x = items[(left + right) / 2];

    do {
        while ((items[i] < x) && (i < right)) i++;
        while ((x < items[j]) && (j > left)) j--;

        if (i <= j) {
            l = num_vec[i];
            num_vec[i] = num_vec[j];
            num_vec[j] = l;
            y = items[i];
            items[i] = items[j];
            items[j] = y;
            i++;
            j--;
        }
    } while (i <= j);
    if (left < j) obj_sort(items, num_vec, left, j);
    if (i < right) obj_sort(items, num_vec, i, right);

}

void MainWindow::on_save_button_clicked() {
    QString data = ui->textEdit->toPlainText();

    QFile File(file_path);
    if (File.open(QIODevice::Truncate | QIODevice::ReadWrite)) {
        QTextStream stream(&File);
        stream << data;
    }
    File.close();
    read_file();

    ui->read_label->setText("Saved");
}

