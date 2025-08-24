#pragma once
#include "logging.hpp"

namespace app
{
    constexpr LogMode logMode   = LOG_MODE_STDOUT;
    constexpr LogLevel logLevel = LogLevel::trace;

    constexpr const char* workDir = "../";
}


#if defined(_WIN32) || defined(__WIN32__) || defined(_WIN64) || defined(__NT__)
    #define PLATFORM_WINDOWS 1
#elif defined(__APPLE__) || defined(__MACH__)
    #define PLATFORM_MACOS 1
#elif defined(__linux__)
    #define PLATFORM_LINUX 1
#else
    #define PLATFORM_UNKNOWN 1
#endif
