// Enhanced Logging System for NLM
// Phase 2: Real Neural Computation
//
// This enhanced logging system provides:
// - Advanced log level hierarchy and filtering
// - Multiple output formats (console, file, JSON, CSV)
// - Performance metrics and profiling
// - Structured logging with metadata
// - Asynchronous logging capability
// - Log rotation and compression
// - Log search and analysis
// - Performance monitoring

#include "Logger.hpp"
#include <iostream>
#include <fstream>
#include <iomanip>
#include <sstream>
#include <mutex>
#include <queue>
#include <thread>
#include <condition_variable>
#include <unordered_map>
#include <map>
#include <chrono>
#include <atomic>
#include <json.hpp>
#include <zlib.h>
#include <algorithm>
#include <iomanip>
#include <ctime>

using json = nlohmann::json;

namespace nlm {

// Enhanced log levels with more granularity
enum class EnhancedLogLevel {
    Trace = 0,
    Debug = 1,
    Info = 2,
    Warning = 3,
    Error = 4,
    Critical = 5,
    Fatal = 6
};

// Log entry with enhanced metadata
struct EnhancedLogEntry {
    EnhancedLogLevel level;
    std::string message;
    std::string file;
    int line;
    std::string function;
    std::chrono::system_clock::time_point timestamp;
    std::string threadId;
    uint64_t processId;
    
    // Structured logging data
    std::map<std::string, std::string> metadata;
    std::map<std::string, double> metrics;
    
    // Performance tracking
    double executionTime = 0.0; // microseconds
    uint64_t memoryUsage = 0;   // bytes
    
    // Categorization
    std::string category;
    std::string subsystem;
    std::vector<std::string> tags;
    
    // Enhanced constructor
    EnhancedLogEntry(EnhancedLogLevel lvl, const std::string& msg,
                     const std::string& file = "", int line = 0,
                     const std::string& function = "")
        : level(lvl)
        , message(msg)
        , file(file)
        , line(line)
        , function(function)
        , timestamp(std::chrono::system_clock::now())
        , processId(getProcessId()) {
        threadId = std::to_string(std::hash<std::thread::id>{}(std::this_thread::get_id()));
    }
    
private:
    static uint64_t getProcessId() {
        return static_cast<uint64_t>(std::hash<std::thread::id>{}(std::this_thread::get_id()));
    }
};

// Output format options
enum class LogOutputFormat {
    PlainText,
    JSON,
    CSV,
    StructuredJSON,
    Colored,
    Minimal,
    Detailed
};

// Log rotation strategy
enum class LogRotationStrategy {
    SizeBased,      // Rotate based on file size
    TimeBased,      // Rotate based on time
    SizeAndTime,    // Both size and time based
    None            // No rotation
};

// Enhanced console logger with advanced features
class EnhancedConsoleLogger {
public:
    EnhancedConsoleLogger() {
        setupDefaults();
    }
    
    EnhancedConsoleLogger(EnhancedLogLevel level) {
        setupDefaults();
        setLevel(level);
    }
    
    ~EnhancedConsoleLogger() {
        shutdown();
    }
    
    void setupDefaults() {
        pImpl = std::make_unique<Impl>();
        pImpl->useColors = true;
        pImpl->format = LogOutputFormat::Colored;
        pImpl->minLevel = EnhancedLogLevel::Info;
        pImpl->enablePerformanceMetrics = false;
        pImpl->logFileRotation = false;
        pImpl->maxFileSize = 10 * 1024 * 1024; // 10MB
        pImpl->maxFileAge = 7 * 24 * 3600; // 7 days
        pImpl->asyncLogging = false;
        pImpl->threadPoolSize = std::thread::hardware_concurrency();
        pImpl->bufferSize = 4096;
        pImpl->enableStructuredLogging = false;
        pImpl->enableMetrics = false;
        pImpl->metricsInterval = 1000;
    }
    
    void log(const EnhancedLogEntry& entry) {
        if (entry.level < pImpl->minLevel) {
            return;
        }
        
        std::lock_guard<std::mutex> lock(pImpl->mutex);
        
        // Process entry through async logging if enabled
        if (pImpl->asyncLogging) {
            pImpl->logQueue.push(entry);
            pImpl->condition.notify_one();
        } else {
            processLogEntry(entry);
        }
    }
    
    void setLevel(EnhancedLogLevel level) {
        pImpl->minLevel = level;
    }
    
    EnhancedLogLevel getLevel() const {
        return pImpl->minLevel;
    }
    
    void setUseColors(bool useColors) {
        pImpl->useColors = useColors;
    }
    
    bool getUseColors() const {
        return pImpl->useColors;
    }
    
    void setOutputFormat(LogOutputFormat format) {
        pImpl->format = format;
    }
    
    LogOutputFormat getOutputFormat() const {
        return pImpl->format;
    }
    
