
// Copyright (C) 2006 Michael Eddington <meddington@gmail.com>
// $Id$

#include "stdafx.h"

#include <stdio.h>
#include <windows.h>
#include "detours.h"
#include "syelog.h"

#include "duma.h"

#pragma warning(disable:4100)   // Trampolines don't use formal parameters.

//////////////////////////////////////////////////////////////////////////////

static BOOL bInternal = TRUE;

DETOUR_TRAMPOLINE(HANDLE WINAPI Real_HeapCreate(DWORD flOptions, SIZE_T dwInitialSize, SIZE_T dwMaximumSize), 
				  HeapCreate);

HANDLE WINAPI My_HeapCreate( DWORD flOptions, SIZE_T dwInitialSize, SIZE_T dwMaximumSize )
{
	Syelog(SYELOG_SEVERITY_INFORMATION, "My_HeadCreate");
	// Duma will handle this
    //HANDLE pvRet = Real_HeapCreate(flOptions, dwInitialSize, dwMaximumSize);
    //return pvRet;
	return (HANDLE)1;
}

DETOUR_TRAMPOLINE(BOOL WINAPI Real_HeapDestroy(HANDLE hHeap),
                  HeapDestroy);

BOOL WINAPI My_HeapDestroy(HANDLE hHeap)
{
	Syelog(SYELOG_SEVERITY_INFORMATION, "My_HeapDestroy");
	// Duma will do this magic
    //BOOL pvRet = Real_HeapDestroy(hHeap);
    //return pvRet;
	return TRUE;
}

DETOUR_TRAMPOLINE(BOOL WINAPI Real_HeapFree(HANDLE hHeap, DWORD dwFlags, LPVOID lpMem),
                  HeapFree);

BOOL WINAPI My_HeapFree(HANDLE hHeap, DWORD dwFlags, LPVOID lpMem)
{
	Syelog(SYELOG_SEVERITY_INFORMATION, "My_HeapFree %x, %x", hHeap, lpMem);
    //BOOL pvRet = Real_HeapFree(hHeap, dwFlags, lpMem);
	_duma_free(lpMem, __FILE__, __LINE__);
    return TRUE;
}

DETOUR_TRAMPOLINE(LPVOID WINAPI Real_HeapAlloc(HANDLE Heap, DWORD Flags, DWORD Bytes),
                  HeapAlloc);

LPVOID WINAPI My_HeapAlloc(HANDLE hHeap, DWORD dwFlags, DWORD dwBytes)
{
	Syelog(SYELOG_SEVERITY_INFORMATION, "My_HeapAlloc");
    //return = Real_HeapAlloc(hHeap, dwFlags, dwBytes);
    return _duma_malloc(dwBytes, __FILE__, __LINE__);
}

DETOUR_TRAMPOLINE(LPVOID WINAPI Real_HeapReAlloc(HANDLE hHeap, DWORD dwFlags, LPVOID lpMem, SIZE_T dwBytes),
                  HeapReAlloc);

LPVOID WINAPI My_HeapReAlloc(HANDLE hHeap, DWORD dwFlags, LPVOID lpMem, SIZE_T dwBytes)
{
	Syelog(SYELOG_SEVERITY_INFORMATION, "My_HeapReAlloc");
    //LPVOID pvRet = Real_HeapReAlloc(hHeap, dwFlags, lpMem, dwBytes);
    return _duma_realloc(lpMem, dwBytes, __FILE__, __LINE__);
}

///////////////////////////////////////////////////////////////////////////

int myonexit(void);

int myonexit(void)
{
	Syelog(SYELOG_SEVERITY_INFORMATION, "myonexit");
	_duma_exit();
	return TRUE;
}

