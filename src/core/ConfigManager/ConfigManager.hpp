#pragma once

#include <string>
#include <vector>
#include <unordered_map>
#include <functional>
#include <memory>
#include <chrono>
#include "../core/Config/Config.hpp"
#include "../core/Logger/Logger.hpp"

namespace nlm {

/**
 * Enhanced configuration utilities with validation, serialization, and validation
 * Phase 5: Configuration infrastructure
 */
class ConfigManager {
public:
    // Configuration templates for validation
    struct ConfigSchema {
        std::string name;
        std::string description;
        bool required;
        ConfigSource source;
        
        // Validation
        std::function<bool(const ConfigValue&)> validator;
        std::string defaultValue;
        std::vector<std::string> allowedValues;
        
        // Metadata for documentation
        std::string category;
        int priority; // For ordered loading
        
        ConfigSchema() : name(), description(), required(false), 
                         source(ConfigSource::Default), priority(0) {}
    };
    
    // Validation results
    struct ValidationResult {
        bool valid;
        std::vector<std::string> errors;
        std::vector<std::string> warnings;
        
        ValidationResult() : valid(true) {}
    };
    
    // Configuration change callback
    struct ConfigChange {
        std::string key;
        ConfigValue oldValue;
        ConfigValue newValue;
        ConfigSource source;
        double timestamp;
    };
    
    using ConfigChangeCallback = std::function<void(const ConfigChange&)>;
    
    ConfigManager();
    ~ConfigManager();
    
    // Disable copying, enable moving
    ConfigManager(const ConfigManager&) = delete;
    ConfigManager& operator=(const ConfigManager&) = delete;
    ConfigManager(ConfigManager&&) noexcept;
    ConfigManager& operator=(ConfigManager&&) noexcept;
    
    /**
     * Load configuration from file with validation
     * @param filepath Configuration file path (JSON, YAML, or INI)
     * @param validate Whether to validate against schema
     * @return true if successful
     */
    bool loadFromFile(const std::string& filepath, bool validate = true);
    
    /**
     * Load configuration from JSON string
     * @param jsonString JSON configuration string
     * @param validate Whether to validate against schema
     * @return true if successful
     */
    bool loadFromJSON(const std::string& jsonString, bool validate = true);
    
    /**
     * Export configuration to JSON
     * @param pretty Whether to format with indentation
     * @return JSON string representation
     */
    std::string exportToJSON(bool pretty = true) const;
    
    /**
     * Register configuration schema
     * @param schema Configuration schema
     */
    void registerSchema(const ConfigSchema& schema);
    
    /**
     * Validate current configuration against all registered schemas
     * @return Validation result
     */
    ValidationResult validate() const;
    
    /**
     * Set configuration value with validation
     * @param key Configuration key
     * @param value Configuration value
     * @param source Source of the configuration
     * @param validate Whether to validate against schema
     * @return true if valid and set
     */
    bool setValidated(const std::string& key, const ConfigValue& value, 
                      ConfigSource source = ConfigSource::Runtime, 
                      bool validate = true);
    
    /**
     * Get configuration schema for a key
     * @param key Configuration key
     * @return Schema if found, nullptr otherwise
     */
    const ConfigSchema* getSchema(const std::string& key) const;
    
    /**
     * Check if configuration has a key
     * @param key Configuration key
     */
    bool has(const std::string& key) const;
    
    /**
     * Get configuration keys matching pattern
     * @param pattern Wildcard pattern (e.g., "sensor_*")
     * @return Vector of matching keys
     */
    std::vector<std::string> getKeys(const std::string& pattern = "*") const;
    
    /**
     * Add configuration change callback
     * @param callback Function to call on changes
     */
    void addChangeCallback(const ConfigChangeCallback& callback);
    
    /**
     * Remove all change callbacks
     */
    void clearChangeCallbacks();
    
    /**
     * Get configuration summary with validation status
     * @return Formatted summary string
     */
    std::string getSummary() const;
    
