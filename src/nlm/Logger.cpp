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
    using LogHandler = std::function<void(const LogMessage&)>

    Logger(const std::string& name = "NLM")
        : name(name), minLevel(Level::Info), initialized(true) {}

    ~Logger() = default;

    // Add a log handler
    void addHandler(LogHandler handler) {
        std::lock_guard<std::mutex> lock(mutex);
        handlers.push_back(handler);
    }

    // Remove a log handler
    void removeHandler(LogHandler handler) {
        std::lock_guard<std::mutex> lock(mutex);
        auto it = std::find(handlers.begin(), handlers.end(), handler);
        if (it != handlers.end()) {
            handlers.erase(it);
        }
    }

    // Set minimum log level
    void setLevel(Level level) {
        std::lock_guard<std::mutex> lock(mutex);
        minLevel = level;
    }

    // Get minimum log level
    Level getLevel() const { return minLevel; }

    // Log message methods
    void trace(const std::string& message) {
        if (minLevel <= Level::Trace) {
            log(Level::Trace, message);
        }
    }

    void debug(const std::string& message) {
        if (minLevel <= Level::Debug) {
            log(Level::Debug, message);
        }
    }

    void info(const std::string& message) {
        if (minLevel <= Level::Info) {
            log(Level::Info, message);
        }
    }

    void warning(const std::string& message) {
        if (minLevel <= Level::Warning) {
            log(Level::Warning, message);
        }
    }

    void error(const std::string& message) {
        if (minLevel <= Level::Error) {
            log(Level::Error, message);
        }
    }

    void critical(const std::string& message) {
        if (minLevel <= Level::Critical) {
            log(Level::Critical, message);
        }
    }

    // Log message with formatting
    template<typename... Args>
    void trace(const std::string& format, Args&&... args) {
        if (minLevel <= Level::Trace) {
            log(Level::Trace, formatMessage(format, std::forward<Args>(args)...));
        }
    }

    template<typename... Args>
    void debug(const std::string& format, Args&&... args) {
        if (minLevel <= Level::Debug) {
            log(Level::Debug, formatMessage(format, std::forward<Args>(args)...));
        }
    }

    template<typename... Args>
    void info(const std::string& format, Args&&... args) {
        if (minLevel <= Level::Info) {
            log(Level::Info, formatMessage(format, std::forward<Args>(args)...));
        }
    }

    template<typename... Args>
    void warning(const std::string& format, Args&&... args) {
        if (minLevel <= Level::Warning) {
            log(Level::Warning, formatMessage(format, std::forward<Args>(args)...));
        }
    }

    template<typename... Args>
    void error(const std::string& format, Args&&... args) {
        if (minLevel <= Level::Error) {
            log(Level::Error, formatMessage(format, std::forward<Args>(args)...));
        }
    }

    template<typename... Args>
    void critical(const std::string& format, Args&&... args) {
        if (minLevel <= Level::Critical) {
            log(Level::Critical, formatMessage(format, std::forward<Args>(args)...));
        }
    }

    // Log message with metadata
    void traceWithMetadata(const std::string& message, const std::vector<std::string>& metadata) {
        if (minLevel <= Level::Trace) {
            logWithMetadata(Level::Trace, message, metadata);
        }
    }

    void debugWithMetadata(const std::string& message, const std::vector<std::string>& metadata) {
        if (minLevel <= Level::Debug) {
            logWithMetadata(Level::Debug, message, metadata);
        }
    }

    void infoWithMetadata(const std::string& message, const std::vector<std::string>& metadata) {
        if (minLevel <= Level::Info) {
            logWithMetadata(Level::Info, message, metadata);
        }
    }

    void warningWithMetadata(const std::string& message, const std::vector<std::string>& metadata) {
        if (minLevel <= Level::Warning) {
            logWithMetadata(Level::Warning, message, metadata);
        }
    }

    void errorWithMetadata(const std::string& message, const std::vector<std::string>& metadata) {
        if (minLevel <= Level::Error) {
            logWithMetadata(Level::Error, message, metadata);
        }
    }

    void criticalWithMetadata(const std::string& message, const std::vector<std::string>& metadata) {
        if (minLevel <= Level::Critical) {
            logWithMetadata(Level::Critical, message, metadata);
        }
    }

    // Set global logger
    static void setGlobal(std::shared_ptr<Logger> logger) {
        std::lock_guard<std::mutex> lock(globalMutex);
        globalLogger = logger;
    }

    // Get global logger
    static std::shared_ptr<Logger> getGlobal() {
        std::lock_guard<std::mutex> lock(globalMutex);
        return globalLogger;
    }

    // Create console logger
    static std::shared_ptr<Logger> createConsoleLogger(Level level = Level::Info) {
        auto logger = std::make_shared<Logger>("Console");
        logger->setLevel(level);
        
        // Add console handler
        logger->addHandler([](const LogMessage& msg) {
            std::string levelStr;
            switch (msg.level) {
                case Level::Trace: levelStr = "TRACE"; break;
                case Level::Debug: levelStr = "DEBUG"; break;
                case Level::Info: levelStr = "INFO"; break;
                case Level::Warning: levelStr = "WARN"; break;
                case Level::Error: levelStr = "ERROR"; break;
                case Level::Critical: levelStr = "CRIT"; break;
                default: levelStr = "UNK"; break;
            }
            
            std::cout << "[" << msg.timestamp << "] [" << msg.threadId << "] [" 
                      << levelStr << "] " << msg.source << ": " << msg.message << std::endl;
        });
        
        return logger;
    }

    // Create file logger
    static std::shared_ptr<Logger> createFileLogger(const std::string& filename, Level level = Level::Info) {
        auto logger = std::make_shared<Logger>("File");
        logger->setLevel(level);
        
        // Create file stream
        std::ofstream fileStream(filename, std::ios::app);
        if (!fileStream.is_open()) {
            throw NLMException(NLMException::ErrorCode::FileNotFound, 
                             "Failed to open log file: " + filename);
        }
        
        // Add file handler
        logger->addHandler([filename, fileStream](const LogMessage& msg) {
            std::string levelStr;
            switch (msg.level) {
                case Level::Trace: levelStr = "TRACE"; break;
                case Level::Debug: levelStr = "DEBUG"; break;
                case Level::Info: levelStr = "INFO"; break;
                case Level::Warning: levelStr = "WARN"; break;
                case Level::Error: levelStr = "ERROR"; break;
                case Level::Critical: levelStr = "CRIT"; break;
                default: levelStr = "UNK"; break;
            }
            
            fileStream << "[" << msg.timestamp << "] [" << msg.threadId << "] [" 
                       << levelStr << "] " << msg.source << ": " << msg.message << std::endl;
            fileStream.flush();
        });
        
        return logger;
    }

    // Flush all handlers
    void flush() {
        std::lock_guard<std::mutex> lock(mutex);
        // Currently, handlers flush automatically
        // This could be extended for buffered handlers
    }

