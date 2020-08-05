#include "mousehook.h"

#include <stdio.h>
#include <iostream>
#include <inttypes.h>
#include <vector>

#include <QDebug>
#include <QString>

#include "../lib.h"

#pragma comment(linker, "/SECTION:.SHARED,RWS")
#pragma data_seg(".SHARED")

static HWND g_callerHWND = NULL;
static HHOOK g_hook = NULL;

#pragma data_seg()

static HMODULE hInstance = nullptr;

BOOL APIENTRY DllMain(HMODULE hModule, DWORD  ul_reason_for_call, LPVOID lpReserved)
{
    (void)lpReserved;
    (void)hModule;
    qInstallMessageHandler(myMessageHandler);

    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
        qDebug() << "Attached to process ";
        report();
        hInstance = (HINSTANCE)hModule;
        break;
    case DLL_PROCESS_DETACH:
        qDebug() << "Detached from process ";
        report();
        break;
    case DLL_THREAD_ATTACH:
        qDebug() << "Attached to thread ";
        report();
        break;
    case DLL_THREAD_DETACH:
        qDebug() << "Detached from thread ";
        report();
        break;
    }

    return TRUE;
}


LRESULT CALLBACK MultiHookProc(int nCode, WPARAM wParam, LPARAM lParam)
{
    if (nCode < 0 || nCode == HC_NOREMOVE) {
        return ::CallNextHookEx(NULL, nCode, wParam, lParam);
    }
    //qDebug() << "MultiHookProc ";
    //report();
    if(g_callerHWND != NULL)
        PostMessage(g_callerHWND, WM_KEYSTROKE, wParam, lParam);
    return ::CallNextHookEx(NULL, nCode, wParam, lParam);
}


void setGlobalHwnd(HWND hwnd)
{
    g_callerHWND = hwnd;
}


HHOOK InstallGlobalHookDll(HWND hwndCaller) {
    g_callerHWND = hwndCaller;
    // It actually puts hook on itself, but it works for global events for some reason,
    // probably need to install LL hook instead
    g_hook = ::SetWindowsHookEx(WH_MOUSE, MultiHookProc, ModuleFromAddress((HOOKPROC*)MultiHookProc), 0);
    return g_hook;
}


//void UninstallGlobalHook() {
//    BOOL ret = ::UnhookWindowsHookEx(g_hook);
//    if(!ret) throw std::exception("[ TASK FAILED SUCCESSFULLY ] Could not remove the hook.");
//}

void report() {
    TCHAR szExeFileName[MAX_PATH];
    GetModuleFileName(NULL, szExeFileName, MAX_PATH);

    std::wstring exeName = szExeFileName;
    qint64 pos = exeName.find_last_of(L"\\");
    exeName = exeName.substr(pos + 1, exeName.length());

    qDebug() << "exe name: " << exeName
             << " process id: " << GetCurrentProcessId();
}


HMODULE WINAPI ModuleFromAddress(PVOID pv)
{
    MEMORY_BASIC_INFORMATION mbi;
    if (::VirtualQuery(pv, &mbi, sizeof(mbi)) != 0) {
        return (HMODULE)mbi.AllocationBase;
    }
    else return nullptr;
}
