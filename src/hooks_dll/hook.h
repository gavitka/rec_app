#pragma once
#include "pch.h"

#ifdef DLLEXPORT
#define DLL_API __declspec(dllexport)
#else
#define DLL_API __declspec(dllimport)
#endif

#define WM_KEYSTROKE (WM_USER + 101)

BOOL APIENTRY DllMain(HMODULE hModule, DWORD ul_reason_for_call, LPVOID lpReserved);

extern "C" DLL_API LRESULT CALLBACK HookProc(int nCode, WPARAM wParam, LPARAM lParam);

extern "C" DLL_API void InstallGlobalHookDll(HWND hwnd);

extern "C" DLL_API void UninstallGlobalHookDll();
