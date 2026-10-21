#include "Logger.hpp"
#include <iostream>
#include <fstream>
#include <iomanip>
#include <sstream>
#include <mutex>
#include <atomic>
#include <thread>
#include <filesystem>
#include <algorithm>
#include <ctime>
#include <cstdio>
#include <cerrno>

namespace nlm {

// Global error reporting utilities
namespace ErrorUtils {
    std::string getSystemError() {
        return std::string(strerror(errno));
    }
    
    std::string formatError(const std::string& operation, const std::string& source = "") {
        std::string error = "Operation: " + operation;
        if (!source.empty()) {
            error += " (Source: " + source + ")";
        }
        error += " - System error: " + getSystemError();
        return error;
    }
    
    bool isFileAccessible(const std::string& path, std::ifstream::openmode mode = std::ifstream::in) {
        std::ifstream file(path, mode);
        return file.good();
    }
    
    bool ensureDirectoryExists(const std::string& path) {
        try {
            std::filesystem::path dir(path);
            if (!dir.empty() && !std::filesystem::exists(dir)) {
                return std::filesystem::create_directories(dir);
            }
            return true;
        } catch (const std::exception&) {
            return false;
        }
    }
}

// Thread-safe atomic counter for statistics
class AtomicCounter {
public:
    AtomicCounter() : value_(0) {}
    void increment() { ++value_; }
    void add(size_t n) { value_ += n; }
    size_t get() const { return value_; }
    void reset() { value_ = 0; }
    
private:
    std::atomic<size_t> value_;
};

// Structured logging formatter
class LogFormatter {
public:
    std::string format(const LogEntry& entry, const std::string& pattern) {
        std::string result = pattern;
        
        // Simple placeholder replacement
        size_t pos = 0;
        while ((pos = result.find("%level", pos)) != std::string::npos) {
            result.replace(pos, 7, levelToString(entry.level));
            pos += levelToString(entry.level).size();
        }
        
        pos = 0;
        while ((pos = result.find("%message", pos)) != std::string::npos) {
            result.replace(pos, 8, entry.message);
            pos += entry.message.size();
        }
        
        pos = 0;
        while ((pos = result.find("%timestamp", pos)) != std::string::npos) {
            result.replace(pos, 10, formatTimestamp(entry.timestamp));
            pos += formatTimestamp(entry.timestamp).size();
        }
        
        pos = 0;
        while ((pos = result.find("%file", pos)) != std::string::npos) {
            result.replace(pos, 6, extractFilename(entry.file));
            pos += extractFilename(entry.file).size();
        }
        
        pos = 0;
        while ((pos = result.find("%line", pos)) != std::string::npos) {
            result.replace(pos, 6, std::to_string(entry.line));
            pos += std::to_string(entry.line).size();
        }
        
        pos = 0;
        while ((pos = result.find("%function", pos)) != std::string::npos) {
            result.replace(pos, 9, entry.function);
            pos += entry.function.size();
        }
        
        pos = 0;
        while ((pos = result.find("%thread", pos)) != std::string::npos) {
            result.replace(pos, 8, formatThreadId(entry.threadId));
            pos += formatThreadId(entry.threadId).size();
        }
        
        return result;
    }
    
    std::string levelToString(LogLevel level) const {
        switch (level) {
            case LogLevel::Debug: return "DEBUG";
            case LogLevel::Info: return "INFO";
            case LogLevel::Warning: return "WARNING";
            case LogLevel::Error: return "ERROR";
            case LogLevel::Critical: return "CRITICAL";
            default: return "UNKNOWN";
        }
    }
    
    std::string formatTimestamp(const std::chrono::system_clock::time_point& timestamp) {
        auto time = std::chrono::system_clock::to_time_t(timestamp);
        std::tm tm_time;
        
        // Thread-safe version of localtime
        std::tm* result = std::localtime(&time);
        if (result == nullptr) {
            return "1970-01-01 00:00:00";
        }
        
        std::ostringstream oss;
        oss << std::put_time(result, "%Y-%m-%d %H:%M:%S");
        return oss.str();
    }
    
