#include "updater.h"

int update(){

    int local_version=0;
    int actual_version=0;
    QString platform="windows";
    QString bit="64";
    QString extension="";

    QString local_ver_file_name;    //(not full path)
    QString local_version_text="";  //
    QString local_app_name;         //local application path (not full path)
    QFileInfo local_app;            //
    QString updater_name;           //updater path (not full path)
    QFileInfo updater;              //
    QString actual_path;            //path to actual versions
    QString actual_ver_file_name;   //absolute path for actual version file
    QString actual_version_text=""; //
    QString actual_app_path;        //absolute path for actual version of application

    qDebug()<<"Checking updates...";
    local_ver_file_name = "gt_version";
    QFile loc_version_file(local_ver_file_name);
    if(!loc_version_file.open(QIODevice::ReadOnly | QIODevice::Text)){
        qDebug()<<"Can not open local version file!";
        return -1;        
    }
    while (!loc_version_file.atEnd()){
        QByteArray str = loc_version_file.readLine().trimmed();
        // read local version
        if (str.contains("version")){
            local_version_text = str.right(str.size()-(str.indexOf('=')+1));
            bool ok;
            QString lvt_without_point = local_version_text;
            lvt_without_point.remove('.');
            local_version = lvt_without_point.toInt(&ok);
            if ((local_version == 0)||(!ok)) {
                qDebug()<<"Can not read [version] from local version file!";
                loc_version_file.close();
                return -1;
            }
        }
        // read platform
        if (str.contains("platform")){
            platform = str.right(str.size()-(str.indexOf('=')+1));
            if (platform=="windows"){
                extension = ".exe";
            }
            else if (platform=="linux") {
                extension.clear();
            }
            else {
                qDebug()<<"Can not read [platform] from local version file!";
                loc_version_file.close();
                return -1;
            }
        }
        // read bit
        if (str.contains("bit")){
            bit = str.right(str.size()-(str.indexOf('=')+1));
            if (bit.isEmpty()){
                qDebug()<<"Can not read [bit] from local version file!";
                loc_version_file.close();
                return -1;
            }
        }
    }
    loc_version_file.close();

    local_app_name = "gerber_translator" + extension;
    updater_name = "gt_u" + extension;
    updater.setFile(updater_name);
    if (!updater.exists()){
        qDebug()<<"Updater gt_u not found!";
        return -1;
    }

    // reading path with acual versions
    QFile path_file("path");
    if(!path_file.open(QIODevice::ReadOnly | QIODevice::Text)){
        qDebug()<<"Can not open path file!";
        return -1;
    }
    while (!path_file.atEnd()){
        actual_path = path_file.readLine().trimmed();
    }
    if (actual_path.isEmpty()){
        path_file.close();
        qDebug()<<"Failed to determine the path to the actual version!";
        return -1;
    }

    // creating actual paths
    QString platform_folder_name;
    QString bit_folder_name;
    platform_folder_name = "/" + platform;
    bit_folder_name = "/gerber_translator_" + bit;
    actual_path.append(platform_folder_name);
    actual_path.append(bit_folder_name);
    actual_ver_file_name = actual_path + "/gt_version";
    actual_app_path = actual_path + "/gerber_translator" + extension;
    path_file.close();

    QFile act_version_file(actual_ver_file_name);

    if(!act_version_file.open(QIODevice::ReadOnly | QIODevice::Text)){
        qDebug()<<"Can not open actual version file!";
        return -1;
    }
    while (!act_version_file.atEnd()){
        QByteArray str = act_version_file.readLine().trimmed();
        // read version
        if (str.contains("version")){
            actual_version_text = str.right(str.size()-(str.indexOf('=')+1));
            bool ok;
            QString avt_without_point = actual_version_text;
            avt_without_point.remove('.');
            actual_version = avt_without_point.toInt(&ok);
            if ((actual_version == 0)||(!ok)) {
                qDebug()<<"Can not read acual version from actual version file!";
                act_version_file.close();
                return -1;
            }
        }
    }
    act_version_file.close();

    // compare versions
    if (actual_version>local_version){
        //argv[1] - local version path
        //argv[2] - actual version string
        //argv[3] - local application path
        //argv[4] - actual application path
        QStringList arguments;
        arguments.append(local_ver_file_name);  //
        arguments.append(actual_version_text);  //
        arguments.append(local_app_name);       //
        arguments.append(actual_app_path);      //absolute path
        // run updater
        QProcess updater_process(nullptr);
        updater_process.startDetached(updater.absoluteFilePath(), arguments);
        // exit
        return 1;
    }
    else if (actual_version==local_version){
        return 2; //app is up to date
    }
    qDebug()<<"Error! Local version > Actual version!";
    return -1;
}
