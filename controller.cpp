#include "controller.h"

controller::controller()
{

}

void controller::load_ini_file(){
    //  Чтение файла настроек, инициализация...
    QFile file("gerber_translator.ini");
    QStringList strings_of_ini;
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)){
        emit message("Не найден gerber_translator.ini. Будет создан новый файл.");
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
                        emit message("Невозможно установить разрешение из файла настроек gerber_translator.ini!\n Будет установлено значение по умолчанию.");
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
    if ((QDir(m_save_path_ini).exists())&&(!m_save_path_ini.isEmpty())){

        // Особождение динамической памяти
        for (int i=0; i<threads.size(); i++){
            delete threads.at(i).future_handle;
            delete threads.at(i).processor_handle;
        }
        //  Сброс счетчика потоков
        count_of_finished_processes = 0;
        // Очищение спика потоков
        threads.clear();

        thread_struct thread;
        thread.widget_index = 0;
        thread.future_handle = nullptr;
        thread.processor_handle = nullptr;
        Processor* p = nullptr;
        QFuture<int>* future = nullptr;

        everything_was_ok = true;
        at_least_one_done = false;
        QString outline_name;
        double w = m_default_image_width_ini.toDouble(), h = m_default_image_height_ini.toDouble(), dx = m_default_dx_ini.toDouble(), dy = m_default_dy_ini.toDouble();   //размеры платы и смещение начала координат по умолчанию...

        //  поиск в списке загруженных герберов файла с ".board",
        //  чтобы рассчитать размеры (платы) изображения для всех загруженных герберов по контуру.
        for (int i=0;i<list_of_gerbers.size();i++) {
            if (list_of_gerbers.at(i).contains(".board")){
                p = new Processor(1);
                p->set_frame_thickness(m_frame_thickness_ini.toDouble());
                p->set_dpi(m_dpi_ini.toInt());     //  утстановка пользовательского разрешения в dpi
                p->set_image_format(m_image_format_ini);
                p->set_paths(list_of_gerbers.at(i),m_save_path_ini);
                p->load_file();      //  загрузка файла в память
                if (m_image_size_ini=="by_outline"){
                    p->get_outline_size(&w, &h, &dx, &dy);           //  рассчет ширины и высоты рисунка, а также смещения начала координат (например, оно в центре круглой платы)
                }
                if ((m_opacity_mode_ini=="on")&&(m_image_format_ini=="png")){
                    p->set_opacity_value(m_opacity_value_ini.toFloat());
                }
                outline_name = p->get_outline_filename();
                p->set_w_h_dx_dy(w,h,dx,dy);

                // обработка:
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
                break; // т.к. нашелся контур платы - выход из цикла
            }
        }

        //  Цикл обработки всех загруженных файлов, кроме файла контура - формирование изображения
        //  Если обработка файла контура прошла успешно
        if ((at_least_one_done)||(m_image_size_ini=="by_ini")){
            for (int i=0; i<list_of_gerbers.size(); i++) {
                if (!(list_of_gerbers.at(i).contains(".board"))){

                    p = new Processor(0);
                    p->set_frame_thickness(m_frame_thickness_ini.toDouble());
                    p->set_dpi(m_dpi_ini.toInt());
                    p->set_image_format(m_image_format_ini);
                    p->set_paths(list_of_gerbers.at(i),m_save_path_ini);
                    p->load_file();                                                  //  загрузка файла в память
                    if ((m_opacity_mode_ini=="on")&&(m_image_format_ini=="png")){
                        p->set_opacity_value(m_opacity_value_ini.toFloat());
                    }
                    p->set_outline_file_name(outline_name); //  установка пути к изображению контура
                    p->set_w_h_dx_dy(w,h,dx,dy);

                    //обработка:
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
            emit processing_done("Файлы не будут обработаны, т.к. не обработан файл контура!");
        }
    } // end of if
    else {
        //  Выдача сообщения об ошибке
        emit processing_done("Указанный каталог не существует!");
    }
}

void controller::process_finished(){
    int i = 0;  // индекс процесса, который послал сигнал в этот слот.

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
            err_msg = "Ошибка!\nФайл " + list_of_gerbers.at(threads.at(i).widget_index) + " не обработан.\nВозможно, файл слишком большой и недостаточно памяти для его обработки.";
        }
        else if (return_code == -4) {
            err_msg = "Ошибка!\nФайл " + list_of_gerbers.at(threads.at(i).widget_index) + " не удается сохранить.\nВозможно, файл слишком большой и недостаточно памяти для его обработки.";
        }
        else {
            err_msg = "Ошибка!\nФайл " + list_of_gerbers.at(threads.at(i).widget_index) + " не обработан.\nФайл не существует или неверный тип файла.";
        }
        emit message(err_msg);
    }
    count_of_finished_processes++;
    emit increase_progress(1);

    if (count_of_finished_processes==list_of_gerbers.size()) {

        if (everything_was_ok) {
            emit processing_done("Обработка файлов успешно завершена!");
        }
        else if (at_least_one_done) {
            emit processing_done("Обработка файлов завершена c ошибками!");
        }
        else {
            emit processing_done("Ошибка! Ни один файл не обработан!");
        }

        if ((m_open_folder_after_processing_ini=="on")&&at_least_one_done) {

            QString local_ver_file_name = "gt_version";
            QString platform;

            QFile loc_version_file(local_ver_file_name);
            if(!loc_version_file.open(QIODevice::ReadOnly | QIODevice::Text)){
                qDebug()<<"Невозможно открыть локальный файл версии!";
                //сообщение об ошибке
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
                process.startDetached("explorer " + m_save_path_ini.replace(('/'),('\\')));
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
    // Особождение динамической памяти
        for (int i=0; i<threads.size(); i++){
            delete threads.at(i).future_handle;
            delete threads.at(i).processor_handle;
        }

        //  Запись файла настроек перед закрытием...
        QFile file("gerber_translator.ini");
        if (!file.open(QIODevice::WriteOnly | QIODevice::Text)){
            //            ui->listWidget->addItem("Error opening gerber_translator.ini!");
            emit message("Error opening gerber_translator.ini for writing!");
            emit ready_to_exit();
            //  сообщение об ошибке чтения файла с настройками
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
            file.write("\n% Комментарии:\n");
            file.write("% open_path = [путь для открытия файлов по-умолчанию]\n");
            file.write("% save_path = [путь для сохранения файлов по-умолчанию]\n");
            file.write("% image_format = [bmp][png]\n");
            file.write("% dpi = [разрешение в точках на дюйм - целое число]\n");
            file.write("% quick_translation = [on][off]\n");
            file.write("% open_folder_after_processing = [on][off]\n");
            file.write("% image_size = [by_outline][by_ini]\n");
            file.write("% default_image_width = [ширина платы по-умолчанию]\n");
            file.write("% default_image_height = [длина платы по-умолчанию]\n");
            file.write("% default_dx = [смещение начала координат платы по-умолчанию]\n");
            file.write("% default_dy = [смещение начала координат платы по-умолчанию]\n");
            file.write("% opacity_mode = [on][off]\n");
            file.write("% opacity_value = [прозрачность - вещественное число от 0 до 1]\n");
            file.write("% frame_thickness = [толщина полей вокруг изображения в мм]");

            file.close();
            qDebug()<<"ini was saved. ready to exit.";
            emit ready_to_exit();
        }
}
