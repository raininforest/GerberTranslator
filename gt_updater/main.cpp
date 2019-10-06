#include <QCoreApplication>
#include <QFile>
#include <QDebug>
#include <QProcess>
#include <iostream>

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    if (argc<5){
        std::cout << "Not enough command line options to run updates!\n";
        return -1;
    }

    std::cout << "Updating...\n";

    QString platform;
    QString bit;

    QString local_ver_file_name = argv[1];
    QString actual_version_text = argv[2];
    QString local_app_path = argv[3];
    QString actual_app_path = argv[4];

    if (local_ver_file_name.isEmpty()) {
        std::cout << "Error! Empty local version file path.\n";
        return -1;
    }
    if (local_app_path.isEmpty()) {
        std::cout << "Error! Empty local application path.\n";
        return -1;
    }
    if (actual_app_path.isEmpty()) {
        std::cout << "Error! Empty actual application path.\n";
        return -1;
    }

    QFile loc_version_file(local_ver_file_name,nullptr);
    QFile local_app(local_app_path,nullptr);
    QFile actual_app(actual_app_path,nullptr);

    if (actual_app.exists()&&local_app.exists()){
        local_app.rename("old_version_app.exe");
        std::cout << "Rename old version...\n";
        if (!(actual_app.copy(local_app_path))){
            std::cout << "Can not copy new version of application!\n";
            local_app.rename(local_app_path);
            return -1;
        }
        std::cout << "Copy was successful!\n";
        if (local_app.remove()){
            std::cout << "Old version was removed!\n";
        }
        else {
            std::cout << "Can not delete old version!\n";
        }

    }
    else {
        std::cout << "Error! Either the local or current version of the application was not found.\n";
        return -1;
    }

    if(!loc_version_file.open(QIODevice::ReadOnly | QIODevice::Text)){
        std::cout << "Can not read local version file!\n";
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
        std::cout << "Can not write local version file!\n";
        return -1;
    }
    loc_version_file.write("version="+actual_version_text.toUtf8()+'\n');
    loc_version_file.write("platform="+platform.toUtf8()+'\n');
    loc_version_file.write("bit="+bit.toUtf8()+'\n');
    loc_version_file.close();
    std::cout << "Update completed successfully!\n";
    std::cout << "Launching new version...\n";
    QProcess main_app;
    main_app.startDetached(local_app_path);
    return 1;

}
