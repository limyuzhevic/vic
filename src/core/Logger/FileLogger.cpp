// File logger implementation - Enhanced with rotation
#include "FileLogger.hpp"
#include <iostream>
#include <fstream>
#include <iomanip>
#include <sstream>
#include <filesystem>
#include <algorithm>
#include <iomanip>
#include <ctime>
#include <regex>
#include <sys/stat.h>
#include <unistd.h>

namespace nlm {

struct FileLogger::Impl {
    std::string filepath;
    std::ofstream fileOut;
    std::mutex mutex;
    bool timeBasedRotation = true;
    std::string rotationPattern = "%Y-%m-%d_%H-%M-%S";
    size_t maxFileSize = 10 * 1024 * 1024; // 10 MB
    size_t maxFiles = 5;
    bool compressionEnabled = false;
    std::string compressionCmd = "gzip";
    std::string currentFilename;
    std::chrono::system_clock::time_point lastFileCheck;
};

FileLogger::FileLogger(const std::string& filepath) : BaseLogger() {
    pImpl.reset(new Impl);
    pImpl->filepath = filepath;
    pImpl->currentFilename = filepath;
    openLogFile();
}

FileLogger::FileLogger(const std::string& filepath, LogLevel level) : BaseLogger(level) {
    pImpl.reset(new Impl);
    pImpl->filepath = filepath;
    pImpl->currentFilename = filepath;
    openLogFile();
}

FileLogger::~FileLogger() {
    closeLogFile();
}

void FileLogger::doCustomLog(const LogEntry& entry) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    // Check if rotation is needed
    if (pImpl->fileOut.is_open()) {
        // Check file size
        struct stat fileStat;
        if (stat(pImpl->currentFilename.c_str(), &fileStat) == 0) {
            if (static_cast<size_t>(fileStat.st_size) >= pImpl->maxFileSize) {
                rotate();
            }
        }
        
        // Format the log entry
        std::string formatted = pImpl->formatter->format(entry);
        
        // Write to file
        pImpl->fileOut << formatted << "\n";
        pImpl->fileOut.flush();
    }
}

void FileLogger::flush() {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    if (pImpl->fileOut.is_open()) {
        pImpl->fileOut.flush();
    }
}

void FileLogger::setRotationMode(bool timeBased, const std::string& pattern) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->timeBasedRotation = timeBased;
    pImpl->rotationPattern = pattern;
}

void FileLogger::setMaxFileSize(size_t maxSize) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->maxFileSize = maxSize;
}

void FileLogger::setMaxFiles(size_t maxFiles) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->maxFiles = maxFiles;
}

void FileLogger::rotate() {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    closeLogFile();
    
    if (pImpl->timeBasedRotation) {
        // Time-based rotation
        auto now = std::chrono::system_clock::now();
        auto time = std::chrono::system_clock::to_time_t(now);
        std::stringstream ss;
        ss << std::put_time(std::localtime(&time), pImpl->rotationPattern.c_str());
        pImpl->currentFilename = pImpl->filepath + "." + ss.str();
    } else {
        // Size-based rotation
        std::regex pattern("(.*)(\.\d+)?$");
        std::smatch matches;
        if (std::regex_match(pImpl->currentFilename, matches, pattern)) {
            pImpl->currentFilename = matches[1].str() + ".1";
        }
    }
    
    openLogFile();
    cleanupOldLogs();
}

void FileLogger::enableCompression(bool enable, const std::string& compressionCmd) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->compressionEnabled = enable;
    pImpl->compressionCmd = compressionCmd;
}

void FileLogger::setLogFile(const std::string& filepath) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->filepath = filepath;
    closeLogFile();
    pImpl->currentFilename = filepath;
    openLogFile();
}

std::string FileLogger::getCurrentLogFile() const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    return pImpl->currentFilename;
}

void FileLogger::openLogFile() {
    pImpl->fileOut.open(pImpl->currentFilename, std::ios::app);
    if (!pImpl->fileOut.is_open()) {
        std::cerr << "Failed to open log file: " << pImpl->currentFilename << std::endl;
    }
}

void FileLogger::closeLogFile() {
    if (pImpl->fileOut.is_open()) {
        pImpl->fileOut.close();
    }
}

bool FileLogger::isLogFileOpen() const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    return pImpl->fileOut.is_open();
}

std::string FileLogger::getNextLogFilename() const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    return pImpl->currentFilename;
}

void FileLogger::cleanupOldLogs() {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    std::filesystem::path dir(pImpl->filepath);
    if (!std::filesystem::exists(dir)) {
        return;
    }
    
    std::vector<std::filesystem::path> logFiles;
    for (const auto& entry : std::filesystem::directory_iterator(dir)) {
        if (entry.is_regular_file() && entry.path().string() != pImpl->currentFilename) {
            logFiles.push_back(entry.path());
        }
    }
    
    // Sort by modification time (oldest first)
    std::sort(logFiles.begin(), logFiles.end(),
              [](const std::filesystem::path& a, const std::filesystem::path& b) {
                  return std::filesystem::last_write_time(a) < std::filesystem::last_write_time(b);
              });
    
    // Remove oldest files if we exceed maxFiles
    if (logFiles.size() > pImpl->maxFiles) {
        size_t filesToRemove = logFiles.size() - pImpl->maxFiles;
        for (size_t i = 0; i < filesToRemove; ++i) {
            std::filesystem::remove(logFiles[i]);
            if (pImpl->compressionEnabled) {
                std::string cmd = pImpl->compressionCmd + " " + logFiles[i].string();
                system(cmd.c_str());
            }
        }
    }
}

} // namespace nlm