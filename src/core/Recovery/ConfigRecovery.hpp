// Configuration validation and recovery for NLM
#pragma once

#include "core/Config/Config.hpp"
#include "core/Errors/ErrorSystem.hpp"
#include "core/Logger/Logger.hpp"
#include <string>
#include <vector>
#include <memory>
#include <functional>
#include <unordered_map>

namespace nlm {

// Validation error structure
struct ValidationError {
    std::string field;
    std::string message;
    ValidationSeverity severity;
    std::string errorCode;
    std::string suggestedAction;
    
    ValidationError() : severity(ValidationSeverity::Warning) {}
    ValidationError(const std::string& f, const std::string& m,
                    ValidationSeverity s, const std::string& ec,
                    const std::string& sa = "")
        : field(f), message(m), severity(s), errorCode(ec), suggestedAction(sa) {}
};

// Configuration validation schema
class ConfigValidator {
private:
    struct ValidationRule {
        std::string field;
        std::function<bool(const ConfigValue&)> validator;
        ValidationSeverity severity;
        std::string errorMessage;
        std::string errorCode;
        std::string suggestedAction;
    };
    
    std::vector<ValidationRule> rules_;
    
public:
    // Add validation rule
    void addRule(const std::string& field,
                 std::function<bool(const ConfigValue&)> validator,
                 ValidationSeverity severity = ValidationSeverity::Error,
                 const std::string& errorMessage = "",
                 const std::string& errorCode = "",
                 const std::string& suggestedAction = "");
    
    // Add range validation
    void addRangeRule(const std::string& field, double min, double max,
                      ValidationSeverity severity = ValidationSeverity::Error,
                      const std::string& errorMessage = "",
                      const std::string& errorCode = "",
                      const std::string& suggestedAction = "");
    
    // Add enum validation
    void addEnumRule(const std::string& field,
                     const std::vector<std::string>& validValues,
                     ValidationSeverity severity = ValidationSeverity::Error,
                     const std::string& errorMessage = "",
                     const std::string& errorCode = "",
                     const std::string& suggestedAction = "");
    
    // Add custom validation rule
    void addCustomRule(const std::string& field,
                       std::function<bool(const ConfigValue&, const std::string&, const Config&)> validator,
                       ValidationSeverity severity = ValidationSeverity::Error,
                       const std::string& errorMessage = "",
                       const std::string& errorCode = "",
                       const std::string& suggestedAction = "");
    
    // Validate configuration
    std::vector<ValidationError> validate(const Config& config) const;
    
    // Validate specific field
    std::vector<ValidationError> validateField(const Config& config, const std::string& field) const;
    
    // Check if configuration is valid
    bool isValid(const Config& config) const;
    
    // Get validation errors
    std::vector<ValidationError> getErrors(const Config& config) const;
    
    // Get critical errors only
    std::vector<ValidationError> getCriticalErrors(const Config& config) const;
    
    // Get warnings only
    std::vector<ValidationError> getWarnings(const Config& config) const;
};

// Configuration recovery system
class ConfigRecovery {
private:
    std::shared_ptr<ConfigValidator> validator_;
    std::vector<std::function<bool(Config&)>> recoveryStrategies_;
    bool enableAutoRecovery_;
    bool enableGracefulDegradation_;
    
public:
    ConfigRecovery()
        : enableAutoRecovery_(true), enableGracefulDegradation_(true) {
        validator_ = std::make_shared<ConfigValidator>();
        initDefaultRules();
    }
    
    // Add recovery strategy
    void addRecoveryStrategy(std::function<bool(Config&)> strategy);
    
    // Set recovery options
    void setAutoRecoveryEnabled(bool enabled);
    void setGracefulDegradationEnabled(bool enabled);
    
    // Validate and recover configuration
    bool validateAndRecover(Config& config) const;
    
    // Sanitize configuration
    void sanitize(Config& config) const;
    
    // Apply safe defaults
    void applySafeDefaults(Config& config) const;
    
    // Fix invalid values
    void fixInvalidValues(Config& config) const;
    
