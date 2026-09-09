#include "FileLogger.hpp"
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
#include <sstream>
#include <zlib.h>
#include <bzlib.h>
#include <lzma.h>
#include <zip.h>

namespace nlm {

struct FileLogger::Impl {
    LogLevel level{LogLevel::Info};
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
    bool enableRotation{true};
    size_t maxFileSize{10 * 1024 * 1024}; // 10MB
    size_t maxFiles{5};
    
    // Compression settings
    bool enableCompression{false};
    int compressionLevel{6};
    bool asyncCompression{false};
    
    // Memory mapping
    bool enableMemoryMapping{false};
    
    // Statistics
    LoggerStats stats;
    std::string customPattern;
    
    // Thread safety
    mutable std::shared_mutex categoryMutex;
    std::map<std::string, bool> categoryFilters;
    
    // Output formatting
    std::function<std::string(const LogEntry&)> formatter;
    
    // File writing optimization
    std::vector<char> writeBuffer;
    size_t writeBufferSize{64 * 1024}; // 64KB
};

FileLogger::FileLogger() : pImpl(std::make_unique<Impl>()) {
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
            pImpl->workerThreads.emplace_back(&FileLogger::processAsyncLogs, this);
        }
    }
}

FileLogger::FileLogger(LogLevel level) : FileLogger() {
    pImpl->level = level;
}

FileLogger::FileLogger(const std::string& filepath, LogLevel level) : FileLogger() {
    setFilepath(filepath);
    pImpl->level = level;
}

FileLogger::FileLogger(const LoggingConfig& config) : FileLogger() {
    setLevel(config.defaultLevel);
    
    if (config.enableFileLogging && !config.logFile.empty()) {
        setFilepath(config.logFile);
        enableLogRotation(config.enableRotation);
        setMaximumFileSize(config.maxFileSize);
        setMaximumBackupFiles(config.maxFiles);
        enableCompression(config.enableRotation); // Using compression for rotated files
    }
    
    setBufferSize(config.enableAsyncLogging ? 1024 : 0);
    setWorkerThreadCount(config.asyncThreadCount);
    enableMemoryMapping(config.enableAsyncLogging);
}

FileLogger::~FileLogger() {
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

void FileLogger::log(const LogEntry& entry) {
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
        
        // Write to file
        if (pImpl->fileOut.is_open()) {
            pImpl->fileOut << formattedOutput << "\n";
            pImpl->fileOut.flush();
            
            // Check for rotation
            if (enableMemoryMapping) {
                rotateLogs();
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

void FileLogger::setLevel(LogLevel level) {
    pImpl->level = level;
}

LogLevel FileLogger::getLevel() const {
    return pImpl->level;
}

bool FileLogger::isLevelEnabled(LogLevel level) const {
    return level >= pImpl->level;
}

void FileLogger::setCategoryFilter(const std::string& category, bool enabled) {
    std::unique_lock lock(pImpl->categoryMutex);
    pImpl->categoryFilters[category] = enabled;
}

bool FileLogger::isCategoryEnabled(const std::string& category) const {
    std::shared_lock lock(pImpl->categoryMutex);
    auto it = pImpl->categoryFilters.find(category);
    if (it != pImpl->categoryFilters.end()) {
        return it->second;
    }
    return false;
}

void FileLogger::flush() {
    if (pImpl->bufferSize > 0) {
        std::unique_lock<std::mutex> lock(pImpl->bufferMutex);
        bufferCondVar.wait(lock, [this] { return pImpl->logBuffer.empty(); });
    }
    
    if (pImpl->fileOut.is_open()) {
        pImpl->fileOut.flush();
    }
}

LoggerStats FileLogger::getStats() const {
    return pImpl->stats;
}

void FileLogger::setFormatter(std::function<std::string(const LogEntry&)> formatter) {
    pImpl->formatter = std::move(formatter);
}

void FileLogger::logAsync(const LogEntry& entry) {
    // This is handled by the worker threads
    log(entry);
}

void FileLogger::setWorkerThreadCount(size_t count) {
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
            pImpl->workerThreads.emplace_back(&FileLogger::processAsyncLogs, this);
        }
    }
}

void FileLogger::waitForCompletion() {
    flush();
}

void FileLogger::setFilepath(const std::string& filepath) {
    std::lock_guard<std::mutex> lock(pImpl->bufferMutex);
    
    if (pImpl->fileOut.is_open()) {
        pImpl->fileOut.close();
    }
    
    pImpl->filePath = filepath;
    pImpl->fileOut.open(filepath, std::ios::app);
}

void FileLogger::enableCompression(bool enable) {
    pImpl->enableCompression = enable;
}

void FileLogger::setCompressionLevel(int level) {
    pImpl->compressionLevel = std::max(0, std::min(9, level));
}

void FileLogger::setMaximumFileSize(size_t maxSize) {
    pImpl->maxFileSize = maxSize;
}

void FileLogger::setMaximumBackupFiles(size_t maxFiles) {
    pImpl->maxFiles = maxFiles;
}

void FileLogger::enableLogRotation(bool enable) {
    pImpl->enableRotation = enable;
}

void FileLogger::setCustomPattern(const std::string& pattern) {
    pImpl->customPattern = pattern;
}

void FileLogger::setBufferSize(size_t bufferSize) {
    std::lock_guard<std::mutex> lock(pImpl->bufferMutex);
    pImpl->bufferSize = bufferSize;
}

void FileLogger::enableMemoryMapping(bool enable) {
    pImpl->enableMemoryMapping = enable;
}

void FileLogger::enableAsyncCompression(bool enable) {
    pImpl->asyncCompression = enable;
}

void FileLogger::processAsyncLogs() {
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
            
            // Write to file
            if (pImpl->fileOut.is_open()) {
                pImpl->fileOut << formattedOutput << "\n";
                pImpl->fileOut.flush();
                
                // Check for rotation
                if (pImpl->enableRotation) {
                    rotateLogs();
                }
            }
            
            // Compress if needed
            if (pImpl->enableCompression && pImpl->asyncCompression) {
                compressFile(pImpl->filePath);
            }
            
            auto endTime = std::chrono::high_resolution_clock::now();
            auto duration = std::chrono::duration<double>(endTime - startTime).count();
            std::atomic_fetch_add(&pImpl->stats.totalProcessingTime, duration);
        }
    }
}

