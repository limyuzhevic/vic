NLM Error Handling and Robustness Enhancement
===============================================

## Overview

This document details the comprehensive error handling and robustness improvements for the NLM (Neural Learning Machine) project, including error classification, recovery mechanisms, graceful degradation, and improved validation.

## Changes Made

### 1. Enhanced Error Classification

#### Error Types and Categories
```cpp
enum class NLMErrorType {
    // Configuration Errors
    CONFIG_FILE_NOT_FOUND,
    CONFIG_FILE_READ_ERROR,
    CONFIG_FILE_WRITE_ERROR,
    CONFIG_SYNTAX_ERROR,
    CONFIG_TYPE_MISMATCH,
    CONFIG_RANGE_VIOLATION,
    CONFIG_MISSING_REQUIRED_FIELD,
    CONFIG_INVALID_DEPENDENCY,
    CONFIG_INCONSISTENT,
    
    // Brain/Neural Errors
    BRAIN_INITIALIZATION_ERROR,
    BRAIN_STEP_ERROR,
    BRAIN_RESET_ERROR,
    BRAIN_CONNECTION_ERROR,
    BRAIN_SYNAPSE_ERROR,
    BRAIN_NEURON_ERROR,
    BRAIN_REGION_ERROR,
    BRAIN_POPULATION_ERROR,
    
    // Memory Errors
    MEMORY_ALLOCATION_ERROR,
    MEMORY_CORRUPTION_ERROR,
    MEMORY_ACCESS_VIOLATION,
    MEMORY_POOL_EXHAUSTION,
    
    // Plasticity Errors
    PLASTICITY_RULE_ERROR,
    STDP_ERROR,
    HEBBIAN_ERROR,
    STRUCTURAL_PLASTICITY_ERROR,
    
    // Neuromodulation Errors
    NEUROMODULATION_ERROR,
    DOPAMINE_ERROR,
    CURIOUSITY_ERROR,
    NOVELTY_ERROR,
    PREDICTION_ERROR,
    
    // Development Errors
    DEVELOPMENT_ERROR,
    CRITICAL_PERIOD_ERROR,
    MATURATION_ERROR,
    AGING_ERROR,
    
    // Simulation Errors
    SIMULATION_STEP_ERROR,
    TIMESTEP_ERROR,
    SIMULATION_TIMEOUT,
    SIMULATION_OVERFLOW,
    
    // I/O Errors
    FILE_IO_ERROR,
    SERIALIZATION_ERROR,
    DESERIALIZATION_ERROR,
    CHECKPOINT_ERROR,
    
    // Resource Errors
    MEMORY_ALLOCATION_FAILED,
    FILE_DESCRIPTOR_EXHAUSTED,
    THREAD_POOL_EXHAUSTED,
    GPU_MEMORY_EXHAUSTED,
    
    // Network Errors
    NETWORK_CONNECTION_ERROR,
    NETWORK_TIMEOUT,
    NETWORK_PROTOCOL_ERROR,
    
    // System Errors
    SYSTEM_CALL_ERROR,
    PERMISSION_ERROR,
    SECURITY_VIOLATION,
    
    // Unknown/Unexpected Errors
    UNKNOWN_ERROR,
    UNEXPECTED_ERROR,
    UNHANDLED_EXCEPTION
};

struct NLMError {
    NLMErrorType type;
    std::string message;
    std::string context;
    std::string suggested_fix;
    std::string file_path;
    int line_number;
    int column_number;
    std::chrono::system_clock::time_point timestamp;
    std::shared_ptr<NLMError> inner_error; // For chained exceptions
    std::vector<std::string> recovery_actions;
    
    // Error severity levels
    enum Severity {
        INFO,           // Information only, not an error
        WARNING,        // Non-critical issue, system continues
        RECOVERABLE,    // Error that can be recovered from
        NON_RECOVERABLE, // Error that prevents operation
        CRITICAL,       // System-critical error
        FATAL           // System cannot continue
    };
    
    Severity severity;
    
    NLMError(NLMErrorType t, const std::string& msg, Severity sev = Severity::WARNING)
        : type(t), message(msg), severity(sev), line_number(-1), column_number(-1) {
        timestamp = std::chrono::system_clock::now();
    }
    
    std::string toString() const;
    std::string toDetailedString() const;
    std::string getFormattedMessage() const;
    bool isRecoverable() const;
    bool isCritical() const;
    bool isFatal() const;
    void addRecoveryAction(const std::string& action);
    void setContext(const std::string& ctx) { context = ctx; }
    void setSuggestedFix(const std::string& fix) { suggested_fix = fix; }
    void setFileLocation(const std::string& path, int line = -1, int col = -1) {
        file_path = path;
        line_number = line;
        column_number = col;
    }
};
```

### 2. Enhanced Error Handling Framework

