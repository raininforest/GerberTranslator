import QtQuick 2.2
import Qt.labs.platform 1.1


FileDialog {
    property int mode
    property string dia_title

    id: openDialog
    title: dia_title
    fileMode: mode    
}
