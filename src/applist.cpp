#include "applist.h"

BOOL CALLBACK getWindowsListCallback2(HWND hwnd, LPARAM lParam) {
    const DWORD TITLE_SIZE = 1024;
    WCHAR windowTitle[TITLE_SIZE];

    GetWindowTextW(hwnd, windowTitle, TITLE_SIZE);

    int length = ::GetWindowTextLength(hwnd);
    std::wstring title(&windowTitle[0]);
    if (!IsWindowVisible(hwnd) || length == 0 || title == L"Program Manager") {
        return TRUE;
    }

    QVector<App>& dataList = *reinterpret_cast<QVector<App>*>(lParam);
    dataList.append({QString::fromStdWString(windowTitle), hwnd});

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
}

void AppList::updateVector(std::vector<HWND> *vector)
{
    qDebug() << "request update windows";
    if(!vector) return;
    vector->clear();
    for(int i = 0; i < m_data.size(); ++i) {
        if(m_data[i].selected) {
            HWND hwnd = m_data[i].hWnd;
            vector->push_back(hwnd);
        }
    }
}

void AppList::update()
{
    if(m_data.size() != 0) m_data.clear();
    EnumWindows(getWindowsListCallback2, reinterpret_cast<LPARAM>(&m_data));
    emit listUpdated();
}
