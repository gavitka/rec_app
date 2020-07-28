import QtQuick 2.12
import QtQuick.Window 2.12
import QtQuick.Layouts 1.12
import QtQuick.Controls 2.12
import QtQuick.Controls.Universal 2.12
import Qt.labs.settings 1.0
import kh.components 1.0

import Theme 1.0
import "Components"

BLWindow {
    visible: true
    width: 500
    height: 600
    minimumHeight: 600
    minimumWidth: 500
    title: qsTr("Time lapse recording app")
    id:wnd

    Settings {
        property alias x: wnd.x
        property alias y: wnd.y
        property alias width: wnd.width
        property alias height: wnd.height
    }

//    onClosing: {
//        if(BackEnd.isRecording == true){
//            console.log("deny closing");
//            close.accepted = false;
//            BackEnd.close();
//        }
//    }

//    Connections {
//        target: BackEnd
//        function onCloseReady() {
//            console.log("ready closing");
//            wnd.close();
//        }
//    }

    ColumnLayout {
        anchors.fill: parent
        Rectangle {
            Layout.fillWidth: true
            Layout.preferredHeight: 200
            color: Theme.background_color

            Image {
                anchors.fill:parent
                fillMode: Image.Stretch
                source: "qrc:/images/back.png"
            }

            RowLayout {
                anchors.right: parent.right
                anchors.top: parent.top
                Layout.alignment: Qt.AlignRight
                spacing: 0
                AppButton {
                    Layout.preferredWidth: 46
                    Layout.preferredHeight: 30
                    src:"qrc:/images/collapse.png"
                    onClicked: wnd.showMinimized()
                    }
                AppButton {
                    Layout.preferredWidth: 46
                    Layout.preferredHeight: 30
                    src: (wnd.visibility === 4) ? "qrc:/images/restore.png" :  "qrc:/images/full.png"
                    onClicked: (wnd.visibility === 4) ? wnd.showNormal() : wnd.showMaximized()
                    }
                AppButton {
                    Layout.preferredWidth: 46
                    Layout.preferredHeight: 30
                    hoverColor: Theme.close_hover
                    src:"qrc:/images/close.png"
                    onClicked: {
                            wnd.close();
                        }
                    }
            }// RowLayout

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
                        isRecording: BackEnd.isRecording
                        onClicked: {
                            if(!isRecording) {
                                BackEnd.startRecording()
                            }
                            else {
                                BackEnd.stopRecording()
                            }
                        }
                    } // RecButton
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
                } // Item
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
                } // ColumnLayout
            } // RowLayout
        } // Rectangle
        Flickable{
            Layout.fillWidth: true
            Layout.fillHeight: true
            clip: true
            contentHeight: child.height + 80
            Item {
                width: parent.width
                ColumnLayout {
                    id: child
                    anchors.margins: 32
                    anchors.right: parent.right
                    anchors.left: parent.left
                    spacing: 10
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
                    } // Text
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
                        text: "Crop or fill"
                        font.pointSize: 14
                    }
                    Label {
                        Layout.fillWidth: true
                        text: "Select whether to crop video or to add black rectangles to conform video to aspect ratio."
                        font.pointSize: 10
                        wrapMode: Text.WordWrap
                    }
                    ComboBox{
                        id:cropList
                        Layout.fillWidth: true
                        textRole: "name"
                        model: BackEnd.cropList
                        currentIndex: BackEnd.cropIndex
                        onCurrentIndexChanged: BackEnd.cropIndex = currentIndex
                        //enabled: BackEnd.lockParam
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
                    } // GridLayout
//                    Label {
//                        Layout.fillWidth: true
//                        text: "Window"
//                        font.pointSize: 14
//                        visible: BackEnd.recMode

//                    }
//                    Label {
//                        Layout.fillWidth: true
//                        text: "Select window to record."
//                        font.pointSize: 10
//                        wrapMode: Text.WordWrap
//                        visible: BackEnd.recMode
//                    }
//                    ComboBox{
//                        visible: BackEnd.recMode
//                        id:windowList
//                        Layout.fillWidth: true
//                        model:BackEnd.windowList
//                        textRole: "name"
//                        enabled: BackEnd.lockParam
//                        Binding { target: BackEnd
//                            property: "windowIndex"
//                            value: windowList.currentIndex
//                        }
//                        Binding { target: windowList
//                            property: "currentIndex"
//                            value: BackEnd.windowIndex
//                        }
//                        onPressedChanged: {
//                            BackEnd.getWindowsList()
//                        }
//                    } //
                    RowLayout {
                        Layout.fillWidth: true
                        Layout.preferredHeight: 42
                        spacing: 0
                        Label {
                            Layout.fillWidth: true
                            text: "Window list"
                            font.pointSize: 16
                            font.weight: Font.DemiBold
                        }
                        RefreshButton {
                            onClicked: appListView.model.refresh()
                        }
                    } // RowLayout
                    Label {
                        Layout.fillWidth: true
                        text: "Select windows for recording. If window recording mode is active " +
                              "only selected windows will be recorded, otherwise, selected windows " +
                              "will be used to determine mouse input for sleeping mode option."
                        font.pointSize: 10
                        wrapMode: Text.WordWrap
                    }
                    AppList {
                        id: appListView
                        Layout.fillWidth: true
                        Layout.topMargin: 12
                        Layout.bottomMargin: 12
                        height: 350
                    }
                    Label {
                        Layout.fillWidth: true
                        text: "Sleep when idle"
                        font.pointSize: 14
                    }
                    Label {
                        Layout.fillWidth: true
                        text: "If a selected windows will not receive mouse input for 3 seconds " +
                              "recording will pause until next time mouse is moved."
                        font.pointSize: 10
                        wrapMode: Text.WordWrap
                        MouseArea {
                            anchors.fill: parent
                            hoverEnabled: true
                            onEntered: thumbnailRect.show = true
                            onExited: thumbnailRect.show = false
                        }
                    }
                    RowLayout{
                        Layout.fillWidth: true
                        Switch {
                            id:sleepMode
                            width: 20
                            //enabled: BackEnd.lockParam
                            checked: BackEnd.sleepMode
                            onClicked: BackEnd.sleepMode = checked
                        }
                        Label {
                            Layout.fillWidth: true
                            text: sleepMode.checked ? "On" : "Off"
                            font.pointSize: 16
                            font.weight: Font.DemiBold
                        }
                    } // RowLayout
                } // ColumnLayout
            } // Item
            ScrollBar.vertical: ScrollBar { }
        } // Flickable
    } // ColumnLayout

    OpenFolderDialog {
        id: openFolderDialog
        folder: BackEnd.fileUrl
        onAccepted: BackEnd.filePath = openFolderDialog.folder
    }

    Rectangle {
        id: thumbnailRect
        width: parent.width
        height: 200
        anchors.bottom: parent.bottom
        visible: true
        color: Theme.button_hover_cover

        property bool show: false

        state: show ? "show" : "hide"

        Image{
            width: 300
            height: 200
            anchors.centerIn: parent
            id: thumbnail
            source: "image://previewprovider"
            fillMode: Image.PreserveAspectFit
        }

        transform: Translate { id: thumbnailTransform; y: 200 }

        states: [
            State {
                name : "show"
                PropertyChanges { target: thumbnailTransform; y: 0 }
            },
            State {
                name : "hide"
                PropertyChanges { target: thumbnailTransform; y: 200 }
            }
        ]

        transitions: Transition {
            SequentialAnimation {
                NumberAnimation { property: "y"; duration: 100 }
                ScriptAction { script: { thumbnailRect.visible = thumbnailRect.show } }
            }
        }
    } // Rectangle: thumbnailRect
} // BLWindow
