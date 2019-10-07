#include <QFile>
#include <QTextStream>
#include <QTime>
#include <QtMath>
#include <cstdio>
#include <QDebug>
#include <QMap>
#include "processor.h"

Processor::Processor(const bool is_outline)
{    

    is_outline_flag = is_outline;

}

void Processor::set_paths(const QString gerber_file, const QString output_folder){

    QUrl gerb_url(gerber_file),save_url(output_folder);
    QString extension = "." + image_format;
    name_of_gerber_file = gerb_url.toLocalFile();
    output_folder_path = save_url.toLocalFile();
    name_of_output_file = output_folder_path + name_of_gerber_file.right(name_of_gerber_file.size() - name_of_gerber_file.lastIndexOf('/')) + extension;

}

int Processor::sqrt_from_big_int(const long long int value){

    double const eps = 0.1;
    double const start_value = 1;
    double height = start_value;
    double width = value;
    while (abs(width-height)>eps){
        height = (height+width)/2;
        width = value/height;
    }
    return int(height);

}

int Processor::radius_from_big_I_J(const long long i, const long long j){

    return sqrt_from_big_int(abs(i*i+j*j));

}

void Processor::set_dpi(const int new_scale){

    dpi = new_scale;

}

void Processor::set_frame_thickness(const double fr_in_mm){

    frame_thickness = fr_in_mm/mm_in_inch;

}


void Processor::set_w_h_dx_dy(const double wdt, const double hgt, const double dxx, const double dyy){
    w=wdt;
    h=hgt;
    dx=dxx;
    dy=dyy;
}

void Processor::set_opacity_value(const float val){

    opacity_value = val;

}

void Processor::set_image_format(const QString format){

    image_format = format;

}

QString Processor::get_outline_filename(){

    return name_of_output_file;

}

void Processor::set_outline_file_name(const QString filename){

    name_of_outline_file = filename;

}

int Processor::load_file(){

    QFile file(name_of_gerber_file);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)){
        qDebug()<<"Error! Gerber file " << name_of_gerber_file << " can not be opened!";
        return -1;
    }
    while(!file.atEnd()) {
        QByteArray line = file.readLine();        
        list_of_strings_of_gerber.append(line);
    }
    file.close();
    return list_of_strings_of_gerber.size();

}

