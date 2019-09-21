#ifndef MAIN_WINDOW_H
#define MAIN_WINDOW_H

#include <QMainWindow>
#include <QListWidget>
#include <QMessageBox>
#include <QDebug>
#include <QFuture>
#include <QtConcurrent/QtConcurrent>
#include <QTime>
#include <QFileDialog>
#include <QMessageBox>
#include <QFile>
#include <QDragEnterEvent>
#include <QList>
#include <QUrl>
#include <QMimeData>
#include <QProcess>

#include "processor.h"

namespace Ui {
class Main_window;
}

class Main_window : public QMainWindow
{
    Q_OBJECT

public:
    explicit Main_window(QWidget *parent = nullptr);
    ~Main_window();

protected:

    void dragEnterEvent(QDragEnterEvent* event);

    void dropEvent(QDropEvent *event);

private slots:

    void open_slot();

    void save_slot();

    void what_save_path();

    void exit_slot();

    void show_about();    

    void dpi_changed(const QString &arg1);

    void process_finished();

    void on_comboBox_dpi_currentTextChanged(const QString &arg1);

private:

    struct thread_struct{
        QFuture<int>* future_handle;    // указатель на поток
        Processor* processor_handle;    // указатель на объект
        int widget_index;
    };

    QList<thread_struct> threads;       // список потоков для обработки
    int count_of_finished_processes = 0;// счетчик завершившихся потоков
    int return_code=-1;

    Ui::Main_window *ui;
    QMessageBox msgBox;                 // Окно для сообщений

    bool everything_was_ok;             // флаг: все файлы были корректно обработаны
    bool at_least_one_done;             // флаг: хотя бы один файл был обработан
    QString open_path_ini="d:/";
    QString save_path_ini="d:/";
    QString image_format_ini;
    QString dpi_ini;
    QString quick_translation_ini;
    QString open_folder_after_processing_ini;
    QString image_size_ini;
    QString opacity_mode_ini;
    QString opacity_value_ini="0.8";
    QString default_image_width_ini="100";
    QString default_image_height_ini="100";
    QString default_dx_ini="0";
    QString default_dy_ini="0";
    QString frame_thickness_ini="1";

};

#endif // MAIN_WINDOW_H