    std::string extractFilename(const std::string& path) {
        size_t pos = path.find_last_of('/');
        if (pos != std::string::npos) {
            return path.substr(pos + 1);
        }
        pos = path.find_last_of('\\');
        if (pos != std::string::npos) {
            return path.substr(pos + 1);
        }
        return path;
    }
    
    std::string formatThreadId(std::thread::id id) {
        std::ostringstream oss;
        oss << id;
        return oss.str();
    }
};

// File rotation utility
class LogFileRotator {
public:
    bool rotateIfNeeded(const std::string& filepath, size_t maxSize, size_t maxFiles) {
        try {
            std::filesystem::path path(filepath);
            
            // Check if file exists and its size
            if (std::filesystem::exists(path)) {
                auto fileSize = std::filesystem::file_size(path);
                if (fileSize >= maxSize) {
                    rotateFiles(path, maxFiles);
                }
            }
            
            return true;
        } catch (const std::exception&) {
            return false;
        }
    }
    
private:
    void rotateFiles(const std::filesystem::path& filepath, size_t maxFiles) {
        try {
            // Get all files with same prefix
            std::vector<std::filesystem::path> files;
            std::string stem = filepath.stem().string();
            std::string extension = filepath.extension().string();
            
            for (const auto& entry : std::filesystem::directory_iterator(filepath.parent_path())) {
                if (entry.is_regular_file()) {
                    std::string filename = entry.path().filename().string();
                    if (filename.substr(0, stem.size()) == stem) {
                        files.push_back(entry.path());
                    }
                }
            }
            
            // Sort by modification time (newest first)
            std::sort(files.begin(), files.end(), [](const auto& a, const auto& b) {
                return std::filesystem::last_write_time(a) > std::filesystem::last_write_time(b);
            });
            
            // Remove oldest files
            size_t filesToDelete = files.size() - maxFiles + 1;
            for (size_t i = filesToDelete; i < files.size(); ++i) {
                std::filesystem::remove(files[i]);
            }
            
            // Rename current file
            auto newPath = filepath;
            if (files.size() >= maxFiles) {
                newPath = filepath.parent_path() / (stem + "_" + 
                    std::to_string(std::chrono::system_clock::now().time_since_epoch().count()) +
                    extension);
            }
            
            std::filesystem::rename(filepath, newPath);
            
        } catch (const std::exception&) {
            // Silently fail rotation - log errors would cause infinite recursion
        }
    }
};

struct ConsoleLogger::Impl {
    LogLevel level = LogLevel::Info;
    bool useColors = true;
    LoggerConfig config;
    std::ofstream fileOut;
    std::mutex mutex;
    std::vector<std::string> errors;
    AtomicCounter logCount;
    LogFileRotator rotator;
    LogFormatter formatter;
    bool fileOpen = false;
};

ConsoleLogger::ConsoleLogger() : pImpl(std::make_unique<Impl>()) {
    pImpl->config.defaultLevel = LogLevel::Info;
    pImpl->config.useColors = true;
    pImpl->config.asyncLogging = false;
    pImpl->config.maxFileSize = 10 * 1024 * 1024;
    pImpl->config.maxFileCount = 5;
    pImpl->config.logToConsole = true;
    pImpl->config.logToFile = false;
    pImpl->config.enableTimestamps = true;
    pImpl->config.enableThreadIds = false;
    pImpl->config.logPattern = "[%level] [%timestamp] %message";
}

ConsoleLogger::ConsoleLogger(const LoggerConfig& config) : pImpl(std::make_unique<Impl>()) {
    configure(config);
}

ConsoleLogger::~ConsoleLogger() {
    closeOutputFile();
}

bool ConsoleLogger::log(const LogEntry& entry) {
    if (entry.level < pImpl->level) {
        return false;
    }
    
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->logCount.increment();
    
    try {
        std::string formattedMessage;
        
        if (pImpl->useColors && pImpl->config.logToConsole) {
            formattedMessage = formatColoredMessage(entry);
        } else {
            formattedMessage = formatPlainMessage(entry);
        }
        
        // Console output
        if (pImpl->config.logToConsole) {
            std::cout << formattedMessage << "\n";
            std::cout.flush();
        }
        
        // File output if set
        if (pImpl->fileOpen && pImpl->fileOut.is_open()) {
            if (!writeToFile(formattedMessage + "\n")) {
                pImpl->errors.push_back(ErrorUtils::formatError("writeToFile", "ConsoleLogger"));
            }
        }
        
        return true;
        
    } catch (const std::exception& e) {
        pImpl->errors.push_back(std::string("Log operation failed: ") + e.what());
        return false;
    }
}

std::string ConsoleLogger::formatColoredMessage(const LogEntry& entry) const {
    std::ostringstream oss;
    
    if (pImpl->config.enableTimestamps) {
        oss << "[" << pImpl->formatter.levelToString(entry.level) << "] ";
        oss << pImpl->formatter.formatTimestamp(entry.timestamp) << " ";
    } else {
        oss << "[" << pImpl->formatter.levelToString(entry.level) << "] ";
    }
    
    oss << entry.message;
    
    if (pImpl->useColors) {
        oss << "\033[0m";
    }
    
    return oss.str();
}

std::string ConsoleLogger::formatPlainMessage(const LogEntry& entry) const {
    std::ostringstream oss;
    
    if (pImpl->config.enableTimestamps) {
        oss << "[" << pImpl->formatter.levelToString(entry.level) << "] ";
        oss << pImpl->formatter.formatTimestamp(entry.timestamp) << " ";
    } else {
        oss << "[" << pImpl->formatter.levelToString(entry.level) << "] ";
    }
    
    oss << entry.message;
    
    return oss.str();
}

bool ConsoleLogger::writeToFile(const std::string& message) {
    if (!pImpl->fileOut.is_open()) {
        return false;
    }
    
    // Check file size and rotate if needed
    pImpl->rotator.rotateIfNeeded(pImpl->config.logFilePath, 
                                  pImpl->config.maxFileSize, 
                                  pImpl->config.maxFileCount);
    
    // Try to write
    pImpl->fileOut << message;
    
    if (!pImpl->fileOut.good()) {
        pImpl->errors.push_back(ErrorUtils::formatError("file_write", "ConsoleLogger"));
        closeOutputFile();
        return false;
    }
    
    // Flush periodically
    if (pImpl->logCount.get() % 100 == 0) {
        pImpl->fileOut.flush();
    }
    
    return true;
}

void ConsoleLogger::logError(const std::string& error, const std::string& operation) {
    pImpl->errors.push_back(ErrorUtils::formatError(error, operation));
    // Log error to stderr to avoid infinite recursion
    std::cerr << "[LOGGER ERROR] " << error << " (Source: " << operation << ")\n";
    std::cerr.flush();
}

void ConsoleLogger::setLevel(LogLevel level) {
    pImpl->level = level;
}

LogLevel ConsoleLogger::getLevel() const {
    return pImpl->level;
}

bool ConsoleLogger::isEnabled(LogLevel level) const {
    return level >= pImpl->level;
}

bool ConsoleLogger::configure(const LoggerConfig& config) {
    try {
        // Validate configuration
        if (!validateLoggerConfig(config)) {
            pImpl->errors.push_back("Invalid logger configuration");
            return false;
        }
        
        std::lock_guard<std::mutex> lock(pImpl->mutex);
        
        // Update configuration
        pImpl->config = config;
        pImpl->level = config.defaultLevel;
        pImpl->useColors = config.useColors;
        
        // Handle file configuration changes
        if (config.logToFile && !config.logFilePath.empty()) {
            if (pImpl->fileOpen) {
                closeOutputFile();
            }
            
            // Ensure directory exists
            std::string dir = std::filesystem::path(config.logFilePath).parent_path().string();
            if (!dir.empty() && !ErrorUtils::ensureDirectoryExists(dir)) {
                logError("Cannot create log directory", "configure");
                return false;
            }
            
            setOutputFile(config.logFilePath, true);
        }
        
        return true;
        
    } catch (const std::exception& e) {
        logError(std::string("Configuration failed: ") + e.what(), "configure");
        return false;
    }
}

LoggerConfig ConsoleLogger::getConfiguration() const {
    return pImpl->config;
}

void ConsoleLogger::setUseColors(bool useColors) {
    pImpl->useColors = useColors;
}

bool ConsoleLogger::getUseColors() const {
    return pImpl->useColors;
}

bool ConsoleLogger::setOutputFile(const std::string& filepath, bool append) {
    try {
        std::lock_guard<std::mutex> lock(pImpl->mutex);
        
        closeOutputFile();
        
        // Ensure directory exists
        std::string dir = std::filesystem::path(filepath).parent_path().string();
        if (!dir.empty() && !ErrorUtils::ensureDirectoryExists(dir)) {
            logError("Cannot create log directory", "setOutputFile");
            return false;
        }
        
        std::ios::openmode mode = std::ofstream::out;
        if (append) {
            mode |= std::ofstream::app;
        } else {
            mode |= std::ofstream::trunc;
        }
        
        pImpl->fileOut.open(filepath, mode);
        
        if (!pImpl->fileOut.is_open()) {
            logError(ErrorUtils::formatError("file_open", filepath), "setOutputFile");
            return false;
        }
        
        pImpl->config.logFilePath = filepath;
        pImpl->fileOpen = true;
        
        return true;
        
    } catch (const std::exception& e) {
        logError(std::string("Failed to set output file: ") + e.what(), "setOutputFile");
        return false;
    }
}

void ConsoleLogger::closeOutputFile() {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    if (pImpl->fileOut.is_open()) {
        pImpl->fileOut.close();
        pImpl->fileOpen = false;
    }
}

bool ConsoleLogger::isOutputFileOpen() const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    return pImpl->fileOpen && pImpl->fileOut.is_open();
}

