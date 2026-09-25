// Test utilities and debugging tools
// Advanced debugging utilities for NLM test framework
#pragma once

#include <vector>
#include <string>
#include <memory>
#include <algorithm>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <chrono>
#include <random>
#include <map>
#include <unordered_map>
#include <iostream>
#include <cassert>
#include <filesystem>
#include <cctype>
#include "test_utils.hpp"

namespace nlm {
namespace test {

// Debug mode and verbosity control
class DebugController {
public:
    enum class LogLevel {
        Silent,
        Error,
        Warning,
        Info,
        Debug,
        Trace
    };
    
    struct LogEntry {
        std::chrono::system_clock::time_point timestamp;
        LogLevel level;
        std::string message;
        std::string sourceFile;
        int lineNumber;
        std::unordered_map<std::string, std::string> metadata;
    };
    
private:
    LogLevel currentLevel;
    bool enableDebugOutput;
    bool enablePerformanceLogging;
    std::vector<LogEntry> logEntries;
    std::ofstream logFile;
    std::string logFilename;
    
public:
    DebugController() : currentLevel(LogLevel::Info), enableDebugOutput(true), 
                       enablePerformanceLogging(true), logFilename("test_debug.log") {
        logFile.open(logFilename, std::ios::out | std::ios::trunc);
    }
    
    ~DebugController() {
        if (logFile.is_open()) {
            logFile.close();
        }
    }
    
    void setLogLevel(LogLevel level) {
        currentLevel = level;
        logMessage(LogLevel::Info, "Log level set to " + std::to_string(static_cast<int>(level)));
    }
    
    void setDebugOutput(bool enabled) {
        enableDebugOutput = enabled;
    }
    
    void setPerformanceLogging(bool enabled) {
        enablePerformanceLogging = enabled;
    }
    
    LogLevel getLogLevel() const { return currentLevel; }
    
    void logMessage(LogLevel level, const std::string& message, 
                   const std::string& sourceFile = "", int lineNumber = 0,
                   const std::unordered_map<std::string, std::string>& metadata = {}) {
        if (level < currentLevel && currentLevel != LogLevel::Silent) {
            return;
        }
        
        LogEntry entry;
        entry.timestamp = std::chrono::system_clock::now();
        entry.level = level;
        entry.message = message;
        entry.sourceFile = sourceFile;
        entry.lineNumber = lineNumber;
        entry.metadata = metadata;
        
        // Console output
        if (enableDebugOutput) {
            std::cout << formatLogEntry(entry);
        }
        
        // File output
        if (logFile.is_open()) {
            logFile << formatLogEntry(entry);
        }
        
        // Store in memory
        logEntries.push_back(entry);
    }
    
    void logError(const std::string& message, const std::string& sourceFile = "", int lineNumber = 0) {
        logMessage(LogLevel::Error, message, sourceFile, lineNumber);
    }
    
    void logWarning(const std::string& message, const std::string& sourceFile = "", int lineNumber = 0) {
        logMessage(LogLevel::Warning, message, sourceFile, lineNumber);
    }
    
    void logInfo(const std::string& message, const std::string& sourceFile = "", int lineNumber = 0) {
        logMessage(LogLevel::Info, message, sourceFile, lineNumber);
    }
    
    void logDebug(const std::string& message, const std::string& sourceFile = "", int lineNumber = 0) {
        logMessage(LogLevel::Debug, message, sourceFile, lineNumber);
    }
    
    std::vector<LogEntry> getLogEntries() const { return logEntries; }
    
    void clearLog() {
        logEntries.clear();
        if (logFile.is_open()) {
            logFile.close();
            logFile.open(logFilename, std::ios::out | std::ios::trunc);
        }
    }
    
