#pragma once

#include <string>
#include <memory>
#include <vector>
#include <map>
#include <set>
#include <variant>
#include <optional>
#include <stdexcept>
#include <any>
#include <functional>
#include <filesystem>
#include <algorithm>
#include <chrono>
#include <sstream>
#include "core/Config/Config.hpp"

namespace nlm {

// Configuration validation and management system

// Forward declarations
class ConfigValidator;
class ConfigManager;
class ConfigTemplate;
class ConfigHistory;

// Validation result structure
struct ValidationResult {
    bool valid = true;
    std::vector<std::string> errors;
    std::vector<std::string> warnings;
    std::map<std::string, std::vector<std::string>> parameterErrors;
    
    void addError(const std::string& error) {
        valid = false;
        errors.push_back(error);
    }
    
    void addWarning(const std::string& warning) {
        warnings.push_back(warning);
    }
    
    void addParameterError(const std::string& param, const std::string& error) {
        valid = false;
        parameterErrors[param].push_back(error);
    }
    
    std::string getSummary() const {
        std::ostringstream oss;
        if (valid) {
            oss << "Configuration is valid";
        } else {
            oss << "Configuration is invalid with " << errors.size() << " errors";
            if (!parameterErrors.empty()) {
                oss << " (" << parameterErrors.size() << " parameter-specific errors)";
            }
        }
        if (!warnings.empty()) {
            oss << " (with " << warnings.size() << " warnings)";
        }
        return oss.str();
    }
};

// Configuration schema definition
struct ConfigSchema {
    std::string name;
    std::string description;
    std::map<std::string, std::any> defaultValues;
    std::map<std::string, std::function<bool(const ConfigValue&, const Config&)>> validators;
    std::map<std::string, std::string> dependencies; // parameter -> required parameter
    std::map<std::string, std::pair<double, double>> ranges; // parameter -> (min, max)
    std::map<std::string, std::set<std::string>> enumValues; // parameter -> allowed values
    bool required = false;
    std::string format; // JSON, XML, etc.
    
    ConfigSchema(const std::string& n = "", const std::string& desc = "")
        : name(n), description(desc) {}
};

// Configuration template
class ConfigTemplate {
public:
    ConfigTemplate() = default;
    ConfigTemplate(const std::string& name, const std::string& description);
    
    // Template management
    void setName(const std::string& name);
    void setDescription(const std::string& description);
    void addParameter(const std::string& name, const ConfigValue& defaultValue,
                     const std::string& description, bool required = false);
    void addValidationRule(const std::string& parameter, 
                          std::function<bool(const ConfigValue&, const Config&)> validator);
    void addDependency(const std::string& parameter, const std::string& dependsOn);
    
    // Template operations
    std::shared_ptr<Config> createConfig() const;
    bool validate(const Config& config) const;
    ValidationResult validateWithDetails(const Config& config) const;
    std::shared_ptr<Config> applyTemplate(const Config& baseConfig) const;
    
    // Template properties
    std::string getName() const { return name; }
    std::string getDescription() const { return description; }
    std::vector<std::string> getParameters() const;
    std::map<std::string, ConfigValue> getDefaultValues() const;
    
    // Template serialization
    bool saveToFile(const std::string& filepath) const;
    bool loadFromFile(const std::string& filepath);
    
    // Built-in template factories
    static std::shared_ptr<ConfigTemplate> createPhase2Template();
    static std::shared_ptr<ConfigTemplate> createPhase4Template();
    static std::shared_ptr<ConfigTemplate> createPhase6Template();
    static std::shared_ptr<ConfigTemplate> createCustomTemplate(const std::string& name);
    
private:
    std::string name;
    std::string description;
    std::map<std::string, std::pair<ConfigValue, std::string>> parameters; // name -> (default, description)
    std::map<std::string, std::vector<std::function<bool(const ConfigValue&, const Config&)>>> validationRules;
    std::map<std::string, std::string> dependencies;
    std::map<std::string, std::set<std::string>> enumValues;
    std::map<std::string, std::pair<double, double>> ranges;
};

// Configuration history entry
struct ConfigHistoryEntry {
    std::string id;
    std::string timestamp;
    std::string action; // "create", "update", "delete", "restore", "merge"
    std::shared_ptr<Config> configSnapshot;
    std::string user;
    std::string description;
    std::map<std::string, std::string> changes; // parameter -> old->new value
    
    ConfigHistoryEntry() = default;
    ConfigHistoryEntry(const std::string& aid, const std::string& atimestamp,
                      const std::string& aaction, std::shared_ptr<Config> aconfig,
                      const std::string& auser, const std::string& adescription);
};

// Configuration history manager
class ConfigHistory {
public:
    ConfigHistory();
    ~ConfigHistory();
    