    /**
     * Merge configuration from another manager
     * @param other Other configuration manager
     * @param overwrite Whether to overwrite existing keys
     */
    void merge(const ConfigManager& other, bool overwrite = true);
    
    /**
     * Get configuration statistics
     * @return Map of statistics
     */
    std::unordered_map<std::string, std::string> getStats() const;
    
    /**
     * Reset to default values
     */
    void resetToDefaults();
    
    /**
     * Copy configuration to another manager
     * @param other Destination configuration manager
     */
    void copyTo(ConfigManager& other) const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

/**
 * Performance monitoring utilities
 * Phase 5: Performance infrastructure
 */
class PerformanceMonitor {
public:
    // Performance metrics
    struct Metrics {
        double cpuUsage;           // CPU usage percentage (0-100)
        double memoryUsage;        // Memory usage in MB
        double memoryAvailable;    // Available memory in MB
        double gpuUsage;           // GPU usage percentage (0-100)
        size_t activeThreads;      // Number of active threads
        size_t allocatedObjects;   // Number of allocated objects
        size_t peakAllocated;      // Peak allocated objects
        double currentFps;         // Current FPS
        double averageFps;         // Average FPS over time
        size_t frameTimeMs;        // Current frame time in ms
        size_t spikeCount;         // Number of spikes processed
        size_t neuronCount;        // Number of neurons
        size_t synapseCount;        // Number of synapses
        size_t memoryPoolUsed;     // Memory pool usage
        size_t eventQueueSize;     // Event queue size
        
        Metrics() : cpuUsage(0.0), memoryUsage(0.0), memoryAvailable(0.0),
                   gpuUsage(0.0), activeThreads(0), allocatedObjects(0),
                   peakAllocated(0), currentFps(0.0), averageFps(0.0),
                   frameTimeMs(0), spikeCount(0), neuronCount(0),
                   synapseCount(0), memoryPoolUsed(0), eventQueueSize(0) {}
    };
    
    // Performance sample
    struct Sample {
        double timestamp;          // Wall-clock time
        Metrics metrics;           // Current metrics
    };
    
    // Performance threshold
    struct Threshold {
        std::string name;
        std::string metric;
        double warningLevel;
        double criticalLevel;
        std::string message;
    };
    
    // Performance alert
    struct Alert {
        double timestamp;
        Threshold threshold;
        Metrics currentMetrics;
        std::string severity;     // "warning", "critical"
    };
    
    // Sampling policy
    enum class SamplingPolicy {
        RealTime,      // Sample at fixed interval
        EventDriven,   // Sample on events
        Adaptive,      // Adjust sampling based on activity
        OnDemand       // Sample on request
    };
    
    PerformanceMonitor();
    ~PerformanceMonitor();
    
    // Disable copying, enable moving
    PerformanceMonitor(const PerformanceMonitor&) = delete;
    PerformanceMonitor& operator=(const PerformanceMonitor&) = delete;
    PerformanceMonitor(PerformanceMonitor&&) noexcept;
    PerformanceMonitor& operator=(PerformanceMonitor&&) noexcept;
    
    /**
     * Start performance monitoring
     * @param interval Sampling interval in seconds (0 for event-driven)
     * @param policy Sampling policy
     */
    void start(double interval = 0.1, SamplingPolicy policy = SamplingPolicy::RealTime);
    
    /**
     * Stop performance monitoring
     * @return Vector of collected samples
     */
    std::vector<Sample> stop();
    
    /**
     * Get current performance metrics
     * @return Current metrics
     */
    Metrics getCurrentMetrics() const;
    
    /**
     * Add performance threshold
     * @param threshold Performance threshold
     */
    void addThreshold(const Threshold& threshold);
    
    /**
     * Get active performance alerts
     * @return Vector of current alerts
     */
    std::vector<Alert> getAlerts() const;
    
    /**
     * Clear performance alerts
     */
    void clearAlerts();
    
    /**
     * Sample performance manually
     * @return Sample taken
     */
    Sample sample() const;
    
    /**
     * Get performance samples
     * @param maxSamples Maximum number of samples to return
     * @return Vector of performance samples
     */
    std::vector<Sample> getSamples(size_t maxSamples = 1000) const;
    
