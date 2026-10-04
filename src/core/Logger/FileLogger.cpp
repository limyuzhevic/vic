#include "FileLogger.hpp"
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

struct FileLogger::Impl {
    std::ofstream fileOut;
    LogLevel level = LogLevel::Info;
    std::string format = "[%level] %timestamp %message";
    std::string timestampFormat = "%Y-%m-%d %H:%M:%S";
    bool includeLocationInfo = false;
    size_t maxFileSize = 10 * 1024 * 1024; // 10MB
    int maxFiles = 5;
    std::mutex mutex;
    std::string currentFilepath;
    size_t currentFileSize = 0;
    std::chrono::system_clock::time_point lastRotation;
};

FileLogger::FileLogger(const std::string& filepath, LogLevel level) : pImpl(std::make_unique<Impl>()) {
    pImpl->level = level;
    pImpl->currentFilepath = filepath;
    pImpl->fileOut.open(filepath, std::ios::app);
    if (pImpl->fileOut.is_open()) {
        pImpl->currentFileSize = std::filesystem::file_size(filepath);
    }
}

FileLogger::~FileLogger() {
    if (pImpl->fileOut.is_open()) {
        pImpl->fileOut.close();
    }
}

void FileLogger::log(const LogEntry& entry) {
    if (entry.level < pImpl->level) {
        return;
    }
    
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    // Check if file rotation is needed
    if (shouldRotateFile()) {
        rotateFile();
    }
    
    std::string formattedMessage = formatEntry(entry);
    
    pImpl->fileOut << formattedMessage << "\n";
    pImpl->fileOut.flush();
    pImpl->currentFileSize += formattedMessage.length() + 1; // +1 for newline
}

void FileLogger::setLevel(LogLevel level) {
    pImpl->level = level;
}

LogLevel FileLogger::getLevel() const {
    return pImpl->level;
}

void FileLogger::setFormat(const std::string& format) {
    pImpl->format = format;
}

void FileLogger::setTimestampFormat(const std::string& format) {
    pImpl->timestampFormat = format;
}

void FileLogger::setIncludeLocationInfo(bool include) {
    pImpl->includeLocationInfo = include;
}

void FileLogger::setMaxFileSize(size_t maxSize) {
    pImpl->maxFileSize = maxSize;
}

void FileLogger::setMaxFiles(int maxFiles) {
    pImpl->maxFiles = maxFiles;
}

std::string FileLogger::formatEntry(const LogEntry& entry) const {
    std::ostringstream oss;
    
    std::string message = entry.message;
    
    // Replace placeholders in format string
    size_t pos = 0;
    while ((pos = pImpl->format.find("%level", pos)) != std::string::npos) {
        oss << levelToString(entry.level);
        pos += 7;
    }
    
    pos = 0;
    while ((pos = pImpl->format.find("%timestamp", pos)) != std::string::npos) {
        oss << formatTimestamp(entry.timestamp);
        pos += 10;
    }
    
    pos = 0;
    while ((pos = pImpl->format.find("%message", pos)) != std::string::npos) {
        oss << message;
        pos += 8;
    }
    
    if (pImpl->includeLocationInfo) {
        oss << " [" << entry.file << ":" << entry.line << "] ";
        if (!entry.function.empty()) {
            oss << "(" << entry.function << ")";
        }
    }
    
    return oss.str();
}

std::string FileLogger::formatTimestamp(const std::chrono::system_clock::time_point& timestamp) const {
    auto time = std::chrono::system_clock::to_time_t(timestamp);
    std::tm tm_time = *std::localtime(&time);
    
    std::ostringstream oss;
    oss << std::put_time(&tm_time, pImpl->timestampFormat.c_str());
    
    return oss.str();
}

bool FileLogger::shouldRotateFile() const {
    if (pImpl->currentFileSize >= pImpl->maxFileSize) {
        return true;
    }
    
    auto now = std::chrono::system_clock::now();
    auto elapsed = std::chrono::duration_cast<std::chrono::hours>(now - pImpl->lastRotation);
    
    // Rotate if file is older than 24 hours
    return elapsed.count() >= 24;
}

void FileLogger::rotateFile() {
    if (!pImpl->fileOut.is_open()) {
        return;
    }
    
    pImpl->fileOut.close();
    
    // Close old file
    std::string oldPath = pImpl->currentFilepath;
    
    // Find next available filename
    std::string baseName = oldPath;
    size_t dotPos = oldPath.find_last_of('.');
    if (dotPos != std::string::npos) {
        baseName = oldPath.substr(0, dotPos);
    }
    
    // Find next available index
    int nextIndex = 1;
    std::string newPath = oldPath;
    
    while (std::filesystem::exists(newPath) && nextIndex < pImpl->maxFiles) {
        newPath = baseName + "_" + std::to_string(nextIndex) + 
                  (dotPos != std::string::npos ? oldPath.substr(dotPos) : "");
        nextIndex++;
    }
    
    if (nextIndex >= pImpl->maxFiles) {
        // Too many files, delete the oldest one
        std::vector<std::pair<std::string, time_t>> files;
        
        for (int i = 1; i < pImpl->maxFiles; ++i) {
            std::string filepath = baseName + "_" + std::to_string(i) +
                                  (dotPos != std::string::npos ? oldPath.substr(dotPos) : "");
            if (std::filesystem::exists(filepath)) {
                auto ftime = std::filesystem::last_write_time(filepath);
                files.push_back({filepath, std::chrono::system_clock::to_time_t(ftime)});
            }
        }
        
        if (!files.empty()) {
            // Find oldest file
            auto oldest = std::min_element(files.begin(), files.end(),
                [](const auto& a, const auto& b) { return a.second < b.second; });
            
            std::filesystem::remove(oldest->first);
        }
    }
    
    // Rename current file
    if (oldPath != newPath) {
        std::filesystem::rename(oldPath, newPath);
    }
    
    // Reopen file
    pImpl->fileOut.open(oldPath, std::ios::app);
    pImpl->currentFilepath = oldPath;
    pImpl->currentFileSize = 0;
    pImpl->lastRotation = std::chrono::system_clock::now();
}

} // namespace nlm
