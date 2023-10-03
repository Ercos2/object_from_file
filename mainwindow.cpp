#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include <QMessageBox>>

QString file_path;

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    QString type_mass[]{" ", "Человек", "Машина", "Здание", "Дерево"};      //filling in the lists
    QString group_by_mass[]{" ", "По расстоянию", "По имени", "По времени создания", "По типу"};

    for(QString type : type_mass)
        ui->obj_type_box->addItem(type);

    for(QString type : group_by_mass)
        ui->group_by_type_box->addItem(type);

    thread = new Thread_class;

    connect(thread, &Thread_class::output, this, &MainWindow::output);
    connect(this, &MainWindow::output, [this] (QString text) {
        //qDebug() << "Seccuse";
        ui->textEdit->setText(text);
        //qDebug() << "Seccuse after setText()";
    });
}

MainWindow::~MainWindow()
{
    delete file_objects;
    delete thread;
    delete ui;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// GUI operation block
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void MainWindow::on_read_button_clicked(){
    file_path = QFileDialog::getOpenFileName(this, "Выбрать файл", "C:\\", "TXT (*.txt);");     //dialog box for selecting a file
    //file_path = "C:/Users/dimak/Desktop/Qt projects/object_from_file/file.txt";               //no dialog box for debugging
    QString from_file = "";

    QFile File(file_path);
    if (File.open(QIODevice::ReadOnly)) {
        QTextStream stream(&File);
        file_objects->clear();
        while (!stream.atEnd()) {
            Object_from_file temp_object;
            if (!temp_object.set_object_from_line(stream.readLine())) {             //initialize new objects using strings.
                                                                                    //If initialization is successful, the function will return 0
                                                                                    //Adding objects to a string for output in TextEdit
                file_objects->push_back(temp_object);
                from_file += temp_object.get_output();
            }
        }
        ui->textEdit->setText(from_file);
    }
    File.close();
}

void MainWindow::on_add_button_clicked() {
    auto s_name = std::make_unique<QString>(QString(ui->name_edit->text()));
    auto s_x = std::make_unique<QString>(QString(ui->x_edit->text()));
    auto s_y = std::make_unique<QString>(QString(ui->y_edit->text()));
    auto s_type = std::make_unique<QString>(QString(ui->obj_type_box->currentText()));

    if (s_name->size() < 1) {
        ui->label_name->setText("Name (ERROR)");            //checking the filling to add a new element
        return;
    }
    else ui->label_name->setText("Name");

    if(!s_x->toDouble()) {
        ui->label_x->setText("X (ERROR)");
        return;
    }
    else ui->label_x->setText("X");

    if(!s_y->toDouble()) {
        ui->label_y->setText("Y (ERROR)");
        return;
    }
    else ui->label_y->setText("Y");

    if (*s_type == " ") {
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

    auto time = std::make_unique<QString>(QString(QString::number((QDateTime::currentMSecsSinceEpoch()/1000.0), 'f', 3)));       //we find out the time in MSecs and divide by 1000 to get Secs with a remainder in MSecs

    //qDebug() << time;

    auto data = std::make_unique<QString>(QString(*s_name + " " + *s_x + " " + *s_y + " " + *s_type + " " + *time + "\n"));       //previously, this line was used for output to TextEdit,
                                                                                            //but it was decided to leave it so as not to prescribe code
                                                                                            //for creating an element by separate parameters

    auto temp_obj = std::make_unique<Object_from_file>();
    temp_obj->set_object_from_line(*data);
    file_objects->push_back(*temp_obj);
    auto output = std::make_unique<QString>(QString(""));
    for(int a = 0; a < file_objects->size(); ++a) {
        *output += (*file_objects)[a].get_output();                                    //we enter the current result in the line
    }
    ui->textEdit->setText(*output);                                                      //output a string in TextEdit
    auto cursor = std::make_unique<QTextCursor>(QTextCursor(ui->textEdit->textCursor()));
    cursor->movePosition(QTextCursor::End);                                              //move the cursor down to see the newly added element.
    ui->textEdit->setTextCursor(*cursor);                                                //Just because it's more convenient and more beautiful

}


void MainWindow::on_group_button_clicked() {

    if(ui->group_by_type_box->currentText() == " ") {
        ui->group_label->setText("ERROR");
        return;
    } else ui->group_label->setText(" ");

    int num_of = ui->spinBox_for_type->value();
    if (ui->group_by_type_box->currentText() == "По типу" && num_of < 1) {
        ui->group_label->setText("ERROR");
        return;
    }
    else ui->group_label->setText(" ");

    thread->makeThread(file_objects, ui->group_by_type_box->currentText(), num_of);

}

void MainWindow::on_save_button_clicked() {
    if (file_path.size() < 1) {
        QMessageBox::warning(NULL, "Warning", "No path");
        return;
    }

    auto data = std::make_unique<QString>("");
    for(int a = 0; a < file_objects->size(); ++a) {
        *data += (*file_objects)[a].get_output();
    }

    auto File = std::make_unique<QFile>(file_path);
    if (File->open(QIODevice::Truncate | QIODevice::ReadWrite)) {
        QTextStream stream(File.get());
        stream << *data;

    }
    File->close();
    QMessageBox::about(NULL, "Seccuss", "Saved");
}
