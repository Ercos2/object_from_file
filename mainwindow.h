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
#include "group_and_sort.h"
#include "thread_class.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    friend Group;

    //template <class X> friend void obj_sort(X* items, int* num_vec, int size);

    //template <class X> friend void obj_sort(X* items, int* num_vec, int left, int right);

private slots:
    void on_read_button_clicked();

    void on_add_button_clicked();

    void on_group_button_clicked();

    void on_save_button_clicked();

private:
    Ui::MainWindow *ui;

    Thread_class *thread;

    QVector<Object_from_file>* file_objects = new QVector<Object_from_file>;

signals:
    void output(QString);
};
#endif // MAINWINDOW_H
