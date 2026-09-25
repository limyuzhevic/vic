// Error Recovery Utilities and Fallback Mechanisms

#pragma once

#include <string>
#include <memory>
#include <functional>
#include <vector>
#include <chrono>
#include <map>
#include <set>
#include <unordered_map>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <fstream>
#include <sstream>

#include "nlm_error.hpp"

namespace nlm {

// Recovery Strategy Types
enum class RecoveryStrategy {
    None,
    Retry,
    Fallback,
    Degrade,
    Recoverable,
    CircularBuffer,
    CheckpointRestore,
    GracefulShutdown,
    AdaptiveRetry,
    ResourceReallocation,
    ConfigReset
};

// Recovery action
struct RecoveryAction {
    std::string name;
    RecoveryStrategy strategy;
    std::function<bool()> recoveryFunction;
    std::function<bool()> preCondition;
    std::chrono::milliseconds timeout;
    int maxRetries;
    std::chrono::milliseconds retryDelay;
    std::string errorContext;
    bool active;
    
    RecoveryAction(const std::string& n, RecoveryStrategy s, 
                   std::function<bool()> func,
                   const std::string& context = "",
                   int retries = 3, std::chrono::milliseconds delay = std::chrono::milliseconds(100))
        : name(n), strategy(s), recoveryFunction(func), preCondition(nullptr),
          timeout(std::chrono::milliseconds(5000)), maxRetries(retries),
          retryDelay(delay), errorContext(context), active(true) {}
};

// Fallback configuration
struct FallbackConfig {
    std::string name;
    std::map<std::string, std::string> fallbackValues;
    std::map<std::string, std::string> degradedSettings;
    bool autoRecover;
    std::chrono::milliseconds recoveryTimeout;
    double degradationThreshold;
    
    FallbackConfig(const std::string& n, bool auto = true,
                   std::chrono::milliseconds timeout = std::chrono::milliseconds(10000),
                   double threshold = 0.8)
        : name(n), autoRecover(auto), recoveryTimeout(timeout),
          degradationThreshold(threshold) {}
};

// Error recovery manager
class ErrorRecoveryManager {
private:
    std::vector<RecoveryAction> recoveryActions;
    std::vector<FallbackConfig> fallbackConfigs;
    std::map<std::string, std::vector<std::string>> errorHistory;
    std::mutex recoveryMutex;
    std::condition_variable recoveryCV;
    bool recoveryActive;
    
    // Default recovery actions
    void setupDefaultRecoveryActions();
    
    // Resource monitoring
    struct ResourceUsage {
        double cpuUsage;
        double memoryUsage;
        uint64_t diskSpaceAvailable;
        std::chrono::system_clock::time_point timestamp;
    };
    
    ResourceUsage currentResourceUsage() const;
    bool isResourcePressureHigh() const;
    
    // Recovery action execution
    bool executeRecoveryAction(const RecoveryAction& action);
    bool canExecuteAction(const RecoveryAction& action) const;
    
    // Fallback management
    std::string getFallbackValue(const std::string& key, const std::map<std::string, std::string>& sourceMap,
                                 const std::string& defaultValue = "") const;
    
public:
    ErrorRecoveryManager();
    ~ErrorRecoveryManager();
    
    // Add recovery action
    void addRecoveryAction(const RecoveryAction& action);
    
    // Add fallback configuration
    void addFallbackConfig(const FallbackConfig& config);
    
    // Attempt recovery
    bool attemptRecovery(const std::string& errorContext, const NLMError& error);
    
    // Check if system is in recovery mode
    bool isInRecoveryMode() const { return recoveryActive; }
    
    // Get recovery status
    std::string getRecoveryStatus() const;
    
    // Execute specific recovery action by name
    bool executeRecoveryAction(const std::string& actionName);
    
    // Get list of available recovery actions
    std::vector<std::string> getAvailableRecoveryActions() const;
    
    // Reset recovery state
    void resetRecoveryState();
    
    // Monitor and optimize recovery
    void monitorAndOptimize();
    
