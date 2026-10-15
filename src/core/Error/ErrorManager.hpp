// Error manager - central error handling and logging coordination
// Advanced error management with comprehensive logging integration

#ifndef NLM_ERROR_MANAGER_HPP
#define NLM_ERROR_MANAGER_HPP

#include "ErrorBase.hpp"
#include "ErrorUtilities.hpp"
#include "../core/Logger/Logger.hpp"
#include <thread>
#include <mutex>
#include <queue>
#include <condition_variable>
#include <atomic>
#include <map>
#include <unordered_set>
#include <chrono>
#include <fstream>
#include <json/json.h> // For structured logging

namespace nlm {

// Advanced log entry with structured format
struct StructuredLogEntry {
    std::chrono::system_clock::time_point timestamp;
    LogLevel level;
    std::string message;
    std::string component;
    std::string operation;
    
    // Structured data
    Json::Value data;
    
    // Performance metrics
    std::chrono::milliseconds executionTime;
    size_t memoryUsage;
    
    // Correlation
    std::string correlationId;
    std::string parentLogId;
    
    // Error reference (if this log entry is related to an error)
    std::string errorId;
};

// Log rotation configuration
struct LogRotationConfig {
    size_t maxFileSize = 10 * 1024 * 1024; // 10MB
    size_t maxFiles = 5;
    std::chrono::hours rotationTime{24}; // Rotate daily
    bool compressOldLogs = true;
    std::string backupExtension = ".1";
};

// Log analysis results
struct LogAnalysisResult {
    size_t totalLogs = 0;
    size_t errorCount = 0;
    size_t warningCount = 0;
    std::map<std::string, size_t> logsByComponent;
    std::map<std::string, size_t> logsByOperation;
    std::map<std::string, size_t> errorRates;
    std::vector<std::string> anomalies;
    std::chrono::system_clock::time_point analysisTime;
    
    std::string getAnalysisSummary() const {
        std::ostringstream oss;
        oss << "Log Analysis Summary (" << analysisTime.time_since_epoch().count() << "):\n";
        oss << "  Total logs: " << totalLogs << "\n";
        oss << "  Errors: " << errorCount << "\n";
        oss << "  Warnings: " << warningCount << "\n";
        oss << "  Error rate: " << (totalLogs > 0 ? (errorCount * 100.0 / totalLogs) : 0) << "%\n";
        oss << "  Components:\n";
        for (const auto& [comp, count] : logsByComponent) {
            oss << "    " << comp << ": " << count << "\n";
        }
        if (!anomalies.empty()) {
            oss << "  Anomalies detected:\n";
            for (const auto& anomaly : anomalies) {
                oss << "    - " << anomaly << "\n";
            }
        }
        return oss.str();
    }
};

// Event types for system events
enum class EventType {
    StateChange,
    ConfigurationUpdate,
    SystemStartup,
    SystemShutdown,
    ErrorOccurred,
    RecoveryCompleted,
    PerformanceMetric,
    MemoryAllocation,
    MemoryDeallocation,
    PluginLoaded,
    PluginUnloaded,
    UserAction,
    SecurityEvent,
    Custom
};

// System event
struct SystemEvent {
    EventType type;
    std::chrono::system_clock::time_point timestamp;
    std::string component;
    std::string source;
    Json::Value data;
    
    SystemEvent(EventType t, const std::string& comp, const std::string& src)
        : type(t), component(comp), source(src) {
        timestamp = std::chrono::system_clock::now();
    }
};

// Audit log entry
struct AuditLogEntry {
    std::chrono::system_clock::time_point timestamp;
    std::string userId;
    std::string sessionId;
    std::string action;
    std::string resource;
    std::string outcome;
    std::string ipAddress;
    std::string userAgent;
    Json::Value details;
    
    std::string getSummary() const {
        std::ostringstream oss;
        oss << "[" << getTimestampString() << "] " << userId << " " << action 
            << " " << resource << " " << outcome;
        return oss.str();
    }
    
