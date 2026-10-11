// Logger.cpp - Enhanced implementation with comprehensive error handling
// This file implements robust error handling, validation, and recovery for logging

#include "Logger.hpp"
#include <iostream>
#include <fstream>
#include <iomanip>
#include <sstream>
#include <mutex>
#include <stdexcept>
#include <system_error>
#include <filesystem>

namespace nlm {

// ConsoleLogger implementation with error handling
struct ConsoleLogger::Impl {
    LogLevel level = LogLevel::Info;
    bool useColors = true;
    std::ofstream fileOut;
    std::string outputFilePath;
    mutable std::mutex mutex;
    bool fileError = false;
    int fileErrorCount = 0;
};

ConsoleLogger::ConsoleLogger() : pImpl(std::make_unique<Impl>()) {}

ConsoleLogger::ConsoleLogger(LogLevel level) : pImpl(std::make_unique<Impl>()) {
    try {
        setLevel(level);
    } catch (const std::exception& e) {
        // Set to default level if setting fails
        setLevel(LogLevel::Info);
    }
}

ConsoleLogger::~ConsoleLogger() = default;

void ConsoleLogger::log(const LogEntry& entry) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    try {
        // Validate log entry
        if (entry.message.empty()) {
            return; // Skip empty messages
        }
        
        // Check if level is enabled
        if (entry.level < pImpl->level) {
            return;
        }
        
        // Format the log message
        std::ostringstream oss;
        oss << "[" << levelToString(entry.level) << "] ";
        
        // Add timestamp
        oss << "[" << getFormattedTimestamp(entry.timestamp) << "] ";
        
        // Add file/line/function info if available
        if (!entry.file.empty()) {
            oss << entry.file;
            if (entry.line > 0) oss << ":" << entry.line;
            oss << " " << entry.function << ": ";
        }
        
        oss << entry.message;
        
        std::string output = oss.str();
        
        // Log to console
        if (!logToConsole(entry, output)) {
            // Console logging failed, try to recover
            recoverFromConsoleError();
        }
        
        // Log to file if set
        if (!pImpl->outputFilePath.empty() && !pImpl->fileError) {
            if (!logToFile(entry, output)) {
                // File logging failed, mark as error
                pImpl->fileError = true;
                pImpl->fileErrorCount++;
                
                // Try to recover file logging
                if (pImpl->fileErrorCount <= 5) { // Limit recovery attempts
                    recoverFromFileError();
                }
            }
        }
        
    } catch (const std::exception& e) {
        // If all logging fails, try to log to stderr as last resort
        std::cerr << "[CRITICAL] Logging error: " << e.what() << std::endl;
        std::cerr << "Failed message: " << entry.message << std::endl;
    }
}

bool ConsoleLogger::logToConsole(const LogEntry& entry, const std::string& output) {
    try {
        if (pImpl->useColors) {
            std::cout << levelToColor(entry.level) << output << "\033[0m\n";
        } else {
            std::cout << output << "\n";
        }
        std::cout.flush();
        return true;
    } catch (const std::exception& e) {
        // Console output failed
        std::cerr << "[ERROR] Console logging failed: " << e.what() << std::endl;
        return false;
    }
}

bool ConsoleLogger::logToFile(const LogEntry& entry, const std::string& output) {
    try {
        if (!pImpl->fileOut.is_open()) {
            // Reopen file
            pImpl->fileOut.open(pImpl->outputFilePath, std::ios::app);
            if (!pImpl->fileOut.is_open()) {
                return false;
            }
        }
        
        pImpl->fileOut << output << "\n";
        pImpl->fileOut.flush();
        return true;
    } catch (const std::exception& e) {
        std::cerr << "[ERROR] File logging failed: " << e.what() << std::endl;
        return false;
    }
}

void ConsoleLogger::recoverFromConsoleError() {
    try {
        // Attempt to recover from console error
        // This might involve resetting the stream state
        std::cout.clear();
        std::cout << "[INFO] Console logger recovered from error" << std::endl;
        pImpl->useColors = false; // Disable colors to avoid escape sequence issues
    } catch (const std::exception& e) {
        std::cerr << "[CRITICAL] Console recovery failed: " << e.what() << std::endl;
    }
}

