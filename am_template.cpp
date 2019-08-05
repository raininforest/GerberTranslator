#include "am_template.h"

am_template::am_template(const QString name_of_macro, const QStringList data_blocks)
{
    all_data_blocks = data_blocks;
    name = name_of_macro;
}

QString am_template::get_name(){
    return name;
}

QStringList am_template::get_data_blocks(){
    return all_data_blocks;
}