int Processor::process(){

//  return 1    : succesfully processed
//  return -3   : painter is not running. very big image. try 64bit and more memory
//  return -4   : image was not saved

    if (list_of_strings_of_gerber.isEmpty()){
        qDebug()<<"Gerber file " << name_of_gerber_file << " can not be processed, because list of strings is empty.";
        finished();
        return -1;
    }
    //
    //  Checking the only occurrence of the FS and MO commands, otherwise gerber is invalid.
    //
    int count_of_FS=0, count_of_MO=0;
    double k_mm_or_inch = 1;  //  1 if inch, 25.4 if mm


    for (int i=0;i<list_of_strings_of_gerber.size();i++) {
        if (list_of_strings_of_gerber.at(i).contains("%FS")){
            count_of_FS++;
        }
        if (list_of_strings_of_gerber.at(i).contains("%MO")) {
            count_of_MO++;
            if (list_of_strings_of_gerber.at(i).mid(3,2).contains("MM")) {
                k_mm_or_inch = mm_in_inch;
            }            
        }
    }
    if (!((count_of_FS==1)&&(count_of_MO==1))){
        qDebug()<<"Gerber file " << name_of_gerber_file << " doesn't contain commands %FS or %MO (either contains more than one)! So it is invalid!";
        finished();
        return -2;  //  file is invalid!
    }

    //
    //  Settings for painting..
    //

    //  image size (board size + frame)
    frame_thickness = frame_thickness*k_mm_or_inch;
    board_width = w + frame_thickness*2;
    board_height = h + frame_thickness*2;

    QImage pxmp(qRound(board_width*dpi/k_mm_or_inch), qRound(board_height*dpi/k_mm_or_inch), QImage::Format_RGB16);

    //  if outline image exist, draw on it.., else create clean pixmap
    if (name_of_outline_file!=""){
        if (pxmp.load(name_of_outline_file)){
            // outline is loaded succesfully
        }
        else {
            // error
            pxmp.fill(Qt::white);
        }
    }
    else {
        pxmp.fill(Qt::white);
        if (is_outline_flag == false) {
            // it is not outline
        }
    }

    // painter settings:
    QPainter painter;
    painter.begin(&pxmp);
    if (!painter.isActive()){
        // Error
        qDebug()<<"QPainter is not started. Gerber file " << name_of_gerber_file << " can not be processed!";
        finished();
        return -3;
    }
    painter.setOpacity(qreal(opacity_value));
    painter.translate((frame_thickness-dx)*dpi/k_mm_or_inch,((board_height+dy-frame_thickness)*dpi/k_mm_or_inch));    //  Отражение Оси Y, чтобы начало координат было в нижнем левом углу и необходимое смещение

    QPen global_pen(Qt::black, 5, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin);
    painter.setPen(global_pen);
    painter.scale((1/k_mm_or_inch),(-1/k_mm_or_inch));

    //
    //
    //  Main cycle - all strings processing
    //
    //

    int i=0;                                        //  string counter
    int command=0;                                  //  for enum commands
    int int_unit;                                   //  for enum units
    int int_mirr;                                   //  for enum mirroring
    QString str;                                    //  current string
    QString str_command = "";                       //  command string
    QMap<int,Aperture*> aperture_dictionary;        //  Aperture dictionary
    QMap<QString,am_template*> am_template_dictionary;     //  Macro template dictionary
    current_d_code = 1;
    while (i<list_of_strings_of_gerber.size()) {

        str = list_of_strings_of_gerber.at(i);      //  read string..
        if (str.contains("G04")){
            //  G04 comment. Nothing to do.
        }
        //
        // extended command? begins from '%'?
        //
        else {
            if (str.contains("%")){
                str_command = str.mid(1,2);                             //  2 chars for commands FS.., MO.., AD..etc
                command = string_to_extended_command(str_command);      //
                //
                //  extended gerber commands:
                //
                switch (command) {
                    case FS :{
                    //----------
                    //   FS
                    //----------                        
                        frmt_x_int = 3;                         //  number of integer digits is ALWAYS 3 (many eCADs generate wrong format)
                        frmt_x_dec = str.mid(7,1).toInt();      //  decimals
                        frmt_y_int = 3;                         //  number of integer digits is ALWAYS 3
                        frmt_y_dec = str.mid(10,1).toInt();     //  decimals

                        //  checking command format..
                        if ((frmt_x_int>7)||(frmt_x_dec>6)||(frmt_y_int>7)||(frmt_y_dec>6)||((str.mid(12,1)!="%"))){
                            qDebug()<<"Wrong FS command format. Gerber file " << name_of_gerber_file << " is invalid!";
                            finished();
                            return -2;
                        }

                    }break;
                    case MO :{
                    //----------
                    //   MO
                    //----------                        
                        int_unit = string_to_units(str.mid(3,2));
                        switch (int_unit){
                            case MM : unit = MM; break;
                            case IN : unit = IN; break;
                            default :
                            qDebug()<<"Invalid units in MO command. Gerber file " << name_of_gerber_file << " is invalid!";
                            finished();
                            return -2;
                        }
                    }break;
                    case AD :{
                    //----------
                    //   AD
                    //----------
                        QString d_code_number_of_aperture = "";
                        int int_d_code_of_aperture = 0;
                        QString name_of_aperture_template = "";
                        QString type_of_aperture_template = "";
                        QString modifiers = "";
                        int aprt_temp_index = -1;                   //  index of the first character of the aperture name in the command string
                        am_template* am_pointer = nullptr;          //  aperture template pointer. null by default

                        //
                        //  reading the aperture number (name), finding the index of the beginning of the aperture name
                        //
                        for (int i=4;i<str.size();i++) {
                            if (str.at(i).isDigit()){
                                d_code_number_of_aperture.append(str.at(i));
                            }
                            else if (d_code_number_of_aperture.isEmpty()){
                                qDebug()<< "Empty d-code in AD command. Gerber file " << name_of_gerber_file << " is invalid!";
                                finished();
                                return -2;
                            }
                            else {
                                aprt_temp_index = i;
                                break;
                            }
                        }
                        int_d_code_of_aperture = d_code_number_of_aperture.toInt();
                        //
                        //  reading modifiers
                        //
                        if (str.contains(',')){
                            for (int i=str.indexOf(',')+1;(str.at(i)!='*')&&(str.at(i)!=',');i++) {
                                modifiers.append(str.at(i));
                            }
                        }
                        //
                        //  reading macro template name, and determining its type (as a string: standard template C, R, O, P or macro template)
                        //
                        for (int i=aprt_temp_index;(str.at(i)!='*')&&(str.at(i)!=',');i++) {
                            name_of_aperture_template.append(str.at(i));
                        }
                        if ((name_of_aperture_template.size()==1)&&(name_of_aperture_template=='C'||name_of_aperture_template=='R'||name_of_aperture_template=='O'||name_of_aperture_template=='P')){
                            type_of_aperture_template = name_of_aperture_template;
                        }
                        else{
                            type_of_aperture_template = "MACRO";
                            //
                            //  finding in macro template dictionary
                            //
                            am_pointer = am_template_dictionary.find(name_of_aperture_template).value();
                            if (am_pointer == nullptr){
                                qDebug()<<"An empty element was detected in the macro aperture dictionary. Gerber file " << name_of_gerber_file << " is invalid!";
                                finished();
                                return -2;
                            }
                        }
                        //
                        //  aperture creation and adding it to aperture_dictionary
                        //
                        Aperture *new_aperture = new Aperture(int_d_code_of_aperture, name_of_aperture_template, type_of_aperture_template, modifiers, am_pointer);
                        new_aperture->create(dpi);
                        aperture_dictionary.insert(int_d_code_of_aperture, new_aperture);
                    }break;
                    //----------
                    //   AM
                    //----------
                    case AM :{
                        QString name_of_am_template = "";
                        QStringList data_blocks;
                        //
                        //  reading macro template name
                        //
                        for (int j=3;(str.at(j)!='*');j++) {
                            name_of_am_template.append(str.at(j));
                        }
                        if (name_of_am_template.isEmpty()){
                            qDebug()<<"Can not read macro template name in AM command. Gerber file " << name_of_gerber_file << " is invalid!";
                            finished();
                            return -2;
                        }
                        //
                        //  reading data blocks
                        //

                        //  cycle from %AM to %
                        int percent_counter=0;
                        while (percent_counter<2) {
                            str = list_of_strings_of_gerber.at(i);
                            data_blocks.append(str.split('*'));
                            percent_counter=percent_counter+str.count('%');
                            if (percent_counter == 1)
                                i++;
                        }
                        // check for empty or unnecessary strings and remove them from the list
                        //(for example, a substring with the name of the macro template or the end sign of the template definition '%')
                        for (int j=0; j<data_blocks.size();) {
                            if (!((data_blocks.at(j).at(0).isDigit())||(data_blocks.at(j).at(0)=='$'))){
                                data_blocks.removeAt(j);
                            }
                            else {
                                j++;
                            }
                        }
                        //
                        // macro template creation and adding it to am_template_dictionary
                        //
                        am_template *new_am_template = new am_template(name_of_am_template,data_blocks);
                        am_template_dictionary.insert(name_of_am_template, new_am_template);
                    }break;
                    case AB :{


                    // AB



                    }break;
                    case LP :{
                    //----------
                    //   LP
                    //----------
                        if (str.at(3)=='C'){
                            polarity = C;
                            painter.setBrush(Qt::white);
                        }
                        else if (str.at(3)=='D'){
                            polarity = D;
                            painter.setBrush(Qt::black);
                        }
                        else {
                            qDebug()<<"Wrong LP command format. Gerber file " << name_of_gerber_file << " is invalid!";
                            finished();
                            return -2;
                        }
                    }break;
                    case LM :{
                    //----------
                    //   LM
                    //----------
                        int_mirr = string_to_mirroring(str.mid(3,2));
                        switch (int_mirr) {
                            case NO_MIRRORING : mirroring = NO_MIRRORING; break;
                            case X : mirroring = X; break;
                            case Y : mirroring = Y; break;
                            case XY : mirroring = XY; break;
                        }
                    }break;
                    case LR :{


                    //LR



                    }break;
                    case LS :{


                    //LS



                    }break;
                    case TF :{


                    //TF



                    }break;
                    case TO :{


                    //TO



                    }break;
                    case TD :{


                    //TD



                    }break;
                }//end of switch
            }//end of if (str.contains("%"))

            //
            // D-codes
            //

            else if (str.contains("D")||str.contains("X")||str.contains("Y")) {
                if (str.contains("D")){
                str_command = str.mid(str.indexOf('D'),3);  //  D..(01, 02, 03, nn)
                command = string_to_command(str_command);
                }
                else {
                    if (command==2) {
//                        qDebug()<< "coordinate data without operation code detected...";
//                        qDebug()<< str.contains("D") << "command:" <<command;
//                        qDebug()<< "number of string:0" << i;
                    }
//                      command = current_d_code;  //  deprecated mentor...
//                      command = D01;
                }
                check_for_G_in_D(str,&interpolation_mode);          //  deprecated mentor...

                switch (command) {
                    case D01 :{
                    //----------
                    //   D01
                    //----------
//                        current_d_code = 1;                                 //  deprecated mentor...
//                        check_for_G_in_D(str,&interpolation_mode);          //  deprecated mentor...
                        int command_x = current_x, command_y = current_y;   //  coordinate data from d command
                        int command_i = 0, command_j = 0;                   //  offset

                        //  reading coordinate data from command string
                        if (str.contains('X')){
                            QString x_val;
                            bool minus=0;
                            if ((str.at(str.indexOf('X')+1)) == '-')
                                minus = 1;
                            for (int i=str.indexOf('X') + minus + 1; str.at(i).isDigit();i++) {
                                x_val.append(str.at(i));
                            }
                            command_x = trim_D_argument(x_val, frmt_x_int, frmt_x_dec, minus);
                        }
                        if (str.contains('Y')){
                            bool minus=0;
                            QString y_val;
                            if ((str.at(str.indexOf('Y')+1))=='-')
                                minus = 1;
                            for (int i=str.indexOf('Y') + minus + 1; str.at(i).isDigit();i++) {
                                y_val.append(str.at(i));
                            }
                            command_y = trim_D_argument(y_val, frmt_y_int, frmt_y_dec, minus);
                        }
                        if (str.contains('I')){
                            QString i_val;
                            bool minus=0;
                            if ((str.at(str.indexOf('I')+1))=='-')
                                minus = 1;
                            for (int i=str.indexOf('I') + minus + 1; str.at(i).isDigit();i++) {
                                i_val.append(str.at(i));
                            }
                            command_i = trim_D_argument(i_val, frmt_x_int, frmt_x_dec, minus);
                        }
                        if (str.contains('J')){
                            bool minus=0;
                            QString j_val;
                            if ((str.at(str.indexOf('J')+1))=='-')
                                minus = 1;
                            for (int i=str.indexOf('J') + minus + 1; str.at(i).isDigit();i++) {
                                j_val.append(str.at(i));
                            }
                            command_j = trim_D_argument(j_val, frmt_y_int, frmt_y_dec, minus);
                        }
                        //
                        // drawing in LINEAR mode
                        //
                        if (interpolation_mode == LINEAR){
                            painter.drawLine(current_x, current_y, command_x, command_y);
                        }
                        //
                        // drawing in CLOCKWISE_CIRCULAR mode
                        //
                        else if (interpolation_mode == CLOCKWISE_CIRCULAR){
                            // arc radius
                            int R = radius_from_big_I_J(command_i,command_j);
                            // center:
                            int Cx = 0;
                            int Cy = 0;
                            // start angle and end angle of arc:
                            int start_angle = 0;
                            int end_angle = 0;
                            int span_angle = 0;

                            if (quadrant_mode == SINGLE_QUADRANT){
                                // 1 q.
                                if ((current_x < command_x)&&(current_y > command_y)){
                                    Cx = current_x - command_i;
                                    Cy = current_y - command_j;
                                    start_angle = qRound(atan2(command_j,command_i)*16*180/pi);
                                    end_angle = qRound(atan2(command_y-Cy, command_x-Cx)*16*180/pi);
                                }
                                //4 q.
                                else if ((current_x > command_x)&&(current_y > command_y)){
                                    Cx = current_x - command_i;
                                    Cy = current_y + command_j;
                                    start_angle = qRound(-atan2(command_j,command_i)*16*180/pi);
                                    end_angle = qRound(-atan2(Cy-command_y, command_x-Cx)*16*180/pi);
                                }
                                //3 q.
                                else if ((current_x > command_x)&&(current_y < command_y)){
                                    Cx = current_x + command_i;
                                    Cy = current_y + command_j;
                                    start_angle = 180*16 + qRound(atan2(command_j,command_i)*16*180/pi);
                                    end_angle = 180*16 + qRound(atan2(Cy-command_y, Cx-command_x)*16*180/pi);
                                }
                                //2 q.
                                else {
                                    Cx = current_x + command_i;
                                    Cy = current_y - command_j;
                                    start_angle = 180*16 - qRound(atan2(command_j,command_i)*16*180/pi);
                                    end_angle = 180*16 - qRound(atan2(command_y-Cy, Cx-command_x)*16*180/pi);
                                }
                                // rect for Qt arc:
                                QRect arc_rect(Cx-R, Cy-R, R*2, R*2);
                                // span angle:
                                span_angle = end_angle - start_angle;
                                //
                                if (qAbs(span_angle)>90*16){
                                    if (span_angle<0) span_angle = -90*16;
                                    else {span_angle = 90*16;}
                                }
                                // drawing arc:
                                painter.drawArc(arc_rect, -start_angle, -span_angle);
                            }
                            else if (quadrant_mode == MULTI_QUADRANT){
                                // center:
                                int Cx = current_x + command_i;
                                int Cy = current_y + command_j;
                                // rect for arc:
                                QRect arc_rect(Cx-R, Cy-R, R*2, R*2);
                                // angles:
                                start_angle = qRound(atan2(-command_j,-command_i)*16*180/pi);
                                end_angle = qRound(atan2(command_y-Cy, command_x-Cx)*16*180/pi);
                                // convert angle to the range 0...360
                                norm_angle(&start_angle);
                                norm_angle(&end_angle);
                                if (start_angle<=end_angle){
                                    span_angle = end_angle - (start_angle + 360*16);
                                }
                                else {
                                    span_angle = end_angle - start_angle;
                                }
                                //  drawing arc:
                                painter.drawArc(arc_rect, -start_angle, abs(span_angle));
                            }
                            else {
                                //  Error!
                                qDebug()<<"Error! Quadrant mode is not set. Gerber file " << name_of_gerber_file << " is invalid!";
                                finished();
                                return -1;
                            }
                        }
                        //---------------------------------------------------------------
                        // drawing in COUNTERCLOCKWISE_CIRCULAR mode
                        //---------------------------------------------------------------
                        else if (interpolation_mode==COUNTERCLOCKWISE_CIRCULAR){
                            // arc radius
                            int R = radius_from_big_I_J(command_i,command_j);
                            // arc center:
                            int Cx = 0;
                            int Cy = 0;
                            // angles:
                            int start_angle = 0;
                            int end_angle = 0;
                            int span_angle = 0;

                            if (quadrant_mode == SINGLE_QUADRANT){
                                //  1 q.
                                if ((current_x > command_x)&&(current_y < command_y)){
                                    Cx = current_x - command_i;
                                    Cy = current_y - command_j;
                                    start_angle = qRound(atan2(command_j,command_i)*16*180/pi);
                                    end_angle = qRound(atan2(command_y-Cy, command_x-Cx)*16*180/pi);
                                }
                                //  4 q.
                                else if ((current_x < command_x)&&(current_y < command_y)){
                                    Cx = current_x - command_i;
                                    Cy = current_y + command_j;
                                    start_angle = qRound(-atan2(command_j,command_i)*16*180/pi);
                                    end_angle = qRound(-atan2(Cy-command_y, command_x-Cx)*16*180/pi);
                                }
                                //  3 q.
                                else if ((current_x < command_x)&&(current_y > command_y)){
                                    Cx = current_x + command_i;
                                    Cy = current_y + command_j;
                                    start_angle = 180*16 + qRound(atan2(command_j,command_i)*16*180/pi);
                                    end_angle = 180*16 + qRound(atan2(Cy-command_y, Cx-command_x)*16*180/pi);
                                }
                                //  2 q.
                                else {
                                    Cx = current_x + command_i;
                                    Cy = current_y - command_j;
                                    start_angle = 180*16 - qRound(atan2(command_j,command_i)*16*180/pi);
                                    end_angle = 180*16 - qRound(atan2(command_y-Cy, Cx-command_x)*16*180/pi);
                                }
                                // arc rect:
                                QRect arc_rect(Cx-R, Cy-R, R*2, R*2);
                                // span angle:
                                span_angle = end_angle - start_angle;
                                //
                                if (qAbs(span_angle)>90*16){
                                    if (span_angle<0) span_angle = -90*16;
                                    else {span_angle = 90*16;}
                                }
                                // drawing arc:
                                painter.drawArc(arc_rect, -start_angle, -span_angle);
                            }
                            else if (quadrant_mode == MULTI_QUADRANT){
                                // arc center:
                                int Cx = current_x + command_i;
                                int Cy = current_y + command_j;
                                // arc rect:
                                QRect arc_rect(Cx-R, Cy-R, R*2, R*2);
                                // angles:
                                start_angle = qRound(atan2(-command_j,-command_i)*16*180/pi);
                                end_angle = qRound(atan2(command_y-Cy, command_x-Cx)*16*180/pi);
                                norm_angle(&start_angle);
                                norm_angle(&end_angle);
                                if (start_angle >= end_angle){
                                    span_angle = (end_angle + 360*16) - start_angle;
                                }
                                else {
                                    span_angle = end_angle - start_angle;
                                }
                                //  drawing arc:
                                painter.drawArc(arc_rect, -start_angle, -abs(span_angle));
                            }
                            else {
                                //  Error!
                                qDebug()<<"Error! Quadrant mode is not set. Gerber file " << name_of_gerber_file << " is invalid!";
                                finished();
                                return -1;
                            }
                        }
                        else {
                            //  Error!
                            qDebug()<<"Error! Interpolation mode is not set. Gerber file " << name_of_gerber_file << " is invalid!";
                            finished();
                            return -1;
                        }

                        //  updating current coordinates
                        current_x = command_x;
                        current_y = command_y;

                    }break;
                    case D02 :{
                    //----------
                    //   D02
                    //----------
//                    current_d_code = 1;                         //  deprecated mentor...
                    check_for_G_in_D(str,&interpolation_mode);  //  deprecated mentor...
                    if (str.contains('X')){
                        QString x_val;
                        bool minus=0;
                        if ((str.at(str.indexOf('X')+1))=='-')
                            minus = 1;
                        for (int i=str.indexOf('X') + minus + 1; str.at(i).isDigit();i++) {
                            x_val.append(str.at(i));
                        }
                        current_x = trim_D_argument(x_val, frmt_x_int, frmt_x_dec, minus);
                    }
                    if (str.contains('Y')){
                        bool minus=0;
                        QString y_val;
                        if ((str.at(str.indexOf('Y')+1))=='-')
                            minus = 1;
                        for (int i=str.indexOf('Y') + minus + 1; str.at(i).isDigit();i++) {
                            y_val.append(str.at(i));
                        }
                        current_y = trim_D_argument(y_val, frmt_y_int, frmt_y_dec, minus);
                    }
                    }break;
                    case D03 :{
                    //----------
                    //   D03
                    //----------                    
                        if (str.contains('X')){
                            QString x_val;
                            bool minus=0;
                            if ((str.at(str.indexOf('X')+1))=='-')
                                minus = 1;
                            for (int i=str.indexOf('X') + minus + 1; str.at(i).isDigit();i++) {
                                x_val.append(str.at(i));
                            }
                            current_x = trim_D_argument(x_val, frmt_x_int, frmt_x_dec, minus);
                        }
                        if (str.contains('Y')){
                            bool minus=0;
                            QString y_val;
                            if ((str.at(str.indexOf('Y')+1))=='-')
                                minus = 1;
                            for (int i=str.indexOf('Y') + minus + 1; str.at(i).isDigit();i++) {
                                y_val.append(str.at(i));
                            }
                            current_y = trim_D_argument(y_val, frmt_y_int, frmt_y_dec, minus);
                        }                        
                        //FLASH current aperture
                        current_aperture->draw_me(current_x,current_y,&painter);

                    }break;
                    case Dnn :{
                    //----------
                    //   Dnnn
                    //----------
                        //  reading integer after 'D'..
                        QString number_of_Dnn;
                        for (int i=str.indexOf('D')+1;str.at(i).isDigit();i++) {
                            number_of_Dnn.append(str.at(i));
                        }
                        //  finding this aperture in aperture dictionary and setting it as current aperture
                        current_aperture = aperture_dictionary.find(number_of_Dnn.toInt()).value();

                        //  settings of pen for drawing by D01
                        //**************************************************************
                            global_pen.setWidth(current_aperture->get_std_circ_dia_in_px(dpi));
                            painter.setPen(global_pen);
                        //**************************************************************
                    }break;
                }//end of switch
            }//end of else if (str.contains("D"))
            //---------------------------------------------------------------
            // G and М commands
            //---------------------------------------------------------------
            else if (str.contains("G")||str.contains("M")) {
                str_command = str.mid(0,3);
                command = string_to_command(str_command);
                switch (command) {
                    case G01 :{
                    //----------
                    //   G01
                    //----------
                        interpolation_mode = LINEAR;
                    }break;
                    case G02 :{
                    //----------
                    //   G02
                    //----------
                        interpolation_mode = CLOCKWISE_CIRCULAR;
                    }break;
                    case G03 :{
                    //----------
                    //   G03
                    //----------
                        interpolation_mode = COUNTERCLOCKWISE_CIRCULAR;
                    }break;
                    case G74 :{
                    //----------
                    //   G74
                    //----------
                        quadrant_mode = SINGLE_QUADRANT;
                    }break;
                    case G75 :{
                    //----------
                    //   G75
                    //----------
                        quadrant_mode = MULTI_QUADRANT;
                    }break;
                    case G36 :{

                        //
                        //
                        //
                        // R E G I O N S
                        //
                        //
                        //

                        bool end_of_region = false;
                        bool end_of_contour = false;
                        bool creating_contour_now = false;
                        QList <QPainterPath*> contours;     // contours in current region
                        QPointF startpoint;

                        //  main region cycle
                        painter.save();
                        painter.setPen(Qt::NoPen);
                        while (!end_of_region) {

                            creating_contour_now = false;
                            end_of_contour = false;

                            while (!end_of_contour) {
                                //  if at the moment the contour is not created and there is the D01 command, then I start the creation of a new contour ..
                                str = list_of_strings_of_gerber.at(i);
                                if ((str.contains("D01"))&&(creating_contour_now == false)){
                                    startpoint.setX(current_x);
                                    startpoint.setY(current_y);
                                    QPainterPath* new_contour = new QPainterPath(startpoint);
                                    new_contour->setFillRule(Qt::WindingFill);
                                    contours.append(new_contour);
                                    creating_contour_now = true;
                                }

                                //................................................

                                if (str.contains("D")||str.contains("X")||str.contains("Y")){
                                    if (str.contains("D")){
                                    str_command = str.mid(str.indexOf('D'),3);
                                    command = string_to_command(str_command);
                                    }
                                    else {
                                        command = D01;               //  deprecated mentor...
                                    }
                                    switch (command) {
                                        //
                                        //   D01 for regions
                                        //
                                        case D01:{
//                                        current_d_code = 1;                               //  deprecated mentor...
                                        check_for_G_in_D(str,&interpolation_mode);          //  deprecated mentor...
                                        int command_x = current_x, command_y = current_y;
                                        int command_i = 0, command_j = 0;

                                        //  чтение новых координат из строки с командой
                                        if (str.contains('X')){
                                            QString x_val;
                                            bool minus=0;
                                            if ((str.at(str.indexOf('X')+1)) == '-')
                                                minus = 1;
                                            for (int i=str.indexOf('X') + minus + 1; str.at(i).isDigit();i++) {
                                                x_val.append(str.at(i));
                                            }
                                            command_x = trim_D_argument(x_val, frmt_x_int, frmt_x_dec, minus);
                                        }
                                        if (str.contains('Y')){
                                            bool minus=0;
                                            QString y_val;
                                            if ((str.at(str.indexOf('Y')+1))=='-')
                                                minus = 1;
                                            for (int i=str.indexOf('Y') + minus + 1; str.at(i).isDigit();i++) {
                                                y_val.append(str.at(i));
                                            }
                                            command_y = trim_D_argument(y_val, frmt_y_int, frmt_y_dec, minus);
                                        }
                                        if (str.contains('I')){
                                            QString i_val;
                                            bool minus=0;
                                            if ((str.at(str.indexOf('I')+1))=='-')
                                                minus = 1;
                                            for (int i=str.indexOf('I') + minus + 1; str.at(i).isDigit();i++) {
                                                i_val.append(str.at(i));
                                            }
                                            command_i = trim_D_argument(i_val, frmt_x_int, frmt_x_dec, minus);
                                        }
                                        if (str.contains('J')){
                                            bool minus=0;
                                            QString j_val;
                                            if ((str.at(str.indexOf('J')+1))=='-')
                                                minus = 1;
                                            for (int i=str.indexOf('J') + minus + 1; str.at(i).isDigit();i++) {
                                                j_val.append(str.at(i));
                                            }
                                            command_j = trim_D_argument(j_val, frmt_y_int, frmt_y_dec, minus);
                                        }
                                        //
                                        //  Drawing in LINEAR mode
                                        //
                                        if (interpolation_mode == LINEAR){
                                            contours.last()->lineTo(command_x,command_y);
                                        }
                                        //
                                        //  drawing in CLOCKWISE_CIRCULAR mode
                                        //
                                        else if (interpolation_mode == CLOCKWISE_CIRCULAR){
                                            int R = radius_from_big_I_J(command_i,command_j);   //  arc radius
                                            int Cx = 0;
                                            int Cy = 0;
                                            int start_angle = 0;
                                            int end_angle = 0;
                                            int span_angle = 0;

                                            if (quadrant_mode == SINGLE_QUADRANT){
                                                //  1 q.
                                                if ((current_x < command_x)&&(current_y > command_y)){
                                                    Cx = current_x - command_i;
                                                    Cy = current_y - command_j;
                                                    start_angle = qRound(atan2(command_j,command_i)*16*180/pi);
                                                    end_angle = qRound(atan2(command_y-Cy, command_x-Cx)*16*180/pi);
                                                }
                                                //  4 q.
                                                else if ((current_x > command_x)&&(current_y > command_y)){
                                                    Cx = current_x - command_i;
                                                    Cy = current_y + command_j;
                                                    start_angle = qRound(-atan2(command_j,command_i)*16*180/pi);
                                                    end_angle = qRound(-atan2(Cy-command_y, command_x-Cx)*16*180/pi);
                                                }
                                                //  3 q.
                                                else if ((current_x > command_x)&&(current_y < command_y)){
                                                    Cx = current_x + command_i;
                                                    Cy = current_y + command_j;
                                                    start_angle = 180*16 + qRound(atan2(command_j,command_i)*16*180/pi);
                                                    end_angle = 180*16 + qRound(atan2(Cy-command_y, Cx-command_x)*16*180/pi);
                                                }
                                                //  2 q.
                                                else {
                                                    Cx = current_x + command_i;
                                                    Cy = current_y - command_j;
                                                    start_angle = 180*16 - qRound(atan2(command_j,command_i)*16*180/pi);
                                                    end_angle = 180*16 - qRound(atan2(command_y-Cy, Cx-command_x)*16*180/pi);
                                                }
                                                QRect arc_rect(Cx-R, Cy-R, R*2, R*2);
                                                span_angle = end_angle - start_angle;
                                                if (qAbs(span_angle)>90*16){
                                                    if (span_angle<0) span_angle = -90*16;
                                                    else {span_angle = 90*16;}
                                                }
                                                //  drawing arc:
                                                contours.last()->arcTo(arc_rect,-start_angle/16, -span_angle/16);
                                            }
                                            else if (quadrant_mode == MULTI_QUADRANT){
                                                int Cx = current_x + command_i;
                                                int Cy = current_y + command_j;
                                                QRect arc_rect(Cx-R, Cy-R, R*2, R*2);
                                                start_angle = qRound(atan2(-command_j,-command_i)*16*180/pi);
                                                end_angle = qRound(atan2(command_y-Cy, command_x-Cx)*16*180/pi);
                                                norm_angle(&start_angle);
                                                norm_angle(&end_angle);
                                                if (start_angle<=end_angle){
                                                    span_angle = end_angle - (start_angle + 360*16);
                                                }
                                                else {
                                                    span_angle = end_angle - start_angle;
                                                }
                                                //  drawing arc:

                                                contours.last()->arcTo(arc_rect,-start_angle/16, abs(span_angle/16));
                                            }
                                            else {
                                                //  Error!
                                                qDebug()<<"Error! Quadrant mode is not set (in region). Gerber file " << name_of_gerber_file << " is invalid!";
                                                finished();
                                                return -1;
                                            }
                                        }
                                        //
                                        //  drawing in COUNTERCLOCKWISE_CIRCULAR mode
                                        //
                                        else if (interpolation_mode==COUNTERCLOCKWISE_CIRCULAR){
                                            int R = radius_from_big_I_J(command_i,command_j);
                                            int Cx = 0;
                                            int Cy = 0;
                                            int start_angle = 0;
                                            int end_angle = 0;
                                            int span_angle = 0;

                                            if (quadrant_mode == SINGLE_QUADRANT){
                                                //  1 q.
                                                if ((current_x > command_x)&&(current_y < command_y)){
                                                    Cx = current_x - command_i;
                                                    Cy = current_y - command_j;
                                                    start_angle = qRound(atan2(command_j,command_i)*16*180/pi);
                                                    end_angle = qRound(atan2(command_y-Cy, command_x-Cx)*16*180/pi);
                                                }
                                                //  4 q.
                                                else if ((current_x < command_x)&&(current_y < command_y)){
                                                    Cx = current_x - command_i;
                                                    Cy = current_y + command_j;
                                                    start_angle = qRound(-atan2(command_j,command_i)*16*180/pi);
                                                    end_angle = qRound(-atan2(Cy-command_y, command_x-Cx)*16*180/pi);
                                                }
                                                //  3 q.
                                                else if ((current_x < command_x)&&(current_y > command_y)){
                                                    Cx = current_x + command_i;
                                                    Cy = current_y + command_j;
                                                    start_angle = 180*16 + qRound(atan2(command_j,command_i)*16*180/pi);
                                                    end_angle = 180*16 + qRound(atan2(Cy-command_y, Cx-command_x)*16*180/pi);
                                                }
                                                //  2 q.
                                                else {
                                                    Cx = current_x + command_i;
                                                    Cy = current_y - command_j;
                                                    start_angle = 180*16 - qRound(atan2(command_j,command_i)*16*180/pi);
                                                    end_angle = 180*16 - qRound(atan2(command_y-Cy, Cx-command_x)*16*180/pi);
                                                }
                                                QRect arc_rect(Cx-R, Cy-R, R*2, R*2);
                                                span_angle = end_angle - start_angle;
                                                if (qAbs(span_angle)>90*16){
                                                    if (span_angle<0) span_angle = -90*16;
                                                    else {span_angle = 90*16;}
                                                }
                                                //  drawing arc:
                                                contours.last()->arcTo(arc_rect,-start_angle/16, -span_angle/16);
                                            }
                                            else if (quadrant_mode == MULTI_QUADRANT){
                                                int Cx = current_x + command_i;
                                                int Cy = current_y + command_j;
                                                QRect arc_rect(Cx-R, Cy-R, R*2, R*2);
                                                start_angle = qRound(atan2(-command_j,-command_i)*16*180/pi);
                                                end_angle = qRound(atan2(command_y-Cy, command_x-Cx)*16*180/pi);
                                                norm_angle(&start_angle);
                                                norm_angle(&end_angle);
                                                if (start_angle >= end_angle){
                                                    span_angle = (end_angle + 360*16) - start_angle;
                                                }
                                                else {
                                                    span_angle = end_angle - start_angle;
                                                }
                                                //  drawing arc:
                                                contours.last()->arcTo(arc_rect,-start_angle/16, -abs(span_angle/16));
                                            }
                                            else {
                                                //  Error!
                                                qDebug()<<"Error! Quadrant mode is not set (in region). Gerber file " << name_of_gerber_file << " is invalid!";
                                                finished();
                                                return -1;
                                            }
                                        }
                                        else {
                                            //  Error!
                                            qDebug()<<"Error! Interpolation mode is not set (in region). Gerber file " << name_of_gerber_file << " is invalid!";
                                            finished();
                                            return -1;
                                        }
                                        //  updating current coordinates
                                        current_x = command_x;
                                        current_y = command_y;
                                        }break;
                                        case D02:{
                                            //
                                            //   D02 for regions
                                            //
//                                            current_d_code = 2;     //  deprecated mentor
                                            //  is d02 the end of the contour? if yes, then close the contour
                                            check_for_G_in_D(str,&interpolation_mode); //   deprecated mentor...
                                            if (creating_contour_now == true){
                                                end_of_contour = true;
                                            }
                                            //  then standart processing of command
                                            if (str.contains('X')){
                                                    QString x_val;
                                                    bool minus=0;
                                                    if ((str.at(str.indexOf('X')+1))=='-')
                                                        minus = 1;
                                                    for (int i=str.indexOf('X') + minus + 1; str.at(i).isDigit();i++) {
                                                        x_val.append(str.at(i));
                                                    }
                                                    current_x = trim_D_argument(x_val, frmt_x_int, frmt_x_dec, minus);
                                                }
                                            if (str.contains('Y')){
                                                bool minus=0;
                                                QString y_val;
                                                if ((str.at(str.indexOf('Y')+1))=='-')
                                                    minus = 1;
                                                for (int i=str.indexOf('Y') + minus + 1; str.at(i).isDigit();i++) {
                                                    y_val.append(str.at(i));
                                                }
                                                current_y = trim_D_argument(y_val, frmt_y_int, frmt_y_dec, minus);
                                            }
                                        }break;
                                    }//end of switch
                                }//end of if (str.contains("D"))
                                else if (str.contains("G")) {
                                    str_command = str.mid(str.indexOf('G'),3);
                                    command = string_to_command(str_command);
                                    switch (command) {
                                        case G01 :{
                                        //----------
                                        //   G01
                                        //----------
                                            interpolation_mode = LINEAR;
                                        }break;
                                        case G02 :{
                                        //----------
                                        //   G02
                                        //----------
                                            interpolation_mode = CLOCKWISE_CIRCULAR;
                                        }break;
                                        case G03 :{
                                        //----------
                                        //   G03
                                        //----------
                                            interpolation_mode = COUNTERCLOCKWISE_CIRCULAR;
                                        }break;
                                        case G74 :{
                                        //----------
                                        //   G74
                                        //----------
                                            quadrant_mode = SINGLE_QUADRANT;
                                        }break;
                                        case G75 :{
                                        //----------
                                        //   G75
                                        //----------
                                            quadrant_mode = MULTI_QUADRANT;
                                        }break;
                                    }//end of switch
                                }//end of if (str.contains("G"))

                                //................................................
                                // END OF REGION
                                if (str.contains("G37")){
                                    end_of_contour = true;
                                    end_of_region = true;
                                    //  add all contours to image
                                    for (int i=0;i<contours.size();i++) {
                                        painter.fillPath(*contours.at(i),Qt::black);
                                        painter.drawPath(*contours.at(i));
                                    }
                                    //  free memory
                                    for (int i=0;i<contours.size();i++) {
                                        delete contours.at(i);
                                    }
                                    //  restore global pen settings (aperture, polarity, etc..)
                                    painter.restore();
                                    break;
                                }// end of if (str.contains("G37"))

                            i++;    //  increment counter of strings

                            }// end of while (!end_of_contour)
                        }// end of while (!end_of_region)

                    }break;

                    case M02 :{
                    //----------
                    //   M02
                    //----------
                    //END OF FILE. nothing to do
                    }break;

                }//end of switch
            }//end of else if (str.contains("G")||str.contains("M"))
        }//end of else (if (str.contains("G04")))

        //increment counter of strings
        i++;

    }//end of main cycle

    //
    //
    //  Save QImage
    //
    //

    painter.end();
    QFile file(name_of_output_file);
    file.open(QIODevice::WriteOnly);
    if (image_format == "bmp"){
        if (!pxmp.save(&file,"BMP")) {
            qDebug()<<"Error! Gerber file " << name_of_gerber_file << ". Can not save the image!";
            finished();
            return -4;
        }         //  save BMP
    }
    else if (image_format == "png"){
        if (!pxmp.save(&file,"PNG")){
            qDebug()<<"Error! Gerber file " << name_of_gerber_file << ". Can not save the image!";
            finished();
            return -4;
        }         //  save PNG
    }

    //
    //  Free memory
    //
    aperture_dictionary.clear();
    am_template_dictionary.clear();

    finished();
    return 1;           // File succesfully processed

}

