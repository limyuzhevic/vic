#pragma once

#include "../core/Logger/Logger.hpp"
#include "../core/Config/Config.hpp"
#include <string>
#include <memory>
#include <sstream>
#include <functional>
#include <vector>
#include <chrono>
#include <unordered_map>
#include <optional>
#include <mutex>
#include <iostream>
#include <iomanip>
#include <ctime>
#include <thread>
#include <algorithm>
#include <map>
#include <cstdint>
#include <limits>
#include <numeric>

namespace nlm {

// Enhanced error codes for NLM
enum class ErrorCode {
    Success = 0,
    InitializationFailed = 1,
    MemoryAllocationFailed = 2,
    ConfigurationError = 3,
    FileIOError = 4,
    NetworkConnectionError = 5,
    ValidationError = 6,
    PerformanceDegradationDetected = 7,
    SystemHealthCheckFailed = 8,
    ResourceLimitExceeded = 9,
    DebugModeEnabled = 10,
    EventLoggingEnabled = 11,
    ConnectionTrackingEnabled = 12,
    PerformanceMonitoringEnabled = 13,
    SystemHealthChecksEnabled = 14,
    ErrorCodesEnabled = 15
};

// Enhanced configuration validation
class ConfigValidator {
public:
    struct ValidationResult {
        bool isValid;
        std::string message;
        std::string category;
        std::vector<std::string> errors;
        std::vector<std::string> warnings;
        std::chrono::steady_clock::time_point timestamp;
        
        ValidationResult() : isValid(true), timestamp(std::chrono::steady_clock::now()) {}
    };
    
    static ValidationResult validate(const Config& config);
    static ValidationResult validateParameter(const std::string& key, const std::string& value);
    static ValidationResult validateConnections(const std::map<std::string, std::string>& connections);
    static ValidationResult validateMemoryUsage(uint64_t memoryUsage);
    static ValidationResult validatePerformance(const std::string& operation, double elapsedTime);
    
    static std::string getValidationReport(const ValidationResult& result);
    static std::string getValidationSummary(const std::vector<ValidationResult>& results);
    
    static void enableValidation(bool enable);
    static bool isValidationEnabled();
    static void setValidationLevel(int level);
    static int getValidationLevel();
    
    static void addValidationRule(const std::string& rule);
    static void removeValidationRule(const std::string& rule);
    static void clearValidationRules();
    
    static std::vector<ValidationResult> validateAll(const Config& config,
                                                      const std::map<std::string, std::string>& connections,
                                                      uint64_t memoryUsage);
    
    static void resetValidationResults();
    static std::vector<ValidationResult> getValidationResults();
    
    static void setValidationCallback(std::function<void(const ValidationResult&)> callback);
    static void clearValidationCallback();
    
private:
    static bool isInitialized;
    static bool validationEnabled;
    static int validationLevel;
    static std::vector<std::string> validationRules;
    static std::vector<ValidationResult> validationResults;
    static std::function<void(const ValidationResult&)> validationCallback;
    static std::mutex validationMutex;
    
    static bool validateKeyFormat(const std::string& key);
    static bool validateValueFormat(const std::string& value, const std::string& type);
    static bool validateConnectionFormat(const std::string& source, const std::string& target);
    static std::string formatValidationError(const std::string& message, const std::string& category);
};

// Enhanced error handling
class ErrorHandler {
public:
    struct ErrorInfo {
        ErrorCode code;
        std::string message;
        std::string category;
        std::chrono::steady_clock::time_point timestamp;
        std::string context;
        std::map<std::string, std::string> metadata;
        
        ErrorInfo() : code(ErrorCode::Success), timestamp(std::chrono::steady_clock::now()) {}
        ErrorInfo(ErrorCode c, const std::string& m) : code(c), message(m), timestamp(std::chrono::steady_clock::now()) {}
    };
    
