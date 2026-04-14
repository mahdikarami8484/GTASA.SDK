/**
 * @file logger.cpp
 * @brief Implementation of the thread-safe asynchronous logging system
 * 
 * This file implements the Logger class, providing asynchronous message
 * processing through a dedicated thread and circular buffer.
 * Each session writes to both the debug console and a timestamped log file
 * located at logs/sdk_YYYYMMDD_HHMMSS.log.
 */

#include "logger.hpp"
#include <ctime>
#include <cstdio>
#include <cerrno>
#include <direct.h>  // _mkdir on Windows

namespace Logging
{

// ---------------------------------------------------------------------------
// Helper: return the plain-text level prefix
// ---------------------------------------------------------------------------
const char* Logger::levelPrefix(LogLevel level) {
    switch (level) {
        case LogLevel::Info:    return "[INFO]    ";
        case LogLevel::Warning: return "[WARNING] ";
        case LogLevel::Error:   return "[ERROR]   ";
    }
    return "[UNKNOWN] ";
}

// ---------------------------------------------------------------------------
// Helper: build a UTC timestamp string "YYYY-MM-DD HH:MM:SS"
// ---------------------------------------------------------------------------
static std::string currentTimestamp() {
    std::time_t t = std::time(nullptr);
    std::tm tm = {};
#ifdef _WIN32
    gmtime_s(&tm, &t);
#else
    gmtime_r(&t, &tm);
#endif
    char buf[20];
    std::snprintf(buf, sizeof(buf), "%04d-%02d-%02d %02d:%02d:%02d",
        tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday,
        tm.tm_hour, tm.tm_min, tm.tm_sec);
    return buf;
}

// ---------------------------------------------------------------------------
// Start
// ---------------------------------------------------------------------------
void Logger::Start() {
#ifndef NDEBUG
    AllocConsole();

    FILE* fp = nullptr;
    errno_t err1 = freopen_s(&fp, "CONOUT$", "w", stdout);
    errno_t err2 = freopen_s(&fp, "CONOUT$", "w", stderr);
    (void)err1; (void)err2; // redirection failures are non-fatal
#endif

    // Stop existing thread if running
    if (this->running && this->thread.joinable()) {
        this->running = false;
        this->cv.notify_all();
        this->thread.join();
    }

    // Create logs/ directory; EEXIST (already exists) is not an error
    if (_mkdir("logs") != 0 && errno != EEXIST) {
        fprintf(stderr, "[Logger] Warning: failed to create logs/ directory (errno=%d)\n", errno);
    }

    // Build timestamped filename: logs/sdk_YYYYMMDD_HHMMSS.log
    std::time_t t = std::time(nullptr);
    std::tm tm = {};
#ifdef _WIN32
    gmtime_s(&tm, &t);
#else
    gmtime_r(&t, &tm);
#endif
    char filename[64];
    std::snprintf(filename, sizeof(filename),
        "logs/sdk_%04d%02d%02d_%02d%02d%02d.log",
        tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday,
        tm.tm_hour, tm.tm_min, tm.tm_sec);

    this->logFile.open(filename, std::ios::out | std::ios::app);
    if (!this->logFile.is_open()) {
        fprintf(stderr, "[Logger] Warning: failed to open log file '%s'\n", filename);
    }

    this->running = true;
    this->thread = std::thread(&Logger::loggerThread, this);
}

// ---------------------------------------------------------------------------
// enqueueMessage
// ---------------------------------------------------------------------------
/**
 * @brief Enqueue a log message to the circular buffer
 * 
 * Thread-safe method to add a log message to the buffer.
 * Messages below the current log level are filtered out.
 */
void Logger::enqueueMessage(const char* format, va_list args, LogLevel level, const char* file, uint16_t line) {
    if (level < currentLevel) return;
    
    {
        std::lock_guard<std::mutex> lock(this->mutex);
        
        char buffer[1024];
        vsnprintf(buffer, sizeof(buffer), format, args);

        if (!this->logBuffer.push(LogMessage{ buffer, static_cast<int8_t>(level), file, line })) return;
    }
    this->cv.notify_one();
}

// ---------------------------------------------------------------------------
// loggerThread
// ---------------------------------------------------------------------------
/**
 * @brief Logging thread function
 * 
 * Continuously processes messages from the circular buffer until stopped.
 * Waits on a condition variable when the buffer is empty.
 */
void Logger::loggerThread() {
    LogMessage log;

    while (true) {
        std::unique_lock<std::mutex> lock(this->mutex);
        this->cv.wait(lock, [&] { return !this->logBuffer.empty() || !this->running; });

        if (!this->running)
            break;
        if (this->logBuffer.empty())
            continue;

        this->logBuffer.pop(log);

        lock.unlock();

        LogLevel lvl = static_cast<LogLevel>(log.level);
        writeToConsole(log.text, lvl, log.file, log.line);
        writeToFile(log.text, lvl, log.file, log.line);
    }
}

// ---------------------------------------------------------------------------
// writeToConsole
// ---------------------------------------------------------------------------
/**
 * @brief Write a formatted log message to the console
 * 
 * Formats and outputs the log message with ANSI color coding
 * and optional file/line information.
 */
void Logger::writeToConsole(const std::string& message, LogLevel level, const char* file, uint16_t line) {
    if (level < currentLevel)
        return;

    std::string finalStr;

    // ANSI color + level prefix
    switch (level) {
        case LogLevel::Warning:
            finalStr = "\033[33m";
            break;
        case LogLevel::Error:
            finalStr = "\033[31m";
            break;
        default:
            break;
    }
    finalStr += levelPrefix(level);

    if (file) {
        char buf[512];
        snprintf(buf, sizeof(buf), "[%s:%d] ", file, line);
        finalStr += buf;
    }

    finalStr += message + "\033[0m";

    fwrite(finalStr.c_str(), 1, finalStr.size(), stdout);
    fwrite("\n", 1, 1, stdout);
}

// ---------------------------------------------------------------------------
// writeToFile
// ---------------------------------------------------------------------------
/**
 * @brief Write a formatted log message to the log file
 * 
 * Writes a plain-text entry (no ANSI codes) including a UTC timestamp,
 * level, optional file/line, and message text.
 */
void Logger::writeToFile(const std::string& message, LogLevel level, const char* file, uint16_t line) {
    if (!this->logFile.is_open())
        return;
    if (level < currentLevel)
        return;

    // Format: [TIMESTAMP] [LEVEL] [file:line] message
    this->logFile << '[' << currentTimestamp() << "] "
                  << levelPrefix(level);

    if (file) {
        char buf[512];
        snprintf(buf, sizeof(buf), "[%s:%d] ", file, line);
        this->logFile << buf;
    }

    this->logFile << message << '\n';
    this->logFile.flush();
}

// ---------------------------------------------------------------------------
// SetLevel / Info / Warning / Error / Log overloads
// ---------------------------------------------------------------------------
void Logger::SetLevel(LogLevel level) {
    currentLevel = level;
}

void Logger::Info(const char* format, ...) {
    va_list args;
    va_start(args, format);
    enqueueMessage(format, args, LogLevel::Info);
    va_end(args);
}

void Logger::Warning(const char* format, ...) {
    va_list args;
    va_start(args, format);
    enqueueMessage(format, args, LogLevel::Warning);
    va_end(args);
}

void Logger::Error(const char* format, ...) {
    va_list args;
    va_start(args, format);
    enqueueMessage(format, args, LogLevel::Error);
    va_end(args);
}

void Logger::Log(LogLevel level, const char* file, uint16_t line, const char* format, ...) {
    va_list args;
    va_start(args, format);
    enqueueMessage(format, args, level, file, line);
    va_end(args);
}

void Logger::Log(LogLevel level, const char* format, ...) {
    va_list args;
    va_start(args, format);
    enqueueMessage(format, args, level);
    va_end(args);
}

void Logger::Log(const char* format, ...) {
    va_list args;
    va_start(args, format);
    enqueueMessage(format, args, LogLevel::Info);
    va_end(args);
}

// ---------------------------------------------------------------------------
// Stop
// ---------------------------------------------------------------------------
void Logger::Stop() {
    {
        std::lock_guard<std::mutex> lock(this->mutex);
        this->running = false;
        this->cv.notify_all();
    }
    if (this->thread.joinable())
        this->thread.join();

    if (this->logFile.is_open())
        this->logFile.close();

#ifndef NDEBUG
    FreeConsole();
#endif
}

} // namespace Logging