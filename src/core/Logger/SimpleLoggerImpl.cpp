// nlm - Neural Learning Machine
// Simple Logger Implementation
// Phase 2: Practical Runtime Logging Implementation
//
// This file implements the SimpleLogger and ConsoleLogger classes.

#include "SimpleLogger.hpp"
#include <iostream>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <ctime>
#include <algorithm>

namespace nlm {

// ConsoleLogger implementation
void ConsoleLogger::trace(const std::string& message,
                        const std::string& source,
                        const std::string& function,
                        const std::string& file,
                        int line,
                        const std::map<std::string, std::string>& context) {
    log(LogLevel::Trace, message, source, function, file, line, context);
}

void ConsoleLogger::debug(const std::string& message,
                         const std::string& source,
                         const std::string& function,
                         const std::string& file,
                         int line,
                         const std::map<std::string, std::string>& context) {
    log(LogLevel::Debug, message, source, function, file, line, context);
}

void ConsoleLogger::info(const std::string& message,
                         const std::string& source,
                         const std::string& function,
                         const std::string& file,
                         int line,
                         const std::map<std::string, std::string>& context) {
    log(LogLevel::Info, message, source, function, file, line, context);
}

void ConsoleLogger::notice(const std::string& message,
                           const std::string& source,
                           const std::string& function,
                           const std::string& file,
                           int line,
                           const std::map<std::string, std::string>& context) {
    log(LogLevel::Notice, message, source, function, file, line, context);
}

void ConsoleLogger::warning(const std::string& message,
                           const std::string& source,
                           const std::string& function,
                           const std::string& file,
                           int line,
                           const std::map<std::string, std::string>& context) {
    log(LogLevel::Warning, message, source, function, file, line, context);
}

void ConsoleLogger::error(const std::string& message,
                         const std::string& source,
                         const std::string& function,
                         const std::string& file,
                         int line,
                         const std::string& details,
                         const std::map<std::string, std::string>& context) {
    log(LogLevel::Error, message, source, function, file, line, context);
    if (!details.empty()) {
        log(LogLevel::Error, "Details: " + details, source, function, file, line, context);
    }
}

void ConsoleLogger::critical(const std::string& message,
                            const std::string& source,
                            const std::string& function,
                            const std::string& file,
                            int line,
                            const std::string& details,
                            const std::map<std::string, std::string>& context) {
    log(LogLevel::Critical, message, source, function, file, line, context);
    if (!details.empty()) {
        log(LogLevel::Critical, "Details: " + details, source, function, file, line, context);
    }
}

void ConsoleLogger::alert(const std::string& message,
                         const std::string& source,
                         const std::string& function,
                         const std::string& file,
                         int line,
                         const std::string& details,
                         const std::map<std::string, std::string>& context) {
    log(LogLevel::Alert, message, source, function, file, line, context);
    if (!details.empty()) {
        log(LogLevel::Alert, "Details: " + details, source, function, file, line, context);
    }
}

void ConsoleLogger::emergency(const std::string& message,
                             const std::string& source,
                             const std::string& function,
                             const std::string& file,
                             int line,
                             const std::string& details,
                             const std::map<std::string, std::string>& context) {
    log(LogLevel::Emergency, message, source, function, file, line, context);
    if (!details.empty()) {
        log(LogLevel::Emergency, "Details: " + details, source, function, file, line, context);
    }
}

void ConsoleLogger::log(LogLevel level, const std::string& message,
                       const std::string& source,
                       const std::string& function,
                       const std::string& file,
                       int line,
                       const std::map<std::string, std::string>& context) {
    if (level < config_.minLevel) {
        return;
    }
    
    LogEntry entry(level, message, source, function, file, line, context);
    
    // Format message
    std::string formattedMessage = formatEntry(entry);
    
    // Output to console
    std::cout << formattedMessage << std::endl;
    
    // Output to file
    if (config_.enableFileOutput && logFile_.is_open()) {
        logFile_ << formattedMessage << std::endl;
    }
    
    // Update statistics
    updateStatistics(entry);
}

std::string ConsoleLogger::formatEntry(const LogEntry& entry) const {
    std::ostringstream oss;
    
    // Add level with color
    if (config_.enableColors) {
        oss << getLevelColorCode(entry.level);
    }
    
    // Add timestamp
    if (config_.enableTimestamps) {
        auto time = std::chrono::system_clock::to_time_t(entry.timestamp);
        oss << std::put_time(std::localtime(&time), "%Y-%m-%d %H:%M:%S");
        oss << " ";
    }
    
    // Add thread ID
    if (config_.enableThreadId) {
        oss << "[" << std::this_thread::get_id() << "] ";
    }
    
    // Add source location
    if (config_.enableSourceLocation && !entry.file.empty() && entry.line > 0) {
        oss << entry.file << ":" << entry.line << " ";
    }
    
    oss << entry.message;
    
    // Add context
    if (!entry.context.empty()) {
        oss << " [Context:";
        bool first = true;
        for (const auto& pair : entry.context) {
            if (!first) oss << ",";
            oss << " " << pair.first << "=" << pair.second;
            first = false;
        }
        oss << "]";
    }
    
    if (config_.enableColors) {
        oss << "\033[0m";
    }
    
    return oss.str();
}

std::string ConsoleLogger::getLevelColorCode(LogLevel level) const {
    switch (level) {
        case LogLevel::Trace: return "\033[36m";  // Cyan
        case LogLevel::Debug: return "\033[90m";  // Bright Black
        case LogLevel::Info: return "\033[32m";   // Green
        case LogLevel::Notice: return "\033[34m"; // Blue
        case LogLevel::Warning: return "\033[33m"; // Yellow
        case LogLevel::Error: return "\033[31m";   // Red
        case LogLevel::Critical: return "\033[35m"; // Magenta
        case LogLevel::Alert: return "\033[91m";  // Bright Red
        case LogLevel::Emergency: return "\033[95m"; // Bright Magenta
        default: return "\033[0m";
    }
}

void ConsoleLogger::updateStatistics(const LogEntry& entry) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    totalLogs_++;
    logsByLevel_[entry.level]++;
    