private:
    std::string name;
    Level minLevel;
    std::vector<LogHandler> handlers;
    mutable std::mutex mutex;
    bool initialized;

    // Create log message
    LogMessage createLogMessage(Level level, const std::string& message) const {
        LogMessage msg;
        msg.level = level;
        msg.source = name;
        msg.message = message;
        msg.timestamp = formatTimestamp(std::chrono::system_clock::now());
        msg.threadId = std::to_string(std::hash<std::thread::id>{}(std::this_thread::get_id()));
        return msg;
    }

    // Log message
    void log(Level level, const std::string& message) {
        LogMessage msg = createLogMessage(level, message);
        
        std::lock_guard<std::mutex> lock(mutex);
        for (const auto& handler : handlers) {
            try {
                handler(msg);
            } catch (const std::exception& e) {
                std::cerr << "Error in log handler: " << e.what() << std::endl;
            }
        }
    }

    // Log message with metadata
    void logWithMetadata(Level level, const std::string& message, const std::vector<std::string>& metadata) {
        LogMessage msg = createLogMessage(level, message);
        msg.metadata = metadata;
        
        std::lock_guard<std::mutex> lock(mutex);
        for (const auto& handler : handlers) {
            try {
                handler(msg);
            } catch (const std::exception& e) {
                std::cerr << "Error in log handler: " << e.what() << std::endl;
            }
        }
    }

    // Format timestamp
    static std::string formatTimestamp(const std::chrono::system_clock::time_point& tp) {
        auto time = std::chrono::system_clock::to_time_t(tp);
        auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(
            tp.time_since_epoch()) % 1000;
        
        std::stringstream ss;
        ss << std::put_time(std::localtime(&time), "%Y-%m-%d %H:%M:%S");
        ss << "." << std::setfill('0') << std::setw(3) << ms.count();
        return ss.str();
    }

    // Format message with arguments
    template<typename... Args>
    static std::string formatMessage(const std::string& format, Args&&... args) {
        std::stringstream ss;
        ss << std::forward<const std::string&>(format);
        // Simple formatting - for production, consider using a proper formatting library
        return ss.str();
    }
};

// Global logger instance
std::shared_ptr<Logger> Logger::globalLogger = nullptr;

// Convenience logging macros
#define NLM_TRACE(...) do { if (nlm::Logger::getGlobal()) nlm::Logger::getGlobal()->trace(__VA_ARGS__); } while(0)
#define NLM_DEBUG(...) do { if (nlm::Logger::getGlobal()) nlm::Logger::getGlobal()->debug(__VA_ARGS__); } while(0)
#define NLM_INFO(...) do { if (nlm::Logger::getGlobal()) nlm::Logger::getGlobal()->info(__VA_ARGS__); } while(0)
#define NLM_WARNING(...) do { if (nlm::Logger::getGlobal()) nlm::Logger::getGlobal()->warning(__VA_ARGS__); } while(0)
#define NLM_ERROR(...) do { if (nlm::Logger::getGlobal()) nlm::Logger::getGlobal()->error(__VA_ARGS__); } while(0)
#define NLM_CRITICAL(...) do { if (nlm::Logger::getGlobal()) nlm::Logger::getGlobal()->critical(__VA_ARGS__); } while(0)

} // namespace nlm
