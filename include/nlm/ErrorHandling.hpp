#pragma once

#include <string>
#include <map>
#include <memory>
#include <functional>
#include <mutex>
#include <chrono>
#include <unordered_map>
#include <iomanip>
#include <sstream>
#include <vector>
#include <thread>
#include <algorithm>
#include <numeric>
#include <cmath>
#include "../src/core/Types/Types.hpp"

namespace nlm {

// Custom exception hierarchy for better error handling
class NLMException : public std::runtime_error {
public:
    explicit NLMException(const std::string& message, const std::string& context = "")
        : std::runtime_error(formatMessage(message, context)), 
          context_(context), timestamp_(std::chrono::system_clock::now()) {}
    
    const std::string& getContext() const { return context_; }
    std::chrono::system_clock::time_point getTimestamp() const { return timestamp_; }
    std::string getFormattedTimestamp() const {
        auto time = std::chrono::system_clock::to_time_t(timestamp_);
        std::stringstream ss;
        ss << std::put_time(std::localtime(&time), "%Y-%m-%d %H:%M:%S");
        return ss.str();
    }
    
    virtual std::string getErrorType() const { return "NLMException"; }
    
protected:
    std::string formatMessage(const std::string& message, const std::string& context) {
        if (context.empty()) {
            return message;
        }
        return "[Context: " + context + "] " + message;
    }
    
private:
    std::string context_;
    std::chrono::system_clock::time_point timestamp_;
};

class ConfigException : public NLMException {
public:
    explicit ConfigException(const std::string& message, const std::string& context = "")
        : NLMException("Configuration Error: " + message, context) {}
    
    std::string getErrorType() const override { return "ConfigException"; }
};

class BrainException : public NLMException {
public:
    explicit BrainException(const std::string& message, const std::string& context = "")
        : NLMException("Brain Error: " + message, context) {}
    
    std::string getErrorType() const override { return "BrainException"; }
};

class AgentException : public NLMException {
public:
    explicit AgentException(const std::string& message, const std::string& context = "")
        : NLMException("Agent Error: " + message, context) {}
    
    std::string getErrorType() const override { return "AgentException"; }
};

class MemoryException : public NLMException {
public:
    explicit MemoryException(const std::string& message, const std::string& context = "")
        : NLMException("Memory Error: " + message, context) {}
    
    std::string getErrorType() const override { return "MemoryException"; }
};

class PlasticityException : public NLMException {
public:
    explicit PlasticityException(const std::string& message, const std::string& context = "")
        : NLMException("Plasticity Error: " + message, context) {}
    
    std::string getErrorType() const override { return "PlasticityException"; }
};

// Error context classes for detailed error reporting
class ErrorContext {
public:
    ErrorContext(const std::string& operation, const std::string& component = "")
        : operation_(operation), component_(component), 
          startTime_(std::chrono::system_clock::now()) {}
    
    void setParameter(const std::string& key, const std::string& value) {
        parameters_[key] = value;
    }
    
    void setParameter(const std::string& key, int value) {
        parameters_[key] = std::to_string(value);
    }
    
    void setParameter(const std::string& key, double value) {
        std::stringstream ss;
        ss << std::fixed << std::setprecision(6) << value;
        parameters_[key] = ss.str();
    }
    
    void setParameter(const std::string& key, const std::string& value, const std::string& type) {
        parameters_[key] = "[" + type + "] " + value;
    }
    
    template<typename T>
    void setParameter(const std::string& key, const T& value) {
        setParameter(key, std::to_string(value));
    }
    
    const std::map<std::string, std::string>& getParameters() const { return parameters_; }
    std::chrono::duration<double> getDuration() const {
        return std::chrono::system_clock::now() - startTime_;
    }
    
    std::string getSummary() const {
        std::stringstream ss;
        ss << "Operation: " << operation_;
        if (!component_.empty()) {
            ss << " (Component: " << component_ << ")";
        }
        ss << "\nDuration: " << getDuration().count() << " seconds\n";
        ss << "Parameters:";
        for (const auto& param : parameters_) {
            ss << "\n  " << param.first << " = " << param.second;
        }
        return ss.str();
    }
    
private:
    std::string operation_;
    std::string component_;
    std::chrono::system_clock::time_point startTime_;
    std::map<std::string, std::string> parameters_;
};

// Enhanced error reporting and logging system
class ErrorReporter {
public:
    enum class LogLevel {
        Debug,
        Info,
        Warning,
        Error,
        Critical
    };
    
