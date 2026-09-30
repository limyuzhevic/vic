#pragma once

#include <string>
#include <vector>
#include <memory>
#include <fstream>
#include <sstream>
#include <chrono>
#include <algorithm>
#include <functional>

namespace nlm {

/**
 * Development and Debugging Utilities
 * 
 * Comprehensive development toolkit for NLM including:
 * - Debug logging and tracing
 * - Memory debugging utilities
 * - Performance profiling tools
 * - Network visualization utilities
 * - Simulation debugging aids
 * - Checkpoint recovery tools
 */

class DebugLogger {
public:
    enum class LogLevel {
        TRACE,
        DEBUG,
        INFO,
        WARNING,
        ERROR,
        CRITICAL,
        NONE
    };
    
    struct LogEntry {
        std::chrono::time_point<std::chrono::high_resolution_clock> timestamp;
        LogLevel level;
        std::string source;
        std::string message;
        std::string file;
        int line;
        std::vector<std::string> stackTrace;
    };
    
    DebugLogger()
        : minLevel(LogLevel::INFO), enableStackTrace(true), maxFileSize(10 * 1024 * 1024) { // 10MB
        logFile = "debug.log";
        enableFileOutput(true);
        enableConsoleOutput(true);
    }
    
    ~DebugLogger() {
        flush();
    }
    
    // Disable copying
    DebugLogger(const DebugLogger&) = delete;
    DebugLogger& operator=(const DebugLogger&) = delete;
    
    /**
     * Set minimum log level
     */
    void setLogLevel(LogLevel level) {
        minLevel = level;
    }
    
    /**
     * Enable/disable file logging
     */
    void setFileOutput(bool enabled, const std::string& filename = "") {
        if (enabled && !filename.empty()) {
            logFile = filename;
        }
        enableFileOutput = enabled;
    }
    
    /**
     * Enable/disable console logging
     */
    void setConsoleOutput(bool enabled) {
        enableConsoleOutput = enabled;
    }
    
    /**
     * Enable/disable stack trace collection
     */
    void setStackTrace(bool enabled) {
        enableStackTrace = enabled;
    }
    
    /**
     * Log trace message
     */
    void trace(const std::string& source, const std::string& message, 
              const std::string& file = "", int line = 0) {
        if (minLevel <= LogLevel::TRACE) {
            logMessage(LogLevel::TRACE, source, message, file, line);
        }
    }
    
    /**
     * Log debug message
     */
    void debug(const std::string& source, const std::string& message,
              const std::string& file = "", int line = 0) {
        if (minLevel <= LogLevel::DEBUG) {
            logMessage(LogLevel::DEBUG, source, message, file, line);
        }
    }
    
    /**
     * Log info message
     */
    void info(const std::string& source, const std::string& message,
             const std::string& file = "", int line = 0) {
        if (minLevel <= LogLevel::INFO) {
            logMessage(LogLevel::INFO, source, message, file, line);
        }
    }
    
    /**
     * Log warning message
     */
    void warning(const std::string& source, const std::string& message,
                 const std::string& file = "", int line = 0) {
        if (minLevel <= LogLevel::WARNING) {
            logMessage(LogLevel::WARNING, source, message, file, line);
        }
    }
    
    /**
     * Log error message
     */
    void error(const std::string& source, const std::string& message,
              const std::string& file = "", int line = 0) {
        if (minLevel <= LogLevel::ERROR) {
            logMessage(LogLevel::ERROR, source, message, file, line);
        }
    }
    
    /**
     * Log critical message
     */
    void critical(const std::string& source, const std::string& message,
                  const std::string& file = "", int line = 0) {
        if (minLevel <= LogLevel::CRITICAL) {
            logMessage(LogLevel::CRITICAL, source, message, file, line);
        }
    }
    
    /**
     * Get recent log messages
     */
    std::vector<LogEntry> getRecentLogs(size_t count = 100) const {
        std::lock_guard<std::mutex> lock(logMutex);
        
        std::vector<LogEntry> recentLogs = logEntries;
        if (recentLogs.size() > count) {
            recentLogs.erase(recentLogs.begin(), recentLogs.begin() + (recentLogs.size() - count));
        }
        
        return recentLogs;
    }
    
    /**
     * Clear log history
     */
    void clearLogs() {
        std::lock_guard<std::mutex> lock(logMutex);
        logEntries.clear();
    }
    
    /**
     * Export logs to file
     */
    bool exportLogs(const std::string& filename) const {
        try {
            std::ofstream file(filename);
            if (!file.is_open()) {
                return false;
            }
            
            file << "=== Debug Log Export ===\n";
            file << "Exported: " << getCurrentTimestamp() << "\n";
            file << "Total Entries: " << logEntries.size() << "\n\n";
            
            for (const auto& entry : logEntries) {
                file << "[" << logLevelToString(entry.level) << "] "
                     << entry.timestamp.time_since_epoch().count() << "\n";
                file << "Source: " << entry.source << "\n";
                file << "Message: " << entry.message << "\n";
                if (!entry.file.empty()) {
                    file << "File: " << entry.file << ":" << entry.line << "\n";
                }
                if (!entry.stackTrace.empty()) {
                    file << "Stack Trace:\n";
                    for (const auto& frame : entry.stackTrace) {
                        file << "  " << frame << "\n";
                    }
                }
                file << "\n";
            }
            
            file.close();
            return true;
        } catch (const std::exception& e) {
            return false;
        }
    }
    
    /**
     * Get log statistics
     */
    std::string getLogStatistics() const {
        std::lock_guard<std::mutex> lock(logMutex);
        
        if (logEntries.empty()) {
            return "No log entries available";
        }
        
        std::map<LogLevel, size_t> levelCounts;
        for (const auto& entry : logEntries) {
            levelCounts[entry.level]++;
        }
        
        std::stringstream ss;
        ss << "=== Log Statistics ===\n";
        ss << "Total Entries: " << logEntries.size() << "\n";
        ss << "Log Levels:\n";
        ss << "  TRACE: " << levelCounts[LogLevel::TRACE] << "\n";
        ss << "  DEBUG: " << levelCounts[LogLevel::DEBUG] << "\n";
        ss << "  INFO:  " << levelCounts[LogLevel::INFO] << "\n";
        ss << "  WARNING: " << levelCounts[LogLevel::WARNING] << "\n";
        ss << "  ERROR: " << levelCounts[LogLevel::ERROR] << "\n";
        ss << "  CRITICAL: " << levelCounts[LogLevel::CRITICAL] << "\n";
        
        return ss.str();
    }
    
    /**
     * Search logs for specific pattern
     */
    std::vector<LogEntry> searchLogs(const std::string& pattern) const {
        std::lock_guard<std::mutex> lock(logMutex);
        
        std::vector<LogEntry> results;
        
        for (const auto& entry : logEntries) {
            if (entry.message.find(pattern) != std::string::npos ||
                entry.source.find(pattern) != std::string::npos) {
                results.push_back(entry);
            }
        }
        
        return results;
    }
    
    /**
     * Generate debug session report
     */
    std::string generateDebugReport() const {
        std::stringstream ss;
        
        ss << "=== Debug Session Report ===\n";
        ss << "Generated: " << getCurrentTimestamp() << "\n";
        ss << "Log Level: " << logLevelToString(minLevel) << "\n";
        ss << "File Output: " << (enableFileOutput ? "Enabled" : "Disabled") << "\n";
        ss << "Console Output: " << (enableConsoleOutput ? "Enabled" : "Disabled") << "\n";
        ss << "Stack Trace: " << (enableStackTrace ? "Enabled" : "Disabled") << "\n\n";
        
        ss << getLogStatistics() << "\n";
        
        // Add recent error/warning summary
        size_t errorCount = 0, warningCount = 0;
        for (const auto& entry : logEntries) {
            if (entry.level == LogLevel::ERROR || entry.level == LogLevel::CRITICAL) {
                errorCount++;
            } else if (entry.level == LogLevel::WARNING) {
                warningCount++;
            }
        }
        
        ss << "=== Issues Summary ===\n";
        ss << "Errors: " << errorCount << "\n";
        ss << "Warnings: " << warningCount << "\n";
        
        if (errorCount > 0 || warningCount > 0) {
            ss << "\nRecent Issues:\n";
            for (const auto& entry : logEntries) {
                if (entry.level == LogLevel::ERROR || entry.level == LogLevel::CRITICAL || entry.level == LogLevel::WARNING) {
                    ss << "[" << logLevelToString(entry.level) << "] "
                       << entry.source << ": " << entry.message << "\n";
                }
            }
        }
        
        return ss.str();
    }
    