#### Error Handler Interface
```cpp
class ErrorHandler {
public:
    virtual ~ErrorHandler() = default;
    
    // Handle different error types
    virtual void handleError(const NLMError& error) = 0;
    virtual void handleWarning(const std::string& message) = 0;
    virtual void handleInfo(const std::string& message) = 0;
    
    // Error handling strategies
    enum Strategy {
        ABORT,              // Stop execution immediately
        LOG_ONLY,           // Log error and continue
        USE_DEFAULTS,       // Use default values and continue
        RETRY,              // Retry operation (with limits)
        FALLBACK,           // Use fallback implementation
        SKIP,               // Skip problematic operation
        PROMPT_USER,        // Ask user for action
        MODIFY_CONFIGURATION // Modify configuration automatically
    };
    
    // Configure error handling
    virtual void setErrorHandlingStrategy(NLMErrorType error_type, 
                                          Strategy strategy) = 0;
    virtual void setMaxRetries(int max_retries) = 0;
    virtual void setRetryDelay(double delay) = 0;
    
    // Error filtering and reporting
    virtual void setReportErrors(bool report) = 0;
    virtual void setErrorFilter(const std::function<bool(const NLMError&)>& filter) = 0;
    virtual bool shouldReport(const NLMError& error) const = 0;
    
    // Error recovery
    virtual bool canRecover(const NLMError& error) const = 0;
    virtual bool performRecovery(const NLMError& error) = 0;
    
    // Error statistics
    virtual int getErrorCount(NLMErrorType type = NLMErrorType::UNKNOWN_ERROR) const = 0;
    virtual int getWarningCount() const = 0;
    virtual double getErrorRate() const = 0;
    virtual std::string getErrorStatistics() const = 0;
};
```

#### Default Error Handler Implementation
```cpp
class DefaultErrorHandler : public ErrorHandler {
private:
    std::map<NLMErrorType, Strategy> error_strategies;
    std::map<Severity, std::string> severity_prefixes;
    std::vector<NLMError> error_history;
    std::mutex error_mutex;
    
    std::atomic<int> total_errors;
    std::atomic<int> total_warnings;
    std::atomic<int> total_retries;
    std::chrono::system_clock::time_point session_start;
    
public:
    DefaultErrorHandler();
    
    void handleError(const NLMError& error) override;
    void handleWarning(const std::string& message) override;
    void handleInfo(const std::string& message) override;
    
    void setErrorHandlingStrategy(NLMErrorType error_type, Strategy strategy) override;
    void setMaxRetries(int max_retries) override { max_retries_ = max_retries; }
    void setRetryDelay(double delay) override { retry_delay_ = delay; }
    
    void setReportErrors(bool report) override { report_errors_ = report; }
    void setErrorFilter(const std::function<bool(const NLMError&)>& filter) override {
        error_filter_ = filter;
    }
    bool shouldReport(const NLMError& error) const override {
        if (!error_filter_) return true;
        return error_filter_(error);
    }
    
    bool canRecover(const NLMError& error) const override;
    bool performRecovery(const NLMError& error) override;
    
    int getErrorCount(NLMErrorType type = NLMErrorType::UNKNOWN_ERROR) const override;
    int getWarningCount() const override { return total_warnings.load(); }
    double getErrorRate() const override;
    std::string getErrorStatistics() const override;
    
    // Error management
    void clearErrorHistory();
    std::vector<NLMError> getRecentErrors(size_t count = 10) const;
    std::vector<NLMError> getErrorsByType(NLMErrorType type) const;
    std::vector<NLMError> getErrorsBySeverity(Severity severity) const;
    
    // Configuration and state
    void configureFromConfig(const Config& config);
    void resetCounters();
    void saveErrorLog(const std::string& filepath) const;
    
    // Global error handler access
    static DefaultErrorHandler& getInstance() {
        static DefaultErrorHandler instance;
        return instance;
    }
    
    static void setGlobalErrorHandler(ErrorHandler* handler);
    static ErrorHandler* getGlobalErrorHandler();
    
private:
    void logError(const NLMError& error);
    void applyErrorStrategy(const NLMError& error);
    bool attemptRetry(const NLMError& error);
    void updateErrorStatistics(const NLMError& error);
    
    int max_retries_;
    double retry_delay_;
    bool report_errors_;
    std::function<bool(const NLMError&)> error_filter_;
    mutable std::mutex config_mutex;
};
```

### 3. Enhanced Exception Handling System

#### Error Management System
```cpp
class ErrorManager {
private:
    std::unique_ptr<ErrorHandler> error_handler_;
    std::map<std::string, std::function<void(const NLMError&)>> error_handlers;
    std::map<NLMErrorType, std::vector<std::function<bool(const NLMError&)>>> error_validators;
    std::mutex manager_mutex;
    
public:
    ErrorManager();
    ~ErrorManager();
    
    // Error reporting
    void reportError(const NLMError& error);
    void reportError(NLMErrorType type, const std::string& message, 
                    NLMError::Severity severity = NLMError::Severity::WARNING);
    void reportError(const std::exception& e, 
                    NLMErrorType type = NLMErrorType::UNEXPECTED_ERROR);
    
    // Error handling setup
    void setErrorHandler(std::unique_ptr<ErrorHandler> handler);
    void registerErrorHandler(const std::string& name,
                             std::function<void(const NLMError&)> handler);
    
    // Error validation
    void registerErrorValidator(NLMErrorType type,
                                std::function<bool(const NLMError&)> validator);
    bool validateError(const NLMError& error) const;
    
    // Error recovery
    bool attemptRecovery(const NLMError& error);
    bool canRecover(const NLMError& error) const;
    
    // Error context and correlation
    void setErrorContext(const std::string& context);
    std::string getCurrentContext() const;
    void addErrorContext(const std::string& context);
    
    // Error statistics and reporting
    int getErrorCount(NLMErrorType type = NLMErrorType::UNKNOWN_ERROR) const;
    int getWarningCount() const;
    std::string getSessionStatistics() const;
    void generateErrorReport(const std::string& filepath) const;
    
    // Configuration
    void configureFromConfig(const Config& config);
    void configureDefaultSettings();
    
    // Global error manager access
    static ErrorManager& getInstance() {
        static ErrorManager instance;
        return instance;
    }
    
    static void initialize(const Config& config = Config());
    static void shutdown();
    
    static ErrorManager& instance() { return getInstance(); }
};
```