    std::string getLogSummary() const {
        std::stringstream ss;
        ss << "Log Summary:" << std::endl;
        ss << "  Total entries: " << logEntries.size() << std::endl;
        
        std::map<LogLevel, int> levelCounts;
        for (const auto& entry : logEntries) {
            levelCounts[entry.level]++;
        }
        
        for (const auto& pair : levelCounts) {
            ss << "  " << logLevelToString(pair.first) << ": " << pair.second << std::endl;
        }
        
        return ss.str();
    }
    
private:
    std::string formatLogEntry(const LogEntry& entry) const {
        std::stringstream ss;
        
        // Timestamp
        auto time = std::chrono::system_clock::to_time_t(entry.timestamp);
        ss << "[" << std::put_time(std::localtime(&time), "%H:%M:%S") << "] ";
        
        // Level
        ss << "[" << logLevelToString(entry.level) << "] ";
        
        // Source
        if (!entry.sourceFile.empty()) {
            ss << "(" << entry.sourceFile;
            if (entry.lineNumber > 0) {
                ss << ":" << entry.lineNumber;
            }
            ss << ") ";
        }
        
        // Message
        ss << entry.message;
        
        // Metadata
        if (!entry.metadata.empty()) {
            ss << " [";
            bool first = true;
            for (const auto& pair : entry.metadata) {
                if (!first) ss << ", ";
                ss << pair.first << "=" << pair.second;
                first = false;
            }
            ss << "]";
        }
        
        ss << std::endl;
        return ss.str();
    }
    
    std::string logLevelToString(LogLevel level) const {
        switch (level) {
            case LogLevel::Silent: return "SILENT";
            case LogLevel::Error: return "ERROR";
            case LogLevel::Warning: return "WARNING";
            case LogLevel::Info: return "INFO";
            case LogLevel::Debug: return "DEBUG";
            case LogLevel::Trace: return "TRACE";
            default: return "UNKNOWN";
        }
    }
};

// Global debug controller
extern DebugController debugController;

// Assertion framework with detailed error reporting
class AssertionFramework {
public:
    struct AssertionInfo {
        std::string expression;
        std::string message;
        std::string file;
        int line;
        std::chrono::system_clock::time_point timestamp;
        std::string stackTrace;
        std::unordered_map<std::string, std::string> context;
        
        AssertionInfo(const std::string& expr, const std::string& msg, 
                     const std::string& file, int line)
            : expression(expr), message(msg), file(file), line(line), 
              timestamp(std::chrono::system_clock::now()) {}
    };
    
private:
    std::vector<AssertionInfo> assertions;
    bool enableStackTrace;
    bool enableContextCapture;
    
public:
    AssertionFramework() : enableStackTrace(true), enableContextCapture(true) {}
    
    void enableStackTraceCapture(bool enabled) {
        enableStackTrace = enabled;
    }
    
    void enableContextCapture(bool enabled) {
        enableContextCapture = enabled;
    }
    
    void addAssertion(const std::string& expression, const std::string& message,
                     const std::string& file, int line) {
        AssertionInfo info(expression, message, file, line);
        
        if (enableStackTrace) {
            info.stackTrace = captureStackTrace();
        }
        
        assertions.push_back(info);
        
        debugController.logError("Assertion failed: " + expression + " - " + message, file, line);
    }
    
    bool hasFailedAssertions() const {
        for (const auto& assertion : assertions) {
            if (assertion.message.find("failed") != std::string::npos) {
                return true;
            }
        }
        return false;
    }
    
    std::vector<AssertionInfo> getAssertions() const { return assertions; }
    
    std::string getAssertionSummary() const {
        std::stringstream ss;
        ss << "Assertion Summary:" << std::endl;
        ss << "  Total assertions: " << assertions.size() << std::endl;
        
        int failedCount = 0;
        for (const auto& assertion : assertions) {
            if (assertion.message.find("failed") != std::string::npos) {
                failedCount++;
            }
        }
        
        ss << "  Failed assertions: " << failedCount << std::endl;
        
        if (failedCount > 0) {
            ss << std::endl << "Failed Assertions:" << std::endl;
            for (const auto& assertion : assertions) {
                if (assertion.message.find("failed") != std::string::npos) {
                    ss << "  - " << assertion.expression << ": " << assertion.message << std::endl;
                    if (!assertion.stackTrace.empty()) {
                        ss << "    Stack trace: " << assertion.stackTrace << std::endl;
                    }
                }
            }
        }
        
        return ss.str();
    }
    
