#include "controller.h"

controller::controller()
{
    QObject::connect(this, SIGNAL(gerbers_string_Changed()),this, SLOT(set_list_of_gerbers_from_QML_string()));
}

void controller::load_ini_file(){
    QFile file("gerber_translator.ini");
    QStringList strings_of_ini;
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)){
        emit message("gerber_translator.ini not found. New file will be created.");
    }
    else {
        while (!file.atEnd()) {
            QByteArray str = file.readLine().trimmed();

            if (!str.contains('%')) {
                if (str.contains("open_path")){
                    m_open_path_ini = str.right(str.size()-(str.indexOf('=')+1));
                }
                else if (str.contains("save_path")) {
                    m_save_path_ini = str.right(str.size()-(str.indexOf('=')+1));
                }
                else if (str.contains("image_format")) {
                    m_image_format_ini = str.right(str.size()-(str.indexOf('=')+1));
                }
                else if (str.contains("dpi")) {
                    QString pre_dpi;
                    pre_dpi = str.right(str.size()-(str.indexOf('=')+1));
                    bool is_dpi_correct = false;
                    pre_dpi.toInt(&is_dpi_correct);
                    if (is_dpi_correct){
                        m_dpi_ini = pre_dpi;
                    }
                    else {
                        emit message("Cannot set resolution from gerber_translator.ini!\nThe default value will be set.");
                    }
                }
                else if (str.contains("quick_translation")) {
                    m_quick_translation_ini = str.right(str.size()-(str.indexOf('=')+1));
                }
                else if (str.contains("open_folder_after_processing")) {
                    m_open_folder_after_processing_ini = str.right(str.size()-(str.indexOf('=')+1));
                }
                else if (str.contains("image_size")) {
                    m_image_size_ini = str.right(str.size()-(str.indexOf('=')+1));
                }
                else if (str.contains("default_image_width")) {
                    m_default_image_width_ini = str.right(str.size()-(str.indexOf('=')+1));
                }
                else if (str.contains("default_image_height")) {
                    m_default_image_height_ini = str.right(str.size()-(str.indexOf('=')+1));
                }
                else if (str.contains("default_dx")) {
                    m_default_dx_ini = str.right(str.size()-(str.indexOf('=')+1));
                }
                else if (str.contains("default_dy")) {
                    m_default_dy_ini = str.right(str.size()-(str.indexOf('=')+1));
                }
                else if (str.contains("opacity_mode")) {
                    m_opacity_mode_ini = str.right(str.size()-(str.indexOf('=')+1));
                }
                else if (str.contains("opacity_value")) {
                    m_opacity_value_ini = str.right(str.size()-(str.indexOf('=')+1));
                }
                else if (str.contains("frame_thickness")) {
                    m_frame_thickness_ini = str.right(str.size()-(str.indexOf('=')+1));
                }
            }
        }
        file.close();        
    }
    emit ready_to_init();
}

