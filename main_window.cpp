#include "main_window.h"
#include "ui_main_window.h"

Main_window::Main_window(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::Main_window)
{
    ui->setupUi(this);

    QObject::connect(ui->open_button,SIGNAL(clicked()),SLOT(open_slot()));
    QObject::connect(ui->actionOpen,SIGNAL(triggered()),SLOT(open_slot()));
    QObject::connect(ui->save_button,SIGNAL(clicked()),SLOT(what_save_path()));
    QObject::connect(ui->actionSave,SIGNAL(triggered()),SLOT(what_save_path()));
    QObject::connect(ui->actionExit,SIGNAL(triggered()),SIGNAL(close_app()));
    QObject::connect(ui->action_3,SIGNAL(triggered(bool)),SLOT(quick_translation_changed(bool)));
    QObject::connect(ui->action_7,SIGNAL(triggered(bool)),SLOT(open_folder_after_changed(bool)));
    QObject::connect(ui->action_6,SIGNAL(triggered(bool)),SLOT(image_size_changed(bool)));
    QObject::connect(ui->action_5,SIGNAL(triggered(bool)),SLOT(opacity_mode_changed(bool)));
    QObject::connect(ui->action_2,SIGNAL(triggered()),SLOT(show_about()));

}

Main_window::~Main_window()
{
    delete ui;
}

void Main_window::show_about(){

    QMessageBox::about(this, "О программе", "Gerber-транслятор.\n\nВерсия: 1.10.\nДата релиза: 01.10.2019\nАвтор: Велеско С.А.");

}

void Main_window::open_slot()
{
    //
    //  Получение списка файлов, подлежащих обработке. Запись списка в listWidget.
    //
    QFileDialog *OpnDlg = new QFileDialog(nullptr);
    QStringList List_of_files;
    List_of_files = OpnDlg->getOpenFileNames(this, "Выберите файлы для загрузки", cptr->m_open_path_ini, "");
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
        ui->save_button->setEnabled(true);
        ui->comboBox_dpi->setEnabled(true);
        ui->comboBox_format->setEnabled(true);
        ui->label->setEnabled(true);
        ui->lineEdit->setEnabled(true);

        ui->progressBar->setRange(0,ui->listWidget->count());
        ui->progressBar->setValue(ui->progressBar->minimum());

        cptr->list_of_gerbers.clear();
        cptr->list_of_gerbers=List_of_files;
        cptr->m_open_path_ini = List_of_files.at(0).left(List_of_files.at(0).lastIndexOf('/'));

        //  запуск обработки, если включена опция мгновенной трансляции
        if (ui->action_3->isChecked()) {
            on_start_button_clicked();
        }
    }
    delete OpnDlg;
}

