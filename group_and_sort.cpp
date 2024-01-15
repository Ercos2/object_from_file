#include "group_and_sort.h"
//#include <algorithm>

Group::Group(QObject *parent) : QObject(parent) {}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// group-operation block
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*
bool comparator_by_distance (const Object_from_file &item1, const Object_from_file &item2) {
    return ((item1.get_x_pos() * item1.get_y_pos()) > (item2.get_x_pos() * item2.get_y_pos()));
}

bool comparator_by_name (const Object_from_file &item1, const Object_from_file &item2) {
    return (item1.get_name() > item2.get_name());
}


bool reverse_comparator_by_create_time (const Object_from_file &item1, const Object_from_file &item2) {
    return (item1.get_create_time() < item2.get_create_time());
}

bool comparator_by_type (const Object_from_file &item1, const Object_from_file &item2) {
    if (item1.get_type() == item2.get_type())
        return (item1.get_name() > item2.get_name());
    else
        return (item1.get_type() > item2.get_type());
}
*/

QString Group::group_by_distance(QVector<Object_from_file>& object_vec) {
    const int obj_size = object_vec.size();
    //I heard that std::shared_ptr has problems with standard memory cleanup when creating an array
    //and I used the lambda function that was offered there
    std::shared_ptr<int[]> num_mass(new int[obj_size], [] (int *i) {delete[] i;});
    std::shared_ptr<double[]> dist_mass(new double[obj_size], [] (double *i) {delete[] i;});

    for (int a = 0; a < object_vec.size(); ++a) {
        dist_mass[a] = sqrt(object_vec[a].get_x_pos()*object_vec[a].get_x_pos() +
                            object_vec[a].get_y_pos() * object_vec[a].get_y_pos());     //find the hypotenuse to determine the distance to the object
        num_mass[a] = a;
    }
                                                                                        //passing the vector of objects entailed too many problems,
                                                                                        //so I created a couple of separate arrays in each grouping function:
                                                                                        //data for sorting and numbers in order to know how the new list should be located relative to the old one
                                                                                        //and based on this, arrange a full-fledged set of objects
                                                                                        //crutches? Maybe, but it's fast and intuitive, it seems to me
    obj_sort(dist_mass, num_mass, obj_size);

    auto output = std::make_unique<QString>("");
    const int flag_mass_size = 4;
    auto far_flag = std::unique_ptr<bool[]>(new bool[flag_mass_size]{false});
    auto far_numbers = std::unique_ptr<int[]>(new int[flag_mass_size]{0, 100, 1000, 10000});
    auto far_messages = std::unique_ptr<QString[]>(new QString[flag_mass_size]{"До 100 единиц\n", "До 1000 единиц\n", "До 10000 единиц\n", "Слишком далеко\n"});
                                                                                        //I really didn't want to write a separate "if" and "for" for each group
    int a = 0;
    for(int b = 0; b < flag_mass_size; ++b) {
        for (a; a < obj_size; ++a) {
            if (((b < (flag_mass_size - 1)) ? (dist_mass[a] > far_numbers[b] && dist_mass[a] < far_numbers[b + 1])     //verification within verification. I'm trying this for the first time
                        :  (dist_mass[a] > far_numbers[b]))) {
                if (!far_flag[b]) {
                    *output += far_messages[b];
                    far_flag[b] = true;
                }
                *output += object_vec[num_mass[a]].get_output();
            }
            else break;
        }
    }

    return *output;
}

