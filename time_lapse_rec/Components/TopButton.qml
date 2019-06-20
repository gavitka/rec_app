import QtQuick 2.7
import QtQuick.Controls 2.12
import QtQuick.Layouts 1.12

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
        color:"#3D2732"
        anchors.fill: parent
    }

    states: [
        State {
            name: "hover"
            PropertyChanges {
                target: backgroundRect
                color: "#594851"
            }
        },
        State {
            name: "pressed"
            PropertyChanges {
                target: backgroundRect
                color: "#6D6167"
            }
        },
        State {
            name: "default"
            PropertyChanges {
                target: backgroundRect
                color: "#3D2732"
            }
        }
    ]
    transitions: Transition {
        ColorAnimation {
            duration: 75
        }
    }
}