    if (!firstLog_.time_since_epoch().count()) {
        firstLog_ = entry.timestamp;
    }
    lastLog_ = entry.timestamp;
}

// Global logging setup
namespace Logging {
    static std::shared_ptr<SimpleLogger> globalLogger;
    static ConsoleLogger::Config globalLoggerConfig;
    
    // Initialize logging system
    void initialize() {
        globalLogger = std::make_shared<ConsoleLogger>();
    }
    
    // Get global logger
    std::shared_ptr<SimpleLogger> getLogger() {
        if (!globalLogger) {
            initialize();
        }
        return globalLogger;
    }
    
    // Configure global logger
    void configure(const ConsoleLogger::Config& config) {
        globalLoggerConfig = config;
        if (globalLogger) {
            std::dynamic_pointer_cast<ConsoleLogger>(globalLogger)->configure(config);
        }
    }
    
    // Log functions (global)
    void trace(const std::string& message,
              const std::string& source,
              const std::string& function,
              const std::string& file,
              int line,
              const std::map<std::string, std::string>& context) {
        if (globalLogger) {
            globalLogger->trace(message, source, function, file, line, context);
        }
    }
    
    void debug(const std::string& message,
              const std::string& source,
              const std::string& function,
              const std::string& file,
              int line,
              const std::map<std::string, std::string>& context) {
        if (globalLogger) {
            globalLogger->debug(message, source, function, file, line, context);
        }
    }
    
    void info(const std::string& message,
              const std::string& source,
              const std::string& function,
              const std::string& file,
              int line,
              const std::map<std::string, std::string>& context) {
        if (globalLogger) {
            globalLogger->info(message, source, function, file, line, context);
        }
    }
    
    void notice(const std::string& message,
               const std::string& source,
               const std::string& function,
               const std::string& file,
               int line,
               const std::map<std::string, std::string>& context) {
        if (globalLogger) {
            globalLogger->notice(message, source, function, file, line, context);
        }
    }
    
    void warning(const std::string& message,
                const std::string& source,
                const std::string& function,
                const std::string& file,
                int line,
                const std::map<std::string, std::string>& context) {
        if (globalLogger) {
            globalLogger->warning(message, source, function, file, line, context);
        }
    }
    
    void error(const std::string& message,
              const std::string& source,
              const std::string& function,
              const std::string& file,
              int line,
              const std::string& details,
              const std::map<std::string, std::string>& context) {
        if (globalLogger) {
            globalLogger->error(message, source, function, file, line, details, context);
        }
    }
    
    void critical(const std::string& message,
                 const std::string& source,
                 const std::string& function,
                 const std::string& file,
                 int line,
                 const std::string& details,
                 const std::map<std::string, std::string>& context) {
        if (globalLogger) {
            globalLogger->critical(message, source, function, file, line, details, context);
        }
    }
    
    void alert(const std::string& message,
              const std::string& source,
              const std::string& function,
              const std::string& file,
              int line,
              const std::string& details,
              const std::map<std::string, std::string>& context) {
        if (globalLogger) {
            globalLogger->alert(message, source, function, file, line, details, context);
        }
    }
    
    void emergency(const std::string& message,
                  const std::string& source,
                  const std::string& function,
                  const std::string& file,
                  int line,
                  const std::string& details,
                  const std::map<std::string, std::string>& context) {
        if (globalLogger) {
            globalLogger->emergency(message, source, function, file, line, details, context);
        }
    }
    
    // Get statistics
    SimpleLogger::LoggerStats getStatistics() {
        if (globalLogger) {
            return globalLogger->getStatistics();
        }
        return SimpleLogger::LoggerStats();
    }
    
    // Flush
    void flush() {
        if (globalLogger) {
            globalLogger->flush();
        }
    }
    
    // Close
    void close() {
        if (globalLogger) {
            globalLogger->close();
        }
        globalLogger.reset();
    }
    
    // Graceful shutdown
    void shutdown() {
        flush();
        close();
    }
    
private:
    static std::shared_ptr<SimpleLogger> globalLogger;
    static ConsoleLogger::Config globalLoggerConfig;
}

std::shared_ptr<SimpleLogger> Logging::globalLogger;
ConsoleLogger::Config Logging::globalLoggerConfig;

} // namespace nlm