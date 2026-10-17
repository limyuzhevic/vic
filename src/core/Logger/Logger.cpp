#include "Logger.hpp"
#include <iostream>
#include <fstream>
#include <iomanip>
#include <sstream>
#include <mutex>
#include <thread>
#include <chrono>
#include <filesystem>

namespace nlm {

struct PerformanceMetric {
    std::string name;
    std::chrono::high_resolution_clock::time_point start;
    std::chrono::high_resolution_clock::duration duration;
    bool running;
};

struct ConsoleLogger::Impl {
    LogLevel level = LogLevel::Info;
    bool useColors = true;
    std::ofstream fileOut;
    std::mutex mutex;
    std::string logFile;
    uint64_t maxFileSize = 10 * 1024 * 1024; // 10MB
    uint64_t currentFileSize = 0;
    bool useJsonFormat = false;
    bool enablePerformanceLogging = false;
    std::vector<PerformanceMetric> performanceMetrics;
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
    
    if (pImpl->useJsonFormat) {
        oss << "{\"timestamp\": \"" << formatTimestamp(entry.timestamp) << "\", \"level\": \"
            << levelToString(entry.level) << "\", \"message\": \"" << escapeJson(entry.message)
            << "\", \"file\": \"" << escapeJson(entry.file) << "\", \"line\": "
            << entry.line << ", \"function\": \"" << escapeJson(entry.function) << "\"}";
    } else {
        oss << "[" << levelToString(entry.level) << "] ";
        
        // Add timestamp
        auto time = std::chrono::system_clock::to_time_t(entry.timestamp);
        oss << std::put_time(std::localtime(&time), "%Y-%m-%d %H:%M:%S");
        
        oss << " " << entry.message;
        
        // Add file and line for debugging
        if (!entry.file.empty() && entry.line > 0) {
            oss << " (" << entry.file << ":" << entry.line << ")";
        }
        
        // Add function name for debugging
        if (!entry.function.empty()) {
            oss << " [" << entry.function << "]";
        }
    }
    
    std::string output = oss.str();
    
    // Console output
    if (pImpl->useColors) {
        std::cout << levelToColor(entry.level) << output << "\033[0m\n";
    } else {
        std::cout << output << "\n";
    }
    
