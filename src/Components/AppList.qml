import QtQuick 2.7
import QtQuick.Controls 2.12
import QtQuick.Layouts 1.12
import Theme 1.0
import io.qt.examples.backend 1.0
import kh.components 1.0

ListView {
    id: appListView
    clip: true

    model: AppListModel {
        id: appListModel
        appList: BackEnd.appList
    }

    delegate: CheckDelegate {
        text: model.text
        checked: model.selected
        width: parent.width
        onCheckedChanged: {
            appListView.model.select(index);
        }
    } // CheckDelegate

    ScrollBar.vertical: ScrollBar { }
} // ListView