void controller::run_all()
{
    QUrl savepath(m_save_path_ini);
    QString savepath_not_url = savepath.toLocalFile();

    if ((QDir(savepath_not_url).exists())&&(!savepath_not_url.isEmpty())){

        for (int i=0; i<threads.size(); i++){
            delete threads.at(i).future_handle;
            delete threads.at(i).processor_handle;
        }
        count_of_finished_processes = 0;
        threads.clear();

        thread_struct thread;
        thread.widget_index = 0;
        thread.future_handle = nullptr;
        thread.processor_handle = nullptr;
        Processor* p = nullptr;
        QFuture<int>* future = nullptr;

        everything_was_ok = true;
        at_least_one_done = false;
        bool outline_was_found = false;
        QString outline_name;
        double w = m_default_image_width_ini.toDouble(), h = m_default_image_height_ini.toDouble(), dx = m_default_dx_ini.toDouble(), dy = m_default_dy_ini.toDouble();   //размеры платы и смещение начала координат по умолчанию...

        //  removing duplicates ".board"
        for (int i =0; i<list_of_gerbers.size(); i++){
            if (list_of_gerbers.at(i).contains(".board")){
                if (outline_was_found){
                    emit message("Duplicate of outline files detected!\nFile " + list_of_gerbers.at(i) + " will not be translated.");
                    list_of_gerbers.removeAt(i);
                }
                else {
                    outline_was_found=true;
                }
            }
        }

        //  try to find filename containing ".board",

        for (int i=0;i<list_of_gerbers.size();i++) {
            if (list_of_gerbers.at(i).contains(".board")){
                p = new Processor(1);
                p->set_frame_thickness(m_frame_thickness_ini.toDouble());
                p->set_dpi(m_dpi_ini.toInt());
                p->set_image_format(m_image_format_ini);
                p->set_paths(list_of_gerbers.at(i),m_save_path_ini);
                p->load_file();
                if (m_image_size_ini=="by_outline"){
                    p->get_outline_size(&w, &h, &dx, &dy);
                }
                if ((m_opacity_mode_ini=="on")&&(m_image_format_ini=="png")){
                    p->set_opacity_value(m_opacity_value_ini.toFloat());
                }
                outline_name = p->get_outline_filename();
                p->set_w_h_dx_dy(w,h,dx,dy);

                // starting new thread:
                future = new QFuture<int>;
                thread.future_handle = future;
                thread.processor_handle = p;
                thread.widget_index = i;
                threads.append(thread);
                connect(p, SIGNAL(finished()), this, SLOT(process_finished()));
                *future = QtConcurrent::run(p,&Processor::process);

                return_code = future->result();
                if ((return_code)>-1){
                    at_least_one_done = true;
                }
                else {
                    everything_was_ok = false;
                    break;
                }
                break;
            }
        }



        //  starting processing of all files, if outline was processed succesfully
        if ((at_least_one_done)||(m_image_size_ini=="by_ini")){
            for (int i=0; i<list_of_gerbers.size(); i++) {
                if (!(list_of_gerbers.at(i).contains(".board"))){

                    p = new Processor(0);
                    p->set_frame_thickness(m_frame_thickness_ini.toDouble());
                    p->set_dpi(m_dpi_ini.toInt());
                    p->set_image_format(m_image_format_ini);
                    p->set_paths(list_of_gerbers.at(i),m_save_path_ini);
                    p->load_file();
                    if ((m_opacity_mode_ini=="on")&&(m_image_format_ini=="png")){
                        p->set_opacity_value(m_opacity_value_ini.toFloat());
                    }
                    p->set_outline_file_name(outline_name);
                    p->set_w_h_dx_dy(w,h,dx,dy);

                    // starting new thread:
                    future = new QFuture<int>;
                    thread.future_handle = future;
                    thread.processor_handle = p;
                    thread.widget_index = i;
                    threads.append(thread);
                    connect(p, SIGNAL(finished()), this, SLOT(process_finished()));
                    *future = QtConcurrent::run(p,&Processor::process);
                }//end of if
            }//end of for
        }
        else {
            emit processing_done("Files will not be processed because outline file not processed!");
        }
    } // end of if
    else {
        emit processing_done("The specified directory for saving images does not exist!");
    }
}

