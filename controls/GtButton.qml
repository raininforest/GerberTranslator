import QtQuick 2.2
import QtQuick.Controls 2.2

Button{
    property string down_color: "#4682B4"
    property string up_color: "white"
    property string border_color: "#4682B4"
    property int r: 2
    property int h: 50
    property int w: 100

    id: gt_button

    contentItem: Text {
        text: gt_button.text
        font: gt_button.font
        opacity: enabled ? 1.0 : 0.3
        color: gt_button.down ? up_color : down_color
        horizontalAlignment: Text.AlignHCenter
        verticalAlignment: Text.AlignVCenter
        elide: Text.ElideRight
    }

    background: Rectangle {
        implicitWidth: w
        implicitHeight: h
        opacity: enabled ? 1 : 0.3
        color: gt_button.down ? down_color : up_color
        border.color: border_color
        radius: r
    }
}
