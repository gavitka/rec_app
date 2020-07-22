#pragma once

#include "pch.h"

#include <QAbstractListModel>
#include <QVector>

#include "applist.h"

class AppListModel : public QAbstractListModel
{
    Q_OBJECT

    Q_PROPERTY(AppList* appList WRITE setAppList)

public:

    enum AppRoles {
        TextRole = Qt::UserRole + 1,
        SelectedRole
    };

    AppListModel();

    QHash<int, QByteArray> roleNames() const;

    int rowCount(const QModelIndex &parent = QModelIndex()) const;

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;

    void setAppList(AppList* value);

public slots:

    void select(int i);
    void dataChangedSlot();

private:

    AppList* m_applist = nullptr;
};