    std::string getTimestampString() const {
        auto time = std::chrono::system_clock::to_time_t(timestamp);
        std::ostringstream oss;
        oss << std::put_time(std::localtime(&time), "%Y-%m-%d %H:%M:%S");
        return oss.str();
    }
};

// Log aggregator for combining multiple log sources
class LogAggregator {
public:
    LogAggregator() : running_(false), queueSize_(0) {
        // Initialize with default log levels for aggregation
        logLevels_["error"] = LogLevel::Error;
        logLevels_["warning"] = LogLevel::Warning;
        logLevels_["info"] = LogLevel::Info;
        logLevels_["debug"] = LogLevel::Debug;
    }
    
    void start() {
        running_ = true;
        aggregationThread_ = std::thread(&LogAggregator::aggregationWorker, this);
    }
    
    void stop() {
        running_ = false;
        queue_cv_.notify_all();
        if (aggregationThread_.joinable()) {
            aggregationThread_.join();
        }
    }
    
    void addLog(const StructuredLogEntry& entry) {
        std::lock_guard<std::mutex> lock(queue_mutex_);
        logQueue_.push(entry);
        queueSize_++;
        queue_cv_.notify_one();
    }
    
    void addStructuredLog(const Json::Value& json) {
        // Add JSON log entry
        StructuredLogEntry entry;
        entry.timestamp = std::chrono::system_clock::now();
        entry.level = LogLevel::Info;
        
        // Convert JSON to structured entry
        if (json.isMember("message")) {
            entry.message = json["message"].asString();
        }
        if (json.isMember("component")) {
            entry.component = json["component"].asString();
        }
        if (json.isMember("operation")) {
            entry.operation = json["operation"].asString();
        }
        
        addLog(entry);
    }
    
    // Configure log levels by component
    void configureLogLevel(const std::string& component, LogLevel level) {
        std::lock_guard<std::mutex> lock(config_mutex_);
        logLevels_[component] = level;
    }
    
    // Get aggregated statistics
    std::string getAggregationStats() const {
        std::lock_guard<std::mutex> lock(stats_mutex_);
        std::ostringstream oss;
        oss << "Log Aggregation Statistics:\n";
        oss << "  Queue size: " << queueSize_ << "\n";
        oss << "  Total processed: " << totalProcessed_ << "\n";
        oss << "  By component:\n";
        for (const auto& [comp, count] : componentCounts_) {
            oss << "    " << comp << ": " << count << "\n";
        }
        return oss.str();
    }
    
    // Export aggregated logs
    std::string exportLogs() const {
        std::lock_guard<std::mutex> lock(export_mutex_);
        std::ostringstream oss;
        oss << "Aggregated Logs:\n";
        oss << "  Total entries: " << aggregatedLogs_.size() << "\n";
        
        for (const auto& entry : aggregatedLogs_) {
            oss << "  - " << getTimestampString(entry.timestamp) << " "
                << logLevelToString(entry.level) << " "
                << entry.component << "." << entry.operation << ": "
                << entry.message << "\n";
        }
        
        return oss.str();
    }
    
private:
    void aggregationWorker() {
        while (running_) {
            StructuredLogEntry entry;
            {
                std::unique_lock<std::mutex> lock(queue_mutex_);
                queue_cv_.wait_for(lock, std::chrono::milliseconds(100), 
                                 [this] { return !logQueue_.empty(); });
                
                if (!logQueue_.empty()) {
                    entry = logQueue_.front();
                    logQueue_.pop();
                    queueSize_--;
                }
            }
            
            if (!entry.message.empty()) {
                processLogEntry(entry);
            }
        }
    }
    