    /**
     * Calculate performance statistics
     * @param samples Performance samples
     * @return Metrics with calculated statistics
     */
    static Metrics calculateStats(const std::vector<Sample>& samples);
    
    /**
     * Export performance data to file
     * @param filepath Output file path
     * @param samples Performance samples to export
     * @return true if successful
     */
    bool exportToCSV(const std::string& filepath, const std::vector<Sample>& samples) const;
    
    /**
     * Get system information
     * @return String with system information
     */
    static std::string getSystemInfo();
    
    /**
     * Profile a function
     * @param name Function name
     * @param func Function to profile
     * @param args Arguments to pass
     * @return Result of function call
     */
    template<typename Func, typename... Args>
    auto profile(const std::string& name, Func&& func, Args&&... args) -> decltype(func(std::forward<Args>(args)...));
    
    /**
     * Start/stop profiling for a specific function
     * @param name Function name
     * @param enable Enable or disable profiling
     */
    void toggleProfiling(const std::string& name, bool enable);
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

/**
 * Error handling utilities
 * Phase 2: Error handling infrastructure
 */
class ErrorHandler {
public:
    // Error categories
    enum class ErrorCategory {
        None,
        System,
        Memory,
        Network,
        FileIO,
        Configuration,
        Neural,
        Plasticity,
        Development,
        MemoryManagement,
        Performance,
        Serialization,
        Validation
    };
    
    // Error severity
    enum class Severity {
        Debug,
        Info,
        Warning,
        Error,
        Critical
    };
    
    // Detailed error information
    struct ErrorInfo {
        int code;
        ErrorCategory category;
        Severity severity;
        std::string message;
        std::string description;
        std::string file;
        int line;
        std::string function;
        std::string stackTrace;
        std::chrono::system_clock::time_point timestamp;
        std::unordered_map<std::string, std::string> context;
        
        ErrorInfo() : code(0), category(ErrorCategory::None), 
                     severity(Severity::Error), line(0) {}
    };
    
    // Error recovery information
    struct RecoveryInfo {
        bool recoverable;
        std::string action;
        std::function<bool()> recoveryFunction;
        double timeout;
    };
    
    // Error statistics
    struct Statistics {
        size_t totalErrors;
        size_t errorsByCategory[12];
        size_t errorsBySeverity[5];
        double lastErrorTime;
        double errorFrequency;
        
        Statistics() : totalErrors(0), lastErrorTime(0.0), errorFrequency(0.0) {
            std::fill(std::begin(errorsByCategory), std::end(errorsByCategory), 0);
            std::fill(std::begin(errorsBySeverity), std::end(errorsBySeverity), 0);
        }
    };
    
    using ErrorCallback = std::function<void(const ErrorInfo&)>
    
    ErrorHandler();
    ~ErrorHandler();
    
    // Disable copying, enable moving
    ErrorHandler(const ErrorHandler&) = delete;
    ErrorHandler& operator=(const ErrorHandler&) = delete;
    ErrorHandler(ErrorHandler&&) noexcept;
    ErrorHandler& operator=(ErrorHandler&&) noexcept;
    
    /**
     * Set error handler
     * @param handler Error handler function
     */
    static void setGlobalErrorHandler(const ErrorCallback& handler);
    
    /**
     * Get global error handler
     * @return Current error handler
     */
    static ErrorCallback getGlobalErrorHandler();
    
    /**
     * Report an error
     * @param info Error information
     */
    static void reportError(const ErrorInfo& info);
    
    /**
     * Report an exception
     * @param e Exception to report
     */
    static void reportException(const std::exception& e);
    
    /**
     * Report a system error
     * @param code Error code
     * @param message Error message
     * @param category Error category
     * @param severity Error severity
     */
    static void reportSystemError(int code, const std::string& message,
                                  ErrorCategory category = ErrorCategory::System,
                                  Severity severity = Severity::Error);
    
