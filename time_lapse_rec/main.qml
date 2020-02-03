import QtQuick 2.12
import QtQuick.Window 2.12
import QtQuick.Layouts 1.12
import QtQuick.Controls 2.12
import QtQuick.Controls.Universal 2.12
import Qt.labs.settings 1.0
import io.qt.examples.backend 1.0
//import io.qt.examples.enums 1.0
import Theme 1.0
import "Components"

Window {
    visible: true
    width: 500
    height: 600
    minimumHeight: 600
    minimumWidth: 500
    title: qsTr("Time lapse recording app")
    id:wnd
    //flags: Qt.Window | Qt.FramelessWindowHint
    // Qt.MSWindowsFixedSizeDialogHint
    // Qt.WindowStaysOnTopHint
    Settings {
        property alias x: wnd.x
        property alias y: wnd.y
        property alias width: wnd.width
        property alias height: wnd.height
    }

    ColumnLayout {
        anchors.fill: parent
        RowLayout {
            Layout.fillWidth: true
            Rectangle {
                implicitWidth: wnd.width
                implicitHeight: 200
                color: Theme.background_color

                /*MouseArea {
                    anchors.fill: parent
                    property point lastMousePos: Qt.point(0, 0)
                    onPressed: { lastMousePos = Qt.point(mouseX, mouseY); }
                    onMouseXChanged: wnd.x += (mouseX - lastMousePos.x)
                    onMouseYChanged: wnd.y += (mouseY - lastMousePos.y)
                }

                RowLayout{
                    anchors.top: parent.top
                    anchors.right: parent.right
                    TopButton {
                        Layout.rightMargin: 2
                        Layout.bottomMargin: 2
                        Layout.topMargin: 0
                        text:""
                        Image {
                            width:13
                            height:13
                            anchors.centerIn: parent
                            source: "images/minimize.png"
                        }
                        onClicked: wnd.visibility = Window.Minimized
                    }
                    TopButton {
                        Layout.rightMargin: 2
                        Layout.bottomMargin: 2
                        Layout.topMargin: 0
                        text:""
                        Image {
                            width:13
                            height:13
                            anchors.centerIn: parent
                            source: "images/close.png"
                        }
                        onClicked: wnd.close()
                    }
                }*/

                RowLayout {
                    spacing: 30
                    width: 350
                    anchors.centerIn: parent
                    Item {
                        width: recbutton.width
                        height: recbutton.height
                        RecButton {
                            id:recbutton
                            anchors.fill: parent
                            height:width
                            enabled: BackEnd.recordReady
                            onClicked: {
                                if(!isRecording) {
                                    BackEnd.startRecording()
                                    isRecording = true
                                }
                                else {
                                    BackEnd.stopRecording()
                                    isRecording = false
                                }
                            }
                        }
                        PauseButton {
                            id: pausebutton
                            width: 50
                            height: width
                            visible: recbutton.isRecording ? true : false
                            anchors.bottom: parent.bottom
                            anchors.right: parent.right
                            anchors.rightMargin: -10
                            onClicked: {
                                BackEnd.pauseRecording()
                                buttonToggled = !buttonToggled
                            }
                        }
                    }
                    ColumnLayout{
                        Layout.fillWidth: true
                        implicitWidth: 200
                        Label {
                            Layout.fillWidth: true
                            id: status
                            color: Theme.button_color
                            text: BackEnd.recordingState
                            font.pointSize: 21
                            font.weight: Font.DemiBold
                        }
                        Label {
                            color: "white"
                            font.pointSize: 10
                            text: BackEnd.recordingTime
                        }
                        Label {
                            color: "white"
                            font.pointSize: 10
                            text: BackEnd.statusLine
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
                        font.pointSize: 14
                    }
                    Label {
                        Layout.fillWidth: true
                        text: "Select folder to store recorded files."
                        font.pointSize: 10
                        wrapMode: Text.WordWrap
                    }
//                    Label {
//                        Layout.fillWidth: true
//                        id:filePath
//                        text: BackEnd.filePath
//                        font.pointSize: 14
//                        elide: Text.ElideMiddle
//                    }
                    Text {
                        Layout.fillWidth: true
                        id:fileLabel
                        text: BackEnd.filePath
                        font.pointSize: 14
                        elide: Text.ElideMiddle
                        MouseArea{
                            anchors.fill:parent
                            cursorShape: Qt.PointingHandCursor
                            onClicked: Qt.openUrlExternally(BackEnd.fileUrl)
                        }
                    }
                    Button {
                        text: "Browse"
                        onClicked: openFolderDialog.open()
                        enabled: BackEnd.lockParam
                    }
                    Label {
                        Layout.fillWidth: true
                        text: "File prefix"
                        font.pointSize: 14
                    }
                    Label {
                        Layout.fillWidth: true
                        text: "Specify file prefix. All the recorder files will be named automatically using "
                              + "this prefix."
                        font.pointSize: 10
                        wrapMode: Text.WordWrap
                    }
                    TextField {
                        Layout.fillWidth: true
                        text: BackEnd.filePrefix
                        selectByMouse: true
                        id:filePrefix
                        onEditingFinished: BackEnd.filePrefix = text
                        enabled: BackEnd.lockParam
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
                        font.pointSize: 14
                    }
                    Label {
                        Layout.fillWidth: true
                        text: "Select time lapse speed up recording factor."
                        font.pointSize: 10
                        wrapMode: Text.WordWrap
                    }
                    ComboBox{
                        id: framerateList
                        Layout.fillWidth: true
                        textRole: "name"
                        model: BackEnd.frameRateList
                        currentIndex: BackEnd.frameRateIndex
                        onCurrentIndexChanged: BackEnd.frameRateIndex = currentIndex
                    }
                    Label {
                        Layout.fillWidth: true
                        text: "Resolution"
                        font.pointSize: 14
                    }
                    Label {
                        Layout.fillWidth: true
                        text: "Select output file dimensions."
                        font.pointSize: 10
                        wrapMode: Text.WordWrap
                    }
                    ComboBox{
                        id:resolutionList
                        Layout.fillWidth: true
                        textRole: "name"
                        model: BackEnd.resolutionList
                        currentIndex: BackEnd.resolutionIndex
                        onCurrentIndexChanged: BackEnd.resolutionIndex = currentIndex
                        enabled: BackEnd.lockParam
                    }
                    Label {
                        Layout.fillWidth: true
                        text: "Bitrate"
                        font.family: "Segoe UI"
                        font.pointSize: 14
                    }
                    Label {
                        Layout.fillWidth: true
                        text: "Select bitrate of hte file."
                        font.pointSize: 10
                        wrapMode: Text.WordWrap
                    }
                    ComboBox{
                        id:bitrateListModel
                        Layout.fillWidth: true
                        textRole: "name"
                        model: BackEnd.bitRateList
                        currentIndex: BackEnd.bitRateIndex
                        onCurrentIndexChanged: BackEnd.bitRateIndex = currentIndex
                        enabled: BackEnd.lockParam
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
                        font.pointSize: 14
                    }
                    Label {
                        Layout.fillWidth: true
                        text: "Select window to record window, screen to record whole screen area."
                        font.pointSize: 10
                        wrapMode: Text.WordWrap
                    }
                    GridLayout{
                        Layout.fillWidth: true
                        columns: 2
                        RadioButton {
                            width: 20
                            checked: BackEnd.recMode ? false : true
                            enabled: BackEnd.lockParam
                            onCheckedChanged: checked ? BackEnd.recMode = false : BackEnd.recMode = true
                        }
                        Label{
                            Layout.fillWidth: true
                            text: "Screen"
                            font.pointSize: 14
                        }
                        RadioButton {
                            width: 20
                            checked: BackEnd.recMode ? true : false
                            enabled: BackEnd.lockParam
                            onCheckedChanged: checked ? BackEnd.recMode = true : BackEnd.recMode = false
                        }
                        Label{
                            Layout.fillWidth: true
                            text: "Window"
                            font.pointSize: 14
                        }
                    }
                    /* ----- */
                    Label {
                        Layout.fillWidth: true
                        text: "Window"
                        font.pointSize: 14
                        visible: BackEnd.recMode
                    }
                    Label {
                        Layout.fillWidth: true
                        text: "Select window to record."
                        font.pointSize: 10
                        wrapMode: Text.WordWrap
                        visible: BackEnd.recMode
                    }
                    RowLayout{
                        visible: BackEnd.recMode
                        ComboBox{
                            id:windowList
                            Layout.fillWidth: true
                            model:BackEnd.windowList
                            textRole: "name"
                            enabled: BackEnd.lockParam
                            onCurrentIndexChanged:BackEnd.setWindow(currentIndex)
                        }
                        Button{
                            text: "Refresh"
                            onClicked: BackEnd.getWindowsList()
                        }
                    }
                    /* ----- */
                    Rectangle {
                        Layout.margins: 10
                        Layout.alignment: Qt.AlignCenter
                        color: Universal.background
                        width:300
                        height:200
                        border.width: 2
                        border.color: "black"
                        Image{
                            source: BackEnd.imageSource
                            anchors.fill: parent
                            anchors.margins: 2
                            fillMode: Image.PreserveAspectFit
                        }
                    }
                    Label {
                        Layout.fillWidth: true
                        text: "Sleep when idle"
                        font.pointSize: 14
                    }
                    Label {
                        Layout.fillWidth: true
                        text: "If a window does not receive mouse input for 3 seconds." +
                              " Recording will pause until next time mouse is moved."
                        font.pointSize: 10
                        wrapMode: Text.WordWrap
                    }
                    RowLayout{
                        Layout.fillWidth: true
                        Switch {
                            id:sleepMode
                            width: 20
                            //enabled: BackEnd.lockParam
                            checked: BackEnd.sleepMode
                            onCheckedChanged: BackEnd.sleepMode = checked
                        }
                        Label {
                            Layout.fillWidth: true
                            text: sleepMode.checked ? "On" : "Off"
                            font.pointSize: 16
                            font.weight: Font.DemiBold
                        }
                    }
                }
            }
            ScrollBar.vertical: ScrollBar { }
        }
    }
    OpenFolderDialog {
        id: openFolderDialog
        folder: BackEnd.fileUrl
        onAccepted: BackEnd.filePath = openFolderDialog.folder
    }
}
