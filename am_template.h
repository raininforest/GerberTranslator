#ifndef AM_TEMPLATE_H
#define AM_TEMPLATE_H

#include <QStringList>

class am_template
{

    QString name;                // имя макро шаблона
    QStringList all_data_blocks; // строковый массив из дата блоков, которые описывают макро шаблон

public:

    am_template(const QString name_of_macro, const QStringList data_blocks);
    QString get_name();
    QStringList get_data_blocks();

};

#endif // AM_TEMPLATE_H