    // Get validation errors
    std::vector<ValidationError> getValidationErrors(const Config& config) const;
    
private:
    void initDefaultRules();
    bool attemptRecovery(Config& config) const;
};

// System state checkpoint and recovery
class SystemCheckpoint {
private:
    std::string checkpointPath_;
    bool enableCheckpointing_;
    size_t maxCheckpointHistory_;
    std::chrono::system_clock::time_point lastCheckpoint_;
    
public:
    SystemCheckpoint()
        : enableCheckpointing_(true), maxCheckpointHistory_(10), lastCheckpoint_() {}
    
    // Set checkpoint configuration
    void setCheckpointPath(const std::string& path);
    void setEnableCheckpointing(bool enabled);
    void setMaxCheckpointHistory(size_t history);
    
    // Checkpoint system state
    bool checkpoint(const std::string& filepath, const Config& config) const;
    
    // Load system state from checkpoint
    bool restore(const std::string& filepath, Config& config) const;
    
    // List available checkpoints
    std::vector<std::string> listCheckpoints(const std::string& directory) const;
    
    // Cleanup old checkpoints
    bool cleanupOldCheckpoints(const std::string& directory) const;
    
    // Get last checkpoint time
    std::chrono::system_clock::time_point getLastCheckpointTime() const;
    
    // Set last checkpoint time
    void setLastCheckpointTime(const std::chrono::system_clock::time_point& time);
};

// Memory recovery and cleanup
class MemoryRecovery {
private:
    size_t maxMemoryUsage_;
    bool enableMemoryMonitoring_;
    std::chrono::system_clock::time_point lastMemoryCheck_;
    
public:
    MemoryRecovery()
        : maxMemoryUsage_(1024 * 1024 * 1024),  // 1 GB default
          enableMemoryMonitoring_(true), lastMemoryCheck_() {}
    
    // Set memory recovery configuration
    void setMaxMemoryUsage(size_t maxBytes);
    void setMemoryMonitoringEnabled(bool enabled);
    
    // Check memory usage
    size_t getMemoryUsage() const;
    bool isMemoryUsageExceeded() const;
    
    // Cleanup memory
    void cleanupMemory() const;
    
    // Force garbage collection
    void forceGarbageCollection() const;
    
    // Record memory usage
    void recordMemoryUsage(size_t usage) const;
    
    // Get memory statistics
    struct MemoryStats {
        size_t currentUsage;
        size_t peakUsage;
        size_t availableMemory;
        std::chrono::system_clock::time_point lastCheck;
    };
    
    MemoryStats getMemoryStats() const;
    
    // Monitor memory usage
    void monitorMemory() const;
};

// Resource recovery system
class ResourceRecovery {
private:
    bool enableResourceMonitoring_;
    size_t maxFileHandles_;
    std::chrono::system_clock::time_point lastResourceCheck_;
    
public:
    ResourceRecovery()
        : enableResourceMonitoring_(true), maxFileHandles_(1024), lastResourceCheck_() {}
    
    // Set resource recovery configuration
    void setResourceMonitoringEnabled(bool enabled);
    void setMaxFileHandles(size_t maxHandles);
    
    // Check resource usage
    size_t getOpenFileHandles() const;
    bool isResourceUsageExceeded() const;
    
    // Cleanup resources
    void cleanupResources() const;
    
    // Close unused file handles
    void closeUnusedFileHandles() const;
    
    // Record resource usage
    void recordResourceUsage(size_t fileHandles) const;
    
    // Get resource statistics
    struct ResourceStats {
        size_t openFileHandles;
        size_t maxFileHandles;
        size_t availableHandles;
        std::chrono::system_clock::time_point lastCheck;
    };
    
    ResourceStats getResourceStats() const;
    
    // Monitor resource usage
    void monitorResources() const;
};

// Advanced error mitigation strategies
class ErrorMitigation {
public:
    // Error correlation and pattern analysis
    struct ErrorPattern {
        std::string errorCode;
        std::chrono::system_clock::time_point timestamp;
        ErrorSeverity severity;
        std::string context;
        double frequency; // Occurrences per time unit
    };
    
    // Error prediction and prevention
    bool predictError(const std::exception& error) const;
    
