#include <windows.h>
#include <iostream>
#include <string>
#include <thread>

#include "MinHook.h"

bool hooked = false;

typedef DWORD (NTAPI*pNtAllocateVirtualMemory)(
    HANDLE ProcessHandle,
    PVOID *BaseAddress,
    ULONG_PTR ZeroBits,
    PSIZE_T RegionSize,
    ULONG AllocationType,
    ULONG Protect
);

pNtAllocateVirtualMemory pOriginalNtAllocateVirtualMemory = nullptr;

HANDLE hPipe = INVALID_HANDLE_VALUE;

DWORD NTAPI HookedNtAllocateVirtualMemory(
    HANDLE ProcessHandle,
    PVOID *BaseAddress,
    ULONG_PTR ZeroBits,
    PSIZE_T RegionSize,
    ULONG AllocationType,
    ULONG Protect) {
    if (hooked) {
        DWORD pid = GetProcessId(ProcessHandle);

        DWORD dwWritten;

        switch (Protect) {
            case PAGE_EXECUTE_READWRITE: {
                //std::cerr << "NtAllocateVirtualMemory::PAGE_EXECUTE_READWRITE (pid " << std::to_string(pid) << ")" <<  std::endl;
                //return STATUS_ACCESS_DENIED;
                std::string message = "NtAllocateVirtualMemory::PAGE_EXECUTE_READWRITE";
                WriteFile(hPipe, message.c_str(), message.length(), &dwWritten, nullptr);
                break;
            }
            case PAGE_READWRITE: {
                //std::cerr << "NtAllocateVirtualMemory::PAGE_READWRITE (pid " << std::to_string(pid) << ")" <<  std::endl;
                //return STATUS_ACCESS_DENIED;
                break;
            }
            default: {
                //std::cerr << "NtAllocateVirtualMemory::" << Protect << " (pid " << std::to_string(pid) << ")" <<  std::endl;
                break;
            }
        }
    }

    return pOriginalNtAllocateVirtualMemory(ProcessHandle, BaseAddress, ZeroBits, RegionSize, AllocationType, Protect);
}

void vEDRHookInit() {

    if (hPipe == INVALID_HANDLE_VALUE) {
        hPipe = CreateFile(TEXT("\\\\.\\pipe\\vEDR_Pipe"),
            GENERIC_WRITE,
            0, nullptr, OPEN_EXISTING, 0, nullptr);
        if (hPipe == INVALID_HANDLE_VALUE) {
            return;
        }
    }

    std::string message = "Test message";

    DWORD dwWritten;

    WriteFile(hPipe, message.c_str(), message.length(), &dwWritten, nullptr);

    if (MH_Initialize() != MH_OK) {
        // std::cout << "Failed to initialize MinHook" << std::endl;
        return;
    }

    if (MH_CreateHookApi(L"ntdll", "NtAllocateVirtualMemory", (LPVOID *) &HookedNtAllocateVirtualMemory,
                         (LPVOID *) &pOriginalNtAllocateVirtualMemory) != MH_OK) {
        // std::cout << "Failed to hook NtAllocateVirtualMemory" << std::endl;
        return;
    }

    if (MH_EnableHook(MH_ALL_HOOKS) != MH_OK) {
        return;
    }

    hooked = true;

    // std::cout << "Successfully initialized MinHook" << std::endl;
}

void vEDRHookExit() {
    MH_DisableHook(MH_ALL_HOOKS);

    if (hPipe != INVALID_HANDLE_VALUE) {
        CloseHandle(hPipe);
    }

    if (MH_Uninitialize() != MH_OK) {
        // std::cout << "Failed to uninitialize MinHook" << std::endl;
        return;
    }

    // std::cout << "Successfully uninitialized MinHook" << std::endl;
}

BOOL APIENTRY DllMain(HMODULE hModule, DWORD ul_reason_for_call, LPVOID lpReserved) {
    switch (ul_reason_for_call) {
        case DLL_PROCESS_ATTACH: {
            DisableThreadLibraryCalls(hModule); // 防止多余通知
            vEDRHookInit();
        }
        case DLL_THREAD_ATTACH:
        case DLL_THREAD_DETACH:
        default:
            break;
        case DLL_PROCESS_DETACH: {
            vEDRHookExit();
        }
    }
    return TRUE;
}