    void processLogEntry(const StructuredLogEntry& entry) {
        std::lock_guard<std::mutex> lock(stats_mutex_);
        totalProcessed_++;
        componentCounts_[entry.component]++;
        
        // Apply filtering based on component log level
        auto it = logLevels_.find(entry.component);
        if (it != logLevels_.end() && entry.level < it->second) {
            return; // Filtered out
        }
        
        // Add to aggregated logs
        aggregatedLogs_.push_back(entry);
        
        // Keep only recent logs (rolling window)
        if (aggregatedLogs_.size() > 10000) {
            aggregatedLogs_.erase(aggregatedLogs_.begin(), 
                                aggregatedLogs_.begin() + 5000);
        }
        
        // Convert to JSON and store
        Json::Value jsonEntry;
        jsonEntry["timestamp"] = getTimestampString(entry.timestamp);
        jsonEntry["level"] = logLevelToString(entry.level);
        jsonEntry["message"] = entry.message;
        jsonEntry["component"] = entry.component;
        jsonEntry["operation"] = entry.operation;
        
        if (entry.executionTime.count() > 0) {
            jsonEntry["execution_time_ms"] = entry.executionTime.count();
        }
        if (entry.memoryUsage > 0) {
            jsonEntry["memory_usage_bytes"] = entry.memoryUsage;
        }
        
        // Store in persistent storage (simplified - would use proper logging system)
        storeLogEntry(jsonEntry);
    }
    
    void storeLogEntry(const Json::Value& entry) {
        // In production, this would write to a persistent log store
        // For now, just track that we've received the log
        storedLogsCount_++;
    }
    
    std::string getTimestampString(const std::chrono::system_clock::time_point& time) const {
        auto t = std::chrono::system_clock::to_time_t(time);
        std::ostringstream oss;
        oss << std::put_time(std::localtime(&t), "%Y-%m-%d %H:%M:%S.%f");
        return oss.str();
    }
    
    std::string logLevelToString(LogLevel level) const {
        static const std::map<LogLevel, std::string> levelStrings = {
            {LogLevel::Debug, "DEBUG"},
            {LogLevel::Info, "INFO"},
            {LogLevel::Warning, "WARN"},
            {LogLevel::Error, "ERROR"},
            {LogLevel::Critical, "CRITICAL"}
        };
        
        auto it = levelStrings.find(level);
        return (it != levelStrings.end()) ? it->second : "UNKNOWN";
    }
    
    std::thread aggregationThread_;
    std::mutex queue_mutex_;
    std::mutex config_mutex_;
    std::mutex stats_mutex_;
    std::mutex export_mutex_;
    std::condition_variable queue_cv_;
    std::queue<StructuredLogEntry> logQueue_;
    std::map<std::string, std::vector<StructuredLogEntry>> aggregatedLogs_;
    std::map<std::string, LogLevel> logLevels_;
    std::map<std::string, size_t> componentCounts_;
    
    std::atomic<bool> running_;
    std::atomic<size_t> queueSize_;
    std::atomic<size_t> totalProcessed_;
    std::atomic<size_t> storedLogsCount_;
};

// Advanced logging manager
class AdvancedLogger {
public:
    AdvancedLogger() : running_(false), logRotationConfig_(), analysisRunning_(false) {
        // Initialize with default log rotation
        logRotationConfig_.maxFileSize = 50 * 1024 * 1024; // 50MB
        logRotationConfig_.maxFiles = 10;
        logRotationConfig_.rotationTime = std::chrono::hours(24);
        
        // Create log directories
        ensureLogDirectories();
    }
    
    ~AdvancedLogger() {
        stop();
    }
    
    // Initialize with configuration
    void initialize(const std::string& logDirectory, 
                    const LogRotationConfig& rotationConfig = LogRotationConfig()) {
        logDirectory_ = logDirectory;
        logRotationConfig_ = rotationConfig;
        
        // Create log files
        errorLog_ = std::ofstream(logDirectory + "/errors.log", std::ios::app);
        infoLog_ = std::ofstream(logDirectory + "/info.log", std::ios::app);
        auditLog_ = std::ofstream(logDirectory + "/audit.log", std::ios::app);
        performanceLog_ = std::ofstream(logDirectory + "/performance.log", std::ios::app);
        eventsLog_ = std::ofstream(logDirectory + "/events.log", std::ios::app);
        
        ensureLogDirectories();
        
        // Start background threads
        startMonitoringThreads();
    }
    
