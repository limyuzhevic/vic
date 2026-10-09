#pragma once

#include <string>
#include <vector>
#include <functional>
#include <chrono>
#include <thread>
#include <atomic>
#include <future>
#include <unordered_map>
#include <memory>
#include <algorithm>
#include <sstream>
#include <iomanip>

namespace nlm {

// Error recovery strategies
enum class RecoveryStrategy {
    Retry,
    Degradation,
    Fallback,
    Cleanup,
    Abort
};

// Retry configuration
struct RetryConfig {
    int maxRetries = 3;
    std::chrono::milliseconds initialDelay = std::chrono::milliseconds(100);
    double backoffMultiplier = 2.0;
    std::chrono::milliseconds maxDelay = std::chrono::milliseconds(5000);
    std::vector<ErrorCode> retryableErrors = {
        ErrorCode::TIMEOUT,
        ErrorCode::OUT_OF_MEMORY,
        ErrorCode::COMPUTATION_OVERLOADED,
        ErrorCode::BANDWIDTH_EXCEEDED,
        ErrorCode::LATENCY_EXCEEDED
    };
    
    RetryConfig() = default;
    
    RetryConfig(int maxRetries, std::chrono::milliseconds initialDelay,
                double backoffMultiplier = 2.0, std::chrono::milliseconds maxDelay = std::chrono::milliseconds(5000))
        : maxRetries(maxRetries), initialDelay(initialDelay), 
          backoffMultiplier(backoffMultiplier), maxDelay(maxDelay) {}
};

// Error recovery result
struct RecoveryResult {
    bool success = false;
    std::string message;
    int retryCount = 0;
    std::chrono::milliseconds totalTime;
    ErrorCode lastErrorCode = ErrorCode::OK;
    
    RecoveryResult() = default;
    
    RecoveryResult(bool success, const std::string& message, int retryCount = 0)
        : success(success), message(message), retryCount(retryCount) {}
};

// Error recovery mechanisms
class ErrorRecovery {
public:
    // Retry operation with exponential backoff
    template<typename Function>
    static RecoveryResult executeWithRetry(const Function& operation, 
                                           const RetryConfig& config = RetryConfig(),
                                           const std::string& operationName = "Operation") {
        RecoveryResult result;
        result.success = false;
        
        for (int attempt = 0; attempt <= config.maxRetries; ++attempt) {
            try {
                auto startTime = std::chrono::steady_clock::now();
                operation();
                auto endTime = std::chrono::steady_clock::now();
                
                result.success = true;
                result.retryCount = attempt;
                result.totalTime = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);
                result.message = operationName + " succeeded on attempt " + std::to_string(attempt + 1);
                return result;
            } catch (const NLMException& e) {
                auto endTime = std::chrono::steady_clock::now();
                result.totalTime = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);
                result.lastErrorCode = e.getCode();
                
                // Check if error is retryable
                bool isRetryable = std::find(config.retryableErrors.begin(), 
                                           config.retryableErrors.end(), e.getCode()) != config.retryableErrors.end();
                
                if (!isRetryable || attempt == config.maxRetries) {
                    result.message = operationName + " failed: " + e.getMessage();
                    return result;
                }
                
                // Exponential backoff
                std::chrono::milliseconds delay = config.initialDelay;
                for (int i = 0; i < attempt; ++i) {
                    delay = std::min(delay * static_cast<std::chrono::milliseconds>(config.backoffMultiplier), 
                                   config.maxDelay);
                }
                
                std::this_thread::sleep_for(delay);
            } catch (const std::exception& e) {
                result.message = operationName + " failed with std::exception: " + std::string(e.what());
                break;
            }
        }
        
