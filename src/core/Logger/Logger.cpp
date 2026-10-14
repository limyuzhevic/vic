#include "Logger.hpp"
#include <iostream>
#include <fstream>
#include <iomanip>
#include <sstream>
#include <mutex>

namespace nlm {

struct ConsoleLogger::Impl {
    LogLevel level = LogLevel::Info;
    bool useColors = true;
    std::ofstream fileOut;
    std::mutex mutex;
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
    
    std::ostringstream oss;
    oss << "[" << levelToString(entry.level) << "] ";
    
    // Add timestamp
    auto time = std::chrono::system_clock::to_time_t(entry.timestamp);
    oss << std::put_time(std::localtime(&time), "%Y-%m-%d %H:%M:%S");
    
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
    pImpl->fileOut.open(filepath, std::ios::app);
}

void ConsoleLogger::closeOutputFile() {
    if (pImpl->fileOut.is_open()) {
        pImpl->fileOut.close();
    }
}

#include "Logger.hpp"
#include <iostream>
#include <fstream>
#include <iomanip>
#include <sstream>
#include <mutex>
#include <chrono>
#include <ctime>
#include <iomanip>

namespace nlm {

struct ConsoleLogger::Impl {
    LogLevel level = LogLevel::Info;
    bool useColors = true;
    std::ofstream fileOut;
    std::mutex mutex;
    
    // Advanced logging features
    std::unordered_map<std::string, DebugLevel> categoryLevels;
    std::vector<std::string> categories;
    bool jsonOutput = false;
    std::string currentCorrelationID;
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
    
    std::ostringstream oss;
    oss << "[" << levelToString(entry.level) << "] ";
    
    // Add timestamp
    auto time = std::chrono::system_clock::to_time_t(entry.timestamp);
    oss << std::put_time(std::localtime(&time), "%Y-%m-%d %H:%M:%S");
    
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
    }
}

void ConsoleLogger::setLevel(LogLevel level) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
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
    pImpl->fileOut.open(filepath, std::ios::app);
}

void ConsoleLogger::closeOutputFile() {
    if (pImpl->fileOut.is_open()) {
        pImpl->fileOut.close();
    }
}

void ConsoleLogger::logStructured(const StructuredLogEntry& entry) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    std::ostringstream oss;
    
    if (pImpl->jsonOutput) {
        // JSON formatted output
        oss << "{\"timestamp\": \"" << std::chrono::system_clock::to_time_t(entry.timestamp) << "\", \"correlation_id\": \"" 
            << entry.correlation_id << "\", \"category\": \"" << entry.category << "\", \"level\": " 
            << static_cast<int>(entry.level) << ", \"message\": \"" << entry.message << "\"";
        
        if (!entry.fields.empty()) {
            oss << ", \"fields\": {";
            bool first = true;
            for (const auto& field : entry.fields) {
                if (!first) oss << ", ";
                oss << "\"" << field.first << "\": \"" << field.second << "\"";
                first = false;
            }
            oss << "}";
        }
        
        oss << "}\n";
        std::cout << oss.str();
    } else {
        // Standard format with enhanced debugging info
        oss << "[" << debugLevelToString(entry.level) << "] "
            << "CORRELATION: " << entry.correlation_id << " "
            << "CATEGORY: " << entry.category << " "
            << "TIMESTAMP: " << std::chrono::system_clock::to_time_t(entry.timestamp) << " "
            << "MESSAGE: " << entry.message << "\n";
        
        if (!entry.fields.empty()) {
            oss << "  FIELDS: ";
            bool first = true;
            for (const auto& field : entry.fields) {
                if (!first) oss << ", ";
                oss << field.first << "=" << field.second;
                first = false;
            }
            oss << "\n";
        }
        
        if (entry.perf_data) {
            oss << "  PERFORMANCE: " << entry.perf_data->name << " took " 
                << entry.perf_data->duration.count() << " seconds\n";
        }
        
        if (entry.memory_data) {
            oss << "  MEMORY: " << entry.memory_data->current_usage << " bytes, "
                << "peak: " << entry.memory_data->peak_usage << " bytes\n";
        }
        
        if (entry.neural_state_data) {
            oss << "  NEURAL: region=" << entry.neural_state_data->region_name 
                << " timestep=" << entry.neural_state_data->timestep << "\n";
        }
        
        std::cout << oss.str();
    }
    
    // File output if set
    if (pImpl->fileOut.is_open()) {
        pImpl->fileOut << oss.str();
        pImpl->fileOut.flush();
    }
}

void ConsoleLogger::setCategoryLevel(const std::string& category, DebugLevel level) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->categoryLevels[category] = level;
}

void ConsoleLogger::addCategory(const std::string& category) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    if (std::find(pImpl->categories.begin(), pImpl->categories.end(), category) == pImpl->categories.end()) {
        pImpl->categories.push_back(category);
    }
}

std::vector<std::string> ConsoleLogger::getCategories() const {
    return pImpl->categories;
}

void ConsoleLogger::enableJSONOutput(bool enable) {
    pImpl->jsonOutput = enable;
}

void ConsoleLogger::setCorrelationID(const std::string& id) {
    pImpl->currentCorrelationID = id;
}

std::string ConsoleLogger::getCurrentCorrelationID() const {
    return pImpl->currentCorrelationID;
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

DebugLevel ConsoleLogger::debugLevelToLogLevel(DebugLevel level) const {
    switch (level) {
        case DebugLevel::Trace: return DebugLevel::Trace;
        case DebugLevel::Debug: return DebugLevel::Debug;
        case DebugLevel::Info: return DebugLevel::Info;
        case DebugLevel::Warning: return DebugLevel::Warning;
        case DebugLevel::Error: return DebugLevel::Error;
        case DebugLevel::Critical: return DebugLevel::Critical;
        default: return DebugLevel::Info;
    }
}

std::string ConsoleLogger::debugLevelToString(DebugLevel level) const {
    switch (level) {
        case DebugLevel::Trace: return "TRACE";
        case DebugLevel::Debug: return "DEBUG";
        case DebugLevel::Info: return "INFO";
        case DebugLevel::Warning: return "WARN";
        case DebugLevel::Error: return "ERROR";
        case DebugLevel::Critical: return "CRIT";
        default: return "UNKNOWN";
    }
}

// Logger class implementation
struct Logger::Impl {
    std::vector<std::shared_ptr<ILogger>> loggers;
    std::mutex mutex;
    std::shared_ptr<IAdvancedLogger> advanced_logger;
    std::string current_correlation_id;
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

// LogStream implementation
LogStream::LogStream(LogLevel level, const std::string& file, int line, const std::string& function)
    : level_(level), file_(file), line_(line), function_(function) {}

LogStream::~LogStream() {
    Logger::getGlobal().log(level_, oss_.str(), file_, line_, function_);
}

} // namespace nlm