### 4. Enhanced Error Recovery Mechanisms

#### Automatic Recovery Framework
```cpp
class ErrorRecovery {
public:
    enum RecoveryAction {
        NO_ACTION,
        RETRY,
        FALLBACK,
        SKIP_OPERATION,
        MODIFY_PARAMETERS,
        RESET_STATE,
        RELOAD_CONFIGURATION,
        CREATE_BACKUP,
        REQUEST_USER_INPUT,
        ABORT_OPERATION
    };
    
    struct RecoveryStrategy {
        NLMErrorType error_type;
        RecoveryAction action;
        int max_retries;
        double retry_delay;
        std::function<bool(const NLMError&)> condition;
        std::function<void()> on_success;
        std::function<void(const NLMError&)> on_failure;
    };
    
    // Recovery framework
    static bool attemptRecovery(const NLMError& error, 
                               RecoveryAction action = NO_ACTION);
    
    static bool canRecover(const NLMError& error,
                          RecoveryAction action = NO_ACTION);
    
    static RecoveryAction determineRecoveryAction(const NLMError& error);
    
    // Specific recovery operations
    static bool recoverFileError(const NLMError& error);
    static bool recoverBrainError(const NLMError& error);
    static bool recoverMemoryError(const NLMError& error);
    static bool recoverPlasticityError(const NLMError& error);
    static bool recoverNeuromodulationError(const NLMError& error);
    static bool recoverDevelopmentError(const NLMError& error);
    static bool recoverSimulationError(const NLMError& error);
    
    // Recovery strategies configuration
    static void addRecoveryStrategy(const RecoveryStrategy& strategy);
    static void removeRecoveryStrategy(NLMErrorType error_type);
    static void clearRecoveryStrategies();
    
    static std::vector<RecoveryStrategy> getRecoveryStrategies(NLMErrorType error_type);
    
    // Recovery statistics
    static int getSuccessfulRecoveries(NLMErrorType error_type);
    static int getFailedRecoveries(NLMErrorType error_type);
    static double getRecoverySuccessRate(NLMErrorType error_type);
    static std::string getRecoveryStatistics() const;
};
```

### 5. Graceful Degradation Framework

#### Degradation Management
```cpp
class GracefulDegradation {
public:
    enum DegradationLevel {
        FULL_FUNCTIONALITY,
        DEGRADED_FUNCTIONALITY,
        LIMITED_FUNCTIONALITY,
        MINIMAL_FUNCTIONALITY,
        EMERGENCY_MODE
    };
    
    struct DegradationRule {
        std::vector<NLMErrorType> error_types;
        DegradationLevel level;
        std::function<bool(const NLMError&)> condition;
        std::function<void()> on_enter;
        std::function<void()> on_exit;
    };
    
    // Degradation management
    static void setCurrentDegradationLevel(DegradationLevel level);
    static DegradationLevel getCurrentDegradationLevel();
    
    static bool shouldDegrade(const NLMError& error);
    static DegradationLevel determineDegradationLevel(const NLMError& error);
    
    static void addDegradationRule(const DegradationRule& rule);
    static void removeDegradationRule(const std::vector<NLMErrorType>& error_types);
    static void clearDegradationRules();
    
    static std::vector<DegradationRule> getDegradationRules();
    static DegradationLevel getDegradationLevelForError(const NLMError& error);
    
    // Functionality availability
    static bool isFeatureAvailable(const std::string& feature_name) const;
    static void disableFeature(const std::string& feature_name, 
                               const std::string& reason);
    static void enableFeature(const std::string& feature_name);
    
    // Degradation state management
    static std::string getDegradationState() const;
    static bool isDegraded() const;
    static std::vector<std::string> getActiveDegradationReasons() const;
    
    // Configuration
    static void configureFromConfig(const Config& config);
    static void configureDefaultDegradationRules();
    
    // Reporting
    static std::string getDegradationReport() const;
    static bool canContinueOperation() const;
};
```

### 6. Error Logging and Reporting