    // Log structured error
    void logError(const Error& error) {
        // Log to error log with structured format
        Json::Value errorJson;
        errorJson["id"] = error.getId();
        errorJson["timestamp"] = getTimestampString(error.getContext().timestamp);
        errorJson["type"] = errorTypeToString(error.getType());
        errorJson["severity"] = errorSeverityToString(error.getSeverity());
        errorJson["message"] = error.what();
        errorJson["component"] = error.getContext().component;
        errorJson["operation"] = error.getContext().operation;
        
        // Add parameters
        if (!error.getContext().parameters.empty()) {
            errorJson["parameters"] = Json::Value(Json::objectValue);
            for (const auto& [key, value] : error.getContext().parameters) {
                errorJson["parameters"][key] = value;
            }
        }
        
        // Add suggestions
        if (!error.getContext().suggestions.empty()) {
            errorJson["suggestions"] = Json::Value(Json::arrayValue);
            for (const auto& [suggestion, category] : error.getContext().suggestions) {
                Json::Value suggestionJson;
                suggestionJson["text"] = suggestion;
                suggestionJson["category"] = category;
                errorJson["suggestions"].append(suggestionJson);
            }
        }
        
        // Log to error log
        logToFile(errorLog_, errorJson);
        
        // Also log to standard logger for consistency
        NLM_LOG_ERROR("Error " + error.getId() + ": " + error.what());
        
        // Record in statistics
        getErrorRecoveryManager().recordError(error);
    }
    
    // Log structured info
    void logInfo(const std::string& message, 
                 const std::string& component = "",
                 const std::string& operation = "",
                 const Json::Value& data = Json::Value()) {
        StructuredLogEntry entry;
        entry.timestamp = std::chrono::system_clock::now();
        entry.level = LogLevel::Info;
        entry.message = message;
        entry.component = component;
        entry.operation = operation;
        entry.data = data;
        
        logStructured(entry);
        
        // Also log to standard logger
        if (!component.empty() && !operation.empty()) {
            NLM_LOG_INFO("[" + component + "." + operation + "] " + message);
        } else {
            NLM_LOG_INFO(message);
        }
    }
    
    // Log structured warning
    void logWarning(const std::string& message,
                    const std::string& component = "",
                    const std::string& operation = "",
                    const Json::Value& data = Json::Value()) {
        StructuredLogEntry entry;
        entry.timestamp = std::chrono::system_clock::now();
        entry.level = LogLevel::Warning;
        entry.message = message;
        entry.component = component;
        entry.operation = operation;
        entry.data = data;
        
        logStructured(entry);
        
        NLM_LOG_WARNING("[" + component + "." + operation + "] " + message);
    }
    
    // Log structured performance metrics
    void logPerformance(const std::string& metricName,
                        double value,
                        const std::string& component = "",
                        const std::string& unit = "",
                        const std::map<std::string, std::string>& tags = {}) {
        StructuredLogEntry entry;
        entry.timestamp = std::chrono::system_clock::now();
        entry.level = LogLevel::Info;
        entry.message = "Performance metric: " + metricName + " = " + std::to_string(value) + " " + unit;
        entry.component = component;
        entry.operation = "performance_metric";
        
        Json::Value data;
        data["metric_name"] = metricName;
        data["value"] = value;
        data["unit"] = unit;
        for (const auto& [key, value] : tags) {
            data["tags"][key] = value;
        }
        entry.data = data;
        
        logStructured(entry);
        
        logToFile(performanceLog_, entry.data);
    }
    