    /**
     * Create a structured error
     * @param code Error code
     * @param message Error message
     * @param category Error category
     * @param severity Error severity
     * @return Error information
     */
    static ErrorInfo createError(int code, const std::string& message,
                                ErrorCategory category = ErrorCategory::System,
                                Severity severity = Severity::Error);
    
    /**
     * Add error context
     * @param key Context key
     * @param value Context value
     */
    static void addContext(const std::string& key, const std::string& value);
    
    /**
     * Get error statistics
     * @return Error statistics
     */
    static Statistics getStatistics();
    
    /**
     * Get recent errors
     * @param maxErrors Maximum number of errors to return
     * @return Vector of recent error information
     */
    static std::vector<ErrorInfo> getRecentErrors(size_t maxErrors = 100);
    
    /**
     * Clear error history
     */
    static void clearHistory();
    
    /**
     * Check if error can be recovered
     * @param info Error information
     * @return Recovery information
     */
    static RecoveryInfo getRecoveryInfo(const ErrorInfo& info);
    
    /**
     * Attempt to recover from error
     * @param info Error information
     * @return true if recovery attempted
     */
    static bool attemptRecovery(const ErrorInfo& info);
    
    /**
     * Set error recovery function for a category
     * @param category Error category
     * @param recoveryFunction Recovery function
     */
    static void setRecoveryFunction(ErrorCategory category, 
                                   const std::function<bool()>& recoveryFunction);
    
    /**
     * Get error message for a code
     * @param code Error code
     * @return Error message
     */
    static std::string getErrorMessage(int code);
    
    /**
     * Set error message for a code
     */
    static void setErrorMessage(int code, const std::string& message);
    
    /**
     * Create a user-friendly error summary
     * @param info Error information
     * @return Formatted error summary
     */
    static std::string formatError(const ErrorInfo& info);
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

/**
 * Timing and profiling utilities
 * Phase 2: Timing infrastructure
 */
class Timer {
public:
    Timer();
    ~Timer();
    
    // Disable copying, enable moving
    Timer(const Timer&) = delete;
    Timer& operator=(const Timer&) = delete;
    Timer(Timer&&) noexcept;
    Timer& operator=(Timer&&) noexcept;
    
    /**
     * Start timer
     */
    void start();
    
    /**
     * Stop timer
     */
    void stop();
    
    /**
     * Reset timer
     */
    void reset();
    
    /**
     * Get elapsed time in seconds
     * @return Elapsed time
     */
    double elapsed() const;
    
    /**
     * Get elapsed time in milliseconds
     * @return Elapsed time
     */
    double elapsedMs() const;
    
    /**
     * Get elapsed time in microseconds
     * @return Elapsed time
     */
    double elapsedUs() const;
    
    /**
     * Check if timer is running
     * @return true if timer is running
     */
    bool isRunning() const;
    
    /**
     * Get current time point
     * @return Current time point
     */
    static std::chrono::high_resolution_clock::time_point now();
    
    /**
     * Get time since epoch in seconds
     */
    static double nowAsDouble();
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

/**
 * Profiler for function execution time
 * Phase 2: Profiling infrastructure
 */
class Profiler {
public:
    struct ProfileData {
        std::string functionName;
        std::string file;
        int line;
        std::string module;
        double totalTime;           // Total execution time
        double minTime;            // Minimum execution time
        double maxTime;            // Maximum execution time
        double averageTime;        // Average execution time
        size_t callCount;           // Number of calls
        double overhead;           // Profiling overhead
        
        ProfileData() : functionName(), file(), line(0), module(), 
                       totalTime(0.0), minTime(0.0), maxTime(0.0),
                       averageTime(0.0), callCount(0), overhead(0.0) {}
    };
    
    Profiler();
    ~Profiler();
    
    // Disable copying, enable moving
    Profiler(const Profiler&) = delete;
    Profiler& operator=(const Profiler&) = delete;
    Profiler(Profiler&&) noexcept;
    Profiler& operator=(Profiler&&) noexcept;
    
    /**
     * Start profiling a function
     * @param name Function name
     * @param file Source file
     * @param line Line number
     */
    void startProfile(const std::string& name, const std::string& file = "", int line = 0);
    
