#pragma once

#include <string>
#include <memory>
#include <vector>
#include <variant>
#include <optional>
#include <map>
#include <functional>
#include <any>
#include <fstream>
#include <sstream>
#include <filesystem>
#include <stdexcept>
#include <algorithm>
#include <cctype>
#include <chrono>
#include <unordered_map>
#include <limits>
#include <regex>
#include <stack>
#include <unordered_set>
#include <mutex>
#include "Config.hpp"

namespace nlm {

// Forward declarations
class RuntimeConfigManager;

// Runtime configuration modification types
enum class RuntimeChangeType {
    Enable,
    Disable,
    ParameterAdjustment,
    Switch,
    Insert,
    Remove,
    Replace,
    Custom
};

// Configuration change operation
struct ConfigOperation {
    std::string key;
    RuntimeChangeType type;
    ConfigValue value;
    std::string description;
    ConfigSource source;
    std::chrono::system_clock::time_point timestamp;
    bool validated;
    std::vector<std::string> validationErrors;
    
    ConfigOperation(const std::string& k, RuntimeChangeType t, const ConfigValue& v,
                   const std::string& desc = "", ConfigSource src = ConfigSource::Runtime)
        : key(k), type(t), value(v), description(desc), source(src),
          timestamp(std::chrono::system_clock::now()), validated(false) {}
};

// Runtime configuration change with undo support
class RuntimeConfigChange {
public:
    std::string changeId;
    std::string key;
    ConfigValue oldValue;
    ConfigValue newValue;
    ConfigSource source;
    std::chrono::system_clock::time_point timestamp;
    std::vector<std::string> validationErrors;
    bool applied;
    
    RuntimeConfigChange(const std::string& id, const std::string& k, 
                       const ConfigValue& oldVal, const ConfigValue& newVal,
                       ConfigSource src = ConfigSource::Runtime)
        : changeId(id), key(k), oldValue(oldVal), newValue(newVal), source(src),
          timestamp(std::chrono::system_clock::now()), applied(false) {}
};

// Configuration snapshot
struct ConfigSnapshot {
    std::string name;
    std::chrono::system_clock::time_point timestamp;
    std::vector<RuntimeConfigChange> changes;
    std::string description;
    
    ConfigSnapshot(const std::string& n, const std::string& desc = "")
        : name(n), timestamp(std::chrono::system_clock::now()), description(desc) {}
};

// Change validator interface
class ChangeValidator {
public:
    virtual ~ChangeValidator() = default;
    virtual bool validate(const ConfigOperation& operation, const Config& currentConfig) = 0;
    virtual std::vector<std::string> getValidationErrors(const ConfigOperation& operation, 
                                                       const Config& currentConfig) = 0;
    virtual std::string getName() const = 0;
};

// Configuration history manager
class ConfigHistory {
public:
    using ChangeCallback = std::function<void(const RuntimeConfigChange&)>;
    using ValidationCallback = std::function<void(const ConfigOperation&, const std::vector<std::string>&)>;
    
    ConfigHistory(size_t maxHistorySize = 1000, size_t maxSnapshots = 100);
    
    // Record a configuration change
    bool recordChange(const RuntimeConfigChange& change);
    
    // Apply a configuration change with validation
    bool applyChange(const ConfigOperation& operation, Config& config, 
                     const std::vector<std::shared_ptr<ChangeValidator>>& validators);
    
    // Undo the last change
    bool undoChange(Config& config);
    
    // Redo the last undone change
    bool redoChange(Config& config);
    
    // Create a snapshot
    std::string createSnapshot(const std::string& name, const std::string& description = "");
    
    // Rollback to a snapshot
    bool rollbackToSnapshot(const std::string& name, Config& config);
    
    // Get change history
    std::vector<RuntimeConfigChange> getChanges() const;
    
    // Get snapshots
    std::vector<ConfigSnapshot> getSnapshots() const;
    
    // Get changes by time range
    std::vector<RuntimeConfigChange> getChanges(const std::chrono::system_clock::time_point& from,
                                                const std::chrono::system_clock::time_point& to) const;
    
    // Set callbacks
    void setChangeCallback(ChangeCallback callback) { changeCallback_ = callback; }
    void setValidationCallback(ValidationCallback callback) { validationCallback_ = callback; }
    
    // Get change diff
    std::string getChangeDiff(const RuntimeConfigChange& change) const;
    
    // Get history summary
    std::string getHistorySummary() const;
    
    // Clear history
    void clearHistory();
    
    // Export history to file
    bool exportHistory(const std::string& filepath, const std::string& format = "json") const;
    
    // Import history from file
    bool importHistory(const std::string& filepath);
    
    // Get current state hash for comparison
    std::string getCurrentStateHash(const Config& config) const;
    
    // Check if key has valid type
    bool isValidType(const std::string& key, const ConfigValue& value, const Config& config) const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Callbacks
    ChangeCallback changeCallback_;
    ValidationCallback validationCallback_;
    
    // Configuration
    size_t maxHistorySize_;
    size_t maxSnapshots_;
};

// Configuration monitoring
class ConfigMonitor {
public:
    using MonitorCallback = std::function<void(const ConfigChangeEvent&)>;
    
    ConfigMonitor();
    
    // Add monitor callback
    void addMonitorCallback(MonitorCallback callback);
    
    // Remove monitor callback
    void removeMonitorCallback(MonitorCallback callback);
    
