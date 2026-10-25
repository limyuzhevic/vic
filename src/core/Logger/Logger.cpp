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
    
    // Log rotation settings
    bool enableRotation = false;
    size_t maxFileSize = 10 * 1024 * 1024; // 10 MB
    size_t currentFileSize = 0;
    int rotationCount = 0;
    std::string baseFilename;
    
    // Log filtering
    std::unordered_map<LogLevel, bool> levelFilters;
    
    // Timestamp format options
    std::string timestampFormat;
};

ConsoleLogger::ConsoleLogger() : pImpl(std::make_unique<Impl>()) {
    // Initialize level filters for all levels
    for (int i = 0; i <= 4; ++i) {
        pImpl->levelFilters[static_cast<LogLevel>(i)] = true;
    }
    
    // Default timestamp format
    pImpl->timestampFormat = "%Y-%m-%d %H:%M:%S";
}

ConsoleLogger::ConsoleLogger(LogLevel level) : pImpl(std::make_unique<Impl>()) {
    pImpl->level = level;
    // Initialize level filters for all levels
    for (int i = 0; i <= 4; ++i) {
        pImpl->levelFilters[static_cast<LogLevel>(i)] = true;
    }
    
    pImpl->timestampFormat = "%Y-%m-%d %H:%M:%S";
}

ConsoleLogger::~ConsoleLogger() {
    if (pImpl->fileOut.is_open()) {
        pImpl->fileOut.close();
    }
}

void ConsoleLogger::log(const LogEntry& entry) {
    if (entry.level < pImpl->level) {
        return;
    }
    
    // Check level filters
    if (!pImpl->levelFilters[entry.level]) {
        return;
    }
    
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    std::ostringstream oss;
    oss << "[" << levelToString(entry.level) << "] ";
    
    // Add timestamp
    auto time = std::chrono::system_clock::to_time_t(entry.timestamp);
    if (!pImpl->timestampFormat.empty()) {
        struct tm timeinfo;
        localtime_s(&timeinfo, &time);
        char buffer[100];
        strftime(buffer, sizeof(buffer), pImpl->timestampFormat.c_str(), &timeinfo);
        oss << buffer << " ";
    }
    
    // Add source location if present
    if (!entry.file.empty() && entry.line > 0) {
        oss << "(" << entry.file << ":" << entry.line << ") ";
    }
    if (!entry.function.empty()) {
        oss << "[" << entry.function << "] ";
    }
    
    oss << entry.message;
    
    std::string output = oss.str();
    
    // Apply log rotation if enabled
    if (pImpl->enableRotation && !pImpl->baseFilename.empty()) {
        // Check if file needs rotation
        if (pImpl->fileOut.is_open()) {
            pImpl->currentFileSize += output.size() + 1; // +1 for newline
        }
        
        if (pImpl->currentFileSize >= pImpl->maxFileSize) {
            // Rotate files
            pImpl->fileOut.close();
            
            // Rotate existing files
            for (int i = pImpl->rotationCount; i >= 0; --i) {
                std::string oldName = pImpl->baseFilename;
                if (i > 0) {
                    oldName += "." + std::to_string(i);
                }
                std::string newName = pImpl->baseFilename;
                if (i < pImpl->rotationCount) {
                    newName += "." + std::to_string(i + 1);
                }
                
                if (std::filesystem::exists(oldName)) {
                    std::filesystem::rename(oldName, newName);
                }
            }
            
            // Create new file
            pImpl->fileOut.open(pImpl->baseFilename, std::ios::app);
            pImpl->currentFileSize = 0;
            pImpl->rotationCount = std::min(pImpl->rotationCount + 1, 99);
        }
    }
    
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

void ConsoleLogger::enableLogRotation(bool enable) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->enableRotation = enable;
}

void ConsoleLogger::setMaxFileSize(size_t maxSize) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->maxFileSize = maxSize;
}

void ConsoleLogger::setTimestampFormat(const std::string& format) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->timestampFormat = format;
}

void ConsoleLogger::setLevelFilter(LogLevel level, bool enabled) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->levelFilters[level] = enabled;
}

bool ConsoleLogger::getLevelFilter(LogLevel level) const {
    return pImpl->levelFilters[level];
}

// Logger class implementation
struct Logger::Impl {
    std::vector<std::shared_ptr<ILogger>> loggers;
    std::mutex mutex;
};

Logger::~Logger() = default;

void Logger::addLogger(std::shared_ptr<ILogger> logger) {
    if (!logger) {
        NLM_LOG_ERROR("Attempted to add null logger");
        return;
    }
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->loggers.push_back(logger);
    NLM_LOG_DEBUG("Added logger: " + std::to_string(pImpl->loggers.size()));
}

void Logger::clearLoggers() {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    if (!pImpl->loggers.empty()) {
        NLM_LOG_INFO("Clearing all loggers (" + std::to_string(pImpl->loggers.size()) + " loggers)");
        pImpl->loggers.clear();
    }
}

void Logger::setLevel(LogLevel level) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    for (auto& logger : pImpl->loggers) {
        logger->setLevel(level);
    }
}

void Logger::log(LogLevel level, const std::string& message,
                const std::string& file, int line, const std::string& function) {
    if (!isEnabled(level)) {
        return;
    }
    
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
    
    // Auto-add a console logger to the global logger if none exists
    auto& logger = globalLogger;
    std::lock_guard<std::mutex> lock(logger.pImpl->mutex);
    if (logger.pImpl->loggers.empty()) {
        logger.addLogger(std::make_shared<ConsoleLogger>(LogLevel::Info));
    }
    
    return logger;
}

void Logger::setGlobal(std::shared_ptr<Logger> logger) {
    // Create a thread-safe implementation
    static std::mutex globalMutex;
    std::lock_guard<std::mutex> lock(globalMutex);
    
    // Clear existing loggers
    getGlobal().clearLoggers();
    
    if (logger) {
        // Add the provided logger
        getGlobal().addLogger(std::make_shared<ConsoleLogger>(LogLevel::Info));
        // Note: In practice, we'd want to preserve any custom loggers
        // This is a simplified implementation
    }
}

// LogStream implementation
LogStream::LogStream(LogLevel level, const std::string& file, int line, const std::string& function)
    : level_(level), file_(file), line_(line), function_(function) {}

LogStream::~LogStream() {
    Logger::getGlobal().log(level_, oss_.str(), file_, line_, function_);
}

} // namespace nlm
