// Unified Logging System Implementation

#include "core/Logging/Logging.hpp"
#include <algorithm>
#include <cstdarg>
#include <sstream>
#include <iomanip>
#include <mutex>
#include <chrono>

namespace nlm {
namespace logging {

// ConsoleLogSink implementation
ConsoleLogSink::ConsoleLogSink(bool colored) : colored(colored) {
    currentLevel = LogLevel::INFO;
}

void ConsoleLogSink::write(const LogEntry& entry) {
    std::lock_guard<std::mutex> lock(mutex);
    
    if (!shouldLog(entry.level)) {
        return;
    }
    
    std::cout << formatLevel(entry.level);
    
    if (colored && entry.level != LogLevel::OFF) {
        std::cout << getLevelColor(entry.level);
    }
    
    std::cout << "[" << entry.timestamp << "]";
    
    if (!entry.module.empty()) {
        std::cout << "[" << entry.module << "]";
    }
    
    std::cout << " " << entry.message;
    
    if (colored && entry.level != LogLevel::OFF) {
        std::cout << "\033[0m";
    }
    
    std::cout << std::endl;
}

void ConsoleLogSink::flush() {
    std::lock_guard<std::mutex> lock(mutex);
    std::cout.flush();
}

void ConsoleLogSink::setLevel(LogLevel level) {
    currentLevel = level;
}

std::string ConsoleLogSink::formatLevel(LogLevel level) const {
    switch (level) {
        case LogLevel::FATAL: return "FATAL";   
        case LogLevel::ERROR: return "ERROR";  
        case LogLevel::WARN: return "WARN";    
        case LogLevel::INFO: return "INFO";    
        case LogLevel::DEBUG: return "DEBUG";  
        case LogLevel::TRACE: return "TRACE";  
        case LogLevel::OFF: return "OFF";      
        default: return "UNKNOWN";
    }
}

std::string ConsoleLogSink::getLevelColor(LogLevel level) const {
    switch (level) {
        case LogLevel::FATAL: return "\033[1;31m";  // Bold Red
        case LogLevel::ERROR: return "\033[31m";   // Red
        case LogLevel::WARN: return "\033[33m";    // Yellow
        case LogLevel::INFO: return "\033[32m";    // Green
        case LogLevel::DEBUG: return "\033[36m";   // Cyan
        case LogLevel::TRACE: return "\033[90m";  // Gray
        default: return "";
    }
}

// FileLogSink implementation
FileLogSink::FileLogSink(const std::string& filename) {
    currentLevel = LogLevel::INFO;
    maxFileSize = 10 * 1024 * 1024; // 10MB
    maxFileCount = 5;
    logFormat = "[%timestamp%] [%level] [%module] %message";
    currentFile = filename;
    currentFileSize = 0;
    
    rotateFileIfNeeded();
}

FileLogSink::~FileLogSink() {
    if (file) {
        flush();
    }
}

void FileLogSink::write(const LogEntry& entry) {
    std::lock_guard<std::mutex> lock(mutex);
    
    if (!isOpen() || !shouldLog(entry.level)) {
        return;
    }
    
    rotateFileIfNeeded();
    
    *file << formatLogEntry(entry) << std::endl;
    currentFileSize += entry.message.size() + 100; // Rough estimate
    
    if (logFormat.find("%message\n") != std::string::npos) {
        flush();
    }
}

void FileLogSink::flush() {
    std::lock_guard<std::mutex> lock(mutex);
    if (file) {
        file->flush();
    }
}

void FileLogSink::setLevel(LogLevel level) {
    currentLevel = level;
}

void FileLogSink::setFileSizeLimit(size_t maxSize) {
    maxFileSize = maxSize;
}

void FileLogSink::setFileCountLimit(size_t maxFiles) {
    maxFileCount = maxFiles;
}

void FileLogSink::setLogFormat(const std::string& format) {
    logFormat = format;
}

void FileLogSink::rotateFileIfNeeded() {
    if (!file || file->fail()) {
        file = std::make_unique<std::ofstream>(currentFile, std::ios::app);
        if (!file || file->fail()) {
            return;
        }
        currentFileSize = file->tellp();
    }
    
    if (currentFileSize >= maxFileSize) {
        file->close();
        
        // Rotate files
        for (int i = maxFileCount - 1; i > 0; --i) {
            std::string from = getRotatedFilename(currentFile, i - 1);
            std::string to = getRotatedFilename(currentFile, i);
            std::rename(from.c_str(), to.c_str());
        }
        
        std::string newFile = getRotatedFilename(currentFile, 0);
        std::rename(currentFile.c_str(), newFile.c_str());
        
        file = std::make_unique<std::ofstream>(currentFile, std::ios::app);
        currentFileSize = 0;
    }
}

std::string FileLogSink::getRotatedFilename(const std::string& baseFilename, int index) const {
    if (index == 0) {
        return baseFilename;
    }
    
    size_t dotPos = baseFilename.find_last_of('.');
    size_t slashPos = baseFilename.find_last_of("/\\");
    
    std::string name = (slashPos != std::string::npos) ? 
        baseFilename.substr(slashPos + 1) : baseFilename;
    
    std::string extension = (dotPos != std::string::npos) ? 
        baseFilename.substr(dotPos) : "";
    
    std::string basePath = (slashPos != std::string::npos) ? 
        baseFilename.substr(0, slashPos + 1) : "";
    
    return basePath + name + "." + std::to_string(index) + extension;
}

// JsonLogSink implementation
JsonLogSink::JsonLogSink(const std::string& filename) {
    file = std::make_unique<std::ofstream>(filename);
    if (file) {
        *file << "[\n";
    }
}

JsonLogSink::~JsonLogSink() {
    if (file && !file->fail()) {
        *file << "\n]";
        flush();
    }
}

void JsonLogSink::write(const LogEntry& entry) {
    std::lock_guard<std::mutex> lock(mutex);
    
    if (!isOpen()) {
        return;
    }
    
    *file << "{\n";
    *file << "  \"timestamp\": \"" << entry.timestamp << "\",\n";
    *file << "  \"level\": \"" << static_cast<int>(entry.level) << "\",\n";
    *file << "  \"module\": \"" << entry.module << "\",\n";
    *file << "  \"message\": \"" << entry.message << "\",\n";
    *file << "  \"file\": \"" << entry.file << "\",\n";
    *file << "  \"line\": " << entry.line << ",\n";
    
    if (!entry.metadata.empty()) {
        *file << "  \"metadata\": {\n";
        bool first = true;
        for (const auto& meta : entry.metadata) {
            if (!first) *file << ",\n";
            first = false;
            *file << "    \"" << meta.first << "\": \"" << meta.second << "\"";
        }
        *file << "\n  },\n";
    }
    
    if (!entry.tags.empty()) {
        *file << "  \"tags\": [\n";
        for (size_t i = 0; i < entry.tags.size(); ++i) {
            if (i > 0) *file << ",\n";
            *file << "    \"" << entry.tags[i] << "\"";
        }
        *file << "\n  ],\n";
    }
    
    *file << "}\n";
    
    if (entry != lastEntry) {
        *file << ",\n";
    }
    lastEntry = entry;
}

void JsonLogSink::flush() {
    std::lock_guard<std::mutex> lock(mutex);
    if (file) {
        file->flush();
    }
}

// Logger implementation
Logger& Logger::getInstance() {
    static Logger instance;
    return instance;
}

Logger::Logger() {
    defaultLevel = LogLevel::INFO;
    defaultModule = "NLM";
    maxLogBufferSize = 10000;
    
    fatalCount = errorCount = warnCount = infoCount = debugCount = traceCount = 0;
    
    // Add default console sink
    addSink(std::make_unique<ConsoleLogSink>(true));
}

Logger::~Logger() {
}

void Logger::setLevel(LogLevel level) {
    defaultLevel = level;
}

void Logger::log(LogLevel level, const std::string& message,
                 const std::string& file, int line,
                 const std::map<std::string, std::string>& metadata) {
    log(defaultModule, message, file, line, metadata);
}

void Logger::log(LogLevel level, const std::string& module, const std::string& message,
                 const std::string& file, int line,
                 const std::map<std::string, std::string>& metadata) {
    std::lock_guard<std::mutex> lock(mutex);
    
    if (!shouldLog(level)) {
        return;
    }
    
    LogEntry entry;
    entry.timestamp = std::chrono::system_clock::now();
    entry.level = level;
    entry.module = module;
    entry.message = message;
    entry.file = file;
    entry.line = line;
    entry.metadata = metadata;
    entry.colored = true;
    
    // Update statistics
    switch (level) {
        case LogLevel::FATAL: fatalCount++; break;
        case LogLevel::ERROR: errorCount++; break;
        case LogLevel::WARN: warnCount++; break;
        case LogLevel::INFO: infoCount++; break;
        case LogLevel::DEBUG: debugCount++; break;
        case LogLevel::TRACE: traceCount++; break;
        default: break;
    }
    
    logBuffer.push_back(entry);
    
    // Remove old entries if buffer is too large
    if (logBuffer.size() > maxLogBufferSize) {
        logBuffer.erase(logBuffer.begin(), logBuffer.begin() + 
                       (logBuffer.size() - maxLogBufferSize / 2));
    }
    
    outputToSinks(entry);
}

void Logger::fatal(const std::string& message) {
    log(LogLevel::FATAL, defaultModule, message);
}

void Logger::fatal(const std::string& module, const std::string& message) {
    log(LogLevel::FATAL, module, message);
}

void Logger::error(const std::string& message) {
    log(LogLevel::ERROR, defaultModule, message);
}

void Logger::error(const std::string& module, const std::string& message) {
    log(LogLevel::ERROR, module, message);
}

void Logger::warn(const std::string& message) {
    log(LogLevel::WARN, defaultModule, message);
}

void Logger::warn(const std::string& module, const std::string& message) {
    log(LogLevel::WARN, module, message);
}

void Logger::info(const std::string& message) {
    log(LogLevel::INFO, defaultModule, message);
}

void Logger::info(const std::string& module, const std::string& message) {
    log(LogLevel::INFO, module, message);
}

void Logger::debug(const std::string& message) {
    log(LogLevel::DEBUG, defaultModule, message);
}

void Logger::debug(const std::string& module, const std::string& message) {
    log(LogLevel::DEBUG, module, message);
}

void Logger::trace(const std::string& message) {
    log(LogLevel::TRACE, defaultModule, message);
}

void Logger::trace(const std::string& module, const std::string& message) {
    log(LogLevel::TRACE, module, message);
}

void Logger::addSink(std::unique_ptr<LogSink> sink) {
    std::lock_guard<std::mutex> lock(mutex);
    sinks.push_back(std::move(sink));
}

void Logger::removeSink(LogSink* sink) {
    std::lock_guard<std::mutex> lock(mutex);
    auto it = std::find_if(sinks.begin(), sinks.end(),
        [sink](const std::unique_ptr<LogSink>& ptr) { return ptr.get() == sink; });
    if (it != sinks.end()) {
        sinks.erase(it);
    }
}

void Logger::clearSinks() {
    std::lock_guard<std::mutex> lock(mutex);
    sinks.clear();
}

std::vector<LogEntry> Logger::getRecentLogs(size_t limit) const {
    std::lock_guard<std::mutex> lock(mutex);
    if (logBuffer.size() <= limit) {
        return logBuffer;
    }
    
    std::vector<LogEntry> recent(logBuffer.end() - limit, logBuffer.end());
    return recent;
}

std::vector<LogEntry> Logger::searchLogs(const std::string& pattern, LogLevel level) const {
    std::lock_guard<std::mutex> lock(mutex);
    std::vector<LogEntry> results;
    
    for (const auto& entry : logBuffer) {
        if (level == LogLevel::OFF || entry.level == level) {
            if (entry.message.find(pattern) != std::string::npos ||
                entry.module.find(pattern) != std::string::npos) {
                results.push_back(entry);
            }
        }
    }
    
    return results;
}

void Logger::logPerformance(const std::string& operation, std::chrono::duration<double> duration,
                           const std::map<std::string, std::string>& metadata) {
    std::stringstream ss;
    ss << "Performance: " << operation << " took " << duration.count() << "s";
    
    for (const auto& meta : metadata) {
        ss << " [" << meta.first << "=" << meta.second << "]";
    }
    
    info("Performance", ss.str());
}

void Logger::outputToSinks(const LogEntry& entry) {
    for (const auto& sink : sinks) {
        sink->write(entry);
    }
}

bool Logger::shouldLog(LogLevel level) const {
    return level >= defaultLevel;
}

// Helper function for formatString
std::string formatString(const std::string& format, ...) {
    va_list args;
    va_start(args, format);
    
    // Format string implementation
    std::vector<char> buffer(256);
    va_list args_copy;
    va_copy(args_copy, args);
    
    while (true) {
        va_list args_copy2;
        va_copy(args_copy2, args_copy);
        int result = std::vsnprintf(buffer.data(), buffer.size(), format.c_str(), args_copy2);
        va_end(args_copy2);
        
        if (result >= 0 && result < static_cast<int>(buffer.size())) {
            return std::string(buffer.data(), result);
        }
        
        buffer.resize(buffer.size() * 2);
        va_end(args_copy);
        va_copy(args_copy, args);
    }
}

// Initialize logging system
void initializeLogging(int argc, char* argv[]) {
    nlm::logging::Logger::getInstance().setLevel(nlm::logging::LogLevel::INFO);
    
    // Parse command line arguments for logging configuration
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        if (arg == "--log-level" && i + 1 < argc) {
            std::string levelStr = argv[++i];
            std::string upperLevel = levelStr;
            std::transform(upperLevel.begin(), upperLevel.end(), upperLevel.begin(), ::toupper);
            
            nlm::logging::LogLevel level;
            if (upperLevel == "OFF") level = nlm::logging::LogLevel::OFF;
            else if (upperLevel == "FATAL") level = nlm::logging::LogLevel::FATAL;
            else if (upperLevel == "ERROR") level = nlm::logging::LogLevel::ERROR;
            else if (upperLevel == "WARN") level = nlm::logging::LogLevel::WARN;
            else if (upperLevel == "INFO") level = nlm::logging::LogLevel::INFO;
            else if (upperLevel == "DEBUG") level = nlm::logging::LogLevel::DEBUG;
            else if (upperLevel == "TRACE") level = nlm::logging::LogLevel::TRACE;
            else level = nlm::logging::LogLevel::INFO;
            
            nlm::logging::Logger::getInstance().setLevel(level);
        }
    }
}

} // namespace logging
} // namespace nlm
