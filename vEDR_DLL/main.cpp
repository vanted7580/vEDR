
#include <windows.h>
#include <thread>
#include <sstream>
#include <iomanip>

#include "MinHook.h"
#include "logger.h"

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

thread_local bool inLogging = false;

DWORD NTAPI HookedNtAllocateVirtualMemory(
    HANDLE ProcessHandle,
    PVOID *BaseAddress,
    ULONG_PTR ZeroBits,
    PSIZE_T RegionSize,
    ULONG AllocationType,
    ULONG Protect)
{
    if (hooked && !inLogging) {

        inLogging = true;

        DWORD pid    = GetProcessId(ProcessHandle);
        DWORD tid    = GetCurrentThreadId();
        void* retAdr = _ReturnAddress();

        NTSTATUS status = pOriginalNtAllocateVirtualMemory(
            ProcessHandle, BaseAddress, ZeroBits,
            RegionSize, AllocationType, Protect);


        std::ostringstream log;

        SYSTEMTIME st; GetLocalTime(&st);
        log << "[" << std::setfill('0')
            << std::setw(2) << st.wHour << ":"
            << std::setw(2) << st.wMinute << ":"
            << std::setw(2) << st.wSecond << "] ";
        log << "PID=" << pid
            << " TID=" << tid
            << " Caller=" << retAdr
            << " BaseAddr=" << *BaseAddress
            << " Size=" << *RegionSize
            << " ZeroBits=" << ZeroBits
            << " Type=" << logger::helper::AllocTypeToString(AllocationType)
            << " Protect=" << logger::helper::ProtectToString(Protect)
            << " => Status=0x" << std::hex << status;

        if (Protect & (PAGE_EXECUTE | PAGE_EXECUTE_READ | PAGE_EXECUTE_READWRITE)) {
            log << " [EXECUTABLE]";
        }

        log << "\n";
        logger::log(log.str());

        inLogging = false;

        return status;
    }

    return pOriginalNtAllocateVirtualMemory(
        ProcessHandle, BaseAddress, ZeroBits,
        RegionSize, AllocationType, Protect);
}

void vEDRHookInit() {

    if (MH_Initialize() != MH_OK) {
        return;
    }

    logger::loggerInit();

    if (MH_CreateHookApi(L"ntdll", "NtAllocateVirtualMemory", (LPVOID *) &HookedNtAllocateVirtualMemory,
                         (LPVOID *) &pOriginalNtAllocateVirtualMemory) != MH_OK) {
        return;
    }

    if (MH_EnableHook(MH_ALL_HOOKS) != MH_OK) {
        return;
    }

    hooked = true;
}

void vEDRHookExit() {

    MH_DisableHook(MH_ALL_HOOKS);

    logger::loggerExit();

    if (MH_Uninitialize() != MH_OK) {
        return;
    }
}

BOOL APIENTRY DllMain(HMODULE hModule, DWORD ul_reason_for_call, LPVOID lpReserved) {
    switch (ul_reason_for_call) {
        case DLL_PROCESS_ATTACH: {
            DisableThreadLibraryCalls(hModule);
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
