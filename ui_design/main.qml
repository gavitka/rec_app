import QtQuick 2.12
import QtQuick.Window 2.12
import QtQuick.Layouts 1.12
import QtQuick.Controls 2.12
import QtQuick.Controls.Universal 2.12
import Theme 1.0
import "Components"

Window {
    visible: true
    width: 500
    height: 600
    minimumHeight: 600
    minimumWidth: 500
    title: qsTr("Hello World")
    id:mainwindow

    ColumnLayout {
        anchors.fill: parent
        RowLayout {
            Layout.fillWidth: true
            Rectangle {
                implicitWidth: mainwindow.width
                implicitHeight: 200
                color: Theme.background_color
                RowLayout {
                    spacing: 30
                    anchors.centerIn: parent
                    Item {
                        width: recbutton.width
                        height: recbutton.height
                        RecButton {
                            id:recbutton
                            anchors.fill: parent
                            height:width
                        }
                        PauseButton {
                            id: pausebutton
                            width: 50
                            height: width
                            anchors.bottom: parent.bottom
                            anchors.right: parent.right
                            anchors.rightMargin: -10
                        }
                    }
                    ColumnLayout{
                        Layout.fillWidth: true
                        Label {
                            id: status
                            color: Theme.button_color
                            text: "Recording"
                            font.pointSize: 21
                            font.weight: Font.DemiBold
                        }
                        Label {
                            color: "white"
                            font.pointSize: 10
                            text: "Time: 01:02:03"
                        }
                        Label {
                            color: "white"
                            font.pointSize: 10
                            text: "Status: good"
                        }
                    }
                }
            }
        }
        Flickable{
            Layout.fillWidth: true
            Layout.fillHeight: true
            clip: true
            contentHeight: child.height + 80
            Item {
                width: parent.width
                ColumnLayout {
                    id: child
                    anchors.margins: 40
                    anchors.right: parent.right
                    anchors.left: parent.left
                    width: parent.width - 80
                    spacing:10;
                    Label {
                        Layout.fillWidth: true
                        text: "File"
                        font.pointSize: 18
                        font.weight: Font.DemiBold
                        color: Theme.text_header_color
                        Layout.topMargin: 10
                        Layout.bottomMargin: 5
                    }
                    Label {
                        Layout.fillWidth: true
                        text: "Folder path"
                        font.pointSize: 16
                        font.weight: Font.DemiBold
                        color: "black"
                    }
                    Label {
                        Layout.fillWidth: true
                        text: "Select folder to store recorded files."
                        color: "black"
                        wrapMode: Text.WordWrap
                    }
                    Label {
                        Layout.fillWidth: true
                        id:filePath
                        text: "C:\\Windows\\system32"
                        font.pointSize: 16
                        color: "black"
                    }
                    Button {
                        text: "Browse"
                    }
                    Label {
                        Layout.fillWidth: true
                        text: "File prefix"
                        font.pointSize: 16
                        font.weight: Font.DemiBold
                        color: "black"
                    }
                    Label {
                        Layout.fillWidth: true
                        text: "Specify file prefix. All the recorder files will be named automatically using "
                              + "this prefix. Specify file prefix. All the recorder files will be named automatically "
                              + "using this prefix."
                        color: "black"
                        wrapMode: Text.WordWrap
                    }
                    TextField {
                        Layout.fillWidth: true
                        text: "pipiska_"
                        selectByMouse: true
                        id:filePrefix
                    }
                    Label {
                        Layout.fillWidth: true
                        text: "Format settings"
                        font.pointSize: 18
                        font.weight: Font.DemiBold
                        color: Theme.text_header_color
                        Layout.topMargin: 10
                        Layout.bottomMargin: 5
                    }
                    Label {
                        Layout.fillWidth: true
                        text: "Frame rate"
                        font.pointSize: 16
                        font.weight: Font.DemiBold
                        color: "black"
                    }
                    Label {
                        Layout.fillWidth: true
                        text: "Select time lapse speed up recording factor."
                        color: "black"
                        wrapMode: Text.WordWrap
                    }
                    ComboBox{
                        Layout.fillWidth: true
                    }
                    Label {
                        Layout.fillWidth: true
                        text: "Resolution"
                        font.pointSize: 16
                        font.weight: Font.DemiBold
                        color: "black"
                    }
                    Label {
                        Layout.fillWidth: true
                        text: "Select output file dimensions."
                        color: "black"
                        wrapMode: Text.WordWrap
                    }
                    ComboBox{
                        Layout.fillWidth: true
                    }
                    Label {
                        Layout.fillWidth: true
                        text: "Bitrate"
                        font.family: "Segoe UI"
                        font.pointSize: 16
                        font.weight: Font.DemiBold
                        color: "black"
                    }
                    Label {
                        Layout.fillWidth: true
                        text: "Select bitrate of hte file."
                        color: "black"
                        wrapMode: Text.WordWrap
                    }
                    ComboBox{
                        Layout.fillWidth: true
                    }
                    Label {
                        Layout.fillWidth: true
                        text: "Capture settings"
                        font.pointSize: 18
                        font.weight: Font.DemiBold
                        color: Theme.text_header_color
                        Layout.topMargin: 10
                        Layout.bottomMargin: 5
                    }
                    Label {
                        Layout.fillWidth: true
                        text: "Source"
                        font.pointSize: 16
                        font.weight: Font.DemiBold
                        color: "black"
                    }
                    Label {
                        Layout.fillWidth: true
                        text: "Select window to record window, screen to record whole screen area."
                        color: "black"
                        wrapMode: Text.WordWrap
                    }
                    GridLayout{
                        Layout.fillWidth: true
                        columns: 2
                        RadioButton { width: 20 }
                        Label{
                            Layout.fillWidth: true
                            text: "Window"
                            font.pointSize: 16
                            font.weight: Font.DemiBold
                            color: "black"
                        }
                        RadioButton { width: 20 }
                        Label{
                            Layout.fillWidth: true
                            text: "Screen"
                            font.pointSize: 16
                            font.weight: Font.DemiBold
                            color: "black"
                        }
                    }
                    Label {
                        Layout.fillWidth: true
                        text: "Window"
                        font.pointSize: 16
                        font.weight: Font.DemiBold
                        color: "black"
                    }
                    Label {
                        Layout.fillWidth: true
                        text: "Select window to record."
                        color: "black"
                        wrapMode: Text.WordWrap
                    }
                    ComboBox{
                        Layout.fillWidth: true
                    }
                    Rectangle {
                        Layout.margins: 10
                        Layout.alignment: Qt.AlignCenter
                        color: Universal.background
                        width:300
                        height:200
                        border.width:1
                        Image{
                            anchors.fill: parent
                        }
                    }
                    Label {
                        Layout.fillWidth: true
                        text: "Sleep when idle"
                        font.pointSize: 16
                        font.weight: Font.DemiBold
                        color: "black"
                    }
                    Label {
                        Layout.fillWidth: true
                        text: "If a window does not receive mouse input for 3 seconds." +
                              " Recording will pause until next time mouse is moved."
                        font.pointSize: 12
                        color: "black"
                        wrapMode: Text.WordWrap
                    }
                    RowLayout{
                        Layout.fillWidth: true
                        RadioButton { width: 20 }
                        Label{
                            Layout.fillWidth: true
                            text: "Window"
                            font.pointSize: 16
                            font.weight: Font.DemiBold
                            color: "black"
                        }
                    }
                    Label {
                        Layout.fillWidth: true
                        text: "Dummy"
                        font.pointSize: 16
                        font.weight: Font.DemiBold
                        color: "black"
                    }
                    Label {
                        Layout.fillWidth: true
                        text: "If a window does not receive mouse input for 3 seconds." +
                              " Recording will pause until next time mouse is moved."
                        color: "black"
                        wrapMode: Text.WordWrap
                    }
                    RowLayout{
                        Layout.fillWidth: true
                        RadioButton { width: 20 }
                        Label{
                            Layout.fillWidth: true
                            text: "Window"
                            font.pointSize: 16
                            font.weight: Font.DemiBold
                            color: "black"
                        }
                    }
                }
            }
            ScrollBar.vertical: ScrollBar { }
        }
    }
}
