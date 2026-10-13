#include "Logger.hpp"
#include <iostream>
#include <fstream>
#include <iomanip>
#include <sstream>
#include <mutex>
#include <thread>
#include <chrono>
#include <algorithm>
#include <filesystem>
#include <iomanip>
#include <ctime>

namespace nlm {

// Configuration file parsing helpers
namespace {
    std::string trim(const std::string& str) {
        size_t first = str.find_first_not_of(" \t\r\n");
        if (first == std::string::npos) return "";
        size_t last = str.find_last_not_of(" \t\r\n");
        return str.substr(first, (last - first + 1));
    }
    
    std::string toLower(const std::string& str) {
        std::string result = str;
        std::transform(result.begin(), result.end(), result.begin(), ::tolower);
        return result;
    }
    
    bool stringToBool(const std::string& str) {
        std::string lower = toLower(str);
        return lower == "true" || lower == "yes" || lower == "1" || lower == "on";
    }
    
    LogLevel stringToLogLevel(const std::string& str) {
        std::string lower = toLower(str);
        if (lower == "debug") return LogLevel::Debug;
        if (lower == "info") return LogLevel::Info;
        if (lower == "warning") return LogLevel::Warning;
        if (lower == "error") return LogLevel::Error;
        if (lower == "critical") return LogLevel::Critical;
        return LogLevel::Info; // Default
    }
}}

struct ConsoleLogger::Impl {
    LogLevel level = LogLevel::Info;
    LogFormatConfig format;
    LogRotationConfig rotation;
    bool useColors = true;
    std::ofstream fileOut;
    std::mutex mutex;
    std::string currentLogFile;
    size_t currentFileSize = 0;
};

ConsoleLogger::ConsoleLogger() : pImpl(std::make_unique<Impl>()) {
    // Set up default format
    pImpl->format.includeTimestamp = true;
    pImpl->format.includeLevel = true;
    pImpl->format.includeFile = false;
    pImpl->format.includeLineNumber = false;
    pImpl->format.includeFunction = false;
    pImpl->format.useColors = true;
    pImpl->format.useAnsiColors = true;
}

ConsoleLogger::ConsoleLogger(LogLevel level) : pImpl(std::make_unique<Impl>()) {
    pImpl->level = level;
    // Set up default format
    pImpl->format.includeTimestamp = true;
    pImpl->format.includeLevel = true;
    pImpl->format.includeFile = false;
    pImpl->format.includeLineNumber = false;
    pImpl->format.includeFunction = false;
    pImpl->format.useColors = true;
    pImpl->format.useAnsiColors = true;
}

ConsoleLogger::ConsoleLogger(const LoggingConfig& config) : pImpl(std::make_unique<Impl>()) {
    // Apply configuration
    pImpl->level = config.level;
    pImpl->format = config.format;
    pImpl->rotation = config.rotation;
    pImpl->useColors = config.format.useColors;
    
    if (config.outputFile.empty()) {
        pImpl->useColors = config.format.useColors && config.format.useAnsiColors;
    } else {
        setOutputFile(config.outputFile);
    }
}

ConsoleLogger::~ConsoleLogger() = default;

void ConsoleLogger::log(const LogEntry& entry) {
    if (entry.level < pImpl->level) {
        return;
    }
    
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    // Format the log entry
    std::string formattedMessage = formatEntry(entry);
    
    // Output to console
    if (pImpl->useColors && pImpl->format.useColors) {
        std::cout << levelToColor(entry.level) << formattedMessage << "\033[0m\n";
    } else {
        std::cout << formattedMessage << "\n";
    }
    
    // File output if set
    if (pImpl->fileOut.is_open()) {
        pImpl->fileOut << formattedMessage << "\n";
        pImpl->currentFileSize += formattedMessage.size() + 1; // +1 for newline
        pImpl->fileOut.flush();
        
        // Check if file rotation is needed
        if (pImpl->rotation.enabled && pImpl->currentFileSize >= pImpl->rotation.maxFileSize) {
            rotateLogFiles();
        }
    }
}