    /**
     * Flush all logs to file
     */
    void flush() {
        if (enableFileOutput && !logFile.empty()) {
            std::lock_guard<std::mutex> lock(logMutex);
            
            // Check file size and rotate if needed
            std::ifstream checkFile(logFile, std::ios::ate);
            if (checkFile.tellg() >= maxFileSize) {
                // Rotate log file
                std::rename(logFile.c_str(), (logFile + ".old").c_str());
            }
            
            std::ofstream file(logFile, std::ios::app);
            if (file.is_open()) {
                for (const auto& entry : logEntries) {
                    file << "[" << logLevelToString(entry.level) << "] "
                         << entry.timestamp.time_since_epoch().count() << "\n";
                    file << "Source: " << entry.source << "\n";
                    file << "Message: " << entry.message << "\n";
                    if (!entry.file.empty()) {
                        file << "File: " << entry.file << ":" << entry.line << "\n";
                    }
                    file << "\n";
                }
                file.close();
                logEntries.clear();
            }
        }
    }
    
    /**
     * Get current timestamp
     */
    std::string getCurrentTimestamp() {
        auto now = std::chrono::system_clock::now();
        auto time = std::chrono::system_clock::to_time_t(now);
        
        std::stringstream ss;
        ss << std::put_time(std::localtime(&time), "%Y-%m-%d %H:%M:%S");
        
        return ss.str();
    }
    
    /**
     * Convert log level to string
     */
    std::string logLevelToString(LogLevel level) {
        switch (level) {
            case LogLevel::TRACE:   return "TRACE";
            case LogLevel::DEBUG:   return "DEBUG";
            case LogLevel::INFO:    return "INFO";
            case LogLevel::WARNING: return "WARNING";
            case LogLevel::ERROR:   return "ERROR";
            case LogLevel::CRITICAL: return "CRITICAL";
            default:                return "UNKNOWN";
        }
    }
    
protected:
    void logMessage(LogLevel level, const std::string& source, const std::string& message,
                   const std::string& file, int line) {
        std::lock_guard<std::mutex> lock(logMutex);
        
        LogEntry entry;
        entry.timestamp = std::chrono::high_resolution_clock::now();
        entry.level = level;
        entry.source = source;
        entry.message = message;
        entry.file = file;
        entry.line = line;
        
        if (enableStackTrace) {
            // Collect stack trace (simplified version)
            entry.stackTrace = collectStackTrace();
        }
        
        logEntries.push_back(entry);
        
        // Output to console if enabled
        if (enableConsoleOutput) {
            std::cout << "[" << logLevelToString(level) << "] "
                      << source << ": " << message << std::endl;
            if (!file.empty()) {
                std::cout << "  " << file << ":" << line << std::endl;
            }
        }
    }
    
    std::vector<std::string> collectStackTrace() {
        std::vector<std::string> stackTrace;
        // Simplified stack trace collection
        stackTrace.push_back("Stack trace collection not fully implemented");
        return stackTrace;
    }
    
    void enableFileOutput(bool enabled, const std::string& filename = "") {
        enableFileOutput = enabled;
        if (enabled && !filename.empty()) {
            logFile = filename;
        }
    }
    
    void enableConsoleOutput(bool enabled) {
        enableConsoleOutput = enabled;
    }
    
    bool enableStackTrace;
    bool enableFileOutput;
    bool enableConsoleOutput;
    LogLevel minLevel;
    size_t maxFileSize;
    std::string logFile;
    
    std::vector<LogEntry> logEntries;
    mutable std::mutex logMutex;
};

/**
 * Memory Debugger
 * 
 * Tools for memory debugging and leak detection
 */
class MemoryDebugger {
public:
    struct MemoryAllocation {
        void* address;
        size_t size;
        std::string file;
        int line;
        std::chrono::time_point<std::chrono::high_resolution_clock> timestamp;
        std::string stackTrace;
    };
    
    MemoryDebugger()
        : trackAllocations(true), detectLeaks(true), maxAllocations(10000) {
        totalAllocated = 0;
        totalFreed = 0;
    }
    
    ~MemoryDebugger() {
        if (detectLeaks) {
            checkForLeaks();
        }
    }
    
    // Disable copying
    MemoryDebugger(const MemoryDebugger&) = delete;
    MemoryDebugger& operator=(const MemoryDebugger&) = delete;
    
    /**
     * Track memory allocation
     */
    void* trackAllocation(size_t size, const std::string& file, int line) {
        if (!trackAllocations) {
            return std::malloc(size);
        }
        
        void* address = std::malloc(size);
        if (address) {
            std::lock_guard<std::mutex> lock(memoryMutex);
            
            MemoryAllocation alloc;
            alloc.address = address;
            alloc.size = size;
            alloc.file = file;
            alloc.line = line;
            alloc.timestamp = std::chrono::high_resolution_clock::now();
            alloc.stackTrace = collectStackTrace();
            
            allocations.push_back(alloc);
            totalAllocated += size;
            
            // Limit number of tracked allocations
            if (allocations.size() > maxAllocations) {
                allocations.erase(allocations.begin());
            }
        }
        
        return address;
    }
    
    /**
     * Track memory deallocation
     */
    void trackDeallocation(void* address) {
        if (!trackAllocations || !detectLeaks) {
            std::free(address);
            return;
        }
        
        std::lock_guard<std::mutex> lock(memoryMutex);
        
        // Find and remove allocation
        auto it = std::find_if(allocations.begin(), allocations.end(),
                              [address](const MemoryAllocation& alloc) {
                                  return alloc.address == address;
                              });
        
        if (it != allocations.end()) {
            totalFreed += it->size;
            allocations.erase(it);
        }
        
        std::free(address);
    }
    
    /**
     * Get memory statistics
     */
    std::string getMemoryStatistics() const {
        std::lock_guard<std::mutex> lock(memoryMutex);
        
        std::stringstream ss;
        ss << "=== Memory Statistics ===\n";
        ss << "Total Allocated: " << totalAllocated << " bytes\n";
        ss << "Total Freed: " << totalFreed << " bytes\n";
        ss << "Current Allocations: " << allocations.size() << "\n";
        ss << "Tracked Size: " << (totalAllocated - totalFreed) << " bytes\n";
        ss << "Leak Rate: " << ((totalAllocated - totalFreed) / (totalAllocated > 0 ? totalAllocated : 1)) * 100 << "%\n";
        
        return ss.str();
    }
    
    /**
     * Check for memory leaks
     */
    std::vector<MemoryAllocation> checkForLeaks() {
        std::lock_guard<std::mutex> lock(memoryMutex);
        return allocations;
    }
    
    /**
     * Export memory allocations to file
     */
    bool exportAllocations(const std::string& filename) {
        try {
            std::ofstream file(filename);
            if (!file.is_open()) {
                return false;
            }
            
            file << "=== Memory Allocation Log ===\n";
            file << "Exported: " << getCurrentTimestamp() << "\n";
            file << "Total Allocations: " << allocations.size() << "\n\n";
            
            for (const auto& alloc : allocations) {
                file << "Address: " << alloc.address << "\n";
                file << "Size: " << alloc.size << " bytes\n";
                file << "File: " << alloc.file << ":" << alloc.line << "\n";
                file << "Timestamp: " << alloc.timestamp.time_since_epoch().count() << "\n";
                file << "Stack Trace:\n" << alloc.stackTrace << "\n\n";
            }
            
            file.close();
            return true;
        } catch (const std::exception& e) {
            return false;
        }
    }
    
    /**
     * Get allocation by address
     */
    MemoryAllocation* getAllocation(void* address) {
        std::lock_guard<std::mutex> lock(memoryMutex);
        
        auto it = std::find_if(allocations.begin(), allocations.end(),
                              [address](const MemoryAllocation& alloc) {
                                  return alloc.address == address;
                              });
        
        if (it != allocations.end()) {
            return &(*it);
        }
        
        return nullptr;
    }
    
