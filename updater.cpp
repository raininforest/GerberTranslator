#include "updater.h"

int update(){

    int local_version=0;
    int actual_version=0;
    QString platform="windows";
    QString bit="64";
    QString extension="";

    QString local_ver_file_name;
    QString local_version_text="";
    QString local_app_path;
    QString updater_path;
    QString actual_path;
    QString actual_ver_file_name;
    QString actual_version_text="";
    QString actual_app_path;

    local_ver_file_name = "gt_version";
    QFile loc_version_file(local_ver_file_name);
    if(!loc_version_file.open(QIODevice::ReadOnly | QIODevice::Text)){
        qDebug()<<"Невозможно открыть локальный файл версии!";
        return -1;
        //сообщение об ошибке
    }
    while (!loc_version_file.atEnd()){
        QByteArray str = loc_version_file.readLine().trimmed();
        // читаю номер версии
        if (str.contains("version")){
            local_version_text = str.right(str.size()-(str.indexOf('=')+1));
            bool ok;
            QString lvt_without_point = local_version_text;
            lvt_without_point.remove('.');
            local_version = lvt_without_point.toInt(&ok);
            if ((local_version == 0)||(!ok)) {
                qDebug()<<"Не удалось прочитать локальный номер версии из файла версии!";
                loc_version_file.close();
                return -1;
            }
        }
        // читаю ОС
        if (str.contains("platform")){
            platform = str.right(str.size()-(str.indexOf('=')+1));
            if (platform=="windows"){
                extension = ".exe";
            }
            else if (platform=="linux") {
                extension.clear();
            }
            else {
                qDebug()<<"Не удалось прочитать тип операционной системы из файла версии!";
                loc_version_file.close();
                return -1;
            }
        }
        // читаю разрядность ОС
        if (str.contains("bit")){
            bit = str.right(str.size()-(str.indexOf('=')+1));
            if (bit.isEmpty()){
                qDebug()<<"Не удалось прочитать разрядность операционной системы из файла версии!";
                loc_version_file.close();
                return -1;
            }
        }
    }
    loc_version_file.close();

    local_app_path = "gerber_translator" + extension;
    updater_path = "gt_u" + extension;

    // Читаем путь к папке-источнику из файла path
    QFile path_file("path");
    if(!path_file.open(QIODevice::ReadOnly | QIODevice::Text)){
        qDebug()<<"Не удалось открыть файл path!";
        return -1;
        // сообщение об ошибке
    }
    while (!path_file.atEnd()){
        actual_path = path_file.readLine().trimmed();
    }
    if (actual_path.isEmpty()){
        path_file.close();
        qDebug()<<"Не удалось определить путь к актуальной версии!";
        return -1;
    }

    // формирование путей к актуальному файлу версии в источнике и актуальному исполняемому файлу
    QString platform_folder_name;
    QString bit_folder_name;
    platform_folder_name = "/" + platform;
    bit_folder_name = "/gerber_translator_" + bit;
    actual_path.append(platform_folder_name);
    actual_path.append(bit_folder_name);
    actual_ver_file_name = actual_path + "/gt_version";
    actual_app_path = actual_path + "/gerber_translator" + extension;
    path_file.close();

    // читаем из файлов и сравниваем версии в локальной папке и в источнике
    QFile act_version_file(actual_ver_file_name);

    if(!act_version_file.open(QIODevice::ReadOnly | QIODevice::Text)){
        qDebug()<<"Не удалось открыть актуальный файл версии!";
        return -1;
        //сообщение об ошибке
    }
    while (!act_version_file.atEnd()){
        QByteArray str = act_version_file.readLine().trimmed();
        // читаю номер версии
        if (str.contains("version")){
            actual_version_text = str.right(str.size()-(str.indexOf('=')+1));
            bool ok;
            QString avt_without_point = actual_version_text;
            avt_without_point.remove('.');
            actual_version = avt_without_point.toInt(&ok);
            if ((actual_version == 0)||(!ok)) {
                qDebug()<<"Не удалось прочитать актуальный номер версии!";
                act_version_file.close();
                return -1;
            }
        }
    }

    act_version_file.close();

    // сравнение версий...
    if (actual_version>local_version){
        //argv[1] - путь к локальному файлу версии
        //argv[2] - строка с номером актуальной версии
        //argv[3] - путь к локальной версии программы
        //argv[4] - путь к актуальной версии программы
        QStringList arguments;
        arguments.append(local_ver_file_name);
        arguments.append(actual_version_text);
        arguments.append(local_app_path);
        arguments.append(actual_app_path);
        // запуск апдейтера
        QProcess updater(nullptr);
        updater.startDetached(updater_path, arguments);
        // выход из главной программы с положительным кодом
        return 1;
    }
    return -1;
}
