#include "Logger.hpp"
#include <iostream>
#include <fstream>
#include <iomanip>
#include <sstream>
#include <mutex>
#include <thread>
#include <unordered_map>
#include <ctime>
#include <algorithm>
#include <filesystem>
#include <chrono>

namespace nlm {

struct ConsoleLogger::Impl {
    LogLevel level = LogLevel::Info;
    bool useColors = true;
    std::ofstream fileOut;
    std::string format = "[%level] %timestamp %category %message";
    std::mutex mutex;
    std::string timestampFormat = "%Y-%m-%d %H:%M:%S";
};

ConsoleLogger::ConsoleLogger() : pImpl(std::make_unique<Impl>()) {}

ConsoleLogger::ConsoleLogger(LogLevel level) : pImpl(std::make_unique<Impl>()) {
    pImpl->level = level;
}

ConsoleLogger::~ConsoleLogger() = default;

void ConsoleLogger::write(const LogEntry& entry) {
    if (entry.level < pImpl->level) {
        return;
    }
    
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    std::ostringstream oss;
    std::string format = pImpl->format;
    
    // Simple format replacement
    size_t pos = 0;
    while ((pos = format.find("%level", pos)) != std::string::npos) {
        format.replace(pos, 7, levelToString(entry.level));
        pos += levelToString(entry.level).length();
    }
    
    pos = 0;
    while ((pos = format.find("%category", pos)) != std::string::npos) {
        format.replace(pos, 9, categoryToString(entry.category));
        pos += categoryToString(entry.category).length();
    }
    
    pos = 0;
    while ((pos = format.find("%timestamp", pos)) != std::string::npos) {
        auto time = std::chrono::system_clock::to_time_t(entry.timestamp);
        char timestampBuf[100];
        std::strftime(timestampBuf, sizeof(timestampBuf), pImpl->timestampFormat.c_str(), std::localtime(&time));
        format.replace(pos, 10, timestampBuf);
        pos += strlen(timestampBuf);
    }
    
    pos = 0;
    while ((pos = format.find("%message", pos)) != std::string::npos) {
        format.replace(pos, 8, entry.message);
        pos += entry.message.length();
    }
    
    format += "\n";
    
    // Output
    if (pImpl->useColors) {
        std::cout << levelToColor(entry.level) << format << "\033[0m";
    } else {
        std::cout << format;
    }
    
    // File output if set
    if (pImpl->fileOut.is_open()) {
        pImpl->fileOut << format;
        pImpl->fileOut.flush();
    }
}

void ConsoleLogger::flush() {
    if (pImpl->fileOut.is_open()) {
        pImpl->fileOut.flush();
    }
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

void ConsoleLogger::setFormat(const std::string& format) {
    pImpl->format = format;
}

std::string ConsoleLogger::getFormat() const {
    return pImpl->format;
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

std::string ConsoleLogger::categoryToString(LogCategory category) const {
    switch (category) {
        case LogCategory::System: return "SYSTEM";
        case LogCategory::Neural: return "NEURAL";
        case LogCategory::Sensory: return "SENSORY";
        case LogCategory::Motor: return "MOTOR";
        case LogCategory::Learning: return "LEARNING";
        case LogCategory::Memory: return "MEMORY";
        case LogCategory::World: return "WORLD";
        case LogCategory::Performance: return "PERF";
        case LogCategory::Debug: return "DEBUG";
        case LogCategory::User: return "USER";
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

// FileLogger implementation
struct FileLogger::Impl {
    LogLevel level = LogLevel::Info;
    std::ofstream fileOut;
    std::string filepath;
    size_t maxFileSize = 10 * 1024 * 1024; // 10 MB
    int maxFiles = 5;
    bool enableRotation = true;
    std::string format = "%timestamp [%level] %category %message";
    std::string timestampFormat = "%Y-%m-%d %H:%M:%S.%f";
    size_t bufferSize = 8192;
    std::string currentFilename;
    std::mutex mutex;
};

FileLogger::FileLogger() : pImpl(std::make_unique<Impl>()) {}

FileLogger::FileLogger(LogLevel level) : pImpl(std::make_unique<Impl>()) {
    pImpl->level = level;
}

FileLogger::~FileLogger() = default;

void FileLogger::write(const LogEntry& entry) {
    if (entry.level < pImpl->level) {
        return;
    }
    
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    // Handle file rotation
    if (pImpl->enableRotation && pImpl->fileOut.is_open()) {
        auto currentPos = pImpl->fileOut.tellp();
        if (currentPos >= static_cast<std::ofstream::pos_type>(pImpl->maxFileSize)) {
            pImpl->fileOut.close();
            rotateFiles();
        }
    }
    
    // Format the log entry
    std::ostringstream oss;
    std::string format = pImpl->format;
    
    // Replace placeholders
    size_t pos = 0;
    while ((pos = format.find("%level", pos)) != std::string::npos) {
        format.replace(pos, 7, levelToString(entry.level));
        pos += levelToString(entry.level).length();
    }
    
    pos = 0;
    while ((pos = format.find("%category", pos)) != std::string::npos) {
        format.replace(pos, 9, categoryToString(entry.category));
        pos += categoryToString(entry.category).length();
    }
    
    pos = 0;
    while ((pos = format.find("%timestamp", pos)) != std::string::npos) {
        auto time = std::chrono::system_clock::to_time_t(entry.timestamp);
        char timestampBuf[100];
        std::strftime(timestampBuf, sizeof(timestampBuf), pImpl->timestampFormat.c_str(), std::localtime(&time));
        format.replace(pos, 10, timestampBuf);
        pos += strlen(timestampBuf);
    }
    
    pos = 0;
    while ((pos = format.find("%message", pos)) != std::string::npos) {
        format.replace(pos, 8, entry.message);
        pos += entry.message.length();
    }
    
    format += "\n";
    
    pImpl->fileOut << format;
}

void FileLogger::flush() {
    if (pImpl->fileOut.is_open()) {
        pImpl->fileOut.flush();
    }
}

void FileLogger::setFilePath(const std::string& filepath) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    if (pImpl->fileOut.is_open()) {
        pImpl->fileOut.close();
    }
    pImpl->filepath = filepath;
    openFile();
}

void FileLogger::setMaxFileSize(size_t maxSize) {
    pImpl->maxFileSize = maxSize;
}

void FileLogger::setMaxFiles(int maxFiles) {
    pImpl->maxFiles = maxFiles;
}

void FileLogger::enableRotation(bool enable) {
    pImpl->enableRotation = enable;
}

void FileLogger::setFormat(const std::string& format) {
    pImpl->format = format;
}

void FileLogger::setTimestampFormat(const std::string& format) {
    pImpl->timestampFormat = format;
}

void FileLogger::setBufferSize(size_t bufferSize) {
    pImpl->bufferSize = bufferSize;
}

size_t FileLogger::getBufferSize() const {
    return pImpl->bufferSize;
}

void FileLogger::rotateFiles() {
    if (!pImpl->enableRotation) return;
    
    // Close current file
    if (pImpl->fileOut.is_open()) {
        pImpl->fileOut.close();
    }
    
    // Rotate existing files
    for (int i = pImpl->maxFiles - 1; i >= 0; --i) {
        std::string src = pImpl->filepath;
        std::string dst = pImpl->filepath;
        
        if (i > 0) {
            dst += "." + std::to_string(i);
            std::rename(src.c_str(), dst.c_str());
        } else {
            dst += ".0";
            std::rename(src.c_str(), dst.c_str());
        }
    }
    
    // Open new file
    openFile();
}

void FileLogger::openFile() {
    if (pImpl->filepath.empty()) return;
    
    pImpl->currentFilename = pImpl->filepath;
    pImpl->fileOut.open(pImpl->filepath, std::ios::app);
    
    if (!pImpl->fileOut.is_open()) {
        throw std::runtime_error("Failed to open log file: " + pImpl->filepath);
    }
}

std::string FileLogger::levelToString(LogLevel level) const {
    switch (level) {
        case LogLevel::Debug: return "DEBUG";
        case LogLevel::Info: return "INFO";
        case LogLevel::Warning: return "WARN";
        case LogLevel::Error: return "ERROR";
        case LogLevel::Critical: return "CRIT";
        default: return "UNKNOWN";
    }
}

std::string FileLogger::categoryToString(LogCategory category) const {
    switch (category) {
        case LogCategory::System: return "SYSTEM";
        case LogCategory::Neural: return "NEURAL";
        case LogCategory::Sensory: return "SENSORY";
        case LogCategory::Motor: return "MOTOR";
        case LogCategory::Learning: return "LEARNING";
        case LogCategory::Memory: return "MEMORY";
        case LogCategory::World: return "WORLD";
        case LogCategory::Performance: return "PERF";
        case LogCategory::Debug: return "DEBUG";
        case LogCategory::User: return "USER";
        default: return "UNKNOWN";
    }
}

// MemoryLogger implementation
struct MemoryLogger::Impl {
    LogLevel level = LogLevel::Info;
    std::vector<LogEntry> entries;
    size_t maxEntries = 10000;
    mutable std::mutex mutex;
    size_t totalWritten = 0;
    double averageWriteTime = 0.0;
    std::chrono::system_clock::time_point firstEntry;
    std::chrono::system_clock::time_point lastEntry;
    size_t memoryUsage = 0;
};

MemoryLogger::MemoryLogger() : pImpl(std::make_unique<Impl>()) {}

MemoryLogger::MemoryLogger(LogLevel level) : pImpl(std::make_unique<Impl>()) {
    pImpl->level = level;
}

MemoryLogger::~MemoryLogger() = default;

void MemoryLogger::write(const LogEntry& entry) {
    if (entry.level < pImpl->level) {
        return;
    }
    
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    // Calculate memory usage
    size_t entrySize = sizeof(LogEntry) + entry.message.size() + entry.file.size() + 
                      entry.function.size();
    pImpl->memoryUsage += entrySize;
    pImpl->totalWritten++;
    
    // Set timestamps
    if (pImpl->totalWritten == 1) {
        pImpl->firstEntry = entry.timestamp;
    }
    pImpl->lastEntry = entry.timestamp;
    
    // Add entry
    pImpl->entries.push_back(entry);
    
    // Limit entries
    if (pImpl->entries.size() > pImpl->maxEntries) {
        pImpl->entries.erase(pImpl->entries.begin());
        pImpl->memoryUsage -= sizeof(LogEntry) + 100; // Estimate
    }
}

void MemoryLogger::flush() {
    // Nothing to flush for memory logger
}

std::vector<LogEntry> MemoryLogger::getRecentLogs(size_t count) const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    if (count > pImpl->entries.size()) {
        count = pImpl->entries.size();
    }
    return std::vector<LogEntry>(pImpl->entries.end() - count, pImpl->entries.end());
}

std::vector<LogEntry> MemoryLogger::getLogsByLevel(LogLevel level) const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    std::vector<LogEntry> result;
    result.reserve(pImpl->entries.size());
    
    for (const auto& entry : pImpl->entries) {
        if (entry.level == level) {
            result.push_back(entry);
        }
    }
    
    return result;
}

std::vector<LogEntry> MemoryLogger::getLogsByCategory(LogCategory category) const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    std::vector<LogEntry> result;
    result.reserve(pImpl->entries.size());
    
    for (const auto& entry : pImpl->entries) {
        if (entry.category == category) {
            result.push_back(entry);
        }
    }
    
    return result;
}

std::vector<LogEntry> MemoryLogger::search(const std::string& pattern) const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    std::vector<LogEntry> result;
    
