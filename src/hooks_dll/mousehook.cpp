#include "mousehook.h"
#include <stdio.h>
#include <iostream>
#include <inttypes.h>
#include <vector>

#pragma comment(linker, "/SECTION:.SHARED,RWS")
#pragma data_seg(".SHARED")

static HWND g_hWnd = nullptr;	        // Window handle
static HHOOK g_hHook;                   // Hook handle
//static std::vector<InstalledHook> gHooksArray;
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
    if (::VirtualQuery(pv, &mbi, sizeof(mbi)) != 0) {
        return (HMODULE)mbi.AllocationBase;
    }
    else {
        return nullptr;
    }
}

extern "C" LRESULT CALLBACK MouseHookProc(int nCode, WPARAM wParam, LPARAM lParam) {
    if (nCode < 0 || nCode == HC_NOREMOVE)
        return ::CallNextHookEx(g_hHook, nCode, wParam, lParam);

    //fprintf(stderr, "######## Target hwnd: %p, Active hwnd: %p\n", t_hWnd, GetForegroundWindow());

    if(t_hWnd == GetForegroundWindow()) {
        PostMessage(g_hWnd, WM_KEYSTROKE, wParam, lParam);
    }

    return CallNextHookEx(g_hHook, nCode, wParam, lParam);
}

extern "C" LRESULT CALLBACK globalMouseHookProc(int nCode, WPARAM wParam, LPARAM lParam) {
    if (nCode < 0 || nCode == HC_NOREMOVE)
        return ::CallNextHookEx(g_hHook, nCode, wParam, lParam);

    fprintf(stderr, "######## Target hwnd: , Active hwnd: %p\n", GetForegroundWindow());

    PostMessage(g_hWnd, WM_KEYSTROKE, wParam, lParam);

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

void InstallGlobalHook(HWND hWndCaller) {
    //DWORD ThreadId;
    //ThreadId = GetWindowThreadProcessId(target, nullptr);
    fprintf(stderr, "###################### Trying to install global mouse hook\n\n");
    //``t_hWnd = target;
    g_hWnd = hWndCaller;
    g_hHook = SetWindowsHookEx(WH_MOUSE, globalMouseHookProc, ModuleFromAddress(globalMouseHookProc), 0);
}

void RemoveHooks(void) {
    UnhookWindowsHookEx(g_hHook);
    g_hHook = nullptr;
}

//void InstallHook2(HWND hWndCaller, HWND target) {
//    DWORD ThreadId;
//    ThreadId = GetWindowThreadProcessId(target, nullptr);
//    fprintf(stderr, "###################### Trying to install mouse hook\n\n");
//    fprintf(stderr, "###################### Thread  id: %lu \n\n\n", ThreadId);
//    g_hWnd = hWndCaller;
//    HHOOK hook = SetWindowsHookEx(WH_MOUSE, MouseHookProc2, ModuleFromAddress(MouseHookProc2), ThreadId);
//    InstalledHook h = {target, hook};
//    gHooksArray.push_back(h);
//}

//extern "C" LRESULT CALLBACK MouseHookProc2(int nCode, WPARAM wParam, LPARAM lParam) {
//    if (nCode < 0 || nCode == HC_NOREMOVE)
//        return ::CallNextHookEx(0, nCode, wParam, lParam);

//    fprintf(stderr, "###################### Mouse proc called\n\n");

//    auto hook = (MOUSEHOOKSTRUCT*)lParam;
//    auto target = hook->hwnd;

//    // Temporarily remove this hook
//    if(target == GetForegroundWindow()) {
//        PostMessage(g_hWnd, WM_KEYSTROKE, wParam, lParam);
//    }

//    return CallNextHookEx(0, nCode, wParam, lParam);
//}

//void RemoveHook2(HWND target) {
//    for (auto it = gHooksArray.begin(); it != gHooksArray.end(); it++ ) {
//        InstalledHook h = (*it);
//        HWND hwnd = (HWND)h.hWnd;
//        if(hwnd == target) {
//            HHOOK hook;
//            hook = h.hook;
//            fprintf(stderr, "###################### Trying to remove mouse hook\n\n");
//            fprintf(stderr, "###################### Thread hook: %p \n\n\n", hook);
//            UnhookWindowsHookEx((*it).hook);
//            gHooksArray.erase(it);
//            return;
//        }
//    }
//}
