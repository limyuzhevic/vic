#include "Logger.hpp"
#include <iostream>
#include <fstream>
#include <iomanip>
#include <sstream>
#include <mutex>
#include <stdexcept>
#include <chrono>
#include <filesystem>

namespace nlm {

namespace LoggerConstants {
    constexpr size_t MAX_LOG_MESSAGE_LENGTH = 4096;
    constexpr size_t MAX_FILE_PATH_LENGTH = 1024;
    constexpr size_t MAX_LOG_LEVELS = 5;
    constexpr size_t MAX_LOG_QUEUES = 100;
    
    const std::vector<std::string> LOG_LEVEL_NAMES = {"DEBUG", "INFO", "WARN", "ERROR", "CRIT"};
    
    const std::map<std::string, LogLevel> STRING_TO_LEVEL = {
        {"debug", LogLevel::Debug},
        {"info", LogLevel::Info},
        {"warning", LogLevel::Warning},
        {"warn", LogLevel::Warning},
        {"error", LogLevel::Error},
        {"critical", LogLevel::Critical},
        {"crit", LogLevel::Critical}
    };
}

struct ConsoleLogger::Impl {
    LogLevel level = LogLevel::Info;
    bool useColors = true;
    std::ofstream fileOut;
    std::mutex mutex;
    std::string outputFile;
    bool fileError = false;
    
    Impl() : fileError(false) {}
    
    ~Impl() {
        if (fileOut.is_open()) {
            fileOut.close();
        }
    }
};

ConsoleLogger::ConsoleLogger() : pImpl(std::make_unique<Impl>()) {
    NLM_LOG_INFO("ConsoleLogger: Initialized with default level " + 
                 LoggerConstants::LOG_LEVEL_NAMES[static_cast<size_t>(pImpl->level)]);
}

ConsoleLogger::ConsoleLogger(LogLevel level) : pImpl(std::make_unique<Impl>()) {
    if (level < LogLevel::Debug || level > LogLevel::Critical) {
        NLM_LOG_WARNING("ConsoleLogger: Invalid log level provided, using default (Info)");
        level = LogLevel::Info;
    }
    
    pImpl->level = level;
    NLM_LOG_INFO("ConsoleLogger: Initialized with level " + 
                 LoggerConstants::LOG_LEVEL_NAMES[static_cast<size_t>(pImpl->level)]);
}

ConsoleLogger::~ConsoleLogger() {
    if (pImpl->fileOut.is_open()) {
        pImpl->fileOut.close();
        NLM_LOG_DEBUG("ConsoleLogger: Closed output file");
    }
}

void ConsoleLogger::log(const LogEntry& entry) {
    // Validate entry
    if (entry.level < LogLevel::Debug || entry.level > LogLevel::Critical) {
        NLM_LOG_ERROR("ConsoleLogger::log: Invalid log level in entry: " + 
                     std::to_string(static_cast<int>(entry.level)));
        return;
    }
    
    if (entry.message.length() > LoggerConstants::MAX_LOG_MESSAGE_LENGTH) {
        NLM_LOG_ERROR("ConsoleLogger::log: Log message too long: " + 
                     std::to_string(entry.message.length()) + " characters (max: " + 
                     std::to_string(LoggerConstants::MAX_LOG_MESSAGE_LENGTH) + ")");
        // Truncate for safety
        LogEntry truncatedEntry = entry;
        truncatedEntry.message = entry.message.substr(0, LoggerConstants::MAX_LOG_MESSAGE_LENGTH - 100) + 
                                 "[TRUNCATED]";
        logInternal(truncatedEntry);
        return;
    }
    
    logInternal(entry);
}

void ConsoleLogger::logInternal(const LogEntry& entry) {
    if (entry.level < pImpl->level) {
        return;
    }
    
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    std::ostringstream oss;
    oss << "[" << LoggerConstants::LOG_LEVEL_NAMES[static_cast<size_t>(entry.level)] << "] ";
    
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
        try {
            pImpl->fileOut << output << "\n";
            pImpl->fileOut.flush();
        } catch (const std::exception& e) {
            if (!pImpl->fileError) {
                NLM_LOG_ERROR("ConsoleLogger::log: Failed to write to log file: " + std::string(e.what()));
                pImpl->fileError = true;
            }
        }
    }
}

void ConsoleLogger::setLevel(LogLevel level) {
    if (level < LogLevel::Debug || level > LogLevel::Critical) {
        NLM_LOG_WARNING("ConsoleLogger::setLevel: Invalid log level " + 
                        std::to_string(static_cast<int>(level)) + ", ignoring");
        return;
    }
    
    pImpl->level = level;
    NLM_LOG_INFO("ConsoleLogger::setLevel: Set level to " + 
                 LoggerConstants::LOG_LEVEL_NAMES[static_cast<size_t>(level)]);
}

