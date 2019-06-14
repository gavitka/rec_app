import QtQuick 2.12
import QtQuick.Window 2.12
import QtQuick.Layouts 1.12
import QtQuick.Controls 2.12
import io.qt.examples.backend 1.0
import Qt.labs.settings 1.0

Window {
    visible: true
    width: 640
    height: 480
    title: qsTr("Marxist recording program")
    id: wnd

    Settings {
        property alias x: wnd.x
        property alias y: wnd.y
        property alias width: wnd.width
        property alias height: wnd.height
        property alias fileNameValue: fileName.text
        property alias outWidthValue: outWidth.text
        property alias outHeightalue: outHeight.text
        property alias framesPerSecondValue: framesPerSecond.text
        property alias shotsPerSecondValue: shotsPerSecond.text
    }

    ColumnLayout{
        id: grid
        anchors.fill: parent

        GroupBox{
            title: "Parameters"
            Layout.fillWidth: true
            padding: 20;

            ColumnLayout{
                anchors.fill: parent
                RowLayout{
                    Label { text: "File name" }
                    TextField  {
                        id: fileName
                        Layout.fillWidth: true
                        text: BackEnd.outFileName
                        selectByMouse: true
                        enabled: BackEnd.lockParam
                        //onTextChanged: BackEnd.outFileName = text
                        onEditingFinished: BackEnd.outFileName = text
                    }
                }
                RowLayout{
                    Label { text: "Output Width" }
                    TextField  {
                        id:outWidth
                        text: BackEnd.outWidth
                        validator: IntValidator {bottom:100; top:1920}
                        onTextChanged: BackEnd.outWidth = text
                        selectByMouse: true
                        enabled: BackEnd.lockParam
                    }
                    Label{ text: "Output Height" }
                    TextField  {
                        id:outHeight
                        text: BackEnd.outHeight
                        validator: IntValidator {bottom:100; top:1920}
                        onTextChanged: BackEnd.outHeight = text
                        selectByMouse: true
                        enabled: BackEnd.lockParam
                    }
                }

                RowLayout{
                    Label { text: "Playback fps" }
                    TextField  {
                        id:framesPerSecond
                        text: BackEnd.framesPerSecond
                        validator: IntValidator {bottom:0; top:25}
                        onTextChanged: BackEnd.framesPerSecond = text
                        selectByMouse: true
                        enabled: BackEnd.lockParam
                    }
                    Label{ text: "Record fps" }
                    TextField  {
                        id:shotsPerSecond
                        text: BackEnd.shotsPerSecond
                        validator: IntValidator {bottom:0; top:25}
                        onTextChanged: BackEnd.shotsPerSecond = text
                        selectByMouse: true
                        enabled: BackEnd.lockParam
                    }
                }
                RowLayout{
                    Label { text: "Record window" }
                    CheckBox{
                        id: recCheck
                        enabled: BackEnd.lockParam
                        onClicked: {
                            BackEnd.recMode = checked;
                            BackEnd.getWindowsList()
                        }
                        Component.onCompleted: checked = BackEnd.recMode
                        Connections {
                            target: BackEnd
                            onDataChanged: myCheck.checked = BackEnd.recMode
                        }
                    }
                    Label { text: "Select window" }
                    ComboBox{
                        Layout.fillWidth: true
                        model: BackEnd.windowList
                        textRole:"name"
                        onCurrentIndexChanged:BackEnd.setWindow(currentIndex)
                        enabled: BackEnd.lockParam
                    }
                }
                RowLayout{
                    Label{ text: "mouse position" }
                    Text { text: BackEnd.mouseX }
                    Text { text: BackEnd.mouseY }
                }
            }
        }

        GroupBox{
            title: "Recording"
            Layout.fillWidth: true
            RowLayout{
                anchors.fill: parent
                Button{
                    text: BackEnd.startButtonText
                    onClicked: BackEnd.startRecording()
                }
                Button{
                    text: "Stop"
                    enabled: BackEnd.stopEnabled
                    onClicked: BackEnd.stopRecording()
                }
//                Button{
//                    text: "ShowWindows"
//                    enabled: true
//                    onClicked: BackEnd.getWindowsList()
//                }
            }
        }

        Flickable{
            Layout.fillWidth: true
            Layout.fillHeight: true
            clip: true

            contentHeight: textArea.implicitHeight

            TextArea {
                id:textArea
                selectByMouse: true
                text: BackEnd.outputText
                onTextChanged: BackEnd.outputText = text
            }

            ScrollBar.vertical: ScrollBar { }
        }
    }
}
