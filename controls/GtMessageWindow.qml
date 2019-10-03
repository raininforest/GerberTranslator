import QtQuick 2.2
import QtQuick.Window 2.12
import QtQuick.Controls 2.2
import QtQuick.Layouts 1.3

Window{
    property string msg_text

    id:messagewindow
    width: msg_column.width
    height: msg_column.height
    modality: Qt.ApplicationModal
    flags: Qt.FramelessWindowHint
    opacity: 0.95

    MouseArea{
        anchors.fill: parent
        property variant clickPos: "0,0"
        onPressed: {
            clickPos  = Qt.point(mouse.x,mouse.y)
        }
        onPositionChanged: {
            var delta = Qt.point(mouse.x-clickPos.x, mouse.y-clickPos.y)
            messagewindow.x += delta.x;
            messagewindow.y += delta.y;
        }
    }
    Rectangle{
        border.color: main_color
        border.width: 1
        anchors.fill: parent
    }
    ColumnLayout {
        id: msg_column
        spacing: 0

        Text {
            id: msg_string
            text: msg_text
            color: main_color
            horizontalAlignment: Text.AlignHCenter
            Layout.alignment: Qt.AlignTop | Qt.AlignHCenter
            Layout.margins: 15
            //width: parent.width-20
            wrapMode: TextEdit.Wrap
            clip: true
        }
        GtButton{
            id: close_settings_button
            text: "OK"
            Layout.alignment: Qt.AlignBottom | Qt.AlignHCenter
            Layout.margins: 15
            down_color: main_color
            up_color: back_color
            border_color: main_color
            onClicked: close()
        }
    }
}
