#include <windows.h>
#include <iostream>

BOOL APIENTRY DllMain(HANDLE hModule, DWORD ul_reason_for_call, LPVOID lpReserved) {
    switch (ul_reason_for_call) {
        case DLL_PROCESS_ATTACH:
            std::cout << "Dll attached!" << std::endl;
            break;
        case DLL_THREAD_ATTACH:
        case DLL_THREAD_DETACH:
        default:
        case DLL_PROCESS_DETACH:
            std::cout << "Dll dettached!" << std::endl;
            break;
    }
    return TRUE;
}