std::string ConsoleLogger::getOutputFilePath() const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    return pImpl->config.logFilePath;
}

std::vector<std::string> ConsoleLogger::getLastErrors() const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    return pImpl->errors;
}

void ConsoleLogger::clearErrors() {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->errors.clear();
}

bool ConsoleLogger::hasErrors() const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    return !pImpl->errors.empty();
}

// Logger class implementation
struct Logger::Impl {
    LoggerConfig config;
    std::vector<std::shared_ptr<ILogger>> loggers;
    std::mutex mutex;
    std::vector<std::string> errors;
    LoggerStats stats;
    std::atomic<bool> structuredLogging{false};
    size_t maxLogSize{10 * 1024 * 1024};
    LogFileRotator rotator;
};

Logger::Logger() : pImpl(std::make_unique<Impl>()) {
    pImpl->config.defaultLevel = LogLevel::Info;
    pImpl->config.useColors = true;
    pImpl->config.asyncLogging = false;
    pImpl->config.maxFileSize = 10 * 1024 * 1024;
    pImpl->config.maxFileCount = 5;
    pImpl->config.logToConsole = true;
    pImpl->config.logToFile = false;
    pImpl->config.enableTimestamps = true;
    pImpl->config.enableThreadIds = false;
    pImpl->config.logPattern = "[%level] [%timestamp] %message";
    
    // Add default console logger
    auto consoleLogger = std::make_shared<ConsoleLogger>();
    consoleLogger->configure(pImpl->config);
    addLogger(consoleLogger);
    
    pImpl->stats.startTime = std::chrono::system_clock::now();
}

