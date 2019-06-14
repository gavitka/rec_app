#include "mousehook.h"
#include <stdio.h>
#include <iostream>
#include <inttypes.h>

#pragma comment(linker, "/SECTION:.SHARED,RWS")
#pragma data_seg(".SHARED")

static HWND g_hWnd = nullptr;	        // Window handle
static HHOOK g_hHook = nullptr;         // Hook handle
static HWND t_hWnd = nullptr;

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
    if (::VirtualQuery(pv, &mbi, sizeof(mbi)) != 0)
    {
        return (HMODULE)mbi.AllocationBase;
    }
    else
    {
        return nullptr;
    }
}

extern "C" LRESULT CALLBACK MouseHookProc(int nCode, WPARAM wParam, LPARAM lParam) {
    if (nCode < 0 || nCode == HC_NOREMOVE)
        return ::CallNextHookEx(g_hHook, nCode, wParam, lParam);

    //fprintf(stderr, "######## Target hwnd: %p, Active hwnd: %p\n", t_hWnd, GetForegroundWindow());

    if(t_hWnd == GetForegroundWindow())
    {
        PostMessage(g_hWnd, WM_KEYSTROKE, wParam, lParam);
    }

    return CallNextHookEx(g_hHook, nCode, wParam, lParam);
}

void InstallMouseHook(HWND hWndCaller, HWND target) {
    DWORD ThreadId;
    ThreadId = GetWindowThreadProcessId(target, nullptr);
    fprintf(stderr, "###################### Trying to install mouse hook\n\n");
    fprintf(stderr, "###################### Thread id: %lu \n\n\n", ThreadId);
    t_hWnd = target;
    g_hWnd = hWndCaller;
    g_hHook = SetWindowsHookEx(WH_MOUSE, MouseHookProc, ModuleFromAddress(MouseHookProc), ThreadId);
}

void RemoveHooks(void) {
    UnhookWindowsHookEx(g_hHook);
    g_hHook = nullptr;
}
