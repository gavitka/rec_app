#pragma once
#include <Windows.h>
#include <vector>

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

MOUSEHOOK_API void InstallMultiHook(HWND hwndCaller);

MOUSEHOOK_API void UninstallMultiHook();

MOUSEHOOK_API void UpdateWindowsList(std::vector<HWND>* vector);

//extern "C" MOUSEHOOK_API LRESULT CALLBACK MultiHookProc2(int nCode, WPARAM wParam, LPARAM lParam);