    static void logError(ErrorCode code, const std::string& message, const std::string& category = "Error");
    static void logError(ErrorCode code, const std::string& message, const std::string& category,
                         const std::string& context);
    static void logError(ErrorCode code, const std::string& message, const std::string& category,
                         const std::string& context, const std::map<std::string, std::string>& metadata);
    
    static std::vector<ErrorInfo> getErrorHistory();
    static std::vector<ErrorInfo> getErrorHistory(ErrorCode code);
    static std::vector<ErrorInfo> getErrorHistory(const std::string& category);
    static std::vector<ErrorInfo> getErrorHistory(const std::string& category, const std::string& context);
    
    static bool hasErrors();
    static bool hasErrors(ErrorCode code);
    static bool hasErrors(const std::string& category);
    
    static size_t getErrorCount();
    static size_t getErrorCount(ErrorCode code);
    static size_t getErrorCount(const std::string& category);
    
    static void clearErrorHistory();
    static void clearErrorHistory(ErrorCode code);
    static void clearErrorHistory(const std::string& category);
    
    static std::string getErrorReport();
    static std::string getErrorSummary();
    
    static void setErrorCallback(std::function<void(const ErrorInfo&)> callback);
    static void clearErrorCallback();
    
    static void enableErrorReporting(bool enable);
    static bool isErrorReportingEnabled();
    
    static void setMaxErrorHistorySize(size_t maxSize);
    static size_t getMaxErrorHistorySize();
    
    static std::string formatError(const ErrorInfo& error);
    static std::string formatErrorReport(const std::vector<ErrorInfo>& errors);
    
    static ErrorInfo createError(ErrorCode code, const std::string& message,
                                 const std::string& category = "Error");
    static ErrorInfo createError(ErrorCode code, const std::string& message,
                                 const std::string& category, const std::string& context);
    
    static bool isRecoverable(ErrorCode code);
    static std::string getErrorRecoverySuggestion(ErrorCode code);
    
    static void resetErrorHandler();
    
private:
    static bool isInitialized;
    static bool errorReportingEnabled;
    static size_t maxErrorHistorySize;
    static std::vector<ErrorInfo> errorHistory;
    static std::function<void(const ErrorInfo&)> errorCallback;
    static std::mutex errorMutex;
    
    static std::string getErrorCodeName(ErrorCode code);
    static std::string getErrorCodeDescription(ErrorCode code);
};

// Enhanced connection tracking
class ConnectionTracker {
public:
    struct ConnectionInfo {
        std::string source;
        std::string target;
        float weight;
        Delay delay;
        std::chrono::steady_clock::time_point lastUpdate;
        size_t connectionCount;
        bool isActive;
        std::map<std::string, std::string> metadata;
        
        ConnectionInfo() : weight(0.0f), delay(1), connectionCount(0), isActive(false) {}
        ConnectionInfo(const std::string& s, const std::string& t, float w = 0.0f, Delay d = 1)
            : source(s), target(t), weight(w), delay(d), connectionCount(1), isActive(true) {}
    };
    
    static void trackConnection(const std::string& source, const std::string& target, float weight = 0.0f);
    static void untrackConnection(const std::string& source, const std::string& target);
    static void updateConnection(const std::string& source, const std::string& target, float weight);
    static bool hasConnection(const std::string& source, const std::string& target);
    
    static ConnectionInfo getConnection(const std::string& source, const std::string& target);
    static std::vector<ConnectionInfo> getConnections();
    static std::vector<ConnectionInfo> getConnections(const std::string& source);
    static std::vector<ConnectionInfo> getConnections(const std::string& source, const std::string& target);
    
    static size_t getConnectionCount();
    static size_t getConnectionCount(const std::string& source);
    static size_t getConnectionCount(const std::string& source, const std::string& target);
    
    static void clearConnections();
    static void clearConnections(const std::string& source);
    static void clearConnections(const std::string& source, const std::string& target);
    
    static std::string getConnectionReport();
    static std::string getConnectionSummary();
    
    static void setConnectionCallback(std::function<void(const ConnectionInfo&)> callback);
    static void clearConnectionCallback();
    
