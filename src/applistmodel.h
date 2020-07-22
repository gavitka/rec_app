#pragma once

#include "pch.h"

#include <QObject>
#include <QAbstractListModel>
#include <QVector>
#include <QDebug>

struct App {
    QString name;
    HWND hWnd;
    bool selected = false;
};

class AppListModel : public QAbstractListModel
{
    Q_OBJECT

public:

    Q_PROPERTY(int curIndex READ curIndex WRITE setCurIndex NOTIFY curIndexChanged)

    enum AppRoles {
        TextRole = Qt::UserRole + 1,
        SelectedRole
    };

    AppListModel();

    QHash<int, QByteArray> roleNames() const;

    int rowCount(const QModelIndex &parent = QModelIndex()) const;

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;

    int curIndex() {return m_curIndex;}
    void setCurIndex(int value) {
        m_curIndex = value;
        emit curIndexChanged();
    }

public slots:

    void appListChanged();
    void select(int i);
    void printMe();

signals:

    void curIndexChanged();

private:

    void toggleHook(int i);

    QVector<App> m_applist;
    int m_curIndex;

};

BOOL CALLBACK getWindowsListCallback2(HWND hWnd, LPARAM lParam);