void ConsoleLogger::recoverFromFileError() {
    try {
        // Close and attempt to reopen file
        if (pImpl->fileOut.is_open()) {
            pImpl->fileOut.close();
        }
        
        // Try to reopen the file
        pImpl->fileOut.open(pImpl->outputFilePath, std::ios::app);
        if (pImpl->fileOut.is_open()) {
            std::cout << "[INFO] File logging recovered" << std::endl;
            pImpl->fileError = false;
        } else {
            std::cerr << "[WARNING] File recovery failed - file will not be used for logging" << std::endl;
        }
    } catch (const std::exception& e) {
        std::cerr << "[ERROR] File recovery error: " << e.what() << std::endl;
    }
}

void ConsoleLogger::setLevel(LogLevel level) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->level = level;
}

LogLevel ConsoleLogger::getLevel() const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    return pImpl->level;
}

void ConsoleLogger::setUseColors(bool useColors) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->useColors = useColors;
}

bool ConsoleLogger::getUseColors() const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    return pImpl->useColors;
}

void ConsoleLogger::setOutputFile(const std::string& filepath) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    try {
        // Validate file path
        if (!validateFilePath(filepath)) {
            throw std::invalid_argument("Invalid file path: " + filepath);
        }
        
        // Close existing file if open
        if (pImpl->fileOut.is_open()) {
            pImpl->fileOut.close();
        }
        
        pImpl->outputFilePath = filepath;
        pImpl->fileError = false;
        pImpl->fileErrorCount = 0;
        
        // Attempt to open the file
        pImpl->fileOut.open(filepath, std::ios::app);
        if (!pImpl->fileOut.is_open()) {
            std::cerr << "[WARNING] Failed to open log file: " << filepath << std::endl;
            pImpl->fileError = true;
            pImpl->fileErrorCount++;
        } else {
            std::cout << "[INFO] Log file opened: " << filepath << std::endl;
        }
    } catch (const std::exception& e) {
        std::cerr << "[ERROR] Failed to set output file: " << e.what() << std::endl;
        pImpl->fileError = true;
        pImpl->fileErrorCount++;
    }
}

void ConsoleLogger::closeOutputFile() {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    if (pImpl->fileOut.is_open()) {
        pImpl->fileOut.close();
        std::cout << "[INFO] Log file closed" << std::endl;
    }
}

bool ConsoleLogger::isFileOpen() const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    return pImpl->fileOut.is_open();
}

std::string ConsoleLogger::getOutputFilePath() const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    return pImpl->outputFilePath;
}

void ConsoleLogger::flush() {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    try {
        std::cout.flush();
        if (pImpl->fileOut.is_open()) {
            pImpl->fileOut.flush();
        }
    } catch (const std::exception& e) {
        std::cerr << "[ERROR] Flush failed: " << e.what() << std::endl;
    }
}

