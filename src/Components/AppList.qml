import QtQuick 2.7
import QtQuick.Controls 2.12
import QtQuick.Layouts 1.12
import Theme 1.0
import io.qt.examples.backend 1.0

ListView {
    Layout.fillWidth: true
    id: appListView
    model: BackEnd.appListModel
    clip: true

    height: 300

    delegate: CheckDelegate {
        text: model.text
        checked: model.sel
        width: parent.width
        onCheckedChanged: {
            appListView.model.select(index);
            appListView.model.printMe();
        }
    }
}