    struct LogEntry {
        std::chrono::system_clock::time_point timestamp;
        LogLevel level;
        std::string message;
        std::string context;
        std::map<std::string, std::string> metadata;
    };
    
    ErrorReporter() : enableConsoleOutput_(true), enableFileOutput_(false) {}
    
    void enableConsoleOutput(bool enable) { enableConsoleOutput_ = enable; }
    void enableFileOutput(bool enable, const std::string& filepath) {
        enableFileOutput_ = enable;
        logFilePath_ = filepath;
    }
    
    void log(const std::string& message, LogLevel level = LogLevel::Info,
             const std::string& context = "", const std::map<std::string, std::string>& metadata = {});
    
    std::vector<LogEntry> getLogs(LogLevel minLevel = LogLevel::Debug) const;
    
    void clearLogs() { logs_.clear(); }
    
    std::string formatLogs(LogLevel minLevel = LogLevel::Debug) const;
    
    // Exception handling utilities
    template<typename Func>
    auto executeWithErrorHandling(const std::string& operation,
                                 const std::string& component,
                                 const std::map<std::string, std::string>& params,
                                 Func&& func) {
        ErrorContext context(operation, component);
        for (const auto& param : params) {
            context.setParameter(param.first, param.second);
        }
        
        try {
            auto result = func();
            log("Successfully completed operation: " + operation, LogLevel::Info, component, params);
            return result;
        } catch (const NLMException& e) {
            log(e.what(), LogLevel::Error, component, params);
            throw;
        } catch (const std::exception& e) {
            std::string errorMsg = "Unexpected error: " + std::string(e.what());
            log(errorMsg, LogLevel::Critical, component, params);
            throw NLMException(errorMsg, context.getSummary());
        }
    }
    
    // Performance monitoring
    void recordOperation(const std::string& operation, const std::string& component,
                        std::chrono::duration<double> duration,
                        bool success = true,
                        const std::map<std::string, std::string>& metadata = {});
    
    std::string getPerformanceStats() const;
    
private:
    bool enableConsoleOutput_;
    bool enableFileOutput_;
    std::string logFilePath_;
    std::vector<LogEntry> logs_;
    mutable std::mutex logMutex_;
    std::unordered_map<std::string, std::vector<std::chrono::duration<double>>> operationStats_;
};

// Enhanced validation utilities
class Validator {
public:
    struct ValidationResult {
        bool valid;
        std::vector<std::string> errors;
        std::vector<std::string> warnings;
        std::string context;
        
        ValidationResult() : valid(true) {}
        
        void addError(const std::string& error) {
            valid = false;
            errors.push_back(error);
        }
        
        void addWarning(const std::string& warning) {
            warnings.push_back(warning);
        }
        
        std::string getSummary() const {
            std::stringstream ss;
            ss << "Validation " << (valid ? "PASSED" : "FAILED");
            if (!context.empty()) {
                ss << " for " << context;
            }
            ss << "\nErrors: " << errors.size();
            ss << "\nWarnings: " << warnings.size();
            if (!errors.empty()) {
                ss << "\nError details:";
                for (const auto& error : errors) {
                    ss << "\n  - " << error;
                }
            }
            if (!warnings.empty()) {
                ss << "\nWarning details:";
                for (const auto& warning : warnings) {
                    ss << "\n  - " << warning;
                }
            }
            return ss.str();
        }
    };
    
    // Validation functions for different components
    static ValidationResult validateConfig(const class Config& config);
    static ValidationResult validateBrain(const class Brain& brain);
    static ValidationResult validateAgent(const class AgentBrain& agent);
    static ValidationResult validateWorld(const class SimpleWorld& world);
    
    // Parameter validation
    static bool isValidRange(double value, double min, double max, bool inclusive = true);
    static bool isValidRange(int value, int min, int max, bool inclusive = true);
    static bool isValidString(const std::string& value, size_t minLength = 0, size_t maxLength = 0);
    static bool isValidPositive(double value, bool includeZero = false);
    static bool isValidPositive(int value, bool includeZero = false);
    
    // Neural-specific validation
    static bool isValidNeuronType(NeuronType type);
    static bool isValidSynapseType(SynapseType type);
    static bool isValidActionType(ActionType type);
    static bool isValidWorldObjectType(WorldObjectType type);
    
