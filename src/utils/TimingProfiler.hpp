#pragma once

#include <string>
#include <vector>
#include <unordered_map>
#include <functional>
#include <memory>
#include <chrono>

namespace nlm {

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
    
    using ErrorCallback = std::function<void(const ErrorInfo&)>;
    
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

} // namespace nlm