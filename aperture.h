#ifndef APERTURE_H
#define APERTURE_H

#include "am_template.h"
#include <QString>
#include <QPainter>
#include <QTextStream>

class Aperture
{

    am_template* my_am_template;    //  указатель на макрошаблон, определенный в AM

    //  Параметры апертуры:
    int d_code = -1;                //  номер апертуры
    QString name_of_template = "";  //  имя шаблона апертуры
    QString type_of_template = "";  //  STANDART_C, R, O, P or MACRO
    QString modifiers = "";         //  строка с параметрами, необходимыми для рисования(размеры и т д).
    QStringList mod_list;           //  строковый массив параметров по отдельности, разделенных "Х"

    //  Формат координат:
    int x_int;  //  количество разрядов под целую часть
    int x_dec;  //  количество разрядов под дробную часть
    int y_int;  //  количество разрядов под целую часть
    int y_dec;  //  количество разрядов под дробную часть

    struct primitive_struct{
        QPainterPath path;                  //  изображение примитива
        float rotation = 0;                 //  угол поворота
        bool std_aperture = 1;              //  флаг стандартной апертуры (если 1 - не нужно брать угол и вращать)
    };

    QList <primitive_struct> primitives;    //список примитивов для макрошаблона

    struct variable{
        int index=0;    //имя (номер) переменной в гербере
        float value=0;  //значение переменной
    };

    //  вычисление выражения в модификаторах описания примитива из макро-шаблона
    float calculate_expression(const QString expression, QList<variable>* dict);

public:
    //  конструктор:
    Aperture(const int d_code_number_of_aperture = 0, const QString name_of_temp = "", const QString type_of_temp = "", const QString modifs = "", am_template* am_temp = nullptr);

    int get_d_code();                   //  получить номер апертуры
    QString get_name();                 //  получить имя апертуры
    QString get_type();                 //  получить тип апертуры (4 типа стандартных или макро)

    void create(const int dpi);                                         //  создание апертуры
    int draw_me(const int x_pos, const int y_pos, QPainter* painter);   //  отрисовка изображения апертуры
    int get_std_circ_dia_in_px(const int dpi);                          //  получить диаметр стандартной апертуры-окружности для рисования линий

};

#endif // APERTURE_H
