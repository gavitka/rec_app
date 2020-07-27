#pragma once

#include "pch.h"

struct App {
    QString name;
    //QString exename;
    HWND hwnd;
    bool selected = false;
};

BOOL CALLBACK getWindowsListCallback2(HWND hwnd, LPARAM lParam);

class AppList : public QObject
{
    Q_OBJECT

public:

    AppList(QObject* parent);
    void update();
    const App at(int i);
    int size();
    void select(int i);
    void addWindows(QVector<HWND>* add_list);

public slots:

    void updateVector(std::vector<HWND>* vector);

private:

    int AppList::windowsExists(HWND hwnd);

    QVector<App> m_data;

signals:

    void listUpdated();
    void vectorChanged();
};

QString GetWindowTitle(HWND whnd);