#### Enhanced Error Logging
```cpp
class ErrorLogger {
public:
    enum LogFormat {
        SIMPLE,
        DETAILED,
        JSON,
        XML,
        CSV,
        CUSTOM
    };
    
    enum LogDestination {
        CONSOLE,
        FILE,
        DATABASE,
        NETWORK,
        SYSLOG,
        STDOUT,
        STDERR
    };
    
    struct LogEntry {
        std::chrono::system_clock::time_point timestamp;
        LogDestination destination;
        std::string source_file;
        int line_number;
        std::string function_name;
        NLMError::Severity severity;
        std::string message;
        std::string formatted_message;
        std::map<std::string, std::string> metadata;
        std::vector<std::string> tags;
        std::shared_ptr<LogEntry> related_entry;
    };
    
    // Logging interface
    static void logError(const NLMError& error);
    static void logWarning(const std::string& message);
    static void logInfo(const std::string& message);
    static void logDebug(const std::string& message);
    
    // Log configuration
    static void configureLogging(const std::string& config_file = "nlm_error_log.json");
    static void setLogFormat(LogFormat format);
    static void setLogDestination(LogDestination destination);
    static void setLogFile(const std::string& filepath);
    static void setMaxLogSize(size_t max_size);
    static void setMaxLogFiles(size_t max_files);
    static void setLogRotation(bool enabled);
    static void setLogCompression(bool enabled);
    
    // Log management
    static void flushLogs();
    static void rotateLogs();
    static void clearLogs();
    static size_t getLogSize() const;
    static std::string getLogFile() const;
    
    // Log retrieval
    static std::vector<LogEntry> getRecentLogs(size_t count = 100);
    static std::vector<LogEntry> getLogsBySeverity(NLMError::Severity severity);
    static std::vector<LogEntry> getLogsByType(NLMErrorType type);
    static std::vector<LogEntry> getLogsByTimeRange(
        std::chrono::system_clock::time_point start,
        std::chrono::system_clock::time_point end);
    
    static std::string generateLogReport(const std::vector<LogEntry>& entries) const;
    static std::string generateLogReportBySeverity() const;
    static std::string generateLogReportByType() const;
    
    // Error log specific methods
    static void logErrorToSystem(const NLMError& error);
    static void logErrorToFile(const NLMError& error);
    static void logErrorToDatabase(const NLMError& error);
    static void logErrorToNetwork(const NLMError& error);
    
    // Log analysis
    static std::map<NLMErrorType, int> analyzeErrorFrequency() const;
    static std::map<NLMError::Severity, int> analyzeErrorSeverity() const;
    static std::vector<NLMErrorType> getMostFrequentErrors(size_t count = 10) const;
    static double getErrorTrend(NLMErrorType type) const;
};
```

### 7. Error Testing and Validation

#### Error Testing Framework
```cpp
class ErrorTestFramework {
public:
    struct TestCase {
        std::string name;
        std::function<bool()> test_function;
        std::function<void()> setup_function;
        std::function<void()> teardown_function;
        std::vector<std::string> expected_errors;
        std::vector<std::string> expected_warnings;
        double timeout;
    };
    
    // Test management
    static void addTestCase(const TestCase& test_case);
    static void removeTestCase(const std::string& name);
    static void clearTestCases();
    static std::vector<TestCase> getTestCases();
    static TestCase getTestCase(const std::string& name);
    
    // Test execution
    static bool runTestCase(const std::string& name);
    static bool runTestCase(const TestCase& test_case);
    static std::map<std::string, bool> runAllTestCases();
    static std::map<std::string, std::string> runAllTestCasesWithResults();
    
    // Test configuration
    static void setTestTimeout(double timeout);
    static void setTestRetryCount(int count);
    static void setTestRandomSeed(unsigned int seed);
    static void setTestErrorHandling(ErrorHandler::Strategy strategy);
    
    // Test results
    static std::vector<std::string> getPassedTestCases();
    static std::vector<std::string> getFailedTestCases();
    static std::vector<std::string> getSkippedTestCases();
    static std::map<std::string, std::string> getTestResults();
    static std::string getTestStatistics() const;
    
    // Test reporting
    static void generateTestReport(const std::string& filepath);
    static void generateTestSummaryReport(const std::string& filepath);
    static std::string getTestSummary() const;
    
    // Error injection for testing
    static void injectError(NLMErrorType type, const std::string& message);
    static void clearErrorInjection();
    static bool isErrorInjectionActive() const;
    
    // Performance testing
    static double measureErrorHandlingPerformance(const std::string& test_name);
    static double measureErrorRecoveryPerformance(const std::string& test_name);
    static double measureErrorReportingPerformance(const std::string& test_name);
};
```

### 8. Error Handling in Core Systems

#### Integration with Brain System
```cpp
// Enhanced brain error handling
class EnhancedBrain {
public:
    // Error handling methods
    bool initialize() override;
    bool step(double timestep) override;
    bool reset() override;
    
    // Error handling hooks
    virtual bool onBrainError(const NLMError& error);
    virtual bool onBrainWarning(const std::string& message);
    virtual bool onBrainInfo(const std::string& message);
    
    // Error recovery
    virtual bool recoverFromBrainError(const NLMError& error);
    virtual bool canRecoverFromBrainError(const NLMError& error) const;
    
    // Error statistics
    virtual int getBrainErrorCount(NLMErrorType type = NLMErrorType::UNKNOWN_ERROR) const;
    virtual std::string getBrainErrorStatistics() const;
    
protected:
    // Error reporting
    void reportBrainError(NLMErrorType type, const std::string& message,
                         NLMError::Severity severity = NLMError::Severity::WARNING);
    void reportBrainWarning(const std::string& message);
    void reportBrainInfo(const std::string& message);
    
    // Error recovery
    bool attemptBrainErrorRecovery(const NLMError& error);
    
    // Error management
    void updateBrainErrorState(const NLMError& error);
    
private:
    // Brain error state
    std::atomic<int> brain_error_count_;
    std::atomic<int> brain_warning_count_;
    std::atomic<int> brain_recovery_attempts_;
    std::atomic<int> brain_recovery_successes_;
    std::chrono::system_clock::time_point last_brain_error_time_;
    
    // Brain error context
    std::string current_brain_context_;
    std::vector<NLMError> recent_brain_errors_;
};
```

