import QtQuick 2.2
import Qt.labs.platform 1.1


FolderDialog {
    property string dia_title
    property string save_folder_name

    id: saveDialog
    title: dia_title
    options: FolderDialog.ShowDirsOnly
    folder: StandardPaths.standardLocations(StandardPaths.PicturesLocation)[0]
    onAccepted: {
        save_folder_name = saveDialog.folder.toString()
    }
    onRejected: {
        console.log("Canceled")
    }
}