    // File output if set
    if (pImpl->fileOut.is_open()) {
        // Check file size and rotate if needed
        if (pImpl->currentFileSize + output.size() + 1 > pImpl->maxFileSize) {
            rotateLogFile();
        }
        
        pImpl->fileOut << output << "\n";
        pImpl->fileOut.flush();
        pImpl->currentFileSize += output.size() + 1;
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
    
    // Close existing file
    if (pImpl->fileOut.is_open()) {
        pImpl->fileOut.close();
    }
    
    pImpl->logFile = filepath;
    pImpl->fileOut.open(filepath, std::ios::app);
    pImpl->currentFileSize = pImpl->fileOut.tellp();
}

void ConsoleLogger::closeOutputFile() {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    if (pImpl->fileOut.is_open()) {
        pImpl->fileOut.close();
    }
}

void ConsoleLogger::rotateLogFile() {
    if (pImpl->fileOut.is_open()) {
        pImpl->fileOut.close();
        
        // Rotate with timestamp
        auto now = std::chrono::system_clock::now();
        auto time = std::chrono::system_clock::to_time_t(now);
        std::stringstream ss;
        ss << std::put_time(std::localtime(&time), "%Y%m%d_%H%M%S");
        
        std::string rotatedFile = pImpl->logFile + "." + ss.str();
        std::rename(pImpl->logFile.c_str(), rotatedFile.c_str());
        
        pImpl->fileOut.open(pImpl->logFile, std::ios::app);
        pImpl->currentFileSize = 0;
    }
}

std::string ConsoleLogger::formatTimestamp(std::chrono::system_clock::time_point timestamp) const {
    auto time = std::chrono::system_clock::to_time_t(timestamp);
    std::stringstream ss;
    ss << std::put_time(std::localtime(&time), "%Y-%m-%dT%H:%M:%S.%3fZ");
    return ss.str();
}

std::string ConsoleLogger::escapeJson(const std::string& str) const {
    std::string result;
    result.reserve(str.size());
    
    for (char c : str) {
        switch (c) {
            case '\"': result += "\\\""; break;
            case '\\': result += "\\\\"; break;
            case '\b': result += "\\b"; break;
            case '\f': result += "\\f"; break;
            case '\n': result += "\\n"; break;
            case '\r': result += "\\r"; break;
            case '\t': result += "\\t"; break;
            default:
                if (c < ' ' || c == '\x7f') {
                    // Control character
                    std::stringstream ss;
                    ss << "\\u" << std::hex << std::setw(4) << std::setfill('0') << (int)c;
                    result += ss.str();
                } else {
                    result += c;
                }
        }
    }
    
    return result;
}

void ConsoleLogger::setUseJsonFormat(bool useJson) {
    pImpl->useJsonFormat = useJson;
}

bool ConsoleLogger::getUseJsonFormat() const {
    return pImpl->useJsonFormat;
}

void ConsoleLogger::setMaxFileSize(uint64_t maxSize) {
    pImpl->maxFileSize = maxSize;
}

uint64_t ConsoleLogger::getMaxFileSize() const {
    return pImpl->maxFileSize;
}

void ConsoleLogger::enablePerformanceLogging(bool enable) {
    pImpl->enablePerformanceLogging = enable;
}

bool ConsoleLogger::isPerformanceLoggingEnabled() const {
    return pImpl->enablePerformanceLogging;
}

std::string ConsoleLogger::startPerformanceMetric(const std::string& name) {
    if (!pImpl->enablePerformanceLogging) {
        return "";
    }
    
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    PerformanceMetric metric;
    metric.name = name;
    metric.start = std::chrono::high_resolution_clock::now();
    metric.running = true;
    
    std::stringstream ss;
    ss << std::chrono::high_resolution_clock::now().time_since_epoch().count();
    std::string id = ss.str();
    
    pImpl->performanceMetrics.push_back(std::move(metric));
    return id;
}

void ConsoleLogger::endPerformanceMetric(const std::string& metricId) {
    if (!pImpl->enablePerformanceLogging) {
        return;
    }
    
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    for (auto& metric : pImpl->performanceMetrics) {
        if (metric.running && metric.name == metricId) {
            auto end = std::chrono::high_resolution_clock::now();
            metric.duration = end - metric.start;
            metric.running = false;
            
            // Log performance
            std::ostringstream oss;
            oss << "[PERF] " << metric.name << ": "
                << std::chrono::duration<double, std::milli>(metric.duration).count() << " ms";
            
            if (pImpl->useColors) {
                std::cout << "\033[90m" << oss.str() << "\033[0m\n";
            } else {
                std::cout << oss.str() << "\n";
            }
            
            break;
        }
    }
}

void ConsoleLogger::printPerformanceMetrics() {
    if (!pImpl->enablePerformanceLogging) {
        return;
    }
    
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    if (pImpl->performanceMetrics.empty()) {
        return;
    }
    
    std::cout << "=== Performance Metrics ===\n";
    for (const auto& metric : pImpl->performanceMetrics) {
        double duration_ms = std::chrono::duration<double, std::milli>(metric.duration).count();
        std::cout << "  " << metric.name << ": " << duration_ms << " ms\n";
    }
    std::cout << "======================\n";
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
    std::chrono::high_resolution_clock::time_point lastLogTime;
    uint64_t logCount = 0;
    double averageLogTime = 0.0;
};

Logger::Logger() : pImpl(std::make_unique<Impl>()) {
    auto now = std::chrono::high_resolution_clock::now();
    pImpl->lastLogTime = now;
}

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
    
    auto now = std::chrono::high_resolution_clock::now();
    auto duration = now - pImpl->lastLogTime;
    
    // Update performance metrics
    pImpl->averageLogTime = (pImpl->averageLogTime * pImpl->logCount + 
                           std::chrono::duration<double, std::milli>(duration).count()) /
                          (pImpl->logCount + 1);
    pImpl->logCount++;
    pImpl->lastLogTime = now;
    
    LogEntry entry;
    entry.level = level;
    entry.message = message;
    entry.file = file;
    entry.line = line;
    entry.function = function;
    entry.timestamp = now;
    
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