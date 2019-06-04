import QtQuick 2.12
import QtQuick.Window 2.12
import QtQuick.Layouts 1.12
import QtQuick.Controls 2.12

Window {
    visible: true
    width: 640
    height: 480
    title: qsTr("Hello World")
    id: wnd

    ColumnLayout{
        id: grid
        anchors.fill: parent

        RowLayout{
            Layout.fillWidth: true

            Button{
                text: "Start"
                }

            Button{
                text: "Stop"
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
                text: "TextArea\n...\n...\n...\n...\n...\n...\n...\n...\n...\n...\n...\n...\n...\n...\n...\n...\n...\n
                        ...\n...\n...\n...\n...\n...\n...\n...\n...\n...\n...\n...\n...\n...\n...\n...\n...\n"
            }

            ScrollBar.vertical: ScrollBar { }
        }
    }
}
