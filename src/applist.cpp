#include <QDir>

#include "applist.h"
#include "psapi.h"
#include "hooks.h"

BOOL CALLBACK getWindowsListCallback2(HWND hwnd, LPARAM lParam)
{
    auto add_list = reinterpret_cast<QVector<HWND>*>(lParam);
    add_list->push_back(hwnd);

    return TRUE;
}

AppList::AppList(QObject *parent):
    QObject(parent)
{
    update();
}

const App AppList::at(int i)
{
    return m_data.at(i);
}

int AppList::size()
{
    return m_data.size();
}

void AppList::select(int i)
{
    if(i >=0 && i <m_data.size()) {
        m_data[i].selected = !m_data[i].selected;
    }
    resetHooks();
    emit selectedChanged();
}

bool AppList::isSelected()
{
    for(auto w : m_data) {
        if(w.selected == true) return true;
    }
    return false;
}

void AppList::setHooks()
{
    if(m_hooks) {
        qDebug() << "Hooks already set.";
        return;
    }

    if(isSelected()) {
        for(auto w : m_data) {
            if (w.selected) {
                try {
                    ::InstallHook(w.hwnd);
                }
                catch (std::exception e) {
                    qDebug() << "e.what()" << e.what();
                }
            }
        }
    } else {
        ::InstallGlobalHook();
    }

    m_hooks = true;
}

void AppList::unsetHooks()
{
    if(!m_hooks) {
        qDebug() << "Hooks not set.";
        return;
    }

    qDebug() << "uninstalling hooks";
    ::UninstallHooks();
    m_hooks = false;
}

void AppList::resetHooks()
{
    unsetHooks();
    setHooks();
}

int AppList::windowsExists(HWND hwnd)
{
    QString title = GetWindowTitle(hwnd);
    QString exename = getWindowExeName(hwnd);

    // TODO: better check for existing windows
    for(int i = 0; i < m_data.size(); ++i) {
        if(m_data[i].name == title ||
               m_data[i].hwnd == hwnd ||
               m_data[i].exename == exename ) {
            return i;
        }
    }
    return -1;
}

void AppList::addWindows(QVector<HWND>* add_list)
{
    QString title;
    QString exename;
    bool is64;

    // Removing empty windows
    QMutableVectorIterator<HWND> it(*add_list);
    while(it.hasNext()) {
        HWND hwnd = it.next();
        title = GetWindowTitle(hwnd);

        if (!IsWindowVisible(hwnd)
            || title.length() == 0
            || title == L"Program Manager"
            || title == "Time lapse rec"
            || title == "Time lapse recording app") { // TODO: investigate, why 2 windows
            it.remove();
        } else {
            try {
                getWindowInfo(hwnd, exename, is64);
                if(exename == "time_lapse_rec.exe" || !is64) it.remove();
            }
            catch(std::exception e) {
                qDebug() << "e.what()" << e.what();
                it.remove();
            }
        }
    } // dat syntax though

    // removing non-existent windows
    QMutableVectorIterator<App> jt(m_data);
    while(jt.hasNext()) {
        App &app = jt.next();
        [&]() {
            for(auto hwnd : *add_list)
                if(hwnd == app.hwnd) {
                    return; // found
                }
            jt.remove(); // now found, remove
        }();
    }

    // adding windows
    for(auto hwnd : *add_list) {

        title = GetWindowTitle(hwnd);
        getWindowInfo(hwnd, exename, is64);
        QVector<QString> words;

        int index = windowsExists(hwnd);
        if(index != -1) {
            // update Window data;
            App& app = m_data[index];
            app.name = title;
            app.exename = exename;
            app.is64 = is64;
            continue;
        }

        // create new Window structure
        App app;
        app.name = title;
        app.hwnd = hwnd;
        app.exename = exename;
        app.selected = false; // TODO: save/load
        app.is64 = is64;

        m_data.append(std::move(app));
    }
}

void AppList::update()
{
    QVector<HWND>* add_list = new QVector<HWND>;
    EnumWindows(getWindowsListCallback2, reinterpret_cast<LPARAM>(add_list));
    addWindows(add_list);

    emit listUpdated();
    delete add_list;
    if(m_hooks) resetHooks();
}

QString GetWindowTitle(HWND hwnd)
{
    const DWORD TITLE_SIZE = 1024;
    WCHAR windowTitle[TITLE_SIZE];

    if(::GetWindowTextW(hwnd, windowTitle, TITLE_SIZE) == 0)
        return QString("");
    QString title(QString::fromWCharArray(windowTitle));

    return title;
}

QString getWindowExeName(HWND hwnd)
{
    WCHAR fileName[MAX_PATH];
    DWORD dwPID;
    ::GetWindowThreadProcessId(hwnd, &dwPID);
    HANDLE Handle = ::OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, dwPID);
    if(!Handle) return QString("");

    ::GetModuleFileNameEx(Handle, 0, fileName, MAX_PATH);
    return QFileInfo (QString::fromWCharArray(fileName)).fileName();
}

BOOL getWindowInfo(HWND hwnd, QString &exeName, bool &is64)
{
    WCHAR fileName[MAX_PATH];
    DWORD dwPID;
    ::GetWindowThreadProcessId(hwnd, &dwPID);
    HANDLE hproc = ::OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, dwPID);
    if(!hproc) {
        exeName = QString("");
        is64 = true;
        throw std::exception("[ TASK FAILED SUCCESSFULLY ] Unable to read process.");
        return FALSE;
    }

    ::GetModuleFileNameEx(hproc, 0, fileName, MAX_PATH);
    exeName = QFileInfo(QString::fromWCharArray(fileName)).fileName();

    BOOL ret;
    BOOL isWow64;
    ret = IsWow64Process(hproc, &isWow64);
    if(!ret) {
        throw std::exception("[ TASK FAILED SUCCESSFULLY ] Could not get determie process bitness");
        return FALSE;
    }
    is64 = !isWow64;
    return TRUE;
}
