#pragma once
#include <Windows.h>
#include <vector>

#include <QString>

#ifdef MOUSEHOOK_EXPORTS
#define MOUSEHOOK_API __declspec(dllexport)
#else
#define MOUSEHOOK_API __declspec(dllimport)
#endif

#define WM_KEYSTROKE (WM_USER + 101)

BOOL APIENTRY DllMain(HMODULE hModule,
    DWORD  ul_reason_for_call,
    LPVOID lpReserved
);

extern "C" MOUSEHOOK_API LRESULT CALLBACK MultiHookProc(int nCode, WPARAM wParam, LPARAM lParam);

extern "C" MOUSEHOOK_API void setGlobalHwnd(HWND hwnd);

MOUSEHOOK_API void InstallGlobalHook(HWND hwndCaller);

MOUSEHOOK_API void UninstallGlobalHook();

void report();

HMODULE WINAPI ModuleFromAddress(PVOID pv);