#### Integration with Memory System
```cpp
// Enhanced memory error handling
class EnhancedMemorySystem {
public:
    // Error handling methods
    bool allocateMemory(size_t size);
    bool freeMemory(size_t size);
    bool reserveMemory(size_t size);
    bool getAvailableMemory() const;
    
    // Error handling hooks
    virtual bool onMemoryError(const NLMError& error);
    virtual bool onMemoryWarning(const std::string& message);
    virtual bool onMemoryInfo(const std::string& message);
    
    // Error recovery
    virtual bool recoverFromMemoryError(const NLMError& error);
    virtual bool canRecoverFromMemoryError(const NLMError& error) const;
    
    // Error statistics
    virtual int getMemoryErrorCount(NLMErrorType type = NLMErrorType::UNKNOWN_ERROR) const;
    virtual std::string getMemoryErrorStatistics() const;
    
protected:
    // Error reporting
    void reportMemoryError(NLMErrorType type, const std::string& message,
                          NLMError::Severity severity = NLMError::Severity::WARNING);
    void reportMemoryWarning(const std::string& message);
    void reportMemoryInfo(const std::string& message);
    
    // Error recovery
    bool attemptMemoryErrorRecovery(const NLMError& error);
    
    // Error management
    void updateMemoryErrorState(const NLMError& error);
    
private:
    // Memory error state
    std::atomic<size_t> allocated_memory_;
    std::atomic<size_t> available_memory_;
    std::atomic<int> memory_error_count_;
    std::atomic<int> memory_warning_count_;
    std::chrono::system_clock::time_point last_memory_error_time_;
    
    // Memory error context
    std::string current_memory_context_;
    std::vector<NLMError> recent_memory_errors_;
};
```

### 9. Error Handling Configuration

#### Error Handling Configuration Schema
```json
{
    "error_handling": {
        "global": {
            "enabled": true,
            "report_level": "all",
            "max_error_history": 10000,
            "error_statistics_interval": 60,
            "auto_recovery_enabled": true,
            "graceful_degradation_enabled": true,
            "performance_monitoring_enabled": true
        },
        
        "error_reporting": {
            "console_output": true,
            "file_output": true,
            "file_path": "nlm_errors.log",
            "file_rotation": true,
            "file_max_size": 10485760,
            "file_max_files": 5,
            "structured_logging": true,
            "json_format": false,
            "include_stack_traces": true,
            "include_error_context": true,
            "include_recovery_actions": true
        },
        
        "error_recovery": {
            "default_strategy": "use_defaults",
            "max_retries": 3,
            "retry_delay": 1.0,
            "recovery_timeout": 30.0,
            "recoverable_errors": [
                "CONFIG_FILE_READ_ERROR",
                "CONFIG_TYPE_MISMATCH",
                "CONFIG_RANGE_VIOLATION",
                "FILE_IO_ERROR",
                "TEMPORARY_RESOURCE_ERROR"
            ],
            "non_recoverable_errors": [
                "MEMORY_ALLOCATION_FAILED",
                "PERMISSION_ERROR",
                "SYSTEM_CALL_ERROR"
            ]
        },
        
        "error_filtering": {
            "filter_enabled": true,
            "filter_rules": [
                {"type": "CONFIG", "severity": "WARNING", "action": "REPORT"},
                {"type": "CONFIG", "severity": "ERROR", "action": "RECOVERY"},
                {"type": "BRAIN", "severity": "ERROR", "action": "FALLBACK"},
                {"type": "MEMORY", "severity": "ERROR", "action": "RECOVERY"},
                {"type": "PLASTICITY", "severity": "ERROR", "action": "SKIP"},
                {"type": "NEUROMODULATION", "severity": "ERROR", "action": "LOG_ONLY"},
                {"type": "DEVELOPMENT", "severity": "ERROR", "action": "DEGRADATION"}
            ]
        },
        
        "graceful_degradation": {
            "enabled": true,
            "degradation_levels": [
                {"level": "FULL_FUNCTIONALITY", "description": "All features available"},
                {"level": "DEGRADED_FUNCTIONALITY", "description": "Some features disabled"},
                {"level": "LIMITED_FUNCTIONALITY", "description": "Minimal features available"},
                {"level": "MINIMAL_FUNCTIONALITY", "description": "Essential functionality only"},
                {"level": "EMERGENCY_MODE", "description": "Emergency operation mode"}
            ],
            "degradation_triggers": [
                "CRITICAL_MEMORY_ERROR",
                "SYSTEM_RESOURCE_EXHAUSTION",
                "NETWORK_CONNECTION_LOSS",
                "FILE_SYSTEM_FULL"
            ]
        },
        
        "performance_monitoring": {
            "enabled": true,
            "monitoring_interval": 10.0,
            "threshold_cpu": 90.0,
            "threshold_memory": 90.0,
            "threshold_errors": 10,
            "performance_alert_enabled": true,
            "auto_scale_enabled": false
        },
        
        "error_testing": {
            "enabled": true,
            "test_interval": 3600,
            "test_error_types": [
                "CONFIG_FILE_READ_ERROR",
                "CONFIG_TYPE_MISMATCH",
                "BRAIN_INITIALIZATION_ERROR",
                "MEMORY_ALLOCATION_ERROR",
                "FILE_IO_ERROR"
            ],
            "test_recovery": true,
            "test_degradation": true
        }
    }
}
```

