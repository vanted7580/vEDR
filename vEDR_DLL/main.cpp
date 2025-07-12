#include <windows.h>
#include <iostream>

#include "MinHook.h"

void vEDRHookInit() {
    const MH_STATUS status = MH_Initialize();
    if (status != MH_OK) {
        std::cerr << "Failed to initialize MinHook" << std::endl;
        return;
    }


    std::cerr << "Successfully initialized MinHook" << std::endl;
}

void vEDRHookExit() {
    const MH_STATUS status = MH_Uninitialize();
    if (status != MH_OK) {
        std::cerr << "Failed to uninitialize MinHook" << std::endl;
        return;
    }
    std::cerr << "Successfully uninitialized MinHook" << std::endl;
}

BOOL APIENTRY DllMain(HANDLE hModule, DWORD ul_reason_for_call, LPVOID lpReserved) {
    switch (ul_reason_for_call) {
        case DLL_PROCESS_ATTACH: {
            std::cerr << "Dll attached!" << std::endl;
            vEDRHookInit();
        }
        case DLL_THREAD_ATTACH:
        case DLL_THREAD_DETACH:
        default:
            break;
        case DLL_PROCESS_DETACH: {
            std::cerr << "Dll dettached!" << std::endl;
            vEDRHookExit();
        }

    }
    return TRUE;
}
