#include "hooks.h"

typedef int (__stdcall *f_funci)(HWND hwnd);

std::vector<std::pair<HWND, HHOOK>> g_hookList;

extern HWND g_hwnd;


void InstallHook(HWND hwnd)
{
    return;

    // Getting the thread of the window and the PID
    DWORD pid = NULL;
    DWORD tid = GetWindowThreadProcessId(hwnd, &pid);
    if (tid == NULL) throw std::exception("[ FAILED ] Could not get thread ID of the target window.");

    // Loading DLL
    HMODULE dll = LoadLibraryEx(L"hooks_dll.dll", NULL, DONT_RESOLVE_DLL_REFERENCES);
    if (dll == NULL) throw std::exception("[ FAILED ] The DLL could not be found.");

    // Getting exported function address
    HOOKPROC addr = (HOOKPROC)GetProcAddress(dll, "MultiHookProc2");
    // TODO: set g_hwnd in this module
    if (addr == NULL) throw std::exception("[ FAILED ] The function was not found.");

    f_funci setGlobalHwnd = (f_funci)GetProcAddress(dll, "setGlobalHwnd");
    if(!setGlobalHwnd) throw std::exception("[ FAILED ] The function was not found.");

    setGlobalHwnd(g_hwnd);

    // Setting the hook in the hook chain
    HHOOK handle = SetWindowsHookEx(WH_MOUSE, addr, dll, tid); // Or WH_KEYBOARD if you prefer to trigger the hook manually
    if (handle == NULL) throw std::exception("[ FAILED ] Couldn't set the hook with SetWindowsHookEx.");

    g_hookList.push_back(std::pair<HWND, HHOOK>(hwnd, handle));
}


void UninstallHook(HWND hwnd)
{
    return;
    std::vector<std::pair<HWND, HHOOK>>::iterator it;
    it = g_hookList.begin();

    for(std::vector<std::pair<HWND, HHOOK>>::iterator it = g_hookList.begin(); it != g_hookList.end(); ) {
        if((*it).first == hwnd) {
            if(::IsWindow(hwnd)) {
                BOOL unhook = UnhookWindowsHookEx((*it).second);
                if (unhook == FALSE) throw std::exception("[ FAILED ] Could not remove the hook.");
            }
            g_hookList.erase(it);
        } else {
            ++it;
        }
    }

    throw std::exception("[ FAILED ] Hook not found");
}
