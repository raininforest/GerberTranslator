//#include "main_window.h"
#include "controller.h"
#include "updater.h"
#include <QApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QObject>
#include <QVariant>

int main(int argc, char *argv[])
{    
    // запуск апдейта
    int was_update_succesfull = 0;
    was_update_succesfull = update();
    switch (was_update_succesfull) {
    case -1:{
        qDebug()<< "Failed to update the current version of the program! The old version will be launched...";
    } break;
    case 1:{
        qDebug()<< "Current version will be updated!";
    } return 1;
    case 2:{
        qDebug()<< "No update required!";
    } break;
    }

    QApplication a(argc, argv);

    QQmlApplicationEngine engine;
    controller c;
    engine.rootContext()->setContextProperty("controller",&c);
    const QUrl url(QStringLiteral("qrc:/main.qml"));
    QObject::connect(&engine, &QQmlApplicationEngine::objectCreated,
                     &a, [url](QObject *obj, const QUrl &objUrl) {
        if (!obj && url == objUrl)
            QCoreApplication::exit(-1);
    }, Qt::QueuedConnection);
    engine.load(url);

    QObject *mainwindow = engine.rootObjects().at(0);

    QObject::connect(&c,SIGNAL(message(QVariant)),mainwindow,SLOT(show_message(QVariant)));
    QObject::connect(&c,SIGNAL(ready_to_init()),mainwindow,SLOT(set_ini_parameters()));
    QObject::connect(&c,SIGNAL(increase_progress(QVariant)),mainwindow,SLOT(progress_bar(QVariant)));
    QObject::connect(&c,SIGNAL(processing_done(QVariant)),mainwindow,SLOT(done_slot(QVariant)));
    QObject::connect(&c,SIGNAL(ready_to_exit()),mainwindow,SLOT(exit_slot()));
    QObject::connect(mainwindow,SIGNAL(run_processing()),&c,SLOT(run_all()));
    QObject::connect(mainwindow,SIGNAL(close_app()),&c,SLOT(prepare_for_exit()));

    c.load_ini_file();

    return a.exec();
}
