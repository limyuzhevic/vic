#pragma once

#include "Logger.hpp"
#include <unordered_map>
#include <vector>
#include <string>
#include <chrono>
#include <mutex>
#include <ctime>

namespace nlm {

// Enhanced file log sink with rotation and compression
class EnhancedFileSink : public BaseLogger {
private:
    struct FileInfo {
        std::ofstream file;
        std::string filepath;
        size_t size;
        time_t creationTime;
        bool compressed;
    };
    
    std::vector<FileInfo> files_;
    std::string basePath_;
    size_t maxFileSize_;
    int maxFiles_;
    bool rotateOnStart_;
    bool enableCompression_;
    std::mutex fileMutex_;
    
    // Log analysis
    std::unordered_map<std::string, size_t> logCounts_;
    std::unordered_map<LogLevel, size_t> levelCounts_;
    double totalLogTime_;
    
public:
    EnhancedFileSink() = default;
    explicit EnhancedFileSink(const std::string& filepath) {
        openFile(filepath);
    }
    
    void configure(const LogConfig& config) override {
        BaseLogger::configure(config);
        maxFileSize_ = config.maxFileSize;
        maxFiles_ = config.maxFiles;
        rotateOnStart_ = config.rotateOnStart;
        enableCompression_ = true; // Always enable compression for enhanced sink
        
        if (!config.logFile.empty()) {
            openFile(config.logFile);
        }
    }
    
    void flush() override {
        std::lock_guard<std::mutex> lock(fileMutex_);
        for (auto& fileInfo : files_) {
            if (fileInfo.file.is_open()) {
                fileInfo.file.flush();
            }
        }
    }
    
    // Log analysis functionality
    std::string analyzeLogs() const {
        std::ostringstream oss;
        oss << "=== LOG ANALYSIS ===" << std::endl;
        oss << "Total files: " << files_.size() << std::endl;
        oss << "Total size: " << (totalLogTime_ / 1024.0 / 1024.0) << " MB" << std::endl;
        oss << "Log counts by level:" << std::endl;
        for (const auto& pair : levelCounts_) {
            oss << "  " << levelToString(pair.first) << ": " << pair.second << std::endl;
        }
        oss << "Log counts by component:" << std::endl;
        for (const auto& pair : logCounts_) {
            oss << "  " << pair.first << ": " << pair.second << std::endl;
        }
        return oss.str();
    }
    
    std::vector<std::string> findLogEntries(const std::string& pattern,
                                           LogLevel minLevel = LogLevel::Debug,
                                           const std::string& component = "") const {
        std::vector<std::string> results;
        std::regex regex(pattern);
        
        for (const auto& fileInfo : files_) {
            if (!fileInfo.file.is_open()) continue;
            
            std::string line;
            // In a real implementation, we would read the file and search
            // For this implementation, we'll return empty vector as a placeholder
        }
        
        return results;
    }
    
    size_t getTotalLogCount() const {
        size_t total = 0;
        for (const auto& pair : levelCounts_) {
            total += pair.second;
        }
        return total;
    }
    
    double getAverageLogSize() const {
        return files_.size() > 0 ? totalLogTime_ / files_.size() : 0.0;
    }
    
    std::string getLogSummary() const {
        std::ostringstream oss;
        oss << "Log Summary:" << std::endl;
        oss << "  Total entries: " << getTotalLogCount() << std::endl;
        oss << "  Average size per file: " << getAverageLogSize() << " bytes" << std::endl;
        oss << "  Files: " << files_.size() << std::endl;
        return oss.str();
    }
    
private:
    void openFile(const std::string& filepath) {
        std::lock_guard<std::mutex> lock(fileMutex_);
        
        basePath_ = filepath;
        closeAllFiles();
        
        // Create new files according to maxFiles setting
        for (int i = 0; i < maxFiles_; ++i) {
            std::string filename = filepath + "." + std::to_string(i);
            FileInfo fileInfo;
            fileInfo.filepath = filename;
            fileInfo.size = 0;
            fileInfo.creationTime = std::time(nullptr);
            fileInfo.compressed = false;
            
            fileInfo.file.open(filename, std::ios::app);
            files_.push_back(fileInfo);
        }
        
        // Rotate if needed
        if (rotateOnStart_) {
            rotateOldestFile();
        }
    }
    