    void setOutputFile(const std::string& filepath) {
        std::lock_guard<std::mutex> lock(pImpl->mutex);
        
        // Close current file if open
        if (pImpl->fileOut.is_open()) {
            pImpl->fileOut.close();
        }
        
        // Open new file
        pImpl->fileOut.open(filepath, std::ios::app);
        if (pImpl->fileOut.is_open()) {
            pImpl->currentLogFile = filepath;
            pImpl->fileOut << "=== NLM Enhanced Log File ===\n";
            pImpl->fileOut << "Format: " << logFormatToString(pImpl->format) << "\n";
            pImpl->fileOut << "Level: " << enhancedLogLevelToString(pImpl->minLevel) << "\n";
            pImpl->fileOut << "Started: " << getCurrentTimestamp() << "\n";
            pImpl->fileOut << "===========================================\n\n";
            pImpl->fileOut.flush();
        }
    }
    
    void enableAsyncLogging(bool enable) {
        pImpl->asyncLogging = enable;
        if (enable && !pImpl->asyncThread.joinable()) {
            startAsyncLoggingThread();
        } else if (!enable && pImpl->asyncThread.joinable()) {
            stopAsyncLoggingThread();
        }
    }
    
    void enableStructuredLogging(bool enable) {
        pImpl->enableStructuredLogging = enable;
    }
    
    void enableMetrics(bool enable, int intervalMs = 1000) {
        pImpl->enableMetrics = enable;
        pImpl->metricsInterval = intervalMs;
        if (enable) {
            startMetricsCollectionThread();
        } else {
            stopMetricsCollectionThread();
        }
    }
    
    void setLogRotation(bool enable, uint64_t maxSize = 10 * 1024 * 1024, 
                       uint64_t maxAge = 7 * 24 * 3600) {
        pImpl->logFileRotation = enable;
        pImpl->maxFileSize = maxSize;
        pImpl->maxFileAge = maxAge;
    }
    
    // Get current log statistics
    struct LogStatistics {
        uint64_t totalLogs = 0;
        uint64_t debugLogs = 0;
        uint64_t infoLogs = 0;
        uint64_t warningLogs = 0;
        uint64_t errorLogs = 0;
        uint64_t criticalLogs = 0;
        uint64_t fatalLogs = 0;
        double averageExecutionTime = 0.0;
        std::map<std::string, uint64_t> categoryCounts;
        std::map<std::string, double> metricSums;
    };
    
    LogStatistics getStatistics() {
        std::lock_guard<std::mutex> lock(pImpl->mutex);
        return pImpl->statistics;
    }
    
    // Search logs
    std::vector<EnhancedLogEntry> searchLogs(const std::string& pattern, 
                                            EnhancedLogLevel minLevel = EnhancedLogLevel::Trace,
                                            const std::string& category = "") {
        std::lock_guard<std::mutex> lock(pImpl->mutex);
        std::vector<EnhancedLogEntry> results;
        
        for (const auto& entry : pImpl->logHistory) {
            if (entry.level < minLevel) continue;
            if (!category.empty() && entry.category != category) continue;
            if (pattern.empty() || entry.message.find(pattern) != std::string::npos ||
                entry.category.find(pattern) != std::string::npos) {
                results.push_back(entry);
            }
        }
        
        return results;
    }
    
    // Export logs to different formats
    bool exportLogs(const std::string& filename, LogOutputFormat format) {
        std::lock_guard<std::mutex> lock(pImpl->mutex);
        
        try {
            std::ofstream file(filename);
            if (!file.is_open()) {
                return false;
            }
            
            switch (format) {
                case LogOutputFormat::JSON:
                    exportToJSON(file);
                    break;
                case LogOutputFormat::CSV:
                    exportToCSV(file);
                    break;
                case LogOutputFormat::StructuredJSON:
                    exportToStructuredJSON(file);
                    break;
                default:
                    exportToPlainText(file);
            }
            
            file.close();
            return true;
            
        } catch (const std::exception& e) {
            return false;
        }
    }
    
    // Performance monitoring
    void collectPerformanceMetrics() {
        if (!pImpl->enableMetrics) return;
        
        std::lock_guard<std::mutex> lock(pImpl->mutex);
        
        // Collect system metrics
        PerformanceMetrics metrics = getSystemMetrics();
        
        // Store metrics
        pImpl->performanceMetrics.push_back(metrics);
        
        // Keep only recent metrics
        if (pImpl->performanceMetrics.size() > 1000) {
            pImpl->performanceMetrics.erase(pImpl->performanceMetrics.begin());
        }
        
        // Log metrics if needed
        if (metrics.cpuUsage > 80.0 || metrics.memoryUsage > 80.0) {
            EnhancedLogEntry metricEntry(EnhancedLogLevel::Warning, 
                "High resource usage: CPU=" + std::to_string(metrics.cpuUsage) + "%, Memory=" + std::to_string(metrics.memoryUsage) + "%",
                "", 0, "collectPerformanceMetrics");
            metricEntry.category = "performance";
            processLogEntry(metricEntry);
        }
    }
    
