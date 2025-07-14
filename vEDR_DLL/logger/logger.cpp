
#include "logger.h"

#include <windows.h>
#include <sstream>
#include <iomanip>

std::string pipeName = "\\\\.\\pipe\\vEDR_Pipe";

HANDLE hPipe = INVALID_HANDLE_VALUE;

void logger::log(const std::string &message) {
    WriteFile(hPipe, message.c_str(), message.length(), nullptr, nullptr);
}

bool logger::loggerInit() {
    if (hPipe == INVALID_HANDLE_VALUE) {
        hPipe = CreateFile(TEXT(pipeName.c_str()),
                           GENERIC_WRITE,
                           0, nullptr, OPEN_EXISTING, 0, nullptr);
        if (hPipe == INVALID_HANDLE_VALUE) {
            return false;
        }
    }
    return true;
}

bool logger::loggerExit() {
    if (hPipe != INVALID_HANDLE_VALUE) {
        CloseHandle(hPipe);
        hPipe = INVALID_HANDLE_VALUE;
    }
    return true;
}

std::string logger::helper::AllocTypeToString(ULONG allocType) {
    std::ostringstream ss;
    bool first = true;
    if (allocType & MEM_COMMIT)      { ss << (first?"":"|") << "COMMIT"; first = false; }
    if (allocType & MEM_RESERVE)     { ss << (first?"":"|") << "RESERVE"; first = false; }
    if (allocType & MEM_TOP_DOWN)    { ss << (first?"":"|") << "TOP_DOWN"; first = false; }
    if (allocType & MEM_WRITE_WATCH) { ss << (first?"":"|") << "WRITE_WATCH"; first = false; }
    if (allocType & MEM_PHYSICAL)    { ss << (first?"":"|") << "PHYSICAL"; first = false; }
    if (first) ss << "0";
    return ss.str();
}

std::string logger::helper::ProtectToString(ULONG protect) {
    switch (protect) {
        case PAGE_NOACCESS:          return "NOACCESS";
        case PAGE_READONLY:          return "READONLY";
        case PAGE_READWRITE:         return "READWRITE";
        case PAGE_WRITECOPY:         return "WRITECOPY";
        case PAGE_EXECUTE:           return "EXECUTE";
        case PAGE_EXECUTE_READ:      return "EXECUTE_READ";
        case PAGE_EXECUTE_READWRITE: return "EXECUTE_READWRITE";
        case PAGE_EXECUTE_WRITECOPY: return "EXECUTE_WRITECOPY";
        case PAGE_GUARD:             return "GUARD";
        case PAGE_NOCACHE:           return "NOCACHE";
        case PAGE_WRITECOMBINE:      return "WRITECOMBINE";
        default:
            std::ostringstream ss;
            ss << "0x" << std::hex << protect;
            return ss.str();
    }
}

