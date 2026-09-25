// Logger.cpp - Enhanced Logging System with Rotation and Structured Logging

#include "Logger.hpp"
#include <iostream>
#include <fstream>
#include <iomanip>
#include <sstream>
#include <mutex>
#include <algorithm>
#include <chrono>
#include <ctime>
#include <filesystem>
#include <thread>
#include <atomic>

namespace nlm {

// Helper functions for structured logging
namespace {
    std::string logFieldToString(const LogField& field) {
        std::ostringstream oss;
        std::visit([&oss](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, bool>) {
                oss << (arg ? "true" : "false");
            } else if constexpr (std::is_same_v<T, int64_t> || 
                                 std::is_same_v<T, int>) {
                oss << arg;
            } else if constexpr (std::is_same_v<T, double>) {
                oss << std::fixed << std::setprecision(6) << arg;
            } else if constexpr (std::is_same_v<T, std::string>) {
                oss << "\"" << arg << "\"";
            }
        }, field);
        return oss.str();
    }
    
    std::string escapeJSONString(const std::string& str) {
        std::ostringstream oss;
        for (char c : str) {
            switch (c) {
                case '\\': oss << "\\\\"; break;
                case '\"': oss << "\\\""; break;
                case '\n': oss << "\\n"; break;
                case '\r': oss << "\\r"; break;
                case '\t': oss << "\\t"; break;
                case '\b': oss << "\\b"; break;
                case '\f': oss << "\\f"; break;
                default:
                    if (c < 32 || c > 126) {
                        oss << "\\u" << std::hex << std::setw(4) << std::setfill('0') << (int)c;
                    } else {
                        oss << c;
                    }
            }
        }
        return oss.str();
    }
}

// ConsoleLogger implementation
struct ConsoleLogger::Impl {
    LogLevel level = LogLevel::Info;
    bool useColors = true;
    std::ofstream fileOut;
    std::mutex mutex;
    
    // Enhanced features
    LogFormat format = LogFormat::Text;
    std::string outputPath;
    size_t maxFileSize = 10 * 1024 * 1024; // 10MB default
    LogRotation rotationPolicy = LogRotation::None;
    std::string timePattern = "%Y%m%d";
    std::atomic<size_t> currentFileSize{0};
    std::string currentLogFile;
};

ConsoleLogger::ConsoleLogger() : pImpl(std::make_unique<Impl>()) {}

ConsoleLogger::ConsoleLogger(LogLevel level) : pImpl(std::make_unique<Impl>()) {
    pImpl->level = level;
}

ConsoleLogger::~ConsoleLogger() = default;