LogLevel ConsoleLogger::getLevel() const {
    return pImpl->level;
}

void ConsoleLogger::setUseColors(bool useColors) {
    pImpl->useColors = useColors;
    NLM_LOG_DEBUG("ConsoleLogger::setUseColors: Set colors to " + std::string(useColors ? "enabled" : "disabled"));
}

bool ConsoleLogger::getUseColors() const {
    return pImpl->useColors;
}

void ConsoleLogger::setOutputFile(const std::string& filepath) {
    if (filepath.empty()) {
        NLM_LOG_WARNING("ConsoleLogger::setOutputFile: Empty filepath provided");
        return;
    }
    
    if (filepath.length() > LoggerConstants::MAX_FILE_PATH_LENGTH) {
        NLM_LOG_ERROR("ConsoleLogger::setOutputFile: File path too long: " + 
                     std::to_string(filepath.length()) + " characters");
        return;
    }
    
    // Close existing file
    if (pImpl->fileOut.is_open()) {
        pImpl->fileOut.close();
    }
    
    // Create directory if needed
    std::filesystem::path filePath(filepath);
    std::error_code ec;
    if (filePath.has_parent_path()) {
        std::filesystem::create_directories(filePath.parent_path(), ec);
        if (ec) {
            NLM_LOG_ERROR("ConsoleLogger::setOutputFile: Failed to create directory " + 
                         filePath.parent_path().string() + ": " + ec.message());
            return;
        }
    }
    
    // Open file
    pImpl->fileOut.open(filepath, std::ios::app);
    if (!pImpl->fileOut.is_open()) {
        NLM_LOG_ERROR("ConsoleLogger::setOutputFile: Failed to open log file: " + filepath);
        return;
    }
    
    pImpl->outputFile = filepath;
    pImpl->fileError = false;
    
    NLM_LOG_INFO("ConsoleLogger::setOutputFile: Set output file to " + filepath);
}

void ConsoleLogger::closeOutputFile() {
    if (pImpl->fileOut.is_open()) {
        pImpl->fileOut.close();
        NLM_LOG_DEBUG("ConsoleLogger::closeOutputFile: Closed output file " + pImpl->outputFile);
        pImpl->outputFile.clear();
    }
}

std::string ConsoleLogger::levelToString(LogLevel level) const {
    size_t levelIndex = static_cast<size_t>(level);
    if (levelIndex < LoggerConstants::LOG_LEVEL_NAMES.size()) {
        return LoggerConstants::LOG_LEVEL_NAMES[levelIndex];
    }
    return "UNKNOWN";
}

std::string ConsoleLogger::levelToColor(LogLevel level) const {
    if (!pImpl->useColors) {
        return "";
    }
    
    switch (level) {
        case LogLevel::Debug: return "\033[36m";    // Cyan
        case LogLevel::Info: return "\033[32m";      // Green
        case LogLevel::Warning: return "\033[33m";   // Yellow
        case LogLevel::Error: return "\033[31m";     // Red
        case LogLevel::Critical: return "\033[35m"; // Magenta
        default: return "\033[0m";
    }
}

// Logger class implementation
struct Logger::Impl {
    std::vector<std::shared_ptr<ILogger>> loggers;
    std::mutex mutex;
    std::string name;
    bool initialized;
    
    Impl() : initialized(false) {
        name = "NLM Logger";
    }
};

Logger::Logger() : pImpl(std::make_unique<Impl>()) {
    pImpl->initialized = true;
    NLM_LOG_INFO("Logger: Initialized with name '" + pImpl->name + "'");
}

Logger::~Logger() {
    clearLoggers();
}

void Logger::addLogger(std::shared_ptr<ILogger> logger) {
    if (!logger) {
        NLM_LOG_ERROR("Logger::addLogger: Attempted to add null logger");
        return;
    }
    
    if (pImpl->loggers.size() >= LoggerConstants::MAX_LOG_QUEUES) {
        NLM_LOG_ERROR("Logger::addLogger: Maximum log queue size reached (" + 
                     std::to_string(LoggerConstants::MAX_LOG_QUEUES) + ")");
        return;
    }
    
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->loggers.push_back(logger);
    NLM_LOG_INFO("Logger::addLogger: Added logger, total loggers: " + std::to_string(pImpl->loggers.size()));
}

