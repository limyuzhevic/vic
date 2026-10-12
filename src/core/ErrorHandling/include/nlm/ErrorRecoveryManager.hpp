// Error handling and recovery system
#pragma once

#include "NLMError.hpp"
#include "SpecificErrors.hpp"
#include "../Logger/Logger.hpp"
#include <thread>
#include <future>
#include <unordered_map>
#include <functional>
#include <chrono>
#include <numeric>
#include <algorithm>

namespace nlm {

// Error handling strategies for recovery
enum class RecoveryStrategy {
    None,             // No recovery
    Retry,            // Retry the operation
    Fallback,         // Use fallback functionality
    Degrade,          // Reduce functionality gracefully
    CheckpointRestore,// Restore from checkpoint
    CircuitBreaker,   // Open circuit breaker to prevent cascade failures
    ResourceReclaim,  // Reclaim resources
    Repair,           // Attempt to repair the issue
    Isolate,          // Isolate the problematic component
    Escalate,         // Escalate to human operator
    Wait,             // Wait and retry later
    Reset,            // Reset system state
    Migrate,          // Migrate to alternative system
    Ignore            // Ignore the error
};

// Error aggregation and correlation
struct ErrorGroup {
    std::string id;
    std::vector<std::unique_ptr<NLMError>> errors;
    std::chrono::system_clock::time_point firstOccurred;
    std::chrono::system_clock::time_point lastOccurred;
    uint32_t frequency;
    ErrorSeverity severity;
    std::unordered_map<std::string, std::string> correlationData;
    bool isCorrelated;

    ErrorGroup(const std::string& errorId) : id(errorId), frequency(0), isCorrelated(false) {
        firstOccurred = std::chrono::system_clock::now();
        lastOccurred = firstOccurred;
        severity = ErrorSeverity::Error;
    }

    void addError(std::unique_ptr<NLMError> error) {
        errors.push_back(std::move(error));
        lastOccurred = std::chrono::system_clock::now();
        frequency++;
        if (severity < errors.back()->getSeverity()) {
            severity = errors.back()->getSeverity();
        }
    }

    std::string getSummary() const {
        std::stringstream ss;
        ss << "Error Group " << id << ": " << frequency << " errors, Severity: "
           << static_cast<int>(severity) << ", First: "
           << std::chrono::system_clock::to_time_t(firstOccurred)
           << ", Last: " << std::chrono::system_clock::to_time_t(lastOccurred);
        return ss.str();
    }
};

// Error handler interface
class IErrorHandler {
public:
    virtual ~IErrorHandler() = default;
    virtual void handleError(std::unique_ptr<NLMError> error) = 0;
    virtual std::vector<std::unique_ptr<NLMError>> handleErrorSync(std::unique_ptr<NLMError> error) = 0;
    virtual bool canHandle(const NLMError& error) const = 0;
    virtual std::string getName() const = 0;
    virtual void configure(const std::unordered_map<std::string, std::string>& config) = 0;
    virtual std::string getConfiguration() const = 0;
};

// Error recovery manager
class ErrorRecoveryManager {
public:
    ErrorRecoveryManager() : maxConcurrentRecoveries_(4), recoveryTimeoutMs_(30000) {
        recoveryStats_.successfulRecoveries = 0;
        recoveryStats_.failedRecoveries = 0;
        recoveryStats_.totalRecoveryTimeMs = 0;
    }

    // Attempt to recover from an error
    std::unique_ptr<NLMError> attemptRecovery(std::unique_ptr<NLMError> error) {
        if (!error) return nullptr;

        std::lock_guard<std::mutex> lock(recoveryMutex_);

        // Check if we can handle this error
        if (!canHandleRecovery(*error)) {
            return std::move(error); // Cannot recover, return as-is
        }

        // Determine recovery strategy
        RecoveryStrategy strategy = determineRecoveryStrategy(*error);
        if (strategy == RecoveryStrategy::None) {
            return std::move(error);
        }

        // Check recovery limits
        if (getActiveRecoveryCount() >= maxConcurrentRecoveries_) {
            addRecoveryAction(error.get(), "queued", "System at capacity");
            return std::move(error);
        }

        // Perform recovery
        auto result = performRecovery(strategy, std::move(error));
        updateRecoveryStats(strategy, result != nullptr);
        return result;
    }