void ConsoleLogger::setLevel(LogLevel level) {
    pImpl->level = level;
}

LogLevel ConsoleLogger::getLevel() const {
    return pImpl->level;
}

void ConsoleLogger::setFormatConfig(const LogFormatConfig& config) {
    pImpl->format = config;
    pImpl->useColors = config.useColors && config.useAnsiColors;
}

LogFormatConfig ConsoleLogger::getFormatConfig() const {
    return pImpl->format;
}

void ConsoleLogger::setRotationConfig(const LogRotationConfig& config) {
    pImpl->rotation = config;
}

LogRotationConfig ConsoleLogger::getRotationConfig() const {
    return pImpl->rotation;
}

void ConsoleLogger::setOutputFile(const std::string& filepath) {
    closeOutputFile(); // Close existing file
    
    pImpl->fileOut.open(filepath, std::ios::app);
    if (pImpl->fileOut.is_open()) {
        pImpl->currentLogFile = filepath;
        // Get current file size
        pImpl->currentFileSize = static_cast<size_t>(pImpl->fileOut.tellp());
    }
}

void ConsoleLogger::closeOutputFile() {
    if (pImpl->fileOut.is_open()) {
        pImpl->fileOut.close();
        pImpl->currentLogFile.clear();
        pImpl->currentFileSize = 0;
    }
}

void ConsoleLogger::setUseColors(bool useColors) {
    pImpl->useColors = useColors;
}

bool ConsoleLogger::getUseColors() const {
    return pImpl->useColors;
}

bool ConsoleLogger::hasFileOutput() const {
    return pImpl->fileOut.is_open();
}

