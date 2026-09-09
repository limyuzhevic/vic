#include "ConsoleLogger.hpp"
#include <iostream>
#include <fstream>
#include <mutex>
#include <atomic>
#include <thread>
#include <chrono>
#include <filesystem>
#include <deque>
#include <shared_mutex>
#include <algorithm>
#include <map>
#include <condition_variable>

namespace nlm {

struct ConsoleLogger::Impl {
    LogLevel level{LogLevel::Info};
    bool useColors{true};
    std::ofstream fileOut;
    std::string filePath;
    size_t bufferSize{1024};
    std::deque<LogEntry> logBuffer;
    std::mutex bufferMutex;
    std::condition_variable bufferCondVar;
    bool stopWorker{false};
    size_t workerThreadCount{1};
    std::vector<std::thread> workerThreads;
    std::atomic<size_t> activeWorkers{0};
    
    // Log rotation settings
    size_t maxFileSize{10 * 1024 * 1024}; // 10MB
    size_t maxFiles{5};
    
    // Statistics
    LoggerStats stats;
    std::string customPattern;
    
    // Thread safety
    mutable std::shared_mutex categoryMutex;
    std::map<std::string, bool> categoryFilters;
    
    // Output formatting
    std::function<std::string(const LogEntry&)> formatter;
};

ConsoleLogger::ConsoleLogger() : pImpl(std::make_unique<Impl>()) {
    // Initialize default categories
    pImpl->categoryFilters[LogCategory::All] = true;
    pImpl->categoryFilters[LogCategory::Neural] = true;
    pImpl->categoryFilters[LogCategory::Memory] = true;
    pImpl->categoryFilters[LogCategory::Sensory] = true;
    pImpl->categoryFilters[LogCategory::Motor] = true;
    pImpl->categoryFilters[LogCategory::Cognition] = true;
    pImpl->categoryFilters[LogCategory::Plasticity] = true;
    pImpl->categoryFilters[LogCategory::Development] = true;
    pImpl->categoryFilters[LogCategory::Neuromodulation] = true;
    pImpl->categoryFilters[LogCategory::Experiment] = true;
    pImpl->categoryFilters[LogCategory::Performance] = true;
    pImpl->categoryFilters[LogCategory::Config] = true;
    pImpl->categoryFilters[LogCategory::System] = true;
    
    // Start worker threads for async processing if enabled
    if (pImpl->bufferSize > 0) {
        startLoggingThreadPool(pImpl->workerThreadCount);
        for (size_t i = 0; i < pImpl->workerThreadCount; ++i) {
            pImpl->workerThreads.emplace_back(&ConsoleLogger::processAsyncLogs, this);
        }
    }
}

ConsoleLogger::ConsoleLogger(LogLevel level) : ConsoleLogger() {
    pImpl->level = level;
}

ConsoleLogger::ConsoleLogger(const LoggingConfig& config) : ConsoleLogger() {
    setLevel(config.defaultLevel);
    setUseColors(config.enableColors);
    
    if (config.enableFileLogging && !config.logFile.empty()) {
        setOutputFile(config.logFile);
        enableLogRotation(config.maxFileSize, config.maxFiles);
    }
    
    setBufferSize(config.enableAsyncLogging ? 1024 : 0);
    setWorkerThreadCount(config.asyncThreadCount);
}

ConsoleLogger::~ConsoleLogger() {
    stopWorker = true;
    bufferCondVar.notify_all();
    
    for (auto& thread : pImpl->workerThreads) {
        if (thread.joinable()) {
            thread.join();
        }
    }
    
    if (pImpl->fileOut.is_open()) {
        pImpl->fileOut.close();
    }
}

void ConsoleLogger::log(const LogEntry& entry) {
    // Check level filter
    if (entry.level < pImpl->level) {
        std::atomic_fetch_add(&pImpl->stats.messagesFiltered, 1);
        return;
    }
    
    // Check category filter
    bool categoryEnabled = false;
    {
        std::shared_lock lock(pImpl->categoryMutex);
        auto it = pImpl->categoryFilters.find(entry.category);
        if (it != pImpl->categoryFilters.end()) {
            categoryEnabled = it->second;
        }
    }
    
    if (!categoryEnabled) {
        std::atomic_fetch_add(&pImpl->stats.messagesFiltered, 1);
        return;
    }
    
    // Update statistics
    std::atomic_fetch_add(&pImpl->stats.messagesLogged, 1);
    pImpl->stats.messagesByLevel[entry.level]++;
    pImpl->stats.messagesByCategory[entry.category]++;
    pImpl->stats.currentMemoryUsage += entry.messageSize;
    
    // Process synchronously if no async enabled
    if (pImpl->bufferSize == 0) {
        std::lock_guard<std::mutex> lock(pImpl->bufferMutex);
        
        // Process immediately
        auto startTime = std::chrono::high_resolution_clock::now();
        
        std::string formattedOutput = formatEntry(entry);
        
        // Console output
        if (pImpl->useColors) {
            std::cout << levelToColor(entry.level) << formattedOutput << "\033[0m\n";
        } else {
            std::cout << formattedOutput << "\n";
        }
        
        // File output
        if (pImpl->fileOut.is_open()) {
            pImpl->fileOut << formattedOutput << "\n";
            pImpl->fileOut.flush();
            
            // Check for rotation
            if (pImpl->fileOut.tellp() >= static_cast<std::ofstream::pos_type>(pImpl->maxFileSize)) {
                rotateLogFile();
            }
        }
        
        auto endTime = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration<double>(endTime - startTime).count();
        std::atomic_fetch_add(&pImpl->stats.totalProcessingTime, duration);
    } else {
        // Async logging
        std::unique_lock<std::mutex> lock(pImpl->bufferMutex);
        pImpl->logBuffer.push_back(entry);
        
        auto startTime = std::chrono::high_resolution_clock::now();
        pImpl->stats.entriesAllocated++;
        
        lock.unlock();
        pImpl->bufferCondVar.notify_one();
    }
}

void ConsoleLogger::setLevel(LogLevel level) {
    pImpl->level = level;
}

LogLevel ConsoleLogger::getLevel() const {
    return pImpl->level;
}

bool ConsoleLogger::isLevelEnabled(LogLevel level) const {
    return level >= pImpl->level;
}

void ConsoleLogger::setCategoryFilter(const std::string& category, bool enabled) {
    std::unique_lock lock(pImpl->categoryMutex);
    pImpl->categoryFilters[category] = enabled;
}

bool ConsoleLogger::isCategoryEnabled(const std::string& category) const {
    std::shared_lock lock(pImpl->categoryMutex);
    auto it = pImpl->categoryFilters.find(category);
    if (it != pImpl->categoryFilters.end()) {
        return it->second;
    }
    return false;
}

void ConsoleLogger::flush() {
    if (pImpl->bufferSize > 0) {
        std::unique_lock<std::mutex> lock(pImpl->bufferMutex);
        bufferCondVar.wait(lock, [this] { return pImpl->logBuffer.empty(); });
    }
}

LoggerStats ConsoleLogger::getStats() const {
    return pImpl->stats;
}

void ConsoleLogger::setFormatter(std::function<std::string(const LogEntry&)> formatter) {
    pImpl->formatter = std::move(formatter);
}

void ConsoleLogger::logAsync(const LogEntry& entry) {
    // This is handled by the worker threads
    log(entry);
}

void ConsoleLogger::setWorkerThreadCount(size_t count) {
    if (count != pImpl->workerThreadCount) {
        pImpl->workerThreadCount = count;
        
        // Restart worker threads
        stopWorker = true;
        bufferCondVar.notify_all();
        
        for (auto& thread : pImpl->workerThreads) {
            if (thread.joinable()) {
                thread.join();
            }
        }
        
        pImpl->workerThreads.clear();
        stopWorker = false;
        
        startLoggingThreadPool(pImpl->workerThreadCount);
        for (size_t i = 0; i < pImpl->workerThreadCount; ++i) {
            pImpl->workerThreads.emplace_back(&ConsoleLogger::processAsyncLogs, this);
        }
    }
}

void ConsoleLogger::waitForCompletion() {
    flush();
}

void ConsoleLogger::setUseColors(bool useColors) {
    pImpl->useColors = useColors;
}

bool ConsoleLogger::getUseColors() const {
    return pImpl->useColors;
}

void ConsoleLogger::setOutputFile(const std::string& filepath) {
    std::lock_guard<std::mutex> lock(pImpl->bufferMutex);
    
    if (pImpl->fileOut.is_open()) {
        pImpl->fileOut.close();
    }
    
    pImpl->filePath = filepath;
    pImpl->fileOut.open(filepath, std::ios::app);
}

void ConsoleLogger::closeOutputFile() {
    std::lock_guard<std::mutex> lock(pImpl->bufferMutex);
    if (pImpl->fileOut.is_open()) {
        pImpl->fileOut.close();
    }
}

void ConsoleLogger::setBufferSize(size_t bufferSize) {
    std::lock_guard<std::mutex> lock(pImpl->bufferMutex);
    pImpl->bufferSize = bufferSize;
}

size_t ConsoleLogger::getBufferSize() const {
    return pImpl->bufferSize;
}

void ConsoleLogger::enableLogRotation(size_t maxFileSize, size_t maxFiles) {
    pImpl->maxFileSize = maxFileSize;
    pImpl->maxFiles = maxFiles;
}

void ConsoleLogger::setCustomPattern(const std::string& pattern) {
    pImpl->customPattern = pattern;
}

void ConsoleLogger::processAsyncLogs() {
    while (!stopWorker) {
        LogEntry entry;
        bool hasEntry = false;
        
        {
            std::unique_lock<std::mutex> lock(pImpl->bufferMutex);
            bufferCondVar.wait(lock, [this] { return !pImpl->logBuffer.empty() || stopWorker; });
            
            if (!pImpl->logBuffer.empty() && !stopWorker) {
                entry = pImpl->logBuffer.front();
                pImpl->logBuffer.pop_front();
                hasEntry = true;
            }
        }
        
        if (hasEntry) {
            auto startTime = std::chrono::high_resolution_clock::now();
            
            std::string formattedOutput = formatEntry(entry);
            
            // Console output
            if (pImpl->useColors) {
                std::cout << levelToColor(entry.level) << formattedOutput << "\033[0m\n";
            } else {
                std::cout << formattedOutput << "\n";
            }
            
            // File output
            if (pImpl->fileOut.is_open()) {
                pImpl->fileOut << formattedOutput << "\n";
                pImpl->fileOut.flush();
                
                // Check for rotation
                if (pImpl->fileOut.tellp() >= static_cast<std::ofstream::pos_type>(pImpl->maxFileSize)) {
                    rotateLogFile();
                }
            }
            
            auto endTime = std::chrono::high_resolution_clock::now();
            auto duration = std::chrono::duration<double>(endTime - startTime).count();
            std::atomic_fetch_add(&pImpl->stats.totalProcessingTime, duration);
        }
    }
}

void ConsoleLogger::rotateLogFile() {
    if (pImpl->fileOut.is_open()) {
        pImpl->fileOut.close();
        
        std::error_code ec;
        std::filesystem::path filepath(pImpl->filePath);
        std::string stem = filepath.stem().string();
        std::string ext = filepath.extension().string();
        
        // Rotate existing files
        for (size_t i = pImpl->maxFiles - 1; i > 0; --i) {
            std::filesystem::path oldFile = filepath.parent_path() / (stem + "_" + std::to_string(i) + ext);
            std::filesystem::path newFile = filepath.parent_path() / (stem + "_" + std::to_string(i + 1) + ext);
            
            if (std::filesystem::exists(oldFile, ec)) {
                std::filesystem::rename(oldFile, newFile, ec);
            }
        }
        
        // Move current log to _1
        std::filesystem::path rotatedFile = filepath.parent_path() / (stem + "_1" + ext);
        if (std::filesystem::exists(filepath, ec)) {
            std::filesystem::rename(filepath, rotatedFile, ec);
        }
        
        // Reopen the log file
        pImpl->fileOut.open(filepath, std::ios::app);
    }
}

std::string ConsoleLogger::formatEntry(const LogEntry& entry) const {
    if (pImpl->formatter) {
        return pImpl->formatter(entry);
    }
    
    // Default formatting
    std::ostringstream oss;
    oss << "[" << levelToString(entry.level) << "] ";
    
    // Add timestamp
    auto time = std::chrono::system_clock::to_time_t(entry.timestamp);
    oss << std::put_time(std::localtime(&time), "%Y-%m-%d %H:%M:%S");
    
    // Add thread ID
    oss << " [tid:" << entry.threadId << "]";
    
    // Add category
    if (!entry.category.empty() && entry.category != LogCategory::All) {
        oss << " [" << entry.category << "]";
    }
    
    // Add context if available
    bool hasContext = false;
    if (!entry.neuronId.empty()) {
        oss << " [neuron:" << entry.neuronId << "]";
        hasContext = true;
    }
    if (!entry.simulationStep.empty()) {
        oss << " [step:" << entry.simulationStep << "]";
        hasContext = true;
    }
    if (!entry.regionId.empty()) {
        oss << " [region:" << entry.regionId << "]";
        hasContext = true;
    }
    
    oss << " " << entry.message;
    
    // Add structured data
    for (const auto& kv : entry.kvPairs) {
        oss << " " << kv.first << "=" << kv.second;
    }
    
    return oss.str();
}

std::string ConsoleLogger::levelToString(LogLevel level) const {
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

std::string ConsoleLogger::levelToColor(LogLevel level) const {
    switch (level) {
        case LogLevel::Trace: return "\033[37m";    // White
        case LogLevel::Debug: return "\033[36m";    // Cyan
        case LogLevel::Info: return "\033[32m";      // Green
        case LogLevel::Warning: return "\033[33m";   // Yellow
        case LogLevel::Error: return "\033[31m";     // Red
        case LogLevel::Critical: return "\033[35m"; // Magenta
        default: return "\033[0m";
    }
}

} // namespace nlm