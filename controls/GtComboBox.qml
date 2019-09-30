import QtQuick 2.12
import QtQuick.Controls 2.5

ComboBox{
    property string down_color: "#4682B4"
    property string up_color: "white"
    property string border_color: "#4682B4"
    property string static_text
    property int r: 0
    property int h: 50
    property int w: 100

    id: gt_combo
    opacity: enabled ? 1 : 0.3

    Label{
        id: dpi_label
        text: static_text
        color: gt_combo.down ? up_color : down_color
        x: gt_combo.width/2-5
        y: gt_combo.height/2-dpi_label.height/2
    }

    delegate: ItemDelegate {
        width: gt_combo.width
        contentItem: Text {
            text: modelData
            color: down_color
            font: gt_combo.font
            elide: Text.ElideRight
            verticalAlignment: Text.AlignVCenter
        }
        highlighted: gt_combo.highlightedIndex === index
    }
    indicator: Canvas {
        id: canvas
        x: gt_combo.width - width - gt_combo.rightPadding
        y: gt_combo.topPadding + (gt_combo.availableHeight - height) / 2
        width: 12
        height: 8
        contextType: "2d"

        Connections {
            target: gt_combo
            onPressedChanged: canvas.requestPaint()
        }

        onPaint: {
            context.reset();
            context.moveTo(0, 0);
            context.lineTo(width, 0);
            context.lineTo(width / 2, height);
            context.closePath();
            context.fillStyle = gt_combo.pressed ? up_color : down_color;
            context.fill();
        }
    }

    contentItem: Text {
        leftPadding: 10
        text: gt_combo.displayText

        color: gt_combo.down ? up_color : down_color
        font: gt_combo.font
        elide: Text.ElideMiddle
        verticalAlignment: Text.AlignVCenter
    }
    background: Rectangle {
        implicitWidth: 100
        implicitHeight: 50

        color: gt_combo.down ? down_color : up_color
        border.color: border_color
        radius: r
    }
}