    // Recovery strategy selection
    RecoveryStrategy determineRecoveryStrategy(const NLMError& error) const {
        ErrorClassification classification = error.getClassification();
        ErrorCategory category = error.getCategory();
        ErrorSeverity severity = error.getSeverity();

        switch (classification) {
            case ErrorClassification::Retryable:
                return RecoveryStrategy::Retry;
            case ErrorClassification::Fallback:
                return RecoveryStrategy::Fallback;
            case ErrorClassification::Degrade:
                return RecoveryStrategy::Degrade;
            case ErrorClassification::Checkpoint:
                return RecoveryStrategy::CheckpointRestore;
            case ErrorClassification::Repair:
                return RecoveryStrategy::Repair;
            case ErrorClassification::Isolate:
                return RecoveryStrategy::Isolate;
            case ErrorClassification::Escalate:
                return RecoveryStrategy::Escalate;
            case ErrorClassification::Ignore:
                return RecoveryStrategy::Ignore;
            default:
                break;
        }

        switch (category) {
            case ErrorCategory::Memory:
                return RecoveryStrategy::ResourceReclaim;
            case ErrorCategory::System:
                return RecoveryStrategy::Reset;
            case ErrorCategory::Network:
                return RecoveryStrategy::CircuitBreaker;
            case ErrorCategory::Performance:
                return RecoveryStrategy::Degrade;
            case ErrorCategory::Temporary:
                return RecoveryStrategy::Wait;
            case ErrorCategory::Security:
                return RecoveryStrategy::Isolate;
            default:
                return RecoveryStrategy::Retry;
        }
    }

    // Recovery execution
    std::unique_ptr<NLMError> performRecovery(RecoveryStrategy strategy, std::unique_ptr<NLMError> error) {
        if (!error) return nullptr;

        std::promise<std::unique_ptr<NLMError>> resultPromise;
        std::future<std::unique_ptr<NLMError>> resultFuture = resultPromise.get_future();

        // Execute recovery in thread pool
        std::thread([this, strategy, error = std::move(error), &resultPromise]() mutable {
            try {
                std::unique_ptr<NLMError> result = executeRecovery(strategy, std::move(error));
                resultPromise.set_value(std::move(result));
            } catch (...) {
                resultPromise.set_exception(std::current_exception());
            }
        }).detach();

        // Wait for result with timeout
        auto status = resultFuture.wait_for(std::chrono::milliseconds(recoveryTimeoutMs_));
        if (status == std::future_status::ready) {
            try {
                return resultFuture.get();
            } catch (...) {
                return error; // Return original error on exception
            }
        } else {
            addRecoveryAction(error.get(), "timeout", "Recovery operation timed out");
            return error; // Return original error on timeout
        }
    }

    // Individual recovery implementations
    std::unique_ptr<NLMError> executeRecovery(RecoveryStrategy strategy, std::unique_ptr<NLMError> error) {
        switch (strategy) {
            case RecoveryStrategy::Retry:
                return executeRetryRecovery(std::move(error));
            case RecoveryStrategy::Fallback:
                return executeFallbackRecovery(std::move(error));
            case RecoveryStrategy::Degrade:
                return executeDegradeRecovery(std::move(error));
            case RecoveryStrategy::CheckpointRestore:
                return executeCheckpointRecovery(std::move(error));
            case RecoveryStrategy::CircuitBreaker:
                return executeCircuitBreakerRecovery(std::move(error));
            case RecoveryStrategy::ResourceReclaim:
                return executeResourceReclaimRecovery(std::move(error));
            case RecoveryStrategy::Repair:
                return executeRepairRecovery(std::move(error));
            case RecoveryStrategy::Isolate:
                return executeIsolateRecovery(std::move(error));
            case RecoveryStrategy::Escalate:
                return executeEscalateRecovery(std::move(error));
            case RecoveryStrategy::Wait:
                return executeWaitRecovery(std::move(error));
            case RecoveryStrategy::Reset:
                return executeResetRecovery(std::move(error));
            case RecoveryStrategy::Migrate:
                return executeMigrateRecovery(std::move(error));
            case RecoveryStrategy::Ignore:
                return nullptr; // Error is ignorable
            default:
                return error; // Default: return error as-is
        }
    }