    /**
     * Stop profiling a function
     * @param name Function name
     * @return Profile data
     */
    ProfileData stopProfile(const std::string& name);
    
    /**
     * Check if function is being profiled
     * @param name Function name
     * @return true if function is being profiled
     */
    bool isProfiling(const std::string& name) const;
    
    /**
     * Get profile data for a function
     * @param name Function name
     * @return Profile data
     */
    ProfileData getProfile(const std::string& name) const;
    
    /**
     * Get all profile data
     * @return Map of function name to profile data
     */
    std::unordered_map<std::string, ProfileData> getAllProfiles() const;
    
    /**
     * Clear profile data
     */
    void clearProfiles();
    
    /**
     * Export profile data to file
     * @param filepath Output file path
     */
    bool exportToFile(const std::string& filepath) const;
    
    /**
     * Get profiling overhead percentage
     */
    static double getProfilingOverhead();
    
    /**
     * Set profiling overhead
     */
    static void setProfilingOverhead(double overhead);
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

/**
 * Utility functions for debugging and analysis
 * Phase 2: Debugging utilities
 */
class DebugUtils {
public:
    DebugUtils();
    ~DebugUtils();
    
    // Disable copying, enable moving
    DebugUtils(const DebugUtils&) = delete;
    DebugUtils& operator=(const DebugUtils&) = delete;
    DebugUtils(DebugUtils&&) noexcept;
    DebugUtils& operator=(DebugUtils&&) noexcept;
    
    /**
     * Print stack trace
     */
    static void printStackTrace();
    
    /**
     * Get current stack trace as string
     * @return Stack trace
     */
    static std::string getStackTrace();
    
    /**
     * Print memory usage
     */
    static void printMemoryUsage();
    
    /**
     * Get memory usage info
     * @return Memory usage info string
     */
    static std::string getMemoryUsageInfo();
    
    /**
     * Print thread information
     */
    static void printThreadInfo();
    
    /**
     * Get thread count
     * @return Number of threads
     */
    static size_t getThreadCount();
    
    /**
     * Get process ID
     */
    static size_t getProcessId();
    
    /**
     * Get thread ID
     */
    static size_t getThreadId();
    
    /**
     * Get current working directory
     */
    static std::string getWorkingDirectory();
    
    /**
     * Set current working directory
     * @param path Directory path
     * @return true if successful
     */
    static bool setWorkingDirectory(const std::string& path);
    
    /**
     * Create directory
     * @param path Directory path
     * @return true if successful
     */
    static bool createDirectory(const std::string& path);
    
    /**
     * Check if file exists
     * @param path File path
     */
    static bool fileExists(const std::string& path);
    
    /**
     * Get file size
     * @param path File path
     * @return File size in bytes
     */
    static size_t getFileSize(const std::string& path);
    
    /**
     * Read file content
     * @param path File path
     * @return File content
     */
    static std::string readFile(const std::string& path);
    
    /**
     * Write file content
     * @param path File path
     * @param content File content
     * @param append Whether to append
     */
    static bool writeFile(const std::string& path, const std::string& content, 
                          bool append = false);
    
    /**
     * Generate unique ID
     */
    static std::string generateId();
    
    /**
     * Format duration
     * @param duration Duration in seconds
     * @return Formatted duration string
     */
    static std::string formatDuration(double duration);
    
    /**
     * Format bytes
     * @param bytes Number of bytes
     */
    static std::string formatBytes(size_t bytes);
    
    /**
     * Parse command line arguments
     * @param argc Argument count
     * @param argv Argument values
     * @return Map of parsed arguments
     */
    static std::unordered_map<std::string, std::string> parseCommandLine(int argc, char** argv);
    
    /**
     * Get human-readable string from error code
     * @param code Error code
     */
    static std::string getErrorString(int code);
    
    /**
     * Initialize debug environment
     */
    static void initializeDebugEnvironment();
    
    /**
     * Cleanup debug environment
     */
    static void cleanupDebugEnvironment();
};

} // namespace nlm