import QtQuick 2.2
import QtQuick.Window 2.12
import QtQuick.Controls 2.2
import QtQuick.Layouts 1.3

Window{
    property bool quick_translation_chckbox_checked
    property bool open_folder_chckbox_checked
    property bool board_size_chckbox_checked
    property bool transparent_img_chckbox_checked

    id:modalwindow
    width: 400
    height: settings_column.height
    modality: Qt.ApplicationModal
    flags: Qt.FramelessWindowHint

    MouseArea{
        anchors.fill: parent
        property variant clickPos: "0,0"
        onPressed: {
            clickPos  = Qt.point(mouse.x,mouse.y)
        }
        onPositionChanged: {
            var delta = Qt.point(mouse.x-clickPos.x, mouse.y-clickPos.y)
            modalwindow.x += delta.x;
            modalwindow.y += delta.y;
        }
    }
    Rectangle{
        border.color: main_color
        border.width: 1
        anchors.fill: parent
    }
    GtButton{
        id: close_settings_button
        text: "x"
        w: 30
        h: 30
        anchors.right: parent.right
        anchors.rightMargin: 5
        anchors.top: parent.top
        anchors.topMargin: 5
        down_color: main_color
        up_color: back_color
        border_color: main_color

        onClicked: settings_window.close()
    }

    ColumnLayout {
        id: settings_column
        spacing: 0
        GtCheckBox {
            id: quick_translation_chckbox
            checked: quick_translation_chckbox_checked
            text: qsTr("Quick translation on open")
            checkbox_color: main_color
            onCheckedChanged: quick_translation_chckbox_checked=checked
        }
        GtCheckBox {
            id: open_folder_chckbox
            checked: open_folder_chckbox_checked
            text: qsTr("Open folder after processing")
            checkbox_color: main_color
            onCheckedChanged: open_folder_chckbox_checked=checked
        }
        GtCheckBox {
            id: board_size_chckbox
            checked: board_size_chckbox_checked
            text: qsTr("Board size from .ini")
            checkbox_color: main_color
            onCheckedChanged: board_size_chckbox_checked=checked
        }
        GtCheckBox {
            id: transparent_img_chckbox
            checked: transparent_img_chckbox_checked
            text: qsTr("Semitransparent image")
            checkbox_color: main_color
            onCheckedChanged: transparent_img_chckbox_checked=checked
        }
    }
}
