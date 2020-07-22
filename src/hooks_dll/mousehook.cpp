#include "mousehook.h"
#include <stdio.h>
#include <iostream>
#include <inttypes.h>
#include <vector>

#include <QDebug>

#pragma comment(linker, "/SECTION:.SHARED,RWS")
#pragma data_seg(".SHARED")

static HWND g_hWnd = nullptr;
static HHOOK g_hHook;
static std::vector<HWND>* g_targets;

#pragma data_seg()

static HMODULE hInstance = nullptr;

BOOL APIENTRY DllMain(HMODULE hModule, DWORD  ul_reason_for_call, LPVOID lpReserved) {
    (void)lpReserved;
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
        hInstance = (HINSTANCE)hModule;
        break;
    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
    case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;
}

// Get module from address
HMODULE WINAPI ModuleFromAddress(PVOID pv) {
    MEMORY_BASIC_INFORMATION mbi;
    if (::VirtualQuery(pv, &mbi, sizeof(mbi)) != 0) {
        return (HMODULE)mbi.AllocationBase;
    }
    else {
        return nullptr;
    }
}

LRESULT CALLBACK MultiHookProc(int nCode, WPARAM wParam, LPARAM lParam)
{
    if (nCode < 0 || nCode == HC_NOREMOVE) {
        return CallNextHookEx(g_hHook, nCode, wParam, lParam);
    }
    if(g_targets->size() == 0) {
        PostMessage(g_hWnd, WM_KEYSTROKE, wParam, lParam);
    }
    for(auto t : *g_targets) {
        if((HWND)t == GetForegroundWindow())
        {
            qDebug() << "kicking";
            PostMessage(g_hWnd, WM_KEYSTROKE, wParam, lParam);
        }
    }

    return CallNextHookEx(g_hHook, nCode, wParam, lParam);
}

void InstallMultiHook(HWND hwndCaller, std::vector<HWND>* targets)
{
    g_targets = targets;
    g_hWnd = hwndCaller;
    g_hHook = SetWindowsHookEx(WH_MOUSE, MultiHookProc, ModuleFromAddress((HOOKPROC*)MultiHookProc), 0);
}

void UninstallMultiHook()
{
    UnhookWindowsHookEx(g_hHook);
}
