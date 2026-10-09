#pragma once

#include <string>
#include <chrono>
#include <memory>
#include <map>
#include <vector>
#include <algorithm>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <filesystem>
#include <nlohmann/json.hpp>
#include "Enhanced/LogFormatter.hpp"
#include "Enhanced/Analysis.hpp"

namespace nlm {

// Log file metadata
struct LogFileInfo {
    std::string path;
    std::string name;
    uint64_t size;
    std::chrono::system_clock::time_point creationTime;
    std::chrono::system_clock::time_point modificationTime;
    size_t lineCount;
    std::string format;
    std::map<std::string, std::string> metadata;
};

// Log rotation policy
class RotationPolicy {
public:
    virtual ~RotationPolicy() = default;
    virtual bool shouldRotate(const LogFileInfo& fileInfo) const = 0;
    virtual std::string getRotatedFilename(const std::string& originalPath, 
                                          const std::chrono::system_clock::time_point& time) const = 0;
};

// Size-based rotation policy
class SizeRotationPolicy : public RotationPolicy {
public:
    explicit SizeRotationPolicy(size_t maxSize) : maxSize_(maxSize) {}
    
    bool shouldRotate(const LogFileInfo& fileInfo) const override {
        return fileInfo.size >= maxSize_;
    }
    
    std::string getRotatedFilename(const std::string& originalPath,
                                   const std::chrono::system_clock::time_point& time) const override {
        auto time_t = std::chrono::system_clock::to_time_t(time);
        char timeBuf[100];
        strftime(timeBuf, sizeof(timeBuf), "%Y%m%d_%H%M%S", std::localtime(&time_t));
        
        std::string::size_type pos = originalPath.find_last_of('.');
        if (pos != std::string::npos) {
            return originalPath.substr(0, pos) + "_" + timeBuf + 
                   originalPath.substr(pos);
        }
        return originalPath + "_" + timeBuf;
    }
    
private:
    size_t maxSize_;
};

// Time-based rotation policy
class TimeRotationPolicy : public RotationPolicy {
public:
    explicit TimeRotationPolicy(const std::string& schedule) : schedule_(schedule) {}
    
    bool shouldRotate(const LogFileInfo& fileInfo) const override {
        auto now = std::chrono::system_clock::now();
        auto today = std::chrono::system_clock::time_point(
            std::chrono::duration_cast<std::chrono::seconds>(
                std::chrono::duration_cast<std::chrono::days>(now).count() *
                std::chrono::duration_cast<std::chrono::days>(std::chrono::hours(24)).count()
            )
        );
        return fileInfo.modificationTime < today;
    }
    
    std::string getRotatedFilename(const std::string& originalPath,
                                   const std::chrono::system_clock::time_point& time) const override {
        // Daily rotation
        auto time_t = std::chrono::system_clock::to_time_t(time);
        char dateBuf[11];
        strftime(dateBuf, sizeof(dateBuf), "%Y%m%d", std::localtime(&time_t));
        
        std::string::size_type pos = originalPath.find_last_of('.');
        if (pos != std::string::npos) {
            return originalPath.substr(0, pos) + "_" + dateBuf +
                   originalPath.substr(pos);
        }
        return originalPath + "_" + dateBuf;
    }
    
private:
    std::string schedule_;
};

// Log manager for rotation, search, export/import, and monitoring
class LogManager {
public:
    LogManager(const std::string& logDirectory = "logs") 
        : logDirectory_(logDirectory), maxLogFiles_(10) {
        if (!std::filesystem::exists(logDirectory)) {
            std::filesystem::create_directories(logDirectory);
        }
    }
    
    // Log rotation
    void rotateLogs(const RotationPolicy& policy) {
        std::lock_guard<std::mutex> lock(rotationMutex_);
        
        for (const auto& pattern : logFilePatterns_) {
            std::vector<LogFileInfo> files = getLogFiles(pattern);
            for (const auto& fileInfo : files) {
                if (policy.shouldRotate(fileInfo)) {
                    rotateFile(fileInfo, policy);
                }
            }
        }
    }
    
    // Add log file pattern
    void addLogFilePattern(const std::string& pattern) {
        std::lock_guard<std::mutex> lock(patternMutex_);
        logFilePatterns_.push_back(pattern);
    }
    