    /**
     * Generate memory debug report
     */
    std::string generateMemoryDebugReport() {
        std::stringstream ss;
        
        ss << "=== Memory Debug Report ===\n";
        ss << "Generated: " << getCurrentTimestamp() << "\n";
        ss << "Tracking Allocations: " << (trackAllocations ? "Yes" : "No") << "\n";
        ss << "Leak Detection: " << (detectLeaks ? "Enabled" : "Disabled") << "\n";
        ss << "Max Tracked Allocations: " << maxAllocations << "\n\n";
        
        ss << getMemoryStatistics() << "\n";
        
        // List top memory allocations
        if (!allocations.empty()) {
            ss << "=== Top Memory Allocations ===\n";
            std::vector<const MemoryAllocation*> sorted;
            for (const auto& alloc : allocations) {
                sorted.push_back(&alloc);
            }
            
            std::sort(sorted.begin(), sorted.end(),
                     [](const MemoryAllocation* a, const MemoryAllocation* b) {
                         return a->size > b->size;
                     });
            
            for (size_t i = 0; i < std::min(size_t(10), sorted.size()); ++i) {
                const auto& alloc = *sorted[i];
                ss << "Size: " << alloc.size << " bytes\n";
                ss << "File: " << alloc.file << ":" << alloc.line << "\n";
                ss << "Age: " << std::chrono::duration_cast<std::chrono::seconds>(
                    std::chrono::high_resolution_clock::now() - alloc.timestamp).count() << " seconds\n\n";
            }
        }
        
        return ss.str();
    }
    
    /**
     * Enable/disable allocation tracking
     */
    void setTrackAllocations(bool enabled) {
        trackAllocations = enabled;
    }
    
    /**
     * Enable/disable leak detection
     */
    void setDetectLeaks(bool enabled) {
        detectLeaks = enabled;
    }
    
    /**
     * Set maximum tracked allocations
     */
    void setMaxAllocations(size_t max) {
        maxAllocations = max;
    }
    
protected:
    std::string collectStackTrace() {
        // Simplified stack trace collection
        std::stringstream ss;
        ss << "Memory allocation at line " << "[unknown]" << " in file " << "[unknown]";
        return ss.str();
    }
    
    std::string getCurrentTimestamp() {
        auto now = std::chrono::system_clock::now();
        auto time = std::chrono::system_clock::to_time_t(now);
        
        std::stringstream ss;
        ss << std::put_time(std::localtime(&time), "%Y-%m-%d %H:%M:%S");
        
        return ss.str();
    }
    
    bool trackAllocations;
    bool detectLeaks;
    size_t maxAllocations;
    size_t totalAllocated;
    size_t totalFreed;
    
    std::vector<MemoryAllocation> allocations;
    mutable std::mutex memoryMutex;
};

/**
 * Performance Profiler
 * 
 * Tools for performance profiling and optimization
 */
class PerformanceProfiler {
public:
    struct ProfileData {
        std::string functionName;
        std::string fileName;
        int lineNumber;
        double executionTime;
        size_t callCount;
        double averageTime;
        double minTime;
        double maxTime;
        std::chrono::time_point<std::chrono::high_resolution_clock> startTime;
    };
    
    PerformanceProfiler()
        : profileEnabled(true), maxProfilePoints(10000), samplingRate(0.1) { // 10% sampling
        currentProfile.clear();
        totalProfiles = 0;
    }
    
    ~PerformanceProfiler() {
        exportProfile("performance_profile.json");
    }
    
    // Disable copying
    PerformanceProfiler(const PerformanceProfiler&) = delete;
    PerformanceProfiler& operator=(const PerformanceProfiler&) = delete;
    
    /**
     * Start profiling a function
     */
    void startProfile(const std::string& functionName, const std::string& fileName = "", int lineNumber = 0) {
        if (!profileEnabled) return;
        
        std::lock_guard<std::mutex> lock(profileMutex);
        
        ProfileData profile;
        profile.functionName = functionName;
        profile.fileName = fileName;
        profile.lineNumber = lineNumber;
        profile.startTime = std::chrono::high_resolution_clock::now();
        profile.executionTime = 0.0;
        profile.callCount = 0;
        profile.averageTime = 0.0;
        profile.minTime = std::numeric_limits<double>::max();
        profile.maxTime = 0.0;
        
        currentProfile = profile;
    }
    
    /**
     * Stop profiling and record data
     */
    void stopProfile() {
        if (!profileEnabled || currentProfile.functionName.empty()) return;
        
        auto endTime = std::chrono::high_resolution_clock::now();
        double executionTime = std::chrono::duration<double, std::milli>(endTime - currentProfile.startTime).count();
        
        std::lock_guard<std::mutex> lock(profileMutex);
        
        currentProfile.executionTime = executionTime;
        currentProfile.callCount++;
        currentProfile.averageTime = (currentProfile.averageTime * (currentProfile.callCount - 1) + executionTime) / currentProfile.callCount;
        currentProfile.minTime = std::min(currentProfile.minTime, executionTime);
        currentProfile.maxTime = std::max(currentProfile.maxTime, executionTime);
        
        // Add to profile history if sampling condition is met
        if (rand() / (RAND_MAX + 1.0) < samplingRate) {
            profileHistory.push_back(currentProfile);
            totalProfiles++;
        }
        
        // Limit profile history
        if (profileHistory.size() > maxProfilePoints) {
            profileHistory.erase(profileHistory.begin());
        }
        
        currentProfile.functionName.clear();
    }
    
    /**
     * Get current profile
     */
    ProfileData getCurrentProfile() const {
        std::lock_guard<std::mutex> lock(profileMutex);
        return currentProfile;
    }
    
    /**
     * Get profile statistics
     */
    std::string getProfileStatistics() const {
        std::lock_guard<std::mutex> lock(profileMutex);
        
        if (profileHistory.empty()) {
            return "No profile data available";
        }
        
        std::stringstream ss;
        ss << "=== Profile Statistics ===\n";
        ss << "Total Profiled Functions: " << profileHistory.size() << "\n";
        ss << "Total Profiles: " << totalProfiles << "\n";
        ss << "Sampling Rate: " << (samplingRate * 100) << "%\n\n";
        
        // Sort by execution time (descending)
        std::vector<const ProfileData*> sorted;
        for (const auto& profile : profileHistory) {
            sorted.push_back(&profile);
        }
        
        std::sort(sorted.begin(), sorted.end(),
                 [](const ProfileData* a, const ProfileData* b) {
                     return a->executionTime > b->executionTime;
                 });
        
        ss << "=== Top 10 Most Time-Consuming Functions ===\n";
        for (size_t i = 0; i < std::min(size_t(10), sorted.size()); ++i) {
            const auto& profile = *sorted[i];
            ss << "1. " << profile.functionName << "\n";
            ss << "   File: " << profile.fileName << ":" << profile.lineNumber << "\n";
            ss << "   Total Time: " << profile.executionTime << " ms\n";
            ss << "   Average Time: " << profile.averageTime << " ms\n";
            ss << "   Calls: " << profile.callCount << "\n";
            ss << "   Min/Max: " << profile.minTime << " / " << profile.maxTime << " ms\n\n";
        }
        
        return ss.str();
    }
    
    /**
     * Export profile to file
     */
    bool exportProfile(const std::string& filename) {
        try {
            std::ofstream file(filename);
            if (!file.is_open()) {
                return false;
            }
            
            file << "{\n";
            file << "  \"profile_version\": \"1.0\",\n";
            file << "  \"export_time\": \"" << getCurrentTimestamp() << "\",\n";
            file << "  \"total_functions\": " << profileHistory.size() << ",\n";
            file << "  \"total_profiles\": " << totalProfiles << ",\n";
            file << "  \"sampling_rate\": " << samplingRate << ",\n";
            file << "  \"profiles\": [\n";
            
            for (size_t i = 0; i < profileHistory.size(); ++i) {
                const auto& profile = profileHistory[i];
                file << "    {\n";
                file << "      \"function\": \"" << profile.functionName << "\",\n";
                file << "      \"file\": \"" << profile.fileName << "\",\n";
                file << "      \"line\": " << profile.lineNumber << ",\n";
                file << "      \"execution_time\": " << profile.executionTime << ",\n";
                file << "      \"call_count\": " << profile.callCount << ",\n";
                file << "      \"average_time\": " << profile.averageTime << ",\n";
                file << "      \"min_time\": " << profile.minTime << ",\n";
                file << "      \"max_time\": " << profile.maxTime << "\n";
                file << "    }";
                if (i < profileHistory.size() - 1) {
                    file << ",\n";
                } else {
                    file << "\n";
                }
            }
            
            file << "  ]\n";
            file << "}\n";
            
            file.close();
            return true;
        } catch (const std::exception& e) {
            return false;
        }
    }
    
