#include "applistmodel.h"
#include "mousehook.h"
#include "backend.h"

AppListModel::AppListModel() {
    EnumWindows(getWindowsListCallback2, reinterpret_cast<LPARAM>(&m_applist));
}

QHash<int, QByteArray> AppListModel::roleNames() const {
    QHash<int, QByteArray> roles;
    roles[TextRole] = "text";
    roles[SelectedRole] = "sel";
    return roles;
}

int AppListModel::rowCount(const QModelIndex &parent) const {
    if(m_applist.isEmpty()) return 0;
    return m_applist.size();
}

QVariant AppListModel::data(const QModelIndex &index, int role) const {
    if(m_applist.isEmpty()) return "";
    switch(role) {
    case TextRole:
        return m_applist.at(index.row()).name;
    case SelectedRole:
        return m_applist.at(index.row()).selected;
    }
    return false;
}

void AppListModel::appListChanged() {
    QModelIndex topLeft = createIndex(0, 0);
    QModelIndex bottomRight = createIndex(m_applist.size()-1, 0);
    QVector<int> roleVector;
    emit dataChanged(topLeft, bottomRight, roleVector);
}

void AppListModel::select(int i) {
    //toggleHook(i);
    m_applist[i].selected = !m_applist[i].selected;

    QModelIndex topLeft = createIndex(i, 0);
    QModelIndex bottomRight = createIndex(i, 0);
    QVector<int> roleVector;
    emit dataChanged(topLeft, bottomRight, roleVector);
}

void AppListModel::printMe() {
    for(int i = 0; i < m_applist.size(); ++i) {
        qDebug() << "hwnd" << m_applist[i].hWnd << "name" << m_applist[i].name << "sel" << m_applist[i].selected;
    }
}

void AppListModel::toggleHook(int i) {
    if(m_applist[i].selected == false) {
        qDebug() << "Setting hook on" << m_applist[i].name << "hwnd" << m_applist[i].hWnd;
        //InstallHook2(BackEnd::getInstance()->getHwnd(), m_applist[i].hWnd);
    } else {
        qDebug() << "Removing hook on" << m_applist[i].name << "hwnd" << m_applist[i].hWnd;
        //RemoveHook2(m_applist[i].hWnd);
    }
}


BOOL CALLBACK getWindowsListCallback2(HWND hwnd, LPARAM lParam) {
    const DWORD TITLE_SIZE = 1024;
    WCHAR windowTitle[TITLE_SIZE];

    GetWindowTextW(hwnd, windowTitle, TITLE_SIZE);

    int length = ::GetWindowTextLength(hwnd);
    std::wstring title(&windowTitle[0]);
    if (!IsWindowVisible(hwnd) || length == 0 || title == L"Program Manager") {
        return TRUE;
    }

    QVector<App>& dataList = *reinterpret_cast<QVector<App>*>(lParam);
    dataList.append({QString::fromStdWString(windowTitle), hwnd});

    return TRUE;
}

