#ifndef PROCESSOR_H
#define PROCESSOR_H

#include <QString>
#include <QStringList>
#include <QPainter>
#include "aperture.h"

class Processor : public QObject
{    
    Q_OBJECT

    bool is_outline_flag;                   //  флаг, который свидетельствует о том, что файл является контуром

    double w, h, dx, dy;                    //  габариты платы, смещение
    double board_width;                     //  с полями
    double board_height;                    //  с полями
    double frame_thickness = 0.05;          //  толщина свободного поля вокруг контура в дюймах
    int    dpi;                             //  dpi разрешение
    float  opacity_value=1;                 //  степень прозрачности

    QString image_format = "png";           //  формат изображения

    QString name_of_gerber_file;            //  имя входного файла (полный путь)
    QString output_folder_path;             //  путь для сохранения изображения
    QString name_of_output_file;            //  имя файла с изображением (полный путь)
    QString name_of_outline_file="";        //  имя файла с изображением контура (полный путь)

    QStringList list_of_strings_of_gerber;  //  массив строк гербер-файла

public:

    Processor(const bool is_outline);       //  конструктор

    void set_image_format(const QString format);                                    //  установить формат изображения
    void set_paths(const QString gerber_file, const QString output_folder);         //  установка необходимых входных и выходных путей
    int load_file();                                                                //  открывает и загружает весь gerber-файл в ОЗУ (StringList), затем закрывает gerber.
    void get_outline_size(double *width, double *height, double *dx, double *dy);   //  (!только для файла контура) расчет размеров платы и смещения начала координат по габаритам контура
    QString get_outline_filename();                                                 //  (!только для файла контура) получить имя выходного файла с изображением
    void set_outline_file_name(const QString);                                      //  установка имени файла с изображением контура для остальных файлов (которые не являются контуром)
    void set_opacity_value(const float val);                                        //  установить степень прозрачности
    void set_dpi(const int new_scale);                                              //  установить количество точек на дюйм
    void set_frame_thickness(const double);                                         //  установка толщины рамки вокруг изображения в мм
    void set_w_h_dx_dy(const double wdt, const double hgt, const double dxx, const double dyy);
    bool is_outline(){
        if (is_outline_flag) return 1;
        else return 0;
    }

    int process();      //  главная функция - обработка и формирование изображения по заданным размерам и смещению начала координат.

signals:

    void finished();    //  признак конца обработки

private:    

    const double pi = 3.1415926;            //  число ПИ
    const double mm_in_inch = 25.41;        //  мм в дюйме

    //
    //  Команды стандартного гербера
    //
    enum commands{
        D01 = 1,
        D02 = 2,
        D03 = 3,
        G01 = 4,
        G02 = 5,
        G03 = 6,
        G74 = 7,
        G75 = 8,
        G36 = 9,
        G37 = 10,
        G04 = 11,
        M02 = 12,
        Dnn = 13
    };
    //
    //  Команды расширенного гербера
    //
    enum extended_commands{
        FS = 1,
        MO = 2,
        AD = 3,
        AM = 4,
        AB = 5,
        LP = 6,
        LM = 7,
        LR = 8,
        LS = 9,
        TF = 10,
        TA = 11,
        TO = 12,
        TD = 13
    };

    //
    //  Graphic state:
    //

    //  Coordinate parameters:
    int frmt_x_int = 1;
    int frmt_x_dec = 1;
    int frmt_y_int = 1;
    int frmt_y_dec = 1;

    enum unit{MM,IN};
    unit unit;

    //  Generation parameters:
    int current_x = 0, current_y = 0;
    Aperture* current_aperture;         //  указатель на апертуру из словаря апертур
    int current_d_code;                 //  текущий d-код для поддержких устаревших конструкции с координатами
                                        //  без указания d-кода в каждой операции...1 - D01, 2 - D02, 3 - D03

    enum interpolation_mode{LINEAR, CLOCKWISE_CIRCULAR, COUNTERCLOCKWISE_CIRCULAR};
    interpolation_mode interpolation_mode = LINEAR;
    enum quadrant_mode{SINGLE_QUADRANT, MULTI_QUADRANT};
    quadrant_mode quadrant_mode;

    //  Aperture transformation parameters:
    enum polarity{C,D};
    polarity polarity = D;
    enum mirroring{NO_MIRRORING,X,Y,XY};
    mirroring mirroring = NO_MIRRORING;
    float rotation = -1;
    float scaling = -1;


    int string_to_command(const QString);           //  преобразует строку с командой в тип enum commands
    int string_to_extended_command(const QString);  //  преобразует строку с командой в тип enum enum extended_commands
    int string_to_units(const QString);             //  преобразует строку с единицами (дюймы, миллиметры) в тип enum unit
    int string_to_mirroring(const QString);         //  преобразует строку с режимом отражения в тип enum mirroring
    int trim_D_argument(QString, const int int_format, const int dec_format, const bool minus);     //  читает строку с координатными данными (число), и приводит его к десятичной дроби
                                                                                                    //  на основании формата %FSLAX...Y...
    int radius_from_big_I_J(const long long int,const long long int);   //  вычисление радиуса из смещений центра дуги по теореме Пифагора
    void norm_angle(int*);                                              //  преобразование угла в градусах в угол в [градус х 16] - необходимо для методов QT
    void check_for_G_in_D(const QString, enum interpolation_mode*);     //  поддержка устаревших конструкций, когда G-код содержится в команде с D-кодом
    int sqrt_from_big_int(const long long int);                         //  вычисление корня квадратного из большого целого (qtmath не справляется)

};

#endif // PROCESSOR_H
