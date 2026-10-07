#include "Logger.hpp"
#include <iostream>
#include <fstream>
#include <iomanip>
#include <sstream>
#include <mutex>
#include <algorithm>
#include <filesystem>
#include <unordered_map>
#include <algorithm>

namespace nlm {

// ConsoleLogger implementation

struct ConsoleLogger::Impl {
    LogLevel level = LogLevel::Info;
    bool useColors = true;
    std::ofstream fileOut;
    std::mutex mutex;
    
    // Advanced formatting
    std::string format = "[%level] %timestamp %message";
    std::function<bool(const LogEntry&)> filter;
    
    // Log rotation
    size_t maxFileSize = 10 * 1024 * 1024; // 10 MB
    int rotationCount = 5;
    std::string currentFilepath;
    size_t currentFileSize = 0;
    
    // Performance timing
    std::unordered_map<std::string, std::pair<std::chrono::system_clock::time_point, std::chrono::milliseconds>> timers;
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
    
    if (pImpl->filter && !pImpl->filter(entry)) {
        return;
    }
    
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    std::ostringstream oss;
    oss << formatEntry(entry);
    
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
        
        // Check file size and rotate if needed
        pImpl->currentFileSize += output.size() + 1; // +1 for newline
        if (pImpl->currentFileSize >= pImpl->maxFileSize && pImpl->rotationCount > 0) {
            rotateFile();
        }
    }
}

void ConsoleLogger::setLevel(LogLevel level) {
    pImpl->level = level;
}

LogLevel ConsoleLogger::getLevel() const {
    return pImpl->level;
}

void ConsoleLogger::setFormat(const std::string& format) {
    pImpl->format = format;
}

std::string ConsoleLogger::getFormat() const {
    return pImpl->format;
}

void ConsoleLogger::setFilter(const std::function<bool(const LogEntry&)>& filter) {
    pImpl->filter = filter;
}

void ConsoleLogger::clearFilter() {
    pImpl->filter = nullptr;
}

void ConsoleLogger::setUseColors(bool useColors) {
    pImpl->useColors = useColors;
}

bool ConsoleLogger::getUseColors() const {
    return pImpl->useColors;
}

void ConsoleLogger::setOutputFile(const std::string& filepath) {
    closeOutputFile();
    pImpl->fileOut.open(filepath, std::ios::app);
    if (pImpl->fileOut.is_open()) {
        pImpl->currentFilepath = filepath;
        pImpl->currentFileSize = pImpl->fileOut.tellp();
    }
}

void ConsoleLogger::closeOutputFile() {
    if (pImpl->fileOut.is_open()) {
        pImpl->fileOut.close();
    }
}

bool ConsoleLogger::isFileOutputOpen() const {
    return pImpl->fileOut.is_open();
}

void ConsoleLogger::setMaxFileSize(size_t maxSizeBytes) {
    pImpl->maxFileSize = maxSizeBytes;
}

void ConsoleLogger::setFileRotationCount(int rotationCount) {
    pImpl->rotationCount = rotationCount;
}

size_t ConsoleLogger::getMaxFileSize() const {
    return pImpl->maxFileSize;
}

int ConsoleLogger::getFileRotationCount() const {
    return pImpl->rotationCount;
}

void ConsoleLogger::startTimer(const std::string& operation) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->timers[operation] = std::make_pair(std::chrono::system_clock::now(), std::chrono::milliseconds(0));
}

void ConsoleLogger::stopTimer(const std::string& operation) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    auto it = pImpl->timers.find(operation);
    if (it != pImpl->timers.end()) {
        auto startTime = it->second.first;
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now() - startTime);
        it->second.second = duration;
    }
}

std::chrono::milliseconds ConsoleLogger::getLastDuration(const std::string& operation) const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    auto it = pImpl->timers.find(operation);
    if (it != pImpl->timers.end()) {
        return it->second.second;
    }
    return std::chrono::milliseconds(0);
}