BOOL WINAPI DllMain(HINSTANCE hinst, DWORD dwReason, LPVOID reserved)
{
    if (dwReason == DLL_PROCESS_ATTACH)
	{
        BOOL bOk;
        PBYTE pbRealTrampoline;
        PBYTE pbRealTarget;

		SyelogOpen("duma", SYELOG_FACILITY_APPLICATION);
		duma_init();

		_onexit(myonexit);

		Syelog(SYELOG_SEVERITY_INFORMATION, "Installing!");

        bOk = DetourFunctionWithTrampolineEx((PBYTE)Real_HeapCreate, (PBYTE)My_HeapCreate,
                                             &pbRealTrampoline, &pbRealTarget);
        bOk = DetourFunctionWithTrampolineEx((PBYTE)Real_HeapDestroy, (PBYTE)My_HeapDestroy,
                                             &pbRealTrampoline, &pbRealTarget);
        bOk = DetourFunctionWithTrampolineEx((PBYTE)Real_HeapFree, (PBYTE)My_HeapFree,
                                             &pbRealTrampoline, &pbRealTarget);
        bOk = DetourFunctionWithTrampolineEx((PBYTE)Real_HeapAlloc, (PBYTE)My_HeapAlloc,
                                             &pbRealTrampoline, &pbRealTarget);
        bOk = DetourFunctionWithTrampolineEx((PBYTE)Real_HeapReAlloc, (PBYTE)My_HeapReAlloc,
                                             &pbRealTrampoline, &pbRealTarget);

        bInternal = FALSE;
    }
    else if (dwReason == DLL_PROCESS_DETACH)
	{
		Syelog(SYELOG_SEVERITY_INFORMATION, "DLL_PROCESS_DETACH");
        bInternal = TRUE;
        DetourRemove((PBYTE)Real_HeapCreate, (PBYTE)My_HeapCreate);
        DetourRemove((PBYTE)Real_HeapDestroy, (PBYTE)My_HeapDestroy);
        DetourRemove((PBYTE)Real_HeapFree, (PBYTE)My_HeapFree);
        DetourRemove((PBYTE)Real_HeapAlloc, (PBYTE)My_HeapAlloc);
        DetourRemove((PBYTE)Real_HeapReAlloc, (PBYTE)My_HeapReAlloc);
		_duma_exit();
		SyelogClose();
    }

    return TRUE;
}

extern "C" {
    //  Trampolines for SYELOG library.
    //
    DETOUR_TRAMPOLINE(HANDLE WINAPI
                      Real_CreateFileW(LPCWSTR a0, DWORD a1, DWORD a2,
                                       LPSECURITY_ATTRIBUTES a3, DWORD a4, DWORD a5,
                                       HANDLE a6),
                      CreateFileW);

    DETOUR_TRAMPOLINE(BOOL WINAPI
                      Real_WriteFile(HANDLE hFile,
                                     LPCVOID lpBuffer,
                                     DWORD nNumberOfBytesToWrite,
                                     LPDWORD lpNumberOfBytesWritten,
                                     LPOVERLAPPED lpOverlapped),
                      WriteFile);
    DETOUR_TRAMPOLINE(BOOL WINAPI
                      Real_FlushFileBuffers(HANDLE hFile),
                      FlushFileBuffers);
    DETOUR_TRAMPOLINE(BOOL WINAPI
                      Real_CloseHandle(HANDLE hObject),
                      CloseHandle);

    DETOUR_TRAMPOLINE(BOOL WINAPI
                      Real_WaitNamedPipeW(LPCWSTR lpNamedPipeName, DWORD nTimeOut),
                      WaitNamedPipeW);
    DETOUR_TRAMPOLINE(BOOL WINAPI
                      Real_SetNamedPipeHandleState(HANDLE hNamedPipe,
                                                   LPDWORD lpMode,
                                                   LPDWORD lpMaxCollectionCount,
                                                   LPDWORD lpCollectDataTimeout),
                      SetNamedPipeHandleState);

    DETOUR_TRAMPOLINE(DWORD WINAPI
                      Real_GetCurrentProcessId(VOID),
                      GetCurrentProcessId);
    DETOUR_TRAMPOLINE(VOID WINAPI
                      Real_GetSystemTimeAsFileTime(LPFILETIME lpSystemTimeAsFileTime),
                      GetSystemTimeAsFileTime);

    DETOUR_TRAMPOLINE(VOID WINAPI
                      Real_InitializeCriticalSection(LPCRITICAL_SECTION lpSection),
                      InitializeCriticalSection);
    DETOUR_TRAMPOLINE(VOID WINAPI
                      Real_EnterCriticalSection(LPCRITICAL_SECTION lpSection),
                      EnterCriticalSection);
    DETOUR_TRAMPOLINE(VOID WINAPI
                      Real_LeaveCriticalSection(LPCRITICAL_SECTION lpSection),
                      LeaveCriticalSection);
}

// end