    // Log system event
    void logEvent(EventType eventType,
                  const std::string& component,
                  const std::string& source,
                  const Json::Value& data = Json::Value()) {
        SystemEvent event(eventType, component, source);
        event.data = data;
        
        logToFile(eventsLog_, Json::Value::from(event));
        
        // Also log to structured system
        StructuredLogEntry entry;
        entry.timestamp = event.timestamp;
        entry.level = LogLevel::Info;
        entry.message = eventTypeToString(eventType);
        entry.component = component;
        entry.operation = "system_event";
        entry.data = data;
        
        logStructured(entry);
    }
    
    // Log audit entry
    void logAudit(const AuditLogEntry& auditEntry) {
        Json::Value auditJson;
        auditJson["timestamp"] = auditEntry.getTimestampString();
        auditJson["user_id"] = auditEntry.userId;
        auditJson["session_id"] = auditEntry.sessionId;
        auditJson["action"] = auditEntry.action;
        auditJson["resource"] = auditEntry.resource;
        auditJson["outcome"] = auditEntry.outcome;
        auditJson["ip_address"] = auditEntry.ipAddress;
        auditJson["user_agent"] = auditEntry.userAgent;
        auditJson["details"] = auditEntry.details;
        
        logToFile(auditLog_, auditJson);
    }
    
    // Analyze logs
    LogAnalysisResult analyzeLogs(const std::string& component = "") {
        LogAnalysisResult result;
        result.analysisTime = std::chrono::system_clock::now();
        
        // In production, this would analyze actual log files
        // For now, provide a summary
        result.totalLogs = getLogCount();
        result.errorCount = getErrorCount();
        result.warningCount = getWarningCount();
        
        return result;
    }
    
    // Export logs in various formats
    std::string exportLogs(const std::string& format = "json") {
        if (format == "json") {
            return exportAsJSON();
        } else if (format == "csv") {
            return exportAsCSV();
        } else if (format == "text") {
            return exportAsText();
        }
        return "Unsupported format: " + format;
    }
    
    // Rotate logs
    void rotateLogs() {
        auto now = std::chrono::system_clock::now();
        auto timeSinceLastRotation = std::chrono::duration_cast<std::chrono::hours>(
            now - lastRotationTime_);
        
        if (timeSinceLastRotation > logRotationConfig_.rotationTime) {
            performLogRotation();
            lastRotationTime_ = now;
        }
    }
    
    // Start monitoring threads
    void startMonitoringThreads() {
        running_ = true;
        
        // Start log rotation monitor
        rotationMonitorThread_ = std::thread(&AdvancedLogger::rotationMonitorWorker, this);
        
        // Start log analysis monitor
        analysisMonitorThread_ = std::thread(&AdvancedLogger::analysisMonitorWorker, this);
    }
    
    // Stop all threads
    void stop() {
        running_ = false;
        
        if (rotationMonitorThread_.joinable()) {
            rotationMonitorThread_.join();
        }
        if (analysisMonitorThread_.joinable()) {
            analysisMonitorThread_.join();
        }
        
        // Close log files
        errorLog_.close();
        infoLog_.close();
        auditLog_.close();
        performanceLog_.close();
        eventsLog_.close();
    }
    
private:
    void ensureLogDirectories() {
        // Ensure log directory exists
        if (!std::filesystem::exists(logDirectory_)) {
            std::filesystem::create_directories(logDirectory_);
        }
    }
    
    void logStructured(const StructuredLogEntry& entry) {
        // Send to log aggregator
        if (logAggregator_) {
            logAggregator_->addLog(entry);
        }
        
        // Also write to structured log file
        Json::Value jsonEntry;
        jsonEntry["timestamp"] = getTimestampString(entry.timestamp);
        jsonEntry["level"] = logLevelToString(entry.level);
        jsonEntry["message"] = entry.message;
        jsonEntry["component"] = entry.component;
        jsonEntry["operation"] = entry.operation;
        jsonEntry["execution_time_ms"] = entry.executionTime.count();
        jsonEntry["memory_usage_bytes"] = entry.memoryUsage;
        jsonEntry["data"] = entry.data;
        
        logToFile(infoLog_, jsonEntry);
    }
    
