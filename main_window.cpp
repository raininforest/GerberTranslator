#include "main_window.h"
#include "ui_main_window.h"

Main_window::Main_window(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::Main_window)
{
    ui->setupUi(this);

    QObject::connect(ui->open_button,SIGNAL(clicked()),SLOT(open_slot()));
    QObject::connect(ui->actionOpen,SIGNAL(triggered()),SLOT(open_slot()));
    QObject::connect(ui->start_button,SIGNAL(clicked()),SLOT(save_slot()));
    QObject::connect(ui->save_button,SIGNAL(clicked()),SLOT(what_save_path()));
    QObject::connect(ui->actionSave,SIGNAL(triggered()),SLOT(what_save_path()));
    QObject::connect(ui->action_2,SIGNAL(triggered()),SLOT(show_about()));
    QObject::connect(ui->actionExit,SIGNAL(triggered()),SLOT(exit_slot()));

    //  Чтение файла настроек, инициализация...
    QFile file("gerber_translator.ini");
    QStringList strings_of_ini;
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)){
        msgBox.setWindowTitle("Gerber-транслятор");
        msgBox.setText("Не найден gerber_translator.ini! Будет создан новый файл.");
        msgBox.exec();
    }
    else {
        while (!file.atEnd()) {
            QByteArray str = file.readLine().trimmed();

            if (!str.contains('%')) {
                if (str.contains("open_path")){
                    open_path_ini = str.right(str.size()-(str.indexOf('=')+1));
                }
                else if (str.contains("save_path")) {
                    save_path_ini = str.right(str.size()-(str.indexOf('=')+1));
                    ui->lineEdit->setText(save_path_ini);
                }
                else if (str.contains("image_format")) {
                    image_format_ini = str.right(str.size()-(str.indexOf('=')+1));
                    ui->comboBox_format->setCurrentText(image_format_ini);
                }
                else if (str.contains("dpi")) {
                    dpi_ini = str.right(str.size()-(str.indexOf('=')+1));
                    bool is_dpi_correct = false;
                    dpi_ini.toInt(&is_dpi_correct);
                    if (is_dpi_correct){
                        ui->comboBox_dpi->setCurrentText(dpi_ini);
                    }
                    else {
                        msgBox.setWindowTitle("Gerber-транслятор");
                        msgBox.setText("Невозможно установить разрешение из файла настроек gerber_translator.ini!\n Будет установлено значение по умолчанию 150 dpi.");
                        msgBox.exec();
                    }
                }
                else if (str.contains("quick_translation")) {
                    quick_translation_ini = str.right(str.size()-(str.indexOf('=')+1));
                    if (quick_translation_ini=="on"){
                        ui->action_3->setChecked(true);
                    }
                    else if (quick_translation_ini=="off") {
                        ui->action_3->setChecked(false);
                    }
                }
                else if (str.contains("open_folder_after_processing")) {
                    open_folder_after_processing_ini = str.right(str.size()-(str.indexOf('=')+1));
                    if (open_folder_after_processing_ini=="on"){
                        ui->action_7->setChecked(true);
                    }
                    else if (open_folder_after_processing_ini=="off") {
                        ui->action_7->setChecked(false);
                    }
                }
                else if (str.contains("image_size")) {
                    image_size_ini = str.right(str.size()-(str.indexOf('=')+1));
                    if (image_size_ini=="by_ini"){
                        ui->action_6->setChecked(true);
                    }
                    else if (image_size_ini=="by_outline") {
                        ui->action_6->setChecked(false);
                    }
                }
                else if (str.contains("default_image_width")) {
                    default_image_width_ini = str.right(str.size()-(str.indexOf('=')+1));
                }
                else if (str.contains("default_image_height")) {
                    default_image_height_ini = str.right(str.size()-(str.indexOf('=')+1));
                }
                else if (str.contains("default_dx")) {
                    default_dx_ini = str.right(str.size()-(str.indexOf('=')+1));
                }
                else if (str.contains("default_dy")) {
                    default_dy_ini = str.right(str.size()-(str.indexOf('=')+1));
                }
                else if (str.contains("opacity_mode")) {
                    opacity_mode_ini = str.right(str.size()-(str.indexOf('=')+1));
                    if (opacity_mode_ini=="on"){
                        ui->action_5->setChecked(true);
                    }
                    else if (opacity_mode_ini=="off") {
                        ui->action_5->setChecked(false);
                    }
                }
                else if (str.contains("opacity_value")) {
                    opacity_value_ini = str.right(str.size()-(str.indexOf('=')+1));
                }
                else if (str.contains("frame_thickness")) {
                    frame_thickness_ini = str.right(str.size()-(str.indexOf('=')+1));
                }
            }
        }
        file.close();
    }

}