    static void enableConnectionTracking(bool enable);
    static bool isConnectionTrackingEnabled();
    
    static void setMaxConnectionHistorySize(size_t maxSize);
    static size_t getMaxConnectionHistorySize();
    
    static void addConnectionMetadata(const std::string& source, const std::string& target,
                                      const std::string& key, const std::string& value);
    static std::string getConnectionMetadata(const std::string& source, const std::string& target,
                                             const std::string& key);
    static void removeConnectionMetadata(const std::string& source, const std::string& target,
                                         const std::string& key);
    
    static bool validateConnection(const std::string& source, const std::string& target);
    static std::string getConnectionValidationReport();
    
    static void resetConnectionTracker();
    
private:
    static bool isInitialized;
    static bool connectionTrackingEnabled;
    static size_t maxConnectionHistorySize;
    static std::map<std::pair<std::string, std::string>, ConnectionInfo> connections;
    static std::function<void(const ConnectionInfo&)> connectionCallback;
    static std::mutex connectionMutex;
    
    static std::pair<std::string, std::string> createConnectionKey(const std::string& source,
                                                                     const std::string& target);
    static bool validateConnectionKey(const std::string& source, const std::string& target);
    static std::string formatConnectionReport(const ConnectionInfo& connection);
    static std::string formatConnectionSummary();
};

// Enhanced event logging
class EventLogger {
public:
    struct EventInfo {
        std::string event;
        std::string category;
        std::string details;
        std::chrono::steady_clock::time_point timestamp;
        std::map<std::string, std::string> metadata;
        
        EventInfo() : timestamp(std::chrono::steady_clock::now()) {}
        EventInfo(const std::string& e, const std::string& c, const std::string& d = "")
            : event(e), category(c), details(d), timestamp(std::chrono::steady_clock::now()) {}
    };
    
    static void logEvent(const std::string& event, const std::string& category = "Event",
                         const std::string& details = "");
    static void logEvent(const std::string& event, const std::string& category,
                         const std::string& details, const std::map<std::string, std::string>& metadata);
    
    static std::vector<EventInfo> getEventHistory();
    static std::vector<EventInfo> getEventHistory(const std::string& category);
    static std::vector<EventInfo> getEventHistory(const std::string& category, const std::string& event);
    
    static size_t getEventCount();
    static size_t getEventCount(const std::string& category);
    static size_t getEventCount(const std::string& category, const std::string& event);
    
    static void clearEventHistory();
    static void clearEventHistory(const std::string& category);
    static void clearEventHistory(const std::string& category, const std::string& event);
    
    static std::string getEventReport();
    static std::string getEventSummary();
    
    static void setEventCallback(std::function<void(const EventInfo&)> callback);
    static void clearEventCallback();
    
    static void enableEventLogging(bool enable);
    static bool isEventLoggingEnabled();
    
    static void setMaxEventHistorySize(size_t maxSize);
    static size_t getMaxEventHistorySize();
    
    static void addEventMetadata(const std::string& event, const std::string& category,
                                 const std::string& key, const std::string& value);
    static std::string getEventMetadata(const std::string& event, const std::string& category,
                                        const std::string& key);
    static void removeEventMetadata(const std::string& event, const std::string& category,
                                    const std::string& key);
    
    static bool validateEvent(const std::string& event, const std::string& category);
    static std::string getEventValidationReport();
    
    static void resetEventLogger();
    
private:
    static bool isInitialized;
    static bool eventLoggingEnabled;
    static size_t maxEventHistorySize;
    static std::vector<EventInfo> eventHistory;
    static std::function<void(const EventInfo&)> eventCallback;
    static std::mutex eventMutex;
    
    static bool validateEventInfo(const EventInfo& eventInfo);
    static std::string formatEventReport(const EventInfo& event);
    static std::string formatEventSummary();
};

// Performance monitoring
class PerformanceMonitor {
public:
    struct PerformanceMetric {
        std::string operation;
        double elapsedTime;
        std::chrono::steady_clock::time_point timestamp;
        std::map<std::string, std::string> metadata;
        bool isCompleted;
        