    for (const auto& entry : pImpl->entries) {
        if (entry.message.find(pattern) != std::string::npos) {
            result.push_back(entry);
        }
    }
    
    return result;
}

void MemoryLogger::clear() {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->entries.clear();
    pImpl->memoryUsage = 0;
    pImpl->totalWritten = 0;
}

size_t MemoryLogger::size() const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    return pImpl->entries.size();
}

std::string MemoryLogger::exportAsJSON() const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    nlohmann::json json = pImpl->entries;
    return json.dump(2);
}

std::string MemoryLogger::exportAsCSV() const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    std::ostringstream oss;
    oss << "timestamp,level,category,message\n";
    
    for (const auto& entry : pImpl->entries) {
        auto time = std::chrono::system_clock::to_time_t(entry.timestamp);
        char timestampBuf[100];
        std::strftime(timestampBuf, sizeof(timestampBuf), "%Y-%m-%d %H:%M:%S", std::localtime(&time));
        
        oss << timestampBuf << ","
            << static_cast<int>(entry.level) << ","
            << static_cast<int>(entry.category) << ","
            << "\"" << entry.message << "\"\n";
    }
    
    return oss.str();
}

MemoryLogger::Stats MemoryLogger::getStats() const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    Stats stats;
    stats.totalWritten = pImpl->totalWritten;
    stats.memoryUsage = pImpl->memoryUsage;
    stats.averageWriteTime = pImpl->averageWriteTime;
    stats.firstEntry = pImpl->firstEntry;
    stats.lastEntry = pImpl->lastEntry;
    return stats;
}