bool ConsoleLogger::recoverFromError() {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    try {
        // Attempt to recover from various error states
        bool recovered = true;
        
        // Recover console output
        std::cout.clear();
        std::cout << "[INFO] Console logger recovered from error" << std::endl;
        
        // Recover file output
        if (pImpl->fileError) {
            if (pImpl->fileOut.is_open()) {
                pImpl->fileOut.close();
            }
            
            if (!pImpl->outputFilePath.empty()) {
                pImpl->fileOut.open(pImpl->outputFilePath, std::ios::app);
                if (pImpl->fileOut.is_open()) {
                    std::cout << "[INFO] File logging recovered" << std::endl;
                    pImpl->fileError = false;
                } else {
                    std::cerr << "[WARNING] File recovery failed" << std::endl;
                    recovered = false;
                }
            }
        }
        
        return recovered;
    } catch (const std::exception& e) {
        std::cerr << "[ERROR] Recovery failed: " << e.what() << std::endl;
        return false;
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
    switch (level) {
        case LogLevel::Debug: return "\033[36m";    // Cyan
        case LogLevel::Info: return "\033[32m";      // Green
        case LogLevel::Warning: return "\033[33m";   // Yellow
        case LogLevel::Error: return "\033[31m";     // Red
        case LogLevel::Critical: return "\033[35m"; // Magenta
        default: return "\033[0m";
    }
}

std::string ConsoleLogger::getFormattedTimestamp(std::chrono::system_clock::time_point timestamp) const {
    auto time = std::chrono::system_clock::to_time_t(timestamp);
    char buf[100];
    std::strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S", std::localtime(&time));
    return buf;
}

bool ConsoleLogger::validateFilePath(const std::string& filepath) const {
    // Basic file path validation
    if (filepath.empty()) {
        return false;
    }
    
    // Check for invalid characters
    for (char c : filepath) {
        if (std::iscntrl(c)) { // Control characters
            return false;
        }
    }
    
    // Check if path is a valid directory or can be created
    std::filesystem::path path(filepath);
    
    // If the file doesn't exist, check if we can create its parent directory
    if (!std::filesystem::exists(path)) {
        std::filesystem::path parent = path.parent_path();
        if (!parent.empty() && !std::filesystem::exists(parent)) {
            try {
                std::filesystem::create_directories(parent);
            } catch (const std::exception& e) {
                std::cerr << "[ERROR] Cannot create directory " << parent.string() << ": " << e.what() << std::endl;
                return false;
            }
        }
    }
    
    return true;
}

// Logger implementation with error handling
struct Logger::Impl {
    std::vector<std::shared_ptr<ILogger>> loggers;
    std::mutex mutex;
    std::shared_ptr<IErrorReporter> errorReporter;
    uint64_t totalErrorsReported;
    uint64_t totalWarningsReported;
    std::map<ErrorCategory, uint64_t> errorsByCategory;
    std::map<ErrorCategory, uint64_t> warningsByCategory;
};

Logger::Logger() : pImpl(std::make_unique<Impl>()) {
    pImpl->errorReporter = nullptr;
    pImpl->totalErrorsReported = 0;
    pImpl->totalWarningsReported = 0;
    
    // Set up default error reporter if not already set
    if (!ErrorReporting::isSystemHealthy()) {
        ErrorReporting::initialize();
    }
}

Logger::~Logger() = default;

Logger::Logger(Logger&& other) noexcept : pImpl(std::move(other.pImpl)) {}

Logger& Logger::operator=(Logger&& other) noexcept {
    if (this != &other) {
        pImpl = std::move(other.pImpl);
    }
    return *this;
}

void Logger::addLogger(std::shared_ptr<ILogger> logger) {
    if (!logger) {
        throw std::invalid_argument("Logger cannot be null");
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
        try {
            logger->setLevel(level);
        } catch (const std::exception& e) {
            // Continue with other loggers if one fails
            std::cerr << "[ERROR] Failed to set level for logger: " << e.what() << std::endl;
        }
    }
}

void Logger::log(LogLevel level, const std::string& message,
                 const std::string& file, int line, const std::string& function) {
    log(level, message, file, line, function, "");
}

void Logger::log(LogLevel level, const std::string& message,
                 const std::string& file, int line, const std::string& function,
                 const std::string& errorContext) {
    try {
        // Create log entry
        LogEntry entry(level, message, file, line, function);
        entry.errorContext = errorContext;
        
        // Log to all loggers
        for (auto& logger : pImpl->loggers) {
            try {
                logger->log(entry);
            } catch (const std::exception& e) {
                std::cerr << "[ERROR] Logger failed: " << e.what() << std::endl;
            }
        }
        
        // Check if this should be reported as an error
        logAsError(entry);
        
    } catch (const std::exception& e) {
        std::cerr << "[CRITICAL] Failed to log message: " << e.what() << std::endl;
        std::cerr << "Message: " << message << std::endl;
    }
}

void Logger::logAsError(const LogEntry& entry) {
    // Convert LogEntry to DetailedError and report it
    DetailedError error = createDetailedError(entry);
    
    // Check if this should be reported as an error
    if (shouldLogAsError(entry)) {
        // Report to error reporter if available
        if (pImpl->errorReporter) {
            try {
                pImpl->errorReporter->reportError(error);
                pImpl->totalErrorsReported++;
                pImpl->errorsByCategory[error.category]++;
            } catch (const std::exception& e) {
                std::cerr << "[ERROR] Failed to report error: " << e.what() << std::endl;
            }
        } else {
            // Try to get global error reporter
            ErrorReporter& globalReporter = ErrorReporting::getErrorReporter();
            try {
                globalReporter.reportError(error);
                pImpl->totalErrorsReported++;
                pImpl->errorsByCategory[error.category]++;
            } catch (const std::exception& e) {
                std::cerr << "[ERROR] Failed to report error to global reporter: " << e.what() << std::endl;
            }
        }
    }
}

void Logger::logAsWarning(const LogEntry& entry) {
    // Convert LogEntry to DetailedError and report it as a warning
    DetailedError warning = createDetailedError(entry);
    warning.severity = ErrorSeverity::Warning;
    
    // Check if this should be reported as a warning
    if (entry.level >= LogLevel::Warning && entry.level <= LogLevel::Error) {
        // Report to error reporter if available
        if (pImpl->errorReporter) {
            try {
                pImpl->errorReporter->reportWarning(warning);
                pImpl->totalWarningsReported++;
                pImpl->warningsByCategory[warning.category]++;
            } catch (const std::exception& e) {
                std::cerr << "[ERROR] Failed to report warning: " << e.what() << std::endl;
            }
        } else {
            // Try to get global error reporter
            ErrorReporter& globalReporter = ErrorReporting::getErrorReporter();
            try {
                globalReporter.reportWarning(warning);
                pImpl->totalWarningsReported++;
                pImpl->warningsByCategory[warning.category]++;
            } catch (const std::exception& e) {
                std::cerr << "[ERROR] Failed to report warning to global reporter: " << e.what() << std::endl;
            }
        }
    }
}

bool Logger::shouldLogAsError(const LogEntry& entry) const {
    // Determine if this log entry should be reported as an error
    // based on severity, message content, and context
    
    // Convert log level to error severity
    ErrorSeverity errorSeverity;
    if (entry.level >= LogLevel::Error) {
        errorSeverity = (entry.level >= LogLevel::Critical) ? 
                       ErrorSeverity::Critical : ErrorSeverity::Error;
    } else if (entry.level == LogLevel::Warning) {
        errorSeverity = ErrorSeverity::Warning;
    } else {
        errorSeverity = ErrorSeverity::Info;
    }
    
    // Check if error severity is high enough
    if (errorSeverity < ErrorSeverity::Error) {
        return false;
    }
    
    // Check message content for error indicators
    std::string message = entry.message;
    std::transform(message.begin(), message.end(), message.begin(), ::tolower);
    
    // Check for error keywords
    if (message.find("error") != std::string::npos ||
        message.find("fail") != std::string::npos ||
        message.find("exception") != std::string::npos ||
        message.find("critical") != std::string::npos) {
        return true;
    }
    
    // Check for context indicators
    if (!entry.errorContext.empty()) {
        return true;
    }
    
    return false;
}

void Logger::debug(const std::string& message, const std::string& file, int line, 
                   const std::string& function, const std::string& errorContext) {
    log(LogLevel::Debug, message, file, line, function, errorContext);
}

void Logger::info(const std::string& message, const std::string& file, int line, 
                  const std::string& function, const std::string& errorContext) {
    log(LogLevel::Info, message, file, line, function, errorContext);
}

void Logger::warning(const std::string& message, const std::string& file, int line, 
                     const std::string& function, const std::string& errorContext) {
    log(LogLevel::Warning, message, file, line, function, errorContext);
}

void Logger::error(const std::string& message, const std::string& file, int line, 
                   const std::string& function, const std::string& errorContext) {
    log(LogLevel::Error, message, file, line, function, errorContext);
}

void Logger::critical(const std::string& message, const std::string& file, int line, 
                      const std::string& function, const std::string& errorContext) {
    log(LogLevel::Critical, message, file, line, function, errorContext);
}

bool Logger::isEnabled(LogLevel level) const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    // Check if any logger supports this level
    for (const auto& logger : pImpl->loggers) {
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
    if (!logger) {
        throw std::invalid_argument("Global logger cannot be null");
    }
    
    // This is a simple implementation - in practice, you might want a more sophisticated
    // mechanism for managing the global logger
    getGlobal() = *logger;
}

void Logger::setErrorReporter(std::shared_ptr<IErrorReporter> errorReporter) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->errorReporter = errorReporter;
}

