import QtQuick 2.0
import QtQuick.Controls 2.1
import QtGraphicalEffects 1.12

import Theme 1.0

Button {
    id: rootId
    implicitWidth: 46
    implicitHeight: 30

    property string src: ""
    property color hoverColor: Theme.appbutton_hover

    background:
        Rectangle {
            id: backgroundRect
                PropertyChanges {
                    target: color_effect
                    enabled: true
                }
        } // Rectangle

    contentItem:
        Item {
            anchors.fill: parent
            Image {
                id: img
                source: src
                width: 10
                height: 10
                anchors.centerIn: parent

                layer {
                    id: color_effect
                    enabled: true
                    effect:
                        ColorOverlay {
                        source: img
                        color: "white"
                    }
                } // layer
            } // Image
        }//Item

    state: rootId.down ? "pressed" : rootId.hovered ? "hover" : "default"

    states: [
        State{
            name : "hover"
            PropertyChanges {
                target: color_effect
                enabled: true
            }
            PropertyChanges {
                target: backgroundRect
                color: hoverColor
            }
        },
        State{
            name : "pressed"
            PropertyChanges {
                target:color_effect
                enabled: true
            }
            PropertyChanges {
                target: backgroundRect
                color: hoverColor
            }
        },
        State{
            name : "default"
            PropertyChanges {
                target: color_effect
                enabled: false
            }
            PropertyChanges {
                target: backgroundRect
                color: "transparent"
            }
        }
    ] // states
} // Button