// Logger implementation
struct Logger::Impl {
    std::vector<std::shared_ptr<ILogSink>> sinks;
    std::shared_ptr<ILogFilter> globalFilter;
    std::mutex mutex;
    PerformanceStats perfStats;
};

Logger::Logger() : pImpl(std::make_unique<Impl>()) {}

Logger::~Logger() = default;

void Logger::addSink(std::shared_ptr<ILogSink> sink) {
    if (sink) {
        std::lock_guard<std::mutex> lock(pImpl->mutex);
        pImpl->sinks.push_back(sink);
    }
}

void Logger::clearSinks() {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->sinks.clear();
}

std::vector<std::shared_ptr<ILogSink>> Logger::getSinks() const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    return pImpl->sinks;
}

void Logger::setLevel(LogLevel level) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    for (auto& sink : pImpl->sinks) {
        // Cast to known logger types
        if (auto consoleLogger = std::dynamic_pointer_cast<ConsoleLogger>(sink)) {
            consoleLogger->setLevel(level);
        } else if (auto fileLogger = std::dynamic_pointer_cast<FileLogger>(sink)) {
            fileLogger->setLevel(level);
        } else if (auto memoryLogger = std::dynamic_pointer_cast<MemoryLogger>(sink)) {
            memoryLogger->setLevel(level);
        }
    }
}