    std::string captureStackTrace() {
        // Simplified stack trace capture
        return "Stack trace would be captured here";
    }
    
    void addContext(const std::string& key, const std::string& value) {
        if (assertions.empty()) return;
        
        assertions.back().context[key] = value;
    }
};

// Global assertion framework
extern AssertionFramework assertionFramework;

// Resource monitoring utilities
class ResourceMonitor {
public:
    struct ResourceSnapshot {
        std::chrono::system_clock::time_point timestamp;
        size_t memoryUsage;
        size_t peakMemoryUsage;
        double cpuUsage;
        double diskUsage;
        std::thread::id threadId;
        std::unordered_map<std::string, double> customMetrics;
    };
    
private:
    std::vector<ResourceSnapshot> snapshots;
    bool monitoringEnabled;
    std::chrono::milliseconds samplingInterval;
    
public:
    ResourceMonitor() : monitoringEnabled(false), samplingInterval(std::chrono::milliseconds(100)) {}
    
    void startMonitoring() {
        monitoringEnabled = true;
        debugController.logInfo("Resource monitoring started");
    }
    
    void stopMonitoring() {
        monitoringEnabled = false;
        debugController.logInfo("Resource monitoring stopped");
    }
    
    void captureSnapshot() {
        if (!monitoringEnabled) return;
        
        ResourceSnapshot snapshot;
        snapshot.timestamp = std::chrono::system_clock::now();
        snapshot.threadId = std::this_thread::get_id();
        snapshot.memoryUsage = getCurrentMemoryUsage(); // TODO: Implement
        snapshot.peakMemoryUsage = getPeakMemoryUsage(); // TODO: Implement
        snapshot.cpuUsage = getCPUUsage(); // TODO: Implement
        snapshot.diskUsage = getDiskUsage(); // TODO: Implement
        
        snapshots.push_back(snapshot);
    }
    
    std::vector<ResourceSnapshot> getSnapshots() const { return snapshots; }
    
    std::string analyzeSnapshots() const {
        std::stringstream ss;
        ss << "Resource Usage Analysis:" << std::endl;
        
        if (snapshots.empty()) {
            ss << "  No snapshots collected" << std::endl;
            return ss.str();
        }
        
        size_t totalMemory = 0;
        double totalCPU = 0.0;
        size_t maxMemory = 0;
        
        for (const auto& snapshot : snapshots) {
            totalMemory += snapshot.memoryUsage;
            totalCPU += snapshot.cpuUsage;
            maxMemory = std::max(maxMemory, snapshot.memoryUsage);
        }
        
        size_t avgMemory = totalMemory / snapshots.size();
        double avgCPU = totalCPU / snapshots.size();
        
        ss << "  Total snapshots: " << snapshots.size() << std::endl;
        ss << "  Average memory usage: " << avgMemory << " bytes" << std::endl;
        ss << "  Peak memory usage: " << maxMemory << " bytes" << std::endl;
        ss << "  Average CPU usage: " << std::fixed << std::setprecision(2) << avgCPU << "%" << std::endl;
        
        // Detect anomalies
        std::vector<size_t> memoryAnomalies;
        for (size_t i = 1; i < snapshots.size(); ++i) {
            double memoryChange = static_cast<double>(snapshots[i].memoryUsage - snapshots[i-1].memoryUsage) / 
                                snapshots[i-1].memoryUsage * 100.0;
            if (std::abs(memoryChange) > 50.0) { // 50% change threshold
                memoryAnomalies.push_back(i);
            }
        }
        
        if (!memoryAnomalies.empty()) {
            ss << "  Memory anomalies detected at snapshots: ";
            for (size_t i : memoryAnomalies) {
                ss << i << " ";
            }
            ss << std::endl;
        }
        
        return ss.str();
    }
    
