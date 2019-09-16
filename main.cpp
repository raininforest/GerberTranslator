#include "main_window.h"
#include "updater.h"
#include <QApplication>

int main(int argc, char *argv[])
{    
    // запуск апдейта
    int was_update_succesfull = 0;
    was_update_succesfull = update();
    if (was_update_succesfull>0){
        return 1;
    }
    else {
        qDebug()<< "Не удалось обновить текущую версию программы либо обновление не требуется.";
    }

    // запуск главного окна
    QApplication a(argc, argv);
    Main_window w;
    w.setGeometry(500,300,820,480);
    w.setFixedSize(820,480);
    w.show();

    return a.exec();
}