    // Recovery statistics
    struct RecoveryStats {
        uint64_t successfulRecoveries;
        uint64_t failedRecoveries;
        uint64_t totalRecoveryTimeMs;
        uint64_t consecutiveFailures;
        std::unordered_map<RecoveryStrategy, uint64_t> strategyCounts;

        RecoveryStats() : successfulRecoveries(0), failedRecoveries(0), 
                         totalRecoveryTimeMs(0), consecutiveFailures(0) {}

        std::string getSummary() const {
            std::stringstream ss;
            ss << "Recovery Stats:\n"
               << "  Successful: " << successfulRecoveries << "\n"
               << "  Failed: " << failedRecoveries << "\n"
               << "  Success Rate: " 
               << (successfulRecoveries + failedRecoveries > 0 ? 
                   (static_cast<double>(successfulRecoveries) / (successfulRecoveries + failedRecoveries) * 100) : 0)
               << "%\n"
               << "  Avg Recovery Time: " 
               << (successfulRecoveries > 0 ? totalRecoveryTimeMs / successfulRecoveries : 0) << " ms\n"
               << "  Consecutive Failures: " << consecutiveFailures;
            return ss.str();
        }
    };

    RecoveryStats getRecoveryStats() const { return recoveryStats_; }
    void resetRecoveryStats() {
        std::lock_guard<std::mutex> lock(recoveryMutex_);
        recoveryStats_.successfulRecoveries = 0;
        recoveryStats_.failedRecoveries = 0;
        recoveryStats_.totalRecoveryTimeMs = 0;
        recoveryStats_.consecutiveFailures = 0;
    }

    // Error context and diagnostics
    void recordErrorContext(NLMError* error, const std::string& contextKey, const std::string& contextValue) {
        if (error) {
            error->addContext(contextKey, contextValue);
        }
    }

    void recordPerformanceMetrics(NLMError* error) {
        if (!error) return;

        // Record memory usage
        recordMemoryUsage(error);

        // Record processing time
        recordProcessingTime(error);

        // Record CPU usage
        recordCPUUsage(error);
    }

    // Error filtering and matching
    bool shouldHandleError(const NLMError& error) const {
        ErrorSeverity severity = error.getSeverity();
        ErrorCategory category = error.getCategory();

        // Filter by severity
        if (severity < minHandledSeverity_) {
            return false;
        }

        // Filter by category
        if (!categoriesToHandle_.empty() && 
            categoriesToHandle_.find(category) == categoriesToHandle_.end()) {
            return false;
        }

        return true;
    }

    // Error handlers management
    void registerErrorHandler(std::unique_ptr<IErrorHandler> handler) {
        if (handler) {
            handlers_.push_back(std::move(handler));
        }
    }

    void unregisterErrorHandler(const std::string& name) {
        handlers_.erase(
            std::remove_if(handlers_.begin(), handlers_.end(),
                          [&name](const std::unique_ptr<IErrorHandler>& handler) {
                              return handler->getName() == name;
                          }),
            handlers_.end()
        );
    }

    // Error aggregation
    void addErrorToGroup(const std::string& groupId, std::unique_ptr<NLMError> error) {
        std::lock_guard<std::mutex> lock(groupMutex_);
        if (errorGroups_.find(groupId) == errorGroups_.end()) {
            errorGroups_[groupId] = std::make_unique<ErrorGroup>(groupId);
        }
        errorGroups_[groupId]->addError(std::move(error));
    }

    std::unique_ptr<ErrorGroup> getErrorGroup(const std::string& groupId) const {
        std::lock_guard<std::mutex> lock(groupMutex_);
        auto it = errorGroups_.find(groupId);
        if (it != errorGroups_.end()) {
            return std::make_unique<ErrorGroup>(*it->second);
        }
        return nullptr;
    }

    std::vector<std::unique_ptr<ErrorGroup>> getAllErrorGroups() const {
        std::lock_guard<std::mutex> lock(groupMutex_);
        std::vector<std::unique_ptr<ErrorGroup>> result;
        for (const auto& pair : errorGroups_) {
            result.push_back(std::make_unique<ErrorGroup>(*pair.second));
        }
        return result;
    }

    // Error monitoring and alerting
    void setErrorAlertThreshold(uint32_t threshold) { errorAlertThreshold_ = threshold; }
    void setErrorAlertInterval(uint32_t intervalMs) { errorAlertIntervalMs_ = intervalMs; }

