#ifndef APERTURE_H
#define APERTURE_H

#include "am_template.h"
#include <QString>
#include <QPainter>
#include <QTextStream>

class Aperture
{

    am_template* my_am_template;    //  macro template pointer

    //Aperure properties:
    int d_code = -1;                //  aperture number
    QString name_of_template = "";  //  aperture name
    QString type_of_template = "";  //  aperture type: STANDART_C, R, O, P or MACRO
    QString modifiers = "";         //  aperture modifiers
    QStringList mod_list;           //  aperture modifiers (split by 'X')

    //Format:
    int x_int;
    int x_dec;
    int y_int;
    int y_dec;

    struct primitive_struct{
        QPainterPath path;                  //  primitive image
        float rotation = 0;                 //  angle
        bool std_aperture = 1;              //  stanfart aperture flag (is rotation needed)
    };

    QList <primitive_struct> primitives;    // all primitives, if this is macro aperture

    struct variable{
        int index=0;    //  variable name (number)
        float value=0;  //  value of variable
    };

    //function for calculation arithmetic expressions in modifiers
    float calculate_expression(const QString expression, QList<variable>* dict);

public:

    Aperture(const int d_code_number_of_aperture = 0, const QString name_of_temp = "", const QString type_of_temp = "", const QString modifs = "", am_template* am_temp = nullptr);

    int get_d_code();
    QString get_name();
    QString get_type();

    void create(const int dpi);                                         //  creation of aperure image
    int draw_me(const int x_pos, const int y_pos, QPainter* painter);   //  flashing this aperture
    int get_std_circ_dia_in_px(const int dpi);                          //  getting diameter (if this is standart circle aperture) for drawing operations

};

#endif // APERTURE_H
