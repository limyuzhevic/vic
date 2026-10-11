// RecoverySystem.hpp
#pragma once

#include <cstddef>
#include <cstdint>
#include <string>
#include <vector>
#include <memory>
#include <functional>
#include <map>
#include <set>
#include <unordered_map>
#include <chrono>
#include <atomic>
#include <thread>
#include <future>
#include <filesystem>
#include <algorithm>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <optional>
#include <variant>
#include "CheckpointManager.hpp"
#include "BrainCheckpointSerializer.hpp"
#include "MemoryCheckpointSerializer.hpp"

namespace nlm {

/**
 * Advanced recovery system for automatic checkpoint recovery and state reconstruction
 * 
 * Features:
 * - Automatic recovery from last valid checkpoint
 * - Incremental recovery (resume from most recent checkpoint)
 * - State reconstruction from checkpoints
 * - Partial recovery (from incomplete checkpoints)
 * - Rollback (revert to previous checkpoint states)
 * - Recovery validation and integrity checking
 * - Automatic recovery strategies
 * - Recovery logging and monitoring
 * - Recovery point identification
 * - Restoration progress tracking
 */

enum class RecoveryMode {
    Automatic,        // Automatically recover when failures occur
    Manual,           // Require user confirmation for recovery
    Hybrid,           // Combination of automatic and manual
    Conservative,     // Conservative recovery (minimal state changes)
    Aggressive        // Aggressive recovery (maximum state restoration)
};

enum class RecoveryStrategy {
    FullRestore,      // Restore complete state from checkpoint
    IncrementalRestore, // Restore only changed state from checkpoint
    DifferentialRestore, // Restore only differences from checkpoint
    PartialRestore,    // Restore only available components
    MinimalRestore     // Restore only critical state components
};

enum class RecoveryStatus {
    Idle,             // Not performing recovery
    Validating,       // Validating checkpoint integrity
    Loading,          // Loading checkpoint data
    Reconstructing,   // Reconstructing state from checkpoint
    Verifying,        // Verifying restored state
    Completed,        // Recovery completed successfully
    Failed,           // Recovery failed
    InProgress,       // Recovery in progress
    Paused,           // Recovery paused by user
    Cancelled        // Recovery cancelled by user
};

enum class RecoveryPointType {
    Stable,           // Checkpoint is stable and verified
    Candidate,        // Checkpoint is candidate for recovery
    Temporary,        // Temporary checkpoint for testing
    External,         // External checkpoint (from remote storage)
    Corrupted,        // Checkpoint is corrupted/invalid
    Pending,          // Checkpoint is pending validation
    Archived          // Checkpoint is archived
};

struct RecoveryPoint {
    std::string id;                    // Unique recovery point ID
    std::string checkpointId;          // Associated checkpoint ID
    uint64_t step;                     // Simulation step
    double simulationTime;             // Simulation time
    time_t timestamp;                  // Creation timestamp
    RecoveryPointType type;            // Type of recovery point
    std::string description;           // Description
    std::string reason;                // Why this point is recommended
    uint64_t stabilityScore{0};        // Stability score (0-100)
    uint64_t integrityScore{0};        // Integrity score (0-100)
    bool isAvailable{true};            // Is checkpoint available
    bool isValidated{false};           // Is checkpoint validated
    bool isStale{false};               // Is checkpoint stale
    double memoryUsage{0.0};           // Memory usage during checkpoint
    double recoveryTime{0.0};          // Time needed for recovery
    std::string recoveryPlan;          // Recovery plan description
    std::vector<std::string> dependencies; // Dependencies for recovery
    std::map<std::string, std::string> metadata; // Additional metadata
    
    RecoveryPoint() : step(0), simulationTime(0.0), timestamp(0), 
                     type(RecoveryPointType::Pending), stabilityScore(0), 
                     integrityScore(0), isAvailable(true), isValidated(false),
                     isStale(false), memoryUsage(0.0), recoveryTime(0.0) {}
    
    bool isGood() const {
        return isAvailable && isValidated && !isStale && 
               stabilityScore >= 70 && integrityScore >= 80;
    }
    
    std::string getAge() const {
        time_t now = std::time(nullptr);
        time_t age = now - timestamp;
        
        if (age < 60) {
            return "seconds ago";
        } else if (age < 3600) {
            return std::to_string(age / 60) + " minutes ago";
        } else if (age < 86400) {
            return std::to_string(age / 3600) + " hours ago";
        } else {
            return std::to_string(age / 86400) + " days ago";
        }
    }
};

struct RecoveryResult {
    bool success{false};               // Was recovery successful
    std::string message;               // Status message
    std::string checkpointId;          // Checkpoint ID used for recovery
    uint64_t recoveredStep{0};         // Step recovered to
    double recoveredSimulationTime{0.0}; // Simulation time recovered to
    double recoveryTime{0.0};          // Time taken for recovery
    uint64_t neuronsRecovered{0};      // Number of neurons recovered
    uint64_t synapsesRecovered{0};     // Number of synapses recovered
    uint64_t memoryItemsRecovered{0};  // Number of memory items recovered
    double memoryUsageRecovered{0.0};  // Memory usage after recovery
    std::vector<std::string> warnings; // Warnings during recovery
    std::vector<std::string> errors;   // Errors during recovery
    std::map<std::string, std::string> metadata; // Recovery metadata
    