        PerformanceMetric() : elapsedTime(0.0), isCompleted(false) {}
        PerformanceMetric(const std::string& op, double time)
            : operation(op), elapsedTime(time), isCompleted(true) {}
    };
    
    static void startOperation(const std::string& operation);
    static void stopOperation(const std::string& operation);
    static double getOperationTime(const std::string& operation);
    static bool isOperationRunning(const std::string& operation);
    
    static PerformanceMetric getMetric(const std::string& operation);
    static std::vector<PerformanceMetric> getMetrics();
    static std::vector<PerformanceMetric> getMetrics(const std::string& operation);
    
    static size_t getMetricCount();
    static size_t getMetricCount(const std::string& operation);
    
    static void clearMetrics();
    static void clearMetrics(const std::string& operation);
    
    static std::string getPerformanceReport();
    static std::string getPerformanceSummary();
    
    static void setPerformanceCallback(std::function<void(const PerformanceMetric&)> callback);
    static void clearPerformanceCallback();
    
    static void enablePerformanceMonitoring(bool enable);
    static bool isPerformanceMonitoringEnabled();
    
    static void setMaxMetricHistorySize(size_t maxSize);
    static size_t getMaxMetricHistorySize();
    
    static double getAverageOperationTime(const std::string& operation);
    static double getTotalOperationTime(const std::string& operation);
    static double getMinOperationTime(const std::string& operation);
    static double getMaxOperationTime(const std::string& operation);
    
    static std::string getPerformanceTrend(const std::string& operation);
    static bool isPerformanceDegrading(const std::string& operation, double threshold);
    
    static void resetPerformanceMonitor();
    
private:
    static bool isInitialized;
    static bool performanceMonitoringEnabled;
    static size_t maxMetricHistorySize;
    static std::map<std::string, PerformanceMetric> metrics;
    static std::map<std::string, std::chrono::steady_clock::time_point> activeOperations;
    static std::function<void(const PerformanceMetric&)> performanceCallback;
    static std::mutex performanceMutex;
    
    static double calculateAverageTime(const std::string& operation);
    static double calculateTotalTime(const std::string& operation);
    static double calculateMinTime(const std::string& operation);
    static double calculateMaxTime(const std::string& operation);
    static std::string calculateTrend(const std::string& operation);
    static bool checkDegrading(const std::string& operation, double threshold);
};

// System health checks
class SystemHealthChecker {
public:
    struct HealthCheckResult {
        bool isHealthy;
        std::string checkName;
        std::string message;
        std::chrono::steady_clock::time_point timestamp;
        std::map<std::string, std::string> metadata;
        double executionTime;
        
        HealthCheckResult() : isHealthy(true), executionTime(0.0), timestamp(std::chrono::steady_clock::now()) {}
        HealthCheckResult(const std::string& name, bool healthy, const std::string& msg)
            : isHealthy(healthy), checkName(name), message(msg), executionTime(0.0), timestamp(std::chrono::steady_clock::now()) {}
    };
    
    static HealthCheckResult checkMemoryUsage();
    static HealthCheckResult checkCPUUsage();
    static HealthCheckResult checkConnectionHealth();
    static HealthCheckResult checkPerformanceHealth();
    static HealthCheckResult checkConfigurationHealth();
    static HealthCheckResult checkEventLogHealth();
    static HealthCheckResult checkConnectionMapHealth();
    
    static std::vector<HealthCheckResult> performAllChecks();
    static std::vector<HealthCheckResult> performCheck(const std::string& checkName);
    
    static bool isSystemHealthy();
    static std::string getSystemHealthStatus();
    
    static std::string getHealthReport();
    static std::string getHealthSummary();
    
    static void setHealthCheckCallback(std::function<void(const HealthCheckResult&)> callback);
    static void clearHealthCheckCallback();
    
    static void enableHealthChecks(bool enable);
    static bool areHealthChecksEnabled();
    