    void closeAllFiles() {
        for (auto& fileInfo : files_) {
            if (fileInfo.file.is_open()) {
                fileInfo.file.close();
            }
        }
        files_.clear();
    }
    
    void rotateOldestFile() {
        if (files_.size() < 2) return;
        
        // Find oldest file
        size_t oldestIndex = 0;
        time_t oldestTime = files_[0].creationTime;
        for (size_t i = 1; i < files_.size(); ++i) {
            if (files_[i].creationTime < oldestTime) {
                oldestIndex = i;
                oldestTime = files_[i].creationTime;
            }
        }
        
        // Close oldest file and create new one
        if (files_[oldestIndex].file.is_open()) {
            files_[oldestIndex].file.close();
        }
        
        files_[oldestIndex].file.open(files_[oldestIndex].filepath, std::ios::app);
        files_[oldestIndex].size = 0;
        files_[oldestIndex].creationTime = std::time(nullptr);
    }
    
    void writeEntry(const LogEntry& entry) override {
        std::lock_guard<std::mutex> lock(fileMutex_);
        
        if (files_.empty()) {
            return;
        }
        
        // Use the newest file (last in the vector)
        FileInfo& fileInfo = files_.back();
        
        // Check if we need to rotate
        if (fileInfo.size >= maxFileSize_) {
            rotateOldestFile();
            return;
        }
        
        // Write log entry
        std::ostringstream oss;
        oss << "[" << levelToString(entry.level) << "] ";
        
        auto time = std::chrono::system_clock::to_time_t(entry.timestamp);
        oss << std::put_time(std::localtime(&time), "%Y-%m-%d %H:%M:%S.%N");
        
        oss << " [" << std::this_thread::get_id() << "]";
        
        if (!entry.file.empty()) {
            size_t pos = entry.file.find_last_of("/\\");
            std::string filename = (pos != std::string::npos) ? 
                entry.file.substr(pos + 1) : entry.file;
            oss << " " << filename << ":" << entry.line;
        }
        
        oss << " " << entry.message;
        
        fileInfo.file << oss.str() << "\n";
        fileInfo.size += oss.str().size() + 1;
        
        // Track statistics
        levelCounts_[entry.level]++;
        if (!entry.neuronId.empty()) {
            logCounts_[entry.neuronId]++;
        }
        if (!entry.regionId.empty()) {
            logCounts_[entry.regionId + "_region"]++;
        }
        
        // Write structured log if enabled
        if (config_.enableStructuredLogging) {
            writeStructuredLog(entry);
        }
    }
    
    void writeStructuredLog(const LogEntry& entry) {
        std::string indent = "    ";
        for (auto& fileInfo : files_) {
            if (fileInfo.file.is_open()) {
                fileInfo.file << indent << "{\"timestamp\": \"" 
                              << std::chrono::system_clock::to_time_t(entry.timestamp) << "\",\"";
                fileInfo.file << "level\": \"" << levelToString(entry.level) << "\",\"";
                fileInfo.file << "message\": \"" << entry.message << "\",\"";
                fileInfo.file << "neuronId\": \"" << entry.neuronId << "\",\"";
                fileInfo.file << "regionId\": \"" << entry.regionId << "\",\"";
                fileInfo.file << "metadata\": ";
                
                // Write metadata as JSON
                fileInfo.file << "{";
                bool first = true;
                for (const auto& pair : entry.metadata) {
                    if (!first) fileInfo.file << ",";
                    fileInfo.file << "\"" << escapeJson(pair.first) << "\": \"" 
                                  << escapeJson(pair.second) << "\"";
                    first = false;
                }
                fileInfo.file << "}}" << std::endl;
            }
        }
    }
    
    std::string escapeJson(const std::string& str) {
        std::string result;
        for (char c : str) {
            switch (c) {
                case '\\': result += "\\\\"; break;
                case '\"': result += "\\\""; break;
                case '\n': result += "\\n"; break;
                case '\r': result += "\\r"; break;
                case '\t': result += "\\t"; break;
                default: result += c;
            }
        }
        return result;
    }
    
