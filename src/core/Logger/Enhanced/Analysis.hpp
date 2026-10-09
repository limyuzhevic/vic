#pragma once

#include <string>
#include <chrono>
#include <memory>
#include <map>
#include <unordered_map>
#include <vector>
#include <algorithm>
#include <numeric>
#include <sstream>
#include <iomanip>
#include <regex>
#include <ctime>
#include "Enhanced/LogFormatter.hpp"

namespace nlm {

// Log entry statistics
struct LogStatistics {
    size_t totalCount;
    size_t errorCount;
    size_t warningCount;
    size_t infoCount;
    size_t debugCount;
    size_t criticalCount;
    
    std::chrono::system_clock::time_point firstLog;
    std::chrono::system_clock::time_point lastLog;
    
    std::map<std::string, size_t> logFrequency;
    std::map<std::string, size_t> fileFrequency;
    std::map<std::string, size_t> functionFrequency;
    std::map<std::string, size_t> threadFrequency;
    
    LogStatistics() : totalCount(0), errorCount(0), warningCount(0), infoCount(0), 
                     debugCount(0), criticalCount(0) {}
};

// Pattern detection for anomalies
struct PatternMatch {
    std::string pattern;
    std::string type; // "regex", "sequence", "frequency", "temporal"
    std::vector<EnhancedLogEntry> matches;
    double confidence;
    std::string description;
};

// Log analyzer for real-time statistics and pattern detection
class LogAnalyzer {
public:
    LogAnalyzer(size_t maxHistorySize = 100000) : maxHistorySize_(maxHistorySize) {}
    
    void addLog(const EnhancedLogEntry& entry) {
        std::lock_guard<std::mutex> lock(logHistoryMutex_);
        
        // Add to history
        logHistory_.push_back(entry);
        
        // Maintain history size limit
        if (logHistory_.size() > maxHistorySize_) {
            logHistory_.erase(logHistory_.begin());
        }
        
        // Update statistics
        updateStatistics(entry);
        
        // Detect patterns
        detectPatterns(entry);
    }
    
    LogStatistics getStatistics() const {
        std::lock_guard<std::mutex> lock(logHistoryMutex_);
        return statistics_;
    }
    
    std::vector<PatternMatch> getPatternMatches() const {
        std::lock_guard<std::mutex> lock(patternMutex_);
        return patternMatches_;
    }
    
    std::vector<EnhancedLogEntry> getLogsByLevel(LogLevel level) const {
        std::lock_guard<std::mutex> lock(logHistoryMutex_);
        std::vector<EnhancedLogEntry> result;
        
        for (const auto& entry : logHistory_) {
            if (entry.level == level) {
                result.push_back(entry);
            }
        }
        
        return result;
    }
    
    std::vector<EnhancedLogEntry> getLogsByTag(const std::string& tag, const std::string& value) const {
        std::lock_guard<std::mutex> lock(logHistoryMutex_);
        std::vector<EnhancedLogEntry> result;
        
        for (const auto& entry : logHistory_) {
            if (entry.tags.find(tag) != entry.tags.end() && 
                entry.tags.at(tag) == value) {
                result.push_back(entry);
            }
        }
        
        return result;
    }
    
    std::vector<EnhancedLogEntry> getLogsInTimeRange(
        const std::chrono::system_clock::time_point& start,
        const std::chrono::system_clock::time_point& end) const {
        std::lock_guard<std::mutex> lock(logHistoryMutex_);
        std::vector<EnhancedLogEntry> result;
        
        for (const auto& entry : logHistory_) {
            if (entry.timestamp >= start && entry.timestamp <= end) {
                result.push_back(entry);
            }
        }
        
        return result;
    }
    
    std::string generateReport(const std::string& format = "text") const {
        if (format == "json") {
            return generateJsonReport();
        } else if (format == "csv") {
            return generateCsvReport();
        } else {
            return generateTextReport();
        }
    }
    