    static void addCustomHealthCheck(std::function<HealthCheckResult()> check);
    static void removeCustomHealthCheck(const std::string& name);
    static void clearCustomHealthChecks();
    
    static size_t getCheckCount();
    static size_t getFailedCheckCount();
    static size_t getPassedCheckCount();
    
    static std::chrono::steady_clock::time_point getLastCheckTime();
    static double getAverageCheckTime();
    
    static void setMaxHistorySize(size_t maxSize);
    static size_t getMaxHistorySize();
    
    static std::vector<HealthCheckResult> getRecentChecks(size_t count = 10);
    static std::vector<HealthCheckResult> getFailedChecks();
    static std::vector<HealthCheckResult> getPassedChecks();
    
    static void resetHealthChecker();
    
private:
    static bool isInitialized;
    static bool healthChecksEnabled;
    static size_t maxHistorySize;
    static std::vector<HealthCheckResult> checkHistory;
    static std::function<void(const HealthCheckResult&)> healthCallback;
    static std::mutex healthMutex;
    
    static std::vector<std::function<HealthCheckResult()>> customHealthChecks;
    static std::chrono::steady_clock::time_point lastCheckTime;
    static double totalCheckTime;
    static size_t checkCount;
    
    static double getCurrentMemoryUsage();
    static float getCurrentCPUUsage();
    static size_t getCurrentConnectionCount();
    static size_t getCurrentEventCount();
};

// Configuration validation
class ConfigurationValidator {
public:
    struct ConfigValidationResult {
        bool isValid;
        std::string parameter;
        std::string value;
        std::string message;
        std::chrono::steady_clock::time_point timestamp;
        std::string category;
        std::vector<std::string> errors;
        std::vector<std::string> warnings;
        
        ConfigValidationResult() : isValid(true), timestamp(std::chrono::steady_clock::now()) {}
        ConfigValidationResult(const std::string& param, const std::string& val, bool valid, const std::string& msg)
            : isValid(valid), parameter(param), value(val), message(msg), timestamp(std::chrono::steady_clock::now()) {}
    };
    
    static ConfigValidationResult validateParameter(const std::string& key, const std::string& value);
    static ConfigValidationResult validateAll(const Config& config);
    
    static bool isConfigurationValid(const Config& config);
    static std::string getConfigurationValidationReport(const Config& config);
    
    static std::vector<ConfigValidationResult> getValidationResults();
    static std::vector<ConfigValidationResult> getValidationResults(const std::string& category);
    static std::vector<ConfigValidationResult> getValidationResults(const std::string& category, const std::string& parameter);
    
    static size_t getValidationCount();
    static size_t getValidationCount(const std::string& category);
    static size_t getFailedValidationCount();
    static size_t getPassedValidationCount();
    
    static void clearValidationResults();
    static void clearValidationResults(const std::string& category);
    
    static std::string getValidationSummary();
    
    static void setValidationCallback(std::function<void(const ConfigValidationResult&)> callback);
    static void clearValidationCallback();
    
    static void enableValidation(bool enable);
    static bool isValidationEnabled();
    
    static void addValidationRule(const std::string& rule);
    static void removeValidationRule(const std::string& rule);
    static void clearValidationRules();
    
    static void setMaxValidationHistorySize(size_t maxSize);
    static size_t getMaxValidationHistorySize();
    
    static std::chrono::steady_clock::time_point getLastValidationTime();
    static double getAverageValidationTime();
    
    static std::vector<ConfigValidationResult> getRecentValidations(size_t count = 10);
    static std::vector<ConfigValidationResult> getFailedValidations();
    static std::vector<ConfigValidationResult> getPassedValidations();
    
    static void resetConfigurationValidator();
    
private:
    static bool isInitialized;
    static bool validationEnabled;
    static size_t maxHistorySize;
    static std::vector<ConfigValidationResult> validationResults;
    static std::function<void(const ConfigValidationResult&)> validationCallback;
    static std::mutex validationMutex;
    
