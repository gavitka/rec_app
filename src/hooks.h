#include "pch.h"
#include "hooks_dll/mousehook.h"

typedef struct _HOOKDADA {
    HMODULE hdll;
    HHOOK hhook;
} HOOKDATA;

void InstallHook(HWND hwnd);

void InstallGlobalHook();

void UninstallHooks();