    void clear() {
        std::lock_guard<std::mutex> lock(logHistoryMutex_);
        logHistory_.clear();
        statistics_ = LogStatistics();
        patternMatches_.clear();
    }
    
private:
    void updateStatistics(const EnhancedLogEntry& entry) {
        // Update basic counts
        statistics_.totalCount++;
        
        switch (entry.level) {
            case LogLevel::Debug: statistics_.debugCount++; break;
            case LogLevel::Info: statistics_.infoCount++; break;
            case LogLevel::Warning: statistics_.warningCount++; break;
            case LogLevel::Error: statistics_.errorCount++; break;
            case LogLevel::Critical: statistics_.criticalCount++; break;
        }
        
        // Update timestamps
        if (statistics_.totalCount == 1) {
            statistics_.firstLog = entry.timestamp;
        }
        statistics_.lastLog = entry.timestamp;
        
        // Update frequency counts
        statistics_.logFrequency[entry.message]++;
        statistics_.fileFrequency[entry.file]++;
        statistics_.functionFrequency[entry.function]++;
        statistics_.threadFrequency[entry.threadId]++;
    }
    
    void detectPatterns(const EnhancedLogEntry& entry) {
        // Simple pattern detection for demo
        // In a real implementation, this would be more sophisticated
        
        // Detect repeated messages
        if (recentMessages_.find(entry.message) != recentMessages_.end()) {
            PatternMatch pattern;
            pattern.pattern = entry.message;
            pattern.type = "frequency";
            pattern.confidence = calculateConfidence(entry.message);
            pattern.description = "Repeated message detected";
            
            // Find all matches
            for (const auto& historyEntry : logHistory_) {
                if (historyEntry.message == entry.message) {
                    pattern.matches.push_back(historyEntry);
                }
            }
            
            std::lock_guard<std::mutex> lock(patternMutex_);
            patternMatches_.push_back(pattern);
        }
        
        recentMessages_[entry.message] = entry.timestamp;
    }
    
    double calculateConfidence(const std::string& message) const {
        auto it = recentMessages_.find(message);
        if (it != recentMessages_.end()) {
            auto now = std::chrono::system_clock::now();
            auto duration = std::chrono::duration_cast<std::chrono::seconds>(now - it->second);
            if (duration.count() < 60) { // Within last minute
                return 0.8;
            } else if (duration.count() < 300) { // Within last 5 minutes
                return 0.5;
            }
        }
        return 0.1;
    }
    
    std::string generateTextReport() const {
        std::stringstream report;
        report << "Log Analysis Report" << std::endl;
        report << "====================" << std::endl;
        report << "Total logs: " << statistics_.totalCount << std::endl;
        report << "Errors: " << statistics_.errorCount << std::endl;
        report << "Warnings: " << statistics_.warningCount << std::endl;
        report << "Info: " << statistics_.infoCount << std::endl;
        report << "Debug: " << statistics_.debugCount << std::endl;
        report << "Critical: " << statistics_.criticalCount << std::endl;
        
        if (statistics_.totalCount > 0) {
            auto duration = std::chrono::duration<double>(
                statistics_.lastLog - statistics_.firstLog);
            report << "Time range: " << duration.count() << " seconds" << std::endl;
            report << "Logs per second: " << (statistics_.totalCount / duration.count()) << std::endl;
        }
        
        report << "\nTop messages:" << std::endl;
        for (const auto& pair : statistics_.logFrequency) {
            report << "  " << pair.second << ": " << pair.first << std::endl;
        }
        
        report << "\nTop files:" << std::endl;
        for (const auto& pair : statistics_.fileFrequency) {
            report << "  " << pair.second << ": " << pair.first << std::endl;
        }
        
        return report.str();
    }
    
    std::string generateJsonReport() const {
        std::stringstream report;
        report << "{\n  \"statistics\": {\n";
        report << "    \"totalCount\": " << statistics_.totalCount << ",\n";
        report << "    \"errorCount\": " << statistics_.errorCount << ",\n";
        report << "    \"warningCount\": " << statistics_.warningCount << ",\n";
        report << "    \"infoCount\": " << statistics_.infoCount << ",\n";
        report << "    \"debugCount\": " << statistics_.debugCount << ",\n";
        report << "    \"criticalCount\": " << statistics_.criticalCount << "\n";
        report << "  }\n";
        
        if (!statistics_.logFrequency.empty()) {
            report << ",\n  \"topMessages\": [\n";
            for (size_t i = 0; i < statistics_.logFrequency.size(); ++i) {
                const auto& pair = statistics_.logFrequency;
                auto it = pair.begin();
                std::advance(it, i);
                report << "    {\"message\": \"" << escapeJson(it->first) << "\", \"count\": " << it->second << "}" << (i < pair.size() - 1 ? "," : "") << "\n";
            }
            report << "  ]\n";
        }
        
        report << "}\n";
        return report.str();
    }
    
