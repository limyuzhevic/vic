#include "LoggerHelper.hpp"
#include "Logger.hpp"
#include <iomanip>
#include <sstream>

namespace nlm {

// LoggerHelper implementation

std::mutex LoggerHelper::logMutex_;

void LoggerHelper::logMessage(LogLevel level, const std::string& message,
                             const std::string& file, int line,
                             const std::string& function) {
    std::lock_guard<std::mutex> lock(logMutex_);
    
    // Create standardized log entry
    LogEntry entry(level, message, file, line, function);
    
    // Format message with timestamp
    std::ostringstream oss;
    oss << "[" << static_cast<int>(level) << "] "
        << "[" << getCurrentTimestamp() << "] "
        << message;
    
    if (!file.empty()) {
        oss << " (" << file;
        if (line > 0) oss << ":" << line;
        oss << ")";
    }
    
    if (!function.empty()) {
        oss << " [" << function << "]";
    }
    
    Logger::getGlobal().log(static_cast<LogLevel>(static_cast<int>(level)), oss.str(), file, line, function);
}

bool LoggerHelper::isEnabled(LogLevel level) {
    return Logger::getGlobal().isEnabled(static_cast<LogLevel>(static_cast<int>(level)));
}

std::string LoggerHelper::formatLogEntry(const LogEntry& entry) {
    std::ostringstream oss;
    oss << "[" << static_cast<int>(entry.level) << "] "
        << "[" << getCurrentTimestamp() << "] "
        << entry.message;
    
    if (!entry.file.empty()) {
        oss << " (" << entry.file;
        if (entry.line > 0) oss << ":" << entry.line;
        oss << ")";
    }
    
    if (!entry.function.empty()) {
        oss << " [" << entry.function << "]";
    }
    
    return oss.str();
}

std::string LoggerHelper::formatDuration(const std::chrono::steady_clock::duration& duration) {
    auto seconds = std::chrono::duration_cast<std::chrono::seconds>(duration).count();
    auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(duration).count() % 1000;
    
    if (seconds >= 60) {
        auto minutes = seconds / 60;
        auto secs = seconds % 60;
        return std::to_string(minutes) + "m " + std::to_string(secs) + "s " + std::to_string(ms) + "ms";
    } else if (seconds > 0) {
        return std::to_string(seconds) + "." + std::to_string(ms) + "s";
    } else if (ms >= 1000) {
        auto us = std::chrono::duration_cast<std::chrono::microseconds>(duration).count() / 1000;
        return std::to_string(us) + "ms";
    } else {
        return std::to_string(ms) + "ms";
    }
}

std::string LoggerHelper::getCurrentTimestamp() {
    auto now = std::chrono::system_clock::now();
    auto time_t_now = std::chrono::system_clock::to_time_t(now);
    auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(
        now.time_since_epoch()) % 1000;
    
    std::ostringstream oss;
    oss << std::put_time(std::localtime(&time_t_now), "%Y-%m-%d %H:%M:%S");
    oss << "." << std::setfill('0') << std::setw(3) << ms.count();
    
    return oss.str();
}

} // namespace nlm
