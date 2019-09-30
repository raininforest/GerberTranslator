import QtQuick 2.12
import QtQuick.Window 2.12
import Qt.labs.platform 1.1
import QtQuick.Layouts 1.3
import QtQuick.Controls 2.12
import QtQuick.VirtualKeyboard 2.4
import "controls"
//import "componentCreation.js" as CreationScript


Window {
    property int general_margin: 7
    property int main_radius: 2
    property string back_color: "white"
    property string main_color: "#4682B4"

    signal run_signal(string list_of_files, string format, string dpi, string savepath)

    id: window
    visible: true
    minimumWidth: 850
    height: 480
    title: qsTr("Gerber-translator")
    color: back_color
    opacity: 1
    flags: Qt.FramelessWindowHint

    OpacityAnimator {
        id: text_anim
        target: text_area;
        from: 0.3;
        to: 1;
        duration: 2000
        running: false
    }
    OpacityAnimator {
        id: bottom_controls_anim
        target: bot_row_layout
        from: 0.3;
        to: 1;
        duration: 2000
        running: false        
    }

    MouseArea{
        anchors.fill: parent
        property variant clickPos: "0,0"
        onPressed: {
            clickPos  = Qt.point(mouse.x,mouse.y)
        }
        onPositionChanged: {
            var delta = Qt.point(mouse.x-clickPos.x, mouse.y-clickPos.y)
            window.x += delta.x;
            window.y += delta.y;
        }
    }

    GtFileDialog{
        id: open_dialog
        onAccepted: {
            text_area.clear()
            for (var i=0;i<open_dialog.files.length; i++)
                text_area.append(open_dialog.files[i].toString())
                text_anim.running=true
                bottom_controls_anim.running=true
                bot_row_layout.enabled=true
        }
    }
    GtFolderDialog{
        id: save_dialog
        onAccepted: {
            save_path.text=save_dialog.save_folder_name
        }
    }

    ColumnLayout{
        id: column_layout
        width: parent.width
        height: parent.height
        RowLayout {
            id: top_row_layout
            height: 50
            width: parent.width
            Layout.fillWidth: true
            Layout.margins: general_margin
            spacing: general_margin

            GtButton{
                id: open_button
                text: qsTr("Open")
                down_color: main_color
                up_color: back_color
                border_color: main_color
                onClicked: {
                    open_dialog.mode=FileDialog.OpenFiles
                    open_dialog.dia_title="Please choose gerbers"
                    open_dialog.open()
                }
            }

            ProgressBar {
                id: control
                value: 0
                opacity: 0.3
                Layout.fillWidth: true
                indeterminate: true

                background: Rectangle {
                    implicitWidth: 200
                    implicitHeight: 50
                    color: "#e6e6e6"
                    radius: main_radius
                }

                contentItem: Item {
                    implicitWidth: 200
                    implicitHeight: 30

                    Rectangle {
                        width: control.visualPosition * parent.width
                        height: parent.height
                        radius: main_radius
                        color: main_color
                    }
                }
            }
            GtButton{
                id: exit_button
                text: "x"
                w: 50
                down_color: main_color
                up_color: back_color
                border_color: main_color

                onClicked: window.close()                
            }
        }
        ScrollView{
            Layout.fillHeight: true
            Layout.fillWidth: true
            Layout.leftMargin: general_margin
            Layout.rightMargin: general_margin
            TextArea {
                id: text_area
                placeholderText: "Press 'Open' and choose gerber-files to process..."
                readOnly: false
                cursorVisible: false
                selectByMouse: true
                color: main_color
                selectionColor: main_color
                selectedTextColor: back_color
                focus: true
                background: Rectangle {
                    anchors.fill: parent
                    opacity: 0.3
                    color: "#e6e6e6"
                }
            }
        }

        RowLayout {
            id: bot_row_layout
            height: 50
            width: parent.width
            Layout.margins: general_margin
            Layout.fillWidth: true
            spacing: general_margin
            enabled: false

            GtButton{
                id: save_button
                text: qsTr("Save path")
                down_color: main_color
                up_color: back_color
                border_color: main_color
                onClicked: {
                    save_dialog.dia_title="Please choose folder for saving images"
                    save_dialog.open()
                }
            }
            TextInput{
                id: save_path
                text: "d:\\"
                height: 50
                cursorVisible: false
                color: main_color
                selectionColor: main_color
                Layout.fillWidth: true
                Layout.maximumHeight: 50
                selectByMouse: true
                wrapMode: TextEdit.Wrap
                clip: true
                opacity: enabled ? 1 : 0.3
            }
            GtComboBox{
                id: format_combo
                model: ["png", "bmp"]
                down_color: main_color
                up_color: back_color
                border_color: main_color                
            }
            GtComboBox{
                id: dpi_combo
                static_text: "dpi"
                model: ["150", "300", "600", "900", "1200"]
                down_color: main_color
                up_color: back_color
                border_color: main_color

            }
            GtButton{
                id: go_button
                text: qsTr("GO!")
                w: 150
                down_color: back_color
                up_color: main_color
                border_color: main_color
                onClicked: window.run_signal(text_area.text, format_combo.currentText, dpi_combo.currentText, save_path.text)
            }
        }
    }
}
