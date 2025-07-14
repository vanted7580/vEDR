#pragma once

#include <string>


namespace logger {
    void log(const std::string &message);
    bool loggerInit();
    bool loggerExit();

    namespace helper {
        std::string AllocTypeToString(unsigned long allocType);
        std::string ProtectToString(unsigned long protect);
    }

}