void ConsoleLogger::log(const LogEntry& entry) {
    if (entry.level < pImpl->level) {
        return;
    }
    
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    // Handle rotation if needed
    rotateIfNeeded();
    
    std::string formattedMessage;
    
    switch (pImpl->format) {
        case LogFormat::JSON:
            formattedMessage = formatEntryJSON(entry);
            break;
        default:
            formattedMessage = formatEntry(entry);
            break;
    }
    
    // Console output
    if (pImpl->useColors) {
        std::cout << levelToColor(entry.level) << formattedMessage << "\033[0m\n";
    } else {
        std::cout << formattedMessage << "\n";
    }
    
    // File output if set
    if (pImpl->fileOut.is_open()) {
        pImpl->fileOut << formattedMessage << "\n";
        pImpl->fileOut.flush();
        pImpl->currentFileSize += formattedMessage.length() + 1; // +1 for newline
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

void ConsoleLogger::setOutputFile(const std::string& filepath) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    closeOutputFile();
    pImpl->outputPath = filepath;
    
    // Create directory if it doesn't exist
    std::filesystem::path path(filepath);
    if (!std::filesystem::exists(path.parent_path())) {
        std::filesystem::create_directories(path.parent_path());
    }
    
    pImpl->fileOut.open(filepath, std::ios::app);
    if (pImpl->fileOut.is_open()) {
        pImpl->currentFileSize = pImpl->fileOut.tellp();
        pImpl->currentLogFile = filepath;
    }
}

void ConsoleLogger::closeOutputFile() {
    if (pImpl->fileOut.is_open()) {
        pImpl->fileOut.close();
        pImpl->currentFileSize = 0;
        pImpl->currentLogFile.clear();
    }
}

void ConsoleLogger::setMaxFileSize(size_t maxSize) {
    pImpl->maxFileSize = maxSize;
}

void ConsoleLogger::setRotationPolicy(LogRotation policy, size_t maxSize, const std::string& timePattern) {
    pImpl->rotationPolicy = policy;
    pImpl->maxFileSize = maxSize;
    pImpl->timePattern = timePattern;
}

void ConsoleLogger::setFormat(LogFormat format) {
    pImpl->format = format;
}

LogFormat ConsoleLogger::getFormat() const {
    return pImpl->format;
}

bool ConsoleLogger::shouldRotate() {
    switch (pImpl->rotationPolicy) {
        case LogRotation::SizeBased:
            return pImpl->currentFileSize >= pImpl->maxFileSize;
        case LogRotation::TimeBased:
            // For time-based rotation, check if we need a new file for today
            if (pImpl->currentLogFile.empty()) return false;
            std::filesystem::path path(pImpl->currentLogFile);
            std::time_t now = std::time(nullptr);
            std::tm tm = *std::localtime(&now);
            char dateStr[9];
            std::strftime(dateStr, sizeof(dateStr), pImpl->timePattern.c_str(), &tm);
            
            std::string expectedFile = path.parent_path().string() + "/" + 
                                      path.stem().string() + "_" + dateStr + ".log";
            return pImpl->currentLogFile != expectedFile;
        case LogRotation::SizeAndTime:
            if (pImpl->currentFileSize >= pImpl->maxFileSize) {
                std::filesystem::path path(pImpl->currentLogFile);
                std::time_t now = std::time(nullptr);
                std::tm tm = *std::localtime(&now);
                char dateStr[9];
                std::strftime(dateStr, sizeof(dateStr), pImpl->timePattern.c_str(), &tm);
                
                std::string expectedFile = path.parent_path().string() + "/" + 
                                          path.stem().string() + "_" + dateStr + ".log";
                return pImpl->currentLogFile != expectedFile;
            }
            return false;
        default:
            return false;
    }
}

void ConsoleLogger::rotateIfNeeded() {
    if (!shouldRotate()) return;
    
    closeOutputFile();
    
    if (pImpl->rotationPolicy != LogRotation::None) {
        std::filesystem::path path(pImpl->outputPath);
        std::time_t now = std::time(nullptr);
        std::tm tm = *std::localtime(&now);
        char dateStr[9];
        std::strftime(dateStr, sizeof(dateStr), pImpl->timePattern.c_str(), &tm);
        
        std::string rotatedFile = path.parent_path().string() + "/" + 
                                 path.stem().string() + "_" + dateStr + ".log";
        
        setOutputFile(rotatedFile);
    }
}

std::string ConsoleLogger::getLogFilePath() const {
    return pImpl->outputPath;
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
    switch (level) {
        case LogLevel::Debug: return "\033[36m";    // Cyan
        case LogLevel::Info: return "\033[32m";      // Green
        case LogLevel::Warning: return "\033[33m";   // Yellow
        case LogLevel::Error: return "\033[31m";     // Red
        case LogLevel::Critical: return "\033[35m"; // Magenta
        default: return "\033[0m";
    }
}

std::string ConsoleLogger::formatEntry(const LogEntry& entry) const {
    std::ostringstream oss;
    oss << "[" << levelToString(entry.level) << "] ";
    
    // Add timestamp
    auto time = std::chrono::system_clock::to_time_t(entry.timestamp);
    oss << std::put_time(std::localtime(&time), "%Y-%m-%d %H:%M:%S");
    
    oss << " " << entry.message;
    
    // Add structured fields if any
    if (!entry.fields.empty()) {
        oss << " {";
        bool first = true;
        for (const auto& field : entry.fields) {
            if (!first) oss << ", ";
            first = false;
            oss << field.first << "=" << logFieldToString(field.second);
        }
        oss << "}";
    }
    
    return oss.str();
}

std::string ConsoleLogger::formatEntryJSON(const LogEntry& entry) const {
    std::ostringstream oss;
    oss << "{";
    oss << "\"timestamp\": \"" << std::chrono::system_clock::to_time_t(entry.timestamp) << "\", ";
    oss << "\"level\": \"" << levelToString(entry.level) << "\", ";
    oss << "\"message\": \"" << escapeJSONString(entry.message) << "\"";
    
    if (!entry.fields.empty()) {
        oss << ", \"fields\": {";
        bool first = true;
        for (const auto& field : entry.fields) {
            if (!first) oss << ", ";
            first = false;
            oss << "\"" << field.first << "\": " << logFieldToString(field.second);
        }
        oss << "}";
    }
    
    oss << "}";
    return oss.str();
}

// Logger implementation
struct Logger::Impl {
    std::vector<std::shared_ptr<ILogger>> loggers;
    std::mutex mutex;
    
    // Enhanced features
    std::shared_ptr<ConsoleLogger> consoleLogger;
    std::string outputPath;
    bool autoCreateDirs = true;
    LogFormat defaultFormat = LogFormat::Text;
    size_t maxFileSize = 10 * 1024 * 1024;
    LogRotation rotationPolicy = LogRotation::None;
    std::string timePattern = "%Y%m%d";
};

Logger::Logger() : pImpl(std::make_unique<Impl>()) {
    pImpl->consoleLogger = std::make_shared<ConsoleLogger>();
    pImpl->consoleLogger->setFormat(pImpl->defaultFormat);
    pImpl->consoleLogger->setRotationPolicy(pImpl->rotationPolicy, pImpl->maxFileSize, pImpl->timePattern);
    addLogger(pImpl->consoleLogger);
}

Logger::~Logger() = default;

void Logger::addLogger(std::shared_ptr<ILogger> logger) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->loggers.push_back(logger);
}