    /**
     * Generate performance report
     */
    std::string generatePerformanceReport() {
        std::stringstream ss;
        
        ss << "=== Performance Profiling Report ===\n";
        ss << "Generated: " << getCurrentTimestamp() << "\n";
        ss << "Profile Enabled: " << (profileEnabled ? "Yes" : "No") << "\n";
        ss << "Max Profile Points: " << maxProfilePoints << "\n";
        ss << "Sampling Rate: " << (samplingRate * 100) << "%\n\n";
        
        ss << getProfileStatistics() << "\n";
        
        // Performance recommendations
        ss << "=== Performance Recommendations ===\n";
        
        std::vector<const ProfileData*> sorted;
        for (const auto& profile : profileHistory) {
            sorted.push_back(&profile);
        }
        
        std::sort(sorted.begin(), sorted.end(),
                 [](const ProfileData* a, const ProfileData* b) {
                     return a->averageTime > b->averageTime;
                 });
        
        ss << "Top 5 performance bottlenecks:\n";
        for (size_t i = 0; i < std::min(size_t(5), sorted.size()); ++i) {
            const auto& profile = *sorted[i];
            double improvement = ((profile.maxTime - profile.minTime) / profile.minTime) * 100;
            ss << "1. " << profile.functionName << "\n";
            ss << "   Range: " << profile.minTime << " - " << profile.maxTime << " ms\n";
            ss << "   Variation: " << improvement << "%\n";
            ss << "   Calls: " << profile.callCount << "\n\n";
        }
        
        return ss.str();
    }
    
    /**
     * Enable/disable profiling
     */
    void setProfileEnabled(bool enabled) {
        profileEnabled = enabled;
    }
    
    /**
     * Set sampling rate
     */
    void setSamplingRate(double rate) {
        samplingRate = std::max(0.0, std::min(1.0, rate));
    }
    
    /**
     * Set maximum profile points
     */
    void setMaxProfilePoints(size_t max) {
        maxProfilePoints = max;
    }
    
protected:
    std::string getCurrentTimestamp() {
        auto now = std::chrono::system_clock::now();
        auto time = std::chrono::system_clock::to_time_t(now);
        
        std::stringstream ss;
        ss << std::put_time(std::localtime(&time), "%Y-%m-%d %H:%M:%S");
        
        return ss.str();
    }
    
    bool profileEnabled;
    double samplingRate;
    size_t maxProfilePoints;
    size_t totalProfiles;
    
    ProfileData currentProfile;
    std::vector<ProfileData> profileHistory;
    mutable std::mutex profileMutex;
};

/**
 * Network Visualizer
 * 
 * Tools for network visualization and analysis
 */
class NetworkVisualizer {
public:
    struct NeuronInfo {
        size_t id;
        std::string name;
        float x, y, z;          // Position in 3D space
        float radius;           // Visual radius
        std::string type;       // Neuron type (excitatory/inhibitory)
        float activation;       // Current activation level
        std::vector<size_t> connections; // Connected neuron IDs
    };
    
    struct SynapseInfo {
        size_t sourceId;
        size_t targetId;
        float weight;
        float delay;
        std::string type;       // Excitatory/inhibitory
        float activity;         // Current synaptic activity
    };
    
    NetworkVisualizer()
        : showWeights(true), showActivity(false), showConnections(true), 
          enable3D(true), pointSize(2.0), lineWidth(1.0) {
        backgroundColor = "black";
        neuronColor = "white";
        excitatoryColor = "red";
        inhibitoryColor = "blue";
    }
    
    ~NetworkVisualizer() = default;
    
    // Disable copying
    NetworkVisualizer(const NetworkVisualizer&) = delete;
    NetworkVisualizer& operator=(const NetworkVisualizer&) = delete;
    
    /**
     * Add neuron to visualization
     */
    void addNeuron(const NeuronInfo& neuron) {
        neurons.push_back(neuron);
    }
    
    /**
     * Add synapse to visualization
     */
    void addSynapse(const SynapseInfo& synapse) {
        synapses.push_back(synapse);
    }
    
    /**
     * Clear all data
     */
    void clear() {
        neurons.clear();
        synapses.clear();
    }
    
    /**
     * Generate visualization data
     */
    std::string generateVisualization() {
        std::stringstream ss;
        
        ss << "=== Network Visualization Data ===\n";
        ss << "Total Neurons: " << neurons.size() << "\n";
        ss << "Total Synapses: " << synapses.size() << "\n";
        ss << "Visualization Mode: " << (enable3D ? "3D" : "2D") << "\n";
        ss << "Background Color: " << backgroundColor << "\n";
        ss << "Neuron Color: " << neuronColor << "\n";
        ss << "Excitatory Color: " << excitatoryColor << "\n";
        ss << "Inhibitory Color: " << inhibitoryColor << "\n\n";
        
        // Generate neuron data
        ss << "=== Neuron Data ===\n";
        for (size_t i = 0; i < neurons.size(); ++i) {
            const auto& neuron = neurons[i];
            ss << "Neuron " << neuron.id << ":\n";
            ss << "  Name: " << neuron.name << "\n";
            ss << "  Position: (" << neuron.x << ", " << neuron.y << ", " << neuron.z << ")\n";
            ss << "  Radius: " << neuron.radius << "\n";
            ss << "  Type: " << neuron.type << "\n";
            ss << "  Activation: " << neuron.activation << "\n";
            ss << "  Connections: [";
            for (size_t j = 0; j < neuron.connections.size(); ++j) {
                ss << neuron.connections[j];
                if (j < neuron.connections.size() - 1) ss << ", ";
            }
            ss << "]\n\n";
        }
        
        // Generate synapse data
        ss << "=== Synapse Data ===\n";
        for (size_t i = 0; i < synapses.size(); ++i) {
            const auto& synapse = synapses[i];
            ss << "Synapse " << i << ":\n";
            ss << "  Source: " << synapse.sourceId << "\n";
            ss << "  Target: " << synapse.targetId << "\n";
            ss << "  Weight: " << synapse.weight << "\n";
            ss << "  Delay: " << synapse.delay << "\n";
            ss << "  Type: " << synapse.type << "\n";
            ss << "  Activity: " << synapse.activity << "\n\n";
        }
        
        return ss.str();
    }
    
    /**
     * Export visualization to file
     */
    bool exportVisualization(const std::string& filename) {
        try {
            std::ofstream file(filename);
            if (!file.is_open()) {
                return false;
            }
            
            // Generate different output formats
            std::string jsonData = generateJSONVisualization();
            std::string csvData = generateCSVVisualization();
            std::string htmlData = generateHTMLVisualization();
            
            // Write JSON file
            file << jsonData;
            file.close();
            
            // Write CSV file
            std::ofstream csvFile(filename + ".csv");
            if (csvFile.is_open()) {
                csvFile << csvData;
                csvFile.close();
            }
            
            // Write HTML file
            std::ofstream htmlFile(filename + ".html");
            if (htmlFile.is_open()) {
                htmlFile << htmlData;
                htmlFile.close();
            }
            
            return true;
        } catch (const std::exception& e) {
            return false;
        }
    }
    
    /**
     * Generate network statistics
     */
    std::string generateNetworkStatistics() {
        std::stringstream ss;
        
        ss << "=== Network Statistics ===\n";
        ss << "Total Neurons: " << neurons.size() << "\n";
        ss << "Total Synapses: " << synapses.size() << "\n";
        
        if (!neurons.empty()) {
            double totalActivation = 0.0;
            double maxActivation = -std::numeric_limits<double>::max();
            double minActivation = std::numeric_limits<double>::max();
            
            for (const auto& neuron : neurons) {
                totalActivation += neuron.activation;
                maxActivation = std::max(maxActivation, neuron.activation);
                minActivation = std::min(minActivation, neuron.activation);
            }
            
            double avgActivation = totalActivation / neurons.size();
            
            ss << "Activation Statistics:\n";
            ss << "  Average Activation: " << avgActivation << "\n";
            ss << "  Maximum Activation: " << maxActivation << "\n";
            ss << "  Minimum Activation: " << minActivation << "\n";
        }
        
        // Calculate connectivity statistics
        if (!synapses.empty()) {
            std::map<size_t, size_t> neuronConnectionCount;
            
            for (const auto& synapse : synapses) {
                neuronConnectionCount[synapse.sourceId]++;
                neuronConnectionCount[synapse.targetId]++;
            }
            
            size_t maxConnections = 0;
            size_t totalConnections = 0;
            
            for (const auto& pair : neuronConnectionCount) {
                maxConnections = std::max(maxConnections, pair.second);
                totalConnections += pair.second;
            }
            
            double avgConnections = static_cast<double>(totalConnections) / neurons.size();
            
            ss << "Connectivity Statistics:\n";
            ss << "  Average Connections per Neuron: " << avgConnections << "\n";
            ss << "  Maximum Connections: " << maxConnections << "\n";
        }
        
        return ss.str();
    }
    
