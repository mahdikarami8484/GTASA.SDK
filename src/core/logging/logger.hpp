/**
 * @file logger.hpp
 * @brief Thread-safe asynchronous logging system
 * 
 * This file provides a singleton Logger class that implements thread-safe,
 * asynchronous logging with support for multiple log levels (Info, Warning, Error).
 * Messages are queued in a circular buffer and processed by a dedicated thread.
 * Output is written to both the debug console and a timestamped log file under logs/.
 */

#pragma once

#include <Windows.h>
#include <mutex>
#include <condition_variable>
#include <thread>
#include <fstream>
#include <string>
#include "circular_buffer.hpp"
#include <cstdarg>

namespace Logging {

/**
 * @brief Structure representing a log message
 */
struct LogMessage {
    std::string text;       ///< Log message text
    int8_t level;           ///< Log level
    const char* file;       ///< Source file name (optional)
    uint16_t line;          ///< Source line number (optional)
};

/**
 * @brief Enumeration of available log levels
 */
enum class LogLevel { 
    Info,       ///< Informational messages
    Warning,    ///< Warning messages
    Error       ///< Error messages
};

/**
 * @brief Singleton logger class for thread-safe asynchronous logging
 * 
 * The Logger class implements the singleton pattern and provides thread-safe
 * logging functionality. Messages are enqueued and processed asynchronously
 * by a dedicated logging thread. Each session writes to both the console
 * (debug builds only) and a timestamped log file at logs/sdk_YYYYMMDD_HHMMSS.log.
 * 
 * Usage:
 *   Logger::Instance().Start();  // Start the logging thread
 *   LOG_INFO("Message: %s", text);
 *   Logger::Instance().Stop();   // Stop the logging thread and flush the log file
 */
class Logger {
public:
    /**
     * @brief Get the singleton instance of the Logger
     * @return Reference to the Logger instance
     */
    static Logger& Instance() {
        static Logger instance;
        return instance;
    }
    
    void Info(const char* format, ...);
    void Warning(const char* format, ...);
    void Error(const char* format, ...);
    
    void Log(LogLevel level, const char* file, uint16_t line, const char* format, ...);
    void Log(LogLevel level, const char* format, ...);
    void Log(const char* format, ...);

    void SetLevel(LogLevel level);
    
    void Start(); ///< Start the logging thread and open the log file
    void Stop();  ///< Stop the logging thread and close the log file

private:
    Logger() = default;
    ~Logger() = default;

    Logger(const Logger&) = delete;            ///< Delete copy constructor
    Logger& operator=(const Logger&) = delete; ///< Delete copy assignment operator

    void writeToConsole(const std::string& message, LogLevel level, const char* file = nullptr, uint16_t line = 0);
    void writeToFile(const std::string& message, LogLevel level, const char* file = nullptr, uint16_t line = 0);
    void enqueueMessage(const char* format, va_list args, LogLevel level, const char* file = nullptr, uint16_t line = 0);
    void loggerThread();

    /// Build the level prefix string (e.g. "[INFO] ")
    static const char* levelPrefix(LogLevel level);

    LogLevel currentLevel = LogLevel::Info;
    CircularBuffer<LogMessage, 1024> logBuffer;
    std::mutex mutex;
    std::condition_variable cv;
    std::thread thread;
    bool running = false;
    std::ofstream logFile; ///< Output file stream for the current log file
};

/**
 * @brief Macro for logging informational messages with file and line information
 * @param msg Format string (printf-style)
 * @param ... Variable arguments for format string
 */
#define LOG_INFO(msg, ...)    Logger::Instance().Log(LogLevel::Info,    __FILE__, __LINE__, msg, ##__VA_ARGS__)

/**
 * @brief Macro for logging warning messages with file and line information
 * @param msg Format string (printf-style)
 * @param ... Variable arguments for format string
 */
#define LOG_WARNING(msg, ...)    Logger::Instance().Log(LogLevel::Warning,    __FILE__, __LINE__, msg, ##__VA_ARGS__)

/**
 * @brief Macro for logging error messages with file and line information
 * @param msg Format string (printf-style)
 * @param ... Variable arguments for format string
 */
#define LOG_ERROR(msg, ...)    Logger::Instance().Log(LogLevel::Error,    __FILE__, __LINE__, msg, ##__VA_ARGS__)

} // namespace Logging