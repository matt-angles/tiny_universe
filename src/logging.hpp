#pragma once
#include <spdlog/spdlog.h>


using LogLevel = spdlog::level::level_enum;
enum LogMode {LOG_MODE_NULL, LOG_MODE_STDOUT, LOG_MODE_FILE};
void log_init(LogMode mode, LogLevel level);

class Logger {
public:
    Logger(const char* name) : name(name) {}
    const std::string name;
    
    template <typename... Args>
    void log(LogLevel lvl, fmt::format_string<Args...> fmt, Args&&... args)
    {
        spdlog::log(lvl, "{}: {}", name, fmt::format(fmt, std::forward<Args>(args)...));
    }
    template <typename T>
    void log(LogLevel lvl, const T &msg)
    {
        spdlog::log(lvl, name + ": " + msg);
    }

    template <typename... Args>
    void trace(fmt::format_string<Args...> fmt, Args&&... args)
    {
        spdlog::trace("{}: {}", name, fmt::format(fmt, std::forward<Args>(args)...));
    }
    template <typename T>
    void trace(const T& msg)
    {
        spdlog::trace(name + ": " + msg);
    }

    template <typename... Args>
    void debug(fmt::format_string<Args...> fmt, Args &&... args)
    {
        spdlog::debug("{}: {}", name, fmt::format(fmt, std::forward<Args>(args)...));
    }
    template <typename T>
    void debug(const T& msg)
    {
        spdlog::debug(name + ": " + msg);
    }

    template <typename... Args>
    void info(fmt::format_string<Args...> fmt, Args&&... args)
    {
        spdlog::info("{}: {}", name, fmt::format(fmt, std::forward<Args>(args)...));
    }
    template <typename T>
    void info(const T& msg)
    {
        spdlog::info(name + ": " + msg);
    }
    
    template <typename... Args>
    void warn(fmt::format_string<Args...> fmt, Args&&... args)
    {
        spdlog::warn("{}: {}", name, fmt::format(fmt, std::forward<Args>(args)...));
    }
    template <typename T>
    void warn(const T& msg)
    {
        spdlog::warn(name + ": " + msg);
    }

    template <typename... Args>
    void error(fmt::format_string<Args...> fmt, Args&&... args)
    {
        spdlog::error("{}: {}", name, fmt::format(fmt, std::forward<Args>(args)...));
    }
    template <typename T>
    void error(const T& msg)
    {
        spdlog::error(name + ": " + msg);
    }

    template <typename... Args>
    void critical(fmt::format_string<Args...> fmt, Args&&... args)
    {
        spdlog::critical("{}: {}", name, fmt::format(fmt, std::forward<Args>(args)...));
    }
    template <typename T>
    void critical(const T& msg)
    {
        spdlog::critical(name + ": " + msg);
    }
};