    // History management
    std::string recordChange(const std::string& action, const std::shared_ptr<Config>& config,
                           const std::string& user = "", const std::string& description = "");
    std::vector<std::shared_ptr<ConfigHistoryEntry>> getHistory() const;
    std::optional<std::shared_ptr<ConfigHistoryEntry>> getEntry(const std::string& id) const;
    std::vector<std::shared_ptr<ConfigHistoryEntry>> getRecentEntries(size_t count = 10) const;
    
    // History operations
    bool restoreFromHistory(const std::string& id, std::shared_ptr<Config>& restoredConfig);
    bool rollback(const std::string& id, std::shared_ptr<Config>& rolledBackConfig);
    bool deleteEntry(const std::string& id);
    bool clearHistory();
    
    // History filtering
    std::vector<std::shared_ptr<ConfigHistoryEntry>> getHistoryByAction(const std::string& action) const;
    std::vector<std::shared_ptr<ConfigHistoryEntry>> getHistoryByUser(const std::string& user) const;
    std::vector<std::shared_ptr<ConfigHistoryEntry>> getHistoryByTimeRange(
        const std::string& startTime, const std::string& endTime) const;
    
    // History export/import
    bool exportHistory(const std::string& filepath) const;
    bool importHistory(const std::string& filepath);
    
    // Version control integration (placeholder for future implementation)
    bool commitToVersionControl(const std::string& message);
    bool restoreFromVersionControl(const std::string& version);
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Configuration validator
class ConfigValidator {
public:
    ConfigValidator();
    ~ConfigValidator();
    
    // Validation setup
    void addSchema(const ConfigSchema& schema);
    void setParameterValidator(const std::string& parameter,
                               std::function<bool(const ConfigValue&, const Config&)> validator);
    void setRangeValidator(const std::string& parameter, double min, double max);
    void setEnumValidator(const std::string& parameter, const std::set<std::string>& values);
    void setDependencyValidator(const std::string& parameter, const std::string& dependsOn);
    
    // Validation execution
    ValidationResult validate(const Config& config) const;
    bool isValid(const Config& config) const;
    ValidationResult validateAgainstSchema(const Config& config, const ConfigSchema& schema) const;
    
    // Parameter validation
    ValidationResult validateParameter(const std::string& parameter, const ConfigValue& value,
                                       const Config& config) const;
    ValidationResult validateAllParameters(const Config& config) const;
    
    // Special validation rules
    ValidationResult validateBrainParameters(const Config& config) const;
    ValidationResult validatePlasticityParameters(const Config& config) const;
    ValidationResult validateNeuromodulationParameters(const Config& config) const;
    ValidationResult validateDevelopmentParameters(const Config& config) const;
    ValidationResult validateMemoryParameters(const Config& config) const;
    ValidationResult validatePredictionParameters(const Config& config) const;
    
    // Configuration comparison
    bool compareConfigurations(const Config& config1, const Config& config2) const;
    std::map<std::string, std::pair<std::string, std::string>> getDifferences(
        const Config& config1, const Config& config2) const;
    
    // Configuration normalization
    std::shared_ptr<Config> normalizeConfig(const Config& config) const;
    std::shared_ptr<Config> applyTemplate(const Config& config, const ConfigTemplate& template) const;
    
    // Error management
    std::vector<std::string> getErrors() const { return currentErrors; }
    void clearErrors() { currentErrors.clear(); }
    
    // Built-in schema creation
    static ConfigSchema createBrainSchema();
    static ConfigSchema createPlasticitySchema();
    static ConfigSchema createNeuromodulationSchema();
    static ConfigSchema createDevelopmentSchema();
    static ConfigSchema createMemorySchema();
    static ConfigSchema createPredictionSchema();
    
    // Environment variable substitution (placeholder)
    std::shared_ptr<Config> resolveEnvironmentVariables(const Config& config) const;
    
    // File inclusion and macro support (placeholder)
    std::shared_ptr<Config> resolveIncludes(const Config& config) const;
    
    // Conditional configuration (placeholder)
    std::shared_ptr<Config> evaluateConditions(const Config& config) const;
    
    // Override management (placeholder)
    std::shared_ptr<Config> applyOverrides(const Config& config, const Config& overrides) const;
    
    // Hot configuration reload (placeholder)
    bool reloadConfiguration(const std::string& filepath, std::shared_ptr<Config>& config);
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    std::vector<std::string> currentErrors;
    
    void collectValidationErrors(const ValidationResult& result);
    bool validateParameterType(const ConfigValue& value, const std::string& expectedType) const;
    bool validateParameterRange(const ConfigValue& value, double min, double max) const;
    bool validateParameterEnum(const ConfigValue& value, const std::set<std::string>& allowed) const;
    bool validateParameterDependency(const std::string& parameter, const ConfigValue& value,
                                    const Config& config) const;
};

// Configuration manager
class ConfigManager {
public:
    ConfigManager();
    ~ConfigManager();
    
