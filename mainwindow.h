#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QFile>
#include <QDir>
#include <QDebug>
#include <QString>
#include <QTextStream>
#include <QFileDialog>
#include "object_from_file.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_read_button_clicked();

    void on_add_button_clicked();

    void on_group_button_clicked();

    void on_save_button_clicked();

private:
    Ui::MainWindow *ui;

    QVector<Object_from_file> file_objects;

    QString group_by_distance(QVector<Object_from_file>& object_vec);

    QString group_by_name(QVector<Object_from_file>& object_vec);

    QString group_by_create_time(QVector<Object_from_file>& object_vec);

    QString group_by_type(QVector<Object_from_file>& object_vec);

    //Object_from_file* file_objects;

    void read_file();

    template <class X> void obj_sort(X* items, int* num_vec, int size);

    template <class X> void obj_sort(X* items, int* num_vec, int left, int right);
};
#endif // MAINWINDOW_H
