# Enhanced Configuration System for NLM
# This file extends the basic Config class with advanced features

#pragma once

#include "Config.hpp"
#include <unordered_map>
#include <vector>
#include <string>
#include <memory>
#include <functional>
#include <chrono>
#include <mutex>
#include <stack>
#include <fstream>
#include <sstream>
#include <variant>
#include <cmath>
#include <algorithm>

namespace nlm {

// Forward declarations
class ParameterValidator;
class ConfigDependencyManager;
class ConfigPresetsManager;
class ConfigVersionManager;
class ConfigBackupManager;
class ConfigMacroManager;
class ConfigExpressionEvaluator;
class OptimizationEngine;
class SensitivityAnalyzer;
class ConfigInheritanceManager;

// Enhanced configuration value types
using EnhancedConfigValue = std::variant<
    int, int64_t, double, bool, std::string,
    std::vector<int>, std::vector<double>, std::vector<std::string>,
    std::vector<std::variant<int, double, bool, std::string>>,
    std::unordered_map<std::string, EnhancedConfigValue>
>;

// Parameter validation result structure
struct ParameterValidationResult {
    bool isValid;
    std::string errorMessage;
    std::vector<std::string> warnings;
    std::unordered_map<std::string, std::string> suggestions;
    
    ParameterValidationResult() : isValid(true) {}
    ParameterValidationResult(bool valid, const std::string& error) 
        : isValid(valid), errorMessage(error) {}
};

// Parameter dependency specification
struct ParameterDependency {
    std::string dependentKey;
    std::string requiredKey;
    std::function<bool(const EnhancedConfigValue&, const EnhancedConfigValue&)> dependencyRule;
    std::string dependencyType; // "required", "range_dependent", "conditional"
    
    ParameterDependency(const std::string& dependent, const std::string& required,
                       const std::function<bool(const EnhancedConfigValue&, const EnhancedConfigValue&)>& rule,
                       const std::string& type = "required")
        : dependentKey(dependent), requiredKey(required), 
          dependencyRule(rule), dependencyType(type) {}
};

// Parameter metadata for enhanced configuration
struct ParameterMetadata {
    std::string key;
    std::string description;
    std::string category;
    std::string type; // "basic", "advanced", "experimental"
    EnhancedConfigValue defaultValue;
    EnhancedConfigValue minValue;
    EnhancedConfigValue maxValue;
    std::vector<std::string> allowedValues;
    bool required;
    bool readOnly;
    std::vector<std::string> tags;
    std::unordered_map<std::string, std::string> extraAttributes;
    
    ParameterMetadata() : required(false), readOnly(false) {}
    ParameterMetadata(const std::string& k, const std::string& desc, const std::string& cat,
                     const EnhancedConfigValue& defaultVal, bool req = false)
        : key(k), description(desc), category(cat), defaultValue(defaultVal), required(req), readOnly(false) {}
};

// Parameter change notification
struct ParameterChangeNotification {
    std::string key;
    EnhancedConfigValue oldValue;
    EnhancedConfigValue newValue;
    ConfigSource source;
    std::chrono::system_clock::time_point timestamp;
    std::string reason;
    
    ParameterChangeNotification(const std::string& k, const EnhancedConfigValue& oldVal,
                               const EnhancedConfigValue& newVal, ConfigSource src,
                               const std::string& reason = "manual")
        : key(k), oldValue(oldVal), newValue(newVal), source(src), reason(reason) {
        timestamp = std::chrono::system_clock::now();
    }
};

// Configuration history entry
struct ConfigurationHistoryEntry {
    std::chrono::system_clock::time_point timestamp;
    std::string version;
    std::vector<ParameterChangeNotification> changes;
    std::string userComment;
    
    ConfigurationHistoryEntry(const std::string& ver, const std::string& comment = "")
        : version(ver), userComment(comment) {
        timestamp = std::chrono::system_clock::now();
    }
};

// Configuration change callback
using ConfigChangeCallback = std::function<void(const ParameterChangeNotification&)>;

// Enhanced configuration class with advanced features
class EnhancedConfig : public Config {
public:
    EnhancedConfig();
    EnhancedConfig(const EnhancedConfig&) = delete;
    EnhancedConfig& operator=(const EnhancedConfig&) = delete;
    EnhancedConfig(EnhancedConfig&&) noexcept = default;
    EnhancedConfig& operator=(EnhancedConfig&&) noexcept = default;
    ~EnhancedConfig() override;
    
    // Core configuration interface (overrides)
    bool loadFromFile(const std::string& filepath) override;
    bool loadFromArgs(int argc, char** argv) override;
    bool saveToFile(const std::string& filepath) const override;
    template<typename T>
    std::optional<T> get(const std::string& key) const override;
    template<typename T>
    T getOr(const std::string& key, const T& defaultValue) const override;
    void set(const std::string& key, const ConfigValue& value, ConfigSource source = ConfigSource::Runtime) override;
    bool has(const std::string& key) const override;
    void remove(const std::string& key) override;
    std::vector<std::string> getKeys() const override;
    void clear() override;
    std::string summary() const override;
    