    RecoveryResult() = default;
    RecoveryResult(bool success, const std::string& message) : 
        success(success), message(message) {}
};

struct RecoveryPolicy {
    RecoveryMode mode{RecoveryMode::Automatic};     // Recovery mode
    RecoveryStrategy strategy{RecoveryStrategy::FullRestore}; // Recovery strategy
    uint64_t maxRecoveryAttempts{3};               // Maximum recovery attempts
    uint64_t recoveryTimeout{300};                 // Recovery timeout (seconds)
    bool autoValidate{true};                      // Auto-validate checkpoints
    bool autoRetry{true};                          // Auto-retry failed recoveries
    bool backupBeforeRecovery{true};               // Create backup before recovery
    uint64_t backupRetention{3};                   // Number of backups to keep
    bool allowPartialRecovery{true};               // Allow partial recovery
    bool warnBeforeRecovery{true};                 // Warn before recovery
    bool logRecovery{true};                        // Log recovery operations
    std::set<std::string> skipRecoverySteps;       // Steps to skip in recovery
    std::map<std::string, std::string> customSettings; // Custom settings
    
    RecoveryPolicy() = default;
    RecoveryPolicy(RecoveryMode m, RecoveryStrategy s) : mode(m), strategy(s) {}
    
    bool shouldRecover(RecoveryStatus status, const RecoveryResult& result) const {
        if (status == RecoveryStatus::Completed) {
            return false;
        }
        
        if (status == RecoveryStatus::Failed && !autoRetry) {
            return false;
        }
        
        if (result.success) {
            return false;
        }
        
        return true;
    }
};

struct RecoveryStatistics {
    uint64_t totalRecoveryAttempts{0};        // Total recovery attempts
    uint64_t successfulRecoveries{0};        // Successful recoveries
    uint64_t failedRecoveries{0};            // Failed recoveries
    double averageRecoveryTime{0.0};         // Average recovery time
    uint64_t totalCheckpointsRestored{0};    // Total checkpoints restored
    uint64_t totalNeuronsRestored{0};        // Total neurons restored
    uint64_t totalSynapsesRestored{0};       // Total synapses restored
    uint64_t totalMemoryItemsRestored{0};    // Total memory items restored
    time_t lastRecoveryTime{0};             // Last recovery time
    std::string lastRecoveryCheckpointId;   // Last recovery checkpoint ID
    std::map<std::string, uint64_t> recoveryCounts; // Recovery counts by type
    
    void addRecovery(const RecoveryResult& result) {
        ++totalRecoveryAttempts;
        if (result.success) {
            ++successfulRecoveries;
            totalCheckpointsRestored++;
            totalNeuronsRestored += result.neuronsRecovered;
            totalSynapsesRestored += result.synapsesRecovered;
            totalMemoryItemsRestored += result.memoryItemsRecovered;
        } else {
            ++failedRecoveries;
        }
        
        lastRecoveryTime = std::time(nullptr);
        lastRecoveryCheckpointId = result.checkpointId;
    }
    
    double getSuccessRate() const {
        if (totalRecoveryAttempts == 0) return 0.0;
        return static_cast<double>(successfulRecoveries) / totalRecoveryAttempts;
    }
};

class RecoverySystem {
public:
    RecoverySystem();
    ~RecoverySystem();
    
    // Disable copying
    RecoverySystem(const RecoverySystem&) = delete;
    RecoverySystem& operator=(const RecoverySystem&) = delete;
    
    /**
     * Configure recovery system
     * @param checkpointDir Directory containing checkpoints
     * @param policy Recovery policy
     * @param checkpointManager Checkpoint manager instance
     * @param brainSerializer Brain checkpoint serializer
     * @param memorySerializer Memory checkpoint serializer
     */
    void configure(
        const std::string& checkpointDir,
        const RecoveryPolicy& policy = RecoveryPolicy(),
        CheckpointManager* checkpointManager = nullptr,
        BrainCheckpointSerializer* brainSerializer = nullptr,
        MemoryCheckpointSerializer* memorySerializer = nullptr
    );
    
    /**
     * Set recovery callback
     * @param callback Callback function for recovery progress
     */
    void setRecoveryCallback(
        std::function<void(double progress, const std::string& message)> callback
    ) {
        recoveryCallback_ = callback;
    }
    
    /**
     * Set recovery validation callback
     * @param callback Callback function for validation results
     */
    void setValidationCallback(
        std::function<void(bool valid, const std::string& error)> callback
    ) {
        validationCallback_ = callback;
    }
    
