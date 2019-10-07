#ifndef PROCESSOR_H
#define PROCESSOR_H

#include <QString>
#include <QStringList>
#include <QPainter>
#include <QUrl>
#include "aperture.h"

class Processor : public QObject
{    
    Q_OBJECT

    bool is_outline_flag;                   //  file is outline

    double w, h, dx, dy;                    //  image width, height, delta dx and dy of origin
    double board_width;
    double board_height;
    double frame_thickness = 0.05;          //  frame thickness around board outline on the image
    int    dpi;                             //  image dpi
    float  opacity_value=1;                 //  value from 0 to 1

    QString image_format = "png";           //  image type png or bmp

    QString name_of_gerber_file;            //  input filename (full path)
    QString output_folder_path;             //  save folder path
    QString name_of_output_file;            //  output image filename (full path)
    QString name_of_outline_file="";        //  outline image filename (full path)

    QStringList list_of_strings_of_gerber;

public:

    Processor(const bool is_outline);

    void set_image_format(const QString format);
    void set_paths(const QString gerber_file, const QString output_folder);
    int load_file();                                                                //  open gerber file and load its strings into StringList, then close gerber.
    void get_outline_size(double *width, double *height, double *dx, double *dy);   //  (if this is outline) calculate global width, height, dx, dy for all files in session by this outline
    QString get_outline_filename();                                                 //  (if this is outline)
    void set_outline_file_name(const QString);                                      //  (if this is NOT outline)
    void set_opacity_value(const float val);
    void set_dpi(const int new_scale);
    void set_frame_thickness(const double);
    void set_w_h_dx_dy(const double wdt, const double hgt, const double dxx, const double dyy);
    bool is_outline(){
        if (is_outline_flag) return 1;
        else return 0;
    }

    int process();      //  main function. processing gerber and make image.

signals:

    void finished();    //  when return from main function process()

private:    

    const double pi = 3.1415926;
    const double mm_in_inch = 25.41;

    //
    //  Standart Gerber commands
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
    //  Extended Gerber commands
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
    Aperture* current_aperture;         //  current aperture pointer (which aperture from aperture dictionary is set up now)
    int current_d_code;                 //  current (last) d-code. because of deprecated constructions, when D-code
                                        //  is not always set

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


    int string_to_command(const QString);           //  command by string to enum commands
    int string_to_extended_command(const QString);  //  command by string to enum extended_commands
    int string_to_units(const QString);             //  command by string to enum unit
    int string_to_mirroring(const QString);         //  command bu string to enum mirroring
    int trim_D_argument(QString, const int int_format, const int dec_format, const bool minus);     //  read coordinate data and convert it to decimal
                                                                                                    //  using data from %FSLAX...Y...
    int radius_from_big_I_J(const long long int,const long long int);   //  calculate radius from Pifagor theorem
    void norm_angle(int*);                                              //  convert degrees to [degree х 16] - for QT arcs
    void check_for_G_in_D(const QString, enum interpolation_mode*);     //  find g-code in operations with d-code (deprecated constructions)
    int sqrt_from_big_int(const long long int);                         //  calculate sqrt from big int number

};

#endif // PROCESSOR_H
