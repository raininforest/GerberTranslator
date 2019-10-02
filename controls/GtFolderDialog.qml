import QtQuick 2.2
import Qt.labs.platform 1.1


FolderDialog {
    property string dia_title

    id: saveDialog
    title: dia_title
    options: FolderDialog.ShowDirsOnly    

}
