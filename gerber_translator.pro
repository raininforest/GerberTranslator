#-------------------------------------------------
#
# Project created by QtCreator 2019-02-24T18:11:59
#
#-------------------------------------------------

QT += qml
QT += widgets


greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = gerber_translator
#TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

CONFIG += c++11

SOURCES += \
    controller.cpp \
        main.cpp \
    processor.cpp \
    aperture.cpp \
    am_template.cpp \
    updater.cpp

RESOURCES += qml.qrc

# Additional import path used to resolve QML modules in Qt Creator's code model
QML_IMPORT_PATH = controls/

# Additional import path used to resolve QML modules just for Qt Quick Designer
QML_DESIGNER_IMPORT_PATH =

HEADERS += \
    controller.h \
    processor.h \
    aperture.h \
    am_template.h \
    updater.h

FORMS +=

#RC_ICONS = config/icon.ico

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
