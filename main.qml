import QtQuick 2.12
import QtQuick.Window 2.12
import Qt.labs.platform 1.1
import QtQuick.Layouts 1.3
import QtQuick.Controls 2.12
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
        if(msg_dialog.visible==true){
            msg_dialog.msg_text+="\n\n"+msg
        }
        else{
            msg_dialog.msg_text=msg
            msg_dialog.visible=true
        }
        msg_dialog.x=(window.width-msg_dialog.width)/2+window.x
        msg_dialog.y=(window.height-msg_dialog.height)/2+window.y
    }
    function set_ini_parameters(){
        open_dialog.folder=controller.open_path_ini
        save_dialog.folder=controller.save_path_ini
        save_path.text=controller.save_path_ini
        dpi_combo.currentIndex=dpi_combo.find(controller.dpi_ini)
        format_combo.currentIndex=format_combo.find(controller.image_format_ini)
        if (controller.open_folder_after_processing_ini=="on"){
            settings_window.open_folder_chckbox_checked=true
        }
        else {
            settings_window.open_folder_chckbox_checked=false
        }
        if (controller.opacity_mode_ini=="on"){
            settings_window.transparent_img_chckbox_checked=true
        }
        else {
            settings_window.transparent_img_chckbox_checked=false
        }
        if (controller.quick_translation_ini=="on"){
            settings_window.quick_translation_chckbox_checked=true
        }
        else {
            settings_window.quick_translation_chckbox_checked=false
        }
        if (controller.image_size_ini=="by_ini"){
            settings_window.board_size_chckbox_checked=true
        }
        else {
            settings_window.board_size_chckbox_checked=false
        }
    }
    function progress_bar(val){
        if (count_of_files!=0){
            progress.value=progress.value+(1/count_of_files)
        }
    }
    function done_slot(msg){
        open_button.enabled=true
        settings_button.enabled=true
        help_button.enabled=true
        exit_button.enabled=true
        bot_row_layout.enabled=true
        progress.value=1
        progress_text.text="Done!"
        show_message(msg)
    }
    function exit_slot(){
        window.close()
    }

//    // Convert URL to path function
//    function urlToPath(urlpath){
//        var f_path=urlToPath
//        if(f_path.startsWith("file://")){
//            f_path.replace("file://","")
//        }
//        else {
//            f_path.replace("file:///","")
//        }
//        return f_path
//    }

    id: window
    visible: true
    minimumWidth: 850
    height: 480
    title: qsTr("Gerber-translator")
    color: back_color
    opacity: 1
    flags: Qt.FramelessWindowHint | Qt.Window


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

    GtMessageWindow{
        id: msg_dialog
        title: "Gerber-translator"
        visible: false
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
                text_area.append(open_dialog.files[i].toString())
            }
            controller.gerbers_string=text_area.text
            controller.open_path_ini=folder.toString()
            text_anim.running=true
            bottom_controls_anim.running=true
            bot_row_layout.enabled=true
            progress.value=0
            progress_text.text=""
            if (settings_window.quick_translation_chckbox_checked){
                run_processing()
            }
        }
    }
    GtFolderDialog{
        id: save_dialog
        onAccepted: {
            controller.save_path_ini=folder.toString()
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
                    Text {
                        id: progress_text
                        text: ""
                        anchors.centerIn: parent
                        color: back_color
                    }
                }
            }
            GtButton{
                id: settings_button
                text: qsTr("Settings")
                down_color: main_color
                up_color: back_color
                border_color: main_color
                onClicked: {

                    settings_window.visible=true

                }
            }
            GtButton{
                id: help_button
                text: qsTr("?")
                w: 50
                down_color: main_color
                up_color: back_color
                border_color: main_color
                onClicked: show_message("Gerber translator.\n\nVersion: 1.10.\nRelease date: 01.10.2019\nAuthor: Velesko S.A.")
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
                    progress_text.text=""
                    open_button.enabled=false
                    settings_button.enabled=false
                    exit_button.enabled=false
                    bot_row_layout.enabled=false
                    help_button.enabled=false
                    window.run_processing()
                }
            }
        }
    }//end of ColumnLayout



    GtModalWindow{
        id: settings_window
        x: (window.width-width)/2+window.x
        y: (window.height-height)/2+window.y
        color: back_color
        opacity: 0.9
        visible: false

        onQuick_translation_chckbox_checkedChanged: {
            if(quick_translation_chckbox_checked){
                controller.quick_translation_ini="on"
            }
            else{
                controller.quick_translation_ini="off"
            }
        }
        onOpen_folder_chckbox_checkedChanged: {
            if(open_folder_chckbox_checked){
                controller.open_folder_after_processing_ini="on"
            }
            else{
                controller.open_folder_after_processing_ini="off"
            }
        }
        onBoard_size_chckbox_checkedChanged: {
            if(board_size_chckbox_checked){
                controller.image_size_ini="by_ini"
            }
            else{
                controller.image_size_ini="by_outline"
            }
        }
        onTransparent_img_chckbox_checkedChanged: {
            if(transparent_img_chckbox_checked){
                controller.opacity_mode_ini="on"
            }
            else{
                controller.opacity_mode_ini="off"
            }
        }

    }
}
