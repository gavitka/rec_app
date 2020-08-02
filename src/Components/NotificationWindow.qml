import QtQuick 2.12
import QtQuick.Window 2.12
import QtQuick.Layouts 1.12
import QtQuick.Controls 2.12
import QtGraphicalEffects 1.12

import kh.components 1.0
import Theme 1.0

// TODO: hide on mouse over

Window {
    id: statusWindow

    width: 200
    height: 32
    x: statusWindow.screen.width - statusWindow.width
    y: statusWindow.screen.height - statusWindow.height - 40
    visible: BackEnd.isRecording
    title: qsTr("Time lapse rec - status")
    color: Theme.background_color

    property int status: 0

    flags: Qt.FramelessWindowHint | Qt.WindowStaysOnTopHint | Qt.Popup

    RowLayout {
        anchors.fill:parent
        spacing:0

        Item {
            Layout.preferredHeight: 32
            Layout.preferredWidth: 32

            Rectangle {
                id:recIcon
                anchors.centerIn: parent
                width:20
                height:width
                radius: width/2
                color: isRec == true ? blinking : "gray"

                property bool isRec: (BackEnd.isRecording && !BackEnd.isSleeping)
                property color blinking: Theme.close_hover

                SequentialAnimation on blinking {
                    id: blinck
                    loops: Animation.Infinite
                    ColorAnimation {
                        from: Theme.close_hover
                        to: Theme.background_color
                        duration: 800
                    }
                    ColorAnimation {
                        from: Theme.background_color
                        to: Theme.close_hover
                        duration: 800
                    }
                }
            }
        }

        Label {
            Layout.fillWidth: true
            text: BackEnd.statusLine
        }

        Button {
            id: closeButton
            padding: 4

            implicitHeight: 32
            implicitWidth: 32

            state: closeButton.down ? "pressed" : closeButton.hovered ? "hover" : "default"

            contentItem: Item {
                    anchors.fill: parent
                    Image {
                        width: 10
                        height: 10
                        source: "qrc:/images/close.png"
                        anchors.centerIn: parent
                    }
                }

            background: Rectangle {
                id:backgroundRect
                color: Theme.background_color
                anchors.fill: parent
            }

            states: [
                State {
                    name: "hover"
                    PropertyChanges {
                        target: backgroundRect
                        color: Theme.button_color
                    }
                },
                State {
                    name: "pressed"
                    PropertyChanges {
                        target: backgroundRect
                        color: "white"
                    }
                },
                State {
                    name: "default"
                    PropertyChanges {
                        target: backgroundRect
                        color: Theme.background_color
                    }
                }
            ] // states

            transitions: Transition {
                ColorAnimation {
                    duration: 75
                }
            }

            onClicked: statusWindow.hide()
        } // Button: closeButton
    } // RowLayout
}
