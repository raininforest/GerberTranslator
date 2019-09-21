#include <QCoreApplication>
#include <QFile>
#include <QDebug>
#include <QProcess>
#include <iostream>

//аргументы:
//argv[1] - путь к локальному файлу версии
//argv[2] - строка с номером актуальной версии
//argv[3] - путь к локальной версии программы
//argv[4] - путь к актуальной версии программы

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    if (argc<5){
        std::cout << "Недостаточно параметров командной строки для запуска обновления!\n";
        return -1;
    }

    std::cout << "Обновление версии...\n";

    QString platform;
    QString bit;

    QString local_ver_file_name = argv[1];
    QString actual_version_text = argv[2];          //эту строку запишем в локальный файл с версией в случае успешного обновления
    QString local_app_path = argv[3];
    QString actual_app_path = argv[4];

    if (local_ver_file_name.isEmpty()) {
        std::cout << "Ошибка! Пустой путь локального файла версии.\n";
        return -1;
    }
    if (local_app_path.isEmpty()) {
        std::cout << "Ошибка! Пустой путь локальной версии приложения.\n";
        return -1;
    }
    if (actual_app_path.isEmpty()) {
        std::cout << "Ошибка! Пустой путь актуальной версии приложения.\n";
        return -1;
    }

    QFile loc_version_file(local_ver_file_name,nullptr);
    QFile local_app(local_app_path,nullptr);
    QFile actual_app(actual_app_path,nullptr);

    if (actual_app.exists()&&local_app.exists()){
        local_app.rename("old_version_app.exe");
        std::cout << "Переименование старой версии...\n";
        if (!(actual_app.copy(local_app_path))){
            std::cout << "Ошибка копирования новой версии!\n";
            local_app.rename(local_app_path);
            return -1;
        }
        std::cout << "Копирование прошло успешно!\n";
        if (local_app.remove()){
            std::cout << "Старая версия программы удалена!\n";
        }
        else {
            std::cout << "Невозможно удалить старую версию программы!\n";
        }

    }
    else {
        std::cout << "Ошибка! Не найдена либо локальная либо актуальная версия приложения.\n";
        return -1;
    }

    if(!loc_version_file.open(QIODevice::ReadOnly | QIODevice::Text)){
        std::cout << "Ошибка открытия локального файла версии для чтения!\n";
        return -1;
    }
    while (!loc_version_file.atEnd()){
        QByteArray str = loc_version_file.readLine().trimmed();
        // читаю ОС
        if (str.contains("platform")){
            platform = str.right(str.size()-(str.indexOf('=')+1));
        }
        // читаю разрядность ОС
        if (str.contains("bit")){
            bit = str.right(str.size()-(str.indexOf('=')+1));
        }
    }
    loc_version_file.close();
    if(!loc_version_file.open(QIODevice::WriteOnly | QIODevice::Text)){
        std::cout << "Ошибка открытия локального файла версии для записи!\n";
        return -1;
    }
    loc_version_file.write("version="+actual_version_text.toUtf8()+'\n');
    loc_version_file.write("platform="+platform.toUtf8()+'\n');
    loc_version_file.write("bit="+bit.toUtf8()+'\n');
    loc_version_file.close();
    std::cout << "Обновление успешно завершено.\n";
    QProcess main_app;
    main_app.startDetached(local_app_path);
    return 1;
//    return a.exec();
}