    // Configuration
    void configure(const std::unordered_map<std::string, std::string>& config) {
        if (config.find("max_concurrent_recoveries") != config.end()) {
            maxConcurrentRecoveries_ = std::stoul(config.at("max_concurrent_recoveries"));
        }
        if (config.find("recovery_timeout_ms") != config.end()) {
            recoveryTimeoutMs_ = std::stoul(config.at("recovery_timeout_ms"));
        }
        if (config.find("min_handled_severity") != config.end()) {
            minHandledSeverity_ = stringToSeverity(config.at("min_handled_severity"));
        }
        if (config.find("categories_to_handle") != config.end()) {
            // Parse categories
        }
    }

    std::string getConfiguration() const {
        std::stringstream ss;
        ss << "{\"max_concurrent_recoveries\": " << maxConcurrentRecoveries_ << ",\n"
           << " \"recovery_timeout_ms\": " << recoveryTimeoutMs_ << ",\n"
           << " \"min_handled_severity\": \"" << severityToString(minHandledSeverity_) << "\"\n}";
        return ss.str();
    }

protected:
    // Helper methods
    bool canHandleRecovery(const NLMError& error) const {
        return shouldHandleError(error) && 
               error.getRetryCount() < error.getMaxRetries();
    }

    uint32_t getActiveRecoveryCount() const {
        uint32_t count = 0;
        for (const auto& group : activeRecoveryGroups_) {
            count += group.second;
        }
        return count;
    }

    void addRecoveryAction(NLMError* error, const std::string& action, const std::string& reason) {
        if (error) {
            error->addRecoveryAction(action, reason);
        }
    }

    void updateRecoveryStats(RecoveryStrategy strategy, bool success) {
        std::lock_guard<std::mutex> lock(recoveryMutex_);
        recoveryStats_.strategyCounts[strategy]++;
        if (success) {
            recoveryStats_.successfulRecoveries++;
            recoveryStats_.consecutiveFailures = 0;
        } else {
            recoveryStats_.failedRecoveries++;
            recoveryStats_.consecutiveFailures++;
        }
    }

    // Individual recovery implementations
    std::unique_ptr<NLMError> executeRetryRecovery(std::unique_ptr<NLMError> error) {
        error->incrementRetryCount();
        if (error->getRetryCount() <= error->getMaxRetries()) {
            addRecoveryAction(error.get(), "retried", "Attempt " + std::to_string(error->getRetryCount()) + "/" + std::to_string(error->getMaxRetries()));
            return nullptr; // Return nullptr to indicate recovery was successful (no error)
        }
        return error;
    }

    std::unique_ptr<NLMError> executeFallbackRecovery(std::unique_ptr<NLMError> error) {
        addRecoveryAction(error.get(), "fallback_used", "Using fallback functionality");
        // Could implement fallback logic here
        return nullptr; // Return nullptr to indicate recovery was successful
    }

    std::unique_ptr<NLMError> executeDegradeRecovery(std::unique_ptr<NLMError> error) {
        addRecoveryAction(error.get(), "functionality_degraded", "Reduced functionality to continue");
        // Could implement degradation logic here
        return nullptr; // Return nullptr to indicate recovery was successful
    }

    std::unique_ptr<NLMError> executeCheckpointRecovery(std::unique_ptr<NLMError> error) {
        addRecoveryAction(error.get(), "checkpoint_restore", "Restoring from checkpoint");
        // Could implement checkpoint restore logic here
        return nullptr; // Return nullptr to indicate recovery was successful
    }

    std::unique_ptr<NLMError> executeCircuitBreakerRecovery(std::unique_ptr<NLMError> error) {
        addRecoveryAction(error.get(), "circuit_breaker_opened", "Opening circuit breaker to prevent cascade failures");
        // Could implement circuit breaker logic here
        return nullptr; // Return nullptr to indicate recovery was successful
    }

    std::unique_ptr<NLMError> executeResourceReclaimRecovery(std::unique_ptr<NLMError> error) {
        addRecoveryAction(error.get(), "resources_reclaimed", "Reclaiming system resources");
        // Could implement resource reclamation logic here
        return nullptr; // Return nullptr to indicate recovery was successful
    }

