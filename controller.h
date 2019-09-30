#ifndef CONTROLLER_H
#define CONTROLLER_H

#include "processor.h"
#include <QDebug>
#include <QFuture>
#include <QtConcurrent/QtConcurrent>
#include <QProcess>

struct ini_params_str{
    Q_GADGET

    Q_PROPERTY(QString open_path_ini MEMBER m_open_path_ini)
    Q_PROPERTY(QString save_path_ini MEMBER m_save_path_ini)
    Q_PROPERTY(QString image_format_ini MEMBER m_image_format_ini)
    Q_PROPERTY(QString dpi_ini MEMBER m_dpi_ini)
    Q_PROPERTY(QString quick_translation_ini MEMBER m_quick_translation_ini)
    Q_PROPERTY(QString open_folder_after_processing_ini MEMBER m_open_folder_after_processing_ini)
    Q_PROPERTY(QString image_size_ini MEMBER m_image_size_ini)
    Q_PROPERTY(QString opacity_mode_ini MEMBER m_opacity_mode_ini)
    Q_PROPERTY(QString opacity_value_ini MEMBER m_opacity_value_ini)
    Q_PROPERTY(QString default_image_width_ini MEMBER m_default_image_width_ini)
    Q_PROPERTY(QString default_image_height_ini MEMBER m_default_image_height_ini)
    Q_PROPERTY(QString default_dx_ini MEMBER m_default_dx_ini)
    Q_PROPERTY(QString default_dy_ini MEMBER m_default_dy_ini)
    Q_PROPERTY(QString frame_thickness_ini MEMBER m_frame_thickness_ini)
public:
    QString m_open_path_ini;
    QString m_save_path_ini;
    QString m_image_format_ini;
    QString m_dpi_ini;
    QString m_quick_translation_ini;
    QString m_open_folder_after_processing_ini;
    QString m_image_size_ini;
    QString m_opacity_mode_ini;
    QString m_opacity_value_ini;
    QString m_default_image_width_ini;
    QString m_default_image_height_ini;
    QString m_default_dx_ini;
    QString m_default_dy_ini;
    QString m_frame_thickness_ini;
};

class controller: public QObject
{
    Q_OBJECT

public:
    explicit controller();

    ini_params_str ini_params;      //  глобальная структура со всеми параметрами из ини файла
    QStringList list_of_gerbers;    //  глобальный список обрабатываемых файлов

    void load_ini_file();

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

    //генерируется после чтения инифайла
    void ready_to_init();

    //во время обработки приращение прогресса
    void increase_progress(int);

    //при необходимости показать диалог с сообщением
    void message(QString);

    //признак конца текущей обработки всех файлов
    void processing_done(QString msg_text);

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