    /**
     * Generate JSON visualization data
     */
    std::string generateJSONVisualization() {
        std::stringstream ss;
        
        ss << "{\n";
        ss << "  \"type\": \"network\",\n";
        ss << "  \"version\": \"1.0\",\n";
        ss << "  \"metadata\": {\n";
        ss << "    \"timestamp\": \"" << getCurrentTimestamp() << "\",\n";
        ss << "    \"neuron_count\": " << neurons.size() << ",\n";
        ss << "    \"synapse_count\": " << synapses.size() << "\n";
        ss << "  },\n";
        ss << "  \"settings\": {\n";
        ss << "    \"show_weights\": " << (showWeights ? "true" : "false") << ",\n";
        ss << "    \"show_activity\": " << (showActivity ? "true" : "false") << ",\n";
        ss << "    \"show_connections\": " << (showConnections ? "true" : "false") << ",\n";
        ss << "    \"enable_3d\": " << (enable3D ? "true" : "false") << ",\n";
        ss << "    \"point_size\": " << pointSize << ",\n";
        ss << "    \"line_width\": " << lineWidth << ",\n";
        ss << "    \"background_color\": \"" << backgroundColor << "\",\n";
        ss << "    \"neuron_color\": \"" << neuronColor << "\",\n";
        ss << "    \"excitatory_color\": \"" << excitatoryColor << "\",\n";
        ss << "    \"inhibitory_color\": \"" << inhibitoryColor << "\"\n";
        ss << "  },\n";
        ss << "  \"neurons\": [\n";
        
        for (size_t i = 0; i < neurons.size(); ++i) {
            const auto& neuron = neurons[i];
            ss << "    {\n";
            ss << "      \"id\": " << neuron.id << ",\n";
            ss << "      \"name\": \"" << neuron.name << "\",\n";
            ss << "      \"position\": [" << neuron.x << ", " << neuron.y << ", " << neuron.z << "],\n";
            ss << "      \"radius\": " << neuron.radius << ",\n";
            ss << "      \"type\": \"" << neuron.type << "\",\n";
            ss << "      \"activation\": " << neuron.activation << ",\n";
            ss << "      \"connections\": [";
            for (size_t j = 0; j < neuron.connections.size(); ++j) {
                ss << neuron.connections[j];
                if (j < neuron.connections.size() - 1) ss << ", ";
            }
            ss << "\n";
            ss << "    }";
            if (i < neurons.size() - 1) ss << ",\n";
            else ss << "\n";
        }
        
        ss << "  ],\n";
        ss << "  \"synapses\": [\n";
        
        for (size_t i = 0; i < synapses.size(); ++i) {
            const auto& synapse = synapses[i];
            ss << "    {\n";
            ss << "      \"source\": " << synapse.sourceId << ",\n";
            ss << "      \"target\": " << synapse.targetId << ",\n";
            ss << "      \"weight\": " << synapse.weight << ",\n";
            ss << "      \"delay\": " << synapse.delay << ",\n";
            ss << "      \"type\": \"" << synapse.type << "\",\n";
            ss << "      \"activity\": " << synapse.activity << "\n";
            ss << "    }";
            if (i < synapses.size() - 1) ss << ",\n";
            else ss << "\n";
        }
        
        ss << "  ]\n";
        ss << "}\n";
        
        return ss.str();
    }
    
    /**
     * Generate CSV visualization data
     */
    std::string generateCSVVisualization() {
        std::stringstream ss;
        
        ss << "NeuronID,Name,X,Y,Z,Radius,Type,Activation\n";
        for (const auto& neuron : neurons) {
            ss << neuron.id << "," << neuron.name << "," 
               << neuron.x << "," << neuron.y << "," << neuron.z << "," 
               << neuron.radius << "," << neuron.type << "," << neuron.activation << "\n";
        }
        
        ss << "\nSynapseID,Source,Target,Weight,Delay,Type,Activity\n";
        for (size_t i = 0; i < synapses.size(); ++i) {
            const auto& synapse = synapses[i];
            ss << i << "," << synapse.sourceId << "," << synapse.targetId << "," 
               << synapse.weight << "," << synapse.delay << "," << synapse.type << "," << synapse.activity << "\n";
        }
        
        return ss.str();
    }
    
    /**
     * Generate HTML visualization
     */
    std::string generateHTMLVisualization() {
        std::stringstream ss;
        
        ss << "<!DOCTYPE html>\n";
        ss << "<html>\n";
        ss << "<head>\n";
        ss << "  <title>NLM Network Visualization</title>\n";
        ss << "  <style>\n";
        ss << "    body { margin: 0; padding: 20px; background-color: " << backgroundColor << "; }\n";
        ss << "    .neuron { fill: " << neuronColor << "; stroke: white; stroke-width: 1; }\n";
        ss << "    .excitatory { fill: " << excitatoryColor << "; }\n";
        ss << "    .inhibitory { fill: " << inhibitoryColor << "; }\n";
        ss << "    .synapse { stroke: gray; stroke-width: 0.5; stroke-opacity: 0.5; }\n";
        ss << "  </style>\n";
        ss << "</head>\n";
        ss << "<body>\n";
        ss << "  <h1>NLM Network Visualization</h1>\n";
        ss << "  <div id=\"network\">\n";
        ss << "    <!-- Network visualization will be rendered here -->\n";
        ss << "  </div>\n";
        ss << "  <script>\n";
        ss << "    // Network data for visualization\n";
        ss << "    const neurons = " << generateJSONVisualization() << ";\n";
        ss << "    // Initialize visualization (simplified)\n";
        ss << "    console.log('Network visualization loaded with ' + neurons.neurons.length + ' neurons');\n";
        ss << "  </script>\n";
        ss << "</body>\n";
        ss << "</html>\n";
        
        return ss.str();
    }
    
    /**
     * Get current timestamp
     */
    std::string getCurrentTimestamp() {
        auto now = std::chrono::system_clock::now();
        auto time = std::chrono::system_clock::to_time_t(now);
        
        std::stringstream ss;
        ss << std::put_time(std::localtime(&time), "%Y-%m-%d %H:%M:%S");
        
        return ss.str();
    }
    
    /**
     * Enable/disable weight display
     */
    void setShowWeights(bool enabled) {
        showWeights = enabled;
    }
    
    /**
     * Enable/disable activity display
     */
    void setShowActivity(bool enabled) {
        showActivity = enabled;
    }
    
    /**
     * Enable/disable connection display
     */
    void setShowConnections(bool enabled) {
        showConnections = enabled;
    }
    
    /**
     * Enable/disable 3D visualization
     */
    void setEnable3D(bool enabled) {
        enable3D = enabled;
    }
    
    /**
     * Set point size
     */
    void setPointSize(double size) {
        pointSize = size;
    }
    
    /**
     * Set line width
     */
    void setLineWidth(double width) {
        lineWidth = width;
    }
    
    /**
     * Set background color
     */
    void setBackgroundColor(const std::string& color) {
        backgroundColor = color;
    }
    
    /**
     * Set neuron color
     */
    void setNeuronColor(const std::string& color) {
        neuronColor = color;
    }
    
    /**
     * Set excitatory neuron color
     */
    void setExcitatoryColor(const std::string& color) {
        excitatoryColor = color;
    }
    
    /**
     * Set inhibitory neuron color
     */
    void setInhibitoryColor(const std::string& color) {
        inhibitoryColor = color;
    }
    
