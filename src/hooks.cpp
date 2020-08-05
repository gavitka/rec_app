#include "hooks.h"
#include <QFile>

typedef void (__stdcall *f_funci)(HWND hwnd);

extern HWND g_hwnd;

std::vector<HOOKDATA> g_hooks = std::vector<HOOKDATA>();


void InstallHook(HWND hwnd)
{
    BOOL ret;

    DWORD pid = NULL;
    DWORD tid = GetWindowThreadProcessId(hwnd, &pid);
    if (tid == NULL)
        throw std::exception("[ TASK FAILED SUCCESSFULLY ] Could not get thread ID of the target window.");

    HANDLE hproc = OpenProcess(PROCESS_QUERY_INFORMATION, FALSE, pid);
    if (hproc == NULL)
        throw std::exception("[ TASK FAILED SUCCESSFULLY ] Failed to open process.");

    BOOL isWow64;
    ret = IsWow64Process(hproc, &isWow64);
    if(!ret)
        throw std::exception("[ TASK FAILED SUCCESSFULLY ] Could not get determie process bitness");

    if(isWow64)
        throw std::exception("[ TASK FAILED SUCCESSFULLY ] It's a 32bit process");

    HMODULE dll = LoadLibraryEx(L"hooks_dll.dll", NULL, DONT_RESOLVE_DLL_REFERENCES);
    if (dll == NULL)
        throw std::exception("[ TASK FAILED SUCCESSFULLY ] The DLL could not be found.");

    HOOKPROC addr = (HOOKPROC)GetProcAddress(dll, "MultiHookProc");
    if (addr == NULL)
        throw std::exception("[ TASK FAILED SUCCESSFULLY ] The function was not found.");

    f_funci setGlobalHwnd = (f_funci)GetProcAddress(dll, "setGlobalHwnd");
    if(!setGlobalHwnd)
        throw std::exception("[ TASK FAILED SUCCESSFULLY ] The function setGlobalHwnd was not found.");

    setGlobalHwnd(g_hwnd);

    HHOOK handle = SetWindowsHookEx(WH_MOUSE, addr, dll, tid);
    if (handle == NULL)
        throw std::exception("[ TASK FAILED SUCCESSFULLY ] Couldn't set the hook with SetWindowsHookEx.");

    g_hooks.push_back({dll, handle});
}


void InstallGlobalHook()
{
//    //temporarily disabled
//    HHOOK handle = ::InstallGlobalHookDll(g_hwnd);
//    if (handle == NULL)
//        throw std::exception("[ TASK FAILED SUCCESSFULLY ] Couldn't set the hook with SetWindowsHookEx.");

//    g_hooks.push_back({NULL, handle});
}


void UninstallHooks()
{
    for(HOOKDATA d : g_hooks) {
        BOOL ret;

        ret = UnhookWindowsHookEx(d.hhook);
        if (ret == FALSE)
            throw std::exception("[ TASK FAILED SUCCESSFULLY ] Could not remove the hook.");

        if(d.hdll) {
            ret = FreeLibrary(d.hdll);
            if (ret == FALSE)
                throw std::exception("[ TASK FAILED SUCCESSFULLY ] Could not unload dll.");
        }
    }

    g_hooks.clear();
}