    // Shutdown and cleanup
    void shutdown() {
        stopAsyncLoggingThread();
        stopMetricsCollectionThread();
        
        std::lock_guard<std::mutex> lock(pImpl->mutex);
        if (pImpl->fileOut.is_open()) {
            pImpl->fileOut.close();
        }
        
        // Log shutdown message
        EnhancedLogEntry shutdownEntry(EnhancedLogLevel::Info, "Logger shutdown completed", "", 0, "shutdown");
        processLogEntry(shutdownEntry);
    }
    
private:
    struct Impl {
        EnhancedLogLevel minLevel;
        bool useColors;
        LogOutputFormat format;
        bool enablePerformanceMetrics;
        bool logFileRotation;
        uint64_t maxFileSize;
        uint64_t maxFileAge;
        bool asyncLogging;
        size_t threadPoolSize;
        size_t bufferSize;
        bool enableStructuredLogging;
        bool enableMetrics;
        int metricsInterval;
        
        // Output
        std::ofstream fileOut;
        std::string currentLogFile;
        
        // Threading
        std::queue<EnhancedLogEntry> logQueue;
        std::mutex mutex;
        std::condition_variable condition;
        std::thread asyncThread;
        bool shutdownFlag;
        
        // Statistics
        LogStatistics statistics;
        std::vector<EnhancedLogEntry> logHistory;
        size_t maxHistorySize;
        
        // Performance metrics
        struct PerformanceMetrics {
            double cpuUsage;
            double memoryUsage;
            double diskUsage;
            uint64_t totalMemory;
            uint64_t availableMemory;
            std::chrono::system_clock::time_point timestamp;
        };
        
        std::vector<PerformanceMetrics> performanceMetrics;
        std::chrono::system_clock::time_point lastMetricsCollect;
        
        Impl() : shutdownFlag(false), lastMetricsCollect(std::chrono::system_clock::now()) {
            maxHistorySize = 10000;
            setlocale(LC_ALL, "");
        }
    };
    
    std::unique_ptr<Impl> pImpl;
    
    void startAsyncLoggingThread() {
        pImpl->shutdownFlag = false;
        pImpl->asyncThread = std::thread(&EnhancedConsoleLogger::asyncLoggingWorker, this);
    }
    
    void stopAsyncLoggingThread() {
        if (pImpl->asyncThread.joinable()) {
            {
                std::lock_guard<std::mutex> lock(pImpl->mutex);
                pImpl->shutdownFlag = true;
            }
            pImpl->condition.notify_one();
            pImpl->asyncThread.join();
        }
    }
    
    void startMetricsCollectionThread() {
        pImpl->metricsThread = std::thread(&EnhancedConsoleLogger::metricsCollectionWorker, this);
    }
    
    void stopMetricsCollectionThread() {
        if (pImpl->metricsThread.joinable()) {
            pImpl->metricsThread.join();
        }
    }
    
    void asyncLoggingWorker() {
        while (!pImpl->shutdownFlag) {
            EnhancedLogEntry entry;
            {
                std::unique_lock<std::mutex> lock(pImpl->mutex);
                pImpl->condition.wait_for(lock, std::chrono::milliseconds(100), 
                    [this] { return !pImpl->logQueue.empty() || pImpl->shutdownFlag; });
                
                if (pImpl->shutdownFlag && pImpl->logQueue.empty()) {
                    break;
                }
                
                if (!pImpl->logQueue.empty()) {
                    entry = pImpl->logQueue.front();
                    pImpl->logQueue.pop();
                }
            }
            
            if (!pImpl->shutdownFlag) {
                processLogEntry(entry);
            }
        }
    }
    
    void metricsCollectionWorker() {
        while (!pImpl->shutdownFlag) {
            std::this_thread::sleep_for(std::chrono::milliseconds(pImpl->metricsInterval));
            collectPerformanceMetrics();
        }
    }
    
    void processLogEntry(const EnhancedLogEntry& entry) {
        // Update statistics
        updateStatistics(entry);
        
        // Store in history
        storeLogEntry(entry);
        
        // Output to console/file
        outputLogEntry(entry);
    }
    