std::shared_ptr<IErrorReporter> Logger::getErrorReporter() const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    return pImpl->errorReporter;
}

uint64_t Logger::getTotalErrorCount() const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    return pImpl->totalErrorsReported;
}

uint64_t Logger::getTotalWarningCount() const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    return pImpl->totalWarningsReported;
}

uint64_t Logger::getErrorCount(ErrorCategory category) const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    auto it = pImpl->errorsByCategory.find(category);
    return (it != pImpl->errorsByCategory.end()) ? it->second : 0;
}

uint64_t Logger::getWarningCount(ErrorCategory category) const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    auto it = pImpl->warningsByCategory.find(category);
    return (it != pImpl->warningsByCategory.end()) ? it->second : 0;
}

std::string Logger::getErrorStatistics() const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    std::ostringstream oss;
    oss << "Error Statistics:\n";
    oss << "Total Errors: " << pImpl->totalErrorsReported << "\n";
    oss << "Total Warnings: " << pImpl->totalWarningsReported << "\n";
    
    if (!pImpl->errorsByCategory.empty()) {
        oss << "\nErrors by Category:\n";
        for (const auto& pair : pImpl->errorsByCategory) {
            oss << "  " << getCategoryString(pair.first) << ": " << pair.second << "\n";
        }
    }
    
    if (!pImpl->warningsByCategory.empty()) {
        oss << "\nWarnings by Category:\n";
        for (const auto& pair : pImpl->warningsByCategory) {
            oss << "  " << getCategoryString(pair.first) << ": " << pair.second << "\n";
        }
    }
    
    return oss.str();
}