void Logger::clearLoggers() {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    size_t oldCount = pImpl->loggers.size();
    pImpl->loggers.clear();
    
    if (oldCount > 0) {
        NLM_LOG_INFO("Logger::clearLoggers: Cleared " + std::to_string(oldCount) + " loggers");
    }
}

void Logger::setLevel(LogLevel level) {
    if (level < LogLevel::Debug || level > LogLevel::Critical) {
        NLM_LOG_ERROR("Logger::setLevel: Invalid log level " + std::to_string(static_cast<int>(level)));
        return;
    }
    
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    for (auto& logger : pImpl->loggers) {
        try {
            logger->setLevel(level);
        } catch (const std::exception& e) {
            NLM_LOG_ERROR("Logger::setLevel: Failed to set level on logger: " + std::string(e.what()));
        }
    }
    
    NLM_LOG_INFO("Logger::setLevel: Set all loggers to level " + 
                 LoggerConstants::LOG_LEVEL_NAMES[static_cast<size_t>(level)]);
}

void Logger::log(LogLevel level, const std::string& message,
                const std::string& file, int line, const std::string& function) {
    if (!pImpl->initialized) {
        NLM_LOG_ERROR("Logger::log: Logger not initialized");
        return;
    }
    
    if (level < LogLevel::Debug || level > LogLevel::Critical) {
        NLM_LOG_ERROR("Logger::log: Invalid log level " + std::to_string(static_cast<int>(level)));
        return;
    }
    
    if (message.empty()) {
        NLM_LOG_ERROR("Logger::log: Empty message provided");
        return;
    }
    
    if (message.length() > LoggerConstants::MAX_LOG_MESSAGE_LENGTH) {
        NLM_LOG_ERROR("Logger::log: Log message too long: " + 
                     std::to_string(message.length()) + " characters");
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
        try {
            logger->log(entry);
        } catch (const std::exception& e) {
            NLM_LOG_ERROR("Logger::log: Failed to log to logger: " + std::string(e.what()));
        }
    }
}

void Logger::debug(const std::string& message, const std::string& file, int line, const std::string& function) {
    if (isEnabled(LogLevel::Debug)) {
        log(LogLevel::Debug, message, file, line, function);
    }
}

void Logger::info(const std::string& message, const std::string& file, int line, const std::string& function) {
    if (isEnabled(LogLevel::Info)) {
        log(LogLevel::Info, message, file, line, function);
    }
}

void Logger::warning(const std::string& message, const std::string& file, int line, const std::string& function) {
    if (isEnabled(LogLevel::Warning)) {
        log(LogLevel::Warning, message, file, line, function);
    }
}

void Logger::error(const std::string& message, const std::string& file, int line, const std::string& function) {
    if (isEnabled(LogLevel::Error)) {
        log(LogLevel::Error, message, file, line, function);
    }
}

void Logger::critical(const std::string& message, const std::string& file, int line, const std::string& function) {
    if (isEnabled(LogLevel::Critical)) {
        log(LogLevel::Critical, message, file, line, function);
    }
}

bool Logger::isEnabled(LogLevel level) const {
    if (level < LogLevel::Debug || level > LogLevel::Critical) {
        return false;
    }
    
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    for (auto& logger : pImpl->loggers) {
        try {
            if (logger->getLevel() <= level) {
                return true;
            }
        } catch (const std::exception& e) {
            NLM_LOG_ERROR("Logger::isEnabled: Error checking logger level: " + std::string(e.what()));
        }
    }
    
    return false;
}

Logger& Logger::getGlobal() {
    static Logger globalLogger;
    return globalLogger;
}

void Logger::setGlobal(std::shared_ptr<Logger> logger) {
    if (!logger) {
        NLM_LOG_ERROR("Logger::setGlobal: Attempted to set null global logger");
        return;
    }
    
    // This is a placeholder for advanced global logger management
    // In practice, we just use getGlobal() which is already static
    NLM_LOG_WARNING("Logger::setGlobal: This function is a placeholder - use getGlobal() for global access");
}

// LogStream implementation
LogStream::LogStream(LogLevel level, const std::string& file, int line, const std::string& function)
    : level_(level), file_(file), line_(line), function_(function) {
    
    if (level_ < LogLevel::Debug || level_ > LogLevel::Critical) {
        NLM_LOG_ERROR("LogStream: Invalid log level " + std::to_string(static_cast<int>(level_)));
    }
}

LogStream::~LogStream() {
    try {
        Logger::getGlobal().log(level_, oss_.str(), file_, line_, function_);
    } catch (const std::exception& e) {
        std::cerr << "LogStream: Failed to log stream: " << e.what() << std::endl;
    }
}

} // namespace nlm
