#pragma once

#include "pch.h"

#include <QAbstractListModel>
#include <QVector>

#include "appmanager.h"

class AppListModel : public QAbstractListModel
{
    Q_OBJECT

    Q_PROPERTY(AppManager* appManager WRITE setAppManager)

public:

    enum AppRoles {
        TextRole = Qt::UserRole + 1,
        SelectedRole,
        FileName
    };

    AppListModel();
    QHash<int, QByteArray> roleNames() const;
    int rowCount(const QModelIndex &parent = QModelIndex()) const;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
    void setAppManager(AppManager* value);

public slots:

    void select(int i);
    void dataChangedSlot();

private:

    AppManager* m_appmanager = nullptr;
};