Logger::Logger(const LoggerConfig& config) : pImpl(std::make_unique<Impl>()) {
    configureGlobal(config);
}

Logger::~Logger() {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    clearLoggers();
}

bool Logger::addLogger(std::shared_ptr<ILogger> logger) {
    if (!logger) {
        pImpl->errors.push_back("Cannot add null logger");
        return false;
    }
    
    try {
        std::lock_guard<std::mutex> lock(pImpl->mutex);
        
        // Check if logger with same name already exists
        std::string name = logger->getName();
        for (const auto& existing : pImpl->loggers) {
            if (existing->getName() == name) {
                pImpl->errors.push_back("Logger with name '" + name + "' already exists");
                return false;
            }
        }
        
        // Configure logger with global config
        logger->configure(pImpl->config);
        
        pImpl->loggers.push_back(logger);
        return true;
        
    } catch (const std::exception& e) {
        pImpl->errors.push_back(std::string("Failed to add logger: ") + e.what());
        return false;
    }
}

bool Logger::removeLogger(const std::string& name) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    auto it = std::find_if(pImpl->loggers.begin(), pImpl->loggers.end(),
                          [&name](const std::shared_ptr<ILogger>& logger) {
                              return logger->getName() == name;
                          });
    
    if (it != pImpl->loggers.end()) {
        pImpl->loggers.erase(it);
        return true;
    }
    
    pImpl->errors.push_back("Logger not found: " + name);
    return false;
}