Main_window::~Main_window()
{

    image_format_ini = ui->comboBox_format->currentText();
    dpi_ini = ui->comboBox_dpi->currentText();
    save_path_ini = ui->lineEdit->text();

    if (ui->action_3->isChecked()){
        quick_translation_ini = "on";
    }
    else {
        quick_translation_ini = "off";
    }
    if (ui->action_7->isChecked()){
        open_folder_after_processing_ini = "on";
    }
    else {
        open_folder_after_processing_ini = "off";
    }
    if (ui->action_6->isChecked()){
        image_size_ini = "by_ini";
    }
    else {
        image_size_ini = "by_outline";
    }
    if (ui->action_5->isChecked()){
        opacity_mode_ini = "on";
    }
    else {
        opacity_mode_ini = "off";
    }

    //  Запись файла настроек перед закрытием...
    QFile file("gerber_translator.ini");
    QStringList strings_of_ini;
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)){
        ui->listWidget->addItem("Error opening gerber_translator.ini!");
        //  сообщение об ошибке чтения файла с настройками
    }

        file.write("open_path="+open_path_ini.toUtf8()+'\n');
        file.write("save_path="+save_path_ini.toUtf8()+'\n');
        file.write("image_format="+image_format_ini.toUtf8()+'\n');
        file.write("dpi="+dpi_ini.toUtf8()+'\n');
        file.write("quick_translation="+quick_translation_ini.toUtf8()+'\n');
        file.write("open_folder_after_processing="+open_folder_after_processing_ini.toUtf8()+'\n');
        file.write("image_size="+image_size_ini.toUtf8()+'\n');
        file.write("default_image_width="+default_image_width_ini.toUtf8()+'\n');
        file.write("default_image_height="+default_image_height_ini.toUtf8()+'\n');
        file.write("default_dx="+default_dx_ini.toUtf8()+'\n');
        file.write("default_dy="+default_dy_ini.toUtf8()+'\n');
        file.write("opacity_mode="+opacity_mode_ini.toUtf8()+'\n');
        file.write("opacity_value="+opacity_value_ini.toUtf8()+'\n');
        file.write("frame_thickness="+frame_thickness_ini.toUtf8()+'\n');
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
    delete ui;
}

void Main_window::show_about(){

    QMessageBox::about(this, "О программе", "Gerber-транслятор.\n\nВерсия: 1.9.\nДата релиза: 21.09.2019\nАвтор: Велеско С.А.");

}

void Main_window::open_slot()
{
    //
    //  Получение списка файлов, подлежащих обработке. Запись списка в listWidget.
    //
    QFileDialog *OpnDlg = new QFileDialog(nullptr);
    QStringList List_of_files;
    List_of_files = OpnDlg->getOpenFileNames(this, "Выберите файлы для загрузки", open_path_ini, "");
    if (List_of_files.size())  //если список не пуст(когда ничего не выбрал), то..
    {
        int outlines_counter = 0;
        for (int i=0; i<List_of_files.size(); i++){
            if (List_of_files.at(i).contains(".board")){
                outlines_counter++;
                if (outlines_counter>1){
                    List_of_files.removeAt(i);
                }
            }
        }

        ui->listWidget->clear();
        ui->listWidget->addItems(List_of_files);
        ui->listWidget->setEnabled(true);        
        ui->start_button->setEnabled(true);
        ui->actionSave->setEnabled(true);
        ui->progressBar->setEnabled(true);
        ui->save_button->setEnabled(true);
        ui->comboBox_dpi->setEnabled(true);
        ui->comboBox_format->setEnabled(true);
        ui->label->setEnabled(true);
        ui->lineEdit->setEnabled(true);

        ui->progressBar->setRange(0,ui->listWidget->count());
        ui->progressBar->setValue(ui->progressBar->minimum());

        open_path_ini = List_of_files.at(0).left(List_of_files.at(0).lastIndexOf('/'));

        //  запуск обработки, если включена опция мгновенной трансляции
        if (ui->action_3->isChecked()) {
            save_slot();
        }
    }
    delete OpnDlg;
}

void Main_window::exit_slot()
{
    // Особождение динамической памяти
        for (int i=0; i<threads.size(); i++){
            delete threads.at(i).future_handle;
            delete threads.at(i).processor_handle;
        }
    this->close();
    //  остальные заключительные операции выполняются в деструкторе окна..
}