void ConsoleLogger::rotateFile() {
    if (!pImpl->fileOut.is_open() || pImpl->rotationCount <= 0) {
        return;
    }
    
    // Close current file
    pImpl->fileOut.close();
    
    // Rotate existing files
    for (int i = pImpl->rotationCount - 1; i >= 0; --i) {
        std::string source = (i == pImpl->rotationCount - 1) ? pImpl->currentFilepath : pImpl->currentFilepath + "." + std::to_string(i);
        std::string dest = pImpl->currentFilepath + "." + std::to_string(i + 1);
        
        if (std::filesystem::exists(source)) {
            std::filesystem::rename(source, dest);
        }
    }
    
    // Create new empty file
    pImpl->fileOut.open(pImpl->currentFilepath, std::ios::trunc);
    pImpl->currentFileSize = 0;
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

std::string ConsoleLogger::formatEntry(const LogEntry& entry) const {
    std::string output = pImpl->format;
    
    // Replace placeholders
    size_t pos = 0;
    while ((pos = output.find("%level", pos)) != std::string::npos) {
        output.replace(pos, 7, levelToString(entry.level));
        pos += levelToString(entry.level).size();
    }
    
    pos = 0;
    while ((pos = output.find("%timestamp", pos)) != std::string::npos) {
        auto time = std::chrono::system_clock::to_time_t(entry.timestamp);
        std::ostringstream timeOss;
        timeOss << std::put_time(std::localtime(&time), "%Y-%m-%d %H:%M:%S");
        output.replace(pos, 10, timeOss.str());
        pos += timeOss.str().size();
    }
    
    pos = 0;
    while ((pos = output.find("%message", pos)) != std::string::npos) {
        output.replace(pos, 8, entry.message);
        pos += entry.message.size();
    }
    
    pos = 0;
    while ((pos = output.find("%file", pos)) != std::string::npos) {
        output.replace(pos, 6, entry.file);
        pos += entry.file.size();
    }
    
    pos = 0;
    while ((pos = output.find("%line", pos)) != std::string::npos) {
        output.replace(pos, 6, std::to_string(entry.line));
        pos += std::to_string(entry.line).size();
    }
    
    pos = 0;
    while ((pos = output.find("%function", pos)) != std::string::npos) {
        output.replace(pos, 9, entry.function);
        pos += entry.function.size();
    }
    
    // Add metadata if present
    if (!entry.metadata.empty()) {
        output += " [";
        bool first = true;
        for (const auto& item : entry.metadata) {
            if (!first) output += ", ";
            output += item.first + "=" + item.second;
            first = false;
        }
        output += "]";
    }
    
    return output;
}

bool ConsoleLogger::shouldLog(const LogEntry& entry) const {
    if (pImpl->filter) {
        return pImpl->filter(entry);
    }
    return true;
}

// LogAggregator implementation

struct LogAggregator::Impl {
    std::vector<LogEntry> entries;
    std::mutex mutex;
    size_t batchSize = 100;
    std::chrono::milliseconds flushInterval = std::chrono::milliseconds(1000);
    std::chrono::system_clock::time_point lastFlush;
    std::function<void(const std::vector<LogEntry>&)> callback;
};

LogAggregator::LogAggregator() : pImpl(std::make_unique<Impl>()) {
    pImpl->lastFlush = std::chrono::system_clock::now();
}

LogAggregator::~LogAggregator() = default;

void LogAggregator::addEntry(const LogEntry& entry) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->entries.push_back(entry);
    
    // Auto-flush if batch size reached
    if (pImpl->entries.size() >= pImpl->batchSize) {
        processEntries();
    }
    
    // Auto-flush if interval exceeded
    auto now = std::chrono::system_clock::now();
    if (std::chrono::duration_cast<std::chrono::milliseconds>(now - pImpl->lastFlush) >= pImpl->flushInterval) {
        processEntries();
    }
}

void LogAggregator::processEntries() {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    if (!pImpl->entries.empty()) {
        if (pImpl->callback) {
            pImpl->callback(pImpl->entries);
        }
        pImpl->entries.clear();
        pImpl->lastFlush = std::chrono::system_clock::now();
    }
}

void LogAggregator::setBatchSize(size_t batchSize) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->batchSize = batchSize;
}

void LogAggregator::setFlushInterval(std::chrono::milliseconds interval) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->flushInterval = interval;
}

size_t LogAggregator::getEntryCount() const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    return pImpl->entries.size();
}

// ScopedTimer implementation

struct ScopedTimer::Impl {
    std::string operation;
    std::chrono::system_clock::time_point startTime;
    std::chrono::milliseconds duration;
    bool running;
};

ScopedTimer::ScopedTimer(const std::string& operation) : pImpl(std::make_unique<Impl>()) {
    pImpl->operation = operation;
    pImpl->running = false;
    pImpl->duration = std::chrono::milliseconds(0);
}

ScopedTimer::~ScopedTimer() {
    stop();
}

void ScopedTimer::start() {
    pImpl->startTime = std::chrono::system_clock::now();
    pImpl->running = true;
}

void ScopedTimer::stop() {
    if (pImpl->running) {
        auto endTime = std::chrono::system_clock::now();
        pImpl->duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - pImpl->startTime);
        pImpl->running = false;
    }
}

std::chrono::milliseconds ScopedTimer::duration() const {
    return pImpl->duration;
}

} // namespace nlm