    /**
     * Recover from last valid checkpoint
     * @param reason Reason for recovery
     * @param strategy Recovery strategy
     * @return Recovery result
     */
    RecoveryResult recoverFromLastValid(const std::string& reason = "Last valid checkpoint",
                                        RecoveryStrategy strategy = RecoveryStrategy::FullRestore);
    
    /**
     * Recover from specific checkpoint
     * @param checkpointId Checkpoint ID to recover from
     * @param reason Reason for recovery
     * @param strategy Recovery strategy
     * @return Recovery result
     */
    RecoveryResult recoverFromCheckpoint(const std::string& checkpointId,
                                         const std::string& reason = "Specified checkpoint",
                                         RecoveryStrategy strategy = RecoveryStrategy::FullRestore);
    
    /**
     * Recover from checkpoint by step
     * @param step Simulation step
     * @param reason Reason for recovery
     * @param strategy Recovery strategy
     * @return Recovery result
     */
    RecoveryResult recoverFromStep(uint64_t step,
                                   const std::string& reason = "Checkpoint by step",
                                   RecoveryStrategy strategy = RecoveryStrategy::FullRestore);
    
    /**
     * Incremental recovery (resume from most recent checkpoint)
     * @param reason Reason for recovery
     * @param strategy Recovery strategy
     * @return Recovery result
     */
    RecoveryResult incrementalRecovery(const std::string& reason = "Incremental recovery",
                                       RecoveryStrategy strategy = RecoveryStrategy::IncrementalRestore);
    
    /**
     * Rollback to previous checkpoint state
     * @param checkpointId Checkpoint ID to rollback to
     * @param reason Reason for rollback
     * @param strategy Recovery strategy
     * @return Recovery result
     */
    RecoveryResult rollback(const std::string& checkpointId,
                            const std::string& reason = "Rollback to checkpoint",
                            RecoveryStrategy strategy = RecoveryStrategy::FullRestore);
    
    /**
     * List available recovery points
     * @param limit Maximum number of recovery points
     * @param includeUnstable Whether to include unstable recovery points
     * @return Vector of recovery points
     */
    std::vector<RecoveryPoint> listRecoveryPoints(
        size_t limit = 20,
        bool includeUnstable = false
    ) const;
    
    /**
     * Find best recovery point
     * @param reason Reason for recovery
     * @return Best recovery point
     */
    std::optional<RecoveryPoint> findBestRecoveryPoint(const std::string& reason = "Best recovery point") const;
    
    /**
     * Validate recovery point
     * @param recoveryPoint Recovery point to validate
     * @return true if valid
     */
    bool validateRecoveryPoint(const RecoveryPoint& recoveryPoint) const;
    
    /**
     * Validate all recovery points
     * @return Number of valid recovery points
     */
    size_t validateAllRecoveryPoints();
    
    /**
     * Get recovery status
     */
    RecoveryStatus getStatus() const { return currentStatus_; }
    
    /**
     * Check if recovery is in progress
     */
    bool isRecoveryInProgress() const { return currentStatus_ != RecoveryStatus::Idle && 
                                          currentStatus_ != RecoveryStatus::Completed && 
                                          currentStatus_ != RecoveryStatus::Failed; }
    
    /**
     * Get current recovery checkpoint ID
     */
    std::string getCurrentRecoveryCheckpoint() const { return currentRecoveryCheckpoint_; }
    
    /**
     * Get recovery statistics
     */
    RecoveryStatistics getStatistics() const { return stats_; }
    
    /**
     * Get recovery policy
     */
    const RecoveryPolicy& getPolicy() const { return policy_; }
    
    /**
     * Update recovery policy
     */
    void setPolicy(const RecoveryPolicy& policy) { policy_ = policy; }
    
    /**
     * Get checkpoint directory
     */
    const std::string& getCheckpointDir() const { return checkpointDir_; }
    
    /**
     * Set checkpoint directory
     */
    void setCheckpointDir(const std::string& dir) { checkpointDir_ = dir; }
    
    /**
     * Create backup before recovery
     * @param checkpointId Checkpoint ID to backup
     * @return true if backup created successfully
     */
    bool createBackup(const std::string& checkpointId);
    
    /**
     * Restore from backup
     * @param backupPath Path to backup file
     * @return Recovery result
     */
    RecoveryResult restoreFromBackup(const std::string& backupPath);
    
    /**
     * Clean old backups
     */
    void cleanOldBackups();
    
    /**
     * Get backup count
     */
    size_t getBackupCount() const { return backupCount_; }
    
    /**
     * Enable/disable automatic recovery
     */
    void setAutoRecoveryEnabled(bool enabled) { autoRecoveryEnabled_ = enabled; }
    
    /**
     * Get auto recovery enabled
     */
    bool isAutoRecoveryEnabled() const { return autoRecoveryEnabled_; }
    
    /**
     * Enable/disable recovery logging
     */
    void setRecoveryLoggingEnabled(bool enabled) { recoveryLoggingEnabled_ = enabled; }
    
    /**
     * Get recovery logging enabled
     */"{"id":"value"}}'"': '{"version": 3.1}'}