bool Logger::removeLogger(std::shared_ptr<ILogger> logger) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    auto it = std::find(pImpl->loggers.begin(), pImpl->loggers.end(), logger);
    if (it != pImpl->loggers.end()) {
        pImpl->loggers.erase(it);
        return true;
    }
    
    pImpl->errors.push_back("Logger not found");
    return false;
}

void Logger::clearLoggers() {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->loggers.clear();
}

bool Logger::configureGlobal(const LoggerConfig& config) {
    if (!validateLoggerConfig(config)) {
        pImpl->errors.push_back("Invalid global logger configuration");
        return false;
    }
    
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    try {
        pImpl->config = config;
        
        // Configure all existing loggers
        for (auto& logger : pImpl->loggers) {
            logger->configure(config);
        }
        
        return true;
        
    } catch (const std::exception& e) {
        pImpl->errors.push_back(std::string("Global configuration failed: ") + e.what());
        return false;
    }
}

LoggerConfig Logger::getGlobalConfiguration() const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    return pImpl->config;
}

void Logger::setLevel(LogLevel level) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    for (auto& logger : pImpl->loggers) {
        logger->setLevel(level);
    }
}

bool Logger::log(LogLevel level, const std::string& message, 
                 const std::string& file, int line, 
                 const std::string& function) {
    LogEntry entry(level, message, file, line, function);
    return log(entry);
}

bool Logger::log(const LogEntry& entry) {
    std::vector<std::string> errors;
    logToAllLoggers(entry, errors);
    
    // Store errors
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->errors.insert(pImpl->errors.end(), errors.begin(), errors.end());
    
    // Update statistics
    pImpl->stats.totalLogs++;
    switch (entry.level) {
        case LogLevel::Debug: pImpl->stats.debugLogs++; break;
        case LogLevel::Info: pImpl->stats.infoLogs++; break;
        case LogLevel::Warning: pImpl->stats.warningLogs++; break;
        case LogLevel::Error: pImpl->stats.errorLogs++; break;
        case LogLevel::Critical: pImpl->stats.criticalLogs++; break;
    }
    
    return errors.empty();
}

void Logger::logToAllLoggers(const LogEntry& entry, std::vector<std::string>& errors) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    for (auto& logger : pImpl->loggers) {
        if (logger->isEnabled(entry.level)) {
            if (!logger->log(entry)) {
                errors.push_back("Logger '" + logger->getName() + "' failed to log entry");
                pImpl->stats.droppedLogs++;
            }
        }
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
    return std::any_of(pImpl->loggers.begin(), pImpl->loggers.end(),
                      [level](const std::shared_ptr<ILogger>& logger) {
                          return logger->isEnabled(level);
                      });
}

Logger& Logger::getGlobal() {
    static Logger globalLogger;
    return globalLogger;
}

void Logger::setGlobal(std::shared_ptr<Logger> logger) {
    // This is a placeholder for advanced global logger management
    // In practice, we just use getGlobal() which is already static
}