    // Save recovery state
    bool saveRecoveryState(const std::string& filepath) const;
    
    // Load recovery state
    bool loadRecoveryState(const std::string& filepath);
};

// Resource manager for recovery
class ResourceManager {
private:
    struct ResourceInfo {
        std::string name;
        std::string type;
        double currentUsage;
        double maxCapacity;
        std::chrono::system_clock::time_point lastCheck;
        bool available;
        std::string backupResource;
    };
    
    std::map<std::string, ResourceInfo> resources;
    std::mutex resourceMutex;
    bool healthMonitoringActive;
    
public:
    ResourceManager();
    ~ResourceManager();
    
    // Resource registration
    void registerResource(const std::string& name, const std::string& type,
                         double maxCapacity, const std::string& backupResource = "");
    
    // Resource allocation
    bool allocateResource(const std::string& name, double amount);
    bool deallocateResource(const std::string& name, double amount);
    
    // Resource status
    bool isResourceAvailable(const std::string& name, double required = 1.0) const;
    double getResourceUsage(const std::string& name) const;
    double getResourceCapacity(const std::string& name) const;
    
    // Health check
    bool performHealthCheck();
    std::string getResourceHealthReport() const;
    
    // Emergency reallocation
    bool emergencyReallocate(const std::string& primaryResource, double amount,
                            const std::string& alternativeResource);
    
    // Resource backup/recovery
    bool createResourceBackup(const std::string& resourceName, const std::string& backupPath);
    bool restoreResourceBackup(const std::string& resourceName, const std::string& backupPath);
};

// Checkpoint manager for error recovery
class CheckpointManager {
private:
    struct Checkpoint {
        std::string name;
        std::chrono::system_clock::time_point timestamp;
        std::map<std::string, std::string> stateData;
        std::string metadata;
        uint64_t sequenceNumber;
    };
    
    std::vector<Checkpoint> checkpoints;
    uint64_t currentSequenceNumber;
    std::string checkpointDirectory;
    size_t maxCheckpoints;
    bool autoCheckpointEnabled;
    std::chrono::milliseconds checkpointInterval;
    std::mutex checkpointMutex;
    
public:
    CheckpointManager(const std::string& directory = "checkpoints",
                     size_t maxPoints = 100,
                     bool autoEnabled = true,
                     std::chrono::milliseconds interval = std::chrono::milliseconds(60000));
    
    // Create checkpoint
    bool createCheckpoint(const std::string& name, const std::map<std::string, std::string>& stateData);
    
    // Restore from checkpoint
    bool restoreFromCheckpoint(const std::string& name, std::map<std::string, std::string>& outStateData);
    
    // Get checkpoint list
    std::vector<std::string> getCheckpointNames() const;
    
    // Checkpoint management
    bool deleteCheckpoint(const std::string& name);
    bool cleanupOldCheckpoints(size_t keepCount = 50);
    
    // Checkpoint info
    Checkpoint getCheckpointInfo(const std::string& name) const;
    
    // Recovery helpers
    bool canRecoverFromCheckpoint(const std::string& name) const;
    std::vector<std::string> getRecoverableCheckpoints() const;
    
    // Automatic checkpointing
    void enableAutoCheckpointing(bool enable);
    void updateCheckpoint();
    
    // Checkpoint integrity check
    bool verifyCheckpointIntegrity(const std::string& name) const;
};

// Adaptive error recovery system
class AdaptiveRecoverySystem {
private:
    struct ErrorPattern {
        ErrorCategory category;
        ErrorSeverity severity;
        std::string component;
        std::chrono::system_clock::time_point firstOccurrence;
        int occurrenceCount;
        double recoverySuccessRate;
        std::chrono::milliseconds averageRecoveryTime;
    };
    
    std::map<std::string, ErrorPattern> errorPatterns;
    std::chrono::system_clock::time_point lastOptimization;
    bool learningEnabled;
    double patternThreshold;
    std::chrono::milliseconds optimizationInterval;
    
