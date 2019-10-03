#include "main_window.h"
#include "controller.h"
#include "updater.h"
#include <QApplication>
#include <QObject>

int main(int argc, char *argv[])
{    
    // запуск апдейта
    int was_update_succesfull = 0;
    was_update_succesfull = update();
    switch (was_update_succesfull) {
    case -1:{
        qDebug()<< "Не удалось обновить текущую версию программы! Будет запущена старая версия...";
    } break;
    case 1:{
        qDebug()<< "Текущая версия программы будет обновлена!";
    } return 1;
    case 2:{
        qDebug()<< "Обновление не требуется!";
    } break;
    }

    QApplication a(argc, argv);
    controller c;       // создание контроллера
    Main_window w;      // создание и запуск главного приложения
    w.set_controller(&c);
    w.setWindowIcon(QIcon(QDir::currentPath() + "/icon.ico"));
    w.setGeometry(200,200,820,480);
    w.setFixedSize(820,480);

    QObject::connect(&c,SIGNAL(message(QString)),&w,SLOT(show_message(QString)));
    QObject::connect(&c,SIGNAL(ready_to_init()),&w,SLOT(set_ini_parameters()));
    QObject::connect(&c,SIGNAL(increase_progress(int)),&w,SLOT(progress_bar(int)));
    QObject::connect(&c,SIGNAL(processing_done(QString)),&w,SLOT(done_slot(QString)));
    QObject::connect(&c, SIGNAL(file_done(int)), &w, SLOT(change_item_color(int)));
    QObject::connect(&w,SIGNAL(run_processing()),&c,SLOT(run_all()));
    QObject::connect(&w,SIGNAL(close_app()),&c,SLOT(prepare_for_exit()));

    w.show();
    c.load_ini_file();

    return a.exec();
}