## Implementation Examples

### 1. Basic Error Handling Usage
```cpp
// In main.cpp or application startup
#include "NLMErrorManager.hpp"

int main(int argc, char** argv) {
    // Initialize error manager
    NLMErrorManager::initialize(config);
    
    // Configure error handling
    NLMErrorManager::setErrorHandlingStrategy(
        NLMErrorType::CONFIG_FILE_READ_ERROR,
        ErrorHandler::Strategy::USE_DEFAULTS);
    
    NLMErrorManager::setErrorHandlingStrategy(
        NLMErrorType::MEMORY_ALLOCATION_ERROR,
        ErrorHandler::Strategy::RETRY);
    
    // Run simulation
    try {
        EnhancedBrain brain;
        if (!brain.initialize()) {
            NLMErrorManager::reportError(
                NLMErrorType::BRAIN_INITIALIZATION_ERROR,
                "Failed to initialize brain",
                NLMError::Severity::CRITICAL);
            return 1;
        }
        
        for (int step = 0; step < 1000; ++step) {
            try {
                brain.step(0.001);
            } catch (const std::exception& e) {
                NLMErrorManager::reportError(e,
                    NLMErrorType::SIMULATION_STEP_ERROR);
                
                // Attempt recovery
                if (NLMErrorManager::canRecover(current_error)) {
                    NLMErrorManager::performRecovery(current_error);
                } else {
                    NLMErrorManager::reportError(
                        NLMErrorType::BRAIN_STEP_ERROR,
                        "Cannot recover from simulation error",
                        NLMError::Severity::FATAL);
                    return 1;
                }
            }
        }
        
    } catch (const std::exception& e) {
        NLMErrorManager::reportError(e,
            NLMErrorType::UNEXPECTED_ERROR);
        return 1;
    }
    
    // Generate error report
    NLMErrorManager::generateErrorReport("nlm_session_report.json");
    
    // Shutdown error manager
    NLMErrorManager::shutdown();
    
    return 0;
}
```

### 2. Custom Error Handler
```cpp
custom_error_handler.h
```cpp
#ifndef CUSTOM_ERROR_HANDLER_H
#define CUSTOM_ERROR_HANDLER_H

#include "NLMErrorManager.hpp"
#include "DefaultErrorHandler.hpp"

class CustomErrorHandler : public DefaultErrorHandler {
public:
    CustomErrorHandler();
    
    void handleError(const NLMError& error) override;
    void handleWarning(const std::string& message) override;
    void handleInfo(const std::string& message) override;
    
    // Custom error handling strategies
    void handleBrainError(const NLMError& error);
    void handleMemoryError(const NLMError& error);
    void handleConfigurationError(const NLMError& error);
    void handlePlasticityError(const NLMError& error);
    
    // Error recovery
    bool recoverBrainError(const NLMError& error) override;
    bool recoverMemoryError(const NLMError& error);
    bool recoverConfigurationError(const NLMError& error);
    
    // Configuration
    void configureFromConfig(const Config& config) override;
    
    // Error statistics
    std::string getCustomErrorStatistics() const;
    
private:
    // Custom error state
    std::atomic<int> brain_errors_recovered_;
    std::atomic<int> memory_errors_recovered_;
    std::atomic<int> configuration_errors_recovered_;
    
    // Custom error handlers
    std::map<NLMErrorType, std::function<void(const NLMError&)>> custom_handlers_;
    
    void initializeCustomHandlers();
    void logErrorToCustomSystem(const NLMError& error);
    void sendErrorNotification(const NLMError& error);
    bool attemptCustomRecovery(const NLMError& error);
};