    /**
     * Generate visualization report
     */
    std::string generateVisualizationReport() {
        std::stringstream ss;
        
        ss << "=== Network Visualization Report ===\n";
        ss << "Generated: " << getCurrentTimestamp() << "\n";
        ss << "Visualization Settings:\n";
        ss << "  Show Weights: " << (showWeights ? "Yes" : "No") << "\n";
        ss << "  Show Activity: " << (showActivity ? "Yes" : "No") << "\n";
        ss << "  Show Connections: " << (showConnections ? "Yes" : "No") << "\n";
        ss << "  Enable 3D: " << (enable3D ? "Yes" : "No") << "\n";
        ss << "  Point Size: " << pointSize << "\n";
        ss << "  Line Width: " << lineWidth << "\n";
        ss << "  Background Color: " << backgroundColor << "\n";
        ss << "  Neuron Color: " << neuronColor << "\n";
        ss << "  Excitatory Color: " << excitatoryColor << "\n";
        ss << "  Inhibitory Color: " << inhibitoryColor << "\n\n";
        
        ss << generateNetworkStatistics() << "\n";
        
        return ss.str();
    }
    
protected:
    bool showWeights;
    bool showActivity;
    bool showConnections;
    bool enable3D;
    double pointSize;
    double lineWidth;
    std::string backgroundColor;
    std::string neuronColor;
    std::string excitatoryColor;
    std::string inhibitoryColor;
    
    std::vector<NeuronInfo> neurons;
    std::vector<SynapseInfo> synapses;
};

/**
 * Simulation Debugger
 * 
 * Tools for debugging simulation issues and analyzing performance
 */
class SimulationDebugger {
public:
    struct StepEvent {
        size_t step;
        double time;
        std::string eventType;
        std::string details;
        std::chrono::time_point<std::chrono::high_resolution_clock> timestamp;
    };
    
    SimulationDebugger()
        : recordSteps(true), recordEvents(true), enableBreakpoints(false), 
          maxStepsToRecord(100000), enablePerformanceAnalysis(true) {
        currentStep = 0;
        simulationStartTime = std::chrono::high_resolution_clock::now();
    }
    
    ~SimulationDebugger() {
        exportStepData("simulation_steps.json");
        if (enablePerformanceAnalysis) {
            generatePerformanceAnalysis();
        }
    }
    
    // Disable copying
    SimulationDebugger(const SimulationDebugger&) = delete;
    SimulationDebugger& operator=(const SimulationDebugger&) = delete;
    
    /**
     * Record step information
     */
    void recordStep(size_t step, double time, const std::string& eventType, const std::string& details = "") {
        if (!recordSteps) return;
        
        std::lock_guard<std::mutex> lock(debugMutex);
        
        StepEvent event;
        event.step = step;
        event.time = time;
        event.eventType = eventType;
        event.details = details;
        event.timestamp = std::chrono::high_resolution_clock::now();
        
        stepEvents.push_back(event);
        currentStep = step;
        
        // Limit recorded steps
        if (stepEvents.size() > maxStepsToRecord) {
            stepEvents.erase(stepEvents.begin());
        }
        
        // Check breakpoints
        if (enableBreakpoints) {
            checkBreakpoints(step, eventType, details);
        }
    }
    
    /**
     * Record simulation state
     */
    void recordState(const std::string& stateName, const std::string& stateData) {
        if (!recordEvents) return;
        
        std::lock_guard<std::mutex> lock(debugMutex);
        
        StateSnapshot snapshot;
        snapshot.name = stateName;
        snapshot.data = stateData;
        snapshot.step = currentStep;
        snapshot.timestamp = std::chrono::high_resolution_clock::now();
        
        stateSnapshots.push_back(snapshot);
        
        // Check for state conditions
        checkStateConditions(stateName, stateData);
    }
    
    /**
     * Get simulation statistics
     */
    std::string getSimulationStatistics() const {
        std::lock_guard<std::mutex> lock(debugMutex);
        
        std::stringstream ss;
        ss << "=== Simulation Statistics ===\n";
        ss << "Current Step: " << currentStep << "\n";
        ss << "Total Steps Recorded: " << stepEvents.size() << "\n";
        ss << "Total State Snapshots: " << stateSnapshots.size() << "\n";
        
        if (!stepEvents.empty()) {
            double totalTime = 0.0;
            size_t totalSpikes = 0;
            size_t maxStepSize = 0;
            
            for (const auto& event : stepEvents) {
                totalTime += event.time;
                if (event.eventType.find("spike") != std::string::npos) {
                    totalSpikes++;
                }
                if (event.step > maxStepSize) {
                    maxStepSize = event.step;
                }
            }
            
            double avgStepTime = totalTime / stepEvents.size();
            double avgSpikesPerStep = static_cast<double>(totalSpikes) / stepEvents.size();
            
            ss << "Time Statistics:\n";
            ss << "  Total Simulation Time: " << totalTime << " seconds\n";
            ss << "  Average Step Time: " << avgStepTime << " seconds\n";
            ss << "  Maximum Step: " << maxStepSize << "\n";
            
            ss << "Activity Statistics:\n";
            ss << "  Average Spikes per Step: " << avgSpikesPerStep << "\n";
            ss << "  Spike Events: " << totalSpikes << "\n";
        }
        
        return ss.str();
    }
    
    /**
     * Export step data to file
     */
    bool exportStepData(const std::string& filename) {
        try {
            std::ofstream file(filename);
            if (!file.is_open()) {
                return false;
            }
            
            file << "{\n";
            file << "  \"version\": \"1.0\",\n";
            file << "  \"export_time\": \"" << getCurrentTimestamp() << "\",\n";
            file << "  \"total_steps\": " << stepEvents.size() << ",\n";
            file << "  \"current_step\": " << currentStep << ",\n";
            file << "  \"steps\": [\n";
            
            for (size_t i = 0; i < stepEvents.size(); ++i) {
                const auto& event = stepEvents[i];
                file << "    {\n";
                file << "      \"step\": " << event.step << ",\n";
                file << "      \"time\": " << event.time << ",\n";
                file << "      \"event_type\": \"" << event.eventType << "\",\n";
                file << "      \"details\": \"" << event.details << "\",\n";
                file << "      \"timestamp\": \"" << event.timestamp.time_since_epoch().count() << "\"\n";
                file << "    }";
                if (i < stepEvents.size() - 1) file << ",\n";
                else file << "\n";
            }
            
            file << "  ]\n";
            file << "}\n";
            
            file.close();
            return true;
        } catch (const std::exception& e) {
            return false;
        }
    }
    
    /**
     * Generate step analysis report
     */
    std::string generateStepAnalysisReport() {
        std::stringstream ss;
        
        ss << "=== Step Analysis Report ===\n";
        ss << "Generated: " << getCurrentTimestamp() << "\n";
        ss << "Analysis Range: Step 0 to " << (stepEvents.empty() ? 0 : stepEvents.back().step) << "\n\n";
        
        if (stepEvents.empty()) {
            ss << "No step data available for analysis.\n";
            return ss.str();
        }
        
        // Analyze step sizes
        std::vector<size_t> stepSizes;
        for (size_t i = 1; i < stepEvents.size(); ++i) {
            stepSizes.push_back(stepEvents[i].step - stepEvents[i-1].step);
        }
        
        if (!stepSizes.empty()) {
            double totalVariation = 0.0;
            size_t maxVariation = 0;
            
            for (size_t i = 1; i < stepSizes.size(); ++i) {
                size_t variation = std::abs(static_cast<int>(stepSizes[i]) - static_cast<int>(stepSizes[i-1]));
                totalVariation += variation;
                maxVariation = std::max(maxVariation, variation);
            }
            
            double avgVariation = totalVariation / stepSizes.size();
            
            ss << "=== Step Size Analysis ===\n";
            ss << "Step Size Statistics:\n";
            ss << "  Average Step Change: " << avgVariation << " steps\n";
            ss << "  Maximum Step Change: " << maxVariation << " steps\n";
            ss << "  Total Steps: " << stepEvents.size() << "\n\n";
        }
        
        // Analyze event types
        std::map<std::string, size_t> eventCounts;
        for (const auto& event : stepEvents) {
            eventCounts[event.eventType]++;
        }
        
        ss << "=== Event Type Analysis ===\n";
        for (const auto& pair : eventCounts) {
            double percentage = (static_cast<double>(pair.second) / stepEvents.size()) * 100;
            ss << "  " << pair.first << ": " << pair.second << " (" << percentage << "% )\n";
        }
        ss << "\n";
        
        return ss.str();
    }
    
