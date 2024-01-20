#include "group_and_sort.h"

std::mutex obj_lock;

Group::Group(QObject *parent) : QObject(parent) {}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// group-operation block
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

QString Group::group_by_distance(QVector<Object_from_file>& object_vec) {
    obj_lock.lock();
    auto temp_vec = std::make_shared<QVector<Object_from_file>>(object_vec);
    obj_lock.unlock();
    // use std::sort and custom reverse comporatare for result
    std::sort((*temp_vec).begin(), (*temp_vec).end(),
              [] (const Object_from_file &item1, const Object_from_file &item2) {
        return (sqrt(item1.get_x_pos() * item1.get_x_pos() + item1.get_y_pos() * item1.get_y_pos()) <
                            sqrt(item2.get_x_pos() * item2.get_x_pos() + item2.get_y_pos() * item2.get_y_pos()));
              });

    auto output = std::make_unique<QString>("");
    const int flag_mass_size = 4;

    auto far_flag = std::make_unique<QVector<bool>>(flag_mass_size, false);
    auto far_numbers = std::make_unique<QVector<int>>();
    (*far_numbers) = {0, 100, 1000, 10000};
    auto far_messages = std::make_unique<std::vector<QString>>();
    (*far_messages) = {"До 100 единиц\n", "До 1000 единиц\n", "До 10000 единиц\n", "Слишком далеко\n"};

    int a = 0;
    for(int b = 0; b < flag_mass_size; ++b) {
        for (a; a < temp_vec->size(); ++a) {
            auto dist = sqrt((*temp_vec)[a].get_x_pos() * (*temp_vec)[a].get_x_pos() +
                             (*temp_vec)[a].get_y_pos() * (*temp_vec)[a].get_y_pos());
            if (((b < (flag_mass_size - 1)) ? (dist > (*far_numbers)[b] && dist < (*far_numbers)[b + 1])     //verification within verification. I'm trying this for the first time
                                            :  (dist > (*far_numbers)[b]))) {
                if (!(*far_flag)[b]) {
                    *output += (*far_messages)[b];
                    (*far_flag)[b] = true;
                }
                *output += (*temp_vec)[a].get_output();
            }
            else break;
        }
    }

    return *output;
}

QString Group::group_by_name(QVector<Object_from_file>& object_vec) {
    obj_lock.lock();
    auto temp_vec = std::make_shared<QVector<Object_from_file>>(object_vec);
    obj_lock.unlock();
    // use std::sort and custom reverse comporatare for result
    std::sort((*temp_vec).begin(), (*temp_vec).end(),
              [] (const Object_from_file &item1, const Object_from_file &item2) {
        return (item1.get_name().toUpper() < item2.get_name().toUpper());
              });

    auto output = std::make_unique<QString>("");
    auto A_flag = std::make_unique<bool>(false);
    auto not_A_flag = std::make_unique<bool>(false);

    for (int a = 0; a < (*temp_vec).size(); ++a) {
        auto name =  std::make_unique<QString>((*temp_vec)[a].get_name());
        if ((*name)[0].toUpper() == L'А' && !*A_flag)
            *A_flag = true;
        if ((*name)[0] != L'А' && !*not_A_flag) {
            *output += "#\n";
            *not_A_flag = true;
        }
        else if ((*name)[0].toUpper() != (*temp_vec)[a - 1].get_name()[0].toUpper() && *A_flag)
            *output += "Буква " + (QString)(*name)[0].toUpper() + "\n";                 //initially I tried to use the alphabet for grouping,
                                                                                    //but then it turned out that Qt practically does not know how to work with the Russian test
                                                                                    //and I had to adapt

        *output += (*temp_vec)[a].get_output();
    }
    return *output;
}

QString Group::group_by_create_time(QVector<Object_from_file>& object_vec) {
    obj_lock.lock();
    auto temp_vec = std::make_shared<QVector<Object_from_file>>(object_vec);
    obj_lock.unlock();
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

QString Group::group_by_type(QVector<Object_from_file>& object_vec, int num_of) {
    obj_lock.lock();
    auto temp_vec = std::make_shared<QVector<Object_from_file>>(object_vec);
    obj_lock.unlock();
    // use std::sort and custom reverse comporatare for result
    // within groups by type, we need to sort objects by name
    // so we do in same function
    std::sort((*temp_vec).begin(), (*temp_vec).end(),
              [] (const Object_from_file &item1, const Object_from_file &item2) {
        if (item1.get_type() == item2.get_type())
            return item1.get_name().toUpper() < item2.get_name().toUpper();
        return (item1.get_type() < item2.get_type());
              });

    auto output = std::make_unique<QString>("");
    // objects of groups smaller than the threshold value are placed in a separate group
    auto differents = std::make_unique<std::vector<Object_from_file>>();
    auto type = std::make_unique<QString>();

    // the size of the vector is used many times in the loop
    //so we put it into a variable once and not calling the size function many times
    auto obj_size = temp_vec->size();

    for (int a = 0; a < obj_size; ) {
        // actual_num displays how many times an object of the same type occurs.
        // It is set to 1 because each type being read occurs at least 1 time
        int actual_num = 1;
        *type = (*temp_vec)[a].get_type();
        for (int b = a + 1; b < obj_size; ++b) {
            if (*type == (*temp_vec)[b].get_type()) ++actual_num;
            else break;
        }
        // if the size of the variable is greater than the threshold value,
        // then objects of this type are entered into output,
        if (actual_num >= num_of) {
            *output += (*temp_vec)[a].get_type() + "\n";
            for (int b = a; b < obj_size; ++b) {
                if (*type == (*temp_vec)[b].get_type()) {
                    *output += (*temp_vec)[b].get_output();
                }
                else break;
            }
        //and if smaller, they are entered into the vector for further sorting and output
        }else {
            for (int b = a; b < obj_size; ++b) {
                if (*type == (*temp_vec)[b].get_type()) differents->push_back((*temp_vec)[b]);
                else break;
            }
        }
        // increasing the loop variable by the number of objects of the same type
        // to skip objects of this type in the following iterations
        a += actual_num;
    }

    if(!differents->empty()){
        std::sort(differents->begin(), differents->end(),
                  [] (const Object_from_file &item1, const Object_from_file &item2) {
                          return item1.get_name().toUpper() < item2.get_name().toUpper();
                  });

        *output += "Разное\n";
        for(int a = 0; a < differents->size(); ++a ) {
            *output += (*differents)[a].get_output();
        }
    }

    return *output;
}