    std::string generateCsvReport() const {
        std::stringstream report;
        report << "Level,Count,Percentage\n";
        
        if (statistics_.totalCount > 0) {
            report << "ERROR," << statistics_.errorCount << "," 
                   << (statistics_.errorCount * 100.0 / statistics_.totalCount) << "%\n";
            report << "WARNING," << statistics_.warningCount << "," 
                   << (statistics_.warningCount * 100.0 / statistics_.totalCount) << "%\n";
            report << "INFO," << statistics_.infoCount << "," 
                   << (statistics_.infoCount * 100.0 / statistics_.totalCount) << "%\n";
            report << "DEBUG," << statistics_.debugCount << "," 
                   << (statistics_.debugCount * 100.0 / statistics_.totalCount) << "%\n";
            report << "CRITICAL," << statistics_.criticalCount << "," 
                   << (statistics_.criticalCount * 100.0 / statistics_.totalCount) << "%\n";
        }
        
        return report.str();
    }
    
    std::string escapeJson(const std::string& str) const {
        std::string escaped;
        for (char c : str) {
            switch (c) {
                case '\\': escaped += "\\\\"; break;
                case '\"': escaped += "\\\""; break;
                case '\n': escaped += "\\n"; break;
                default: escaped += c; break;
            }
        }
        return escaped;
    }
    
    size_t maxHistorySize_;
    
    std::vector<EnhancedLogEntry> logHistory_;
    LogStatistics statistics_;
    std::map<std::string, std::chrono::system_clock::time_point> recentMessages_;
    
    mutable std::mutex logHistoryMutex_;
    mutable std::mutex patternMutex_;
    std::vector<PatternMatch> patternMatches_;
};

// Log aggregator for combining multiple log sources
class LogAggregator {
public:
    LogAggregator() {}
    
    void addSource(std::unique_ptr<LogAnalyzer> analyzer) {
        sources_.push_back(std::move(analyzer));
    }
    
    LogStatistics aggregateStatistics() const {
        LogStatistics aggregated;
        for (const auto& analyzer : sources_) {
            LogStatistics stats = analyzer->getStatistics();
            aggregated.totalCount += stats.totalCount;
            aggregated.errorCount += stats.errorCount;
            aggregated.warningCount += stats.warningCount;
            aggregated.infoCount += stats.infoCount;
            aggregated.debugCount += stats.debugCount;
            aggregated.criticalCount += stats.criticalCount;
            
            // Merge frequency maps
            for (const auto& pair : stats.logFrequency) {
                aggregated.logFrequency[pair.first] += pair.second;
            }
            for (const auto& pair : stats.fileFrequency) {
                aggregated.fileFrequency[pair.first] += pair.second;
            }
            for (const auto& pair : stats.functionFrequency) {
                aggregated.functionFrequency[pair.first] += pair.second;
            }
            for (const auto& pair : stats.threadFrequency) {
                aggregated.threadFrequency[pair.first] += pair.second;
            }
        }
        return aggregated;
    }
    
    std::vector<PatternMatch> aggregatePatternMatches() const {
        std::vector<PatternMatch> allPatterns;
        for (const auto& analyzer : sources_) {
            std::vector<PatternMatch> patterns = analyzer->getPatternMatches();
            allPatterns.insert(allPatterns.end(), patterns.begin(), patterns.end());
        }
        return allPatterns;
    }
    
    std::vector<EnhancedLogEntry> getAllLogs() const {
        std::vector<EnhancedLogEntry> allLogs;
        for (const auto& analyzer : sources_) {
            // Get logs from analyzer - this would need a method in LogAnalyzer
            // For now, just placeholder
        }
        return allLogs;
    }
    
private:
    std::vector<std::unique_ptr<LogAnalyzer>> sources_;
};

// Log compressor for long-term storage
class LogCompressor {
public:
    LogCompressor() {}
    
