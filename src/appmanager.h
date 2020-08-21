#pragma once

#include "pch.h"

#include <QObject>
#include <QVector>

typedef struct _APPDATA {
    HWND hwnd;
    QString name;
    QString exename;
    bool is64;
    bool selected;
} APPDATA;

class AppManager : public QObject
{
    Q_OBJECT

public:

    AppManager(QObject* parent = nullptr);

    void add(HWND hwnd);
    int size();
    const APPDATA at(int i);
    void select(int i);
    bool check(HWND hwnd);
    bool isSelected();

public slots:

    void installHook();
    void uninstallHook();
    void refresh();

signals:

    void listChanged();
    void selectedChanged();

private:

//    void InstallSingleHook(HWND hwnd);

    QVector<APPDATA> m_data;
    bool m_hooksactive = false;
    bool m_isselected = false; // cache selected state
//    std::vector<HOOKDATA> m_hooks;

};


BOOL CALLBACK windowListCallback(HWND hwnd, LPARAM lParam);