        return result;
    }
    
    // Execute operation with graceful degradation
    template<typename Function>
    static RecoveryResult executeWithDegradation(const Function& operation,
                                                 const std::vector<std::function<void()>>& fallbackOperations,
                                                 const std::string& operationName = "Operation") {
        RecoveryResult result;
        result.success = false;
        
        try {
            operation();
            result.success = true;
            result.message = operationName + " succeeded with full functionality";
            return result;
        } catch (const NLMException& e) {
            result.lastErrorCode = e.getCode();
            result.message = operationName + " failed: " + e.getMessage();
            
            // Try fallback operations
            for (size_t i = 0; i < fallbackOperations.size(); ++i) {
                try {
                    fallbackOperations[i]();
                    result.success = true;
                    result.message += " - Fallback operation " + std::to_string(i + 1) + " succeeded";
                    break;
                } catch (const std::exception& fallbackEx) {
                    result.message += " - Fallback operation " + std::to_string(i + 1) + " failed: " + 
                                    std::string(fallbackEx.what());
                }
            }
        }
        
        return result;
    }
    
    // Execute operation with resource cleanup
    template<typename Function, typename CleanupFunction>
    static RecoveryResult executeWithCleanup(const Function& operation,
                                             const CleanupFunction& cleanup,
                                             const std::string& operationName = "Operation") {
        RecoveryResult result;
        result.success = false;
        
        try {
            operation();
            result.success = true;
            result.message = operationName + " succeeded";
            return result;
        } catch (...) {
            // Always attempt cleanup, even if it fails
            try {
                cleanup();
                result.message = operationName + " failed but cleanup completed";
            } catch (const std::exception& cleanupEx) {
                result.message = operationName + " failed and cleanup failed: " + 
                                std::string(cleanupEx.what());
            }
            throw;
        }
    }
    
    // Execute operation with resource cleanup and retry
    template<typename Function, typename CleanupFunction>
    static RecoveryResult executeWithRetryAndCleanup(const Function& operation,
                                                     const CleanupFunction& cleanup,
                                                     const RetryConfig& config = RetryConfig(),
                                                     const std::string& operationName = "Operation") {
        for (int attempt = 0; attempt <= config.maxRetries; ++attempt) {
            try {
                return executeWithCleanup(operation, cleanup, operationName);
            } catch (const NLMException& e) {
                if (attempt == config.maxRetries) {
                    throw;
                }
                // Cleanup before retry
                try {
                    cleanup();
                } catch (const std::exception&) {}
            }
        }
        throw NLMException(ErrorCode::UNKNOWN_ERROR, ErrorCategory::Generic, ErrorSeverity::Error,
                          "All retry attempts failed", "", 0, "");
    }
    
    // Execute operation with all recovery mechanisms
    template<typename Function, typename CleanupFunction>
    static RecoveryResult executeWithFullRecovery(const Function& operation,
                                                   const CleanupFunction& cleanup,
                                                   const std::vector<std::function<void()>>& fallbackOperations,
                                                   const RetryConfig& retryConfig = RetryConfig(),
                                                   const std::string& operationName = "Operation") {
        RecoveryResult result;
        
        // First try with retry and cleanup
        result = executeWithRetryAndCleanup(operation, cleanup, retryConfig, operationName);
        if (result.success) {
            return result;
        }
        
        // If that fails, try with degradation
        return executeWithDegradation(operation, fallbackOperations, operationName);
    }
    
    // Validate resource availability before operation
    static bool validateResources(const std::function<bool()>& resourceCheck,
                                  const std::string& resourceName = "Resources") {
        try {
            return resourceCheck();
        } catch (...) {
            Logger::getGlobal().error(resourceName + " validation failed");
            return false;
        }
    }
    
    // Safe resource allocation with cleanup
    template<typename Allocator>
    static auto allocateSafe(Allocator allocator) {
        try {
            auto ptr = allocator();
            if (!ptr) {
                throw MemoryError("Resource allocation failed", 0, "", 0, "");
            }
            return ptr;
        } catch (...) {
            throw MemoryError("Safe allocation failed", 0, "", 0, "");
        }
    }
};

// Error utilities
class ErrorUtilities {
public:
    // Safe exception throwing with validation
    template<typename ExceptionType, typename... Args>
    static void throwIf(bool condition, Args&&... args) {
        if (condition) {
            throw ExceptionType(std::forward<Args>(args)...);
        }
    }
    
    // Safe exception throwing with custom error
    template<typename ExceptionType, typename Predicates>
    static void throwIf(Predicates&& predicate, const std::string& message = "") {
        if (predicate()) {
            throw ExceptionType(message, "", 0, "");
        }
    }
    
    // Execute operation with exception handling
    template<typename Function>
    static auto tryExecute(const Function& operation, const std::string& operationName = "Operation") {
        try {
            return operation();
        } catch (const NLMException& e) {
            Logger::getGlobal().error("Operation failed: " + operationName + " - " + e.getMessage());
        } catch (const std::exception& e) {
            Logger::getGlobal().error("Operation failed: " + operationName + " - " + std::string(e.what()));
        }
        return std::nullopt;
    }
    
    // Convert exception to NLMException
    static std::unique_ptr<NLMException> wrapException(const std::exception& e) {
        return std::make_unique<NLMException>(ErrorCode::UNKNOWN_ERROR, 
                                             ErrorCategory::Generic, 
                                             ErrorSeverity::Error, 
                                             e.what());
    }
    
    // Log exception with context
    static void logException(const NLMException& e, ErrorSeverity severity = ErrorSeverity::Error) {
        std::ostringstream oss;
        oss << "Exception caught: " << e.getMessage();
        
        if (severity == ErrorSeverity::Warning) {
            Logger::getGlobal().warning(oss.str(), e.getFile(), e.getLine(), e.what());
        } else if (severity == ErrorSeverity::Critical) {
            Logger::getGlobal().critical(oss.str(), e.getFile(), e.getLine(), e.what());
        } else {
            Logger::getGlobal().error(oss.str(), e.getFile(), e.getLine(), e.what());
        }
    }
    
    // Batch error reporting
    static void reportBatch(const std::vector<std::unique_ptr<NLMException>>& exceptions) {
        if (exceptions.empty()) return;
        
        Logger::getGlobal().error("Batch error report - " + std::to_string(exceptions.size()) + " errors occurred");
        
        for (const auto& e : exceptions) {
            Logger::getGlobal().error("- " + e->formatMessage());
        }
    }
    
    // Get error statistics
    struct ErrorStats {
        ErrorCode code;
        std::string category;
        ErrorSeverity severity;
        std::chrono::system_clock::time_point timestamp;
        std::string context;
    };
    
    static std::vector<ErrorStats> getErrorStats(const std::vector<std::unique_ptr<NLMException>>& exceptions) {
        std::vector<ErrorStats> stats;
        for (const auto& e : exceptions) {
            ErrorStats stat;
            stat.code = e->getCode();
            stat.category = Exception::errorCategoryToString(e->getCategory());
            stat.severity = e->getSeverity();
            stat.timestamp = std::chrono::system_clock::now();
            stat.context = e->formatMessage();
            stats.push_back(stat);
        }
        return stats;
    }
};

} // namespace nlm