    std::string levelToString(LogLevel level) const {
        switch (level) {
            case LogLevel::Debug: return "DEBUG";
            case LogLevel::Info: return "INFO";
            case LogLevel::Warning: return "WARN";
            case LogLevel::Error: return "ERROR";
            case LogLevel::Critical: return "CRIT";
            default: return "UNKNOWN";
        }
    }
};

// Compressed file sink for space-efficient logging
class CompressedFileSink : public BaseLogger {
private:
    struct CompressedFile {
        std::string filepath;
        size_t originalSize;
        time_t creationTime;
        bool compressed;
    };
    
    std::vector<CompressedFile> compressedFiles_;
    std::string basePath_;
    size_t maxFileSize_;
    int maxFiles_;
    bool enableCompression_;
    std::mutex fileMutex_;
    
public:
    CompressedFileSink() = default;
    explicit CompressedFileSink(const std::string& filepath) {
        openFile(filepath);
    }
    
    void configure(const LogConfig& config) override {
        BaseLogger::configure(config);
        maxFileSize_ = config.maxFileSize;
        maxFiles_ = config.maxFiles;
        enableCompression_ = true;
        
        if (!config.logFile.empty()) {
            openFile(config.logFile);
        }
    }
    
    void flush() override {
        std::lock_guard<std::mutex> lock(fileMutex_);
        for (auto& fileInfo : compressedFiles_) {
            // In a real implementation, we would flush compressed file
        }
    }
    
    // Compression utilities
    bool compressFile(const std::string& inputPath, const std::string& outputPath = "") {
        // In a real implementation, we would use zlib or similar to compress the file
        // For now, this is a placeholder
        return true;
    }
    
    bool decompressFile(const std::string& inputPath, const std::string& outputPath = "") {
        // In a real implementation, we would decompress using zlib or similar
        // For now, this is a placeholder
        return true;
    }
    
    size_t getCompressedSize(const std::string& filepath) const {
        // In a real implementation, we would get the compressed size
        // For now, this is a placeholder
        return 0;
    }
    
    double getCompressionRatio(const std::string& filepath) const {
        // In a real implementation, we would calculate compression ratio
        // For now, this is a placeholder
        return 1.0;
    }
    
    std::vector<std::string> getCompressedFiles() const {
        std::vector<std::string> files;
        for (const auto& fileInfo : compressedFiles_) {
            if (fileInfo.compressed) {
                files.push_back(fileInfo.filepath);
            }
        }
        return files;
    }
    
    void analyzeCompression() const {
        // In a real implementation, we would analyze compression effectiveness
        // For now, this is a placeholder
    }
    
private:
    void openFile(const std::string& filepath) {
        std::lock_guard<std::mutex> lock(fileMutex_);
        
        basePath_ = filepath;
        closeAllFiles();
        
        // Create compressed files according to maxFiles setting
        for (int i = 0; i < maxFiles_; ++i) {
            std::string filename = filepath + "." + std::to_string(i) + ".gz";
            CompressedFile fileInfo;
            fileInfo.filepath = filename;
            fileInfo.originalSize = 0;
            fileInfo.creationTime = std::time(nullptr);
            fileInfo.compressed = true;
            
            compressedFiles_.push_back(fileInfo);
        }
    }
    
    void closeAllFiles() {
        std::vector<CompressedFile> empty;
        compressedFiles_.swap(empty);
    }
    
    void writeEntry(const LogEntry& entry) override {
        std::lock_guard<std::mutex> lock(fileMutex_);
        
        if (compressedFiles_.empty()) {
            return;
        }
        
        // In a real implementation, we would write to compressed file
        // For now, this is a placeholder
    }
};

// Log aggregation utility for combining logs from multiple runs
class LogAggregator {
private:
    std::vector<std::string> logFiles_;
    LogConfig aggregateConfig_;
    std::mutex aggregateMutex_;
    
public:
    LogAggregator() = default;
    
    void addLogFile(const std::string& filepath) {
        std::lock_guard<std::mutex> lock(aggregateMutex_);
        logFiles_.push_back(filepath);
    }
    
