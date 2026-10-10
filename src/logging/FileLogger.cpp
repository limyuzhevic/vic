#include "FileLogger.hpp"
#include "LogEntry.hpp"
#include <iostream>
#include <iomanip>
#include <sstream>
#include <chrono>
#include <thread>
#include <unordered_map>
#include <functional>
#include <vector>
#include <algorithm>
#include <ctime>
#include <zlib.h>
#include <fstream>
#include <filesystem>
#include <ctime>

namespace nlm {

struct FileLogger::Impl {
    std::string basePath;
    std::mutex fileMutex;
    std::unordered_map<std::string, std::ofstream> activeFiles;
    LogLevel level = LogLevel::INFO;
    size_t maxFileSize = 10 * 1024 * 1024; // 10MB
    std::chrono::hours maxFileAge = std::chrono::hours(24);
    std::string rotationStrategy = "size_time"; // "size", "time", "size_time"
    bool compressionEnabled = true;
    size_t currentFileSize = 0;
    std::chrono::system_clock::time_point currentFileStart;
    std::string currentFileName;
    PerformanceTracker performanceTracker;
};

FileLogger::FileLogger(const std::string& basePath, size_t maxFileSize, 
                       std::chrono::hours maxFileAge) : pImpl(std::make_unique<Impl>()) {
    this->pImpl->basePath = basePath;
    this->pImpl->maxFileSize = maxFileSize;
    this->pImpl->maxFileAge = maxFileAge;
    
    std::filesystem::create_directories(basePath);
    auto now = std::chrono::system_clock::now();
    auto time = std::chrono::system_clock::to_time_t(now);
    char buffer[20];
    std::strftime(buffer, sizeof(buffer), "%Y%m%d_%H%M%S", std::localtime(&time));
    pImpl->currentFileName = std::string(buffer) + ".log";
    pImpl->currentFileStart = now;
}

FileLogger::~FileLogger() {
    flush();
}

void FileLogger::log(const LogEntry& entry) {
    if (!isEnabled(entry.level)) {
        return;
    }
    
    auto startTime = std::chrono::steady_clock::now();
    
    // Check if we need to rotate file
    {
        std::lock_guard<std::mutex> lock(pImpl->fileMutex);
        
        auto now = std::chrono::system_clock::now();
        
        // Check size-based rotation
        if (pImpl->rotationStrategy.find("size") != std::string::npos && 
            pImpl->currentFileSize >= pImpl->maxFileSize) {
            pImpl->activeFiles[pImpl->currentFileName].close();
            pImpl->currentFileName = "";
            pImpl->currentFileSize = 0;
        }
        
        // Check time-based rotation
        if (pImpl->rotationStrategy.find("time") != std::string::npos) {
            auto fileAge = std::chrono::duration_cast<std::chrono::hours>(now - pImpl->currentFileStart);
            if (fileAge > pImpl->maxFileAge) {
                pImpl->activeFiles[pImpl->currentFileName].close();
                pImpl->currentFileName = "";
                pImpl->currentFileSize = 0;
            }
        }
        
        // Create new file if needed
        if (pImpl->currentFileName.empty()) {
            auto time = std::chrono::system_clock::to_time_t(now);
            char buffer[20];
            std::strftime(buffer, sizeof(buffer), "%Y%m%d_%H%M%S", std::localtime(&time));
            pImpl->currentFileName = std::string(buffer) + ".log";
            pImpl->currentFileStart = now;
            
            auto filePath = pImpl->basePath + "/" + pImpl->currentFileName;
            pImpl->activeFiles[pImpl->currentFileName].open(filePath, std::ios::app);
            if (!pImpl->activeFiles[pImpl->currentFileName].is_open()) {
                throw LogFileError("Failed to open log file: " + filePath);
            }
        }
        
        // Write to file
        std::ostringstream oss;
        oss << "[" << levelToString(entry.level) << "] ";
        
        auto time = std::chrono::system_clock::to_time_t(entry.timestamp);
        oss << std::put_time(std::localtime(&time), "%Y-%m-%d %H:%M:%S");
        
        oss << " " << entry.message;
        
        if (!entry.metadata.empty()) {
            oss << " [";
            bool first = true;
            for (const auto& pair : entry.metadata) {
                if (!first) oss << ", ";
                oss << pair.first << "=" << pair.second;
                first = false;
            }
            oss << "]";
        }
        
        std::string output = oss.str();
        pImpl->activeFiles[pImpl->currentFileName] << output << "\n";
        pImpl->currentFileSize += output.size() + 1; // +1 for newline
        
        // Compress if enabled
        if (pImpl->compressionEnabled && pImpl->currentFileSize >= pImpl->maxFileSize * 2) {
            compressFile(pImpl->currentFileName);
        }
    }
    
    auto endTime = std::chrono::steady_clock::now();
    pImpl->performanceTracker.recordOperation(endTime - startTime);
}

void FileLogger::flush() {
    std::lock_guard<std::mutex> lock(pImpl->fileMutex);
    for (auto& pair : pImpl->activeFiles) {
        pair.second.flush();
    }
}

void FileLogger::setLevel(LogLevel level) {
    std::lock_guard<std::mutex> lock(pImpl->fileMutex);
    pImpl->level = level;
}

LogLevel FileLogger::getLevel() const {
    return pImpl->level;
}

bool FileLogger::isEnabled(LogLevel level) const {
    return level >= pImpl->level;
}

void FileLogger::setRotationStrategy(const std::string& strategy) {
    std::lock_guard<std::mutex> lock(pImpl->fileMutex);
    pImpl->rotationStrategy = strategy;
}

void FileLogger::setCompression(bool enabled) {
    std::lock_guard<std::mutex> lock(pImpl->fileMutex);
    pImpl->compressionEnabled = enabled;
}

void FileLogger::setMaxFileSize(size_t maxSize) {
    std::lock_guard<std::mutex> lock(pImpl->fileMutex);
    pImpl->maxFileSize = maxSize;
}

void FileLogger::setMaxFileAge(std::chrono::hours maxAge) {
    std::lock_guard<std::mutex> lock(pImpl->fileMutex);
    pImpl->maxFileAge = maxAge;
}

} // namespace nlm