#include "core/Logger/Logger.hpp"
#include <iostream>
#include <fstream>
#include <iomanip>
#include <sstream>
#include <mutex>
#include <chrono>
#include <ctime>
#include <algorithm>
#include <ctime>
#include <nlohmann/json.hpp>
#include <yaml-cpp/yaml.h>

namespace nlm {

// Helper function to convert LogLevel to string
std::string levelToString(LogLevel level) {
    switch (level) {
        case LogLevel::Debug: return "DEBUG";
        case LogLevel::Info: return "INFO";
        case LogLevel::Warning: return "WARN";
        case LogLevel::Error: return "ERROR";
        case LogLevel::Critical: return "CRIT";
        default: return "UNKNOWN";
    }
}

struct ConsoleLogger::Impl {
    LogLevel level = LogLevel::Info;
    bool useColors = true;
    std::ofstream fileOut;
    std::mutex mutex;
    std::string format = "default";
    std::string timestampFormat = "%Y-%m-%d %H:%M:%S";
    size_t maxFileSize = 10 * 1024 * 1024; // 10 MB
    size_t maxFileCount = 5;
    std::vector<std::function<bool(const LogEntry&)>> filters;
    std::map<std::string, std::function<void(const LogEntry&)>> patternSinks;
    std::map<std::string, std::function<void(const std::string&)>> fieldSinks;
};

ConsoleLogger::ConsoleLogger() : pImpl(std::make_unique<Impl>()) {}

ConsoleLogger::ConsoleLogger(LogLevel level) : pImpl(std::make_unique<Impl>()) {
    pImpl->level = level;
}

ConsoleLogger::~ConsoleLogger() = default;

void ConsoleLogger::log(const LogEntry& entry) {
    if (entry.level < pImpl->level || !shouldLog(entry)) {
        return;
    }
    
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    // Check filters
    for (const auto& filter : pImpl->filters) {
        if (!filter(entry)) {
            return;
        }
    }
    
    std::ostringstream oss;
    oss << "[" << levelToString(entry.level) << "] ";
    
    // Add timestamp
    auto time = std::chrono::system_clock::to_time_t(entry.timestamp);
    oss << std::put_time(std::localtime(&time), pImpl->timestampFormat.c_str());
    
    oss << " " << entry.message;
    
    std::string output = oss.str();
    
    // Console output
    if (pImpl->useColors) {
        std::cout << levelToColor(entry.level) << output << "\033[0m\n";
    } else {
        std::cout << output << "\n";
    }
    
    // File output if set
    if (pImpl->fileOut.is_open()) {
        pImpl->fileOut << output << "\n";
        pImpl->fileOut.flush();
        
        // Check file size and rotate if needed
        pImpl->fileOut.seekp(0, std::ios::end);
        if (pImpl->fileOut.tellp() > static_cast<std::streampos>(pImpl->maxFileSize)) {
            rotateLog();
        }
    }
    
    // Trigger pattern sinks
    for (const auto& sink : pImpl->patternSinks) {
        if (output.find(sink.first) != std::string::npos) {
            sink.second(entry);
        }
    }
}

void ConsoleLogger::log(const StructuredLogEntry& entry) {
    LogEntry logEntry;
    logEntry.level = entry.level;
    logEntry.message = entry.message;
    logEntry.file = entry.file;
    logEntry.line = entry.line;
    logEntry.function = entry.function;
    logEntry.timestamp = entry.timestamp;
    
    // Log as regular log entry
    log(logEntry);
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

void ConsoleLogger::setOutputFile(const std::string& filepath) {
    closeOutputFile();
    pImpl->fileOut.open(filepath, std::ios::app);
}

void ConsoleLogger::closeOutputFile() {
    if (pImpl->fileOut.is_open()) {
        pImpl->fileOut.close();
    }
}

bool ConsoleLogger::isFileOutputEnabled() const {
    return pImpl->fileOut.is_open();
}

void ConsoleLogger::setMaxFileSize(size_t maxSize) {
    pImpl->maxFileSize = maxSize;
}

void ConsoleLogger::setMaxFileCount(size_t maxCount) {
    pImpl->maxFileCount = maxCount;
}

void ConsoleLogger::rotateLog() {
    if (!pImpl->fileOut.is_open()) {
        return;
    }
    
    pImpl->fileOut.close();
    
    // Close current file
    std::string baseName = "logger"; // Would need actual filename
    for (size_t i = pImpl->maxFileCount; i > 1; --i) {
        std::string from = baseName + "." + std::to_string(i - 1) + ".log";
        std::string to = baseName + "." + std::to_string(i) + ".log";
        // In production, would actually rename files
    }
    
    // Reopen new log file
    pImpl->fileOut.open(baseName + ".log", std::ios::app);
}

void ConsoleLogger::addFilter(std::function<bool(const LogEntry&)> filter) {
    pImpl->filters.push_back(filter);
}

void ConsoleLogger::clearFilters() {
    pImpl->filters.clear();
}

void ConsoleLogger::addPatternSink(const std::string& pattern, std::function<void(const LogEntry&)> sink) {
    pImpl->patternSinks[pattern] = sink;
}

void ConsoleLogger::addFieldSink(const std::string& field, std::function<void(const std::string&)> sink) {
    pImpl->fieldSinks[field] = sink;
}

std::string ConsoleLogger::levelToString(LogLevel level) const {
    return nlm::levelToString(level);
}

std::string ConsoleLogger::levelToColor(LogLevel level) const {
    switch (level) {
        case LogLevel::Debug: return "\033[36m";    // Cyan
        case LogLevel::Info: return "\033[32m";      // Green
        case LogLevel::Warning: return "\033[33m";   // Yellow
        case LogLevel::Error: return "\033[31m";     // Red
        case LogLevel::Critical: return "\033[35m"; // Magenta
        default: return "\033[0m";
    }
}

std::string StructuredLogEntry::toJSON() const {
    nlohmann::json json;
    
    json["timestamp"] = std::chrono::system_clock::to_time_t(timestamp);
    json["level"] = static_cast<int>(level);
    json["message"] = message;
    json["file"] = file;
    json["line"] = line;
    json["function"] = function;
    json["thread_id"] = threadId;
    json["fields"] = fields;
    
    return json.dump(2);
}

std::string StructuredLogEntry::toYAML() const {
    YAML::Emitter yaml;
    
    yaml << YAML::BeginMap;
    yaml << YAML::Key << "timestamp" << YAML::Value << std::chrono::system_clock::to_time_t(timestamp);
    yaml << YAML::Key << "level" << YAML::Value << static_cast<int>(level);
    yaml << YAML::Key << "message" << YAML::Value << message;
    yaml << YAML::Key << "file" << YAML::Value << file;
    yaml << YAML::Key << "line" << YAML::Value << line;
    yaml << YAML::Key << "function" << YAML::Value << function;
    yaml << YAML::Key << "thread_id" << YAML::Value << threadId;
    
    if (!fields.empty()) {
        yaml << YAML::Key << "fields" << YAML::Value << YAML::BeginMap;
        for (const auto& field : fields) {
            yaml << YAML::Key << field.first << YAML::Value << field.second;
        }
        yaml << YAML::EndMap;
    }
    
    yaml << YAML::EndMap;
    
    return yaml.c_str();
}

std::string StructuredLogEntry::toHumanReadable() const {
    std::stringstream ss;
    
    auto time = std::chrono::system_clock::to_time_t(timestamp);
    ss << "[" << std::put_time(std::localtime(&time), "%Y-%m-%d %H:%M:%S") << "] ";
    ss << "[" << levelToString(level) << "] ";
    ss << message;
    
    if (!fields.empty()) {
        ss << " [";
        bool first = true;
        for (const auto& field : fields) {
            if (!first) ss << ", ";
            ss << field.first << "=" << field.second;
            first = false;
        }
        ss << "]";
    }
    
    return ss.str();
}

struct Logger::Impl {
    std::vector<std::shared_ptr<ILogger>> loggers;
    std::mutex mutex;
    
    bool shouldLog(const LogEntry& entry) const {
        if (filters.empty()) return true;
        
        for (const auto& filter : filters) {
            if (!filter(entry)) {
                return false;
            }
        }
        
        return true;
    }
    
    std::vector<std::function<bool(const LogEntry&)>> filters;
};

Logger::Logger() : pImpl(std::make_unique<Impl>()) {}

Logger::~Logger() = default;

void Logger::addLogger(std::shared_ptr<ILogger> logger) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->loggers.push_back(logger);
}

void Logger::clearLoggers() {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->loggers.clear();
}

void Logger::setLevel(LogLevel level) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    for (auto& logger : pImpl->loggers) {
        logger->setLevel(level);
    }
}