    void removeLogFile(const std::string& filepath) {
        std::lock_guard<std::mutex> lock(aggregateMutex_);
        logFiles_.erase(std::remove(logFiles_.begin(), logFiles_.end(), filepath),
                       logFiles_.end());
    }
    
    void clearLogFiles() {
        std::lock_guard<std::mutex> lock(aggregateMutex_);
        logFiles_.clear();
    }
    
    // Aggregate logs from all files
    std::string aggregateLogs(const std::string& outputFormat = "json") {
        std::lock_guard<std::mutex> lock(aggregateMutex_);
        
        if (logFiles_.empty()) {
            return "No log files to aggregate.";
        }
        
        if (outputFormat == "json") {
            return aggregateToJSON();
        } else if (outputFormat == "csv") {
            return aggregateToCSV();
        } else if (outputFormat == "text") {
            return aggregateToText();
        } else {
            return "Unsupported format: " + outputFormat;
        }
    }
    
    // Export aggregated logs
    bool exportToFile(const std::string& filepath, const std::string& format = "json") {
        std::string aggregated = aggregateLogs(format);
        std::ofstream outFile(filepath);
        if (!outFile.is_open()) {
            return false;
        }
        outFile << aggregated;
        outFile.close();
        return true;
    }
    
    // Log analysis utilities
    size_t getTotalLogCount() const {
        std::lock_guard<std::mutex> lock(aggregateMutex_);
        size_t total = 0;
        for (const auto& filepath : logFiles_) {
            // In a real implementation, we would count logs in each file
            // For now, this is a placeholder
        }
        return total;
    }
    
    double getTotalSize() const {
        std::lock_guard<std::mutex> lock(aggregateMutex_);
        double totalSize = 0.0;
        for (const auto& filepath : logFiles_) {
            // In a real implementation, we would get file size
            // For now, this is a placeholder
        }
        return totalSize;
    }
    
    std::string getLogStatistics() const {
        std::lock_guard<std::mutex> lock(aggregateMutex_);
        std::ostringstream oss;
        oss << "=== AGGREGATED LOG STATISTICS ===" << std::endl;
        oss << "Total log files: " << logFiles_.size() << std::endl;
        oss << "Total log entries: " << getTotalLogCount() << std::endl;
        oss << "Total size: " << getTotalSize() / 1024.0 / 1024.0 << " MB" << std::endl;
        return oss.str();
    }
    
private:
    std::string aggregateToJSON() {
        std::ostringstream oss;
        oss << "[";
        for (size_t i = 0; i < logFiles_.size(); ++i) {
            if (i > 0) oss << ",";
            // In a real implementation, we would read each JSON log file and aggregate
            oss << "{\"file\": \"" << logFiles_[i] << "\", \"entries\": []}";
        }
        oss << "]";
        return oss.str();
    }
    
    std::string aggregateToCSV() {
        std::ostringstream oss;
        oss << "timestamp,level,neuronId,message" << std::endl;
        for (const auto& filepath : logFiles_) {
            // In a real implementation, we would read each CSV log file and append
        }
        return oss.str();
    }
    
    std::string aggregateToText() {
        std::ostringstream oss;
        oss << "=== AGGREGATED LOGS ===" << std::endl;
        for (const auto& filepath : logFiles_) {
            oss << "--- File: " << filepath << " ---" << std::endl;
            // In a real implementation, we would read each text log file and append
            oss << "[Log content would go here]" << std::endl;
        }
        return oss.str();
    }
};

// Log export utility for various output formats
class LogExporter {
private:
    std::mutex exportMutex_;
    
public:
    LogExporter() = default;
    
    // Export logs to various formats
    bool exportToFile(const std::string& filepath, 
                     const std::vector<std::string>& logLines,
                     const std::string& format = "text") {
        std::lock_guard<std::mutex> lock(exportMutex_);
        std::ofstream outFile(filepath);
        if (!outFile.is_open()) {
            return false;
        }
        
        if (format == "text") {
            for (const auto& line : logLines) {
                outFile << line << std::endl;
            }
        } else if (format == "json") {
            outFile << "[";
            for (size_t i = 0; i < logLines.size(); ++i) {
                if (i > 0) outFile << ",";
                outFile << logLines[i];
            }
            outFile << "]";
        } else if (format == "csv") {
            // In a real implementation, we would format as CSV
            for (const auto& line : logLines) {
                outFile << line << std::endl;
            }
        }
        
        outFile.close();
        return true;
    }
    
