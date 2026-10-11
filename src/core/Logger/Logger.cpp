#include "Logger.hpp"
#include <iostream>
#include <fstream>
#include <iomanip>
#include <sstream>
#include <mutex>
#include <chrono>
#include <ctime>
#include <algorithm>
#include <filesystem>

namespace nlm {

// ConsoleLogger implementation with enhanced features
struct ConsoleLogger::Impl {
    LogLevel level = LogLevel::Info;
    bool useColors = true;
    bool enableStructured = false;
    std::ofstream fileOut;
    std::string outputFile;
    size_t maxFileSize = 10 * 1024 * 1024; // 10 MB
    int maxLogFiles = 5;
    size_t currentFileSize = 0;
    std::mutex mutex;
    ILogFormatter* customFormatter = nullptr;
    
    std::string levelToString(LogLevel level) const;
    std::string levelToColor(LogLevel level) const;
};

ConsoleLogger::ConsoleLogger() : pImpl(std::make_unique<Impl>()) {
    pImpl->level = LogLevel::Info;
    pImpl->useColors = true;
    pImpl->enableStructured = false;
    pImpl->currentFileSize = 0;
}

ConsoleLogger::ConsoleLogger(LogLevel level) : pImpl(std::make_unique<Impl>()) {
    pImpl->level = level;
    pImpl->useColors = true;
    pImpl->enableStructured = false;
    pImpl->currentFileSize = 0;
}

ConsoleLogger::~ConsoleLogger() = default;

void ConsoleLogger::log(const LogEntry& entry) {
    // Check level first before acquiring mutex (optimization)
    if (entry.level < pImpl->level && !pImpl->enableStructured) {
        return;
    }
    
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    std::ostringstream oss;
    
    if (pImpl->enableStructured) {
        // Format as structured log (logfmt)
        oss << entry.traceId << " " << entry.spanId << " ";
        oss << pImpl->levelToString(entry.level) << " ";
        oss << entry.message << " ";
        
        for (const auto& field : entry.fields) {
            oss << field.first << "=" << field.second << " ";
        }
        oss << "at=" << entry.file << ":" << entry.line;
    } else {
        // Standard formatted log
        oss << "[" << pImpl->levelToString(entry.level) << "] ";
        
        // Add timestamp
        auto time = std::chrono::system_clock::to_time_t(entry.timestamp);
        oss << std::put_time(std::localtime(&time), "%Y-%m-%d %H:%M:%S");
        
        oss << " " << entry.message;
    }
    
    std::string output = oss.str();
    
    // Console output
    if (pImpl->useColors) {
        std::cout << pImpl->levelToColor(entry.level) << output << "\033[0m\n";
    } else {
        std::cout << output << "\n";
    }
    
    // File output with rotation if set
    if (pImpl->fileOut.is_open()) {
        pImpl->fileOut << output << "\n";
        pImpl->fileOut.flush();
        pImpl->currentFileSize += output.size() + 1; // +1 for newline
        
        // Check if file needs rotation
        if (pImpl->currentFileSize > pImpl->maxFileSize) {
            rotateLogFile();
        }
    }
}

void ConsoleLogger::rotateLogFile() {
    if (!pImpl->fileOut.is_open()) {
        return;
    }
    
    pImpl->fileOut.close();
    
    // Close oldest files if we have too many
    if (pImpl->maxLogFiles > 0) {
        std::string baseName = pImpl->outputFile;
        size_t dotPos = baseName.find_last_of('.');
        if (dotPos != std::string::npos) {
            std::string extension = baseName.substr(dotPos);
            std::string stem = baseName.substr(0, dotPos);
            
            for (int i = pImpl->maxLogFiles - 1; i >= 0; --i) {
                std::string oldFile = stem + "." + std::to_string(i) + extension;
                if (i < pImpl->maxLogFiles - 1) {
                    std::string newFile = stem + "." + std::to_string(i + 1) + extension;
                    if (std::filesystem::exists(oldFile)) {
                        std::filesystem::rename(oldFile, newFile);
                    }
                }
            }
        }
    }
    
    // Rename current file to .1
    std::filesystem::rename(pImpl->outputFile, pImpl->outputFile + ".1");
    
    // Reopen the file for new logs
    pImpl->fileOut.open(pImpl->outputFile, std::ios::app);
    pImpl->currentFileSize = 0;
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
    
    pImpl->outputFile = filepath;
    pImpl->fileOut.open(filepath, std::ios::app);
    if (pImpl->fileOut.is_open()) {
        pImpl->fileOut.seekp(0, std::ios::end);
        pImpl->currentFileSize = pImpl->fileOut.tellp();
    }
}

void ConsoleLogger::setMaxLogFileSize(size_t maxSize) {
    pImpl->maxFileSize = maxSize;
}

void ConsoleLogger::setMaxLogFiles(int maxFiles) {
    pImpl->maxLogFiles = maxFiles;
}

void ConsoleLogger::closeOutputFile() {
    if (pImpl->fileOut.is_open()) {
        pImpl->fileOut.close();
    }
}

void ConsoleLogger::setFormat(ILogFormatter* formatter) {
    pImpl->customFormatter = formatter;
}

void ConsoleLogger::setEnableStructured(bool enable) {
    pImpl->enableStructured = enable;
}

bool ConsoleLogger::isEnabled(LogLevel level) const {
    return level >= pImpl->level;
}

const std::string& ConsoleLogger::getOutputFile() const {
    return pImpl->outputFile;
}

size_t ConsoleLogger::getCurrentLogFileSize() const {
    return pImpl->currentFileSize;
}

std::string ConsoleLogger::Impl::levelToString(LogLevel level) const {
    switch (level) {
        case LogLevel::Trace: return "TRACE";
        case LogLevel::Debug: return "DEBUG";
        case LogLevel::Info: return "INFO";
        case LogLevel::Warning: return "WARN";
        case LogLevel::Error: return "ERROR";
        case LogLevel::Critical: return "CRIT";
        default: return "UNKNOWN";
    }
}

std::string ConsoleLogger::Impl::levelToColor(LogLevel level) const {
    if (!useColors) {
        return "";
    }
    
    switch (level) {
        case LogLevel::Trace: return "\033[36m";    // Cyan
        case LogLevel::Debug: return "\033[37m";    // White
        case LogLevel::Info: return "\033[32m";     // Green
        case LogLevel::Warning: return "\033[33m";  // Yellow
        case LogLevel::Error: return "\033[31m";    // Red
        case LogLevel::Critical: return "\033[35m"; // Magenta
        default: return "\033[0m";
    }
}

// JsonLogFormatter implementation
JsonLogFormatter::JsonLogFormatter() = default;

JsonLogFormatter::~JsonLogFormatter() = default;

std::string JsonLogFormatter::format(const LogEntry& entry) {
    return formatJson(entry);
}

std::string JsonLogFormatter::formatJson(const LogEntry& entry) {
    std::ostringstream oss;
    oss << "{";
    
    // Basic fields
    oss << "\"timestamp\": \"" << std::chrono::system_clock::to_time_t(entry.timestamp) << "\",";
    oss << "\"level\": \"" << levelToString(entry.level) << "\",";
    oss << "\"message\": \"" << escapeJsonString(entry.message) << "\",";
    oss << "\"file\": \"" << escapeJsonString(entry.file) << "\",";
    oss << "\"line\": " << entry.line << ",";
    oss << "\"function\": \"" << escapeJsonString(entry.function) << "\"";
    
    // Add fields if present
    if (!entry.fields.empty()) {
        oss << ",\"fields\": {";
        bool first = true;
        for (const auto& field : entry.fields) {
            if (!first) oss << ",";
            oss << "\"" << escapeJsonString(field.first) << "\": \"" << escapeJsonString(field.second) << "\"";
            first = false;
        }
        oss << "}";
    }
    
    oss << "}";
    return oss.str();
}

std::string JsonLogFormatter::formatLogfmt(const LogEntry& entry) {
    std::ostringstream oss;
    oss << entry.traceId << " " << entry.spanId << " ";
    oss << levelToString(entry.level) << " ";
    oss << entry.message << " ";
    
    for (const auto& field : entry.fields) {
        oss << field.first << "=" << field.second << " ";
    }
    oss << "at=" << entry.file << ":" << entry.line;
    
    return oss.str();
}

std::string JsonLogFormatter::escapeJsonString(const std::string& str) {
    std::ostringstream oss;
    for (char c : str) {
        switch (c) {
            case '\\': oss << "\\\\"; break;
            case '\"': oss << "\\\""; break;
            case '\b': oss << "\\b"; break;
            case '\f': oss << "\\f"; break;
            case '\n': oss << "\\n"; break;
            case '\r': oss << "\\r"; break;
            case '\t': oss << "\\t"; break;
            default:
                if (c < 32 || c > 126) {
                    oss << "\\u" << std::hex << std::setw(4) << std::setfill('0') << (int)c;
                } else {
                    oss << c;
                }
                break;
        }
    }
    return oss.str();
}

std::string JsonLogFormatter::levelToString(LogLevel level) const {
    switch (level) {
        case LogLevel::Trace: return "TRACE";
        case LogLevel::Debug: return "DEBUG";
        case LogLevel::Info: return "INFO";
        case LogLevel::Warning: return "WARNING";
        case LogLevel::Error: return "ERROR";
        case LogLevel::Critical: return "CRITICAL";
        default: return "UNKNOWN";
    }
}

// Logger class implementation
struct Logger::Impl {
    std::vector<std::shared_ptr<ILogger>> loggers;
    std::mutex mutex;
    ILogFormatter* defaultFormatter = nullptr;
    bool enableStructured = false;
};

Logger::Logger() : pImpl(std::make_unique<Impl>()) {
    pImpl->defaultFormatter = new JsonLogFormatter();
}

Logger::~Logger() {
    delete pImpl->defaultFormatter;
}

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
    LogEntry entry;
    entry.level = level;
    entry.message = message;
    entry.file = file;
    entry.line = line;
    entry.function = function;
    entry.timestamp = std::chrono::system_clock::now();
    
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    for (auto& logger : pImpl->loggers) {
        logger->log(entry);
    }
}

void Logger::log(LogLevel level, const std::string& message,
                 const std::map<std::string, std::string>& fields,
                 const std::string& file, int line, const std::string& function) {
    LogEntry entry;
    entry.level = level;
    entry.message = message;
    entry.file = file;
    entry.line = line;
    entry.function = function;
    entry.timestamp = std::chrono::system_clock::now();
    entry.fields = fields;
    
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    for (auto& logger : pImpl->loggers) {
        logger->log(entry);
    }
}

void Logger::trace(const std::string& message, const std::string& file, int line, const std::string& function) {
    log(LogLevel::Trace, message, file, line, function);
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

void Logger::setDefaultFormat(ILogFormatter* formatter) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    delete pImpl->defaultFormatter;
    pImpl->defaultFormatter = formatter;
}

void Logger::setEnableStructuredLogging(bool enable) {
    pImpl->enableStructured = enable;
}

} // namespace nlm
