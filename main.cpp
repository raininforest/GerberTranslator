#include "main_window.h"
#include "updater.h"
#include <QApplication>

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

    // запуск главного приложения
    QApplication a(argc, argv);
    Main_window w;
    w.setGeometry(200,200,820,480);
    w.setFixedSize(820,480);
    w.show();

    return a.exec();
}
