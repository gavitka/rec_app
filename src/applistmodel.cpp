#include "applistmodel.h"
#include "backend.h"

AppListModel::AppListModel()
{ }

QHash<int, QByteArray> AppListModel::roleNames() const
{
    QHash<int, QByteArray> roles;
    roles[TextRole] = "text";
    roles[SelectedRole] = "selected";
    roles[FileName] = "filename";
    return roles;
}

int AppListModel::rowCount(const QModelIndex &parent) const
{
    if(!m_appmanager) return 0;
    return m_appmanager->size();
}

QVariant AppListModel::data(const QModelIndex &index, int role) const
{
    if(!m_appmanager && m_appmanager->size() == 0) return "";
    switch(role) {
    case TextRole:
        return m_appmanager->at(index.row()).name;
    case SelectedRole:
        return m_appmanager->at(index.row()).selected;
    case FileName:
        return QString("[") +
                (m_appmanager->at(index.row()).is64 ? "x64" : "x32") +
                "] " + m_appmanager->at(index.row()).exename;
    }
    return false;
}

void AppListModel::setAppManager(AppManager* value)
{
    m_appmanager = value;
    connect(m_appmanager, &AppManager::listChanged, this, &AppListModel::dataChangedSlot);
    connect(m_appmanager, &AppManager::selectedChanged, this, &AppListModel::dataChangedSlot);
    dataChangedSlot();
}

void AppListModel::select(int i)
{
    m_appmanager->select(i);

    QModelIndex topLeft = createIndex(0, 0);
    QModelIndex bottomRight = createIndex(m_appmanager->size()-1, 0);
    QVector<int> roleVector({SelectedRole});
    emit dataChanged(topLeft, bottomRight, roleVector);
}

void AppListModel::dataChangedSlot()
{
    QModelIndex topLeft = QAbstractItemModel::createIndex(0, 0);
    QModelIndex bottomRight = QAbstractItemModel::createIndex(m_appmanager->size()-1, 0);
    emit dataChanged(topLeft, bottomRight, QVector<int>());
    beginResetModel();
    endResetModel();
}
