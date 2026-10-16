// NLM Error Handling Framework
// Defines error categories, codes, and structured error reporting

#ifndef NLM_ERROR_HANDLING_H
#define NLM_ERROR_HANDLING_H

#include <string>
#include <map>
#include <vector>
#include <chrono>
#include <stacktrace>
#include <sstream>
#include <iomanip>
#include <memory>

namespace nlm {

// Error codes for different failure scenarios
enum class ErrorCode {
    // Configuration errors
    CONFIG_FILE_NOT_FOUND = 1000,
    CONFIG_PARSE_ERROR = 1001,
    CONFIG_VALIDATION_FAILED = 1002,
    CONFIG_VALUE_OUT_OF_RANGE = 1003,
    CONFIG_TYPE_MISMATCH = 1004,
    
    // Memory allocation errors
    ALLOCATION_FAILED = 2000,
    NULL_POINTER_DEREFERENCE = 2001,
    MEMORY_CORRUPTION = 2002,
    OUT_OF_MEMORY = 2003,
    
    // Neural computation errors
    NEURON_INVALID_STATE = 3000,
    SYNAPSE_INVALID_WEIGHT = 3001,
    CONNECTION_FAILED = 3002,
    REGION_NOT_FOUND = 3003,
    
    // Plasticity errors
    PLASTICITY_OVERFLOW = 4000,
    STDP_INVALID_TIMING = 4001,
    HEBBIAN_RULE_FAILED = 4002,
    STRUCTURAL_PLATONICITY_FAILURE = 4003,
    
    // Simulation errors
    SIMULATION_TIMEOUT = 5000,
    INVALID_TIMESTEP = 5001,
    STEP_OVERFLOW = 5002,
    STABILITY_FAILURE = 5003,
    
    // System errors
    FILE_IO_ERROR = 6000,
    NETWORK_ERROR = 6001,
    THREADING_ERROR = 6002,
    RESOURCE_UNAVAILABLE = 6003,
    
    // General errors
    UNKNOWN_ERROR = 9999,
    UNEXPECTED_ERROR = 9998,
    NOT_IMPLEMENTED = 9997
};

// Error categories for classification
enum class ErrorCategory {
    WARNING,
    ERROR,
    CRITICAL,
    DEBUG
};

// Exception class for NLM errors with detailed context
class NLMError : public std::exception {
private:
    ErrorCode code;
    ErrorCategory category;
    std::string message;
    std::string file;
    int line;
    std::string function;
    std::chrono::system_clock::time_point timestamp;
    std::stacktrace stackTrace;
    std::map<std::string, std::string> context;
    size_t recoveryAttempts;
    bool recovered;
    
public:
    NLMError(ErrorCode code, const std::string& message, 
             ErrorCategory category = ErrorCategory::ERROR,
             const std::string& file = __builtin_FILE(),
             int line = __builtin_LINE(),
             const std::string& function = __builtin_FUNCTION())
        : code(code), category(category), message(message),
          file(file), line(line), function(function),
          timestamp(std::chrono::system_clock::now()),
          stackTrace(std::stacktrace::current()),
          recoveryAttempts(0), recovered(false) {}
    
    // Add context information
    void addContext(const std::string& key, const std::string& value) {
        context[key] = value;
    }
    
    // Attempt error recovery
    virtual bool recover() {
        recoveryAttempts++;
        recovered = performRecovery();
        return recovered;
    }
    
    // Check if error can be recovered
    virtual bool canRecover() const { return false; }
    
    // Virtual recovery method for subclasses
    virtual bool performRecovery() { return false; }
    
    // Get error code
    ErrorCode getCode() const { return code; }
    
    // Get error category
    ErrorCategory getCategory() const { return category; }
    
    // Get error message
    const char* what() const noexcept override {
        std::ostringstream oss;
        oss << "[" << static_cast<int>(code) << "] " << message;
        if (!context.empty()) {
            oss << " (Context: ";
            for (const auto& pair : context) {
                oss << pair.first << "=" << pair.second << "; ";
            }
            oss << ")";
        }
        cachedMessage = oss.str();
        return cachedMessage.c_str();
    }
    
    // Get formatted error information
    std::string formatError() const {
        std::ostringstream oss;
        
        // Timestamp
        auto time = std::chrono::system_clock::to_time_t(timestamp);
        oss << "Error at " << std::put_time(std::localtime(&time), "%Y-%m-%d %H:%M:%S.%F") << "\n";
        
        // Error code and category
        oss << "Error Code: " << static_cast<int>(code) << " (Category: ";
        switch (category) {
            case ErrorCategory::WARNING: oss << "WARNING"; break;
            case ErrorCategory::ERROR: oss << "ERROR"; break;
            case ErrorCategory::CRITICAL: oss << "CRITICAL"; break;
            case ErrorCategory::DEBUG: oss << "DEBUG"; break;
        }
        oss << ")\n";
        
        // Message
        oss << "Message: " << message << "\n";
        
        // Location
        oss << "Location: " << file << ":" << line << " in " << function << "\n";
        
        // Context
        if (!context.empty()) {
            oss << "Context:\n";
            for (const auto& pair : context) {
                oss << "  " << pair.first << " = " << pair.second << "\n";
            }
        }
        
        // Stack trace
        oss << "Stack Trace:\n";
        for (const auto& frame : stackTrace) {
            oss << "  " << frame << "\n";
        }
        
        return oss.str();
    }
    