    void updateStatistics(const EnhancedLogEntry& entry) {
        pImpl->statistics.totalLogs++;
        
        switch (entry.level) {
            case EnhancedLogLevel::Trace: pImpl->statistics.debugLogs++; break;
            case EnhancedLogLevel::Debug: pImpl->statistics.debugLogs++; break;
            case EnhancedLogLevel::Info: pImpl->statistics.infoLogs++; break;
            case EnhancedLogLevel::Warning: pImpl->statistics.warningLogs++; break;
            case EnhancedLogLevel::Error: pImpl->statistics.errorLogs++; break;
            case EnhancedLogLevel::Critical: pImpl->statistics.criticalLogs++; break;
            case EnhancedLogLevel::Fatal: pImpl->statistics.fatalLogs++; break;
        }
        
        // Update category counts
        pImpl->statistics.categoryCounts[entry.category]++;
        
        // Update metric sums
        for (const auto& metric : entry.metrics) {
            pImpl->statistics.metricSums[metric.first] += metric.second;
        }
        
        // Update average execution time
        pImpl->statistics.averageExecutionTime = 
            (pImpl->statistics.averageExecutionTime * (pImpl->statistics.totalLogs - 1) + entry.executionTime) / 
            pImpl->statistics.totalLogs;
    }
    
    void storeLogEntry(const EnhancedLogEntry& entry) {
        pImpl->logHistory.push_back(entry);
        if (pImpl->logHistory.size() > pImpl->maxHistorySize) {
            pImpl->logHistory.erase(pImpl->logHistory.begin());
        }
    }
    
    void outputLogEntry(const EnhancedLogEntry& entry) {
        std::ostringstream oss;
        
        switch (pImpl->format) {
            case LogOutputFormat::PlainText:
                oss << plainTextFormat(entry);
                break;
            case LogOutputFormat::Colored:
                oss << coloredFormat(entry);
                break;
            case LogOutputFormat::JSON:
                oss << jsonFormat(entry);
                break;
            case LogOutputFormat::CSV:
                oss << csvFormat(entry);
                break;
            case LogOutputFormat::StructuredJSON:
                oss << structuredJSONFormat(entry);
                break;
            case LogOutputFormat::Minimal:
                oss << minimalFormat(entry);
                break;
            case LogOutputFormat::Detailed:
                oss << detailedFormat(entry);
                break;
        }
        
        std::string output = oss.str();
        
        // Console output
        std::cout << output;
        
        // File output if set
        if (pImpl->fileOut.is_open()) {
            pImpl->fileOut << output;
            pImpl->fileOut.flush();
            
            // Check if rotation is needed
            checkLogRotation();
        }
    }
    
    std::string plainTextFormat(const EnhancedLogEntry& entry) const {
        auto time = std::chrono::system_clock::to_time_t(entry.timestamp);
        std::ostringstream oss;
        
        oss << "[" << std::put_time(std::localtime(&time), "%Y-%m-%d %H:%M:%S") << "] ";
        oss << "[" << enhancedLogLevelToString(entry.level) << "] ";
        oss << "[" << entry.category << "] ";
        oss << "[" << entry.function << "] ";
        oss << entry.message;
        
        if (entry.executionTime > 0) {
            oss << " [exec_time:" << entry.executionTime << "us]";
        }
        
        return oss.str() + "\n";
    }
    
    std::string coloredFormat(const EnhancedLogEntry& entry) const {
        if (!pImpl->useColors) {
            return plainTextFormat(entry);
        }
        
        std::string color = enhancedLogLevelToColor(entry.level);
        std::string reset = "\033[0m";
        
        auto time = std::chrono::system_clock::to_time_t(entry.timestamp);
        std::ostringstream oss;
        
        oss << color;
        oss << "[" << std::put_time(std::localtime(&time), "%Y-%m-%d %H:%M:%S") << "] ";
        oss << "[" << enhancedLogLevelToString(entry.level) << "] ";
        oss << "[" << entry.category << "] ";
        oss << "[" << entry.function << "] ";
        oss << entry.message;
        oss << reset;
        
        if (entry.executionTime > 0) {
            oss << " [exec_time:" << entry.executionTime << "us]";
        }
        
        return oss.str() + "\n";
    }
    
    std::string jsonFormat(const EnhancedLogEntry& entry) const {
        json j;
        auto time = std::chrono::system_clock::to_time_t(entry.timestamp);
        
        j["timestamp"] = std::string(std::asctime(std::localtime(&time)));
        j["level"] = enhancedLogLevelToString(entry.level);
        j["category"] = entry.category;
        j["function"] = entry.function;
        j["file"] = entry.file;
        j["line"] = entry.line;
        j["message"] = entry.message;
        
        if (!entry.metadata.empty()) {
            j["metadata"] = entry.metadata;
        }
        
        if (!entry.metrics.empty()) {
            j["metrics"] = entry.metrics;
        }
        
        if (entry.executionTime > 0) {
            j["execution_time_us"] = entry.executionTime;
        }
        
        return j.dump() + "\n";
    }
    