    // Error pattern analysis
    std::vector<ErrorPattern> analyzeErrorPatterns(const std::vector<std::exception>& errors) const;
    
    // Error mitigation strategies
    bool mitigateError(const std::exception& error) const;
    
    // Error prevention
    void preventError(const std::string& errorType) const;
    
    // Error suppression (for non-critical errors)
    bool shouldSuppressError(const std::exception& error) const;
    
    // Error escalation
    bool shouldEscalateError(const std::exception& error) const;
    
    // Error throttling
    bool shouldThrottleError(const std::exception& error) const;
    
    // Error aggregation
    std::exception aggregateErrors(const std::vector<std::exception>& errors) const;
};

// System recovery manager
class SystemRecoveryManager {
private:
    std::shared_ptr<ConfigValidator> configValidator_;
    std::shared_ptr<ConfigRecovery> configRecovery_;
    std::shared_ptr<SystemCheckpoint> checkpoint_;
    std::shared_ptr<MemoryRecovery> memoryRecovery_;
    std::shared_ptr<ResourceRecovery> resourceRecovery_;
    std::shared_ptr<ErrorMitigation> errorMitigation_;
    
    bool enableAutomaticRecovery_;
    bool enableGracefulDegradation_;
    bool enableCheckpointing_;
    bool enableMemoryMonitoring_;
    bool enableResourceMonitoring_;
    
public:
    SystemRecoveryManager()
        : enableAutomaticRecovery_(true), enableGracefulDegradation_(true),
          enableCheckpointing_(true), enableMemoryMonitoring_(true),
          enableResourceMonitoring_(true) {
        // Initialize components
        configValidator_ = std::make_shared<ConfigValidator>();
        configRecovery_ = std::make_shared<ConfigRecovery>();
        checkpoint_ = std::make_shared<SystemCheckpoint>();
        memoryRecovery_ = std::make_shared<MemoryRecovery>();
        resourceRecovery_ = std::make_shared<ResourceRecovery>();
        errorMitigation_ = std::make_shared<ErrorMitigation>();
    }
    
    // Configure recovery manager
    void setAutomaticRecoveryEnabled(bool enabled);
    void setGracefulDegradationEnabled(bool enabled);
    void setCheckpointingEnabled(bool enabled);
    void setMemoryMonitoringEnabled(bool enabled);
    void setResourceMonitoringEnabled(bool enabled);
    
    // System recovery workflow
    bool recoverFromError(const std::exception& error, Config& config);
    
    // Complete system recovery
    bool recoverSystem(Config& config);
    
    // Configuration recovery
    bool recoverConfiguration(Config& config);
    
    // State recovery
    bool recoverState(Config& config);
    
    // Resource recovery
    bool recoverResources(Config& config);
    
    // Memory recovery
    bool recoverMemory(Config& config);
    
    // Error mitigation
    bool mitigateError(const std::exception& error, Config& config);
    
    // System health check
    bool isSystemHealthy(Config& config) const;
    
    // Get recovery statistics
    struct RecoveryStats {
        size_t totalRecoveryAttempts;
        size_t successfulRecoveries;
        size_t failedRecoveries;
        std::chrono::system_clock::time_point lastRecoveryTime;
    };
    
    RecoveryStats getRecoveryStats() const;
    
    // Generate recovery report
    std::string generateRecoveryReport() const;
};

// Global recovery manager instance
inline SystemRecoveryManager& getGlobalRecoveryManager() {
    static SystemRecoveryManager manager;
    return manager;
}

// Convenience functions for configuration recovery
inline bool validateAndRecoverConfig(Config& config) {
    return getGlobalRecoveryManager().recoverConfiguration(config);
}

inline bool validateConfig(const Config& config) {
    return getGlobalRecoveryManager().isSystemHealthy(const_cast<Config&>(config));
}

inline std::vector<ValidationError> getConfigValidationErrors(const Config& config) {
    return getGlobalRecoveryManager().recoverConfiguration(const_cast<Config&>(config));
}

inline void applySafeDefaults(Config& config) {
    ConfigRecovery::getGlobalRecoveryManager().applySafeDefaults(config);
}

} // namespace nlm