QString Group::group_by_name(QVector<Object_from_file>& object_vec) {
    const int obj_size = object_vec.size();
    std::shared_ptr<QString[]> name_mass(new QString[obj_size], [] (QString *i) {delete[] i;});
    std::shared_ptr<int[]> num_mass(new int[obj_size], [] (int *i) {delete[] i;});

    //qDebug() << "Seccuse befor for";

    for (int a = 0; a < obj_size; ++a) {
        name_mass[a] = object_vec[a].get_name().toUpper();              //we use the ToUpper() function so that lowercase letters are not separated from uppercase ones
        num_mass[a] = a;
    }

    //qDebug() << "Seccuse befor sort";

    obj_sort(name_mass, num_mass, obj_size);

    //qDebug() << "Seccuse after sort";

    auto output = std::make_unique<QString>("");
    auto A_flag = std::make_unique<bool>(false);
    auto not_A_flag = std::make_unique<bool>(false);

    for (int a = 0; a < obj_size; ++a) {
        auto name =  std::make_unique<QString>(object_vec[num_mass[a]].get_name());
        if ((*name)[0].toUpper() == L'А' && !*A_flag)
            *A_flag = true;
        if ((*name)[0] != L'А' && !*not_A_flag) {
            *output += "#\n";
            *not_A_flag = true;
        }
        else if ((*name)[0].toUpper() != object_vec[num_mass[a - 1]].get_name()[0].toUpper() && *A_flag)
            *output += "Буква " + (QString)(*name)[0].toUpper() + "\n";                 //initially I tried to use the alphabet for grouping,
                                                                                    //but then it turned out that Qt practically does not know how to work with the Russian test
                                                                                    //and I had to adapt

        *output += object_vec[num_mass[a]].get_output();
    }
    return *output;
}

QString Group::group_by_create_time(QVector<Object_from_file>& object_vec) {

    auto temp_vec = std::make_shared<QVector<Object_from_file>>(object_vec);
    // use std::sort and custom reverse comporatare for result
    std::sort((*temp_vec).begin(), (*temp_vec).end(),
              [] (const Object_from_file &item1, const Object_from_file &item2) {
        return (item1.get_create_time() > item2.get_create_time());
    });

    auto output = std::make_unique<QString>("");
    const int flag_mass_size = 6;
    auto time_flag = std::make_unique<QVector<bool>>(flag_mass_size, false);
    auto time_messages = std::make_unique<QVector<QString>>();
    (*time_messages) = {"Сегодня\n", "Вчера\n", "На этой неделе\n", "В этом месяце\n", "В этом году\n", "Ранее\n"};
    auto time_mass = std::make_unique<std::vector<double>>();

    time_mass->push_back(0);
    time_mass->push_back((QDateTime::currentDateTime().time().hour() * 60 * 60) + (QDateTime::currentDateTime().time().minute() * 60)); //today
    time_mass->push_back((*time_mass)[0] + 24 * 60 * 60); //yeasterday
    time_mass->push_back((QDateTime::currentDateTime().date().dayOfWeek() - 1) * 24 * 60 * 60); //this week
    time_mass->push_back((QDateTime::currentDateTime().date().day() - 1) * 24 * 60 * 60); // this month
    time_mass->push_back((QDateTime::currentDateTime().date().dayOfYear() - 1) * 24 * 60 * 60); // this year

    double current_time = QDateTime::currentDateTime().currentSecsSinceEpoch();

    int last_iter = 0;
    //the outer loop runs through the array objects, while the inner loop runs through possible groups
    for(auto &temp_obj : (*temp_vec)) {
        //the array of objects is sorted in advance,
        //so it makes no sense to check all the groups over and over again,
        //so we keep the last correct one and act on it as "last_iter"
        for (last_iter; last_iter < flag_mass_size; ++last_iter) {
            double difference_time = (current_time - temp_obj.get_create_time());
            if (((last_iter < (flag_mass_size - 1)) ? (difference_time > (*time_mass)[last_iter] && difference_time < (*time_mass)[last_iter + 1])
                                            :  (difference_time > (*time_mass)[last_iter]))) {
                if (!(*time_flag)[last_iter]) {
                    *output += (*time_messages)[last_iter];
                    (*time_flag)[last_iter] = true;
                }
                *output += temp_obj.get_output();
                //If an element belongs to one group,
                //then there is no point in checking it for belonging to others,
                //so we interrupt the cycle
                break;
            }
        }
    }

    return *output;
}

