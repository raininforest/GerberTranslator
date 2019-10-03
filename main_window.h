#ifndef MAIN_WINDOW_H
#define MAIN_WINDOW_H

#include <QMainWindow>
#include <QListWidget>
#include <QMessageBox>
#include <QDebug>
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
#include "controller.h"

namespace Ui {
class Main_window;
}

class Main_window : public QMainWindow
{
    Q_OBJECT

public:
    explicit Main_window(QWidget *parent = nullptr);
    ~Main_window();

    void set_controller(controller* ptr){
        if (ptr!=nullptr) {
            cptr=ptr;
        }
    }

protected:

    void dragEnterEvent(QDragEnterEvent* event);

    void dropEvent(QDropEvent *event);

    void closeEvent(QCloseEvent *event);

public: signals:

    void run_processing();

    void close_app();

private slots:

    void set_ini_parameters();

    void show_message(QString msg_text);

    void progress_bar(int val);

    void change_item_color(int widget_index);

    void done_slot(QString msg_text);

    void open_slot();

    void what_save_path();

    void show_about();

    void dpi_changed(const QString &arg1);

    void on_comboBox_dpi_currentTextChanged(const QString &arg1);

    void on_comboBox_format_currentTextChanged(const QString &arg1);

    void quick_translation_changed(bool checked);

    void open_folder_after_changed(bool checked);

    void image_size_changed(bool checked);

    void opacity_mode_changed(bool checked);

    void on_start_button_clicked();

private:

    Ui::Main_window *ui;
    controller* cptr=nullptr;  // указатель на контроллер приложения
    QMessageBox msgBox;     // Окно для сообщений
};

#endif // MAIN_WINDOW_H
