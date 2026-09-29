// File logger implementation - Enhanced with rotation
#pragma once

#include "Logger.hpp"
#include <chrono>
#include <filesystem>
#include <algorithm>
#include <iomanip>
#include <sstream>

namespace nlm {

class FileLogger : public BaseLogger {
public:
    FileLogger(const std::string& filepath);
    explicit FileLogger(const std::string& filepath, LogLevel level);
    ~FileLogger() override;
    
    void doCustomLog(const LogEntry& entry) override;
    void flush() override;
    
    // Log rotation
    void setRotationMode(bool timeBased = true, const std::string& pattern = "%Y-%m-%d_%H-%M-%S");
    void setMaxFileSize(size_t maxSize);
    void setMaxFiles(size_t maxFiles);
    void rotate();
    
    // Compression
    void enableCompression(bool enable, const std::string& compressionCmd = "gzip");
    
    // Log management
    void setLogFile(const std::string& filepath);
    std::string getCurrentLogFile() const;
    
    // File operations
    void openLogFile();
    void closeLogFile();
    bool isLogFileOpen() const;
    std::string getNextLogFilename() const;
    void cleanupOldLogs();
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm