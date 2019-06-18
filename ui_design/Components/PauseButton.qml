import QtQuick 2.0
import QtQuick.Controls 2.12
import QtQuick.Controls.Universal 2.12
import Theme 1.0

Button {
    width:50
    height:height
    id:rootId

    state: rootId.down ? "pressed" : rootId.hovered ? "hover" : "default"

    contentItem:
        Item {
        Image {
            anchors.centerIn: parent
            source: "qrc:/images/pause_icon.png"
        }
    }

    background: Rectangle {
        id:backgroundRect
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.verticalCenter: parent.verticalCenter
        width:rootId.width
        height: width
        color: Theme.button_color
        radius: width * 0.5
        border.width: 3
        border.color: Theme.background_color
    }

    states: [
        State{
            name : "hover"
            PropertyChanges {
                target: backgroundRect
                color: Theme.button_color_hover
            }
        },
        State{
            name : "pressed"
            PropertyChanges {
                target: backgroundRect
                color: "white"
            }
        },
        State{
            name : "default"
            PropertyChanges {
                target: backgroundRect
                color: Theme.button_color
            }
        }
    ]
    transitions: Transition {
        ColorAnimation {
            duration: 75
        }
    }
}