void Logger::clearLoggers() {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->loggers.clear();
    pImpl->consoleLogger.reset();
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

void Logger::logStructured(LogLevel level, const std::string& message,
                           const std::map<std::string, LogField>& fields,
                           const std::string& file, int line, const std::string& function) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    LogEntry entry;
    entry.level = level;
    entry.message = message;
    entry.file = file;
    entry.line = line;
    entry.function = function;
    entry.timestamp = std::chrono::system_clock::now();
    entry.fields = fields;
    
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

void Logger::debugStructured(const std::string& message, const std::map<std::string, LogField>& fields, const std::string& file, int line, const std::string& function) {
    logStructured(LogLevel::Debug, message, fields, file, line, function);
}

void Logger::infoStructured(const std::string& message, const std::map<std::string, LogField>& fields, const std::string& file, int line, const std::string& function) {
    logStructured(LogLevel::Info, message, fields, file, line, function);
}

void Logger::warningStructured(const std::string& message, const std::map<std::string, LogField>& fields, const std::string& file, int line, const std::string& function) {
    logStructured(LogLevel::Warning, message, fields, file, line, function);
}

void Logger::errorStructured(const std::string& message, const std::map<std::string, LogField>& fields, const std::string& file, int line, const std::string& function) {
    logStructured(LogLevel::Error, message, fields, file, line, function);
}

void Logger::criticalStructured(const std::string& message, const std::map<std::string, LogField>& fields, const std::string& file, int line, const std::string& function) {
    logStructured(LogLevel::Critical, message, fields, file, line, function);
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

void Logger::setLogFile(const std::string& filepath) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->outputPath = filepath;
    pImpl->consoleLogger->setOutputFile(filepath);
}

void Logger::closeLogFile() {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->consoleLogger->closeOutputFile();
}

std::string Logger::getLogFilePath() const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    return pImpl->consoleLogger->getLogFilePath();
}

void Logger::setAutoCreateDirs(bool autoCreate) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->autoCreateDirs = autoCreate;
}

void Logger::setDefaultLogFormat(LogFormat format) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->defaultFormat = format;
    pImpl->consoleLogger->setFormat(format);
}

void Logger::setMaxFileSize(size_t maxSize) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->maxFileSize = maxSize;
    pImpl->consoleLogger->setMaxFileSize(maxSize);
}

void Logger::setRotationPolicy(LogRotation policy, size_t maxSize, const std::string& timePattern) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->rotationPolicy = policy;
    pImpl->maxFileSize = maxSize;
    pImpl->timePattern = timePattern;
    pImpl->consoleLogger->setRotationPolicy(policy, maxSize, timePattern);
}

// LogStream implementation
LogStream::LogStream(LogLevel level, const std::string& file, int line, const std::string& function)
    : level_(level), file_(file), line_(line), function_(function) {}

LogStream::~LogStream() {
    Logger::getGlobal().log(level_, oss_.str(), file_, line_, function_);
}

} // namespace nlm
