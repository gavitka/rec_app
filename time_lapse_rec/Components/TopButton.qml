import QtQuick 2.7
import QtQuick.Controls 2.12
import QtQuick.Layouts 1.12
import Theme 1.0

Button {
    id:rootId
    padding:4

    implicitHeight: 22
    implicitWidth: 45

    state: rootId.down ? "pressed" : rootId.hovered ? "hover" : "default"

    contentItem: Text {
        color: "#A5C2BE"
        text: rootId.text
        font: rootId.font
        horizontalAlignment: Text.AlignHCenter
        verticalAlignment: Text.AlignVCenter
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
    ]
    transitions: Transition {
        ColorAnimation {
            duration: 75
        }
    }
}