    // Get recovery status
    std::string getRecoveryStatus() const {
        std::ostringstream oss;
        oss << "Recovery Attempts: " << recoveryAttempts;
        if (recovered) {
            oss << " (Success)";
        } else {
            oss << " (Failed)";
        }
        return oss.str();
    }
    
private:
    mutable std::string cachedMessage;
};

// Error handling utilities
class ErrorHandler {
public:
    // Handle an error with optional recovery
    static bool handle(const NLMError& error, bool attemptRecovery = true) {
        // Log the error
        logError(error);
        
        // Attempt recovery if requested and possible
        if (attemptRecovery && error.canRecover()) {
            return error.recover();
        }
        
        return false;
    }
    
    // Validate a value against constraints
    template<typename T>
    static bool validate(const T& value, const std::string& minKey = "",
                        const std::string& maxKey = "",
                        const std::string& description = "") {
        bool valid = true;
        
        if (!minKey.empty() && !maxKey.empty()) {
            // Need min and max from somewhere - in real implementation would get from config
            // For now, just basic check
        }
        
        if (!description.empty()) {
            // Log validation warning
        }
        
        return valid;
    }
    
    // Check if value is within bounds
    template<typename T>
    static bool inBounds(const T& value, const T& min, const T& max) {
        return (value >= min) && (value <= max);
    }
    
    // Check if pointer is not null
    template<typename T>
    static bool notNull(T* ptr, ErrorCode code, const std::string& context) {
        if (!ptr) {
            throw NLMError(code, "Null pointer dereference: " + context);
        }
        return true;
    }
    
    // Allocate memory with error handling
    template<typename T>
    static T* safeAllocate(size_t count, ErrorCode allocationFailedCode = ErrorCode::ALLOCATION_FAILED,
                          const std::string& context = "") {
        try {
            T* ptr = new T[count];
            return ptr;
        } catch (const std::bad_alloc&) {
            throw NLMError(allocationFailedCode, 
                          "Memory allocation failed for " + std::to_string(count * sizeof(T)) + 
                          " bytes: " + context);
        }
    }
    
    // Validate configuration parameters
    static bool validateConfig(const std::map<std::string, double>& params,
                              const std::map<std::string, std::pair<double, double>>& ranges) {
        for (const auto& param : params) {
            auto it = ranges.find(param.first);
            if (it != ranges.end()) {
                if (param.second < it->second.first || param.second > it->second.second) {
                    std::cerr << "Warning: Parameter " << param.first 
                              << " value " << param.second
                              << " is outside expected range [" 
                              << it->second.first << ", " << it->second.second << "]\n";
                    return false;
                }
            }
        }
        return true;
    }
    
private:
    static void logError(const NLMError& error) {
        std::cerr << error.formatError() << std::endl;
        
        // Also log to logger if available
        // NLM_LOG_ERROR(error.what());
    }
};

// Error recovery utilities
class ErrorRecovery {
public:
    // Rollback multi-step operation
    template<typename Operation>
    static bool rollbackOperation(std::vector<Operation>& operations, 
                                 size_t startIndex, 
                                 const std::string& operationName) {
        bool success = true;
        std::string errorMessage;
        
        for (size_t i = operations.size() - 1; i >= startIndex; --i) {
            try {
                if (!operations[i].undo()) {
                    errorMessage = "Failed to undo operation " + std::to_string(i);
                    success = false;
                    // Continue to try undoing other operations
                }
            } catch (const std::exception& e) {
                errorMessage = "Exception during rollback: " + std::string(e.what());
                success = false;
                // Continue to try undoing other operations
            }
        }
        
        if (!success) {
            NLM_LOG_ERROR(operationName + " rollback failed: " + errorMessage);
        }
        
        return success;
    }
    
    // Timeout wrapper for long-running operations
    template<typename Func, typename... Args>
    static bool withTimeout(Func&& func, std::chrono::milliseconds timeout,
                           Args&&... args) {
        std::future<bool> future = std::async(std::launch::async, func, std::forward<Args>(args)...);
        
        std::future_status status = future.wait_for(timeout);
        if (status == std::future_status::timeout) {
            NLM_LOG_ERROR("Operation timed out after " + 
                         std::to_string(timeout.count()) + " ms");
            return false;
        }
        
        return future.get();
    }
    
    // Graceful degradation for optional features
    template<typename Func, typename... Args>
    static auto gracefulDegradation(Func&& func, Args&&... args,
                                   const std::string& featureName = "Optional Feature") {
        try {
            return func(std::forward<Args>(args)...);
        } catch (const NLMError& e) {
            NLM_LOG_WARNING(featureName + " failed: " + std::string(e.what()) + 
                           ". System will continue with degraded functionality.");
            return decltype(func(std::forward<Args>(args)...)){}; // Return default
        } catch (const std::exception& e) {
            NLM_LOG_WARNING(featureName + " failed with standard exception: " + 
                           std::string(e.what()) + ". System will continue with degraded functionality.");
            return decltype(func(std::forward<Args>(args)...)){}; // Return default
        }
    }
};

} // namespace nlm

#endif // NLM_ERROR_HANDLING_H