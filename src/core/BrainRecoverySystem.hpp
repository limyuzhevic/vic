// Brain Recovery System - Error handling and recovery mechanisms
// Extracted from Brain::reset() and error handling in step() method
#pragma once

#include "brain/Brain.hpp"
#include "core/BrainExceptions.hpp"
#include <memory>
#include <functional>
#include <chrono>
#include <unordered_map>
#include <vector>

namespace nlm {

class BrainRecoverySystem {
public:
    enum class RecoveryLevel {
        None,           // No recovery - fail immediately
        Basic,          // Reset to safe state
        Partial,        // Continue with degraded functionality
        Full,           // Restore full functionality
        Emergency       // Halt and save for analysis
    };
    
    struct RecoveryStrategy {
        std::string name;
        RecoveryLevel level;
        std::function<bool()> canApply;
        std::function<void()> apply;
        double priority;  // Higher priority = apply first
        size_t maxAttempts;
        std::chrono::seconds cooldown;
        
        RecoveryStrategy(std::string n, RecoveryLevel lvl, 
                        std::function<bool()> canApplyFn,
                        std::function<void()> applyFn,
                        double priority = 1.0, size_t maxAttempts = 1,
                        std::chrono::seconds cooldown = std::chrono::seconds(0))
            : name(std::move(n)), level(lvl), canApply(std::move(canApplyFn)),
              apply(std::move(applyFn)), priority(priority),
              maxAttempts(maxAttempts), cooldown(cooldown) {}
    };
    
    struct ErrorInfo {
        std::chrono::system_clock::time_point timestamp;
        BrainException exception;
        std::string context;
        size_t severity;  // 1-10, higher = more severe
        bool recovered;
        RecoveryLevel recoveryApplied;
        
        ErrorInfo(const BrainException& exc, const std::string& ctx, size_t sev = 5)
            : timestamp(std::chrono::system_clock::now()), exception(exc),
              context(ctx), severity(sev), recovered(false),
              recoveryApplied(RecoveryLevel::None) {}
    };
    
    // Recovery strategies
    BrainRecoverySystem(std::shared_ptr<Brain> brain);
    ~BrainRecoverySystem();
    
    // Error handling and recovery
    bool handleError(const BrainException& error, const std::string& context = "");
    bool attemptRecovery(RecoveryLevel level);
    void resetRecoveryState();
    
    // Recovery strategies
    void registerRecoveryStrategy(const RecoveryStrategy& strategy);
    void applyRecoveryStrategies();
    
    // Error information
    void recordError(const BrainException& error, const std::string& context = "");
    const std::vector<ErrorInfo>& getErrorHistory() const { return errorHistory_; }
    size_t getErrorCount() const { return errorHistory_.size(); }
    size_t getConsecutiveErrors() const { return consecutiveErrors_; }
    
    // Recovery status
    bool isInRecoveryMode() const { return recoveryActive_; }
    RecoveryLevel getCurrentRecoveryLevel() const { return currentRecoveryLevel_; }
    bool shouldRecover() const { return recoveryActive_ || !errorHistory_.empty(); }
    
    // Checkpoint and rollback
    bool saveRecoveryState(const std::string& path);
    bool restoreRecoveryState(const std::string& path);
    bool rollbackToCheckpoint();
    
    // Configuration
    void setMaxErrorsBeforeRecovery(size_t maxErrors) { maxErrorsBeforeRecovery_ = maxErrors; }
    void setRecoveryCooldown(size_t cooldownSeconds) { recoveryCooldown_ = cooldownSeconds; }
    void setEmergencyThreshold(size_t errors) { emergencyThreshold_ = errors; }
    
    // Statistics
    size_t getTotalRecoveryAttempts() const { return totalRecoveryAttempts_; }
    size_t getSuccessfulRecoveries() const { return successfulRecoveries_; }
    double getRecoverySuccessRate() const {
        return totalRecoveryAttempts_ > 0 ? 
            static_cast<double>(successfulRecoveries_) / totalRecoveryAttempts_ : 0.0;
    }
    
private:
    std::shared_ptr<Brain> brain_;
    
    // Error tracking
    std::vector<ErrorInfo> errorHistory_;
    size_t consecutiveErrors_;
    size_t totalRecoveryAttempts_;
    size_t successfulRecoveries_;
    
    // Recovery state
    bool recoveryActive_;
    RecoveryLevel currentRecoveryLevel_;
    std::chrono::system_clock::time_point lastRecoveryTime_;
    std::map<std::string, size_t> strategyAttempts_;
    
    // Configuration
    size_t maxErrorsBeforeRecovery_;
    size_t recoveryCooldown_;
    size_t emergencyThreshold_;
    
    // Registered strategies
    std::vector<RecoveryStrategy> recoveryStrategies_;
    
    // Helper methods
    void updateErrorStats();
    void cleanupOldErrors();
    bool shouldApplyStrategy(const RecoveryStrategy& strategy) const;
    void executeStrategy(const RecoveryStrategy& strategy);
    RecoveryLevel determineRecoveryLevel(const ErrorInfo& error) const;
    void logRecoveryAction(const std::string& strategyName, bool success);
};

// Pre-defined recovery strategies
namespace RecoveryStrategies {
    // Basic recovery strategies
    RecoveryStrategy CreateBasicResetStrategy(std::shared_ptr<Brain> brain);
    RecoveryStrategy CreateMemoryResetStrategy(std::shared_ptr<Brain> brain);
    RecoveryStrategy CreatePlasticityResetStrategy(std::shared_ptr<Brain> brain);
    
    // Partial recovery strategies
    RecoveryStrategy CreateDegradedOperationStrategy(std::shared_ptr<Brain> brain);
    RecoveryStrategy CreateAttentionCompensationStrategy(std::shared_ptr<Brain> brain);
    
    // Advanced recovery strategies
    RecoveryStrategy CreateCheckpointRollbackStrategy(std::shared_ptr<Brain> brain);
    RecoveryStrategy CreateNetworkRecoveryStrategy(std::shared_ptr<Brain> brain);
    
    // Emergency strategies
    RecoveryStrategy CreateHaltStrategy(std::shared_ptr<Brain> brain);
    RecoveryStrategy CreateAnalysisModeStrategy(std::shared_ptr<Brain> brain);
}

} // namespace nlm
