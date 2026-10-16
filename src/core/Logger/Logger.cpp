#include "Logger.hpp"
#include <iostream>
#include <fstream>
#include <iomanip>
#include <sstream>
#include <mutex>
#include <chrono>
#include <ctime>
#include <thread>
#include <iomanip>

namespace nlm {

struct ConsoleLogger::Impl {
    LogLevel level = LogLevel::Info;
    bool useColors = true;
    bool useTimestamps = true;
    bool includeThreadId = false;
    std::string logFormat = "[%LEVEL] %TIMESTAMP% %MESSAGE%";
    std::ofstream fileOut;
    std::mutex mutex;
    std::chrono::steady_clock::time_point sessionStart;
    size_t maxFileSize = 10 * 1024 * 1024;  // 10MB
    size_t fileRotationCount = 5;
    size_t currentFileSize = 0;
    size_t fileIndex = 0;
    bool enablePerformanceTiming = false;
    std::unordered_map<std::string, std::chrono::steady_clock::time_point> timers;
};

ConsoleLogger::ConsoleLogger() : pImpl(std::make_unique<Impl>()) {
    pImpl->sessionStart = std::chrono::steady_clock::now();
}

ConsoleLogger::ConsoleLogger(LogLevel level) : pImpl(std::make_unique<Impl>()) {
    pImpl->level = level;
    pImpl->sessionStart = std::chrono::steady_clock::now();
}

ConsoleLogger::~ConsoleLogger() {
    closeOutputFile();
}

void ConsoleLogger::log(const LogEntry& entry) {
    if (entry.level < pImpl->level) {
        return;
    }
    
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    // Format the log message
    std::string formattedMessage = formatMessage(entry);
    
    // Console output
    std::cout << formattedMessage;
    
    // File output if set
    if (pImpl->fileOut.is_open()) {
        pImpl->fileOut << formattedMessage;
        pImpl->fileOut.flush();
        pImpl->currentFileSize += formattedMessage.size();
        
        // Check file rotation
        if (pImpl->currentFileSize >= pImpl->maxFileSize) {
            rotateLogFile();
        }
    }
}

void ConsoleLogger::setLevel(LogLevel level) {
    pImpl->level = level;
}

LogLevel ConsoleLogger::getLevel() const {
    return pImpl->level;
}

void ConsoleLogger::setUseColors(bool useColors) {
    pImpl->useColors = useColors;
}

bool ConsoleLogger::getUseColors() const {
    return pImpl->useColors;
}

void ConsoleLogger::setUseTimestamps(bool useTimestamps) {
    pImpl->useTimestamps = useTimestamps;
}

bool ConsoleLogger::getUseTimestamps() const {
    return pImpl->useTimestamps;
}

void ConsoleLogger::setIncludeThreadId(bool includeThreadId) {
    pImpl->includeThreadId = includeThreadId;
}

bool ConsoleLogger::getIncludeThreadId() const {
    return pImpl->includeThreadId;
}

void ConsoleLogger::setLogFormat(const std::string& format) {
    pImpl->logFormat = format;
}

std::string ConsoleLogger::getLogFormat() const {
    return pImpl->logFormat;
}

void ConsoleLogger::setOutputFile(const std::string& filepath) {
    closeOutputFile();
    pImpl->fileOut.open(filepath, std::ios::app);
    pImpl->currentFileSize = pImpl->fileOut.tellp();
    pImpl->fileIndex = extractFileIndex(filepath);
}

void ConsoleLogger::closeOutputFile() {
    if (pImpl->fileOut.is_open()) {
        pImpl->fileOut.close();
    }
}

void ConsoleLogger::setFileRotation(size_t maxSizeMB, size_t rotationCount) {
    pImpl->maxFileSize = maxSizeMB * 1024 * 1024;
    pImpl->fileRotationCount = rotationCount;
}

void ConsoleLogger::setEnablePerformanceTiming(bool enable) {
    pImpl->enablePerformanceTiming = enable;
}

void ConsoleLogger::startTimer(const std::string& name) {
    if (pImpl->enablePerformanceTiming) {
        pImpl->timers[name] = std::chrono::steady_clock::now();
    }
}

void ConsoleLogger::stopTimer(const std::string& name) {
    if (pImpl->enablePerformanceTiming && pImpl->timers.find(name) != pImpl->timers.end()) {
        auto start = pImpl->timers[name];
        auto end = std::chrono::steady_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
        
        log(LogEntry(LogLevel::Debug, "Performance: Timer '" + name + "' took " + 
                    std::to_string(duration.count()) + "ms"));
        
        pImpl->timers.erase(name);
    }
}

void ConsoleLogger::logPerformanceData() {
    if (!pImpl->enablePerformanceTiming) return;
    
    size_t totalTimers = pImpl->timers.size();
    size_t activeTimers = 0;
    
    for (const auto& pair : pImpl->timers) {
        auto start = pair.second;
        auto end = std::chrono::steady_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
        
        log(LogEntry(LogLevel::Debug, "Performance: Active timer '" + pair.first + 
                    "' running for " + std::to_string(duration.count()) + "ms"));
        
        activeTimers++;
    }
}

std::string ConsoleLogger::formatMessage(const LogEntry& entry) const {
    std::string result;
    
    // Replace placeholders in the log format
    result = pImpl->logFormat;
    
    // Replace %LEVEL%
    std::string levelStr = levelToString(entry.level);
    size_t pos = result.find("%LEVEL%");
    while (pos != std::string::npos) {
        result.replace(pos, 8, levelStr);
        pos = result.find("%LEVEL%", pos + levelStr.size());
    }
    
    // Replace %MESSAGE%
    pos = result.find("%MESSAGE%");
    while (pos != std::string::npos) {
        result.replace(pos, 10, entry.message);
        pos = result.find("%MESSAGE%", pos + entry.message.size());
    }
    
    // Replace %TIMESTAMP%
    if (pImpl->useTimestamps) {
        auto time = std::chrono::system_clock::to_time_t(entry.timestamp);
        std::string timestamp = std::put_time(std::localtime(&time), "%Y-%m-%d %H:%M:%S");
        pos = result.find("%TIMESTAMP%");
        while (pos != std::string::npos) {
            result.replace(pos, 12, timestamp);
            pos = result.find("%TIMESTAMP%", pos + timestamp.size());
        }
    }
    
    // Replace %THREAD_ID%
    if (pImpl->includeThreadId) {
        std::thread::id threadId = std::this_thread::get_id();
        std::string threadStr = std::to_string(reinterpret_cast<size_t>(threadId));
        pos = result.find("%THREAD_ID%");
        while (pos != std::string::npos) {
            result.replace(pos, 12, threadStr);
            pos = result.find("%THREAD_ID%", pos + threadStr.size());
        }
    }
    
    // Add color if enabled
    if (pImpl->useColors && entry.level != LogLevel::Debug) {
        result = levelToColor(entry.level) + result + "\033[0m";
    }
    
    // Add newline
    result += "\n";
    
    return result;
}

void ConsoleLogger::rotateLogFile() {
    if (!pImpl->fileOut.is_open()) return;
    
    pImpl->fileOut.close();
    
    // Close current file
    std::string basePath = pImpl->fileOut._file.__file;
    for (size_t i = pImpl->fileRotationCount - 1; i > 0; --i) {
        std::string source = basePath + "." + std::to_string(i - 1);
        std::string dest = basePath + "." + std::to_string(i);
        if (std::filesystem::exists(source)) {
            std::filesystem::rename(source, dest);
        }
    }
    
    // Move current file to backup
    std::filesystem::rename(basePath, basePath + ".1");
    
    // Reopen current file
    pImpl->fileOut.open(basePath, std::ios::app);
    pImpl->currentFileSize = 0;
    pImpl->fileIndex++;
}

std::string ConsoleLogger::extractFileIndex(const std::string& filepath) const {
    std::string filename = filepath;
    size_t dotPos = filename.find_last_of('.');
    if (dotPos != std::string::npos) {
        std::string ext = filename.substr(dotPos);
        filename = filename.substr(0, dotPos);
        
        // Look for .1, .2, etc. suffix
        for (size_t i = 1; i <= 100; ++i) {
            std::string indexStr = "." + std::to_string(i);
            if (filename.ends_with(indexStr)) {
                return filename.substr(0, filename.size() - indexStr.size());
            }
        }
    }
    return filepath;
}

std::string ConsoleLogger::levelToString(LogLevel level) const {
    switch (level) {
        case LogLevel::Debug: return "DEBUG";
        case LogLevel::Info: return "INFO";
        case LogLevel::Warning: return "WARN";
        case LogLevel::Error: return "ERROR";
        case LogLevel::Critical: return "CRIT";
        default: return "UNKNOWN";
    }
}

std::string ConsoleLogger::levelToColor(LogLevel level) const {
    if (!pImpl->useColors) return "";
    
    switch (level) {
        case LogLevel::Debug: return "\033[36m";    // Cyan
        case LogLevel::Info: return "\033[32m";      // Green
        case LogLevel::Warning: return "\033[33m";   // Yellow
        case LogLevel::Error: return "\033[31m";     // Red
        case LogLevel::Critical: return "\033[35m"; // Magenta
        default: return "";
    }
}

} // namespace nlm