    // Log search functionality
    std::vector<EnhancedLogEntry> searchLogs(
        const std::string& pattern,
        const std::string& filePattern = "",
        LogLevel minLevel = LogLevel::Debug,
        const std::chrono::system_clock::time_point& startTime = std::chrono::system_clock::time_point(),
        const std::chrono::system_clock::time_point& endTime = std::chrono::system_clock::time_point()) const {
        
        std::vector<EnhancedLogEntry> results;
        
        // Search through log files
        for (const auto& fileInfo : getAllLogFiles()) {
            if (!filePattern.empty() && fileInfo.name.find(filePattern) == std::string::npos) {
                continue;
            }
            
            std::vector<EnhancedLogEntry> fileLogs = parseLogFile(fileInfo.path);
            
            for (const auto& entry : fileLogs) {
                if (entry.level < minLevel) continue;
                if (entry.timestamp < startTime || entry.timestamp > endTime) continue;
                if (pattern.empty() || entry.message.find(pattern) != std::string::npos) {
                    results.push_back(entry);
                }
            }
        }
        
        return results;
    }
    
    // Log export functionality
    bool exportLogs(const std::string& filename, 
                   const std::vector<EnhancedLogEntry>& logs,
                   const std::string& format = "json") const {
        try {
            if (format == "json") {
                return exportToJson(filename, logs);
            } else if (format == "csv") {
                return exportToCsv(filename, logs);
            } else if (format == "xml") {
                return exportToXml(filename, logs);
            } else {
                return exportToText(filename, logs);
            }
        } catch (const std::exception& e) {
            return false;
        }
    }
    
    // Log import functionality
    std::vector<EnhancedLogEntry> importLogs(const std::string& filename, 
                                            const std::string& format = "json") {
        std::vector<EnhancedLogEntry> logs;
        
        try {
            if (format == "json") {
                logs = importFromJson(filename);
            } else if (format == "csv") {
                logs = importFromCsv(filename);
            } else if (format == "xml") {
                logs = importFromXml(filename);
            } else {
                logs = importFromText(filename);
            }
        } catch (const std::exception& e) {
            // Return empty vector on error
        }
        
        return logs;
    }
    
    // Log visualization
    std::string generateLogVisualization(const std::vector<EnhancedLogEntry>& logs, 
                                         const std::string& type = "text") const {
        if (type == "json") {
            return generateJsonVisualization(logs);
        } else if (type == "html") {
            return generateHtmlVisualization(logs);
        } else if (type == "chart") {
            return generateChartVisualization(logs);
        } else {
            return generateTextVisualization(logs);
        }
    }
    
    // Log monitoring and alerting
    void startMonitoring(std::function<void(const std::string&)> alertCallback) {
        monitoring_ = true;
        monitoringThread_ = std::thread(&LogManager::monitoringThread, this, alertCallback);
    }
    
    void stopMonitoring() {
        monitoring_ = false;
        if (monitoringThread_.joinable()) {
            monitoringThread_.join();
        }
    }
    
    // Get log file information
    std::vector<LogFileInfo> getLogFiles(const std::string& pattern = "") const {
        std::lock_guard<std::mutex> lock(fileMutex_);
        std::vector<LogFileInfo> result;
        
        for (const auto& fileInfo : logFiles_) {
            if (pattern.empty() || fileInfo.name.find(pattern) != std::string::npos) {
                result.push_back(fileInfo);
            }
        }
        
        return result;
    }
    
    // Clean up old log files
    void cleanupOldLogs() {
        std::lock_guard<std::mutex> lock(fileMutex_);
        
        auto now = std::chrono::system_clock::now();
        for (auto it = logFiles_.begin(); it != logFiles_.end(); ) {
            auto age = std::chrono::duration_cast<std::chrono::hours>(now - it->modificationTime);
            if (age.count() > 24 * 7) { // Delete logs older than 7 days
                std::filesystem::remove(it->path);
                it = logFiles_.erase(it);
            } else {
                ++it;
            }
        }
    }
    
private:
    void rotateFile(const LogFileInfo& fileInfo, const RotationPolicy& policy) {
        std::string rotatedPath = policy.getRotatedFilename(fileInfo.path, 
                                                           std::chrono::system_clock::now());
        
        // Rename the file
        std::filesystem::rename(fileInfo.path, rotatedPath);
        
        // Update log files list
        for (auto& fileInfo : logFiles_) {
            if (fileInfo.path == fileInfo.path) {
                fileInfo.path = rotatedPath;
                fileInfo.name = std::filesystem::path(rotatedPath).filename().string();
                fileInfo.modificationTime = std::chrono::system_clock::now();
                break;
            }
        }
        
        // Limit number of log files
        cleanupOldLogs();
    }
    
    std::vector<LogFileInfo> getAllLogFiles() const {
        std::lock_guard<std::mutex> lock(fileMutex_);
        return logFiles_;
    }
    
    std::vector<EnhancedLogEntry> parseLogFile(const std::string& filepath) const {
        std::vector<EnhancedLogEntry> logs;
        
        // This is a simplified implementation
        // In practice, you would need to handle different log formats
        std::ifstream file(filepath);
        if (!file) return logs;
        
        std::string line;
        while (std::getline(file, line)) {
            // Parse log line into EnhancedLogEntry
            // This is a simplified example - actual parsing would be more complex
            EnhancedLogEntry entry;
            // ... parse logic ...
            
            logs.push_back(entry);
        }
        
        return logs;
    }
    
