import QtQuick 2.7
import QtQuick.Controls 2.1
import QtQuick.Layouts 1.12
import Theme 1.0

Button {
    id: rootId
    contentItem:
        RowLayout {
            id:buttonItem
            spacing: 0
            Image {
                source: "qrc:/images/refresh.png"
                width:42
                height:42
            }
            Label {
                Layout.leftMargin:8
                text: "Refresh"
                font.pointSize: 16
                font.weight: Font.DemiBold
            }
            transform:
                Scale {
                    id: contentScale
                    origin.x: buttonItem.width/2;
                    origin.y: buttonItem.height/2;
                    xScale: 1
                    yScale: 1
                }
    } // RowLayout

    state: rootId.down ? "pressed" : rootId.hovered ? "hover" : "default"

    background:
        Rectangle {
        id:backgroundRect
        }

    states: [
        State{
            name : "hover"
            PropertyChanges {
                target: contentScale
                xScale: 1
                yScale: 1
            }
            PropertyChanges {
                target: backgroundRect
                color: Theme.button_hover_cover
            }
        }, // state
        State{
            name : "pressed"
            PropertyChanges {
                target: contentScale
                xScale: .95
                yScale: .95
            }
            PropertyChanges {
                target: backgroundRect
                color: Theme.button_hover_cover
            }
        }, // state
        State{
            name : "default"
            PropertyChanges {
                target: contentScale
                xScale: 1
                yScale: 1
            }
            PropertyChanges {
                target: backgroundRect
                color: Theme.button_back_cover
            }
        } // state
    ] //states
    transitions: Transition {
        NumberAnimation {
            properties: "xScale, yScale";
            duration: 75
        }
        ColorAnimation {
            properties: "color";
            duration: 75
        }
    }
} // Button