    // Performance validation
    static bool isValidPerformanceThreshold(double threshold, double maxThreshold = 1000.0);
    static bool isValidTimeStep(double timestep, double maxTimestep = 1.0);
    
    // Advanced validation
    static ValidationResult validateLearningConfiguration(const std::map<std::string, double>& config);
    static ValidationResult validateMemoryConfiguration(const std::map<std::string, int>& config);
    static ValidationResult validatePlasticityConfiguration(const std::map<std::string, double>& config);
};

// Enhanced event system for error handling and logging
class EventSystem {
public:
    enum class EventType {
        ConfigurationChanged,
        BrainInitialized,
        BrainReset,
        AgentInitialized,
        AgentReset,
        ErrorOccurred,
        PerformanceWarning,
        LearningCompleted,
        DevelopmentCompleted
    };
    
    struct Event {
        std::chrono::system_clock::time_point timestamp;
        EventType type;
        std::string source;
        std::string message;
        std::map<std::string, std::string> data;
        std::map<std::string, std::string> metadata;
    };
    
    using EventCallback = std::function<void(const Event&)>;
    
    EventSystem() : maxEvents_(1000), enablePersistentStorage_(false) {}
    
    void subscribe(EventType eventType, EventCallback callback);
    void unsubscribe(EventType eventType, EventCallback callback);
    void unsubscribeAll(EventType eventType);
    
    void publish(EventType eventType, const std::string& source,
                 const std::string& message,
                 const std::map<std::string, std::string>& data = {},
                 const std::map<std::string, std::string>& metadata = {});
    
    std::vector<Event> getEvents(EventType eventType = EventType::ErrorOccurred,
                                std::chrono::system_clock::time_point since = {}) const;
    
    void clearEvents(EventType eventType = EventType::ErrorOccurred);
    
    void enablePersistentStorage(bool enable, const std::string& filepath = "");
    
    std::string getEventHistory(EventType eventType = EventType::ErrorOccurred) const;
    
private:
    std::unordered_map<EventType, std::vector<EventCallback>> subscribers_;
    std::vector<Event> events_;
    mutable std::mutex eventMutex_;
    size_t maxEvents_;
    bool enablePersistentStorage_;
    std::string storageFilePath_;
    
    void cleanupOldEvents();
    void persistEvents() const;
    void loadEvents();
};

// Enhanced resource management with cleanup tracking
class ResourceManager {
public:
    struct ResourceInfo {
        std::string name;
        std::string type;
        std::chrono::system_clock::time_point created;
        std::chrono::system_clock::time_point lastUsed;
        size_t memoryUsage;
        std::map<std::string, std::string> metadata;
    };
    
    ResourceManager() : enableAutoCleanup_(true), maxMemoryUsage_(1024 * 1024 * 1024) {} // 1GB default
    
    template<typename T, typename... Args>
    std::shared_ptr<T> createResource(const std::string& name, const std::string& type, Args&&... args);
    
    bool releaseResource(const std::string& name);
    bool releaseResourceByType(const std::string& type);
    bool releaseAllResources();
    
    std::shared_ptr<ResourceInfo> getResourceInfo(const std::string& name) const;
    std::vector<ResourceInfo> getAllResources() const;
    
    void setAutoCleanup(bool enable) { enableAutoCleanup_ = enable; }
    void setMaxMemoryUsage(size_t maxBytes) { maxMemoryUsage_ = maxBytes; }
    
    size_t getTotalMemoryUsage() const;
    size_t getResourceCount() const { return resources_.size(); }
    
    std::string getResourceReport() const;
    
    void cleanupUnusedResources(std::chrono::hours maxIdleTime = std::chrono::hours(1));
    
private:
    std::unordered_map<std::string, std::shared_ptr<ResourceInfo>> resources_;
    mutable std::mutex resourceMutex_;
    bool enableAutoCleanup_;
    size_t maxMemoryUsage_;
    
    template<typename T>
    std::shared_ptr<T> getResourceAs(const std::string& name) const;
    
    void checkMemoryLimits();
};

// Advanced configuration manager with schema validation
class AdvancedConfigManager {
public:
    struct ConfigurationSchema {
        std::string name;
        std::string description;
        std::map<std::string, std::string> parameterTypes;
        std::map<std::string, std::pair<double, double>> valueRanges;
        std::map<std::string, std::string> defaultValues;
        std::map<std::string, std::string> constraints;
        std::map<std::string, std::string> validationRules;
    };
    