    // Enhanced configuration features
    
    // Parameter management
    bool addParameter(const std::string& key, const EnhancedConfigValue& value, 
                     const ParameterMetadata& metadata = ParameterMetadata());
    bool updateParameter(const std::string& key, const EnhancedConfigValue& value, 
                        ConfigSource source = ConfigSource::Runtime, const std::string& reason = "manual");
    ParameterMetadata getParameterMetadata(const std::string& key) const;
    std::vector<ParameterMetadata> getAllParameterMetadata() const;
    
    // Parameter validation
    ParameterValidationResult validateParameter(const std::string& key, const EnhancedConfigValue& value) const;
    ParameterValidationResult validateAllParameters() const;
    bool validateParameterDependencies(const std::string& key) const;
    std::vector<std::string> getParameterValidationErrors() const;
    
    // Dependency management
    bool addParameterDependency(const ParameterDependency& dependency);
    bool removeParameterDependency(const std::string& dependentKey, const std::string& requiredKey);
    std::vector<ParameterDependency> getParameterDependencies(const std::string& key) const;
    std::vector<ParameterDependency> getAllDependencies() const;
    
    // Parameter tuning and optimization
    bool optimizeParameters(const std::string& algorithm = "bayesian_optimization", 
                           const std::unordered_map<std::string, EnhancedConfigValue>& constraints = {});
    std::vector<EnhancedConfigValue> generateParameterCombinations(const std::string& algorithm = "random_search") const;
    double evaluateParameterSet(const std::unordered_map<std::string, EnhancedConfigValue>& parameters) const;
    
    // Configuration presets
    bool createPreset(const std::string& name, const std::string& description = "");
    bool applyPreset(const std::string& name);
    bool deletePreset(const std::string& name);
    bool renamePreset(const std::string& oldName, const std::string& newName);
    std::vector<std::string> getPresetNames() const;
    std::string getPresetDescription(const std::string& name) const;
    std::unordered_map<std::string, EnhancedConfigValue> getPresetParameters(const std::string& name) const;
    
    // Configuration versioning
    bool saveVersion(const std::string& versionName = "auto", const std::string& comment = "");
    bool loadVersion(const std::string& versionName);
    std::vector<std::string> getAvailableVersions() const;
    std::vector<ConfigurationHistoryEntry> getConfigurationHistory() const;
    std::string getCurrentVersion() const;
    
    // Backup and restoration
    bool createBackup(const std::string& backupName = "auto");
    bool restoreBackup(const std::string& backupName);
    std::vector<std::string> getAvailableBackups() const;
    bool deleteBackup(const std::string& backupName);
    
    // Advanced features
    bool setNestedValue(const std::string& path, const EnhancedConfigValue& value);
    EnhancedConfigValue getNestedValue(const std::string& path) const;
    bool hasNestedValue(const std::string& path) const;
    std::vector<std::string> getNestedKeys(const std::string& pathPrefix) const;
    
    bool enableInheritance(const std::string& parentPath);
    void disableInheritance(const std::string& parentPath);
    bool isInheritanceEnabled(const std::string& path) const;
    
    bool loadMacros(const std::string& macroDirectory);
    EnhancedConfigValue evaluateExpression(const std::string& expression, 
                                          const std::unordered_map<std::string, EnhancedConfigValue>& variables = {}) const;
    
    // Performance and monitoring
    void enableRealTimeMonitoring(bool enabled);
    bool isMonitoringEnabled() const;
    double getParameterChangeRate(const std::string& key) const;
    std::chrono::system_clock::time_point getLastChangeTime(const std::string& key) const;
    
    // Configuration snapshots and comparison
    std::string createSnapshot() const;
    bool restoreSnapshot(const std::string& snapshot);
    std::string compareConfigurations(const EnhancedConfig& other) const;
    
    // Callbacks and notifications
    void addConfigChangeCallback(const ConfigChangeCallback& callback);
    void removeConfigChangeCallback(const ConfigChangeCallback& callback);
    void setChangeNotificationEnabled(bool enabled);
    bool isChangeNotificationEnabled() const;
    
    // Advanced configuration features
    bool enableAdvancedValidation(bool enabled);
    bool isAdvancedValidationEnabled() const;
    
    bool enableSensitivityAnalysis(bool enabled);
    bool isSensitivityAnalysisEnabled() const;
    std::unordered_map<std::string, double> analyzeParameterSensitivity() const;
    
    bool enableAutoGeneration(bool enabled);
    bool isAutoGenerationEnabled() const;
    std::unordered_map<std::string, EnhancedConfigValue> generateConfiguration(const std::string& strategy = "task_optimized") const;
    
