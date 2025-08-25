#pragma once
#include "logging.hpp"

namespace app
{
    constexpr bool fullscreen = false;
    constexpr int windowWidth = 1600;
    constexpr int windowHeight = 900;
    constexpr const char* windowTitle = "Tiny Universe";

    constexpr const char* workDir = "../";

    constexpr LogMode logMode   = LOG_MODE_STDOUT;
    constexpr LogLevel logLevel = LogLevel::trace;
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
