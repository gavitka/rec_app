#include "pch.h"
#include "hooks_dll/mousehook.h"

typedef struct _HOOKDADA {
    HMODULE hdll;
    HHOOK hhook;
} HOOKDATA;

HHOOK InstallHook(HWND hwnd);

//HHOOK InstallGlobalHook();

//void UninstallHook(HHOOK hook);

void UninstallHooks();
