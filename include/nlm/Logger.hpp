#pragma once

#include <memory>
#include <vector>
#include <string>
#include <functional>
#include <chrono>
#include <thread>
#include <atomic>
#include <mutex>
#include <sstream>
#include <iomanip>

#include "nlm/CLI.hpp"
#include "nlm/Exception.hpp"

namespace nlm {

/**
 * Enhanced logging system for NLM
 * Provides multi-level logging with file output, console output, and filtering
 */
class Logger {
public:
    // Log level enumeration
    enum class Level {
        Trace = 0,
        Debug = 1,
        Info = 2,
        Warning = 3,
        Error = 4,
        Critical = 5,
        Off = 6
    };

    // Log message structure
    struct LogMessage {
        Level level;
        std::string source;
        std::string message;
        std::string timestamp;
        std::string threadId;
        std::vector<std::string> metadata;
    };

    // Log handler function type
    using LogHandler = std::function<void(const LogMessage&)>;

    Logger(const std::string& name = "NLM");
    ~Logger();

    // Add a log handler
    void addHandler(LogHandler handler);

    // Remove a log handler
    void removeHandler(LogHandler handler);

    // Set minimum log level
    void setLevel(Level level);

    // Get minimum log level
    Level getLevel() const { return minLevel; }

    // Log message methods
    void trace(const std::string& message);
    void debug(const std::string& message);
    void info(const std::string& message);
    void warning(const std::string& message);
    void error(const std::string& message);
    void critical(const std::string& message);

    // Log message with formatting
    template<typename... Args>
    void trace(const std::string& format, Args&&... args);

    template<typename... Args>
    void debug(const std::string& format, Args&&... args);

    template<typename... Args>
    void info(const std::string& format, Args&&... args);

    template<typename... Args>
    void warning(const std::string& format, Args&&... args);

    template<typename... Args>
    void error(const std::string& format, Args&&... args);

    template<typename... Args>
    void critical(const std::string& format, Args&&... args);

    // Log message with metadata
    void traceWithMetadata(const std::string& message, const std::vector<std::string>& metadata);
    void debugWithMetadata(const std::string& message, const std::vector<std::string>& metadata);
    void infoWithMetadata(const std::string& message, const std::vector<std::string>& metadata);
    void warningWithMetadata(const std::string& message, const std::vector<std::string>& metadata);
    void errorWithMetadata(const std::string& message, const std::vector<std::string>& metadata);
    void criticalWithMetadata(const std::string& message, const std::vector<std::string>& metadata);

    // Set global logger
    static void setGlobal(std::shared_ptr<Logger> logger);

    // Get global logger
    static std::shared_ptr<Logger> getGlobal();

    // Create console logger
    static std::shared_ptr<Logger> createConsoleLogger(Level level = Level::Info);

    // Create file logger
    static std::shared_ptr<Logger> createFileLogger(const std::string& filename, Level level = Level::Info);

    // Flush all handlers
    void flush();

private:
    std::string name;
    Level minLevel;
    std::vector<LogHandler> handlers;
    mutable std::mutex mutex;
    bool initialized;

    // Create log message
    LogMessage createLogMessage(Level level, const std::string& message) const;

    // Format timestamp
    static std::string formatTimestamp(const std::chrono::system_clock::time_point& tp);

    // Format message with arguments
    template<typename... Args>
    static std::string formatMessage(const std::string& format, Args&&... args);
};

// Global logger instance
extern std::shared_ptr<Logger> globalLogger;

// Convenience logging macros
#define NLM_TRACE(...) do { if (nlm::Logger::getGlobal()) nlm::Logger::getGlobal()->trace(__VA_ARGS__); } while(0)
#define NLM_DEBUG(...) do { if (nlm::Logger::getGlobal()) nlm::Logger::getGlobal()->debug(__VA_ARGS__); } while(0)
#define NLM_INFO(...) do { if (nlm::Logger::getGlobal()) nlm::Logger::getGlobal()->info(__VA_ARGS__); } while(0)
#define NLM_WARNING(...) do { if (nlm::Logger::getGlobal()) nlm::Logger::getGlobal()->warning(__VA_ARGS__); } while(0)
#define NLM_ERROR(...) do { if (nlm::Logger::getGlobal()) nlm::Logger::getGlobal()->error(__VA_ARGS__); } while(0)
#define NLM_CRITICAL(...) do { if (nlm::Logger::getGlobal()) nlm::Logger::getGlobal()->critical(__VA_ARGS__); } while(0)

} // namespace nlm
