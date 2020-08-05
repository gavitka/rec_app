#pragma once

#include "pch.h"

struct App {
    QString name;
    QString exename;
    HWND hwnd;
    bool selected = false;
    HHOOK hook = NULL;
    bool is64 = false;
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

    void setHooks();
    void unsetHooks();
    void resetHooks();

signals:

    void listUpdated();
    void vectorChanged();
    void selectedChanged();

private:

    int windowsExists(HWND hwnd);

    QVector<App> m_data;
    bool m_hooks = false;

};

QString GetWindowTitle(HWND whnd);
QString getWindowExeName(HWND hwnd);
BOOL getWindowInfo(HWND hwnd, QString &exeName, bool &is64);

