#pragma once

#include <string>
#include <chrono>
#include <memory>
#include <vector>
#include <unordered_map>
#include <mutex>
#include <thread>
#include <iomanip>
#include <sstream>
#include <cstdint>
#include <fstream>

namespace nlm {

    class DebugLogger {
    public:
        struct LogEntry {
            std::chrono::system_clock::time_point timestamp;
            LogLevel level;
            std::string threadId;
            std::string file;
            int line;
            std::string function;
            std::string message;
            std::unordered_map<std::string, std::string> context;
            uint64_t threadIdHash;
            double memoryUsageMB;
            double cpuUsagePercent;
        };

        enum class LogLevel {
            DEBUG = 0,
            INFO = 1,
            WARNING = 2,
            ERROR = 3,
            CRITICAL = 4
        };

        struct PerformanceMetric {
            std::string name;
            double value;
            double timestamp;
            std::string unit;
        };

        struct MemoryUsage {
            double totalMemoryMB;
            double usedMemoryMB;
            double availableMemoryMB;
            double memoryUsagePercent;
            std::chrono::system_clock::time_point timestamp;
        };

        struct ThreadInfo {
            std::thread::id id;
            std::string name;
            double cpuUsagePercent;
            uint64_t threadId;
        };

    public:
        DebugLogger();
        explicit DebugLogger(const std::string& logFile);
        ~DebugLogger();

        // Thread-safe logging methods
        void log(LogLevel level, const std::string& message,
                 const std::string& file = "", int line = 0,
                 const std::string& function = "", bool isError = false);

        void debug(const std::string& message,
                   const std::string& file = "", int line = 0,
                   const std::string& function = "");

        void info(const std::string& message,
                  const std::string& file = "", int line = 0,
                  const std::string& function = "");

        void warning(const std::string& message,
                     const std::string& file = "", int line = 0,
                     const std::string& function = "");

        void error(const std::string& message,
                   const std::string& file = "", int line = 0,
                   const std::string& function = "");

        void critical(const std::string& message,
                      const std::string& file = "", int line = 0,
                      const std::string& function = "");

        // Performance timing
        void startTiming(const std::string& operationName);
        double endTiming(const std::string& operationName);
        double getAverageTime(const std::string& operationName) const;
        std::unordered_map<std::string, std::vector<double>> getAllTimings() const;

        // Memory tracking
        void trackMemoryUsage();
        MemoryUsage getCurrentMemoryUsage() const;
        std::vector<MemoryUsage> getMemoryHistory() const;

        // Structured logging
        void logWithContext(LogLevel level, const std::string& message,
                           const std::unordered_map<std::string, std::string>& context,
                           const std::string& file = "", int line = 0,
                           const std::string& function = "");

        // Thread-safe operations
        void setThreadName(const std::string& name);
        std::string getCurrentThreadId() const;
        std::vector<ThreadInfo> getActiveThreads() const;

        // File operations
        void setLogFile(const std::string& filepath);
        void closeLogFile();
        bool isLogFileOpen() const;

        // Query operations
        std::vector<LogEntry> getRecentLogs(size_t limit = 100) const;
        std::vector<LogEntry> getLogsByLevel(LogLevel level, size_t limit = 100) const;
        std::string getLogSummary() const;

        // Performance metrics collection
        void recordPerformanceMetric(const std::string& name, double value,
                                     const std::string& unit = "", double timestamp = 0.0);
        std::vector<PerformanceMetric> getPerformanceMetrics(const std::string& name = "") const;
        double getPerformanceMetricAverage(const std::string& name) const;

        // Error tracking
        void recordError(const std::string& errorMessage, const std::string& errorType = "UNKNOWN",
                        const std::unordered_map<std::string, std::string>& errorContext = {});
        std::vector<LogEntry> getRecentErrors(size_t limit = 50) const;
        std::string getErrorReport() const;

        // Memory checkpoint
        void takeMemoryCheckpoint(const std::string& checkpointName = "default");
        std::vector<MemoryUsage> getMemoryCheckpoints(const std::string& checkpointName = "") const;
        double compareMemoryUsage(const std::string& checkpoint1, const std::string& checkpoint2) const;

        // Bottleneck detection
        void analyzePerformanceBottlenecks();
        std::string getBottleneckReport() const;

        // Export
        std::string exportLogsToJSON() const;
        std::string exportMetricsToCSV() const;

    private:
        struct Impl;
        std::unique_ptr<Impl> pImpl;

        // Helper methods
        std::string formatTimestamp(const std::chrono::system_clock::time_point& timestamp) const;
        std::string levelToString(LogLevel level) const;
        uint64_t getCurrentThreadId() const;
        double getCurrentMemoryUsageMB() const;
        double getCurrentCpuUsagePercent() const;
    };

} // namespace nlm
