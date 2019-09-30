import QtQuick 2.2
import Qt.labs.platform 1.1


FileDialog {
    property int mode
    property string dia_title
    property var myfiles


    id: openDialog
    title: dia_title
    fileMode: mode
    onAccepted: {
        console.log("You chose: " + openDialog.files)
        myfiles = openDialog.files
    }
    onRejected: {
        console.log("Canceled")
    }
}
