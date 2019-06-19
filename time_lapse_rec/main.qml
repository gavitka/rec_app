import QtQuick 2.12
import QtQuick.Window 2.12
import QtQuick.Layouts 1.12
import QtQuick.Controls 2.12
import QtQuick.Controls.Universal 2.12
import Qt.labs.settings 1.0
import io.qt.examples.backend 1.0
import io.qt.examples.enums 1.0
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

    Settings {
        property alias x: wnd.x
        property alias y: wnd.y
        property alias width: wnd.width
        property alias height: wnd.height
        //        property alias fileNameValue: fileName.text
        //        property alias outWidthValue: outWidth.text
        //        property alias outHeightalue: outHeight.text
        //        property alias framesPerSecondValue: framesPerSecond.text
        //        property alias shotsPerSecondValue: shotsPerSecond.text
    }

    ColumnLayout {
        anchors.fill: parent
        RowLayout {
            Layout.fillWidth: true
            Rectangle {
                implicitWidth: wnd.width
                implicitHeight: 200
                color: Theme.background_color
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
                        text: BackEnd.filePath
                        font.pointSize: 16
                        color: "black"
                        elide: Text.ElideMiddle
                    }
                    Button {
                        text: "Browse"
                        onClicked: openFolderDialog.open()
                        enabled: BackEnd.lockParam
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
                        id: framerateList
                        Layout.fillWidth: true
                        textRole: "key"
                        model: ListModel {
                            id:frameRatesModel
                            ListElement { key: "1x"; value: FRAMERATES.x1 }
                            ListElement { key: "2x"; value: FRAMERATES.x2 }
                            ListElement { key: "4x"; value: FRAMERATES.x4 }
                            ListElement { key: "8x"; value: FRAMERATES.x8 }
                            ListElement { key: "16x"; value: FRAMERATES.x16 }
                        }
                        onCurrentIndexChanged:BackEnd.setFrameRate(frameRatesModel.get(currentIndex).value)
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
                        id:resolutionList
                        Layout.fillWidth: true
                        textRole: "key"
                        model: ListModel {
                            id:resolutionsModel
                            ListElement { key: "1080p"; value: RESOLUTIONS.res1080p }
                            ListElement { key: "720p"; value: RESOLUTIONS.res720p }
                            ListElement { key: "360p"; value: RESOLUTIONS.res360p }
                        }
                        onCurrentIndexChanged:BackEnd.setResolution(resolutionsModel.get(currentIndex).value)
                        enabled: BackEnd.lockParam
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
                        id:bitrateListModel
                        Layout.fillWidth: true
                        textRole: "key"
                        model: ListModel {
                            id:bitratesModel
                            ListElement { key: "500 Kbps"; value: BITRATES.b500 }
                            ListElement { key: "1500 Kbps"; value: BITRATES.b1500 }
                            ListElement { key: "2000 Kbps"; value: BITRATES.b2000 }
                            ListElement { key: "2500 Kbps"; value: BITRATES.b2500 }
                            ListElement { key: "3000 Kbps"; value: BITRATES.b3000 }
                        }
                        onCurrentIndexChanged:BackEnd.setBitRate(bitratesModel.get(currentIndex).value)
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
                        RadioButton {
                            width: 20
                            checked: BackEnd.recMode ? true : false
                            enabled: BackEnd.lockParam
                            onCheckedChanged: checked ? BackEnd.recMode = true : BackEnd.recMode = false
                        }
                        Label{
                            Layout.fillWidth: true
                            text: "Screen"
                            font.pointSize: 16
                            font.weight: Font.DemiBold
                            color: "black"
                        }
                        RadioButton {
                            width: 20
                            checked: BackEnd.recMode ? false : true
                            enabled: BackEnd.lockParam
                            onCheckedChanged: checked ? BackEnd.recMode = false : BackEnd.recMode = true
                        }
                        Label{
                            Layout.fillWidth: true
                            text: "Window"
                            font.pointSize: 16
                            font.weight: Font.DemiBold
                            color: "black"
                        }
                    }
                    /* ----- */
                    Label {
                        Layout.fillWidth: true
                        text: "Window"
                        font.pointSize: 16
                        font.weight: Font.DemiBold
                        color: "black"
                        visible: !BackEnd.recMode
                    }
                    Label {
                        Layout.fillWidth: true
                        text: "Select window to record."
                        color: "black"
                        wrapMode: Text.WordWrap
                        visible: !BackEnd.recMode
                    }
                    ComboBox{
                        id:windowList
                        Layout.fillWidth: true
                        model:BackEnd.windowList
                        textRole: "name"
                        //onActiveFocusChanged: BackEnd.getWindowsList()
                        //onActivated: BackEnd.getWindowsList()
                        onVisibleChanged: BackEnd.getWindowsList()
                        enabled: BackEnd.lockParam
                        onCurrentIndexChanged:BackEnd.setWindow(currentIndex)
                        visible: !BackEnd.recMode
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
                        Switch {
                            width: 20
                            enabled: BackEnd.lockParam
                            checked: BackEnd.sleepMode
                        }
                        Label {
                            Layout.fillWidth: true
                            text: "On"
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
    OpenFolderDialog {
        id: openFolderDialog
        folder: BackEnd.fileUrl
        onAccepted: BackEnd.filePath = openFolderDialog.folder
    }
}
