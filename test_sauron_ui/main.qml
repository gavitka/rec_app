import QtQuick 2.12
import QtQuick.Window 2.12
import QtQuick.Layouts 1.12
import QtQuick.Controls 2.12
import io.qt.examples.backend 1.0

Window {
    visible: true
    width: 640
    height: 480
    title: qsTr("Hello World")
    id: wnd

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
                        onTextChanged: BackEnd.outWidth = text
                    }
                }

                RowLayout{
                    Label { text: "Output Width" }
                    TextField  {
                        text: BackEnd.outWidth
                        validator: IntValidator {bottom:100; top:1920}
                        onTextChanged: BackEnd.outWidth = text
                        selectByMouse: true
                        enabled: BackEnd.lockParam
                    }
                    Label{ text: "Output Height" }
                    TextField  {
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
                        text: BackEnd.framesPerSecond
                        validator: IntValidator {bottom:0; top:25}
                        onTextChanged: BackEnd.framesPerSecond = text
                        selectByMouse: true
                        enabled: BackEnd.lockParam
                    }
                    Label{ text: "Record fps" }
                    TextField  {
                        text: BackEnd.shotsPerSecond
                        validator: IntValidator {bottom:0; top:25}
                        onTextChanged: BackEnd.shotsPerSecond = text
                        selectByMouse: true
                        enabled: BackEnd.lockParam
                    }
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
