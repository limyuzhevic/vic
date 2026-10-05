#pragma once

#include "Config.hpp"
#include <string>
#include <memory>
#include <map>
#include <vector>
#include <functional>
#include <chrono>
#include <filesystem>
#include <fstream>
#include <limits>
#include <nlohmann/json.hpp>

namespace nlm {

// Configuration schema definition
struct ConfigSchema {
    std::string key;
    std::string type; // "int", "double", "string", "bool", "vector"
    std::string description;
    std::string defaultValue;
    bool required = false;
    std::vector<std::string> allowedValues;
    double minValue = std::numeric_limits<double>::lowest();
    double maxValue = std::numeric_limits<double>::max();
    int minSize = 0;
    int maxSize = INT_MAX;
    bool isArray = false;
    std::string validationRule; // Additional validation rule string
};

// Configuration template
struct ConfigTemplate {
    std::string name;
    std::string description;
    std::vector<ConfigSchema> schema;
    std::map<std::string, std::string> defaultValues;
    std::map<std::string, std::string> optimizationParameters;
};

// Configuration validation error
struct ValidationError {
    std::string key;
    std::string message;
    std::string errorCode;
};

// Dynamic configuration update callback
using ConfigUpdateCallback = std::function<void(const std::string& key, const ConfigValue& oldValue, const ConfigValue& newValue, const std::string& reason)>;

// Configuration comparison result
struct ComparisonResult {
    std::string key;
    ConfigValue oldValue;
    ConfigValue newValue;
    bool changed;
    double similarity;
};

// Optimization suggestion
struct OptimizationSuggestion {
    std::string key;
    ConfigValue currentValue;
    ConfigValue suggestedValue;
    std::string reason;
    double improvementFactor;
};

// Configuration preset
struct ConfigPreset {
    std::string name;
    std::string category; // "training", "simulation", "research", "development"
    std::string description;
    std::map<std::string, std::string> parameters;
    double performanceScore;
    std::vector<std::string> requiredFeatures;
};

class AdvancedConfigManager : public Config {
public:
    AdvancedConfigManager();
    ~AdvancedConfigManager();
    
    // Disable copying, enable moving
    AdvancedConfigManager(const AdvancedConfigManager&) = delete;
    AdvancedConfigManager& operator=(const AdvancedConfigManager&) = delete;
    AdvancedConfigManager(AdvancedConfigManager&&) noexcept;
    AdvancedConfigManager& operator=(AdvancedConfigManager&&) noexcept;
    
    // Schema and template management
    void addSchema(const ConfigSchema& schema);
    void removeSchema(const std::string& key);
    bool validateSchema(const std::string& key, const ConfigValue& value) const;
    std::vector<ValidationError> getValidationErrors() const;
    std::vector<std::string> getSchemaKeys() const;
    bool hasSchema(const std::string& key) const;
    
    // Template management
    void loadTemplate(const std::string& templateName);
    void saveTemplate(const std::string& templateName, const std::string& description);
    void saveCustomTemplate(const ConfigTemplate& templateData);
    bool hasTemplate(const std::string& templateName) const;
    std::vector<std::string> getTemplateNames() const;
    std::vector<ConfigTemplate> getAllTemplates() const;
    ConfigTemplate getTemplate(const std::string& templateName) const;
    
    // Advanced preset management
    void addPreset(const ConfigPreset& preset);
    void removePreset(const std::string& presetName);
    bool hasPreset(const std::string& presetName) const;
    std::vector<std::string> getPresetNames() const;
    std::vector<ConfigPreset> getPresetsByCategory(const std::string& category) const;
    ConfigPreset getPreset(const std::string& presetName) const;
    ConfigPreset applyPreset(const std::string& presetName);
    
    // Schema validation
    void validateAll() const;
    bool isValid() const;
    std::vector<ValidationError> validateWithDetails() const;
    std::string getValidationReport() const;
    
    // Enhanced validation rules
    bool validateValue(const ConfigSchema& schema, const ConfigValue& value, std::string& errorMessage) const;
    std::vector<ValidationError> validateAllRules() const;
    std::vector<std::string> getAllValidationRules() const;
    
    // Dynamic reloading
    bool reloadFromFile(const std::string& filepath);
    bool enableAutoReload(const std::string& filepath, std::chrono::milliseconds interval = std::chrono::seconds(5000));
    void disableAutoReload();
    bool isAutoReloadEnabled() const;
    
