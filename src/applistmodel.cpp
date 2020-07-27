#include "applistmodel.h"
#include "hooks_dll/mousehook.h"
#include "backend.h"

AppListModel::AppListModel()
{ }

QHash<int, QByteArray> AppListModel::roleNames() const
{
    QHash<int, QByteArray> roles;
    roles[TextRole] = "text";
    roles[SelectedRole] = "selected";
    return roles;
}

int AppListModel::rowCount(const QModelIndex &parent) const
{
    if(!m_applist) return 0;
    return m_applist->size();
}

QVariant AppListModel::data(const QModelIndex &index, int role) const
{
    if(!m_applist && m_applist->size() == 0) return "";
    switch(role) {
    case TextRole:
        return m_applist->at(index.row()).name;
    case SelectedRole:
        return m_applist->at(index.row()).selected;
    }
    return false;
}

void AppListModel::setAppList(AppList* value)
{
    m_applist = value;
    connect(m_applist, &AppList::listUpdated, this, &AppListModel::dataChangedSlot);
    dataChangedSlot();
}

void AppListModel::select(int i)
{
    m_applist->select(i);

    QModelIndex topLeft = createIndex(0, 0);
    QModelIndex bottomRight = createIndex(m_applist->size()-1, 0);
    QVector<int> roleVector({SelectedRole});
    emit dataChanged(topLeft, bottomRight, roleVector);
}

void AppListModel::dataChangedSlot()
{
    QModelIndex topLeft = QAbstractItemModel::createIndex(0, 0);
    QModelIndex bottomRight = QAbstractItemModel::createIndex(m_applist->size()-1, 0);
    emit dataChanged(topLeft, bottomRight, QVector<int>());
    beginResetModel();
    endResetModel();
}

void AppListModel::refresh()
{
    m_applist->update();
}