    AdvancedConfigManager() : enableSchemaValidation_(true), enableChangeTracking_(true) {}
    
    void enableSchemaValidation(bool enable) { enableSchemaValidation_ = enable; }
    void enableChangeTracking(bool enable) { enableChangeTracking_ = enable; }
    
    void registerSchema(const std::string& name, const ConfigurationSchema& schema);
    bool validateConfiguration(const std::map<std::string, std::string>& config,
                               const std::string& schemaName) const;
    
    std::map<std::string, std::string> getValidatedConfig(const std::map<std::string, std::string>& config,
                                                         const std::string& schemaName,
                                                         const std::map<std::string, std::string>& defaults = {}) const;
    
    std::vector<std::string> getAvailableSchemas() const;
    
    // Configuration history and rollback
    void enableHistory(bool enable, size_t maxHistorySize = 100);
    std::vector<std::map<std::string, std::string>> getConfigurationHistory() const;
    bool rollbackConfiguration(const std::map<std::string, std::string>& config);
    
    // Advanced configuration operations
    std::map<std::string, std::string> mergeConfigurations(
        const std::map<std::string, std::string>& base,
        const std::map<std::string, std::string>& override,
        const std::string& schemaName = "") const;
    
    std::map<std::string, std::string> interpolateConfiguration(
        const std::map<std::string, std::string>& config,
        const std::map<std::string, double>& variables) const;
    
    std::string getConfigurationDiff(
        const std::map<std::string, std::string>& config1,
        const std::map<std::string, std::string>& config2) const;
    
private:
    std::unordered_map<std::string, ConfigurationSchema> schemas_;
    std::vector<std::map<std::string, std::string>> configurationHistory_;
    mutable std::mutex configMutex_;
    bool enableSchemaValidation_;
    bool enableChangeTracking_;
    bool enableHistory_;
    size_t maxHistorySize_;
};

// Enhanced metrics and statistics collector
class MetricsCollector {
public:
    struct PerformanceMetrics {
        std::chrono::system_clock::time_point timestamp;
        std::string operation;
        std::string component;
        double duration;  // in seconds
        double cpuUsage;  // percentage
        double memoryUsage;  // in MB
        bool success;
        std::map<std::string, double> customMetrics;
        std::string errorMessage;
    };
    
    MetricsCollector() : sampleInterval_(std::chrono::seconds(1)), 
                         maxHistorySize_(10000), enableRealTimeTracking_(false) {}
    
    void setSampleInterval(std::chrono::seconds interval) { sampleInterval_ = interval; }
    void setMaxHistorySize(size_t size) { maxHistorySize_ = size; }
    void enableRealTimeTracking(bool enable) { enableRealTimeTracking_ = enable; }
    
    void recordMetric(const PerformanceMetrics& metric);
    PerformanceMetrics recordOperation(const std::string& operation,
                                       const std::string& component,
                                       const std::function<void()>& func,
                                       bool success = true,
                                       const std::map<std::string, double>& customMetrics = {});
    
    std::vector<PerformanceMetrics> getMetrics(const std::string& operation = "",
                                              const std::string& component = "",
                                              std::chrono::system_clock::time_point since = {}) const;
    
    double getAverageDuration(const std::string& operation,
                             const std::string& component = "",
                             std::chrono::system_clock::time_point since = {}) const;
    
    double getSuccessRate(const std::string& operation,
                         const std::string& component = "",
                         std::chrono::system_clock::time_point since = {}) const;
    
    std::string getPerformanceReport(const std::string& operation = "",
                                    const std::string& component = "",
                                    std::chrono::system_clock::time_point since = {}) const;
    
    // Advanced analytics
    double calculateMovingAverage(const std::string& operation,
                                  const std::string& component,
                                  size_t windowSize = 100,
                                  std::chrono::system_clock::time_point since = {}) const;
    
    std::vector<double> getMovingAverageSeries(const std::string& operation,
                                              const std::string& component = "",
                                              size_t windowSize = 100,
                                              std::chrono::system_clock::time_point since = {}) const;
    