int Processor::string_to_command(const QString str){

    if (str=="D01")       return D01;
    else if (str=="D02")  return D02;
    else if (str=="D03")  return D03;
    else if (str=="G01")  return G01;
    else if (str=="G02")  return G02;
    else if (str=="G03")  return G03;
    else if (str=="G74")  return G74;
    else if (str=="G75")  return G75;
    else if (str=="G36")  return G36;
    else if (str=="G37")  return G37;
    else if (str=="G04")  return G04;
    else if (str=="M02")  return M02;
    else if (str.contains('D'))  return Dnn;
    return -1;

}

int Processor::string_to_extended_command(const QString str){

    if (str=="FS")       return FS;
    else if (str=="MO")  return MO;
    else if (str=="AD")  return AD;
    else if (str=="AM")  return AM;
    else if (str=="AB")  return AB;
    else if (str=="LP")  return LP;
    else if (str=="LM")  return LM;
    else if (str=="LR")  return LR;
    else if (str=="LS")  return LS;
    else if (str=="TF")  return TF;
    else if (str=="TA")  return TA;
    else if (str=="TO")  return TO;
    else if (str=="TD")  return TD;
    return -1;

}

int Processor::string_to_units(const QString str){

    if (str=="MM")       return MM;
    else if (str=="IN")  return IN;
    return -1;

}

