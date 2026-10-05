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

// Enhanced logging with log rotation and file size limits
#include <filesystem>
#include <iomanip>
#include <ctime>
#include <random>

namespace nlm {

struct ConsoleLogger::Impl {
    LogLevel level = LogLevel::Info;
    bool useColors = true;
    std::ofstream fileOut;
    std::mutex mutex;
    
    // Log rotation configuration
    uintmax_t maxFileSize = 10 * 1024 * 1024;  // 10MB default
    uint32_t maxFileCount = 5;                 // Keep 5 rotated files
    bool rotateDaily = false;
    bool rotateOnStartup = false;
    std::string baseFilepath;
    uintmax_t currentFileSize = 0;
    std::chrono::system_clock::time_point lastRotation;
    
    // Format options
    std::string format = "%(level)s - %(message)s";  // Default format
    bool includeTimestamp = true;
    bool includeFileInfo = true;
    bool includeFunctionInfo = true;
    
    // Structured logging
    bool enableStructuredLogging = false;
    std::string structuredFormat = "{\"timestamp\": \"%(timestamp)s\", \"level\": \"%(level)s\", \"message\": \"%(message)s\"}";
};

ConsoleLogger::ConsoleLogger() : pImpl(std::make_unique<Impl>()) {
    // Initialize rotation tracking
    pImpl->lastRotation = std::chrono::system_clock::now();
}

ConsoleLogger::ConsoleLogger(LogLevel level) : pImpl(std::make_unique<Impl>()) {
    pImpl->level = level;
    pImpl->lastRotation = std::chrono::system_clock::now();
}

ConsoleLogger::~ConsoleLogger() = default;

void ConsoleLogger::log(const LogEntry& entry) {
    if (entry.level < pImpl->level) {
        return;
    }
    
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    // Check if rotation is needed
    checkRotation();
    
    std::ostringstream oss;
    
    if (pImpl->enableStructuredLogging) {
        // Structured JSON logging
        oss << formatLogEntryStructured(entry);
    } else {
        // Standard formatted logging
        oss << formatLogEntry(entry);
    }
    
    std::string output = oss.str();
    
    // Console output
    if (pImpl->useColors && !pImpl->enableStructuredLogging) {
        std::cout << levelToColor(entry.level) << output << "\033[0m\n";
    } else {
        std::cout << output << "\n";
    }
    
    // File output if set
    if (pImpl->fileOut.is_open()) {
        pImpl->fileOut << output << "\n";
        pImpl->fileOut.flush();
        pImpl->currentFileSize += output.length() + 1;  // +1 for newline
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
    // Close existing file if open
    if (pImpl->fileOut.is_open()) {
        pImpl->fileOut.close();
    }
    
    pImpl->baseFilepath = filepath;
    pImpl->currentFileSize = 0;
    
    // Open file with rotation if needed
    openOutputFile();
}

void ConsoleLogger::closeOutputFile() {
    if (pImpl->fileOut.is_open()) {
        pImpl->fileOut.close();
    }
}

void ConsoleLogger::setMaxFileSize(uintmax_t maxSize) {
    pImpl->maxFileSize = maxSize;
}

void ConsoleLogger::setMaxFileCount(uint32_t maxCount) {
    pImpl->maxFileCount = maxCount;
}

void ConsoleLogger::setRotateDaily(bool rotate) {
    pImpl->rotateDaily = rotate;
}

void ConsoleLogger::setRotateOnStartup(bool rotate) {
    pImpl->rotateOnStartup = rotate;
}

void ConsoleLogger::setLogFormat(const std::string& format) {
    pImpl->format = format;
}

void ConsoleLogger::setIncludeTimestamp(bool include) {
    pImpl->includeTimestamp = include;
}

void ConsoleLogger::setIncludeFileInfo(bool include) {
    pImpl->includeFileInfo = include;
}

void ConsoleLogger::setIncludeFunctionInfo(bool include) {
    pImpl->includeFunctionInfo = include;
}

void ConsoleLogger::setStructuredLogging(bool enable, const std::string& format) {
    pImpl->enableStructuredLogging = enable;
    if (!format.empty()) {
        pImpl->structuredFormat = format;
    }
}

std::string ConsoleLogger::formatLogEntry(const LogEntry& entry) const {
    std::ostringstream oss;
    
    // Simple template replacement
    std::string message = pImpl->format;
    
    // Replace common placeholders
    size_t pos;
    while ((pos = message.find("%(level)s")) != std::string::npos) {
        message.replace(pos, 10, levelToString(entry.level));
    }
    
    while ((pos = message.find("%(message)s")) != std::string::npos) {
        message.replace(pos, 12, entry.message);
    }
    
    if (pImpl->includeTimestamp) {
        while ((pos = message.find("%(timestamp)s")) != std::string::npos) {
            auto time = std::chrono::system_clock::to_time_t(entry.timestamp);
            std::tm tm_time = *std::localtime(&time);
            char timestamp[100];
            std::strftime(timestamp, sizeof(timestamp), "%Y-%m-%d %H:%M:%S", &tm_time);
            message.replace(pos, 15, timestamp);
        }
    }
    
    if (pImpl->includeFileInfo && !entry.file.empty()) {
        while ((pos = message.find("%(file)s")) != std::string::npos) {
            message.replace(pos, 8, entry.file);
        }
        
        while ((pos = message.find("%(line)d")) != std::string::npos) {
            std::string lineStr = std::to_string(entry.line);
            message.replace(pos, 7, lineStr);
        }
    }
    
    if (pImpl->includeFunctionInfo && !entry.function.empty()) {
        while ((pos = message.find("%(function)s")) != std::string::npos) {
            message.replace(pos, 12, entry.function);
        }
    }
    
    return message;
}

std::string ConsoleLogger::formatLogEntryStructured(const LogEntry& entry) const {
    std::ostringstream oss;
    
    oss << "{";
    oss << "\"timestamp\": \"" << timestampToString(entry.timestamp) << "\", ";
    oss << "\"level\": \"" << levelToString(entry.level) << "\", ";
    oss << "\"message\": \"" << escapeJson(entry.message) << "\"";
    
    if (pImpl->includeFileInfo && !entry.file.empty()) {
        oss << ", \"file\": \"" << escapeJson(entry.file) << "\"";
        oss << ", \"line\": " << entry.line;
    }
    
    if (pImpl->includeFunctionInfo && !entry.function.empty()) {
        oss << ", \"function\": \"" << escapeJson(entry.function) << "\"";
    }
    
    oss << "}";
    return oss.str();
}

void ConsoleLogger::checkRotation() {
    auto now = std::chrono::system_clock::now();
    
    // Check time-based rotation
    if (pImpl->rotateDaily) {
        auto today = std::chrono::time_point_cast<std::chrono::hours>(now).time_since_epoch();
        auto lastToday = std::chrono::time_point_cast<std::chrono::hours>(pImpl->lastRotation).time_since_epoch();
        
        if (today != lastToday) {
            rotateFile();
        }
    }
    
    // Check size-based rotation
    if (pImpl->currentFileSize >= pImpl->maxFileSize) {
        rotateFile();
    }
    
    // Check startup rotation
    if (pImpl->rotateOnStartup && !pImpl->fileOut.is_open()) {
        rotateFile();
    }
}

void ConsoleLogger::rotateFile() {
    if (!pImpl->baseFilepath.empty()) {
        // Close current file
        if (pImpl->fileOut.is_open()) {
            pImpl->fileOut.close();
        }
        
        // Generate rotated filename
        std::string rotatedPath = pImpl->baseFilepath;
        
        // Add timestamp or counter if file exists
        if (std::filesystem::exists(pImpl->baseFilepath)) {
            auto time = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
            char timestamp[20];
            std::strftime(timestamp, sizeof(timestamp), "_%Y%m%d_%H%M%S", std::localtime(&time));
            
            // Insert timestamp before extension
            size_t dotPos = rotatedPath.find_last_of('.');
            if (dotPos != std::string::npos) {
                rotatedPath.insert(dotPos, timestamp);
            } else {
                rotatedPath += timestamp;
            }
        }
        
        // Open rotated file
        pImpl->fileOut.open(rotatedPath, std::ios::app);
        pImpl->currentFileSize = 0;
        pImpl->lastRotation = std::chrono::system_clock::now();
        
        // Clean up old rotated files if count exceeds limit
        cleanupOldFiles();
    }
}

void ConsoleLogger::cleanupOldFiles() {
    if (pImpl->maxFileCount == 0) return;
    
    std::vector<std::string> files;
    std::string directory = std::filesystem::path(pImpl->baseFilepath).parent_path().string();
    std::string filename = std::filesystem::path(pImpl->baseFilepath).filename().string();
    
    for (const auto& entry : std::filesystem::directory_iterator(directory)) {
        if (entry.is_regular_file()) {
            std::string filepath = entry.path().string();
            // Check if file matches our pattern (base + optional timestamp)
            if (filepath.find(filename) != std::string::npos) {
                files.push_back(filepath);
            }
        }
    }
    
    // Sort files by modification time (oldest first)
    std::sort(files.begin(), files.end(), [](const std::string& a, const std::string& b) {
        return std::filesystem::last_write_time(std::filesystem::path(a)) < std::filesystem::last_write_time(std::filesystem::path(b));
    });
    
    // Remove oldest files if count exceeds limit
    while (files.size() > pImpl->maxFileCount) {
        std::error_code ec;
        std::filesystem::remove(files.front(), ec);
        files.erase(files.begin());
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
    if (!pImpl->useColors || pImpl->enableStructuredLogging) {
        return "\033[0m";
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

std::string ConsoleLogger::timestampToString(std::chrono::system_clock::time_point timestamp) const {
    auto time = std::chrono::system_clock::to_time_t(timestamp);
    std::tm tm_time = *std::localtime(&time);
    char buffer[20];
    std::strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", &tm_time);
    return std::string(buffer);
}

std::string ConsoleLogger::escapeJson(const std::string& input) const {
    std::ostringstream oss;
    for (char c : input) {
        switch (c) {
            case '\\': oss << "\\\\"; break;
            case '"': oss << "\\\""; break;
            case '/': oss << "\\/"; break;
            case '\b': oss << "\\b"; break;
            case '\f': oss << "\\f"; break;
            case '\n': oss << "\\n"; break;
            case '\r': oss << "\\r"; break;
            case '\t': oss << "\\t"; break;
            default:
                if (c < ' ' || c == '\x7f') {
                    oss << "\\u" << std::hex << std::setw(4) << std::setfill('0') << (int)c;
                } else {
                    oss << c;
                }
                break;
        }
    }
    return oss.str();
}

void openOutputFile() {
    if (!pImpl->baseFilepath.empty()) {
        pImpl->fileOut.open(pImpl->baseFilepath, std::ios::app);
        pImpl->currentFileSize = 0;
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