#endif // CUSTOM_ERROR_HANDLER_H
```

```cpp
custom_error_handler.cpp
```cpp
#include "custom_error_handler.h"
#include <fstream>
#include <iomanip>
#include <iostream>

CustomErrorHandler::CustomErrorHandler() : 
    brain_errors_recovered_(0), memory_errors_recovered_(0), 
    configuration_errors_recovered_(0) {
    initializeCustomHandlers();
}

void CustomErrorHandler::handleError(const NLMError& error) {
    // Custom error handling logic
    switch (error.type) {
        case NLMErrorType::BRAIN_INITIALIZATION_ERROR:
        case NLMErrorType::BRAIN_STEP_ERROR:
        case NLMErrorType::BRAIN_SYNAPSE_ERROR:
        case NLMErrorType::BRAIN_NEURON_ERROR:
            handleBrainError(error);
            break;
            
        case NLMErrorType::MEMORY_ALLOCATION_ERROR:
        case NLMErrorType::MEMORY_CORRUPTION_ERROR:
        case NLMErrorType::MEMORY_ACCESS_VIOLATION:
            handleMemoryError(error);
            break;
            
        case NLMErrorType::CONFIG_FILE_READ_ERROR:
        case NLMErrorType::CONFIG_TYPE_MISMATCH:
        case NLMErrorType::CONFIG_RANGE_VIOLATION:
            handleConfigurationError(error);
            break;
            
        case NLMErrorType::STDP_ERROR:
        case NLMErrorType::HEBBIAN_ERROR:
        case NLMErrorType::STRUCTURAL_PLASTICITY_ERROR:
            handlePlasticityError(error);
            break;
            
        default:
            // Use default error handling for other types
            DefaultErrorHandler::handleError(error);
            break;
    }
}

void CustomErrorHandler::handleBrainError(const NLMError& error) {
    logErrorToCustomSystem(error);
    sendErrorNotification(error);
    
    if (attemptCustomRecovery(error)) {
        brain_errors_recovered_++;
    } else {
        // If recovery fails, escalate to default handler
        DefaultErrorHandler::handleError(error);
    }
}

bool CustomErrorHandler::recoverBrainError(const NLMError& error) {
    if (error.type == NLMErrorType::BRAIN_INITIALIZATION_ERROR) {
        // Reinitialize brain with fallback configuration
        return reinitializeBrainWithFallback();
    } else if (error.type == NLMErrorType::BRAIN_STEP_ERROR) {
        // Reduce complexity and retry
        return reduceSimulationComplexityAndRetry();
    } else if (error.type == NLMErrorType::BRAIN_SYNAPSE_ERROR) {
        // Fix synapse and continue
        return fixSynapseAndContinue();
    } else if (error.type == NLMErrorType::BRAIN_NEURON_ERROR) {
        // Reset affected neurons
        return resetAffectedNeurons();
    }
    
    return DefaultErrorHandler::recoverFromBrainError(error);
}

void CustomErrorHandler::configureFromConfig(const Config& config) {
    DefaultErrorHandler::configureFromConfig(config);
    
    // Custom configuration logic
    if (config.has("error_handling.custom_notifications.enabled")) {
        bool enable_notifications = config.get<bool>(
            "error_handling.custom_notifications.enabled").value_or(false);
        setNotificationEnabled(enable_notifications);
    }
    
    if (config.has("error_handling.custom_logging.enabled")) {
        bool enable_logging = config.get<bool>(
            "error_handling.custom_logging.enabled").value_or(true);
        setCustomLoggingEnabled(enable_logging);
    }
    
    if (config.has("error_handling.custom_recovery.enabled")) {
        bool enable_recovery = config.get<bool>(
            "error_handling.custom_recovery.enabled").value_or(true);
        setCustomRecoveryEnabled(enable_recovery);
    }
}

std::string CustomErrorHandler::getCustomErrorStatistics() const {
    std::ostringstream oss;
    oss << "Custom Error Handler Statistics:" << std::endl;
    oss << "  Brain Errors Recovered: " << brain_errors_recovered_ << std::endl;
    oss << "  Memory Errors Recovered: " << memory_errors_recovered_ << std::endl;
    oss << "  Configuration Errors Recovered: " << configuration_errors_recovered_ << std::endl;
    oss << "  Total Custom Recoveries: " << 
        (brain_errors_recovered_ + memory_errors_recovered_ + configuration_errors_recovered_) << std::endl;
    
    return oss.str();
}
```

### 3. Error Logging to Custom System
```cpp
void CustomErrorHandler::logErrorToCustomSystem(const NLMError& error) {
    if (!custom_logging_enabled_) {
        return;
    }
    
    // Log to custom log file
    std::ofstream custom_log("nlm_custom_errors.log", std::ios::app);
    if (custom_log.is_open()) {
        custom_log << "[CUSTOM] " << error.timestamp << " " << 
            error.severity_prefix << " " << error.message << std::endl;
        
        if (!error.context.empty()) {
            custom_log << "  Context: " << error.context << std::endl;
        }
        
        if (!error.suggested_fix.empty()) {
            custom_log << "  Suggested Fix: " << error.suggested_fix << std::endl;
        }
        
        if (!error.recovery_actions.empty()) {
            custom_log << "  Recovery Actions:" << std::endl;
            for (const auto& action : error.recovery_actions) {
                custom_log << "    - " << action << std::endl;
            }
        }
        
        custom_log << std::endl;
    }
}

void CustomErrorHandler::sendErrorNotification(const NLMError& error) {
    if (!notification_enabled_) {
        return;
    }
    
    // Send error notification via various channels
    if (notification_email_enabled_) {
        sendEmailNotification(error);
    }
    
    if (notification_slack_enabled_) {
        sendSlackNotification(error);
    }
    
    if (notification_webhook_enabled_) {
        sendWebhookNotification(error);
    }
    
    if (notification_console_enabled_) {
        sendConsoleNotification(error);
    }
}
```

### 4. Error Recovery Implementation
```cpp
bool CustomErrorHandler::attemptCustomRecovery(const NLMError& error) {
    bool recovery_attempted = false;
    bool recovery_successful = false;
    
    // Attempt different recovery strategies based on error type
    switch (error.type) {
        case NLMErrorType::BRAIN_INITIALIZATION_ERROR:
            recovery_attempted = true;
            recovery_successful = reinitializeBrainWithFallback();
            break;
            
        case NLMErrorType::MEMORY_ALLOCATION_ERROR:
            recovery_attempted = true;
            recovery_successful = attemptMemoryReallocation();
            break;
            
        case NLMErrorType::CONFIG_FILE_READ_ERROR:
            recovery_attempted = true;
            recovery_successful = reloadConfiguration();
            break;
            
        case NLMErrorType::FILE_IO_ERROR:
            recovery_attempted = true;
            recovery_successful = attemptFileRecovery();
            break;
            
        default:
            // Use default recovery for unknown error types
            recovery_attempted = true;
            recovery_successful = DefaultErrorHandler::performRecovery(error);
            break;
    }
    
    if (recovery_attempted && recovery_successful) {
        // Record successful recovery
        switch (error.type) {
            case NLMErrorType::BRAIN_INITIALIZATION_ERROR:
                brain_errors_recovered_++;
                break;
                
            case NLMErrorType::MEMORY_ALLOCATION_ERROR:
                memory_errors_recovered_++;
                break;
                
            case NLMErrorType::CONFIG_FILE_READ_ERROR:
                configuration_errors_recovered_++;
                break;
                
            default:
                break;
        }
        
        // Call on_success callback if registered
        auto it = custom_handlers_.find(error.type);
        if (it != custom_handlers_.end()) {
            it->second(error);
        }
    }
    
    return recovery_successful;
}
```