    /**
     * Generate performance analysis
     */
    std::string generatePerformanceAnalysis() {
        std::stringstream ss;
        
        ss << "=== Performance Analysis ===\n";
        ss << "Generated: " << getCurrentTimestamp() << "\n\n";
        
        if (stepEvents.empty()) {
            ss << "No performance data available.\n";
            return ss.str();
        }
        
        // Calculate performance metrics
        double totalTime = 0.0;
        size_t totalEvents = stepEvents.size();
        
        for (const auto& event : stepEvents) {
            totalTime += event.time;
        }
        
        double avgStepTime = totalTime / totalEvents;
        double stepsPerSecond = totalEvents / totalTime;
        
        ss << "=== Performance Metrics ===\n";
        ss << "Total Simulation Time: " << totalTime << " seconds\n";
        ss << "Average Step Time: " << avgStepTime << " seconds\n";
        ss << "Steps per Second: " << stepsPerSecond << "\n";
        ss << "Real-time Simulation Speed: " << (stepsPerSecond / 1000.0) << "x\n\n";
        
        // Performance comparison
        ss << "=== Performance Comparison ===\n";
        ss << "Early Phase (Steps 0-100):\n";
        
        size_t earlySteps = std::min(size_t(100), stepEvents.size());
        double earlyTime = 0.0;
        for (size_t i = 0; i < earlySteps; ++i) {
            earlyTime += stepEvents[i].time;
        }
        double earlySpeed = earlySteps / earlyTime;
        
        ss << "  Steps per Second: " << earlySpeed << "\n";
        
        ss << "Late Phase (Steps -100 to end):\n";
        if (stepEvents.size() >= 100) {
            size_t lateStart = stepEvents.size() - 100;
            double lateTime = 0.0;
            for (size_t i = lateStart; i < stepEvents.size(); ++i) {
                lateTime += stepEvents[i].time;
            }
            double lateSpeed = 100.0 / lateTime;
            ss << "  Steps per Second: " << lateSpeed << "\n";
            
            double speedChange = (lateSpeed - earlySpeed) / earlySpeed * 100;
            ss << "  Speed Change: " << speedChange << "%\n";
        }
        
        return ss.str();
    }
    
    /**
     * Set breakpoint
     */
    void setBreakpoint(size_t step, const std::string& condition) {
        breakpoints.push_back({step, condition});
    }
    
    /**
     * Enable/disable breakpoints
     */
    void setBreakpointsEnabled(bool enabled) {
        enableBreakpoints = enabled;
    }
    
    /**
     * Check breakpoints
     */
    void checkBreakpoints(size_t step, const std::string& eventType, const std::string& details) {
        for (const auto& breakpoint : breakpoints) {
            if (step >= breakpoint.step) {
                // Evaluate condition (simplified)
                bool conditionMet = (eventType.find(breakpoint.condition) != std::string::npos) ||
                                  (details.find(breakpoint.condition) != std::string::npos);
                
                if (conditionMet) {
                    // Trigger breakpoint
                    debug("SimulationDebugger", "Breakpoint triggered at step " + std::to_string(step) +
                          ", condition: " + breakpoint.condition);
                }
            }
        }
    }
    
    /**
     * Check state conditions
     */
    void checkStateConditions(const std::string& stateName, const std::string& stateData) {
        // Simplified state condition checking
        if (stateName.find("error") != std::string::npos || stateData.find("error") != std::string::npos) {
            warning("SimulationDebugger", "Potential error state detected: " + stateName);
        }
    }
    
protected:
    bool recordSteps;
    bool recordEvents;
    bool enableBreakpoints;
    size_t maxStepsToRecord;
    bool enablePerformanceAnalysis;
    
    size_t currentStep;
    std::chrono::time_point<std::chrono::high_resolution_clock> simulationStartTime;
    
    std::vector<StepEvent> stepEvents;
    std::vector<StateSnapshot> stateSnapshots;
    std::vector<Breakpoint> breakpoints;
    
    mutable std::mutex debugMutex;
    
    struct StateSnapshot {
        std::string name;
        std::string data;
        size_t step;
        std::chrono::time_point<std::chrono::high_resolution_clock> timestamp;
    };
    
    struct Breakpoint {
        size_t step;
        std::string condition;
    };
    
    std::string getCurrentTimestamp() {
        auto now = std::chrono::system_clock::now();
        auto time = std::chrono::system_clock::to_time_t(now);
        
        std::stringstream ss;
        ss << std::put_time(std::localtime(&time), "%Y-%m-%d %H:%M:%S");
        
        return ss.str();
    }
};

/**
 * Checkpoint Recovery Tool
 * 
 * Tools for simulation checkpointing and recovery
 */
class CheckpointRecovery {
public:
    struct Checkpoint {
        size_t step;
        double time;
        std::string description;
        std::chrono::time_point<std::chrono::high_resolution_clock> timestamp;
        std::string checksum;
        size_t dataSize;
    };
    
    CheckpointRecovery()
        : autoSave(true), maxCheckpoints(100), enableCompression(true), 
          checkpointInterval(1000), enableAutoRecovery(false) {
        currentCheckpointStep = 0;
    }
    
    ~CheckpointRecovery() {
        saveCheckpointIndex();
    }
    
    // Disable copying
    CheckpointRecovery(const CheckpointRecovery&) = delete;
    CheckpointRecovery& operator=(const CheckpointRecovery&) = delete;
    
    /**
     * Create checkpoint
     */
    bool createCheckpoint(size_t step, double time, const std::string& description = "Simulation checkpoint") {
        std::lock_guard<std::mutex> lock(checkpointMutex);
        
        Checkpoint checkpoint;
        checkpoint.step = step;
        checkpoint.time = time;
        checkpoint.description = description;
        checkpoint.timestamp = std::chrono::high_resolution_clock::now();
        checkpoint.checksum = generateChecksum(step, time, description);
        checkpoint.dataSize = calculateDataSize(step, time, description);
        
        checkpoints.push_back(checkpoint);
        currentCheckpointStep = step;
        
        // Save checkpoint data
        if (!saveCheckpointData(checkpoint)) {
            return false;
        }
        
        // Update checkpoint index
        updateCheckpointIndex(checkpoint);
        
        // Clean old checkpoints if needed
        if (checkpoints.size() > maxCheckpoints) {
            checkpoints.erase(checkpoints.begin());
        }
        
        // Auto-save if enabled
        if (autoSave) {
            saveCheckpointIndex();
        }
        
        return true;
    }
    
    /**
     * Restore checkpoint
     */
    bool restoreCheckpoint(size_t step) {
        std::lock_guard<std::mutex> lock(checkpointMutex);
        
        // Find checkpoint
        auto it = std::find_if(checkpoints.begin(), checkpoints.end(),
                              [step](const Checkpoint& cp) { return cp.step == step; });
        
        if (it == checkpoints.end()) {
            return false;
        }
        
        const Checkpoint& checkpoint = *it;
        
        // Restore checkpoint data
        if (!restoreCheckpointData(checkpoint)) {
            return false;
        }
        
        currentCheckpointStep = checkpoint.step;
        
        return true;
    }
    
    /**
     * List available checkpoints
     */
    std::vector<Checkpoint> listCheckpoints() const {
        std::lock_guard<std::mutex> lock(checkpointMutex);
        return checkpoints;
    }
    
    /**
     * Get current checkpoint
     */
    Checkpoint getCurrentCheckpoint() const {
        std::lock_guard<std::mutex> lock(checkpointMutex);
        
        if (checkpoints.empty()) {
            return Checkpoint{0, 0.0, "No checkpoint", std::chrono::high_resolution_clock::now(), "", 0};
        }
        
        return checkpoints.back();
    }
    
    /**
     * Export checkpoints to file
     */
    bool exportCheckpoints(const std::string& filename) {
        try {
            std::ofstream file(filename);
            if (!file.is_open()) {
                return false;
            }
            
            file << "{\n";
            file << "  \"version\": \"1.0\",\n";
            file << "  \"export_time\": \"" << getCurrentTimestamp() << "\",\n";
            file << "  \"total_checkpoints\": " << checkpoints.size() << ",\n";
            file << "  \"current_checkpoint\": " << currentCheckpointStep << ",\n";
            file << "  \"checkpoints\": [\n";
            
            for (size_t i = 0; i < checkpoints.size(); ++i) {
                const auto& checkpoint = checkpoints[i];
                file << "    {\n";
                file << "      \"step\": " << checkpoint.step << ",\n";
                file << "      \"time\": " << checkpoint.time << ",\n";
                file << "      \"description\": \"" << checkpoint.description << "\",\n";
                file << "      \"timestamp\": \"" << checkpoint.timestamp.time_since_epoch().count() << "\",\n";
                file << "      \"checksum\": \"" << checkpoint.checksum << "\",\n";
                file << "      \"data_size\": " << checkpoint.dataSize << "\n";
                file << "    }";
                if (i < checkpoints.size() - 1) file << ",\n";
                else file << "\n";
            }
            
            file << "  ]\n";
            file << "}\n";
            
            file.close();
            return true;
        } catch (const std::exception& e) {
            return false;
        }
    }
    
