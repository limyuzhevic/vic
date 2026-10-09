// Logger implementation

#include "Logger.hpp"
#include <fstream>
#include <iostream>
#include <chrono>
#include <iomanip>
#include <sstream>
#include <algorithm>
#include <mutex>
#include <thread>

namespace nlm {

struct Logger::Impl {
    LogLevel globalLevel;
    std::unordered_map<LogChannel, std::shared_ptr<ConsoleLogger>> channels;
    std::unordered_map<LogChannel, LogLevel> channelLevels;
    std::unordered_map<std::string, LogLevel> moduleLevels;
    std::string logFilepath;
    size_t maxLogFileSize;
    size_t maxLogFiles;
    std::vector<LogEntry> logBuffer;
    size_t maxBufferSize;
    std::mutex logMutex;
    std::chrono::system_clock::time_point lastRotation;
    std::unordered_map<std::string, double> performanceMetrics;
};

struct ConsoleLogger::Impl {
    LogLevel level;
    std::mutex logMutex;
    std::string format;
    bool colorEnabled;
};

Logger::Logger() : pImpl(new Impl) {
    pImpl->globalLevel = LogLevel::Info;
    pImpl->maxLogFileSize = 10 * 1024 * 1024; // 10 MB
    pImpl->maxLogFiles = 5;
    pImpl->maxBufferSize = 10000;
    pImpl->lastRotation = std::chrono::system_clock::now();
}

Logger::~Logger() {
    delete pImpl;
}

Logger::Logger(Logger&& other) noexcept : pImpl(other.pImpl) {
    other.pImpl = nullptr;
}

Logger& Logger::operator=(Logger&& other) noexcept {
    if (this != &other) {
        delete pImpl;
        pImpl = other.pImpl;
        other.pImpl = nullptr;
    }
    return *this;
}

Logger& Logger::getInstance() {
    static Logger instance;
    return instance;
}

void Logger::setGlobalLevel(LogLevel level) {
    pImpl->globalLevel = level;
}

LogLevel Logger::getGlobalLevel() const {
    return pImpl->globalLevel;
}

void Logger::setLogFile(const std::string& filepath) {
    pImpl->logFilepath = filepath;
}

void Logger::setLogMaxSize(size_t maxSize) {
    pImpl->maxLogFileSize = maxSize;
}

void Logger::setLogMaxFiles(size_t maxFiles) {
    pImpl->maxLogFiles = maxFiles;
}

bool Logger::addChannel(LogChannel channel, LogLevel level, const std::string& filepath) {
    std::shared_ptr<ConsoleLogger> logger(new ConsoleLogger(level));
    pImpl->channels[channel] = logger;
    pImpl->channelLevels[channel] = level;
    return true;
}

bool Logger::removeChannel(LogChannel channel) {
    auto it = pImpl->channels.find(channel);
    if (it != pImpl->channels.end()) {
        pImpl->channels.erase(it);
        pImpl->channelLevels.erase(channel);
        return true;
    }
    return false;
}

bool Logger::setChannelLevel(LogChannel channel, LogLevel level) {
    auto it = pImpl->channels.find(channel);
    if (it != pImpl->channels.end()) {
        it->second->setLevel(level);
        pImpl->channelLevels[channel] = level;
        return true;
    }
    return false;
}

LogLevel Logger::getChannelLevel(LogChannel channel) const {
    auto it = pImpl->channelLevels.find(channel);
    if (it != pImpl->channelLevels.end()) {
        return it->second;
    }
    return LogLevel::Info;
}

void Logger::log(LogLevel level, LogChannel channel, const std::string& module,
                 const std::string& message, const std::string& file, int line) {
    std::lock_guard<std::mutex> lock(pImpl->logMutex);
    
    // Check if log should be written
    if (!shouldLog(level, channel, module)) {
        return;
    }
    
    // Create log entry
    LogEntry entry;
    entry.timestamp = std::chrono::system_clock::now();
    entry.level = level;
    entry.channel = channel;
    entry.module = module;
    entry.message = message;
    entry.file = file;
    entry.line = line;
    
    // Add to buffer
    pImpl->logBuffer.push_back(entry);
    
    // Maintain buffer size
    if (pImpl->logBuffer.size() > pImpl->maxBufferSize) {
        pImpl->logBuffer.erase(pImpl->logBuffer.begin());
    }
    
    // Send to channels
    auto channelIt = pImpl->channels.find(channel);
    if (channelIt != pImpl->channels.end()) {
        channelIt->second->log(entry);
    }
    
    // Rotate logs if needed
    auto now = std::chrono::system_clock::now();
    if (std::chrono::duration_cast<std::chrono::hours>(now - pImpl->lastRotation).count() >= 24) {
        rotateLogs();
    }
}

void Logger::error(const std::string& module, const std::string& message) {
    log(LogLevel::Error, LogChannel::Error, module, message);
}

void Logger::warning(const std::string& module, const std::string& message) {
    log(LogLevel::Warning, LogChannel::Warning, module, message);
}

void Logger::info(const std::string& module, const std::string& message) {
    log(LogLevel::Info, LogChannel::System, module, message);
}

void Logger::debug(const std::string& module, const std::string& message) {
    log(LogLevel::Debug, LogChannel::Neural, module, message);
}

std::vector<LogEntry> Logger::getLogEntries(LogLevel minLevel, LogChannel channel,
                                           const std::string& module,
                                           const std::chrono::system_clock::time_point& since) const {
    std::vector<LogEntry> result;
    
    for (const auto& entry : pImpl->logBuffer) {
        if (entry.level < minLevel) continue;
        if (channel != LogChannel::Console && entry.channel != channel) continue;
        if (!module.empty() && entry.module != module) continue;
        if (since != std::chrono::system_clock::time_point::min() && entry.timestamp < since) continue;
        
        result.push_back(entry);
    }
    
    return result;
}

void Logger::setFilter(const std::string& pattern) {
    pImpl->moduleLevels.clear();
    
    // Simple pattern matching (can be enhanced)
    if (pattern.empty()) return;
    
    // Add exact module matches
    pImpl->moduleLevels[pattern] = LogLevel::Debug;
}

bool Logger::shouldLog(LogLevel level, LogChannel channel, const std::string& module) const {
    // Check global level
    if (level < pImpl->globalLevel) {
        return false;
    }
    
    // Check channel level
    auto channelIt = pImpl->channelLevels.find(channel);
    if (channelIt != pImpl->channelLevels.end() && level < channelIt->second) {
        return false;
    }
    
    // Check module level (if set)
    auto moduleIt = pImpl->moduleLevels.find(module);
    if (moduleIt != pImpl->moduleLevels.end() && level < moduleIt->second) {
        return false;
    }
    
    return true;
}

void Logger::rotateLogs() {
    // For simplicity, just clear old logs
    pImpl->logBuffer.clear();
    pImpl->lastRotation = std::chrono::system_clock::now();
}

void Logger::cleanupOldLogs() {
    auto now = std::chrono::system_clock::now();
    auto cutoff = now - std::chrono::hours(24);
    
    pImpl->logBuffer.erase(std::remove_if(pImpl->logBuffer.begin(), pImpl->logBuffer.end(),
                                          [cutoff](const LogEntry& entry) {
                                              return entry.timestamp < cutoff;
                                          }),
                          pImpl->logBuffer.end());
}

void Logger::configureFromFile(const std::string& filepath) {
    // For simplicity, just set global level
    // In a full implementation, this would parse a config file
    setGlobalLevel(LogLevel::Info);
}

void Logger::saveConfiguration(const std::string& filepath) const {
    // For simplicity, just save basic config
    std::ofstream file(filepath);
    if (file.is_open()) {
        file << "log_level = " << static_cast<int>(pImpl->globalLevel) << std::endl;
        file << "log_filepath = " << pImpl->logFilepath << std::endl;
        file << "max_buffer_size = " << pImpl->maxBufferSize << std::endl;
    }
}

void Logger::startPerformanceMeasurement(const std::string& operation) {
    // Start timing operation
    pImpl->performanceMetrics[operation] = 0.0;
}

void Logger::endPerformanceMeasurement(const std::string& operation) {
    // End timing operation
    if (pImpl->performanceMetrics.find(operation) != pImpl->performanceMetrics.end()) {
        pImpl->performanceMetrics[operation] = 0.0; // Placeholder
    }
}

std::string Logger::getLogStatistics() const {
    std::stringstream ss;
    ss << "Log Statistics:" << std::endl;
    ss << "  Global level: " << static_cast<int>(pImpl->globalLevel) << std::endl;
    ss << "  Buffer size: " << pImpl->logBuffer.size() << std::endl;
    ss << "  Channels: " << pImpl->channels.size() << std::endl;
    
    return ss.str();
}

size_t Logger::getEntryCount(LogLevel minLevel) const {
    size_t count = 0;
    for (const auto& entry : pImpl->logBuffer) {
        if (entry.level >= minLevel) {
            count++;
        }
    }
    return count;
}

ConsoleLogger::ConsoleLogger(LogLevel level) : pImpl(new Impl) {
    pImpl->level = level;
    pImpl->colorEnabled = false; // Can be enabled based on terminal support
}

ConsoleLogger::~ConsoleLogger() {
    delete pImpl;
}

void ConsoleLogger::log(const LogEntry& entry) {
    std::lock_guard<std::mutex> lock(pImpl->logMutex);
    
    if (entry.level < pImpl->level) {
        return;
    }
    
    // Format log message
    std::string timestamp = formatTime(entry.timestamp);
    std::string levelStr = levelToString(entry.level);
    std::string channelStr = channelToString(entry.channel);
    
    std::cout << "[" << timestamp << "] [" << levelStr << "] [" << channelStr 
              << "] [" << entry.module << "] " << entry.message << std::endl;
}

void ConsoleLogger::setLevel(LogLevel level) {
    pImpl->level = level;
}

LogLevel ConsoleLogger::getLevel() const {
    return pImpl->level;
}

std::string ConsoleLogger::formatTime(const std::chrono::system_clock::time_point& time) {
    auto time_t = std::chrono::system_clock::to_time_t(time);
    std::stringstream ss;
    ss << std::put_time(std::localtime(&time_t), "%Y-%m-%d %H:%M:%S");
    return ss.str();
}

std::string ConsoleLogger::levelToString(LogLevel level) {
    switch (level) {
        case LogLevel::Off: return "OFF";
        case LogLevel::Error: return "ERROR";
        case LogLevel::Warning: return "WARNING";
        case LogLevel::Info: return "INFO";
        case LogLevel::Debug: return "DEBUG";
        case LogLevel::Trace: return "TRACE";
        default: return "UNKNOWN";
    }
}

std::string ConsoleLogger::channelToString(LogChannel channel) {
    switch (channel) {
        case LogChannel::Console: return "CONSOLE";
        case LogChannel::File: return "FILE";
        case LogChannel::Network: return "NETWORK";
        case LogChannel::System: return "SYSTEM";
        case LogChannel::Neural: return "NEURAL";
        case LogChannel::Memory: return "MEMORY";
        case LogChannel::Plasticity: return "PLASTICITY";
        case LogChannel::Cognition: return "COGNITION";
        case LogChannel::Development: return "DEVELOPMENT";
        case LogChannel::Neuromodulation: return "NEUROMODULATION";
        default: return "UNKNOWN";
    }
}

} // namespace nlm