QString Group::group_by_type(QVector<Object_from_file>& object_vec, int num_of) {          //the last function that I was finishing by 4 o'clock in the morning,
                                                                                    //I wanted to sleep, I couldn't think,
                                                                                    //but it turned out, not bad, this is the best option that I came up with
                                                                                    //and, most importantly, IT WORKS HAHAHAHAHAHA, although there is a place to grow
    const int obj_size = object_vec.size();
    std::shared_ptr<QString[]> type_mass(new QString[obj_size], [] (QString *i) {delete[] i;});
    std::shared_ptr<int[]> num_mass(new int[obj_size], [] (int *i) {delete[] i;});

    for (int a = 0; a < obj_size; ++a) {
        type_mass[a] = object_vec[a].get_type();
        num_mass[a] = a;
    }

    obj_sort(type_mass, num_mass, obj_size);

    auto output = std::make_unique<QString>("");
    auto differents = std::make_unique<std::vector<Object_from_file>>();
    auto one_type = std::make_unique<std::vector<Object_from_file>>();
    auto type = std::make_unique<QString>();

    for (int a = 0; a < obj_size; ) {
        int actual_num = 1;
        *type = object_vec[num_mass[a]].get_type();
        for (int b = a + 1; b < obj_size; ++b) {
            if (*type == object_vec[num_mass[b]].get_type()) ++actual_num;
            else break;
        }
        if (actual_num >= num_of) {
            for (int b = a; b < obj_size; ++b) {
                if (*type == object_vec[num_mass[b]].get_type()) one_type->push_back(object_vec[num_mass[b]]);
                else break;
            }
        }else {
            for (int b = a; b < obj_size; ++b) {
                if (*type == object_vec[num_mass[b]].get_type()) differents->push_back(object_vec[num_mass[b]]);
                else break;
            }
        }
        a += actual_num;
        if(!one_type->empty()) {
            *output += (*one_type)[0].get_type() + "\n";

            const int type_mass_size = one_type->size();
            std::shared_ptr<QString[]> type_mass(new QString[obj_size], [] (QString *i) {delete[] i;});
            std::shared_ptr<int[]> type_num_mass(new int[obj_size], [] (int *i) {delete[] i;});
            for (int с = 0; с < type_mass_size; ++с) {
                type_mass[с] = (*one_type)[с].get_name().toUpper();
                type_num_mass[с] = с;
            }
            obj_sort(type_mass, type_num_mass, type_mass_size);

            for(int b = 0; b < type_mass_size; ++b)
                *output += (*one_type)[type_num_mass[b]].get_output();
            one_type->clear();
        }
    }
    if(!differents->empty()){
        *output += "Разное\n";

        const int diff_mass_size = differents->size();
        std::shared_ptr<QString[]> diff_mass(new QString[obj_size], [] (QString *i) {delete[] i;});
        std::shared_ptr<int[]> diff_num_mass(new int[obj_size], [] (int *i) {delete[] i;});
        for (int a = 0; a < diff_mass_size; ++a) {
            diff_mass[a] = (*differents)[a].get_name().toUpper();
            diff_num_mass[a] = a;
        }
        obj_sort(diff_mass, diff_num_mass, diff_mass_size);

        for(int a = 0; a < diff_mass_size; ++a ) {
            *output += (*differents)[diff_num_mass[a]].get_output();
        }
    }

    return *output;
}


template <class X> void obj_sort(std::shared_ptr<X> items, std::shared_ptr<int[]> num_vec, int left, int right) {
    int i, j, l;

    //the usual creation via
    //X x, y;
    //did not work because X in this case is an array.
    //I had to a little cheat
    auto x = items[0];
    auto y = items[0];

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

template <class X> void obj_sort(std::shared_ptr<X> items, std::shared_ptr<int[]> num_vec, int size) {            //I would say that I made such a function so as not to repeat one line several times,
                                                                                            //but, in fact, I just had QuickSort written in this form for a long time
                                                                                            //and I just copied and modified it for current needs
    obj_sort(items, num_vec, 0, size - 1);
}