    /**
     * Import checkpoints from file
     */
    bool importCheckpoints(const std::string& filename) {
        try {
            std::ifstream file(filename);
            if (!file.is_open()) {
                return false;
            }
            
            // Parse JSON (simplified)
            std::string content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
            
            // Simple JSON parsing for checkpoint import
            size_t startPos = content.find("\"checkpoints\": [");
            if (startPos == std::string::npos) {
                return false;
            }
            
            startPos += 17; // Skip "\"checkpoints\": ["
            size_t endPos = content.find("]", startPos);
            if (endPos == std::string::npos) {
                return false;
            }
            
            std::string checkpointsJson = content.substr(startPos, endPos - startPos);
            
            // Parse individual checkpoints
            size_t pos = 0;
            while (true) {
                size_t braceStart = checkpointsJson.find("{", pos);
                if (braceStart == std::string::npos) break;
                
                size_t braceEnd = checkpointsJson.find("}", braceStart);
                if (braceEnd == std::string::npos) break;
                
                std::string checkpointJson = checkpointsJson.substr(braceStart + 1, braceEnd - braceStart - 1);
                
                Checkpoint checkpoint = parseCheckpointJson(checkpointJson);
                checkpoints.push_back(checkpoint);
                
                pos = braceEnd + 1;
                if (pos >= checkpointsJson.size()) break;
            }
            
            file.close();
            return true;
        } catch (const std::exception& e) {
            return false;
        }
    }
    
    /**
     * Enable/disable auto-save
     */
    void setAutoSave(bool enabled) {
        autoSave = enabled;
    }
    
    /**
     * Set checkpoint interval
     */
    void setCheckpointInterval(size_t interval) {
        checkpointInterval = interval;
    }
    
    /**
     * Set maximum checkpoints
     */
    void setMaxCheckpoints(size_t max) {
        maxCheckpoints = max;
    }
    
    /**
     * Enable/disable auto-recovery
     */
    void setAutoRecovery(bool enabled) {
        enableAutoRecovery = enabled;
    }
    
    /**
    <?> Generate checkpoint report
     */
    std::string generateCheckpointReport() {
        std::stringstream ss;
        
        ss << "=== Checkpoint Recovery Report ===\n";
        ss << "Generated: " << getCurrentTimestamp() << "\n";
        ss << "Auto Save: " << (autoSave ? "Enabled" : "Disabled") << "\n";
        ss << "Max Checkpoints: " << maxCheckpoints << "\n";
        ss << "Checkpoint Interval: " << checkpointInterval << " steps\n";
        ss << "Auto Recovery: " << (enableAutoRecovery ? "Enabled" : "Disabled") << "\n\n";
        
        ss << "=== Checkpoint Statistics ===\n";
        ss << "Total Checkpoints: " << checkpoints.size() << "\n";
        ss << "Current Checkpoint Step: " << currentCheckpointStep << "\n";
        
        if (!checkpoints.empty()) {
            ss << "\nCheckpoint Range:\n";
            ss << "  First Checkpoint: Step " << checkpoints.front().step << "\n";
            ss << "  Last Checkpoint: Step " << checkpoints.back().step << "\n";
            
            if (checkpoints.size() > 1) {
                ss << "  Average Interval: " << 
                    (static_cast<double>(checkpoints.back().step - checkpoints.front().step) / 
                     (checkpoints.size() - 1)) << " steps\n";
            }
        }
        
        return ss.str();
    }
    
protected:
    bool autoSave;
    size_t maxCheckpoints;
    bool enableCompression;
    size_t checkpointInterval;
    bool enableAutoRecovery;
    
    size_t currentCheckpointStep;
    
    std::vector<Checkpoint> checkpoints;
    mutable std::mutex checkpointMutex;
    
    std::string getCurrentTimestamp() {
        auto now = std::chrono::system_clock::now();
        auto time = std::chrono::system_clock::to_time_t(now);
        
        std::stringstream ss;
        ss << std::put_time(std::localtime(&time), "%Y-%m-%d %H:%M:%S");
        
        return ss.str();
    }
    
    std::string generateChecksum(size_t step, double time, const std::string& description) {
        // Simplified checksum generation
        std::stringstream ss;
        ss << step << ":" << time << ":" << description;
        return std::to_string(std::hash<std::string>{}(ss.str()));
    }
    
    size_t calculateDataSize(size_t step, double time, const std::string& description) {
        // Simplified data size calculation
        return sizeof(step) + sizeof(time) + description.length();
    }
    
    bool saveCheckpointData(const Checkpoint& checkpoint) {
        // Placeholder for actual checkpoint data saving
        return true;
    }
    
    bool restoreCheckpointData(const Checkpoint& checkpoint) {
        // Placeholder for actual checkpoint data restoration
        return true;
    }
    
    void updateCheckpointIndex(const Checkpoint& checkpoint) {
        // Placeholder for checkpoint index update
    }
    
    void saveCheckpointIndex() {
        // Placeholder for saving checkpoint index
    }
    
    Checkpoint parseCheckpointJson(const std::string& json) {
        Checkpoint checkpoint;
        
        // Simple JSON parsing for checkpoint
        size_t stepPos = json.find("\"step\":");
        if (stepPos != std::string::npos) {
            size_t colonPos = json.find(":", stepPos + 8);
            if (colonPos != std::string::npos) {
                size_t commaPos = json.find(",", colonPos);
                if (commaPos != std::string::npos) {
                    std::string stepStr = json.substr(colonPos + 1, commaPos - colonPos - 1);
                    try {
                        checkpoint.step = std::stoul(stepStr);
                    } catch (...) {
                        checkpoint.step = 0;
                    }
                }
            }
        }
        
        size_t timePos = json.find("\"time\":");
        if (timePos != std::string::npos) {
            size_t colonPos = json.find(":", timePos + 8);
            if (colonPos != std::string::npos) {
                size_t commaPos = json.find(",", colonPos);
                if (commaPos != std::string::npos) {
                    std::string timeStr = json.substr(colonPos + 1, commaPos - colonPos - 1);
                    try {
                        checkpoint.time = std::stod(timeStr);
                    } catch (...) {
                        checkpoint.time = 0.0;
                    }
                }
            }
        }
        
        size_t descPos = json.find("\"description\":\"");
        if (descPos != std::string::npos) {
            size_t quotePos = descPos + 17;
            size_t endQuotePos = json.find("\"", quotePos);
            if (endQuotePos != std::string::npos) {
                checkpoint.description = json.substr(quotePos, endQuotePos - quotePos);
            }
        }
        
        return checkpoint;
    }
};

// Main development and debugging utilities factory
class DevelopmentTools {
public:
    static std::unique_ptr<DebugLogger> createDebugLogger() {
        return std::make_unique<DebugLogger>();
    }
    
    static std::unique_ptr<MemoryDebugger> createMemoryDebugger() {
        return std::make_unique<MemoryDebugger>();
    }
    
    static std::unique_ptr<PerformanceProfiler> createPerformanceProfiler() {
        return std::make_unique<PerformanceProfiler>();
    }
    
    static std::unique_ptr<NetworkVisualizer> createNetworkVisualizer() {
        return std::make_unique<NetworkVisualizer>();
    }
    
    static std::unique_ptr<SimulationDebugger> createSimulationDebugger() {
        return std::make_unique<SimulationDebugger>();
    }
    
    static std::unique_ptr<CheckpointRecovery> createCheckpointRecovery() {
        return std::make_unique<CheckpointRecovery>();
    }
    
    static std::unique_ptr<ConfigExporter> createConfigExporter() {
        return std::make_unique<SimpleConfigExporter>();
    }
    
    static std::unique_ptr<PerformanceExporter> createPerformanceExporter() {
        return std::make_unique<PerformanceExporter>();
    }
    
    static std::unique_ptr<ConfigValidator> createConfigValidator() {
        return std::make_unique<ConfigValidator>();
    }
};

} // namespace nlm