### 5. Performance Monitoring and Error Handling
```cpp
void monitorErrorHandlingPerformance() {
    static auto last_check = std::chrono::system_clock::now();
    static int last_error_count = NLMErrorManager::getErrorCount();
    static int last_warning_count = NLMErrorManager::getWarningCount();
    
    auto now = std::chrono::system_clock::now();
    auto elapsed = std::chrono::duration_cast<std::chrono::seconds>(now - last_check);
    
    if (elapsed.count() >= 60) { // Check every minute
        int current_error_count = NLMErrorManager::getErrorCount();
        int current_warning_count = NLMErrorManager::getWarningCount();
        
        int error_increase = current_error_count - last_error_count;
        int warning_increase = current_warning_count - last_warning_count;
        
        // Log performance metrics
        std::cout << "Error Handling Performance Metrics:" << std::endl;
        std::cout << "  Errors per minute: " << error_increase << std::endl;
        std::cout << "  Warnings per minute: " << warning_increase << std::endl;
        std::cout << "  Error rate: " << (static_cast<double>(error_increase) / elapsed.count()) << "/second" << std::endl;
        
        // Check for performance issues
        if (error_increase > 10) {
            NLMErrorManager::reportError(
                NLMErrorType::PERFORMANCE_WARNING,
                "High error rate detected in error handling system",
                NLMError::Severity::WARNING);
        }
        
        if (warning_increase > 50) {
            NLMErrorManager::reportError(
                NLMErrorType::PERFORMANCE_WARNING,
                "High warning rate detected in error handling system",
                NLMError::Severity::WARNING);
        }
        
        last_check = now;
        last_error_count = current_error_count;
        last_warning_count = current_warning_count;
    }
}
```

## Error Handling Configuration Examples

### 1. Basic Configuration
```json
{
    "error_handling": {
        "global": {
            "enabled": true,
            "report_level": "all",
            "max_error_history": 10000
        },
        
        "error_reporting": {
            "console_output": true,
            "file_output": true,
            "file_path": "nlm_errors.log"
        },
        
        "error_recovery": {
            "default_strategy": "use_defaults",
            "max_retries": 3,
            "retry_delay": 1.0
        }
    }
}
```

### 2. Advanced Configuration
```json
{
    "error_handling": {
        "global": {
            "enabled": true,
            "report_level": "all",
            "max_error_history": 50000,
            "auto_recovery_enabled": true,
            "graceful_degradation_enabled": true,
            "performance_monitoring_enabled": true
        },
        
        "error_reporting": {
            "console_output": true,
            "file_output": true,
            "database_output": true,
            "network_output": true,
            "syslog_output": false,
            "structured_logging": true,
            "include_stack_traces": true,
            "include_error_context": true,
            "include_recovery_actions": true
        },
        
        "error_recovery": {
            "default_strategy": "use_defaults",
            "max_retries": 5,
            "retry_delay": 0.5,
            "recovery_timeout": 60.0,
            "recoverable_errors": [
                "CONFIG_FILE_READ_ERROR",
                "CONFIG_TYPE_MISMATCH",
                "TEMPORARY_RESOURCE_ERROR"
            ]
        },
        
        "graceful_degradation": {
            "enabled": true,
            "degradation_levels": [
                "FULL_FUNCTIONALITY",
                "DEGRADED_FUNCTIONALITY",
                "LIMITED_FUNCTIONALITY",
                "MINIMAL_FUNCTIONALITY",
                "EMERGENCY_MODE"
            ]
        },
        
        "performance_monitoring": {
            "enabled": true,
            "monitoring_interval": 30.0,
            "threshold_cpu": 90.0,
            "threshold_memory": 90.0,
            "threshold_errors": 10
        }
    }
}
```

This comprehensive error handling framework provides NLM with:

1. **Detailed Error Classification**: Extensive error type categorization with severity levels
2. **Flexible Error Handling**: Multiple error handling strategies and customizable error handlers
3. **Robust Error Recovery**: Automatic error recovery with configurable recovery actions
4. **Graceful Degradation**: System can gracefully degrade functionality when errors occur
5. **Performance Monitoring**: Continuous monitoring of error handling performance
6. **Comprehensive Error Logging**: Multiple logging destinations with structured logging
7. **Testing Framework**: Built-in error testing and validation capabilities
8. **Integration with Core Systems**: Error handling integrated with brain, memory, plasticity, and neuromodulation systems

The error handling system ensures that NLM can handle errors gracefully, recover from errors when possible, provide detailed error information for debugging, and maintain system stability even when errors occur.