    std::unique_ptr<NLMError> executeRepairRecovery(std::unique_ptr<NLMError> error) {
        addRecoveryAction(error.get(), "repair_attempted", "Attempting to repair the issue");
        // Could implement repair logic here
        return error; // Keep error as repair might have failed
    }

    std::unique_ptr<NLMError> executeIsolateRecovery(std::unique_ptr<NLMError> error) {
        addRecoveryAction(error.get(), "component_isolated", "Isolating problematic component");
        // Could implement isolation logic here
        return nullptr; // Return nullptr to indicate recovery was successful
    }

    std::unique_ptr<NLMError> executeEscalateRecovery(std::unique_ptr<NLMError> error) {
        addRecoveryAction(error.get(), "escalation_triggered", "Escalating to human operator");
        // Could implement escalation logic here
        return error; // Keep error for human review
    }

    std::unique_ptr<NLMError> executeWaitRecovery(std::unique_ptr<NLMError> error) {
        addRecoveryAction(error.get(), "wait_requested", "Waiting before retry");
        // Could implement wait logic here
        return error; // Keep error for retry later
    }

    std::unique_ptr<NLMError> executeResetRecovery(std::unique_ptr<NLMError> error) {
        addRecoveryAction(error.get(), "system_reset", "Resetting system state");
        // Could implement reset logic here
        return nullptr; // Return nullptr to indicate recovery was successful
    }

    std::unique_ptr<NLMError> executeMigrateRecovery(std::unique_ptr<NLMError> error) {
        addRecoveryAction(error.get(), "migration_attempted", "Migrating to alternative system");
        // Could implement migration logic here
        return nullptr; // Return nullptr to indicate recovery was successful
    }

    // Performance monitoring helpers
    void recordMemoryUsage(NLMError* error) {
        if (error) {
            // Record peak memory usage (placeholder)
            error->addDiagnostic("peak_memory_usage_kb", "0");
        }
    }

    void recordProcessingTime(NLMError* error) {
        if (error) {
            // Record processing time (placeholder)
            error->addDiagnostic("processing_time_ms", "0");
        }
    }

    void recordCPUUsage(NLMError* error) {
        if (error) {
            // Record CPU usage (placeholder)
            error->addDiagnostic("cpu_usage_percent", "0");
        }
    }

    // Conversion utilities
    ErrorSeverity stringToSeverity(const std::string& severityStr) const {
        std::string upper = severityStr;
        std::transform(upper.begin(), upper.end(), upper.begin(), ::toupper);
        if (upper == "DEBUG") return ErrorSeverity::Debug;
        if (upper == "INFO") return ErrorSeverity::Info;
        if (upper == "WARNING") return ErrorSeverity::Warning;
        if (upper == "ERROR") return ErrorSeverity::Error;
        if (upper == "CRITICAL") return ErrorSeverity::Critical;
        if (upper == "FATAL") return ErrorSeverity::Fatal;
        if (upper == "ALERT") return ErrorSeverity::Alert;
        return ErrorSeverity::Error;
    }

    std::string severityToString(ErrorSeverity severity) const {
        switch (severity) {
            case ErrorSeverity::Debug: return "DEBUG";
            case ErrorSeverity::Info: return "INFO";
            case ErrorSeverity::Warning: return "WARNING";
            case ErrorSeverity::Error: return "ERROR";
            case ErrorSeverity::Critical: return "CRITICAL";
            case ErrorSeverity::Fatal: return "FATAL";
            case ErrorSeverity::Alert: return "ALERT";
            default: return "ERROR";
        }
    }

    // Error groups for active recovery tracking
    std::unordered_map<std::string, uint32_t> activeRecoveryGroups_;

    // Thread safety
    mutable std::mutex recoveryMutex_;
    mutable std::mutex groupMutex_;

    // Configuration
    uint32_t maxConcurrentRecoveries_;
    uint32_t recoveryTimeoutMs_;
    ErrorSeverity minHandledSeverity_;
    uint32_t errorAlertThreshold_;
    uint32_t errorAlertIntervalMs_;
    std::unordered_set<ErrorCategory> categoriesToHandle_;

    // Statistics
    RecoveryStats recoveryStats_;

    // Error groups
    std::unordered_map<std::string, std::unique_ptr<ErrorGroup>> errorGroups_;

    // Error handlers
    std::vector<std::unique_ptr<IErrorHandler>> handlers_;
};

} // namespace nlm