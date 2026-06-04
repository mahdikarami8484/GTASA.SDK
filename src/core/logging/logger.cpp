/**
 * @file logger.cpp
 * @brief Implementation of the thread-safe asynchronous logging system
 *
 * This file implements the Logger class, providing asynchronous message
 * processing through a dedicated thread and circular buffer.
 */

#include "logger.hpp"

namespace Logging
{

    void Logger::Start()
    {
#ifndef NDEBUG
        AllocConsole();

        FILE* fp = nullptr;
        errno_t err1 = freopen_s(&fp, "CONOUT$", "w", stdout);
        errno_t err2 = freopen_s(&fp, "CONOUT$", "w", stderr);
        if (err1 != 0 || err2 != 0)
        {
            // Console redirection failed, but continue with console allocated
            // Logging will still work but output may not be visible
        }

        // Stop existing thread if running
        if (this->running && this->thread.joinable())
        {
            this->running = false;
            this->cv.notify_all();
            this->thread.join();
        }

        this->running = true;
        this->thread = std::thread(&Logger::loggerThread, this);

#else
        return;
#endif
    }

    /**
     * @brief Enqueue a log message to the circular buffer
     *
     * Thread-safe method to add a log message to the buffer.
     * Messages below the current log level are filtered out.
     */
    void Logger::enqueueMessage(const char* format, va_list args, LogLevel level, const char* file,
                                uint16_t line)
    {
        if (level < currentLevel) return;

        {
            std::lock_guard<std::mutex> lock(this->mutex);

            char buffer[1024];
            vsnprintf(buffer, sizeof(buffer), format, args);

            if (!this->logBuffer.push(LogMessage{buffer, static_cast<int8_t>(level), file, line}))
                return;
        }
        this->cv.notify_one();
    }

    /**
     * @brief Logging thread function
     *
     * Continuously processes messages from the circular buffer until stopped.
     * Waits on a condition variable when the buffer is empty.
     */
    void Logger::loggerThread()
    {
        LogMessage log;

        while (true)
        {
            std::unique_lock<std::mutex> lock(this->mutex);
            this->cv.wait(lock, [&] { return !this->logBuffer.empty() || !this->running; });

            if (!this->running) break;
            if (this->logBuffer.empty()) continue;

            this->logBuffer.pop(log);

            lock.unlock();
            writeToConsole(log.text, static_cast<LogLevel>(log.level), log.file, log.line);
        }
    }

    /**
     * @brief Write a formatted log message to the console
     *
     * Formats and outputs the log message with appropriate color coding
     * and optional file/line information.
     */
    void Logger::writeToConsole(const std::string& message, LogLevel level, const char* file,
                                uint16_t line)
    {
        std::string finalStr;

        if (level < currentLevel) return;

        // Add log level prefix with color
        switch (level)
        {
        case LogLevel::Info:
            finalStr = "[INFO] ";
            break;
        case LogLevel::Warning:
            finalStr = "\033[33m[WARNING] ";
            break;
        case LogLevel::Error:
            finalStr = "\033[31m[ERROR] ";
            break;
        }

        // Add file and line information if provided
        if (file)
        {
            char buffer[512];
            snprintf(buffer, sizeof(buffer), "[%s:%d] ", file, line);
            finalStr += buffer;
        }

        finalStr += message + "\033[0m";

        fwrite(finalStr.c_str(), 1, finalStr.size(), stdout);
        fwrite("\n", 1, 1, stdout);
    }

    void Logger::SetLevel(LogLevel level)
    {
        currentLevel = level;
    }

    void Logger::Info(const char* format, ...)
    {
        va_list args;
        va_start(args, format);
        enqueueMessage(format, args, LogLevel::Info);
        va_end(args);
    }

    void Logger::Warning(const char* format, ...)
    {
        va_list args;
        va_start(args, format);
        enqueueMessage(format, args, LogLevel::Warning);
        va_end(args);
    }

    void Logger::Error(const char* format, ...)
    {
        va_list args;
        va_start(args, format);
        enqueueMessage(format, args, LogLevel::Error);
        va_end(args);
    }

    void Logger::Log(LogLevel level, const char* file, uint16_t line, const char* format, ...)
    {
        va_list args;
        va_start(args, format);
        enqueueMessage(format, args, level, file, line);
        va_end(args);
    }

    void Logger::Log(LogLevel level, const char* format, ...)
    {
        va_list args;
        va_start(args, format);
        enqueueMessage(format, args, level);
        va_end(args);
    }

    void Logger::Log(const char* format, ...)
    {
        va_list args;
        va_start(args, format);
        enqueueMessage(format, args, LogLevel::Info);
        va_end(args);
    }

    void Logger::Stop()
    {
        {
            std::lock_guard<std::mutex> lock(this->mutex);
            this->running = false;
            this->cv.notify_all();
        }
        if (this->thread.joinable()) this->thread.join();

        FreeConsole();
    }

} // namespace Logging