std::vector<std::string> Logger::getGlobalErrors() const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    std::vector<std::string> allErrors = pImpl->errors;
    
    for (const auto& logger : pImpl->loggers) {
        auto loggerErrors = logger->getLastErrors();
        allErrors.insert(allErrors.end(), loggerErrors.begin(), loggerErrors.end());
    }
    
    return allErrors;
}

void Logger::clearGlobalErrors() {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->errors.clear();
    
    for (auto& logger : pImpl->loggers) {
        logger->clearErrors();
    }
}

bool Logger::hasGlobalErrors() const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    if (!pImpl->errors.empty()) {
        return true;
    }
    
    for (const auto& logger : pImpl->loggers) {
        if (logger->hasErrors()) {
            return true;
        }
    }
    
    return false;
}

bool Logger::validateLoggerConfig(const LoggerConfig& config) const {
    if (config.maxFileSize == 0) {
        pImpl->errors.push_back("Invalid max file size");
        return false;
    }
    
    if (config.maxFileCount == 0) {
        pImpl->errors.push_back("Invalid max file count");
        return false;
    }
    
    if (config.logToFile && config.logFilePath.empty()) {
        pImpl->errors.push_back("Log file path is empty when logToFile is true");
        return false;
    }
    
    return true;
}

Logger::LoggerStats Logger::getStatistics() const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    return pImpl->stats;
}

void Logger::resetStatistics() {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->stats = LoggerStats{};
    pImpl->stats.startTime = std::chrono::system_clock::now();
}

// LogStream implementation
LogStream::LogStream(LogLevel level, const std::string& file, int line, const std::string& function, Logger& logger)
    : level_(level), file_(file), line_(line), function_(function), logger_(&logger) {
    oss_ = std::make_unique<std::ostringstream>();
}

LogStream::~LogStream() {
    if (oss_ && oss_->good() && logger_) {
        std::string message = oss_->str();
        if (!message.empty()) {
            logger_->log(level_, message, file_, line_, function_);
        }
    }
}

// Helper function implementations
void LogHelpers::logPerformance(const std::string& operation, double duration,
                                const std::string& details) {
    std::ostringstream oss;
    oss << "[PERF] " << operation << ": " << duration << "s";
    if (!details.empty()) {
        oss << " - " << details;
    }
    Logger::getGlobal().info(oss.str());
}

void LogHelpers::logMemoryAllocation(size_t bytes, const std::string& context) {
    std::ostringstream oss;
    oss << "[MEMORY] Allocated " << bytes << " bytes";
    if (!context.empty()) {
        oss << " (context: " << context << ")";
    }
    Logger::getGlobal().info(oss.str());
}

void LogHelpers::logMemoryDeallocation(size_t bytes, const std::string& context) {
    std::ostringstream oss;
    oss << "[MEMORY] Deallocated " << bytes << " bytes";
    if (!context.empty()) {
        oss << " (context: " << context << ")";
    }
    Logger::getGlobal().info(oss.str());
}

ErrorContext LogHelpers::createErrorContext(const std::string& operation) {
    ErrorContext context;
    context.operation = operation;
    context.file = "unknown";
    context.line = 0;
    context.function = "unknown";
    context.timestamp = std::chrono::system_clock::now();
    return context;
}

std::string LogHelpers::formatErrorContext(const ErrorContext& context) {
    std::ostringstream oss;
    oss << "[ErrorContext] Operation: " << context.operation
        << " at " << context.file << ":" << context.line
        << " in " << context.function
        << " at " << std::chrono::system_clock::to_time_t(context.timestamp);
    return oss.str();
}

void LogHelpers::logAssertion(bool condition, const std::string& message,
                             const std::string& file, int line, const std::string& function) {
    if (!condition) {
        std::ostringstream oss;
        oss << "[ASSERTION FAILED] " << message
            << " (File: " << file << ", Line: " << line
            << ", Function: " << function << ")";
        Logger::getGlobal().error(oss.str());
    }
}

} // namespace nlm