    void saveSnapshotsToFile(const std::string& filename) {
        std::ofstream file(filename);
        if (!file) return;
        
        file << "timestamp,memory_usage,peak_memory,cpu_usage,disk_usage,thread_id" << std::endl;
        
        for (const auto& snapshot : snapshots) {
            auto time = std::chrono::system_clock::to_time_t(snapshot.timestamp);
            file << std::put_time(std::localtime(&time), "%Y-%m-%d %H:%M:%S") << ","
                 << snapshot.memoryUsage << ","
                 << snapshot.peakMemoryUsage << ","
                 << std::fixed << std::setprecision(2) << snapshot.cpuUsage << ","
                 << std::fixed << std::setprecision(2) << snapshot.diskUsage << ","
                 << snapshot.threadId << std::endl;
        }
        
        file.close();
        debugController.logInfo("Snapshots saved to " + filename);
    }
    
private:
    // Platform-specific resource measurement (TODO: Implement)
    size_t getCurrentMemoryUsage() { return 0; }
    size_t getPeakMemoryUsage() { return 0; }
    double getCPUUsage() { return 0.0; }
    double getDiskUsage() { return 0.0; }
};

// Global resource monitor
extern ResourceMonitor resourceMonitor;

// Test progress tracking
class ProgressTracker {
public:
    struct ProgressInfo {
        std::string testName;
        std::string description;
        double progress; // 0.0 to 1.0
        std::chrono::system_clock::time_point startTime;
        std::chrono::system_clock::time_point endTime;
        std::string status;
        std::unordered_map<std::string, std::string> metadata;
    };
    
private:
    std::vector<ProgressInfo> progressHistory;
    bool showProgressUI;
    std::chrono::milliseconds updateInterval;
    
public:
    ProgressTracker() : showProgressUI(true), updateInterval(std::chrono::milliseconds(100)) {}
    
    void startTest(const std::string& testName, const std::string& description) {
        ProgressInfo info;
        info.testName = testName;
        info.description = description;
        info.progress = 0.0;
        info.startTime = std::chrono::system_clock::now();
        info.status = "Running";
        
        progressHistory.push_back(info);
        
        if (showProgressUI) {
            std::cout << "Starting test: " << testName << " - " << description << std::endl;
        }
        
        debugController.logInfo("Test started: " + testName);
    }
    
    void updateProgress(const std::string& testName, double progress) {
        for (auto& info : progressHistory) {
            if (info.testName == testName) {
                info.progress = progress;
                if (showProgressUI) {
                    std::cout << "\r" << testName << ": " << std::fixed << std::setprecision(1) << (progress * 100) << "%";
                    std::cout.flush();
                }
                break;
            }
        }
    }
    
    void completeTest(const std::string& testName, const std::string& status) {
        auto endTime = std::chrono::system_clock::now();
        
        for (auto& info : progressHistory) {
            if (info.testName == testName) {
                info.endTime = endTime;
                info.progress = 1.0;
                info.status = status;
                
                if (showProgressUI) {
                    std::cout << std::endl;
                    auto duration = std::chrono::duration_cast<std::chrono::seconds>(endTime - info.startTime);
                    std::cout << "Test completed: " << testName << " (duration: " << duration.count() << "s)" << std::endl;
                }
                
                debugController.logInfo("Test completed: " + testName + " - " + status);
                break;
            }
        }
    }
    
    std::vector<ProgressInfo> getProgressHistory() const { return progressHistory; }
    