int Processor::string_to_mirroring(const QString str){

    if (str=="XY")                  return XY;
    else if(str.mid(1,1)=='*'||str.mid(1,1)=='%')
    {
        if (str.left(1)=='X')       return X;
        else if (str.left(1)=='Y')  return Y;
        else if (str.left(1)=='N')  return NO_MIRRORING;
    }
    return -1;

}

int Processor::trim_D_argument(QString str, const int int_format, const int dec_format, const bool minus){

    if (str.size()<(int_format+dec_format)) {
        int difference = int_format+dec_format-str.size();
        for (int i=0;i<difference;i++) {
            str = str.insert(0,'0');
        }
    }
    str = str.insert(int_format,'.');
    if (minus){
        str.insert(0,'-');
    }
    return qRound((str.toDouble())*dpi);

}

void Processor::norm_angle(int* angle){

    if (*angle<0){
         *angle = (360*16) + *angle;
    }

}

void Processor::check_for_G_in_D(const QString str, enum interpolation_mode* mode){

    if (str.contains("G01")||str.contains("G1")){
        *mode = LINEAR;
    }
    else if (str.contains("G02")||str.contains("G2")){
        *mode = CLOCKWISE_CIRCULAR;
    }
    else if (str.contains("G03")||str.contains("G3")){
        *mode = COUNTERCLOCKWISE_CIRCULAR;
    }

}

