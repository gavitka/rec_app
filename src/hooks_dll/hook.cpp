#include "hook.h"

#pragma comment(linker, "/SECTION:.SHARED,RWS")
#pragma data_seg(".SHARED")

static HWND g_callerHWND = NULL;
static HHOOK g_hook;

#pragma data_seg()

BOOL APIENTRY DllMain(HMODULE hModule, DWORD ul_reason_for_call, LPVOID lpReserved)
{
    (void)lpReserved;
    (void)hModule;
    (void)ul_reason_for_call;

    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
        break;
    case DLL_PROCESS_DETACH:
        break;
    case DLL_THREAD_ATTACH:
        break;
    case DLL_THREAD_DETACH:
        break;
    }

    // Pumping messages, not sure if i actually need this
    MSG msg;
    while(true) {
        if( !PeekMessage( &msg, 0, 0, 0, PM_REMOVE ) ) {
            return TRUE;
        }
        if( msg.message == WM_QUIT ) {
            return TRUE;
        }
        TranslateMessage( &msg );
        DispatchMessage( &msg );
    }

    return TRUE;
}

LRESULT CALLBACK HookProc(int nCode, WPARAM wParam, LPARAM lParam)
{
    if (nCode < 0 || nCode == HC_NOREMOVE) {
        return ::CallNextHookEx(NULL, nCode, wParam, lParam);
    }
    HWND hwnd = g_callerHWND;
    if(hwnd != NULL)
        PostMessage(hwnd, WM_KEYSTROKE, wParam, lParam);
    return ::CallNextHookEx(NULL, nCode, wParam, lParam);
}


HMODULE WINAPI ModuleFromAddress(PVOID pv)
{
    MEMORY_BASIC_INFORMATION mbi;
    if (::VirtualQuery(pv, &mbi, sizeof(mbi)) != 0) {
        return (HMODULE)mbi.AllocationBase;
    }
    else return nullptr;
}

void InstallGlobalHookDll(HWND hwnd)
{
    // It actually puts hook on itself, but it works for global events for some reason,
    // probably need to install LL hook instead
    g_callerHWND = hwnd;
    g_hook = ::SetWindowsHookEx(WH_MOUSE, HookProc, ModuleFromAddress((HOOKPROC*)HookProc), 0);
}

void UninstallGlobalHookDll()
{
    ::UnhookWindowsHookEx(g_hook);
}

