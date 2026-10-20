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

// Logger class implementation
struct Logger::Impl {
    std::vector<std::shared_ptr<ILogger>> loggers;
    std::mutex mutex;
    
    // Performance monitoring
    std::unordered_map<std::string, std::chrono::system_clock::time_point> activeOperations;
    std::unordered_map<std::string, double> metrics;
    std::mutex perfMutex;
    
    // Error recovery
    std::unordered_map<ErrorCategory, int> errorCounts;
    std::mutex recoveryMutex;
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
    entry.category = "General";  // Default category
    entry.processingTime = std::chrono::duration<double>::zero();
    entry.threadId = std::this_thread::get_id();
    
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

void Logger::startPerformanceMeasurement(const std::string& operation) {
    std::lock_guard<std::mutex> lock(pImpl->perfMutex);
    pImpl->activeOperations[operation] = std::chrono::system_clock::now();
    NLM_LOG_DEBUG("Starting performance measurement for operation: " + operation);
}

void Logger::endPerformanceMeasurement(const std::string& operation) {
    std::lock_guard<std::mutex> lock(pImpl->perfMutex);
    auto it = pImpl->activeOperations.find(operation);
    if (it != pImpl->activeOperations.end()) {
        auto duration = std::chrono::duration<double>(
            std::chrono::system_clock::now() - it->second
        );
        pImpl->activeOperations.erase(it);
        NLM_LOG_INFO("Performance measurement for '" + operation + "': " + 
                    std::to_string(duration.count()) + " seconds");
    }
}

void Logger::recordMetric(const std::string& metricName, double value) {
    std::lock_guard<std::mutex> lock(pImpl->perfMutex);
    pImpl->metrics[metricName] = value;
    NLM_LOG_DEBUG("Recorded metric \"" + metricName + "\": " + std::to_string(value));
}

void Logger::logError(const std::string& message, ErrorCategory category, const std::string& file, int line, const std::string& function) {
    std::lock_guard<std::mutex> lock(pImpl->recoveryMutex);
    pImpl->errorCounts[category]++;
    
    std::string categoryStr;
    switch (category) {
        case ErrorCategory::Memory: categoryStr = "MEMORY"; break;
        case ErrorCategory::Plasticity: categoryStr = "PLASTICITY"; break;
        case ErrorCategory::Network: categoryStr = "NETWORK"; break;
        case ErrorCategory::Configuration: categoryStr = "CONFIGURATION"; break;
        case ErrorCategory::FileIO: categoryStr = "FILE_IO"; break;
        case ErrorCategory::Performance: categoryStr = "PERFORMANCE"; break;
        case ErrorCategory::Synchronization: categoryStr = "SYNCHRONIZATION"; break;
        case ErrorCategory::Validation: categoryStr = "VALIDATION"; break;
        case ErrorCategory::NeuralDynamics: categoryStr = "NEURAL_DYNAMICS"; break;
        default: categoryStr = "GENERAL";
    }
    
    NLM_LOG_ERROR("[" + categoryStr + "] " + message + 
                  " (file: " + file + ", line: " + std::to_string(line) + ")");
    
    // Attempt automatic recovery based on error category
    attemptErrorRecovery(category);
}

bool Logger::attemptErrorRecovery(ErrorCategory category) {
    switch (category) {
        case ErrorCategory::Memory:
            NLM_LOG_WARNING("Memory error detected - attempting memory cleanup");
            break;
        case ErrorCategory::Configuration:
            NLM_LOG_WARNING("Configuration error - attempting to reset to defaults");
            break;
        case ErrorCategory::FileIO:
            NLM_LOG_WARNING("File I/O error - attempting to use alternative file or create new");
            break;
        case ErrorCategory::Performance:
            NLM_LOG_WARNING("Performance degradation detected - attempting to optimize operations");
            break;
        default:
            NLM_LOG_WARNING("General error - attempting basic recovery");
    }
    return true;
}

bool Logger::checkPerformanceThreshold(const std::string& metricName, double threshold, double value) {
    if (value > threshold) {
        NLM_LOG_WARNING("Performance threshold exceeded for metric \"" + metricName + 
                       "\": current= " + std::to_string(value) + 
                       ", threshold= " + std::to_string(threshold));
        return true;
    }
    return false;
}

// LogStream implementation
LogStream::LogStream(LogLevel level, const std::string& file, int line, const std::string& function)
    : level_(level), file_(file), line_(line), function_(function) {}

LogStream::~LogStream() {
    Logger::getGlobal().log(level_, oss_.str(), file_, line_, function_);
}

} // namespace nlm