    // Enhanced configuration modification with validation
    void set(const std::string& key, const ConfigValue& value, ConfigSource source = ConfigSource::Runtime,
             bool validate = true, const std::string& reason = "", bool notify = true);
    void set(const std::string& key, const std::string& value, ConfigSource source = ConfigSource::Runtime,
             bool validate = true, const std::string& reason = "", bool notify = true);
    void set(const std::string& key, int value, ConfigSource source = ConfigSource::Runtime,
             bool validate = true, const std::string& reason = "", bool notify = true);
    void set(const std::string& key, int64_t value, ConfigSource source = ConfigSource::Runtime,
             bool validate = true, const std::string& reason = "", bool notify = true);
    void set(const std::string& key, double value, ConfigSource source = ConfigSource::Runtime,
             bool validate = true, const std::string& reason = "", bool notify = true);
    void set(const std::string& key, bool value, ConfigSource source = ConfigSource::Runtime,
             bool validate = true, const std::string& reason = "", bool notify = true);
    
    // Callbacks for configuration changes
    std::string addUpdateCallback(const ConfigUpdateCallback& callback);
    void removeUpdateCallback(const std::string& callbackId);
    void clearUpdateCallbacks();
    
    // Configuration export/import
    bool exportToJSON(const std::string& filepath, bool includeDefaults = false, bool prettyPrint = false) const;
    bool importFromJSON(const std::string& filepath);
    bool exportToYAML(const std::string& filepath, bool includeDefaults = false) const;
    bool importFromYAML(const std::string& filepath);
    bool importFromINI(const std::string& filepath);
    
    // Enhanced configuration comparison
    bool hasChanges() const;
    std::vector<ComparisonResult> getChanges() const;
    std::map<std::string, std::pair<ConfigValue, ConfigValue>> getChangesMap() const;
    void resetChangeTracking();
    double calculateSimilarity(const AdvancedConfigManager& other) const;
    
    // Configuration statistics
    size_t getEntryCount() const;
    size_t getValidEntryCount() const;
    size_t getInvalidEntryCount() const;
    std::chrono::system_clock::time_point getLastModified() const;
    std::vector<std::string> getModifiedKeys() const;
    
    // Configuration optimization
    std::vector<OptimizationSuggestion> optimize(const std::string& useCase = "general") const;
    ConfigPreset generateOptimizedPreset(const std::string& useCase, const std::map<std::string, double>& weights = {}) const;
    std::map<std::string, double> analyzePerformance(const ConfigPreset& preset) const;
    std::vector<OptimizationSuggestion> suggestImprovements(const std::string& constraint = "performance") const;
    
    // Configuration merging
    void merge(const AdvancedConfigManager& other, ConfigSource source = ConfigSource::Runtime);
    void merge(const std::map<std::string, ConfigValue>& configMap, ConfigSource source = ConfigSource::Runtime);
    std::unique_ptr<AdvancedConfigManager> createSubset(const std::vector<std::string>& keys) const;
    std::unique_ptr<AdvancedConfigManager> createDifference(const AdvancedConfigManager& other) const;
    
    // Configuration documentation generation
    std::string generateDocumentation(const std::string& format = "markdown") const;
    std::string generateSchemaDocumentation() const;
    std::string generateUsageExamples() const;
    std::string generatePresetDocumentation() const;
    
    // Configuration analysis
    std::map<std::string, double> calculateParameterStatistics() const;
    std::vector<std::string> getRecommendedConfiguration(const std::string& taskType) const;
    bool meetsRequirements(const std::map<std::string, double>& requirements) const;
    
    // Configuration backup and restore
    bool backup(const std::string& backupPath) const;
    bool restore(const std::string& backupPath);
    std::string createConfigurationHash() const;
    
    // Configuration analysis and insights
    std::map<std::string, std::string> detectPatterns() const;
    std::vector<std::string> getCorrelatedParameters() const;
    double estimatePerformance(const std::string& useCase) const;
    
protected:
    // Internal helpers
    bool validateValue(const ConfigSchema& schema, const ConfigValue& value) const;
    std::string configValueToString(const ConfigValue& value) const;
    std::string getConfigValueDescription(const std::string& key) const;
    
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm
