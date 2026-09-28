#include "DebugLogger.hpp"
#include <iostream>
#include <fstream>
#include <iomanip>
#include <ctime>
#include <sstream>

namespace nlm {

DebugLogger::DebugLogger() = default;

DebugLogger::~DebugLogger() {
    if (fileOutput_.is_open()) {
        fileOutput_.close();
    }
}

void DebugLogger::log(LogLevel level, const std::string& message, const std::string& context) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    if (level < minLogLevel_) return;
    
    LogEntry entry(level, message, context);
    
    // Add metadata
    for (const auto& kv : currentMetadata_) {
        entry.metadata[kv.first] = kv.second;
    }
    
    // Output to console
    std::cout << entry.toString() << std::endl;
    
    // Output to file if enabled
    if (fileOutput_.is_open()) {
        fileOutput_ << entry.toString() << std::endl;
        // Flush for real-time logging
        fileOutput_.flush();
    }
    
    // Store in history
    logHistory_.push_back(entry);
    
    // Limit history size
    if (logHistory_.size() > maxHistorySize_) {
        logHistory_.erase(logHistory_.begin());
    }
    
    // Performance logging
    if (performanceLoggingEnabled_) {
        logPerformanceMetrics();
    }
}

void DebugLogger::logPerformanceMetrics() {
    // Log performance metrics every 1000 steps
    if (currentStep_ % 1000 == 0) {
        std::stringstream ss;
        ss << "Performance: Step " << currentStep_ << ", Time " << currentTime_ 
           << ", Logs: " << logHistory_.size() << ", Memory: " << getMemoryUsageMB() << "MB";
        log(LogLevel::Info, ss.str(), "PERFORMANCE");
    }
}

void DebugLogger::addLogCallback(std::function<void(const LogEntry&)> callback) {
    std::lock_guard<std::mutex> lock(mutex_);
    logCallbacks_.push_back(std::move(callback));
}

void DebugLogger::removeLogCallback(std::function<void(const LogEntry&)> callback) {
    std::lock_guard<std::mutex> lock(mutex_);
    logCallbacks_.erase(std::remove(logCallbacks_.begin(), logCallbacks_.end(), callback), logCallbacks_.end());
}

void DebugLogger::setFileRotation(size_t maxSizeMB, int maxFiles) {
    std::lock_guard<std::mutex> lock(mutex_);
    fileRotation_.maxSize = maxSizeMB * 1024 * 1024; // Convert to bytes
    fileRotation_.maxFiles = maxFiles;
    fileRotation_.currentSize = fileOutput_.is_open() ? fileOutput_.tellp() : 0;
}

void DebugLogger::checkFileRotation() {
    if (fileOutput_.is_open() && fileRotation_.maxSize > 0) {
        fileRotation_.currentSize = fileOutput_.tellp();
        if (fileRotation_.currentSize >= fileRotation_.maxSize) {
            fileOutput_.close();
            
            // Rotate files
            for (int i = fileRotation_.maxFiles - 1; i > 0; --i) {
                std::string oldPath = getLogFilePath(i);
                std::string newPath = getLogFilePath(i + 1);
                std::filesystem::rename(oldPath, newPath);
            }
            
            std::string backupPath = getLogFilePath(1);
            std::filesystem::rename(getLogFilePath(0), backupPath);
            
            // Reopen log file
            fileOutput_.open(getLogFilePath(0), std::ios::trunc);
        }
    }
}

std::string DebugLogger::getLogFilePath(int index) const {
    // Get log file path based on index
    // This would need to be configured with actual log directory
    return "nlm_log_" + std::to_string(index) + ".txt";
}

void DebugLogger::exportLogs(const std::string& filepath) {
    std::lock_guard<std::mutex> lock(mutex_);
    std::ofstream outFile(filepath);
    if (!outFile.is_open()) {
        throw std::runtime_error("Failed to open export file: " + filepath);
    }
    
    outFile << "NLM Log Export" << std::endl;
    outFile << "Generated at: " << getCurrentTimestamp() << std::endl;
    outFile << "Total log entries: " << logHistory_.size() << std::endl;
    outFile << "=" * 50 << std::endl << std::endl;
    
    for (const auto& entry : logHistory_) {
        outFile << entry.toString() << std::endl;
    }
    
    outFile.close();
}

std::string DebugLogger::getCurrentTimestamp() const {
    auto now = std::chrono::system_clock::now();
    auto in_time_t = std::chrono::system_clock::to_time_t(now);
    std::stringstream ss;
    ss << std::put_time(std::localtime(&in_time_t), "%Y-%m-%d %H:%M:%S");
    return ss.str();
}

double DebugLogger::getMemoryUsageMB() const {
#ifdef __linux__
    // Linux memory reading from /proc/self/status
    std::ifstream statusFile("/proc/self/status");
    if (statusFile.is_open()) {
        std::string line;
        while (std::getline(statusFile, line)) {
            if (line.find("VmRSS:") == 0) {
                // Format: VmRSS:   12345 kB
                size_t pos = line.find(':') + 1;
                size_t end = line.find('k', pos);
                if (pos < end) {
                    long rss_kb = std::stol(line.substr(pos, end - pos));
                    return rss_kb / 1024.0; // Convert to MB
                }
            }
        }
    }
#endif
    return 0.0; // Return 0 if cannot determine
}

DebugLogger::FileRotationInfo::FileRotationInfo() : maxSize(0), maxFiles(0), currentSize(0) {}

} // namespace nlm