std::vector<DetailedError> Logger::getRecentErrors(size_t maxCount) const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    if (pImpl->errorReporter) {
        return pImpl->errorReporter->getRecentErrors(maxCount);
    }
    
    // Fallback to global error reporter
    ErrorReporter& globalReporter = ErrorReporting::getErrorReporter();
    return globalReporter.getRecentErrors(maxCount);
}

std::vector<DetailedError> Logger::getRecentWarnings(size_t maxCount) const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    if (pImpl->errorReporter) {
        return pImpl->errorReporter->getRecentWarnings(maxCount);
    }
    
    // Fallback to global error reporter
    ErrorReporter& globalReporter = ErrorReporting::getErrorReporter();
    return globalReporter.getRecentWarnings(maxCount);
}

void Logger::clearErrors() {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    if (pImpl->errorReporter) {
        pImpl->errorReporter->clear();
    }
    
    // Also clear global error reporter
    ErrorReporting::clearErrors();
}

bool Logger::isSystemHealthy() const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    // Check global error reporter health
    return ErrorReporting::isSystemHealthy();
}

std::string Logger::getHealthReport() const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    std::ostringstream oss;
    oss << "Logger Health Report:\n";
    oss << "Total Errors: " << pImpl->totalErrorsReported << "\n";
    oss << "Total Warnings: " << pImpl->totalWarningsReported << "\n";
    oss << "System Healthy: " << (isSystemHealthy() ? "Yes" : "No") << "\n\n";
    
    // Get global health report
    oss << ErrorReporting::getHealthReport();
    
    return oss.str();
}