    // Get monitor statistics
    struct MonitorStats {
        size_t totalChanges = 0;
        size_t changesThisSecond = 0;
        std::chrono::system_clock::time_point lastChangeTime;
        double avgChangeInterval = 0.0;
        std::unordered_map<std::string, size_t> changesByKey;
    };
    
    MonitorStats getStats() const;
    
    // Reset statistics
    void resetStats();
    
    // Export monitor data
    bool exportMonitorData(const std::string& filepath) const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Runtime configuration manager
class RuntimeConfigManager {
public:
    RuntimeConfigManager(std::shared_ptr<Config> config);
    
    // Apply runtime configuration change
    bool applyRuntimeChange(const std::string& key, const ConfigValue& value,
                           const std::string& description = "",
                           const std::vector<std::shared_ptr<ChangeValidator>>& validators = {});
    
    // Apply runtime operation
    bool applyOperation(const ConfigOperation& operation,
                       const std::vector<std::shared_ptr<ChangeValidator>>& validators);
    
    // Get runtime configuration status
    struct RuntimeStatus {
        bool isInitialized = false;
        std::chrono::system_clock::time_point lastChangeTime;
        std::string lastChangeKey;
        std::string lastChangeDescription;
        size_t totalChanges = 0;
        double uptimeSeconds = 0.0;
    };
    
    RuntimeStatus getStatus() const;
    
    // Get configuration history
    std::shared_ptr<ConfigHistory> getHistory() const { return history_; }
    
    // Get configuration monitor
    std::shared_ptr<ConfigMonitor> getMonitor() const { return monitor_; }
    
    // Add change validator
    void addChangeValidator(std::shared_ptr<ChangeValidator> validator);
    
    // Remove change validator
    void removeChangeValidator(std::shared_ptr<ChangeValidator> validator);
    
    // Enable/disable runtime modifications
    void setEnabled(bool enabled) { enabled_ = enabled; }
    bool isEnabled() const { return enabled_; }
    
    // Pause/Resume runtime modifications
    void pause() { paused_ = true; }
    void resume() { paused_ = false; }
    bool isPaused() const { return paused_; }
    
    // Get validation errors for a potential change
    std::vector<std::string> validateChange(const std::string& key, const ConfigValue& value) const;
    
    // Create rollback point
    std::string createRollbackPoint(const std::string& description = "") const;
    
    // Apply rollback
    bool applyRollback(const std::string& rollbackPointId);
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    std::shared_ptr<Config> config_;
    std::shared_ptr<ConfigHistory> history_;
    std::shared_ptr<ConfigMonitor> monitor_;
    std::vector<std::shared_ptr<ChangeValidator>> validators_;
    bool enabled_;
    bool paused_;
};

// Built-in change validators
class RangeValidator : public ChangeValidator {
public:
    RangeValidator(const std::string& name, const std::string& key, 
                   double minValue, double maxValue);
    
    bool validate(const ConfigOperation& operation, const Config& currentConfig) override;
    std::vector<std::string> getValidationErrors(const ConfigOperation& operation, 
                                                const Config& currentConfig) override;
    std::string getName() const override { return name_; }
    
private:
    std::string name_;
    std::string key_;
    double minValue_;
    double maxValue_;
};

class DependencyValidator : public ChangeValidator {
public:
    DependencyValidator(const std::string& name, const std::string& key, 
                       const std::unordered_map<std::string, ConfigValue>& requiredState);
    
    bool validate(const ConfigOperation& operation, const Config& currentConfig) override;
    std::vector<std::string> getValidationErrors(const ConfigOperation& operation, 
                                                const Config& currentConfig) override;
    std::string getName() const override { return name_; }
    
private:
    std::string name_;
    std::string key_;
    std::unordered_map<std::string, ConfigValue> requiredState_;
};

class StateValidator : public ChangeValidator {
public:
    StateValidator(const std::string& name, std::function<bool(const Config&)> validator);
    
    bool validate(const ConfigOperation& operation, const Config& currentConfig) override;
    std::vector<std::string> getValidationErrors(const ConfigOperation& operation, 
                                                const Config& currentConfig) override;
    std::string getName() const override { return name_; }
    
private:
    std::string name_;
    std::function<bool(const Config&)> validator_;
};

// Runtime configuration commands for main system
namespace RuntimeConfigCommands {
    // Pause/resume simulation
    bool pauseSimulation(std::shared_ptr<RuntimeConfigManager> runtimeConfig);
    
    // Step simulation
    bool stepSimulation(std::shared_ptr<RuntimeConfigManager> runtimeConfig);
    
    // Apply runtime configuration
    bool applyRuntimeConfig(std::shared_ptr<RuntimeConfigManager> runtimeConfig, 
                           const std::string& key, const std::string& value);
    
    // List configuration changes
    void listChanges(std::shared_ptr<RuntimeConfigManager> runtimeConfig);
    
    // Show configuration history
    void showHistory(std::shared_ptr<RuntimeConfigManager> runtimeConfig);
    
    // Create snapshot
    std::string createSnapshot(std::shared_ptr<RuntimeConfigManager> runtimeConfig,
                              const std::string& name, const std::string& description);
    
    // Rollback to snapshot
    bool rollbackToSnapshot(std::shared_ptr<RuntimeConfigManager> runtimeConfig,
                           const std::string& name);
}

} // namespace nlm