    // Pattern learning
    void analyzeErrorPattern(const NLMError& error);
    void optimizeRecoveryStrategies();
    
    // Adaptive strategies
    RecoveryStrategy selectOptimalStrategy(const NLMError& error);
    int adjustRetryCount(const std::string& errorContext) const;
    std::chrono::milliseconds adjustRetryDelay(const std::string& errorContext) const;
    
public:
    AdaptiveRecoverySystem(bool learningEnabled = true,
                          double threshold = 0.7,
                          std::chrono::milliseconds interval = std::chrono::milliseconds(300000));
    
    // Analyze and adapt
    void analyzeError(const NLMError& error);
    std::string getLearningReport() const;
    
    // Get recommendations
    std::vector<std::string> getRecoveryRecommendations() const;
    
    // Configuration
    void enableLearning(bool enable) { learningEnabled = enable; }
    bool isLearningEnabled() const { return learningEnabled; }
};

// Recovery utilities
class RecoveryUtilities {
public:
    // Safe file operations
    static bool safeReadFile(const std::string& path, std::string& content, 
                            const std::string& errorContext = "");
    
    static bool safeWriteFile(const std::string& path, const std::string& content,
                             const std::string& errorContext = "");
    
    static bool safeRemoveFile(const std::string& path, const std::string& errorContext = "");
    
    // Configuration recovery
    static bool recoverConfig(const std::string& configPath, std::map<std::string, std::string>& recoveredConfig,
                             const std::string& errorContext = "");
    
    // Memory recovery
    static bool forceGarbageCollection();
    static bool checkMemoryStatus(double& currentMemoryUsage, double& maxMemoryUsage);
    
    // Resource recovery
    static bool releaseUnusedResources();
    static bool cleanupTemporaryFiles(const std::string& pattern = "tmp*");
    
    // System recovery
    static bool restartComponent(const std::string& componentName);
    static bool resetToSafeState(const std::map<std::string, std::string>& safeSettings);
    
    // Network recovery
    static bool reconnectNetworkResource(const std::string& resourceName);
    static bool retryNetworkOperation(const std::function<bool()>& operation, int maxRetries = 3);
    
    // File system recovery
    static bool validateFileSystemIntegrity(const std::string& path = ".");
    static bool repairFileSystem(const std::string& path = ".");
    
    // Configuration backup and restore
    static bool createConfigBackup(const std::string& sourcePath, const std::string& backupPath);
    static bool restoreConfigBackup(const std::string& backupPath, const std::string& targetPath);
    
    // Error log analysis
    static std::vector<NLMError> analyzeErrorLog(const std::vector<NLMError>& errorLog);
    static bool generateErrorReport(const std::vector<NLMError>& errorLog, const std::string& outputPath);
    
    // Performance optimization for recovery
    static bool optimizePerformanceForRecovery();
    static bool prepareForEmergencyRecovery();
    
    // Recovery validation
    static bool validateRecovery(const std::function<bool()>& recoveryAction, 
                                 const std::string& validationContext = "");
};

// Global error recovery manager instances
extern ErrorRecoveryManager globalRecoveryManager;
extern ResourceManager globalResourceManager;
extern CheckpointManager globalCheckpointManager;
extern AdaptiveRecoverySystem globalAdaptiveRecoverySystem;

// Convenience recovery macros
#define NLM_ATTEMPT_RECOVERY(error_context, error) \
    nlm::globalRecoveryManager.attemptRecovery(error_context, error)

#define NLM_RESOURCE_AVAILABLE(resource) \
    nlm::globalResourceManager.isResourceAvailable(resource, 1.0)

#define NLM_CREATE_CHECKPOINT(name, state) \
    nlm::globalCheckpointManager.createCheckpoint(name, state)

#define NLM_RESTORE_CHECKPOINT(name, out_state) \
    nlm::globalCheckpointManager.restoreFromCheckpoint(name, out_state)

#define NLM_ANALYZE_ERROR(error) \
    nlm::globalAdaptiveRecoverySystem.analyzeError(error)
}