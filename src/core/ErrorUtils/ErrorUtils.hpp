#pragma once

#include <string>
#include <map>
#include <vector>
#include <functional>
#include <chrono>
#include <thread>
#include <atomic>
#include <future>
#include <type_traits>
#include <optional>
#include <any>
#include <mutex>
#include <condition_variable>
#include <chrono>
#include <cmath>
#include <limits>
#include <sstream>

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
    static std::optional<typename std::invoke_result<Function>::type>
    tryExecute(const Function& operation, const std::string& operationName = "Operation") {
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