    std::string getProgressSummary() const {
        std::stringstream ss;
        ss << "Progress Summary:" << std::endl;
        ss << "  Total tests: " << progressHistory.size() << std::endl;
        
        std::map<std::string, int> statusCounts;
        double totalProgress = 0.0;
        
        for (const auto& info : progressHistory) {
            statusCounts[info.status]++;
            totalProgress += info.progress;
        }
        
        ss << "  Average progress: " << std::fixed << std::setprecision(2) << (totalProgress / progressHistory.size()) << std::endl;
        
        ss << "  Status breakdown:" << std::endl;
        for (const auto& pair : statusCounts) {
            ss << "    " << pair.first << ": " << pair.second << std::endl;
        }
        
        return ss.str();
    }
    
    void reset() {
        progressHistory.clear();
    }
};

// Global progress tracker
extern ProgressTracker progressTracker;

// Test result visualization
class TestVisualizer {
public:
    // Generate HTML test report
    static std::string generateHTMLReport(const std::vector<TestResult>& results,
                                          const TestStatistics& stats) {
        std::stringstream ss;
        ss << "<!DOCTYPE html>" << std::endl;
        ss << "<html><head><title>NLM Test Report</title>" << std::endl;
        ss << "<style>" << std::endl;
        ss << "  body { font-family: Arial, sans-serif; margin: 20px; }" << std::endl;
        ss << "  .test-results { border-collapse: collapse; width: 100%; }" << std::endl;
        ss << "  .test-results th, .test-results td { border: 1px solid #ddd; padding: 8px; text-align: left; }" << std::endl;
        ss << "  .test-results th { background-color: #f2f2f2; }" << std::endl;
        ss << "  .passed { background-color: #d4edda; }" << std::endl;
        ss << "  .failed { background-color: #f8d7da; }" << std::endl;
        ss << "  .skipped { background-color: #fff3cd; }" << std::endl;
        ss << "  .statistics { margin: 20px 0; padding: 20px; background-color: #f8f9fa; border-radius: 5px; }" << std::endl;
        ss << "  .progress-bar { background-color: #e9ecef; border-radius: 5px; height: 20px; margin: 10px 0; }" << std::endl;
        ss << "  .progress-fill { background-color: #28a745; height: 100%; border-radius: 5px; }" << std::endl;
        ss << "</style>" << std::endl;
        ss << "</head><body>" << std::endl;
        ss << "<h1>NLM Test Report</h1>" << std::endl;
        ss << "<p>Generated at: " << std::chrono::system_clock::now() << "</p>" << std::endl;
        ss << "<div class=\"statistics\">" << std::endl;
        ss << "<h2>Test Statistics</h2>" << std::endl;
        ss << "<p>Total Tests: " << stats.totalTests << "</p>" << std::endl;
        ss << "<p>Passed: " << stats.passedTests << "</p>" << std::endl;
        ss << "<p>Failed: " << stats.failedTests << "</p>" << std::endl;
        ss << "<p>Skipped: " << stats.skippedTests << "</p>" << std::endl;
        ss << "<p>Timeout: " << stats.timeoutTests << "</p>" << std::endl;
        ss << "<p>Pass Rate: " << std::fixed << std::setprecision(2) << stats.passRate() << "%</p>" << std::endl;
        ss << "<p>Total Execution Time: " << 
              std::chrono::duration_cast<std::chrono::milliseconds>(stats.totalExecutionTime).count() << "ms</p>" << std::endl;
        ss << "<div class=\"progress-bar\">" << std::endl;
        ss << "<div class=\"progress-fill\" style=\"width: " << stats.passRate() << "%\"></div>" << std::endl;
        ss << "</div>" << std::endl;
        ss << "</div>" << std::endl;
        ss << "<h2>Test Results</h2>" << std::endl;
        ss << "<table class=\"test-results\">" << std::endl;
        ss << "<tr><th>Test Name</th><th>Status</th><th>Category</th><th>Execution Time (ns)</th><th>Message</th></tr>" << std::endl;
        
        for (const auto& result : results) {
            std::string rowClass;
            if (result.status == TestStatus::Passed) {
                rowClass = "passed";
            } else if (result.status == TestStatus::Failed) {
                rowClass = "failed";
            } else if (result.status == TestStatus::Skipped) {
                rowClass = "skipped";
            }
            
            ss << "<tr class=\"" << rowClass << "\">" << std::endl;
            ss << "<td>" << result.name << "</td>" << std::endl;
            ss << "<td>" << testStatusToString(result.status) << "</td>" << std::endl;
            ss << "<td>" << static_cast<int>(result.category) << "</td>" << std::endl;
            ss << "<td>" << result.executionTime.count() << "</td>" << std::endl;
            ss << "<td>" << result.message << "</td>" << std::endl;
            ss << "</tr>" << std::endl;
        }
        
        ss << "</table>" << std::endl;
        ss << "</body></html>" << std::endl;
        
        return ss.str();
    }
    