void ConsoleLogger::rotateLogFiles() {
    if (!pImpl->rotation.enabled || !pImpl->fileOut.is_open()) {
        return;
    }
    
    // Close current file
    pImpl->fileOut.close();
    
    // Generate new filename with timestamp
    std::string timestamp = getCurrentTimestamp(pImpl->rotation.rotationFormat);
    std::string newFilename = pImpl->currentLogFile.substr(0, pImpl->currentLogFile.find_last_of('.')) +
                             timestamp + pImpl->currentLogFile.substr(pImpl->currentLogFile.find_last_of('.'));
    
    // Rename current file
    if (std::filesystem::exists(pImpl->currentLogFile)) {
        std::filesystem::rename(pImpl->currentLogFile, newFilename);
    }
    
    // Reopen for appending
    pImpl->fileOut.open(pImpl->currentLogFile, std::ios::app);
    if (pImpl->fileOut.is_open()) {
        pImpl->currentFileSize = 0;
    }
    
    // Compress if requested
    if (pImpl->rotation.compressOldFiles) {
        compressLogFiles();
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

std::string ConsoleLogger::levelToColor(LogLevel level) const {
    if (!pImpl->format.useColors || !pImpl->format.useAnsiColors) {
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

std::string ConsoleLogger::formatTimestamp(const std::chrono::system_clock::time_point& timestamp) const {
    auto time = std::chrono::system_clock::to_time_t(timestamp);
    std::tm tm_time;
    localtime_s(&tm_time, &time);
    
    std::stringstream ss;
    if (pImpl->format.timestampFormat == "%Y-%m-%d %H:%M:%S") {
        ss << std::put_time(&tm_time, "%Y-%m-%d %H:%M:%S");
    } else if (pImpl->format.timestampFormat == "%H:%M:%S") {
        ss << std::put_time(&tm_time, "%H:%M:%S");
    } else if (pImpl->format.timestampFormat == "%Y%m%d%H%M%S") {
        ss << std::put_time(&tm_time, "%Y%m%d%H%M%S");
    } else {
        ss << std::put_time(&tm_time, "%Y-%m-%d %H:%M:%S");
    }
    
    return ss.str();
}

std::string ConsoleLogger::formatEntry(const LogEntry& entry) const {
    std::stringstream ss;
    
    // Add level if enabled
    if (pImpl->format.includeLevel) {
        ss << "[" << levelToString(entry.level) << "] ";
    }
    
    // Add timestamp if enabled
    if (pImpl->format.includeTimestamp) {
        ss << formatTimestamp(entry.timestamp) << " ";
    }
    
    // Add file info if enabled
    if (pImpl->format.includeFile && !entry.file.empty()) {
        ss << pImpl->format.filePrefix << entry.file;
        if (pImpl->format.includeLineNumber && entry.line > 0) {
            ss << ":" << pImpl->format.linePrefix << entry.line;
        }
        if (pImpl->format.includeFunction && !entry.function.empty()) {
            ss << " " << pImpl->format.functionPrefix << entry.function;
        }
        ss << " ";
    }
    
    // Add message
    ss << entry.message;
    
    return ss.str();
}

std::string ConsoleLogger::getCurrentTimestamp(const std::string& format) const {
    auto now = std::chrono::system_clock::now();
    auto time = std::chrono::system_clock::to_time_t(now);
    std::tm tm_time;
    localtime_s(&tm_time, &time);
    
    std::stringstream ss;
    if (format == ".%Y%m%d%H%M%S") {
        ss << "." << std::put_time(&tm_time, "%Y%m%d%H%M%S");
    } else {
        ss << "." << std::put_time(&tm_time, "%Y%m%d%H%M%S");
    }
    
    return ss.str();
}

void ConsoleLogger::compressLogFiles() {
    // Simple gzip compression of old log files
    // This is a placeholder implementation
}

// Implementation for LoggingConfig methods
bool LoggingConfig::loadFromConfig(const std::string& configFile) {
    std::ifstream file(configFile);
    if (!file.is_open()) {
        return false;
    }
    
    std::string line;
    while (std::getline(file, line)) {
        line = trim(line);
        if (line.empty() || line[0] == '#') {
            continue;
        }
        
        size_t pos = line.find('=');
        if (pos == std::string::npos) {
            continue;
        }
        
        std::string key = trim(line.substr(0, pos));
        std::string value = trim(line.substr(pos + 1));
        
        if (key == "level") {
            level = stringToLogLevel(value);
        } else if (key == "console_output") {
            consoleOutput = stringToBool(value);
        } else if (key == "file_output") {
            fileOutput = stringToBool(value);
        } else if (key == "output_file") {
            outputFile = value;
        } else if (key == "async") {
            async = stringToBool(value);
        } else if (key == "async_buffer_size") {
            asyncBufferSize = std::stoul(value);
        }
    }
    
    return true;
}

void LoggingConfig::applyToLogger(std::shared_ptr<Logger> logger) {
    if (!logger) {
        return;
    }
    
    logger->applyConfig(*this);
}

// Logger class implementation
struct Logger::Impl {
    std::vector<std::shared_ptr<ILogger>> loggers;
    std::mutex mutex;
    LoggingConfig config;
    PerformanceStats performanceStats;
};

Logger::Logger() : pImpl(std::make_unique<Impl>()) {
    // Set up default console logger
    auto consoleLogger = std::make_shared<ConsoleLogger>();
    addLogger(consoleLogger);
    
    // Initialize performance stats
    pImpl->performanceStats.lastReset = std::chrono::system_clock::now();
}

Logger::Logger(const LoggingConfig& config) : pImpl(std::make_unique<Impl>()) {
    // Apply configuration
    pImpl->config = config;
    applyConfig(config);
    
    // Initialize performance stats
    pImpl->performanceStats.lastReset = std::chrono::system_clock::now();
}

Logger::~Logger() = default;

void Logger::addLogger(std::shared_ptr<ILogger> logger) {
    if (!logger) {
        return;
    }
    
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
    pImpl->config.level = level;
}

void Logger::applyConfig(const LoggingConfig& config) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    // Update config
    pImpl->config = config;
    
    // Apply to all loggers
    for (auto& logger : pImpl->loggers) {
        logger->setLevel(config.level);
        if (auto consoleLogger = std::dynamic_pointer_cast<ConsoleLogger>(logger)) {
            consoleLogger->setFormatConfig(config.format);
            consoleLogger->setRotationConfig(config.rotation);
            if (config.outputFile.empty()) {
                consoleLogger->closeOutputFile();
            } else {
                consoleLogger->setOutputFile(config.outputFile);
            }
        }
    }
}

LoggingConfig Logger::getConfig() const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    return pImpl->config;
}

void Logger::log(LogLevel level, const std::string& message,
                 const std::string& file, int line, const std::string& function) {
    auto startTime = std::chrono::high_resolution_clock::now();
    
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    LogEntry entry;
    entry.level = level;
    entry.message = message;
    entry.file = file;
    entry.line = line;
    entry.function = function;
    entry.timestamp = std::chrono::system_clock::now();
    
    for (auto& logger : pImpl->loggers) {
        if (logger->isEnabled(level)) {
            logger->log(entry);
        }
    }
    
    // Update performance stats
    auto endTime = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(endTime - startTime);
    
    pImpl->performanceStats.messagesLogged++;
    pImpl->performanceStats.avgLogTimeUs = 
        (pImpl->performanceStats.avgLogTimeUs * (pImpl->performanceStats.messagesLogged - 1) + 
         duration.count()) / pImpl->performanceStats.messagesLogged;
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
        if (logger->isEnabled(level)) {
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

bool Logger::loadGlobalConfigFromFile(const std::string& configFile) {
    LoggingConfig config;
    if (config.loadFromConfig(configFile)) {
        getGlobal().applyConfig(config);
        return true;
    }
    return false;
}

bool Logger::saveGlobalConfigToFile(const std::string& configFile) const {
    std::ofstream file(configFile);
    if (!file.is_open()) {
        return false;
    }
    
    // Write configuration to file
    // This is a simplified implementation
    file << "# NLM Logging Configuration" << std::endl;
    file << "level=INFO" << std::endl;
    file << "console_output=true" << std::endl;
    file << "file_output=false" << std::endl;
    
    return true;
}

bool Logger::validateConfig(const LoggingConfig& config, std::vector<std::string>& errors) {
    errors.clear();
    
    // Validate log level
    if (!isalpha(static_cast<char>(config.level)) && config.level != LogLevel::Debug && 
        config.level != LogLevel::Info && config.level != LogLevel::Warning && 
        config.level != LogLevel::Error && config.level != LogLevel::Critical) {
        errors.push_back("Invalid log level in configuration");
    }
    
    // Validate file output path
    if (config.fileOutput && !config.outputFile.empty()) {
        // Check if directory exists
        std::filesystem::path filepath(config.outputFile);
        if (!filepath.parent_path().empty() && !std::filesystem::exists(filepath.parent_path())) {
            errors.push_back("Output directory does not exist: " + filepath.parent_path().string());
        }
    }
    
    return errors.empty();
}

std::map<std::string, std::string> Logger::getConfigurationSchema() {
    std::map<std::string, std::string> schema;
    
    schema["level"] = "Log level (DEBUG, INFO, WARNING, ERROR, CRITICAL)";
    schema["console_output"] = "Enable console output (true/false)";
    schema["file_output"] = "Enable file output (true/false)";
    schema["output_file"] = "Path to log file";
    schema["async"] = "Enable asynchronous logging (true/false)";
    schema["async_buffer_size"] = "Size of async buffer";
    
    return schema;
}

Logger::PerformanceStats Logger::getPerformanceStats() const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    return pImpl->performanceStats;
}

void Logger::resetPerformanceStats() {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->performanceStats.messagesLogged = 0;
    pImpl->performanceStats.messagesDropped = 0;
    pImpl->performanceStats.avgLogTimeUs = 0.0;
    pImpl->performanceStats.maxQueueSize = 0;
    pImpl->performanceStats.lastReset = std::chrono::system_clock::now();
}

// LogStream implementation
LogStream::LogStream(LogLevel level, const std::string& file, int line, const std::string& function)
    : level_(level), file_(file), line_(line), function_(function) {}

LogStream::~LogStream() {
    Logger::getGlobal().log(level_, oss_.str(), file_, line_, function_);
}

// Configuration namespace implementation
namespace LogConfig {
    LoggingConfig getDefaultConfig() {
        LoggingConfig config;
        config.level = LogLevel::Info;
        config.format.includeTimestamp = true;
        config.format.includeLevel = true;
        config.format.includeFile = false;
        config.format.includeLineNumber = false;
        config.format.includeFunction = false;
        config.format.useColors = true;
        config.format.useAnsiColors = true;
        config.format.timestampFormat = "%Y-%m-%d %H:%M:%S";
        config.consoleOutput = true;
        config.fileOutput = false;
        config.async = false;
        config.asyncBufferSize = 1024;
        return config;
    }
    
    bool parseConfigString(const std::string& configStr, LoggingConfig& config) {
        std::istringstream stream(configStr);
        std::string line;
        while (std::getline(stream, line)) {
            line = trim(line);
            if (line.empty() || line[0] == '#') {
                continue;
            }
            
            size_t pos = line.find('=');
            if (pos == std::string::npos) {
                continue;
            }
            
            std::string key = trim(line.substr(0, pos));
            std::string value = trim(line.substr(pos + 1));
            
            if (key == "level") {
                config.level = stringToLogLevel(value);
            } else if (key == "console_output") {
                config.consoleOutput = stringToBool(value);
            } else if (key == "file_output") {
                config.fileOutput = stringToBool(value);
            } else if (key == "output_file") {
                config.outputFile = value;
            } else if (key == "async") {
                config.async = stringToBool(value);
            } else if (key == "async_buffer_size") {
                config.asyncBufferSize = std::stoul(value);
            }
        }
        
        return true;
    }
    
    std::string generateConfigString(const LoggingConfig& config) {
        std::stringstream ss;
        ss << "# NLM Logging Configuration" << std::endl;
        ss << "level=" << levelToString(config.level) << std::endl;
        ss << "console_output=" << (config.consoleOutput ? "true" : "false") << std::endl;
        ss << "file_output=" << (config.fileOutput ? "true" : "false") << std::endl;
        if (!config.outputFile.empty()) {
            ss << "output_file=" << config.outputFile << std::endl;
        }
        ss << "async=" << (config.async ? "true" : "false") << std::endl;
        ss << "async_buffer_size=" << config.asyncBufferSize << std::endl;
        return ss.str();
    }
    
    std::vector<std::string> validateConfig(const LoggingConfig& config) {
        std::vector<std::string> errors;
        
        // Basic validation
        if (config.consoleOutput && config.outputFile.empty()) {
            errors.push_back("Output file path not specified for file output");
        }
        
        if (config.async && config.asyncBufferSize == 0) {
            errors.push_back("Async buffer size cannot be zero");
        }
        
        return errors;
    }
    
    std::string getLevelColor(LogLevel level) {
        switch (level) {
            case LogLevel::Debug: return "\033[36m";    // Cyan
            case LogLevel::Info: return "\033[32m";      // Green
            case LogLevel::Warning: return "\033[33m";   // Yellow
            case LogLevel::Error: return "\033[31m";     // Red
            case LogLevel::Critical: return "\033[35m"; // Magenta
            default: return "";
        }
    }
    
    std::string getLevelName(LogLevel level) {
        switch (level) {
            case LogLevel::Debug: return "DEBUG";
            case LogLevel::Info: return "INFO";
            case LogLevel::Warning: return "WARN";
            case LogLevel::Error: return "ERROR";
            case LogLevel::Critical: return "CRIT";
            default: return "UNKNOWN";
        }
    }
}

} // namespace nlm
