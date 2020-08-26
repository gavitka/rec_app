#include "appmanager.h"

#include <QFileInfo>

#include "lib.h"
#include "hooks_dll/hook.h"

extern HWND g_hwnd;

typedef void (__stdcall *f_funci)(HWND hwnd);

AppManager::AppManager(QObject *parent):
    QObject(parent)
{

}


void AppManager::refresh()
{
    // remove closed windows
    QMutableVectorIterator<APPDATA> it(m_data);
    while(it.hasNext()) {
        HWND hwnd = it.next().hwnd;
        if(!IsWindow(hwnd))
            it.remove();
    }

    BOOL ret = EnumWindows(windowListCallback, reinterpret_cast<LPARAM>(this));
    if(!ret)
        throw std::runtime_error("[TASK FAILED SUCCESSFULLY] Could not get windows list");

    emit listChanged();
}


void AppManager::add(HWND hwnd)
{
    for(auto it : m_data) {
        if(it.hwnd == hwnd) {
            return;
        }
    }

    QString name = "";
    QString exeName = "";
    bool is64 = false;
    try {
        BOOL res = getWindowInfo(hwnd, name, exeName, is64);
        if(res && IsWindowVisible(hwnd)) {
            m_data.push_back({hwnd, name, exeName, is64, false});
        }
    } catch(std::runtime_error e) {
        qDebug() << e.what();
    }
}


void AppManager::select(int i)
{
    m_data[i].selected = !m_data.at(i).selected;

    bool res = false;
    for(APPDATA it : m_data) {
        if(it.selected) {
            res = true;
            break;
        }
    }
    m_isselected = res;
    emit selectedChanged();
}


int AppManager::size()
{
    return m_data.size();
}


const APPDATA AppManager::at(int i)
{
    return m_data.at(i);
}

bool AppManager::check(HWND hwnd)
{
    for(APPDATA it : m_data) {
        if(it.hwnd == hwnd)
            return it.selected ? true : false;
    }
    return false;
}

bool AppManager::isSelected()
{
    return m_isselected;
}

void AppManager::installHook()
{
#ifdef HOOKS
    InstallGlobalHookDll(g_hwnd);
#endif
}

void AppManager::uninstallHook()
{
#ifdef HOOKS
    UninstallGlobalHookDll();
#endif
}


BOOL windowListCallback(HWND hwnd, LPARAM lParam)
{
    auto appmanager = reinterpret_cast<AppManager*>(lParam);
    appmanager->add(hwnd);
    return TRUE;
}