    // Generate JSON test report
    static std::string generateJSONReport(const std::vector<TestResult>& results,
                                          const TestStatistics& stats) {
        std::stringstream ss;
        ss << "{" << std::endl;
        ss << "  \"timestamp\": \"" << std::chrono::system_clock::now() << "\"," << std::endl;
        ss << "  \"statistics\": {" << std::endl;
        ss << "    \"totalTests\": " << stats.totalTests << "," << std::endl;
        ss << "    \"passedTests\": " << stats.passedTests << "," << std::endl;
        ss << "    \"failedTests\": " << stats.failedTests << "," << std::endl;
        ss << "    \"skippedTests\": " << stats.skippedTests << "," << std::endl;
        ss << "    \"timeoutTests\": " << stats.timeoutTests << "," << std::endl;
        ss << "    \"passRate\": " << std::fixed << std::setprecision(2) << stats.passRate() << "," << std::endl;
        ss << "    \"totalExecutionTime\": " << stats.totalExecutionTime.count() << "," << std::endl;
        ss << "    \"totalSetupTime\": " << stats.totalSetupTime.count() << "," << std::endl;
        ss << "    \"totalTeardownTime\": " << stats.totalTeardownTime.count() << "," << std::endl;
        ss << "    \"memoryLeaks\": " << stats.memoryLeaks << "," << std::endl;
        ss << "    \"memoryUsageGB\": " << std::fixed << std::setprecision(2) << stats.memoryUsageGB << std::endl;
        ss << "  }," << std::endl;
        ss << "  \"testResults\": [" << std::endl;
        
        for (size_t i = 0; i < results.size(); ++i) {
            const auto& result = results[i];
            ss << "    {" << std::endl;
            ss << "      \"name\": \"" << result.name << "\"," << std::endl;
            ss << "      \"status\": " << static_cast<int>(result.status) << "," << std::endl;
            ss << "      \"category\": " << static_cast<int>(result.category) << "," << std::endl;
            ss << "      \"executionTime\": " << result.executionTime.count() << "," << std::endl;
            ss << "      \"setupTime\": " << result.setupTime.count() << "," << std::endl;
            ss << "      \"teardownTime\": " << result.teardownTime.count() << "," << std::endl;
            ss << "      \"memoryDelta\": " << result.memoryDelta << "," << std::endl;
            ss << "      \"peakMemoryUsage\": " << result.peakMemoryUsage << "," << std::endl;
            ss << "      \"message\": \"" << result.message << "\"," << std::endl;
            ss << "      \"file\": \"" << result.file << "\"," << std::endl;
            ss << "      \"line\": " << result.line << std::endl;
            
            if (i < results.size() - 1) {
                ss << "    },";
            } else {
                ss << "    }";
            }
            
            ss << std::endl;
        }
        
        ss << "  ]" << std::endl;
        ss << "}" << std::endl;
        
        return ss.str();
    }
};

}}} // namespace nlm::test