void Main_window::what_save_path(){

    QString path_str;
    QFileDialog *SaveDlg = new QFileDialog(nullptr);
    path_str = SaveDlg->getExistingDirectory(this, "Укажите путь для сохранения изображений", cptr->m_save_path_ini, QFileDialog::ShowDirsOnly);
    if (path_str!=""){
        ui->lineEdit->setText(path_str);
        cptr->m_save_path_ini = path_str;
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
    cptr->list_of_gerbers.clear();

    QString item_path_text="";
    for (int i=0; i<urls.size(); i++) {
        item_path_text = urls.at(i).toString(QUrl::PreferLocalFile);
        item_path_text.replace("%5B","[");
        item_path_text.replace("%5D","]");
        ui->listWidget->addItem(item_path_text);
        cptr->list_of_gerbers.append(item_path_text);
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

    //  старт, если включена мгновенная трансляция
    if (ui->action_3->isChecked()) {
        on_start_button_clicked();
    }
}

void Main_window::closeEvent(QCloseEvent *event){
    event->ignore();
    emit close_app();
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

void Main_window::on_comboBox_format_currentTextChanged(const QString &arg1)
{
    cptr->m_image_format_ini=arg1;
}

void Main_window::on_comboBox_dpi_currentTextChanged(const QString &arg1)
{
    bool is_dpi_correct = false;
    arg1.toInt(&is_dpi_correct);
    if (is_dpi_correct){
        ui->comboBox_dpi->setCurrentText(arg1);
        cptr->m_dpi_ini=arg1;
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

void Main_window::quick_translation_changed(bool checked){
    if (checked){
        cptr->m_quick_translation_ini="on";
    }
    else {
        cptr->m_quick_translation_ini="off";
    }
}

void Main_window::open_folder_after_changed(bool checked){
    if (checked){
        cptr->m_open_folder_after_processing_ini="on";
    }
    else {
        cptr->m_open_folder_after_processing_ini="off";
    }
}

void Main_window::image_size_changed(bool checked){
    if (checked){
        cptr->m_image_size_ini="by_ini";
    }
    else {
        cptr->m_image_size_ini="by_outline";
    }
}

void Main_window::opacity_mode_changed(bool checked){
    if (checked){
        cptr->m_opacity_mode_ini="on";
    }
    else {
        cptr->m_opacity_mode_ini="off";
    }
}

void Main_window::exit_slot()
{
    this->close();
}

void Main_window::progress_bar(int val){
    ui->progressBar->setValue(ui->progressBar->value()+val);
}

void Main_window::show_message(QString msg_text){
    msgBox.setWindowTitle("Gerber-транслятор");
    msgBox.setText(msg_text);
    msgBox.exec();
}

void Main_window::done_slot(QString msg_text){
    ui->menuBar->setEnabled(true);
    ui->listWidget->setEnabled(true);
    ui->open_button->setEnabled(true);
    ui->save_button->setEnabled(true);
    ui->actionSave->setEnabled(true);
    ui->start_button->setEnabled(true);
    ui->comboBox_dpi->setEnabled(true);
    ui->comboBox_format->setEnabled(true);
    ui->label->setEnabled(true);
    ui->lineEdit->setEnabled(true);

    ui->progressBar->setValue(ui->progressBar->maximum());
    ui->progressBar->setEnabled(false);


    msgBox.setWindowTitle("Gerber-транслятор");
    msgBox.setText(msg_text);
    msgBox.exec();
}

void Main_window::set_ini_parameters(){

    ui->lineEdit->setText(cptr->m_save_path_ini);
    ui->comboBox_format->setCurrentText(cptr->m_image_format_ini);
    ui->comboBox_dpi->setCurrentText(cptr->m_dpi_ini);
    if (cptr->m_quick_translation_ini=="on"){
        ui->action_3->setChecked(true);
    }
    else if (cptr->m_quick_translation_ini=="off") {
        ui->action_3->setChecked(false);
    }
    if (cptr->m_open_folder_after_processing_ini=="on"){
        ui->action_7->setChecked(true);
    }
    else if (cptr->m_open_folder_after_processing_ini=="off") {
        ui->action_7->setChecked(false);
    }
    if (cptr->m_image_size_ini=="by_ini"){
        ui->action_6->setChecked(true);
    }
    else if (cptr->m_image_size_ini=="by_outline") {
        ui->action_6->setChecked(false);
    }
    if (cptr->m_opacity_mode_ini=="on"){
        ui->action_5->setChecked(true);
    }
    else if (cptr->m_opacity_mode_ini=="off") {
        ui->action_5->setChecked(false);
    }
}



void Main_window::on_start_button_clicked()
{
    ui->menuBar->setEnabled(false);
    ui->start_button->setEnabled(false);
    ui->open_button->setEnabled(false);
    ui->save_button->setEnabled(false);
    ui->comboBox_format->setEnabled(false);
    ui->comboBox_dpi->setEnabled(false);
    ui->lineEdit->setEnabled(false);
    ui->label->setEnabled(false);
    ui->progressBar->setEnabled(true);
    emit run_processing();
}


void Main_window::on_action_6_triggered()
{

}