void Main_window::save_slot()
{    
    if ((QDir(ui->lineEdit->text()).exists())&&(!ui->lineEdit->text().isEmpty())){

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

        // установка цвета файлов в исходный черный
        for (int i=0; i<ui->listWidget->count(); i++){
            ui->listWidget->item(i)->setForeground(Qt::black);
        }

        // блокировка некоторого gui на время выполнения
        ui->start_button->setEnabled(false);
        ui->open_button->setEnabled(false);
        ui->save_button->setEnabled(false);
        ui->comboBox_format->setEnabled(false);
        ui->comboBox_dpi->setEnabled(false);
        ui->lineEdit->setEnabled(false);

        everything_was_ok = true;
        at_least_one_done = false;
        QString outline_name;
        double w = default_image_width_ini.toDouble(), h = default_image_height_ini.toDouble(), dx = default_dx_ini.toDouble(), dy = default_dy_ini.toDouble();   //размеры платы и смещение начала координат по умолчанию...

        ui->progressBar->setTextVisible(true);
        ui->progressBar->setValue(ui->progressBar->minimum());

        //  поиск в списке загруженных герберов файла с ".board",
        //  чтобы рассчитать размеры (платы) изображения для всех загруженных герберов по контуру.
        for (int i=0;i<ui->listWidget->count();i++) {
            if (ui->listWidget->item(i)->text().contains(".board")){
                p = new Processor(1);
                p->set_frame_thickness(frame_thickness_ini.toDouble());
                p->set_dpi(ui->comboBox_dpi->currentText().toInt());     //  утстановка пользовательского разрешения в dpi
                p->set_image_format(ui->comboBox_format->currentText());
                p->set_paths(ui->listWidget->item(i)->text(),ui->lineEdit->text());
                p->load_file();      //  загрузка файла в память
                if (!ui->action_6->isChecked()){
                    p->get_outline_size(&w, &h, &dx, &dy);           //  рассчет ширины и высоты рисунка, а также смещения начала координат (например, оно в центре круглой платы)
                }
                if ((ui->action_5->isChecked())&&(ui->comboBox_format->currentText()=="png")){
                    p->set_opacity_value(opacity_value_ini.toFloat());
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
        if ((at_least_one_done)||(ui->action_6->isChecked())){
            for (int i=0; i<ui->listWidget->count(); i++) {
                if (!(ui->listWidget->item(i)->text().contains(".board"))){

                    p = new Processor(0);
                    p->set_frame_thickness(frame_thickness_ini.toDouble());
                    p->set_dpi(ui->comboBox_dpi->currentText().toInt());
                    p->set_image_format(ui->comboBox_format->currentText());
                    p->set_paths(ui->listWidget->item(i)->text(),ui->lineEdit->text());
                    p->load_file();                                                  //  загрузка файла в память
                    if ((ui->action_5->isChecked())&&(ui->comboBox_format->currentText()=="png")){
                        p->set_opacity_value(opacity_value_ini.toFloat());
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
            for (int i=0; i<ui->listWidget->count();i++) {
                ui->listWidget->item(i)->setForeground(Qt::red);
            }
            msgBox.setWindowTitle("Gerber-транслятор");
            msgBox.setText("Файлы не будут обработаны, т.к. не обработан файл контура!");
            msgBox.exec();
            ui->listWidget->setEnabled(true);
            ui->open_button->setEnabled(true);
            ui->save_button->setEnabled(true);
            ui->actionSave->setEnabled(true);
            ui->start_button->setEnabled(true);
            ui->comboBox_dpi->setEnabled(true);
            ui->comboBox_format->setEnabled(true);
            ui->label->setEnabled(true);
            ui->lineEdit->setEnabled(true);
        }
    } // end of if
    else {
        //  Выдача сообщения об ошибке
        msgBox.setWindowTitle("Gerber-транслятор");
        msgBox.setText("Указанный каталог не существует!");
        msgBox.exec();
    }

}   //end of slot


void Main_window::process_finished(){

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
        ui->listWidget->item(threads.at(i).widget_index)->setForeground(Qt::green);
        at_least_one_done = true;
    }
    else {
        everything_was_ok = false;
        ui->listWidget->item(threads.at(i).widget_index)->setForeground(Qt::red);
        QString err_msg;
        if (return_code == -3){
            err_msg = "Ошибка!\nФайл " + ui->listWidget->item(threads.at(i).widget_index)->text() + " не обработан.\nВозможно, файл слишком большой и недостаточно памяти для его обработки.";
        }
        else if (return_code == -4) {
            err_msg = "Ошибка!\nФайл " + ui->listWidget->item(threads.at(i).widget_index)->text() + " не удается сохранить.\nВозможно, файл слишком большой и недостаточно памяти для его обработки.";
        }
        else {
            err_msg = "Ошибка!\nФайл " + ui->listWidget->item(threads.at(i).widget_index)->text() + " не обработан.\nФайл не существует или неверный тип файла.";
        }
        msgBox.setWindowTitle("Gerber-транслятор");
        msgBox.setText(err_msg);
        msgBox.exec();
    }
    count_of_finished_processes++;
    ui->progressBar->setValue((ui->progressBar->value()) + 1);  //  приращение индикатора прогресса


    if (count_of_finished_processes==ui->listWidget->count()) {

        ui->progressBar->setValue(ui->progressBar->maximum());
        if (everything_was_ok) {
            msgBox.setWindowTitle("Gerber-транслятор");
            msgBox.setText("Обработка файлов успешно завершена!");
            msgBox.exec();
        }
        else if (at_least_one_done) {
            msgBox.setWindowTitle("Gerber-транслятор");
            msgBox.setText("Обработка файлов завершена c ошибками!");
            msgBox.exec();
        }
        else {
            msgBox.setWindowTitle("Gerber-транслятор");
            msgBox.setText("Ошибка! Ни один файл не обработан!");
            msgBox.exec();
        }

        ui->start_button->setEnabled(true);
        ui->open_button->setEnabled(true);
        ui->save_button->setEnabled(true);
        ui->comboBox_format->setEnabled(true);
        ui->comboBox_dpi->setEnabled(true);
        ui->lineEdit->setEnabled(true);

        if (ui->action_7->isChecked()&&at_least_one_done) {

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
                process.startDetached("explorer " + ui->lineEdit->text().replace(('/'),('\\')));
            }
            else if (platform=="linux") {
                QProcess process(this);
                process.startDetached("nautilus " + ui->lineEdit->text());
            }
        }
        threads.clear();
    }
}


void Main_window::what_save_path(){

    QString path_str;
    QFileDialog *SaveDlg = new QFileDialog(nullptr);
    path_str = SaveDlg->getExistingDirectory(this, "Укажите путь для сохранения изображений", save_path_ini, QFileDialog::ShowDirsOnly);
    if (path_str!=""){
        ui->lineEdit->setText(path_str);
        save_path_ini = path_str;
    }
    delete SaveDlg;
    ui->start_button->setEnabled(true);
    ui->comboBox_dpi->setEnabled(true);
    ui->comboBox_format->setEnabled(true);
    ui->label->setEnabled(true);
    ui->lineEdit->setEnabled(true);

}

void Main_window::dragEnterEvent(QDragEnterEvent* event){
    event->acceptProposedAction();
}

void Main_window::dropEvent(QDropEvent *event){

    QList<QUrl> urls = event->mimeData()->urls();
    if (urls.isEmpty()){
        return;
    }
    ui->listWidget->clear();

    QString item_path_text="";
    for (int i=0; i<urls.size(); i++) {
        item_path_text = urls.at(i).toString(QUrl::PreferLocalFile);
        item_path_text.replace("%5B","[");
        item_path_text.replace("%5D","]");
        ui->listWidget->addItem(item_path_text);
    }
    ui->listWidget->setEnabled(true);
    ui->save_button->setEnabled(true);
    ui->actionSave->setEnabled(true);
    ui->start_button->setEnabled(true);
    ui->comboBox_dpi->setEnabled(true);
    ui->comboBox_format->setEnabled(true);
    ui->label->setEnabled(true);
    ui->lineEdit->setEnabled(true);

    ui->progressBar->setRange(0,ui->listWidget->count());
    ui->progressBar->setValue(ui->progressBar->minimum());

    //  вызов слота save_slot(), если включена мгновенная трансляция
    if (ui->action_3->isChecked()) {
        save_slot();
    }
}

void Main_window::dpi_changed(const QString &arg1)
{
    bool ok;    
    arg1.toInt(&ok);
    if (!ok){
        msgBox.setWindowTitle("Gerber-транслятор");
        msgBox.setText("Количество точек на дюйм должно быть представлено целым числом!");
        msgBox.exec();
    }
}

void Main_window::on_comboBox_dpi_currentTextChanged(const QString &arg1)
{
    bool is_dpi_correct = false;
    arg1.toInt(&is_dpi_correct);
    if (is_dpi_correct){
        ui->comboBox_dpi->setCurrentText(arg1);
    }
    else {
        msgBox.setWindowTitle("Gerber-транслятор");
        msgBox.setText("Разрешение должно быть целым числом!");
        msgBox.exec();
    }
    if (arg1.toInt(&is_dpi_correct)>1200) {
        msgBox.setWindowTitle("Gerber-транслятор");
        msgBox.setText("Текущее разрешение может оказаться слишком большим.\nОбработка файлов может стать невозможной либо займет дополнительное время.");
        msgBox.exec();
    }
}
