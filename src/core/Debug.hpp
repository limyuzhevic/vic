#pragma once

#include "Types.hpp"
#include <string>
#include <vector>
#include <unordered_map>
#include <chrono>
#include <memory>
#include <mutex>
#include <sstream>
#include <iostream>

namespace nlm {

// Debug level enumeration
enum class DebugLevel {
    Off,
    Error,
    Warning,
    Info,
    Debug,
    Trace
};

// Debug event for tracking debug information
struct DebugEvent {
    DebugLevel level;
    std::string message;
    std::string file;
    int line;
    std::string function;
    std::chrono::system_clock::time_point timestamp;
    
    DebugEvent() : level(DebugLevel::Off), message(), file(), line(0), function(), timestamp() {}
    
    DebugEvent(DebugLevel lvl, const std::string& msg, const std::string& file, int line, const std::string& function)
        : level(lvl), message(msg), file(file), line(line), function(function), timestamp(std::chrono::system_clock::now()) {}
};

// Debug system for tracking debug information
class Debug {
public:
    static Debug& getInstance() {
        static Debug instance;
        return instance;
    }
    
    // Set debug level
    void setLevel(DebugLevel level) {
        std::lock_guard<std::mutex> lock(mutex_);
        level_ = level;
    }
    
    // Get current debug level
    DebugLevel getLevel() const {
        std::lock_guard<std::mutex> lock(mutex_);
        return level_;
    }
    
    // Log debug message
    void log(DebugLevel level, const std::string& message, const std::string& file = "", int line = 0, const std::string& function = "") {
        if (level < level_) {
            return;
        }
        
        std::lock_guard<std::mutex> lock(mutex_);
        
        DebugEvent event(level, message, file, line, function);
        events_.push_back(event);
        
        // Output to console
        std::ostringstream oss;
        oss << "[" << debugLevelToString(level) << "] ";
        oss << event.timestamp << " ";
        if (!file.empty()) {
            oss << file << ":" << line << " ";
        }
        oss << event.message;
        
        std::cout << oss.str() << std::endl;
    }
    
    // Get all debug events
    const std::vector<DebugEvent>& getEvents() const {
        std::lock_guard<std::mutex> lock(mutex_);
        return events_;
    }
    
    // Clear debug events
    void clearEvents() {
        std::lock_guard<std::mutex> lock(mutex_);
        events_.clear();
    }
    
    // Get debug events as string
    std::string getReport() const {
        std::lock_guard<std::mutex> lock(mutex_);
        std::ostringstream oss;
        oss << "=== Debug Report ===\n";
        oss << "Debug Level: " << debugLevelToString(level_) << "\n";
        oss << "Total Events: " << events_.size() << "\n\n";
        
        for (const auto& event : events_) {
            oss << "[" << debugLevelToString(event.level) << "] ";
            oss << event.timestamp << " ";
            if (!event.file.empty()) {
                oss << event.file << ":" << event.line << " ";
            }
            oss << event.message << " (" << event.function << ")\n";
        }
        
        return oss.str();
    }
    
    // Enable/disable file output
    void setFileOutput(const std::string& filepath) {
        std::lock_guard<std::mutex> lock(mutex_);
        fileOutput_ = filepath;
        fileStream_.open(filepath, std::ios::app);
    }
    
    // Disable file output
    void disableFileOutput() {
        std::lock_guard<std::mutex> lock(mutex_);
        if (fileStream_.is_open()) {
            fileStream_.close();
        }
        fileOutput_.clear();
    }
    
    // Save debug events to file
    void saveToFile(const std::string& filepath) {
        std::lock_guard<std::mutex> lock(mutex_);
        std::ofstream file(filepath);
        if (file.is_open()) {
            file << getReport();
        }
    }
    
private:
    DebugLevel level_ = DebugLevel::Warning;
    std::vector<DebugEvent> events_;
    mutable std::mutex mutex_;
    std::string fileOutput_;
    std::ofstream fileStream_;
    
    Debug() = default;
    ~Debug() = default;
    
    // Disable copying
    Debug(const Debug&) = delete;
    Debug& operator=(const Debug&) = delete;
    
    // Convert debug level to string
    std::string debugLevelToString(DebugLevel level) const {
        switch (level) {
            case DebugLevel::Off: return "OFF";
            case DebugLevel::Error: return "ERROR";
            case DebugLevel::Warning: return "WARN";
            case DebugLevel::Info: return "INFO";
            case DebugLevel::Debug: return "DEBUG";
            case DebugLevel::Trace: return "TRACE";
            default: return "UNKNOWN";
        }
    }
};

// Convenience macros for easy debugging
#define DEBUG_LOG(level, message) nlm::Debug::getInstance().log(level, message, __FILE__, __LINE__, __FUNCTION__)
#define DEBUG_ERROR(message) DEBUG_LOG(nlm::DebugLevel::Error, message)
#define DEBUG_WARNING(message) DEBUG_LOG(nlm::DebugLevel::Warning, message)
#define DEBUG_INFO(message) DEBUG_LOG(nlm::DebugLevel::Info, message)
#define DEBUG_DEBUG(message) DEBUG_LOG(nlm::DebugLevel::Debug, message)
#define DEBUG_TRACE(message) DEBUG_LOG(nlm::DebugLevel::Trace, message)

} // namespace nlm