    bool exportToJson(const std::string& filename, const std::vector<EnhancedLogEntry>& logs) const {
        nlohmann::json jsonArray;
        
        for (const auto& entry : logs) {
            nlohmann::json jsonEntry;
            jsonEntry["level"] = static_cast<int>(entry.level);
            jsonEntry["message"] = entry.message;
            jsonEntry["file"] = entry.file;
            jsonEntry["line"] = entry.line;
            jsonEntry["function"] = entry.function;
            jsonEntry["timestamp"] = std::chrono::system_clock::to_time_t(entry.timestamp);
            jsonEntry["thread_id"] = entry.threadId;
            jsonEntry["sequence"] = entry.sequenceNumber;
            
            nlohmann::json tagsJson;
            for (const auto& tag : entry.tags) {
                tagsJson[tag.first] = tag.second;
            }
            jsonEntry["tags"] = tagsJson;
            
            nlohmann::json metadataJson;
            for (const auto& meta : entry.metadata) {
                metadataJson[meta.first] = meta.second;
            }
            jsonEntry["metadata"] = metadataJson;
            
            jsonArray.push_back(jsonEntry);
        }
        
        std::ofstream file(filename);
        if (!file) return false;
        
        file << jsonArray.dump(2);
        return true;
    }
    
    std::vector<EnhancedLogEntry> importFromJson(const std::string& filename) {
        std::vector<EnhancedLogEntry> logs;
        
        std::ifstream file(filename);
        if (!file) return logs;
        
        nlohmann::json jsonArray;
        file >> jsonArray;
        
        for (const auto& jsonEntry : jsonArray) {
            EnhancedLogEntry entry;
            entry.level = static_cast<LogLevel>(jsonEntry["level"].get<int>());
            entry.message = jsonEntry["message"].get<std::string>();
            entry.file = jsonEntry["file"].get<std::string>();
            entry.line = jsonEntry["line"].get<int>();
            entry.function = jsonEntry["function"].get<std::string>();
            entry.timestamp = std::chrono::system_clock::time_point(
                std::chrono::seconds(jsonEntry["timestamp"].get<time_t>()));
            entry.threadId = jsonEntry["thread_id"].get<std::string>();
            entry.sequenceNumber = jsonEntry["sequence"].get<uint64_t>();
            
            const auto& tagsJson = jsonEntry["tags"];
            for (const auto& item : tagsJson.items()) {
                entry.tags[item.key()] = item.value().get<std::string>();
            }
            
            const auto& metadataJson = jsonEntry["metadata"];
            for (const auto& item : metadataJson.items()) {
                entry.metadata[item.key()] = item.value().get<std::string>();
            }
            
            logs.push_back(entry);
        }
        
        return logs;
    }
    
    // ... Additional export/import implementations would go here ...
    
    void monitoringThread(std::function<void(const std::string&)> alertCallback) {
        while (monitoring_) {
            std::this_thread::sleep_for(std::chrono::minutes(5));
            
            // Check for log issues
            std::string report = "Log monitoring report: " + 
                                std::to_string(getAllLogFiles().size()) + " log files, " +
                                std::to_string(maxLogFiles_) + " max files";
            
            alertCallback(report);
        }
    }
    
    // ... Additional visualization methods ...
    
    std::string generateTextVisualization(const std::vector<EnhancedLogEntry>& logs) const {
        std::stringstream visualization;
        visualization << "Log Visualization Report\n";
        visualization << "=======================" << std::endl;
        
        size_t totalLogs = logs.size();
        if (totalLogs == 0) return visualization.str();
        
        visualization << "Total logs: " << totalLogs << std::endl;
        
        // Count by level
        std::map<LogLevel, size_t> levelCounts;
        for (const auto& entry : logs) {
            levelCounts[entry.level]++;
        }
        
        visualization << "\nLog counts by level:" << std::endl;
        for (const auto& pair : levelCounts) {
            visualization << "  " << levelToString(pair.first) << ": " << pair.second << std::endl;
        }
        
        return visualization.str();
    }
    
    // ... Additional visualization methods ...
    
    std::string logDirectory_;
    size_t maxLogFiles_;
    
    std::vector<std::string> logFilePatterns_;
    std::vector<LogFileInfo> logFiles_;
    
    mutable std::mutex fileMutex_;
    mutable std::mutex patternMutex_;
    mutable std::mutex rotationMutex_;
    
    bool monitoring_;
    std::thread monitoringThread_;
};

} // namespace nlm