bool Logger::attemptRecovery(const std::string& operation, float maxRetries) {
    try {
        // Attempt recovery using the error reporter if available
        if (pImpl->errorReporter) {
            // Try to recover from recent errors
            auto recentErrors = pImpl->errorReporter->getRecentErrors(10);
            
            for (const auto& error : recentErrors) {
                if (error.category == ErrorCategory::System || 
                    error.category == ErrorCategory::Brain ||
                    error.category == ErrorCategory::Memory) {
                    // These are critical errors that might require recovery
                    // In a real implementation, you would have specific recovery logic
                    std::cerr << "[INFO] Attempting recovery for " << operation << ": " 
                              << error.message << std::endl;
                    
                    // For now, just log the attempt
                    // A real implementation would have actual recovery logic
                }
            }
        }
        
        // Also attempt recovery via global error reporter
        ErrorReporter& globalReporter = ErrorReporting::getErrorReporter();
        auto globalErrors = globalReporter.getRecentErrors(10);
        
        for (const auto& error : globalErrors) {
            if (error.category == ErrorCategory::System || 
                error.category == ErrorCategory::Brain ||
                error.category == ErrorCategory::Memory) {
                std::cerr << "[INFO] Attempting global recovery for " << operation << ": " 
                          << error.message << std::endl;
            }
        }
        
        return true; // Recovery attempted successfully
        
    } catch (const std::exception& e) {
        std::cerr << "[ERROR] Recovery attempt failed: " << e.what() << std::endl;
        return false;
    }
}

std::string Logger::getCategoryString(ErrorCategory category) const {
    static const std::unordered_map<ErrorCategory, std::string> categoryStrings = {
        {ErrorCategory::None, "NONE"},
        {ErrorCategory::Configuration, "CONFIG"},
        {ErrorCategory::Memory, "MEMORY"},
        {ErrorCategory::Brain, "BRAIN"},
        {ErrorCategory::Logger, "LOGGER"},
        {ErrorCategory::World, "WORLD"},
        {ErrorCategory::Validation, "VALIDATION"},
        {ErrorCategory::System, "SYSTEM"},
        {ErrorCategory::Network, "NETWORK"},
        {ErrorCategory::IO, "IO"},
        {ErrorCategory::Plasticity, "PLASTICITY"},
        {ErrorCategory::Neuromodulation, "NEUROMOD"},
        {ErrorCategory::Cognitive, "COGNITIVE"},
        {ErrorCategory::Development, "DEVELOPMENT"},
        {ErrorCategory::Prediction, "PREDICTION"},
        {ErrorCategory::MemorySystem, "MEMSYS"},
        {ErrorCategory::Checkpoint, "CHECKPOINT"}
    };
    
    auto it = categoryStrings.find(category);
    return (it != categoryStrings.end()) ? it->second : "UNKNOWN";
}

// ErrorAwareLogger implementation
struct ErrorAwareLogger::Impl {
    std::shared_ptr<ILogger> underlyingLogger;
    std::shared_ptr<IErrorReporter> errorReporter;
    std::string errorContext;
    ErrorSeverity minErrorSeverity;
    ErrorSeverity maxErrorSeverity;
    std::vector<ErrorCategory> errorCategories;
};

ErrorAwareLogger::ErrorAwareLogger(std::shared_ptr<ILogger> underlyingLogger)
    : pImpl(std::make_unique<Impl>()) {
    if (!underlyingLogger) {
        throw std::invalid_argument("Underlying logger cannot be null");
    }
    
    pImpl->underlyingLogger = underlyingLogger;
    pImpl->minErrorSeverity = ErrorSeverity::Warning;
    pImpl->maxErrorSeverity = ErrorSeverity::Fatal;
}

ErrorAwareLogger::~ErrorAwareLogger() = default;

void ErrorAwareLogger::log(const LogEntry& entry) {
    try {
        // Report as error/warning if needed
        if (shouldReportAsError(entry)) {
            reportAsError(entry);
        } else if (entry.level >= LogLevel::Warning && entry.level <= LogLevel::Error) {
            reportAsWarning(entry);
        }
        
        // Log to underlying logger
        pImpl->underlyingLogger->log(entry);
        
    } catch (const std::exception& e) {
        std::cerr << "[ERROR] ErrorAwareLogger failed: " << e.what() << std::endl;
        // Try to recover
        recoverFromError();
    }
}

void ErrorAwareLogger::setLevel(LogLevel level) {
    pImpl->underlyingLogger->setLevel(level);
}

LogLevel ErrorAwareLogger::getLevel() const {
    return pImpl->underlyingLogger->getLevel();
}

bool ErrorAwareLogger::isEnabled(LogLevel level) const {
    return pImpl->underlyingLogger->isEnabled(level);
}

