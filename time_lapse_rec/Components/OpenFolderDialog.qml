import QtQuick 2.2
import QtQuick.Dialogs 1.0

FileDialog {
    title: "Please choose a file"
    visible: false
    selectFolder: true
    nameFilters: [ "Folder" ]
}
