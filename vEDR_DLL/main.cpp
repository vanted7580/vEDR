#include <windows.h>
#include <iostream>

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

DWORD NTAPI HookedNtAllocateVirtualMemory(
    HANDLE ProcessHandle,
    PVOID *BaseAddress,
    ULONG_PTR ZeroBits,
    PSIZE_T RegionSize,
    ULONG AllocationType,
    ULONG Protect) {
    if (hooked) {
        switch (Protect) {
            case PAGE_EXECUTE_READWRITE: {
                std::cout << "Hooked NtAllocateVirtualMemory::PAGE_EXECUTE_READWRITE [Rejected]" << std::endl;
                return 0;
            }
            default: {
                std::cout << "Hooked NtAllocateVirtualMemory::" << Protect << " [Allowed]" << std::endl;
            }
        }
    }

    return pOriginalNtAllocateVirtualMemory(ProcessHandle, BaseAddress, ZeroBits, RegionSize, AllocationType, Protect);
}

void vEDRHookInit() {
    if (MH_Initialize() != MH_OK) {
        std::cout << "Failed to initialize MinHook" << std::endl;
        return;
    }

    if (MH_CreateHookApi(L"ntdll", "NtAllocateVirtualMemory", (LPVOID *) &HookedNtAllocateVirtualMemory,
                         (LPVOID *) &pOriginalNtAllocateVirtualMemory) != MH_OK) {
        std::cout << "Failed to hook NtAllocateVirtualMemory" << std::endl;
        return;
    }

    if (MH_EnableHook(MH_ALL_HOOKS) != MH_OK) {
        return;
    }

    hooked = true;

    std::cout << "Successfully initialized MinHook" << std::endl;
}

void vEDRHookExit() {
    MH_DisableHook(MH_ALL_HOOKS);

    if (MH_Uninitialize() != MH_OK) {
        std::cout << "Failed to uninitialize MinHook" << std::endl;
        return;
    }

    std::cout << "Successfully uninitialized MinHook" << std::endl;
}

BOOL APIENTRY DllMain(HANDLE hModule, DWORD ul_reason_for_call, LPVOID lpReserved) {
    switch (ul_reason_for_call) {
        case DLL_PROCESS_ATTACH: {
            std::cout << "Dll attached!" << std::endl;
            vEDRHookInit();
        }
        case DLL_THREAD_ATTACH:
        case DLL_THREAD_DETACH:
        default:
            break;
        case DLL_PROCESS_DETACH: {
            std::cout << "Dll dettached!" << std::endl;
            vEDRHookExit();
        }
    }
    return TRUE;
}