    bool enableMigrationWizard(bool enabled);
    bool isMigrationWizardEnabled() const;
    std::string getMigrationReport() const;
    
    // Configuration validation with detailed error messages
    struct DetailedValidationResult {
        bool isValid;
        std::vector<std::string> criticalErrors;
        std::vector<std::string> warnings;
        std::vector<std::string> suggestions;
        std::unordered_map<std::string, std::vector<std::string>> parameterSpecificErrors;
        std::unordered_map<std::string, std::vector<std::string>> parameterSpecificWarnings;
        
        DetailedValidationResult() : isValid(true) {}
    };
    
    DetailedValidationResult validateWithDetails() const;
    std::string getValidationReport() const;
    
    // Performance tuning
    bool enablePerformanceTuning(bool enabled);
    bool isPerformanceTuningEnabled() const;
    void setPerformanceTarget(const std::string& target);
    std::string getPerformanceTarget() const;
    double getEstimatedPerformance() const;
    
    // Configuration expressions
    EnhancedConfigValue evaluateFormula(const std::string& formula,
                                       const std::unordered_map<std::string, EnhancedConfigValue>& variables) const;
    bool validateFormulaSyntax(const std::string& formula) const;
    
    // Configuration macros
    bool defineMacro(const std::string& name, const std::string& definition);
    EnhancedConfigValue expandMacro(const std::string& macroName,
                                   const std::unordered_map<std::string, EnhancedConfigValue>& arguments = {}) const;
    std::vector<std::string> getMacroNames() const;
    
    // Configuration export/import
    std::string exportConfiguration(const std::string& format = "json") const;
    bool importConfiguration(const std::string& data, const std::string& format = "json");
    
    // Configuration statistics
    size_t getParameterCount() const;
    size_t getChangeCount() const;
    double getConfigurationEntropy() const;
    std::unordered_map<std::string, double> getParameterStatistics() const;
    
    // Configuration security
    void enableEncryption(bool enabled, const std::string& encryptionKey = "");
    bool isEncryptionEnabled() const;
    
    // Configuration compression
    void enableCompression(bool enabled);
    bool isCompressionEnabled() const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Internal helper methods
    void notifyChange(const ParameterChangeNotification& notification);
    void validateDependencyChain() const;
    EnhancedConfigValue resolveInheritance(const std::string& path) const;
    bool validateNestedPath(const std::string& path) const;
    std::string normalizePath(const std::string& path) const;
    std::string formatValue(const EnhancedConfigValue& value) const;
    
    // Security and encryption
    std::string encryptConfiguration(const std::string& data) const;
    std::string decryptConfiguration(const std::string& data) const;
    
    // Compression
    std::string compressConfiguration(const std::string& data) const;
    std::string decompressConfiguration(const std::string& data) const;
};

// Template implementations for EnhancedConfig

template<typename T>
std::optional<T> EnhancedConfig::get(const std::string& key) const {
    auto baseValue = Config::get<T>(key);
    if (baseValue.has_value()) {
        return baseValue;
    }
    
    // Try to get from enhanced parameter storage
    // Implementation would need access to internal enhanced storage
    return std::nullopt;
}

template<typename T>
T EnhancedConfig::getOr(const std::string& key, const T& defaultValue) const {
    auto baseValue = Config::get<T>(key);
    if (baseValue.has_value()) {
        return baseValue.value();
    }
    
    // Try to get from enhanced parameter storage
    // Implementation would need access to internal enhanced storage
    return defaultValue;
}

// Parameter type conversion utilities
namespace ConfigTypeUtils {
    std::string enhancedValueToString(const EnhancedConfigValue& value);
    EnhancedConfigValue stringToEnhancedValue(const std::string& str, const std::string& typeHint = "");
    bool isValidType(const EnhancedConfigValue& value, const std::string& typeName);
    std::string getTypeName(const EnhancedConfigValue& value);
    bool isNumericType(const EnhancedConfigValue& value);
    bool isBooleanType(const EnhancedConfigValue& value);
    bool isStringType(const EnhancedConfigValue& value);
    bool isVectorType(const EnhancedConfigValue& value);
    bool isMapType(const EnhancedConfigValue& value);
}

// Enhanced parameter validation utilities
namespace ParameterValidation {
    ParameterValidationResult validateRange(const EnhancedConfigValue& value, 
                                           const EnhancedConfigValue& minValue,
                                           const EnhancedConfigValue& maxValue);
    ParameterValidationResult validateEnum(const EnhancedConfigValue& value,
                                          const std::vector<std::string>& allowedValues);
    ParameterValidationResult validateDependency(const EnhancedConfigValue& dependentValue,
                                                const EnhancedConfigValue& requiredValue,
                                                const std::string& dependencyType);
    ParameterValidationResult validateCustom(const EnhancedConfigValue& value,
                                           const std::unordered_map<std::string, std::string>& customRules);
}

} // namespace nlm