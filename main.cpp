#include "main_window.h"
#include <QApplication>

int update();

int main(int argc, char *argv[])
{

    //Автоматический апдейт программы закомментирован.
    //Как работает апдейт?
    //1. Чтение пути с источником (новой версией). Формирование на его основе путей к файлу версий, исполняемому файлу.
    //2. Сравнение версий в источнике и в локальном расположении.
    //3. Если версии разные - апдейт (запуск апдейтера, который закрывает приложение и заменяет программу свежим файлом
    //(+обновляет номер версии в файле версий), затем сам заканчивает работу и запускает новую версию программы)
    //4. Иначе запуск программы в обычном режиме.

//    int return_code = 0;
//    return_code = update();
//    if (return_code>0){
//        return 1;
//    }
//    else {
//        qDebug()<< "Не удалось обновить текущую версию программы!";
//    }

    QApplication a(argc, argv);
    Main_window w;
    w.setGeometry(500,300,820,480);
    w.setFixedSize(820,480);
    w.show();

    return a.exec();
}

//int update(){
//    QString local_ver_file_name;
//    QString local_version_text="";
//    QString local_app_path;
//    QString updater_path;
//    QString actual_path;
//    QString actual_ver_file_name;
//    QString actual_version_text="";
//    QString actual_app_path;
//    int local_version=0;
//    int actual_version=0;

//    local_ver_file_name = "gt_version";
//    local_app_path = "gerber_translator.exe";
//    updater_path = "gt_u.exe";

//    // Читаем путь к папке-источнику из файла path
//    QFile path_file("path");
//    if(!path_file.open(QIODevice::ReadOnly | QIODevice::Text)){
//        return -1;
//        // сообщение об ошибке
//    }
//    while (!path_file.atEnd()){
//        actual_path = path_file.readLine();
//    }
//    // Если путь пустой - ошибка, выход из функции
//    if (actual_path.isEmpty()){
//        path_file.close();
//        return -1;
//    }

//    // формирование путей к актуальному файлу версии в источнике и актуальному исполняемому файлу
//    actual_ver_file_name = actual_path + "\\gt_version";
//    actual_app_path = actual_path + "\\gerber_translator.exe";
//    path_file.close();

//    // читаем из файлов и сравниваем версии в локальной папке и в источнике
//    QFile loc_version_file(local_ver_file_name);
//    QFile act_version_file(actual_ver_file_name);
//    if(!loc_version_file.open(QIODevice::ReadOnly | QIODevice::Text)){
//        qDebug()<<"Невозможно открыть локальный файл версии!";
//        return -1;
//        //сообщение об ошибке
//    }
//    while (!loc_version_file.atEnd()){
//        bool ok;
//        local_version_text = loc_version_file.readLine();
//        QString lvt_without_point = local_version_text;
//        lvt_without_point.remove('.');
//        local_version = lvt_without_point.toInt(&ok);
//        if (local_version == 0) {
//            qDebug()<<"Не удалось прочитать локальный номер версии!";
//            loc_version_file.close();
//            return -1;
//        }
//    }
//    if(!act_version_file.open(QIODevice::ReadOnly | QIODevice::Text)){
//        qDebug()<<"Невозможно открыть актуальный файл версии!";
//        return -1;
//        //сообщение об ошибке
//    }
//    while (!act_version_file.atEnd()){
//        bool ok;
//        actual_version_text = act_version_file.readLine();
//        QString avt_without_point = actual_version_text;
//        avt_without_point.remove('.');
//        actual_version = avt_without_point.toInt(&ok);
//        if (actual_version == 0) {
//            act_version_file.close();
//            qDebug()<<"Не удалось прочитать актуальный номер версии!";
//            return -1;
//        }
//    }

//    act_version_file.close();
//    loc_version_file.close();

//    // сравнение версий...
//    if (actual_version>local_version){
//        //argv[1] - путь к локальному файлу версии
//        //argv[2] - строка с номером актуальной версии
//        //argv[3] - путь к локальной версии программы
//        //argv[4] - путь к актуальной версии программы
//        QStringList arguments;
//        arguments.append(local_ver_file_name);
//        arguments.append(actual_version_text);
//        arguments.append(local_app_path);
//        arguments.append(actual_app_path);
//        // запуск апдейтера
//        QProcess updater(nullptr);
//        updater.startDetached(updater_path, arguments);
//        // выход из главной программы с положительным кодом
//        return 1;
//    }
//    return -1;
//}