void ErrorAwareLogger::setErrorReporter(std::shared_ptr<IErrorReporter> errorReporter) {
    pImpl->errorReporter = errorReporter;
}

std::shared_ptr<IErrorReporter> ErrorAwareLogger::getErrorReporter() const {
    return pImpl->errorReporter;
}

void ErrorAwareLogger::setErrorContext(const std::string& context) {
    pImpl->errorContext = context;
}

std::string ErrorAwareLogger::getErrorContext() const {
    return pImpl->errorContext;
}

void ErrorAwareLogger::setErrorSeverityFilter(ErrorSeverity minSeverity, ErrorSeverity maxSeverity) {
    pImpl->minErrorSeverity = minSeverity;
    pImpl->maxErrorSeverity = maxSeverity;
}

void ErrorAwareLogger::setErrorCategoryFilter(ErrorCategory category) {
    pImpl->errorCategories.clear();
    pImpl->errorCategories.push_back(category);
}

std::vector<ErrorCategory> ErrorAwareLogger::getErrorCategories() const {
    return pImpl->errorCategories;
}

void ErrorAwareLogger::reportAsError(const LogEntry& entry) {
    // Convert LogEntry to DetailedError
    DetailedError error = createDetailedError(entry);
    
    // Apply error context
    if (!pImpl->errorContext.empty()) {
        error.context = pImpl->errorContext + " | " + error.context;
    }
    
    // Apply category filter if set
    if (!pImpl->errorCategories.empty() && 
        std::find(pImpl->errorCategories.begin(), pImpl->errorCategories.end(), error.category) == pImpl->errorCategories.end()) {
        return; // Skip if not in category filter
    }
    
    // Apply severity filter
    if (error.severity < pImpl->minErrorSeverity || error.severity > pImpl->maxErrorSeverity) {
        return; // Skip if outside severity range
    }
    
    // Report error
    if (pImpl->errorReporter) {
        pImpl->errorReporter->reportError(error);
    } else {
        // Fallback to global error reporter
        ErrorReporter& globalReporter = ErrorReporting::getErrorReporter();
        globalReporter.reportError(error);
    }
}

void ErrorAwareLogger::reportAsWarning(const LogEntry& entry) {
    // Convert LogEntry to DetailedError
    DetailedError warning = createDetailedError(entry);
    warning.severity = ErrorSeverity::Warning;
    
    // Apply error context
    if (!pImpl->errorContext.empty()) {
        warning.context = pImpl->errorContext + " | " + warning.context;
    }
    
    // Apply category filter if set
    if (!pImpl->errorCategories.empty() && 
        std::find(pImpl->errorCategories.begin(), pImpl->errorCategories.end(), warning.category) == pImpl->errorCategories.end()) {
        return; // Skip if not in category filter
    }
    
    // Apply severity filter
    if (warning.severity < pImpl->minErrorSeverity || warning.severity > pImpl->maxErrorSeverity) {
        return; // Skip if outside severity range
    }
    
    // Report warning
    if (pImpl->errorReporter) {
        pImpl->errorReporter->reportWarning(warning);
    } else {
        // Fallback to global error reporter
        ErrorReporter& globalReporter = ErrorReporting::getErrorReporter();
        globalReporter.reportWarning(warning);
    }
}

bool ErrorAwareLogger::shouldReportAsError(const LogEntry& entry) const {
    // Check if this log entry should be reported as an error
    // Based on severity, message content, and context
    
    // Convert log level to error severity
    ErrorSeverity errorSeverity;
    if (entry.level >= LogLevel::Error) {
        errorSeverity = (entry.level >= LogLevel::Critical) ? 
                       ErrorSeverity::Critical : ErrorSeverity::Error;
    } else {
        errorSeverity = ErrorSeverity::Info;
    }
    
    // Check severity range
    if (errorSeverity < pImpl->minErrorSeverity || errorSeverity > pImpl->maxErrorSeverity) {
        return false;
    }
    
    // Check message content for error indicators
    std::string message = entry.message;
    std::transform(message.begin(), message.end(), message.begin(), ::tolower);
    
    if (message.find("error") != std::string::npos ||
        message.find("fail") != std::string::npos ||
        message.find("exception") != std::string::npos) {
        return true;
    }
    
    // Check context indicators
    if (!entry.errorContext.empty() || !pImpl->errorContext.empty()) {
        return true;
    }
    
    return false;
}