    // HTML export
    std::string exportToHTML(const std::vector<std::string>& logLines,
                            const std::string& title = "Log Report") {
        std::ostringstream oss;
        oss << "<!DOCTYPE html>" << std::endl;
        oss << "<html>" << std::endl;
        oss << "<head>" << std::endl;
        oss << "<title>" << title << "</title>" << std::endl;
        oss << "<style>" << std::endl;
        oss << "body { font-family: Arial, sans-serif; margin: 20px; }" << std::endl;
        oss << ".log-entry { margin: 5px 0; padding: 5px; border-left: 3px solid #ccc; }" << std::endl;
        oss << ".debug { border-color: #00f; }" << std::endl;
        oss << ".info { border-color: #0c0; }" << std::endl;
        oss << ".warning { border-color: #ff0; }" << std::endl;
        oss << ".error { border-color: #f00; }" << std::endl;
        oss << ".critical { border-color: #f0f; }" << std::endl;
        oss << "</style>" << std::endl;
        oss << "</head>" << std::endl;
        oss << "<body>" << std::endl;
        oss << "<h1>" << title << "</h1>" << std::endl;
        oss << "<div>" << std::endl;
        
        for (const auto& line : logLines) {
            oss << "<div class=\"log-entry\">" << std::endl;
            oss << line << std::endl;
            oss << "</div>" << std::endl;
        }
        
        oss << "</div>" << std::endl;
        oss << "</body>" << std::endl;
        oss << "</html>" << std::endl;
        
        return oss.str();
    }
    
    // Log filtering
    std::vector<std::string> filterLogs(const std::vector<std::string>& logLines,
                                       LogLevel minLevel = LogLevel::Debug,
                                       const std::string& component = "",
                                       const std::string& pattern = "") {
        std::vector<std::string> filtered;
        std::regex regex(pattern);
        
        for (const auto& line : logLines) {
            // In a real implementation, we would parse each log line and filter based on criteria
            // For now, this is a placeholder that returns all lines
            filtered.push_back(line);
        }
        
        return filtered;
    }
    
    // Log search
    std::vector<std::string> searchLogs(const std::vector<std::string>& logLines,
                                       const std::string& searchTerm,
                                       bool caseSensitive = false) {
        std::vector<std::string> results;
        std::regex searchRegex(searchTerm, caseSensitive ? std::regex_constants::ECMAScript : 
                                                   std::regex_constants::icase);
        
        for (const auto& line : logLines) {
            if (std::regex_search(line, searchRegex)) {
                results.push_back(line);
            }
        }
        
        return results;
    }
    
    // Log statistics
    std::string getLogStatistics(const std::vector<std::string>& logLines) const {
        std::ostringstream oss;
        oss << "=== LOG STATISTICS ===" << std::endl;
        oss << "Total log entries: " << logLines.size() << std::endl;
        
        // Count by level (simplified)
        size_t debugCount = 0, infoCount = 0, warningCount = 0, errorCount = 0, criticalCount = 0;
        for (const auto& line : logLines) {
            if (line.find("[DEBUG]") != std::string::npos) debugCount++;
            else if (line.find("[INFO]") != std::string::npos) infoCount++;
            else if (line.find("[WARN]") != std::string::npos) warningCount++;
            else if (line.find("[ERROR]") != std::string::npos) errorCount++;
            else if (line.find("[CRIT]") != std::string::npos) criticalCount++;
        }
        
        oss << "DEBUG: " << debugCount << std::endl;
        oss << "INFO: " << infoCount << std::endl;
        oss << "WARNING: " << warningCount << std::endl;
        oss << "ERROR: " << errorCount << std::endl;
        oss << "CRITICAL: " << criticalCount << std::endl;
        
        return oss.str();
    }
};

} // namespace nlm

#endif // LOG_SINKS_HPP