void FileLogger::compressFile(const std::string& filepath) {
    // Simplified compression - in production, would use proper compression libraries
    // For now, just note that compression would happen here
}

void FileLogger::rotateLogs() {
    if (!pImpl->enableRotation || !pImpl->fileOut.is_open()) {
        return;
    }
    
    std::error_code ec;
    std::filesystem::path filepath(pImpl->filePath);
    std::string stem = filepath.stem().string();
    std::string ext = filepath.extension().string();
    
    // Check file size
    if (pImpl->fileOut.tellp() < static_cast<std::ofstream::pos_type>(pImpl->maxFileSize)) {
        return;
    }
    
    // Close current file
    pImpl->fileOut.close();
    
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
    
    // Compress if enabled
    if (pImpl->enableCompression) {
        compressFile(rotatedFile.string());
    }
    
    // Reopen the log file
    pImpl->fileOut.open(filepath, std::ios::app);
}

std::string FileLogger::formatEntry(const LogEntry& entry) const {
    if (pImpl->formatter) {
        return pImpl->formatter(entry);
    }
    
    // Default formatting - more detailed for file logs
    std::ostringstream oss;
    oss << "[" << levelToString(entry.level) << "] ";
    
    // Add timestamp with milliseconds
    auto time = std::chrono::system_clock::now();
    auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(
        time.time_since_epoch() % std::chrono::seconds(1)).count();
    auto time_t = std::chrono::system_clock::to_time_t(time);
    oss << std::put_time(std::localtime(&time_t), "%Y-%m-%d %H:%M:%S") << "." 
        << std::setfill('0') << std::setw(3) << ms;
    
    // Add thread ID
    oss << " [tid:" << entry.threadId << "]";
    
    // Add category
    if (!entry.category.empty() && entry.category != LogCategory::All) {
        oss << " [" << entry.category << "]";
    }
    
    // Add context
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
    
    // Add file info
    if (!entry.file.empty()) {
        oss << " [" << entry.file << ":" << entry.line << "]";
        hasContext = true;
    }
    
    oss << " " << entry.message;
    
    // Add structured data
    for (const auto& kv : entry.kvPairs) {
        oss << " " << kv.first << "=" << kv.second;
    }
    
    return oss.str();
}

std::string FileLogger::levelToString(LogLevel level) const {
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

std::string FileLogger::getCurrentTimestamp() const {
    auto now = std::chrono::system_clock::now();
    auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(
        now.time_since_epoch() % std::chrono::seconds(1)).count();
    auto time_t = std::chrono::system_clock::to_time_t(now);
    
    std::ostringstream oss;
    oss << std::put_time(std::localtime(&time_t), "%Y-%m-%d %H:%M:%S") << "." 
        << std::setfill('0') << std::setw(3) << ms;
    
    return oss.str();
}

} // namespace nlm