void controller::process_finished(){
    int i = 0;  // index of process sending signal to that slot

    for (int j=0; j<threads.size(); j++){
        if (threads.at(j).processor_handle == QObject::sender()){
            i = j;
        }
    }

    if (!threads.at(i).processor_handle->is_outline()) {
        return_code = threads.at(i).future_handle->result();
    }

    if ((return_code)>-1){
        at_least_one_done = true;
    }
    else {
        everything_was_ok = false;
        QString err_msg;
        if (return_code == -3){
            err_msg = "Error!\nFile " + list_of_gerbers.at(threads.at(i).widget_index) + " is not processed.\nImage may be too large and not enough memory to process it.";
        }
        else if (return_code == -4) {
            err_msg = "Error!\nImage for file " + list_of_gerbers.at(threads.at(i).widget_index) + " can not be saved.\nIt may be too large and not enough memory to process it.";
        }
        else {
            err_msg = "Error!\nFile " + list_of_gerbers.at(threads.at(i).widget_index) + " is not processed.\nFile does not exist or file type is incorrect.";
        }
        emit message(err_msg);
    }
    count_of_finished_processes++;
    emit increase_progress(1);

    if (count_of_finished_processes==list_of_gerbers.size()) {

        if (everything_was_ok) {
            emit processing_done("Translation is finished succesfully!");
        }
        else if (at_least_one_done) {
            emit processing_done("Translation is finished with errors!");
        }
        else {
            emit processing_done("Error! No file processed!");
        }

        if ((m_open_folder_after_processing_ini=="on")&&at_least_one_done) {

            QString local_ver_file_name = "gt_version";
            QString platform;

            QFile loc_version_file(local_ver_file_name);
            if(!loc_version_file.open(QIODevice::ReadOnly | QIODevice::Text)){
                qDebug()<<"Can not open local version file!";
            }
            while (!loc_version_file.atEnd()){
                QByteArray str = loc_version_file.readLine().trimmed();
                // читаю ОС
                if (str.contains("platform")){
                    platform = str.right(str.size()-(str.indexOf('=')+1));
                    loc_version_file.close();
                    break;
                }
            }

            if (platform=="windows"){
                QProcess process(this);
                process.startDetached("explorer " + m_save_path_ini);
            }
            else if (platform=="linux") {
                QProcess process(this);
                process.startDetached("nautilus " + m_save_path_ini);
            }
        }
        threads.clear();
    }
}

void controller::prepare_for_exit(){    
        for (int i=0; i<threads.size(); i++){
            delete threads.at(i).future_handle;
            delete threads.at(i).processor_handle;
        }

        //  Write settings file before exit..
        QFile file("gerber_translator.ini");
        if (!file.open(QIODevice::WriteOnly | QIODevice::Text)){
            emit message("Error opening gerber_translator.ini for writing!");
            emit ready_to_exit();            
        }
        else{

            file.write("open_path="+m_open_path_ini.toUtf8()+'\n');
            file.write("save_path="+m_save_path_ini.toUtf8()+'\n');
            file.write("image_format="+m_image_format_ini.toUtf8()+'\n');
            file.write("dpi="+m_dpi_ini.toUtf8()+'\n');
            file.write("quick_translation="+m_quick_translation_ini.toUtf8()+'\n');
            file.write("open_folder_after_processing="+m_open_folder_after_processing_ini.toUtf8()+'\n');
            file.write("image_size="+m_image_size_ini.toUtf8()+'\n');
            file.write("default_image_width="+m_default_image_width_ini.toUtf8()+'\n');
            file.write("default_image_height="+m_default_image_height_ini.toUtf8()+'\n');
            file.write("default_dx="+m_default_dx_ini.toUtf8()+'\n');
            file.write("default_dy="+m_default_dy_ini.toUtf8()+'\n');
            file.write("opacity_mode="+m_opacity_mode_ini.toUtf8()+'\n');
            file.write("opacity_value="+m_opacity_value_ini.toUtf8()+'\n');
            file.write("frame_thickness="+m_frame_thickness_ini.toUtf8()+'\n');
            file.write("\n% Comments:\n");
            file.write("% open_path = [default open path]\n");
            file.write("% save_path = [default save path]\n");
            file.write("% image_format = [bmp][png]\n");
            file.write("% dpi = [resolution]\n");
            file.write("% quick_translation = [on][off]\n");
            file.write("% open_folder_after_processing = [on][off]\n");
            file.write("% image_size = [by_outline][by_ini]\n");
            file.write("% default_image_width = [default board width]\n");
            file.write("% default_image_height = [default board height]\n");
            file.write("% default_dx = [default board origin offset]\n");
            file.write("% default_dy = [default board origin offset]\n");
            file.write("% opacity_mode = [on][off]\n");
            file.write("% opacity_value = [opacity - real from 0 to 1]\n");
            file.write("% frame_thickness = [thickness of fields around the image in mm]");

            file.close();
            emit ready_to_exit();
        }
}
