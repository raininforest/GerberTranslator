import QtQuick 2.12
import QtQuick.Window 2.12
import Qt.labs.platform 1.1
import QtQuick.Layouts 1.3
import QtQuick.Controls 2.12
import QtQuick.VirtualKeyboard 2.4
import "controls"

Window {

    property int general_margin: 7
    property int main_radius: 2
    property string back_color: "white"
    property string main_color: "#4682B4"
    property int count_of_files: 0

    signal run_processing()
    signal close_app()

    function show_message(msg){
        msg_dialog.text=msg
        msg_dialog.open()
    }
    function set_ini_parameters(){
        open_dialog.folder="file://"+controller.open_path_ini
        save_dialog.folder="file://"+controller.save_path_ini
        save_path.text=controller.save_path_ini
        dpi_combo.currentIndex=dpi_combo.find(controller.dpi_ini)
        format_combo.currentIndex=format_combo.find(controller.image_format_ini)
    }
    function progress_bar(val){
        if (count_of_files!=0){
            progress.value=progress.value+(1/count_of_files)
        }
    }
    function done_slot(msg){
        open_button.enabled=true
        exit_button.enabled=true
        bot_row_layout.enabled=true
        progress.value=1
        msg_dialog.text=msg
        msg_dialog.open()
    }
    function exit_slot(){
        window.close()
    }


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

    MessageDialog{
        id: msg_dialog
        title: "Gerber-translator"
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
            count_of_files=open_dialog.files.length
            for (var i=0;i<count_of_files; i++){
                text_area.append(open_dialog.files[i].toString().replace("file://",""))
            }
            controller.gerbers_string=text_area.text
            controller.open_path_ini=folder.toString().replace("file://","")
            text_anim.running=true
            bottom_controls_anim.running=true
            bot_row_layout.enabled=true
            progress.value=0
        }
    }
    GtFolderDialog{
        id: save_dialog
        onAccepted: {
            controller.save_path_ini=folder.toString().replace("file://","")
            console.log("save folder to string:"+controller.save_path_ini)
            save_path.text=controller.save_path_ini
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
                id: progress
                value: 0
                opacity: 1
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
                        width: progress.visualPosition * parent.width
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

                onClicked: close_app()
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
                readOnly: true
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
                readOnly: true
                height: 50
                text: controller.save_path_ini
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
                onCurrentTextChanged: {
                    controller.image_format_ini=currentText
                }
            }
            GtComboBox{
                id: dpi_combo
                static_text: "dpi"
                model: ["150", "300", "600", "900", "1200"]                
                down_color: main_color
                up_color: back_color
                border_color: main_color
                onCurrentTextChanged: {
                    controller.dpi_ini=currentText
                }
            }
            GtButton{
                id: go_button
                text: qsTr("GO!")
                w: 150
                down_color: back_color
                up_color: main_color
                border_color: main_color

                onClicked: {
                    progress.value=0
                    open_button.enabled=false
                    exit_button.enabled=false
                    bot_row_layout.enabled=false
                    window.run_processing()

                }
            }
        }
    }
}