    static std::vector<std::function<bool(const std::string&, const std::string&)>> validationRules;
    static std::chrono::steady_clock::time_point lastValidationTime;
    static double totalValidationTime;
    static size_t validationCount;
    
    static bool validateKeyFormat(const std::string& key);
    static bool validateValueFormat(const std::string& value, const std::string& key);
    static bool validateParameterRange(const std::string& key, const std::string& value);
    static bool validateParameterDependencies(const Config& config);
    static std::string formatValidationError(const std::string& message, const std::string& category);
};

// Debug utilities
class DebugUtilities {
public:
    // System information
    static std::string getSystemInfo();
    static std::string getProcessInfo();
    static std::string getMemoryInfo();
    static std::string getCPUInfo();
    static std::string getNetworkInfo();
    static std::string getFileSystemInfo();
    
    // Time and date utilities
    static std::string getCurrentTimestamp();
    static std::string getFormattedTime(const std::chrono::steady_clock::time_point& timePoint);
    static double getUptime();
    static std::string getRelativeTime(double seconds);
    
    // Memory utilities
    static std::string formatBytes(uint64_t bytes);
    static std::string formatKilobytes(uint64_t kilobytes);
    static std::string formatMegabytes(uint64_t megabytes);
    static std::string formatGigabytes(uint64_t gigabytes);
    
    // Performance utilities
    static std::string formatMilliseconds(double milliseconds);
    static std::string formatSeconds(double seconds);
    static std::string formatMinutes(double minutes);
    static std::string formatHours(double hours);
    static std::string formatPercentage(float value);
    
    // Debugging utilities
    static std::string getStackTrace();
    static void printStackTrace();
    static std::string getEnvironmentInfo();
    static void printSystemInfo();
    
    // File utilities
    static bool fileExists(const std::string& filepath);
    static std::string getFileSize(const std::string& filepath);
    static std::string getLastModifiedTime(const std::string& filepath);
    static std::vector<std::string> listDirectory(const std::string& path);
    
    // Network utilities
    static bool isNetworkAvailable();
    static std::string getLocalIPAddress();
    static bool isPortAvailable(int port);
    
    // Configuration utilities
    static std::string getConfigFilePath();
    static bool isConfigFileValid();
    static std::string getConfigSummary();
    
    // Debug output
    static void setDebugOutput(std::ostream& output);
    static void enableDebugOutput(bool enable);
    static bool isDebugOutputEnabled();
    static void setDebugFile(const std::string& filepath);
    static void closeDebugFile();
    
    // Debug logging
    static void debugLog(const std::string& message, const std::string& category = "Debug");
    static void infoLog(const std::string& message, const std::string& category = "Info");
    static void warningLog(const std::string& message, const std::string& category = "Warning");
    static void errorLog(const std::string& message, const std::string& category = "Error");
    static void criticalLog(const std::string& message, const std::string& category = "Critical");
    
    // Debug snapshot
    static void takeSnapshot(const std::string& name);
    static void restoreSnapshot(const std::string& name);
    static void listSnapshots();
    static void clearSnapshots();
    
    // Debug script
    static void executeDebugScript(const std::string& script);
    static bool isDebugScriptFile(const std::string& filepath);
    
    // Debug visualization
    static void generateSystemReport(const std::string& outputFile = "");
    static void generatePerformanceReport(const std::string& outputFile = "");
    static void generateEventReport(const std::string& outputFile = "");
    static void generateConnectionReport(const std::string& outputFile = "");
    static void generateErrorReport(const std::string& outputFile = "");
    
    // Debug statistics
    static std::string getDebugStatistics();
    static void resetDebugStatistics();
    
    // Debug state
    static bool isDebugActive();
    static void setDebugActive(bool active);
    static std::string getDebugSessionId();
    static void setDebugSessionId(const std::string& sessionId);
    
    // Debug cleanup
    static void cleanup();
    static void shutdown();
    