    // Anomaly detection
    std::vector<PerformanceMetrics> detectAnomalies(const std::string& operation,
                                                    const std::string& component = "",
                                                    double threshold = 2.0,
                                                    std::chrono::system_clock::time_point since = {}) const;
    
private:
    std::vector<PerformanceMetrics> metrics_;
    mutable std::mutex metricsMutex_;
    std::chrono::seconds sampleInterval_;
    size_t maxHistorySize_;
    bool enableRealTimeTracking_;
    
    double calculateStandardDeviation(const std::vector<double>& values) const;
    double calculateMean(const std::vector<double>& values) const;
};

// Comprehensive error handling and recovery system
class ErrorRecoverySystem {
public:
    enum class RecoveryStrategy {
        None,
        Retry,
        Fallback,
        GracefulDegradation,
        ResetToDefault,
        Abort
    };
    
    struct RecoveryOperation {
        std::string name;
        RecoveryStrategy strategy;
        std::chrono::seconds maxRetryDelay;
        int maxRetries;
        std::function<bool()> condition;
        std::function<void()> action;
        std::map<std::string, std::string> parameters;
    };
    
    ErrorRecoverySystem() : defaultStrategy_(RecoveryStrategy::Retry),
                           enableAutoRecovery_(true),
                           maxRecoveryTime_(std::chrono::minutes(5)) {}
    
    void setDefaultStrategy(RecoveryStrategy strategy) { defaultStrategy_ = strategy; }
    void enableAutoRecovery(bool enable) { enableAutoRecovery_ = enable; }
    void setMaxRecoveryTime(std::chrono::seconds time) { maxRecoveryTime_ = time; }
    
    void addRecoveryOperation(const RecoveryOperation& operation);
    void removeRecoveryOperation(const std::string& name);
    
    bool executeRecovery(const std::string& errorContext,
                        const std::string& errorMessage,
                        const std::map<std::string, std::string>& context = {});
    
    void setRecoveryStrategy(const std::string& operation, RecoveryStrategy strategy);
    RecoveryStrategy getRecoveryStrategy(const std::string& operation) const;
    
    // Recovery history and statistics
    std::vector<std::pair<std::chrono::system_clock::time_point, std::string>> 
    getRecoveryHistory(const std::string& operation = "") const;
    
    std::string getRecoveryStatistics() const;
    
private:
    std::unordered_map<std::string, RecoveryOperation> recoveryOperations_;
    std::unordered_map<std::string, RecoveryStrategy> operationStrategies_;
    std::vector<std::pair<std::chrono::system_clock::time_point, std::string>> recoveryHistory_;
    mutable std::mutex recoveryMutex_;
    
    RecoveryStrategy determineStrategy(const std::string& errorContext,
                                       const std::string& errorMessage,
                                       const std::map<std::string, std::string>& context) const;
    bool executeRecoveryOperation(const RecoveryOperation& operation,
                                 const std::string& errorContext,
                                 const std::string& errorMessage);
};

} // namespace nlm

// Convenience functions for common error handling patterns
inline std::string safeExecute(const std::string& operation,
                               const std::function<void()>& func,
                               const std::string& errorContext = "") {
    try {
        func();
        return "Success";
    } catch (const std::exception& e) {
        std::string errorMsg = "Operation failed: " + std::string(e.what());
        if (!errorContext.empty()) {
            errorMsg += " (Context: " + errorContext + ")";
        }
        throw nlm::NLMException(errorMsg, errorContext);
    }
}

inline std::string safeExecuteWithRetry(const std::string& operation,
                                        const std::function<void()>& func,
                                        int maxRetries = 3,
                                        std::chrono::seconds retryDelay = std::chrono::seconds(1),
                                        const std::string& errorContext = "") {
    for (int attempt = 0; attempt < maxRetries; ++attempt) {
        try {
            func();
            if (attempt > 0) {
                return "Success after " + std::to_string(attempt + 1) + " attempts";
            }
            return "Success";
        } catch (const std::exception& e) {
            if (attempt == maxRetries - 1) {
                std::string errorMsg = "Operation failed after " + std::to_string(maxRetries) +
                                    " attempts: " + std::string(e.what());
                if (!errorContext.empty()) {
                    errorMsg += " (Context: " + errorContext + ")";
                }
                throw nlm::NLMException(errorMsg, errorContext);
            }
            std::this_thread::sleep_for(retryDelay * (attempt + 1)); // Exponential backoff
        }
    }
    return "Success";
}