void ErrorAwareLogger::recoverFromError() {
    try {
        // Attempt to recover from error state
        std::cerr << "[INFO] ErrorAwareLogger recovering from error" << std::endl;
        
        // Reset error reporter if possible
        if (pImpl->errorReporter) {
            // Try to clear error reporter state
            pImpl->errorReporter->clear();
        }
        
        // Reconnect to global error reporter if needed
        if (!pImpl->errorReporter) {
            ErrorReporter& globalReporter = ErrorReporting::getErrorReporter();
            pImpl->errorReporter = std::shared_ptr<IErrorReporter>(&globalReporter, [](IErrorReporter*) {});
        }
        
        std::cout << "[INFO] Error recovery completed" << std::endl;
        
    } catch (const std::exception& e) {
        std::cerr << "[ERROR] Error recovery failed: " << e.what() << std::endl;
    }
}

// ErrorAwareLogStream implementation
ErrorAwareLogStream::ErrorAwareLogStream(LogLevel level, const std::string& file, int line, 
                                       const std::string& function, const std::string& errorContext)
    : level_(level), file_(file), line_(line), function_(function), 
      errorContext_(errorContext), errorSeverity_(ErrorSeverity::Info), errorCategory_(ErrorCategory::None) {
}
ErrorAwareLogStream::~ErrorAwareLogStream() {
    try {
        // Convert stream content to string
        std::string message = oss_.str();
        if (!message.empty()) {
            // Create log entry
            LogEntry entry(level_, message, file_, line_, function_);
            entry.errorContext = errorContext_;
            
            // Log the message
            Logger::getGlobal().log(entry);
        }
    } catch (const std::exception& e) {
        std::cerr << "[ERROR] ErrorAwareLogStream failed: " << e.what() << std::endl;
    }
}

} // namespace nlm

// Helper function to create DetailedError from LogEntry
inline nlm::DetailedError createDetailedError(const nlm::LogEntry& entry) {
    nlm::ErrorCategory category = nlm::ErrorCategory::None;
    nlm::ErrorSeverity severity = nlm::ErrorSeverity::Info;
    
    // Map log level to error severity
    if (entry.level >= nlm::LogLevel::Error) {
        severity = (entry.level >= nlm::LogLevel::Critical) ? 
                   nlm::ErrorSeverity::Critical : nlm::ErrorSeverity::Error;
    } else if (entry.level == nlm::LogLevel::Warning) {
        severity = nlm::ErrorSeverity::Warning;
    } else if (entry.level == nlm::LogLevel::Info) {
        severity = nlm::ErrorSeverity::Info;
    } else { // Debug
        severity = nlm::ErrorSeverity::Debug;
    }
    
    // Determine error category based on log content and context
    std::string message = entry.message;
    if (message.find("[Error ") != std::string::npos) {
        category = nlm::ErrorCategory::Brain;
    } else if (message.find("[Config ") != std::string::npos) {
        category = nlm::ErrorCategory::Configuration;
    } else if (message.find("[Memory ") != std::string::npos) {
        category = nlm::ErrorCategory::Memory;
    } else if (message.find("[Logger ") != std::string::npos) {
        category = nlm::ErrorCategory::Logger;
    } else if (message.find("[World ") != std::string::npos) {
        category = nlm::ErrorCategory::World;
    } else if (message.find("[Validation ") != std::string::npos) {
        category = nlm::ErrorCategory::Validation;
    }
    
    // Create DetailedError from LogEntry
    nlm::DetailedError error(category, "LOG_" + std::to_string(static_cast<int>(severity)),
                            message, entry.errorContext, severity);
    error.file = entry.file;
    error.line = entry.line;
    error.function = entry.function;
    error.timestamp = entry.timestamp;
    error.errorSeverity = severity;
    
    return error;
}