    void logToFile(std::ofstream& file, const Json::Value& data) {
        if (!file.is_open()) return;
        
        std::lock_guard<std::mutex> lock(file_mutex_);
        
        // Write JSON entry
        Json::StreamWriterBuilder writerBuilder;
        writerBuilder["indentation"] = "  ";
        std::unique_ptr<Json::StreamWriter> writer(writerBuilder.newStreamWriter());
        writer->write(data, file);
        file << "\n";
        file.flush();
        
        // Check file size for rotation
        checkFileSize(file, logRotationConfig_.maxFileSize);
    }
    
    void checkFileSize(std::ofstream& file, size_t maxSize) {
        if (!file.is_open()) return;
        
        auto currentPos = file.tellp();
        if (currentPos > static_cast<std::streampos>(maxSize)) {
            performFileRotation(file);
        }
    }
    
    void performLogRotation() {
        // Rotate all log files
        rotateFile(errorLog_, logRotationConfig_);
        rotateFile(infoLog_, logRotationConfig_);
        rotateFile(auditLog_, logRotationConfig_);
        rotateFile(performanceLog_, logRotationConfig_);
        rotateFile(eventsLog_, logRotationConfig_);
    }
    
    void rotateFile(std::ofstream& file, const LogRotationConfig& config) {
        if (!file.is_open()) return;
        
        file.close();
        
        // Close all files before rotation
        // (In production, would properly close and rename files)
    }
    
    std::string getTimestampString(const std::chrono::system_clock::time_point& time) const {
        auto t = std::chrono::system_clock::to_time_t(time);
        std::ostringstream oss;
        oss << std::put_time(std::localtime(&t), "%Y-%m-%d %H:%M:%S.%f");
        return oss.str();
    }
    
    std::string eventTypeToString(EventType type) const {
        static const std::map<EventType, std::string> typeStrings = {
            {EventType::StateChange, "STATE_CHANGE"},
            {EventType::ConfigurationUpdate, "CONFIG_UPDATE"},
            {EventType::SystemStartup, "SYSTEM_STARTUP"},
            {EventType::SystemShutdown, "SYSTEM_SHUTDOWN"},
            {EventType::ErrorOccurred, "ERROR_OCCURRED"},
            {EventType::RecoveryCompleted, "RECOVERY_COMPLETED"},
            {EventType::PerformanceMetric, "PERFORMANCE_METRIC"},
            {EventType::MemoryAllocation, "MEMORY_ALLOCATION"},
            {EventType::MemoryDeallocation, "MEMORY_DEALLOCATION"},
            {EventType::PluginLoaded, "PLUGIN_LOADED"},
            {EventType::PluginUnloaded, "PLUGIN_UNLOADED"},
            {EventType::UserAction, "USER_ACTION"},
            {EventType::SecurityEvent, "SECURITY_EVENT"},
            {EventType::Custom, "CUSTOM"}
        };
        
        auto it = typeStrings.find(type);
        return (it != typeStrings.end()) ? it->second : "UNKNOWN";
    }
    
    // Thread workers
    void rotationMonitorWorker() {
        while (running_) {
            rotateLogs();
            std::this_thread::sleep_for(std::chrono::hours(1));
        }
    }
    
    void analysisMonitorWorker() {
        while (running_) {
            if (!analysisRunning_.exchange(true)) {
                // Perform log analysis
                auto analysis = analyzeLogs();
                if (!analysis.anomalies.empty()) {
                    logEvent(EventType::Custom, "system", "log_analyzer", 
                            Json::Value::from(analysis.anomalies));
                }
                analysisRunning_ = false;
            }
            std::this_thread::sleep_for(std::chrono::minutes(10));
        }
    }
    
    // Export helpers (simplified implementations)
    std::string exportAsJSON() const { return "JSON export not implemented yet"; }
    std::string exportAsCSV() const { return "CSV export not implemented yet"; }
    std::string exportAsText() const { return "Text export not implemented yet"; }
    
