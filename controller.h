#ifndef CONTROLLER_H
#define CONTROLLER_H

#include "processor.h"
#include <QDebug>
#include <QFuture>
#include <QtConcurrent/QtConcurrent>
#include <QProcess>

class controller: public QObject
{
    Q_OBJECT

public:
    controller();

        //  глобальный список обрабатываемых файлов
    void load_ini_file();
    QStringList list_of_gerbers;

    QString m_open_path_ini="";
    QString m_save_path_ini="";
    QString m_image_format_ini="png";
    QString m_dpi_ini="300";
    QString m_quick_translation_ini="off";
    QString m_open_folder_after_processing_ini="on";
    QString m_image_size_ini="by_outline";
    QString m_opacity_mode_ini="off";
    QString m_opacity_value_ini="0.8";
    QString m_default_image_width_ini="100";
    QString m_default_image_height_ini="100";
    QString m_default_dx_ini="0";
    QString m_default_dy_ini="0";
    QString m_frame_thickness_ini="1";

private:

    struct thread_struct{
        QFuture<int>* future_handle;    // указатель на поток
        Processor* processor_handle;    // указатель на объект
        int widget_index;
    };

    QList<thread_struct> threads;       // список потоков для обработки
    int count_of_finished_processes = 0;// счетчик завершившихся потоков
    int count_of_gerbers = 0;
    int return_code=-1;

    bool everything_was_ok;             // флаг: все файлы были корректно обработаны
    bool at_least_one_done;             // флаг: хотя бы один файл был обработан

signals:

    void open_path_ini_Changed();
    void save_path_ini_Changed();
    void image_format_ini_Changed();
    void dpi_ini_Changed();
    void quick_translation_ini_Changed();
    void open_folder_after_processing_ini_Changed();
    void image_size_ini_Changed();
    void opacity_mode_ini_Changed();
    void opacity_value_ini_Changed();
    void default_image_width_ini_Changed();
    void default_image_height_ini_Changed();
    void default_dx_ini_Changed();
    void default_dy_ini_Changed();
    void frame_thickness_ini_Changed();
    void gerbers_string_Changed();

    //генерируется после чтения инифайла
    void ready_to_init();

    //во время обработки приращение прогресса
    void increase_progress(int);

    //при необходимости показать диалог с сообщением
    void message(QString);

    //текущий файл успешно обработан, в параметре его индекс в списке (виджете)
    void file_done(int);

    //признак конца текущей обработки всех файлов
    void processing_done(QString);

    //данные сохранены, память освобождена - можно закрывать приложение
    void ready_to_exit();

private slots:

    //принятие команды старт обработки от окна
    void run_all();//передать структуру с параметрами

    //когда пришел сигнал завершения от одного из запущенных процессов обработки
    void process_finished();

    //при закрытии окна. сначала нужно сохранить настройки в файл.
    void prepare_for_exit();

};

#endif // CONTROLLER_H