void Processor::get_outline_size(double *width, double *height, double *dx, double *dy){

    int min_x=2100000000, max_x=0, min_y=2100000000, max_y=0;
    int x_val, y_val;
    int frmt_int = 3, frmt_dec=1;
    QString str, str_val;
    bool minus=0;

    for (int i=0;i<list_of_strings_of_gerber.size();i++) {
        str = list_of_strings_of_gerber.at(i);
        //
        //  X
        //
        if (str.contains("%FS")){
            frmt_dec = str.mid(7,1).toInt();
        }
        if ((str.contains('X'))&&(!(str.contains('%')))&&(!(str.contains(',')))){
            minus=false;
            str_val="";
            if ((str.at(str.indexOf('X')+1)) == '-'){
                minus = true;
            }
            for (int i = str.indexOf('X') + minus + 1; str.at(i).isDigit();i++) {
                str_val.append(str.at(i));

            }
            x_val = trim_D_argument(str_val, frmt_int, frmt_dec, minus);
            if (x_val < min_x){
                min_x = x_val;
            }
            if (x_val > max_x) {
                max_x = x_val;
            }
        }
        //
        //  Y
        //
        if ((str.contains('Y'))&&(!(str.contains('%')))&&(!(str.contains(',')))){
            minus=false;
            str_val="";
            if ((str.at(str.indexOf('Y')+1)) == '-'){
                minus = true;
            }
            for (int i = str.indexOf('Y') + minus + 1; str.at(i).isDigit();i++) {
                str_val.append(str.at(i));
            }
            y_val = trim_D_argument(str_val, frmt_int, frmt_dec, minus);
            if (y_val < min_y){
                min_y = y_val;
            }
            if (y_val > max_y) {
                max_y = y_val;
            }
        }
    }

    *dx = double(min_x)/dpi;
    *dy = double(min_y)/dpi;

    *width = double(max_x-min_x)/dpi;
    *height = double(max_y-min_y)/dpi;

}