    // Configuration management commands
    bool createConfig(const std::string& name, const std::string& description = "",
                     const std::string& templateName = "");
    bool createConfigFromTemplate(const std::string& name, const std::string& templateName);
    bool validateConfig(const std::string& configName, const std::shared_ptr<Config>& config);
    bool compareConfigs(const std::string& config1, const std::string& config2,
                       const std::string& outputFile = "");
    bool mergeConfigs(const std::string& config1, const std::string& config2,
                     const std::string& outputFile, bool overwrite = false);
    bool exportConfig(const std::string& configName, const std::string& outputFile,
                     const std::string& format = "json");
    bool importConfig(const std::string& inputFile, const std::string& format = "json",
                     const std::string& name = "");
    bool listConfigs(const std::string& pattern = "*") const;
    bool deleteConfig(const std::string& name, bool force = false);
    bool backupConfig(const std::string& name, const std::string& outputDir = "backups");
    bool restoreConfig(const std::string& name, const std::string& backupFile = "",
                      bool overwrite = false);
    
    // Configuration operations
    bool saveConfig(const std::string& name, const std::shared_ptr<Config>& config);
    bool loadConfig(const std::string& name, std::shared_ptr<Config>& config);
    bool updateConfig(const std::string& name, const std::map<std::string, ConfigValue>& updates);
    bool getConfigNames(std::vector<std::string>& names) const;
    bool getConfigMetadata(const std::string& name, std::map<std::string, std::string>& metadata) const;
    
    // Configuration templates
    bool createTemplate(const std::string& name, const ConfigTemplate& template);
    bool loadTemplate(const std::string& name, ConfigTemplate& template);
    bool listTemplates() const;
    bool deleteTemplate(const std::string& name);
    
    // Configuration history
    bool enableHistory(bool enable = true);
    bool addToHistory(const std::shared_ptr<Config>& config, const std::string& action,
                    const std::string& user = "", const std::string& description = "");
    bool getHistoryEntries(std::vector<std::shared_ptr<ConfigHistoryEntry>>& entries) const;
    bool restoreFromHistory(const std::string& entryId, std::shared_ptr<Config>& config);
    
    // Advanced features
    bool enableEnvironmentVariableSubstitution(bool enable = true);
    bool enableFileInclusion(bool enable = true);
    bool enableMacroSupport(bool enable = true);
    bool enableConditionalConfiguration(bool enable = true);
    bool enableHotReload(bool enable = true);
    
    // Configuration filtering and search
    bool searchConfigs(const std::string& pattern, std::vector<std::string>& results) const;
    bool filterConfigsByTag(const std::string& tag, std::vector<std::string>& results) const;
    bool getConfigTags(const std::string& configName, std::vector<std::string>& tags) const;
    bool addConfigTag(const std::string& configName, const std::string& tag);
    
    // Configuration validation and analysis
    ValidationResult validateConfig(const std::shared_ptr<Config>& config) const;
    std::shared_ptr<Config> normalizeConfig(const std::shared_ptr<Config>& config) const;
    std::map<std::string, double> analyzeConfigComplexity(const std::shared_ptr<Config>& config) const;
    
    // Configuration comparison
    std::map<std::string, std::pair<std::string, std::string>> compareConfigs(
        const std::shared_ptr<Config>& config1, const std::shared_ptr<Config>& config2) const;
    
    // File format conversion
    bool convertConfigFormat(const std::string& inputFile, const std::string& outputFile,
                           const std::string& sourceFormat, const std::string& targetFormat);
    
    // Batch operations
    bool batchProcessConfigs(const std::vector<std::string>& configNames,
                            const std::string& operation, const std::map<std::string, std::any>& options);
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    std::shared_ptr<Config> getOrCreateConfig(const std::string& name);
    std::shared_ptr<ConfigTemplate> getOrCreateTemplate(const std::string& name);
    std::string generateConfigPath(const std::string& name) const;
    std::string generateTemplatePath(const std::string& name) const;
    std::string generateBackupPath(const std::string& name, const std::string& timestamp) const;
};

// Global configuration management functions
extern std::unique_ptr<ConfigValidator> createDefaultConfigValidator();
extern std::unique_ptr<ConfigManager> createDefaultConfigManager();
extern std::unique_ptr<ConfigTemplate> createConfigTemplate(const std::string& name);
extern std::unique_ptr<ConfigHistory> createConfigHistory();

// Convenience functions
std::shared_ptr<Config> loadConfig(const std::string& filepath);
bool saveConfig(const std::string& filepath, const std::shared_ptr<Config>& config);
ValidationResult validateConfig(const std::shared_ptr<Config>& config);
std::shared_ptr<Config> createConfigFromTemplate(const std::string& templateName);
std::map<std::string, std::pair<std::string, std::string>> compareConfigs(
    const std::string& config1, const std::string& config2);

} // namespace nlm