void Logger::log(LogLevel level, const std::string& message,
                 const std::string& file, int line, const std::string& function) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    LogEntry entry;
    entry.level = level;
    entry.message = message;
    entry.file = file;
    entry.line = line;
    entry.function = function;
    entry.timestamp = std::chrono::system_clock::now();
    
    for (auto& logger : pImpl->loggers) {
        logger->log(entry);
    }
}

void Logger::debug(const std::string& message, const std::string& file, int line, const std::string& function) {
    log(LogLevel::Debug, message, file, line, function);
}

void Logger::info(const std::string& message, const std::string& file, int line, const std::string& function) {
    log(LogLevel::Info, message, file, line, function);
}

void Logger::warning(const std::string& message, const std::string& file, int line, const std::string& function) {
    log(LogLevel::Warning, message, file, line, function);
}

void Logger::error(const std::string& message, const std::string& file, int line, const std::string& function) {
    log(LogLevel::Error, message, file, line, function);
}

void Logger::critical(const std::string& message, const std::string& file, int line, const std::string& function) {
    log(LogLevel::Critical, message, file, line, function);
}

bool Logger::isEnabled(LogLevel level) const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    for (auto& logger : pImpl->loggers) {
        if (logger->getLevel() <= level) {
            return true;
        }
    }
    return false;
}

Logger& Logger::getGlobal() {
    static Logger globalLogger;
    return globalLogger;
}

void Logger::setGlobal(std::shared_ptr<Logger> logger) {
    // This is a placeholder for advanced global logger management
    // In practice, we just use getGlobal() which is already static
}

// Helper functions
template<typename T>
std::string toString(const T& value) {
    std::stringstream ss;
    ss << value;
    return ss.str();
}

template std::string toString<int>(const int& value);
template std::string toString<double>(const double& value);
template std::string toString<std::string>(const std::string& value);

} // namespace nlm
