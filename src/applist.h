#pragma once

#include "pch.h"

struct App {
    QString name;
    QString exename;
    HWND hwnd;
    bool selected = false;
    bool hook;
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
    void updateVector(std::vector<HWND>* vector);
    bool isSelected();
//    void installHooks();

private:

    int AppList::windowsExists(HWND hwnd);

    QVector<App> m_data;

signals:

    void listUpdated();
    void vectorChanged();
    void selectedChanged();
};

QString GetWindowTitle(HWND whnd);
QString getWindowExeName(HWND hwnd);