    // Debug level
    enum DebugLevel {
        DEBUG_LEVEL_SILENT = -1,
        DEBUG_LEVEL_ERROR = 0,
        DEBUG_LEVEL_WARNING = 1,
        DEBUG_LEVEL_INFO = 2,
        DEBUG_LEVEL_DETAILED = 3,
        DEBUG_LEVEL_VERBOSE = 4,
        DEBUG_LEVEL_TRACE = 5
    };
    
    static void setDebugLevel(DebugLevel level);
    static DebugLevel getDebugLevel();
    static bool isDebugLevelEnabled(DebugLevel level);
    
    // Debug filtering
    static void addDebugFilter(const std::string& category);
    static void removeDebugFilter(const std::string& category);
    static void clearDebugFilters();
    static bool isCategoryFiltered(const std::string& category);
    
    static void setDebugLevelForCategory(const std::string& category, DebugLevel level);
    static DebugLevel getDebugLevelForCategory(const std::string& category);
    
    // Debug configuration
    static void loadDebugConfig(const std::string& configFile);
    static void saveDebugConfig(const std::string& configFile);
    static void resetDebugConfig();
    
    static std::string getDebugConfig();
    static bool isDebugConfigValid();
    
    static void setDebugConfigValue(const std::string& key, const std::string& value);
    static std::string getDebugConfigValue(const std::string& key);
    
    // Debug help
    static std::string getDebugHelp();
    static void printDebugHelp();
    
    static std::string getDebugCommands();
    static void printDebugCommands();
    
    static std::string getDebugExamples();
    static void printDebugExamples();
    
    // Debug initialization
    static void initialize();
    static bool isInitialized();
    
    static void cleanup();
    static void shutdown();
    
private:
    static bool isInitialized;
    static bool debugActive;
    static std::string debugSessionId;
    static DebugLevel debugLevel;
    static std::unordered_map<std::string, DebugLevel> categoryDebugLevels;
    static std::unordered_set<std::string> debugFilters;
    static std::map<std::string, std::string> debugConfig;
    static std::map<std::string, std::string> snapshots;
    static std::ostream* debugOutput;
    static bool debugOutputEnabled;
    static std::string debugFilepath;
    static std::mutex debugMutex;
    
    static std::string getSystemName();
    static std::string getSystemVersion();
    static std::string getSystemArchitecture();
    static std::string getCompilerInfo();
    static std::string getBuildInfo();
    static std::string getEnvironmentInfo();
    
    static bool isDebugFeatureAvailable(const std::string& feature);
    static void enableDebugFeature(const std::string& feature);
    static void disableDebugFeature(const std::string& feature);
    
    static void updateDebugLevel();
    static bool shouldLog(const std::string& category, DebugLevel level);
    static std::string formatDebugMessage(const std::string& message, const std::string& category, DebugLevel level);
    
    static void saveDebugState();
    static void restoreDebugState();
    static void clearDebugState();
};

// Global instances
extern std::unique_ptr<ConfigValidator> gConfigValidator;
extern std::unique_ptr<ErrorHandler> gErrorHandler;
extern std::unique_ptr<ConnectionTracker> gConnectionTracker;
extern std::unique_ptr<EventLogger> gEventLogger;
extern std::unique_ptr<PerformanceMonitor> gPerformanceMonitor;
extern std::unique_ptr<SystemHealthChecker> gSystemHealthChecker;
extern std::unique_ptr<DebugUtilities> gDebugUtilities;

// Global initialization and cleanup
void initializeDebugging();
void cleanupDebugging();
void shutdownDebugging();

// Global configuration
void configureDebugging(const std::string& configFile);
void resetDebuggingConfig();

// Global status
bool isDebuggingEnabled();
std::string getDebugStatus();
std::string getDebugSessionId();

// Global reports
std::string generateDebugReport();
std::string generateHealthReport();
std::string generatePerformanceReport();
std::string generateEventReport();
std::string generateConnectionReport();
std::string generateErrorReport();

// Global debugging commands
void executeDebugCommand(const std::string& command);
void listDebugCommands();
void printDebugHelp();

} // namespace nlm
