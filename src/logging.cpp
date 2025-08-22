#include "logging.hpp"

#include <spdlog/sinks/null_sink.h>
#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/sinks/stdout_sinks.h>


static std::shared_ptr<spdlog::logger> globalLogger;

void log_init(LogMode mode, LogLevel level)
{
    switch (mode)
    {
        case LOG_MODE_NULL:
            globalLogger = spdlog::null_logger_st("global");
            level = spdlog::level::off;
            break;

        case LOG_MODE_FILE:
            globalLogger = spdlog::basic_logger_mt("global", "log.txt");  // TODO: arbitrary filepath
                                                                          // use asset management system when available
            break;

        default:
            spdlog::warn("[*** LOG WARNING ***] Unrecognized log mode - defaulting to LOG_MODE_STDOUT");
            [[fallthrough]];
        case LOG_MODE_STDOUT:
            globalLogger = spdlog::stdout_logger_mt("global");
            break;
    }
    globalLogger->set_level(level);
    globalLogger->set_pattern("[%T] (%l) %v");
    spdlog::set_default_logger(globalLogger);
}
