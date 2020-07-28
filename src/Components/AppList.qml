import QtQuick 2.12
import QtQuick.Controls 2.12
import QtQuick.Layouts 1.12
import Theme 1.0
import kh.components 1.0

ListView {
    id: appListView
    spacing: 6
    clip: true

    model: AppListModel {
        id: appListModel
        appList: BackEnd.appList
    }

    Component {
        id: appListItemDelegate
        Rectangle {
            id: wrapper
            height: 60
            width: appListView.width - 8
            color: "white"
            RowLayout {
                anchors.fill: parent
                anchors.leftMargin: 4
                ColumnLayout {
                    Layout.fillWidth: true
                    Layout.fillHeight: true
                    Text {
                        Layout.fillWidth: true
                        text: model.text
                        font.pointSize: 12
                        elide: Text.ElideRight
                    }
                    Text {
                        Layout.fillWidth: true
                        text: model.filename
                        font.pointSize: 10
                        elide: Text.ElideRight
                        color: Theme.loosetext
                    }
                } // ColumnLayout
                CheckBox {
                    checked: model.selected
                }
            } // RowLayout
            states: [
                State {
                    name : "selected"
                    PropertyChanges {
                        target: wrapper
                        color: Theme.button_hover_cover
                    }
                },
                State {
                    name : "default"
                    PropertyChanges {
                        target: wrapper
                        color: "white"
                    }
                }
            ] // states

            transitions: Transition {
                ColorAnimation {
                    duration: 75
                }
            }
            MouseArea {
                anchors.fill: parent
                hoverEnabled: true

                onEntered:
                {
                    wrapper.state = "selected"
                    thumbnailRect.show = true
                    //BackEnd.hover(index)
                    thumbnail.source = "image://previewprovider/" + index;
                }
                onExited:
                {
                    wrapper.state = "default"
                    thumbnailRect.show = false
                }
                onClicked: {
                    appListView.model.select(index);
                }
            } // MouseArea
        } // Rectangle

    } // Componenet

    delegate: appListItemDelegate

    ScrollBar.vertical: ScrollBar { }
} // ListView