    size_t getLogCount() const { return totalLogCount_; }
    size_t getErrorCount() const { return errorLogCount_; }
    size_t getWarningCount() const { return warningLogCount_; }
    
    // Log files
    std::string logDirectory_;
    std::ofstream errorLog_;
    std::ofstream infoLog_;
    std::ofstream auditLog_;
    std::ofstream performanceLog_;
    std::ofstream eventsLog_;
    
    // Configuration
    LogRotationConfig logRotationConfig_;
    std::chrono::system_clock::time_point lastRotationTime_;
    
    // Aggregator
    std::unique_ptr<LogAggregator> logAggregator_;
    
    // Threads
    std::thread rotationMonitorThread_;
    std::thread analysisMonitorThread_;
    std::mutex file_mutex_;
    std::atomic<bool> running_;
    std::atomic<bool> analysisRunning_;
    
    // Statistics
    std::atomic<size_t> totalLogCount_;
    std::atomic<size_t> errorLogCount_;
    std::atomic<size_t> warningLogCount_;
    
    friend class ErrorRecoveryManager; // For accessing statistics
};

// Global advanced logger instance
inline AdvancedLogger& getAdvancedLogger() {
    static AdvancedLogger logger;
    return logger;
}

// Global log aggregator instance
inline LogAggregator& getLogAggregator() {
    static LogAggregator aggregator;
    return aggregator;
}

// Enhanced logging macros and functions
#define NLM_STRUCTURED_LOG_ERROR(error) \
    getAdvancedLogger().logError(error)

#define NLM_STRUCTURED_LOG_INFO(message, component, operation, ...) \
    do { \
        Json::Value data; \
        /* Parse additional key-value pairs */ \
        getAdvancedLogger().logInfo(message, component, operation, data); \
    } while(0)

#define NLM_STRUCTURED_LOG_WARNING(message, component, operation, ...) \
    do { \
        Json::Value data; \
        /* Parse additional key-value pairs */ \
        getAdvancedLogger().logWarning(message, component, operation, data); \
    } while(0)

#define NLM_INITIALIZE_ADVANCED_LOGGER(directory, config) \
    getAdvancedLogger().initialize(directory, config)

#define NLM_START_LOG_AGGREGATOR() \
    getLogAggregator().start()

#define NLM_STOP_LOG_AGGREGATOR() \
    getLogAggregator().stop()

#define NLM_PERFORM_LOG_ROTATION() \
    getAdvancedLogger().rotateLogs()

#define NLM_ANALYZE_LOGS(component) \
    getAdvancedLogger().analyzeLogs(component)

#define NLM_EXPORT_LOGS(format) \
    getAdvancedLogger().exportLogs(format)

// Convenience functions
inline void initializeAdvancedLogging(const std::string& logDirectory, 
                                      const LogRotationConfig& config = LogRotationConfig()) {
    getAdvancedLogger().initialize(logDirectory, config);
    getLogAggregator().start();
}

inline void logStructuredEvent(EventType eventType,
                              const std::string& component,
                              const std::string& source,
                              const Json::Value& data = Json::Value()) {
    getAdvancedLogger().logEvent(eventType, component, source, data);
}

inline void logPerformanceMetric(const std::string& name,
                                 double value,
                                 const std::string& component = "",
                                 const std::string& unit = "",
                                 const std::map<std::string, std::string>& tags = {}) {
    getAdvancedLogger().logPerformance(name, value, component, unit, tags);
}

inline void logSystemError(const Error& error) {
    getAdvancedLogger().logError(error);
    logStructuredEvent(EventType::ErrorOccurred, error.getContext().component, "system", 
                      Json::Value::from(error.getId()));
}

inline std::string getErrorStatistics() {
    return getErrorRecoveryManager().getStatistics().getSummary();
}

inline std::string getLogAnalysis(const std::string& component = "") {
    auto result = getAdvancedLogger().analyzeLogs(component);
    return result.getAnalysisSummary();
}

} // namespace nlm

#endif // NLM_ERROR_MANAGER_HPP
