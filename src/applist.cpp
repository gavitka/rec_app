#include <QDir>

#include "applist.h"
#include "psapi.h"


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
    emit selectedChanged();
}

// updates vector of selected items upon request
void AppList::updateVector(std::vector<HWND> *vector)
{
    if(vector == nullptr) return;
    vector->clear();
    for(int i = 0; i < m_data.size(); ++i)
        if(m_data[i].selected) {
            HWND hwnd = m_data[i].hwnd;
            vector->push_back(hwnd);
        }
}

bool AppList::isSelected()
{
    for(auto w : m_data) {
        if(w.selected == true) return true;
    }
    return false;
}

int AppList::windowsExists(HWND hwnd)
{
    QString title = GetWindowTitle(hwnd);

    // TODO: better check for existing windows
    for(int i = 0; i < m_data.size(); ++i) {
        if(m_data[i].name == title ||
               m_data[i].hwnd == hwnd) {
            return i;
        }
    }
    return -1;
}

void AppList::addWindows(QVector<HWND>* add_list)
{
    QString title;
    QString exename;

    // Removing empty windows
    QMutableVectorIterator<HWND> it(*add_list);
    while(it.hasNext()) {
        HWND hwnd = it.next();
        title = GetWindowTitle(hwnd);
        if (!IsWindowVisible(hwnd)
                || title.length() == 0
                || title == L"Program Manager"
                || title == "Time lapse recording app") {
            it.remove();
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
        exename = getWindowExeName(hwnd);
        QVector<QString> words;

        int index = windowsExists(hwnd);
        if(index != -1) {
            // update Window data;
            App& app = m_data[index];
            app.name = title;
            app.exename = exename;
            continue;
        }

        // create new Window structure
        App app;
        app.name = title;
        app.hwnd = hwnd;
        app.exename = exename;
        app.selected = false; // TODO: save/load

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
    GetWindowThreadProcessId(hwnd, &dwPID);
    HANDLE Handle = ::OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, dwPID);
    if(!Handle) return QString("");

    ::GetModuleFileNameEx(Handle, 0, fileName, MAX_PATH);
    return QFileInfo (QString::fromWCharArray(fileName)).fileName();
}