void Logger::setFilter(std::shared_ptr<ILogFilter> filter) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->globalFilter = filter;
    for (auto& sink : pImpl->sinks) {
        sink->setFilter(filter);
    }
}

void Logger::log(LogLevel level, LogCategory category, const std::string& message,
                 const std::string& file, int line, const std::string& function,
                 uint64_t simulationStep) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    pImpl->perfStats.logCount++;
    
    LogEntry entry;
    entry.level = level;
    entry.category = category;
    entry.message = message;
    entry.file = file;
    entry.line = line;
    entry.function = function;
    entry.timestamp = std::chrono::system_clock::now();
    entry.threadId = std::hash<std::thread::id>{}(std::this_thread::get_id());
    entry.simulationStep = simulationStep;
    
    // Check global filter
    if (pImpl->globalFilter && !pImpl->globalFilter->shouldLog(entry)) {
        return;
    }
    
    // Send to all sinks
    for (auto& sink : pImpl->sinks) {
        if (sink->getFilter() && !sink->getFilter()->shouldLog(entry)) {
            continue;
        }
        sink->write(entry);
    }
}

void Logger::debug(LogCategory category, const std::string& message,
                   const std::string& file, int line, const std::string& function,
                   uint64_t simulationStep) {
    log(LogLevel::Debug, category, message, file, line, function, simulationStep);
}

void Logger::info(LogCategory category, const std::string& message,
                  const std::string& file, int line, const std::string& function,
                  uint64_t simulationStep) {
    log(LogLevel::Info, category, message, file, line, function, simulationStep);
}

void Logger::warning(LogCategory category, const std::string& message,
                     const std::string& file, int line, const std::string& function,
                     uint64_t simulationStep) {
    log(LogLevel::Warning, category, message, file, line, function, simulationStep);
}

void Logger::error(LogCategory category, const std::string& message,
                   const std::string& file, int line, const std::string& function,
                   uint64_t simulationStep) {
    log(LogLevel::Error, category, message, file, line, function, simulationStep);
}

void Logger::critical(LogCategory category, const std::string& message,
                      const std::string& file, int line, const std::string& function,
                      uint64_t simulationStep) {
    log(LogLevel::Critical, category, message, file, line, function, simulationStep);
}

bool Logger::isEnabled(LogLevel level, LogCategory category) const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    if (pImpl->globalFilter && !pImpl->globalFilter->shouldLog(
        LogEntry{level, category, "", "", 0, "", std::chrono::system_clock::now()})) {
        return false;
    }
    
    for (const auto& sink : pImpl->sinks) {
        if (sink->getFilter() && !sink->getFilter()->shouldLog(
            LogEntry{level, category, "", "", 0, "", std::chrono::system_clock::now()})) {
            return false;
        }
    }
    
    return true;
}

static std::shared_ptr<Logger>& globalLoggerPtr() {
    static std::shared_ptr<Logger> ptr = std::make_shared<Logger>();
    
    // Add default sinks
    ptr->addSink(std::make_shared<ConsoleLogger>());
    ptr->addSink(std::make_shared<MemoryLogger>());
    
    return ptr;
}

Logger& Logger::getGlobal() {
    return *globalLoggerPtr();
}

void Logger::setGlobal(std::shared_ptr<Logger> logger) {
    if (logger) {
        globalLoggerPtr() = logger;
    }
}

Logger::PerformanceStats Logger::getPerformanceStats() const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    return pImpl->perfStats;
}

void Logger::resetPerformanceStats() {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->perfStats = PerformanceStats();
}

// LogStream implementation
LogStream::LogStream(LogLevel level, LogCategory category, const std::string& file, int line, const std::string& function)
    : level_(level), category_(category), file_(file), line_(line), function_(function) {}

LogStream::~LogStream() {
    Logger::getGlobal().log(level_, category_, oss_.str(), file_, line_, function_);
}

} // namespace nlm
