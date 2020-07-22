#pragma once
#include <Windows.h>

#ifdef MOUSEHOOK_EXPORTS
#define MOUSEHOOK_API __declspec(dllexport)
#else
#define MOUSEHOOK_API __declspec(dllimport)
#endif

#define WM_KEYSTROKE (WM_USER + 101)

struct InstalledHook {
    HWND hWnd;
    HHOOK hook;
};

BOOL APIENTRY DllMain(HMODULE hModule,
    DWORD  ul_reason_for_call,
    LPVOID lpReserved
);

extern "C" MOUSEHOOK_API LRESULT CALLBACK MouseHookProc(int nCode, WPARAM wParam, LPARAM lParam);

extern "C" MOUSEHOOK_API LRESULT CALLBACK globalMouseHookProc(int nCode, WPARAM wParam, LPARAM lParam);

MOUSEHOOK_API void InstallMouseHook(HWND hWndCaller, HWND target);

MOUSEHOOK_API void InstallGlobalHook(HWND hWndCaller);

MOUSEHOOK_API void RemoveHooks(void);

//extern "C" MOUSEHOOK_API LRESULT CALLBACK MouseHookProc2(int nCode, WPARAM wParam, LPARAM lParam);

//MOUSEHOOK_API void InstallHook2(HWND hWndCaller, HWND target);

//MOUSEHOOK_API void RemoveHook2(HWND target);