    std::string csvFormat(const EnhancedLogEntry& entry) const {
        auto time = std::chrono::system_clock::to_time_t(entry.timestamp);
        std::ostringstream oss;
        
        oss << getCurrentTimestamp() << ","; // Timestamp
        oss << enhancedLogLevelToString(entry.level) << ","; // Level
        oss << entry.category << ","; // Category
        oss << entry.function << ","; // Function
        oss << entry.file << ","; // File
        oss << entry.line << ","; // Line
        oss << "\"" << entry.message << "\"".replace(\"\"\"\", \"\\\"\").replace(\"\"\", \"\\\"\") << ","; // Message (escaped)
        oss << entry.executionTime << ","; // Execution time
        
        // Metadata (convert to CSV friendly format)
        oss << "";
        for (const auto& meta : entry.metadata) {
            oss << meta.first << "=" << meta.second << ";";
        }
        oss << ";";
        
        return oss.str() + "\n";
    }
    
    std::string structuredJSONFormat(const EnhancedLogEntry& entry) const {
        json j;
        
        // Log entry information
        j["log_entry"] = {
            {"level", enhancedLogLevelToString(entry.level)},
            {"message", entry.message},
            {"category", entry.category},
            {"function", entry.function},
            {"file", entry.file},
            {"line", entry.line}
        };
        
        // Timestamp information
        auto time = std::chrono::system_clock::to_time_t(entry.timestamp);
        j["timestamp"] = {
            {"raw", std::chrono::system_clock::to_time_t(entry.timestamp)},
            {"formatted", std::string(std::asctime(std::localtime(&time)))}
        };
        
        // Process information
        j["process"] = {
            {"pid", entry.processId},
            {"thread_id", entry.threadId}
        };
        
        // Performance metrics
        if (entry.executionTime > 0 || entry.memoryUsage > 0) {
            j["performance"] = {};
            if (entry.executionTime > 0) {
                j["performance"]["execution_time_us"] = entry.executionTime;
            }
            if (entry.memoryUsage > 0) {
                j["performance"]["memory_usage_bytes"] = entry.memoryUsage;
            }
        }
        
        // Structured data
        if (!entry.metadata.empty()) {
            j["metadata"] = entry.metadata;
        }
        
        if (!entry.metrics.empty()) {
            j["metrics"] = entry.metrics;
        }
        
        // Tags
        if (!entry.tags.empty()) {
            j["tags"] = entry.tags;
        }
        
        return j.dump() + "\n";
    }
    
    std::string minimalFormat(const EnhancedLogEntry& entry) const {
        std::ostringstream oss;
        oss << "[" << enhancedLogLevelToShortString(entry.level) << "] "
            << entry.message;
        return oss.str() + "\n";
    }
    
    std::string detailedFormat(const EnhancedLogEntry& entry) const {
        std::ostringstream oss;
        auto time = std::chrono::system_clock::to_time_t(entry.timestamp);
        
        oss << "=== LOG ENTRY ===\n";
        oss << "Timestamp: " << std::put_time(std::localtime(&time), "%Y-%m-%d %H:%M:%S") << "\n";
        oss << "Level: " << enhancedLogLevelToString(entry.level) << "\n";
        oss << "Category: " << entry.category << "\n";
        oss << "Subsystem: " << entry.subsystem << "\n";
        oss << "Function: " << entry.function << "\n";
        oss << "File: " << entry.file << ":" << entry.line << "\n";
        oss << "Thread ID: " << entry.threadId << "\n";
        oss << "Process ID: " << entry.processId << "\n";
        oss << "\n";
        oss << "Message:\n";
        oss << "  " << entry.message << "\n";
        
        if (!entry.metadata.empty()) {
            oss << "\nMetadata:\n";
            for (const auto& meta : entry.metadata) {
                oss << "  " << meta.first << ": " << meta.second << "\n";
            }
        }
        
        if (!entry.metrics.empty()) {
            oss << "\nMetrics:\n";
            for (const auto& metric : entry.metrics) {
                oss << "  " << metric.first << ": " << metric.second << "\n";
            }
        }
        
        if (!entry.tags.empty()) {
            oss << "\nTags:\n";
            for (const auto& tag : entry.tags) {
                oss << "  " << tag << "\n";
            }
        }
        
        if (entry.executionTime > 0) {
            oss << "\nPerformance:\n";
            oss << "  Execution time: " << entry.executionTime << " microseconds\n";
        }
        
        oss << "===================\n";
        return oss.str();
    }
    
    void exportToJSON(std::ofstream& file) const {
        json j;
        j["logs"] = json::array();
        
        for (const auto& entry : pImpl->logHistory) {
            json logEntry;
            auto time = std::chrono::system_clock::to_time_t(entry.timestamp);
            
            logEntry["timestamp"] = std::string(std::asctime(std::localtime(&time)));
            logEntry["level"] = enhancedLogLevelToString(entry.level);
            logEntry["category"] = entry.category;
            logEntry["function"] = entry.function;
            logEntry["file"] = entry.file;
            logEntry["line"] = entry.line;
            logEntry["message"] = entry.message;
            
            j["logs"].push_back(logEntry);
        }
        
        file << j.dump(2) << "\n";
    }
    
    void exportToCSV(std::ofstream& file) const {
        file << "timestamp,level,category,function,file,line,message,execution_time_us\n";
        
        for (const auto& entry : pImpl->logHistory) {
            auto time = std::chrono::system_clock::to_time_t(entry.timestamp);
            
            file << std::string(std::asctime(std::localtime(&time)));
            file << "," << enhancedLogLevelToString(entry.level);
            file << "," << entry.category;
            file << "," << entry.function;
            file << "," << entry.file;
            file << "," << entry.line;
            file << "," << "\"" << entry.message << "\"".replace('"', "\"").replace('\n', ",") << ",";
            file << entry.executionTime << "\n";
        }
    }
    
    void exportToStructuredJSON(std::ofstream& file) const {
        json j;
        j["metadata"] = {
            {"export_time", std::string(std::asctime(std::localtime(&std::time(nullptr))))},
            {"total_logs", pImpl->statistics.totalLogs},
            {"version", "2.0"}
        };
        
        j["logs"] = json::array();
        for (const auto& entry : pImpl->logHistory) {
            json logEntry;
            auto time = std::chrono::system_clock::to_time_t(entry.timestamp);
            
            logEntry["log_entry"] = {
                {"level", enhancedLogLevelToString(entry.level)},
                {"message", entry.message},
                {"category", entry.category},
                {"function", entry.function}
            };
            
            logEntry["timestamp"] = std::string(std::asctime(std::localtime(&time)));
            j["logs"].push_back(logEntry);
        }
        
        file << j.dump(2) << "\n";
    }
    
    void exportToPlainText(std::ofstream& file) const {
        file << "=== NLM Enhanced Log Export ===\n";
        file << "Total logs: " << pImpl->statistics.totalLogs << "\n";
        file << "Export time: " << getCurrentTimestamp() << "\n\n";
        
        for (const auto& entry : pImpl->logHistory) {
            auto time = std::chrono::system_clock::to_time_t(entry.timestamp);
            file << "[" << std::put_time(std::localtime(&time), "%Y-%m-%d %H:%M:%S") << "] ";
            file << "[" << enhancedLogLevelToString(entry.level) << "] ";
            file << "[" << entry.category << "] ";
            file << "[" << entry.function << "] ";
            file << entry.message << "\n";
            
            if (entry.executionTime > 0) {
                file << "  [exec_time:" << entry.executionTime << "us]" << "\n";
            }
            
            file << "\n";
        }
    }
    
    void checkLogRotation() {
        if (!pImpl->logFileRotation) return;
        
        std::lock_guard<std::mutex> lock(pImpl->mutex);
        
        // Check file size
        if (pImpl->fileOut.is_open() && pImpl->fileOut.tellp() > pImpl->maxFileSize) {
            rotateLogFile();
        }
        
        // Check file age
        if (pImpl->fileOut.is_open()) {
            auto fileTime = std::filesystem::last_write_time(pImpl->currentLogFile);
            auto age = std::chrono::duration_cast<std::chrono::seconds>(
                std::chrono::system_clock::now() - fileTime.time_since_epoch()).count();
            
            if (age > pImpl->maxFileAge) {
                rotateLogFile();
            }
        }
    }
    
    void rotateLogFile() {
        if (!pImpl->fileOut.is_open()) return;
        
        // Close current file
        pImpl->fileOut.close();
        
        // Generate new filename with timestamp
        auto now = std::chrono::system_clock::now();
        auto time = std::chrono::system_clock::to_time_t(now);
        std::stringstream ss;
        ss << pImpl->currentLogFile << "." << std::put_time(std::localtime(&time), "%Y%m%d_%H%M%S");
        
        // Open new file
        pImpl->fileOut.open(ss.str(), std::ios::app);
        if (pImpl->fileOut.is_open()) {
            pImpl->currentLogFile = ss.str();
            pImpl->fileOut << "=== Rotated Log File ===\n";
            pImpl->fileOut << "Previous file: " << pImpl->currentLogFile << "\n";
            pImpl->fileOut << "Rotation time: " << getCurrentTimestamp() << "\n";
            pImpl->fileOut << "===================================\n\n";
        }
    }
    
    PerformanceMetrics getSystemMetrics() {
        PerformanceMetrics metrics;
        metrics.timestamp = std::chrono::system_clock::now();
        
        // Get CPU usage (simplified)
        metrics.cpuUsage = getCpuUsage();
        
        // Get memory usage
        std::tie(metrics.memoryUsage, metrics.totalMemory, metrics.availableMemory) = getMemoryUsage();
        
        // Get disk usage (simplified)
        metrics.diskUsage = getDiskUsage();
        
        return metrics;
    }
    
    double getCpuUsage() {
        // Simplified CPU usage calculation
        // In a real implementation, this would read from /proc/stat or use platform-specific APIs
        return 50.0; // Placeholder
    }
    
    std::tuple<double, uint64_t, uint64_t> getMemoryUsage() {
        // Simplified memory usage calculation
        // In a real implementation, this would read from /proc/meminfo or use platform-specific APIs
        return {50.0, 8589934592, 4294967296}; // 8GB total, 4GB available (placeholder)
    }
    
    double getDiskUsage() {
        // Simplified disk usage calculation
        // In a real implementation, this would read from /proc/mounts or use platform-specific APIs
        return 30.0; // Placeholder
    }
    
    std::string getCurrentTimestamp() {
        auto now = std::chrono::system_clock::now();
        auto time = std::chrono::system_clock::to_time_t(now);
        std::stringstream ss;
        ss << std::put_time(std::localtime(&time), "%Y-%m-%d %H:%M:%S");
        return ss.str();
    }
    
    std::string enhancedLogLevelToString(EnhancedLogLevel level) const {
        switch (level) {
            case EnhancedLogLevel::Trace: return "TRACE";
            case EnhancedLogLevel::Debug: return "DEBUG";
            case EnhancedLogLevel::Info: return "INFO";
            case EnhancedLogLevel::Warning: return "WARN";
            case EnhancedLogLevel::Error: return "ERROR";
            case EnhancedLogLevel::Critical: return "CRIT";
            case EnhancedLogLevel::Fatal: return "FATAL";
            default: return "UNKNOWN";
        }
    }
    
    std::string enhancedLogLevelToShortString(EnhancedLogLevel level) const {
        switch (level) {
            case EnhancedLogLevel::Trace: return "T";
            case EnhancedLogLevel::Debug: return "D";
            case EnhancedLogLevel::Info: return "I";
            case EnhancedLogLevel::Warning: return "W";
            case EnhancedLogLevel::Error: return "E";
            case EnhancedLogLevel::Critical: return "C";
            case EnhancedLogLevel::Fatal: return "F";
            default: return "?";
        }
    }
    
    std::string enhancedLogLevelToColor(EnhancedLogLevel level) const {
        switch (level) {
            case EnhancedLogLevel::Trace: return "\033[36m";    // Cyan
            case EnhancedLogLevel::Debug: return "\033[37m";    // White
            case EnhancedLogLevel::Info: return "\033[32m";      // Green
            case EnhancedLogLevel::Warning: return "\033[33m";   // Yellow
            case EnhancedLogLevel::Error: return "\033[31m";     // Red
            case EnhancedLogLevel::Critical: return "\033[35m"; // Magenta
            case EnhancedLogLevel::Fatal: return "\033[41m";     // Red background
            default: return "\033[0m";
        }
    }
    
    std::string logFormatToString(LogOutputFormat format) const {
        switch (format) {
            case LogOutputFormat::PlainText: return "PlainText";
            case LogOutputFormat::Colored: return "Colored";
            case LogOutputFormat::JSON: return "JSON";
            case LogOutputFormat::CSV: return "CSV";
            case LogOutputFormat::StructuredJSON: return "StructuredJSON";
            case LogOutputFormat::Minimal: return "Minimal";
            case LogOutputFormat::Detailed: return "Detailed";
            default: return "Unknown";
        }
    }
    
    std::thread metricsThread;
    std::mutex metricsMutex;
    std::condition_variable metricsCondition;
    bool shutdownMetricsFlag;
    
    void metricsCollectionWorker() {
        while (!shutdownMetricsFlag) {
            std::this_thread::sleep_for(std::chrono::milliseconds(pImpl->metricsInterval));
            collectPerformanceMetrics();
        }
    }
    
    void stopMetricsCollectionThread() {
        if (metricsThread.joinable()) {
            {
                std::lock_guard<std::mutex> lock(metricsMutex);
                shutdownMetricsFlag = true;
            }
            metricsCondition.notify_one();
            metricsThread.join();
        }
    }
};

// Global enhanced logger instance
EnhancedConsoleLogger globalEnhancedLogger;

// Convenience functions for enhanced logging
void logEnhanced(EnhancedLogLevel level, const std::string& message,
                const std::string& file = "", int line = 0,
                const std::string& function = "",
                const std::string& category = "general",
                const std::map<std::string, std::string>& metadata = {}) {
    EnhancedLogEntry entry(level, message, file, line, function);
    entry.category = category;
    entry.metadata = metadata;
    globalEnhancedLogger.log(entry);
}

void debugEnhanced(const std::string& message,
                   const std::string& file = "", int line = 0,
                   const std::string& function = "",
                   const std::string& category = "debug") {
    logEnhanced(EnhancedLogLevel::Debug, message, file, line, function, category);
}

void infoEnhanced(const std::string& message,
                  const std::string& file = "", int line = 0,
                  const std::string& function = "",
                  const std::string& category = "info") {
    logEnhanced(EnhancedLogLevel::Info, message, file, line, function, category);
}

void warningEnhanced(const std::string& message,
                     const std::string& file = "", int line = 0,
                     const std::string& function = "",
                     const std::string& category = "warning") {
    logEnhanced(EnhancedLogLevel::Warning, message, file, line, function, category);
}

void errorEnhanced(const std::string& message,
                   const std::string& file = "", int line = 0,
                   const std::string& function = "",
                   const std::string& category = "error") {
    logEnhanced(EnhancedLogLevel::Error, message, file, line, function, category);
}

void criticalEnhanced(const std::string& message,
                      const std::string& file = "", int line = 0,
                      const std::string& function = "",
                      const std::string& category = "critical") {
    logEnhanced(EnhancedLogLevel::Critical, message, file, line, function, category);
}

void fatalEnhanced(const std::string& message,
                   const std::string& file = "", int line = 0,
                   const std::string& function = "",
                   const std::string& category = "fatal") {
    logEnhanced(EnhancedLogLevel::Fatal, message, file, line, function, category);
}

// Performance logging helper
void logPerformance(const std::string& operation, double executionTime,
                    const std::map<std::string, double>& metrics = {}) {
    EnhancedLogEntry entry(EnhancedLogLevel::Info, 
        "Performance: " + operation + " took " + std::to_string(executionTime) + "μs",
        "", 0, "logPerformance");
    entry.category = "performance";
    entry.executionTime = executionTime;
    entry.metrics = metrics;
    globalEnhancedLogger.log(entry);
}

// Configuration helper
void setupEnhancedLogging(const std::string& configFile = "configs/enhanced.cfg") {
    // Parse configuration file
    // Setup enhanced logging based on configuration
    
    // For now, just set up basic enhanced logging
    globalEnhancedLogger.setLevel(EnhancedLogLevel::Info);
    globalEnhancedLogger.enableAsyncLogging(true);
    globalEnhancedLogger.enableStructuredLogging(true);
    globalEnhancedLogger.enableMetrics(true);
}

// Export helper
bool exportLogs(const std::string& filename, LogOutputFormat format) {
    return globalEnhancedLogger.exportLogs(filename, format);
}

// Statistics helper
EnhancedConsoleLogger::LogStatistics getLogStatistics() {
    return globalEnhancedLogger.getStatistics();
}

// Search helper
std::vector<EnhancedLogEntry> searchLogs(const std::string& pattern,
                                        EnhancedLogLevel minLevel = EnhancedLogLevel::Trace,
                                        const std::string& category = "") {
    return globalEnhancedLogger.searchLogs(pattern, minLevel, category);
}

// Convenience macros for enhanced logging
#define NLM_ENHANCED_LOG_TRACE(msg) nlm::logEnhanced(nlm::EnhancedLogLevel::Trace, msg, __FILE__, __LINE__, __func__)
#define NLM_ENHANCED_LOG_DEBUG(msg) nlm::logEnhanced(nlm::EnhancedLogLevel::Debug, msg, __FILE__, __LINE__, __func__)
#define NLM_ENHANCED_LOG_INFO(msg) nlm::logEnhanced(nlm::EnhancedLogLevel::Info, msg, __FILE__, __LINE__, __func__)
#define NLM_ENHANCED_LOG_WARNING(msg) nlm::logEnhanced(nlm::EnhancedLogLevel::Warning, msg, __FILE__, __LINE__, __func__)
#define NLM_ENHANCED_LOG_ERROR(msg) nlm::logEnhanced(nlm::EnhancedLogLevel::Error, msg, __FILE__, __LINE__, __func__)
#define NLM_ENHANCED_LOG_CRITICAL(msg) nlm::logEnhanced(nlm::EnhancedLogLevel::Critical, msg, __FILE__, __LINE__, __func__)
#define NLM_ENHANCED_LOG_FATAL(msg) nlm::logEnhanced(nlm::EnhancedLogLevel::Fatal, msg, __FILE__, __LINE__, __func__)

// Performance macro
#define NLM_PERFORMANCE_LOG(operation) \
    auto start_time_ = std::chrono::high_resolution_clock::now(); \
    auto operation; \
    auto end_time_ = std::chrono::high_resolution_clock::now(); \
    auto duration_ = std::chrono::duration_cast<std::chrono::microseconds>(end_time_ - start_time_).count(); \
    nlm::logPerformance(#operation, duration_);

} // namespace nlm