    std::string compress(const std::vector<EnhancedLogEntry>& logs) const {
        // Simple compression: remove duplicate consecutive entries
        std::string compressed;
        if (logs.empty()) return compressed;
        
        compressed.reserve(logs.size() * 100); // Estimate size
        
        for (size_t i = 0; i < logs.size(); ++i) {
            compressed += serializeLogEntry(logs[i]);
            
            // Skip consecutive duplicates
            while (i + 1 < logs.size() && 
                   logs[i].level == logs[i + 1].level && 
                   logs[i].message == logs[i + 1].message &&
                   logs[i].file == logs[i + 1].file &&
                   logs[i].line == logs[i + 1].line) {
                i++;
            }
        }
        
        return compressed;
    }
    
    std::vector<EnhancedLogEntry> decompress(const std::string& compressed) const {
        std::vector<EnhancedLogEntry> logs;
        
        // Simple decompression for demo
        // In a real implementation, this would use more sophisticated compression
        
        // This is a placeholder implementation
        return logs;
    }
    
    double getCompressionRatio(const std::vector<EnhancedLogEntry>& logs) const {
        // Calculate original size
        size_t originalSize = 0;
        for (const auto& entry : logs) {
            originalSize += serializeLogEntry(entry).size();
        }
        
        // Calculate compressed size (placeholder)
        size_t compressedSize = compress(logs).size();
        
        if (originalSize == 0) return 1.0;
        
        return static_cast<double>(compressedSize) / originalSize;
    }
    
private:
    std::string serializeLogEntry(const EnhancedLogEntry& entry) const {
        std::stringstream ss;
        ss << static_cast<int>(entry.level) << ";";
        ss << entry.message << ";";
        ss << entry.file << ";";
        ss << entry.line << ";";
        ss << entry.function << ";";
        
        auto time = std::chrono::system_clock::to_time_t(entry.timestamp);
        ss << time << ";";
        
        ss << entry.threadId << ";";
        
        // Serialize tags
        ss << entry.tags.size() << ";";
        for (const auto& tag : entry.tags) {
            ss << tag.first << "=" << tag.second << ",";
        }
        
        // Serialize metadata
        ss << entry.metadata.size() << ";";
        for (const auto& meta : entry.metadata) {
            ss << meta.first << "=" << meta.second << ",";
        }
        
        ss << entry.sequenceNumber << ";";
        
        return ss.str();
    }
};

// Performance metrics logger
class PerformanceLogger {
public:
    PerformanceLogger() : logCount_(0), lastReportTime_(std::chrono::system_clock::now()) {}
    
    void logPerformance(const std::string& operation, double duration, 
                       size_t memoryUsage, int threadId = -1) {
        EnhancedLogEntry entry;
        entry.level = LogLevel::Info;
        entry.message = "PERFORMANCE: " + operation + " took " + 
                       std::to_string(duration) + "s, memory: " + 
                       std::to_string(memoryUsage) + " bytes";
        entry.file = "performance.log";
        entry.line = 0;
        entry.function = "logPerformance";
        entry.timestamp = std::chrono::system_clock::now();
        entry.threadId = (threadId >= 0 ? "thread_" + std::to_string(threadId) : "main");
        entry.tags["performance"] = "true";
        entry.metadata["operation"] = operation;
        entry.metadata["duration"] = std::to_string(duration);
        entry.metadata["memory_bytes"] = std::to_string(memoryUsage);
        entry.sequenceNumber = ++logCount_;
        
        // Add to performance analyzer
        performanceAnalyzer_.addLog(entry);
    }
    
    std::string getPerformanceReport() const {
        return performanceAnalyzer_.generateReport("text");
    }
    
    double getAverageDuration(const std::string& operation) const {
        return performanceAnalyzer_.getStatistics().logFrequency.count();
    }
    
private:
    LogAnalyzer performanceAnalyzer_;
    std::atomic<size_t> logCount_;
    std::chrono::system_clock::time_point lastReportTime_;
};

} // namespace nlm
