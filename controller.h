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
    Q_PROPERTY(QString open_path_ini
               READ open_path_ini
               WRITE set_open_path_ini
               NOTIFY open_path_ini_Changed)
    Q_PROPERTY(QString save_path_ini
               READ save_path_ini
               WRITE set_save_path_ini
               NOTIFY save_path_ini_Changed)
    Q_PROPERTY(QString image_format_ini
               READ image_format_ini
               WRITE set_image_format_ini
               NOTIFY image_format_ini_Changed)
    Q_PROPERTY(QString dpi_ini
               READ dpi_ini
               WRITE set_dpi_ini
               NOTIFY dpi_ini_Changed)
    Q_PROPERTY(QString quick_translation_ini
               READ quick_translation_ini
               WRITE set_quick_translation_ini
               NOTIFY quick_translation_ini_Changed)
    Q_PROPERTY(QString open_folder_after_processing_ini
               READ open_folder_after_processing_ini
               WRITE set_open_folder_after_processing_ini
               NOTIFY open_folder_after_processing_ini_Changed)
    Q_PROPERTY(QString image_size_ini
               READ image_size_ini
               WRITE set_image_size_ini
               NOTIFY image_size_ini_Changed)
    Q_PROPERTY(QString opacity_mode_ini
               READ opacity_mode_ini
               WRITE set_opacity_mode_ini
               NOTIFY opacity_mode_ini_Changed)
    Q_PROPERTY(QString opacity_value_ini
               READ opacity_value_ini
               WRITE set_opacity_value_ini
               NOTIFY opacity_value_ini_Changed)
    Q_PROPERTY(QString default_image_width_ini
               READ default_image_width_ini
               WRITE set_default_image_width_ini
               NOTIFY default_image_width_ini_Changed)
    Q_PROPERTY(QString default_image_height_ini
               READ default_image_height_ini
               WRITE set_default_image_height_ini
               NOTIFY default_image_height_ini_Changed)
    Q_PROPERTY(QString default_dx_ini
               READ default_dx_ini
               WRITE set_default_dx_ini
               NOTIFY default_dx_ini_Changed)
    Q_PROPERTY(QString default_dy_ini
               READ default_dy_ini
               WRITE set_default_dy_ini
               NOTIFY default_dy_ini_Changed)
    Q_PROPERTY(QString frame_thickness_ini
               READ frame_thickness_ini
               WRITE set_frame_thickness_ini
               NOTIFY frame_thickness_ini_Changed)
    Q_PROPERTY(QString gerbers_string
               READ gerbers_string
               WRITE set_gerbers_string
               NOTIFY gerbers_string_Changed)

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
    QString m_gerbers_string;


    QString open_path_ini() const {return m_open_path_ini;}
    QString save_path_ini() const {return m_save_path_ini;}
    QString image_format_ini() const {return m_image_format_ini;}
    QString dpi_ini() const {return m_dpi_ini;}
    QString quick_translation_ini() const {return m_quick_translation_ini;}
    QString open_folder_after_processing_ini() const {return m_open_folder_after_processing_ini;}
    QString image_size_ini() const {return m_image_size_ini;}
    QString opacity_mode_ini() const {return m_opacity_mode_ini;}
    QString opacity_value_ini() const {return m_opacity_value_ini;}
    QString default_image_width_ini() const {return m_default_image_width_ini;}
    QString default_image_height_ini() const {return m_default_image_height_ini;}
    QString default_dx_ini() const {return m_default_dx_ini;}
    QString default_dy_ini() const {return m_default_dy_ini;}
    QString frame_thickness_ini() const {return m_frame_thickness_ini;}
    QString gerbers_string() const {return m_gerbers_string;}


    void set_open_path_ini(const QString str){m_open_path_ini=str;}
    void set_save_path_ini(const QString str){m_save_path_ini=str;}
    void set_image_format_ini(const QString str){m_image_format_ini=str;}
    void set_dpi_ini(const QString str){m_dpi_ini=str;}
    void set_quick_translation_ini(const QString str){m_quick_translation_ini=str;}
    void set_open_folder_after_processing_ini(const QString str){m_open_folder_after_processing_ini=str;}
    void set_image_size_ini(const QString str){m_image_size_ini=str;}
    void set_opacity_mode_ini(const QString str){m_opacity_mode_ini=str;}
    void set_opacity_value_ini(const QString str){m_opacity_value_ini=str;}
    void set_default_image_width_ini(const QString str){m_default_image_width_ini=str;}
    void set_default_image_height_ini(const QString str){m_default_image_height_ini=str;}
    void set_default_dx_ini(const QString str){m_default_dx_ini=str;}
    void set_default_dy_ini(const QString str){m_default_dy_ini=str;}
    void set_frame_thickness_ini(const QString str){m_frame_thickness_ini=str;}
    void set_gerbers_string(const QString str){
        m_gerbers_string=str;
        emit gerbers_string_Changed();
    }

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
    void increase_progress(QVariant);

    //при необходимости показать диалог с сообщением
    void message(QVariant);

    //признак конца текущей обработки всех файлов
    void processing_done(QVariant);

    //данные сохранены, память освобождена - можно закрывать приложение
    void ready_to_exit();

private slots:

    //принятие команды старт обработки от окна
    void run_all();//передать структуру с параметрами

    //когда пришел сигнал завершения от одного из запущенных процессов обработки
    void process_finished();

    //при закрытии окна. сначала нужно сохранить настройки в файл.
    void prepare_for_exit();

    void set_list_of_gerbers_from_QML_string(){
        list_of_gerbers=m_gerbers_string.split('\n');
    }

};

#endif // CONTROLLER_H
