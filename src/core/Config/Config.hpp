#pragma once

#include <string>
#include <memory>
#include <vector>
#include <variant>
#include <optional>
#include <functional>
#include <map>
#include <set>
#include <iostream>

namespace nlm {

// Enhanced configuration parser with advanced features
class AdvancedConfigParser {
public:
    enum class Format {
        Simple,
        JSON,
        YAML,
        Auto
    };
    
    AdvancedConfigParser();
    ~AdvancedConfigParser();
    
    // Parse configuration from string
    std::vector<ConfigEntry> parseFromString(const std::string& content, 
                                            Format format = Format::Auto);
    
    // Parse configuration from file with error handling
    bool parseFromFile(const std::string& filepath, std::vector<ConfigEntry>& entries,
                      Format format = Format::Auto);
    
    // Convert ConfigEntry to string
    std::string toString(const ConfigEntry& entry) const;
    
    // Enhanced string conversion with type support
    std::string valueToString(const ConfigValue& value) const;
    std::string keyToEnvVar(const std::string& key) const;
    
    // Configuration file format detection
    Format detectFormat(const std::string& content) const;
    
    // Validate configuration entry
    bool validateEntry(const ConfigEntry& entry, const ConfigValidator& validator) const;
    
    // Configuration backup and restore
    bool createBackup(const std::string& filepath, const std::vector<ConfigEntry>& entries);
    std::vector<ConfigEntry> restoreBackup(const std::string& filepath);
    
    // Configuration comparison
    std::vector<std::string> compareConfigs(const std::vector<ConfigEntry>& config1,
                                           const std::vector<ConfigEntry>& config2) const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Helper functions
    static std::string trim(const std::string& str);
    static bool isEmpty(const std::string& str);
    static std::string removeComments(const std::string& content);
    static std::string escapeString(const std::string& str);
    static std::string unescapeString(const std::string& str);
};

// Enhanced configuration validator with comprehensive validation rules
class ConfigValidator {
public:
    ConfigValidator();
    ~ConfigValidator();
    
    // Add validation rule with custom error message
    void addValidationRule(const std::string& key,
                          std::function<bool(const ConfigValue&)> validator,
                          const std::string& errorMessage = "");
    
    // Add type validation
    void addTypeValidation(const std::string& key,
                          const std::string& expectedType);
    
    // Add range validation
    void addRangeValidation(const std::string& key,
                           double min, double max,
                           const std::string& errorMessage = "");
    
    // Add enum validation
    void addEnumValidation(const std::string& key,
                          const std::vector<std::string>& allowedValues,
                          const std::string& errorMessage = "");
    
    // Add conditional validation
    void addConditionalValidation(const std::string& conditionKey,
                                 const ConfigValue& conditionValue,
                                 const std::string& dependentKey,
                                 std::function<bool(const ConfigValue&)> validator,
                                 const std::string& errorMessage = "");
    
    // Validate configuration with detailed error reporting
    struct ValidationResult {
        bool valid;
        std::vector<std::string> errors;
        std::vector<std::string> warnings;
        
        ValidationResult() : valid(true) {}
        
        void addError(const std::string& error) {
            valid = false;
            errors.push_back(error);
        }
        
        void addWarning(const std::string& warning) {
            warnings.push_back(warning);
        }
    };
    
    ValidationResult validate(const std::vector<ConfigEntry>& entries) const;
    
    // Get validation rules for a specific key
    std::vector<std::pair<std::function<bool(const ConfigValue&)>, std::string>> 
    getValidationRules(const std::string& key) const;
    
    // Clear all validation rules
    void clearRules();
    
    // Get validation statistics
    size_t getRuleCount() const { return validationRules_.size(); }
    
private:
    struct ValidationRule {
        std::string key;
        std::function<bool(const ConfigValue&)> validator;
        std::string errorMessage;
        std::function<bool(const ConfigValue&)> condition;
    };
    
    std::vector<ValidationRule> validationRules_;
};

// Configuration preset manager for common experiment configurations
class ConfigPresetManager {
public:
    ConfigPresetManager();
    ~ConfigPresetManager();
    
    // Add configuration preset
    void addPreset(const std::string& name,
                  const std::vector<ConfigEntry>& config,
                  const std::string& description = "",
                  const std::string& category = "general");
    
    // Get configuration preset
    std::vector<ConfigEntry> getPreset(const std::string& name) const;
    
    // Remove configuration preset
    bool removePreset(const std::string& name);
    
    // List all available presets
    std::vector<std::string> listPresets() const;
    
    // Get preset description
    std::string getPresetDescription(const std::string& name) const;
    
    // Get preset category
    std::string getPresetCategory(const std::string& name) const;
    
    // Apply preset with parameter substitution
    std::vector<ConfigEntry> applyPreset(const std::string& name,
                                        const std::unordered_map<std::string, ConfigValue>& parameters = {});
    
    // Get preset parameters
    std::vector<std::string> getPresetParameters(const std::string& name) const;
    
    // Add preset category
    void addCategory(const std::string& category, const std::string& description = "");
    
    // Get all categories
    std::vector<std::string> listCategories() const;
    
    // Get category description
    std::string getCategoryDescription(const std::string& category) const;
    
private:
    struct Preset {
        std::vector<ConfigEntry> config;
        std::string description;
        std::string category;
        std::vector<std::string> parameters;
    };
    
    struct Category {
        std::string description;
        std::vector<std::string> presetNames;
    };
    
    std::unordered_map<std::string, Preset> presets_;
    std::unordered_map<std::string, Category> categories_;
};

// Configuration comparison utility
class ConfigCompare {
public:
    ConfigCompare();
    ~ConfigCompare();
    
    // Compare two configurations
    struct ComparisonResult {
        std::string key;
        enum ComparisonType { Added, Modified, Removed, Same } type;
        std::string oldValue;
        std::string newValue;
        std::string description;
    };
    
    std::vector<ComparisonResult> compare(const std::vector<ConfigEntry>& config1,
                                         const std::vector<ConfigEntry>& config2) const;
    
    // Generate diff report
    std::string generateDiffReport(const std::vector<ComparisonResult>& results) const;
    
    // Check if configurations are equal
    bool areEqual(const std::vector<ConfigEntry>& config1,
                 const std::vector<ConfigEntry>& config2) const;
    
    // Get common entries
    std::vector<ConfigEntry> getCommonEntries(const std::vector<ConfigEntry>& config1,
                                            const std::vector<ConfigEntry>& config2) const;
    
    // Get unique entries
    std::vector<ConfigEntry> getUniqueEntries(const std::vector<ConfigEntry>& config1,
                                            const std::vector<ConfigEntry>& config2) const;
    
    // Get only-in-first configuration
    std::vector<ConfigEntry> getOnlyInConfig1(const std::vector<ConfigEntry>& config1,
                                            const std::vector<ConfigEntry>& config2) const;
    
    // Get only-in-second configuration
    std::vector<ConfigEntry> getOnlyInConfig2(const std::vector<ConfigEntry>& config1,
                                            const std::vector<ConfigEntry>& config2) const;
    
private:
    enum ComparisonType { Added, Modified, Removed, Same };
};

// Enhanced command-line argument parser with advanced features
class AdvancedArgumentParser {
public:
    AdvancedArgumentParser();
    ~AdvancedArgumentParser();
    
    // Option types
    enum OptionType { Flag, Value, Positional };
    
    // Option structure
    struct Option {
        std::string longName;
        char shortName;
        std::string description;
        OptionType type;
        bool required;
        std::string defaultValue;
        std::string value;
        bool specified;
        std::function<bool(const std::string&)> validator;
    };
    
    // Add option
    void addOption(const std::string& longName, char shortName = '\0',
                  const std::string& description = "",
                  OptionType type = OptionType::Value,
                  bool required = false,
                  const std::string& defaultValue = "",
                  std::function<bool(const std::string&)> validator = nullptr);
    
    // Add flag
    void addFlag(const std::string& longName, char shortName = '\0',
                const std::string& description = "",
                bool defaultValue = false);
    
    // Add positional argument
    void addPositional(const std::string& name,
                      const std::string& description = "",
                      bool required = false,
                      std::function<bool(const std::string&)> validator = nullptr);
    
    // Add mutually exclusive group
    void addMutuallyExclusiveGroup(const std::vector<std::string>& optionNames);
    
    // Parse arguments
    bool parse(int argc, char** argv);
    
    // Get option value
    std::string getOptionValue(const std::string& optionName) const;
    bool getFlagValue(const std::string& optionName) const;
    std::string getPositionalValue(size_t index) const;
    
    // Check if option was specified
    bool hasOption(const std::string& optionName) const;
    bool hasFlag(const std::string& optionName) const;
    bool hasPositional(const std::string& name) const;
    
    // Get help text
    std::string getHelpText() const;
    
    // Get parsing errors
    std::vector<std::string> getErrors() const { return errors_; }
    bool hasErrors() const { return !errors_.empty(); }
    bool isSuccessful() const { return !hasErrors(); }
    
    // Get all option values
    std::unordered_map<std::string, std::string> getAllOptionValues() const;
    
    // Get option by name
    const Option* getOption(const std::string& optionName) const;
    
private:
    std::vector<Option> options_;
    std::vector<std::string> positionalArgs_;
    std::vector<std::string> errors_;
    
    struct MutuallyExclusiveGroup {
        std::vector<std::string> optionNames;
        std::vector<std::string> selectedOptions;
    };
    
    std::vector<MutuallyExclusiveGroup> mutuallyExclusiveGroups_;
    
    void parseOption(Option& option, char** argv, int& i, int argc);
    void parsePositionalArguments(const std::vector<std::string>& args);
    void validateRequiredOptions() const;
    void handleMutuallyExclusiveGroups() const;
    std::string generateHelpText() const;
};

// Environment variable configuration handler
class EnvVarConfig {
public:
    EnvVarConfig();
    ~EnvVarConfig();
    
    // Enable/disable environment variable override
    void setEnvOverrideEnabled(bool enabled) { envOverrideEnabled_ = enabled; }
    bool isEnvOverrideEnabled() const { return envOverrideEnabled_; }
    
    // Set environment variable prefix
    void setEnvPrefix(const std::string& prefix) { envPrefix_ = prefix; }
    std::string getEnvPrefix() const { return envPrefix_; }
    
    // Override configuration with environment variables
    std::vector<ConfigEntry> overrideWithEnvVars(const std::vector<ConfigEntry>& baseConfig) const;
    
    // Get environment variable for a configuration key
    std::string getEnvVarForKey(const std::string& key) const;
    
    // Set custom environment variable mapping
    void addEnvVarMapping(const std::string& configKey, const std::string& envVarName);
    
private:
    bool envOverrideEnabled_;
    std::string envPrefix_;
    std::unordered_map<std::string, std::string> envVarMappings_;
    
    // Convert configuration key to environment variable name
    std::string configKeyToEnvVar(const std::string& key) const;
};

// Configuration backup and restore handler
class ConfigBackup {
public:
    ConfigBackup();
    ~ConfigBackup();
    
    // Set backup directory
    void setBackupDirectory(const std::string& directory) { backupDir_ = directory; }
    std::string getBackupDirectory() const { return backupDir_; }
    
    // Create backup of configuration
    bool createBackup(const std::vector<ConfigEntry>& config, const std::string& name = "");
    
    // Restore configuration from backup
    std::vector<ConfigEntry> restoreBackup(const std::string& name = "latest");
    
    // List available backups
    std::vector<std::string> listBackups() const;
    
    // Delete backup
    bool deleteBackup(const std::string& name);
    
    // Set backup retention policy
    void setBackupRetention(size_t maxBackups) { maxBackups_ = maxBackups; }
    size_t getBackupRetention() const { return maxBackups_; }
    
private:
    std::string backupDir_;
    size_t maxBackups_;
    
    // Generate backup filename
    std::string generateBackupFilename(const std::string& name) const;
    
    // Get backup timestamp
    std::string getTimestamp() const;
};

// Configuration history tracker
class ConfigHistory {
public:
    ConfigHistory();
    ~ConfigHistory();
    
    // Add configuration to history
    void addToHistory(const std::vector<ConfigEntry>& config, const std::string& comment = "");
    
    // Get configuration from history by index
    std::vector<ConfigEntry> getFromHistory(size_t index) const;
    
    // Get configuration from history by timestamp
    std::vector<ConfigEntry> getFromHistory(const std::string& timestamp) const;
    
    // List all historical configurations
    std::vector<std::pair<std::string, std::string>> listHistory() const;
    
    // Remove configuration from history
    bool removeFromHistory(size_t index);
    
    // Clear history
    void clearHistory();
    
    // Set maximum history entries
    void setMaxHistory(size_t maxEntries) { maxHistoryEntries_ = maxEntries; }
    size_t getMaxHistory() const { return maxHistoryEntries_; }
    
private:
    struct HistoryEntry {
        std::string timestamp;
        std::vector<ConfigEntry> config;
        std::string comment;
    };
    
    std::vector<HistoryEntry> history_;
    size_t maxHistoryEntries_;
    
    // Generate timestamp
    std::string generateTimestamp() const;
};

// Main enhanced configuration class that integrates all components
class EnhancedConfig {
public:
    EnhancedConfig();
    ~EnhancedConfig();
    
    // Load configuration with all enhancements
    bool load(const std::string& filepath, 
              const std::string& configFormat = "auto",
              bool inherit = true,
              bool validate = true,
              bool overrideWithEnv = true);
    
    // Save configuration with enhanced options
    bool save(const std::string& filepath, 
              const std::string& outputFormat = "simple") const;
    
    // Validate configuration
    bool validate() const;
    
    // Get configuration entries
    const std::vector<ConfigEntry>& getEntries() const { return entries_; }
    
    // Get configuration entry by key
    std::optional<ConfigEntry> getEntry(const std::string& key) const;
    
    // Set configuration entry
    void setEntry(const std::string& key, const ConfigValue& value, 
                  const std::string& description = "",
                  ConfigSource source = ConfigSource::Runtime);
    
    // Remove configuration entry
    bool removeEntry(const std::string& key);
    
    // Merge with another configuration
    bool mergeWith(const EnhancedConfig& other, MergeStrategy strategy = MergeStrategy::Override);
    
    // Compare with another configuration
    std::vector<std::string> compareWith(const EnhancedConfig& other) const;
    
    // Get configuration summary
    std::string getSummary() const;
    
    // Export configuration
    std::string exportTo(const std::string& format, 
                       const std::vector<std::string>& keys = {}) const;
    
    // Import configuration
    bool importFrom(const std::string& format, const std::string& content);
    
    // Configuration management operations
    bool createBackup(const std::string& name = "");
    std::vector<std::string> listBackups() const;
    bool restoreBackup(const std::string& name = "latest");
    
    // History operations
    void addToHistory(const std::string& comment = "");
    std::vector<std::pair<std::string, std::string>> listHistory() const;
    bool restoreFromHistory(size_t index);
    
    // Template operations
    bool addTemplate(const std::string& name, const std::string& configDescription = "");
    bool removeTemplate(const std::string& name);
    std::vector<std::string> listTemplates() const;
    bool applyTemplate(const std::string& name,
                      const std::unordered_map<std::string, std::string>& parameters = {});
    
    // Environment variable operations
    void setEnvOverrideEnabled(bool enabled);
    bool isEnvOverrideEnabled() const;
    void setEnvPrefix(const std::string& prefix);
    std::string getEnvPrefix() const;
    
    // Validation operations
    void addValidationRule(const std::string& key,
                          std::function<bool(const ConfigValue&)> validator,
                          const std::string& errorMessage = "");
    void addRangeValidation(const std::string& key, double min, double max,
                           const std::string& errorMessage = "");
    void addEnumValidation(const std::string& key,
                          const std::vector<std::string>& allowedValues,
                          const std::string& errorMessage = "");
    
    // Get validation errors
    std::vector<std::string> getValidationErrors() const;
    bool isValid() const { return validate(); }
    
private:
    std::vector<ConfigEntry> entries_;
    AdvancedConfigParser parser_;
    ConfigValidator validator_;
    EnvVarConfig envVarConfig_;
    ConfigBackup backupManager_;
    ConfigHistory historyManager_;
    ConfigPresetManager presetManager_;
    ConfigCompare compareUtility_;
    
    // Helper functions
    std::vector<ConfigEntry> applyAllEnhancements(const std::string& filepath) const;
    void updateValidationResult();
    
    friend class EnhancedConfigImpl;
};

// Enhanced application class with advanced features
class EnhancedNLMApplication {
public:
    EnhancedNLMApplication();
    ~EnhancedNLMApplication();
    
    // Initialize application with enhanced features
    bool initialize(int argc, char** argv);
    
    // Run application with advanced features
    int run();
    
    // Shutdown application
    void shutdown();
    
    // Get enhanced configuration
    EnhancedConfig& getConfig() { return config_; }
    
    // Get enhanced argument parser
    AdvancedArgumentParser& getArgumentParser() { return argParser_; }
    
    // Get configuration preset manager
    ConfigPresetManager& getPresetManager() { return presetManager_; }
    
private:
    EnhancedConfig config_;
    AdvancedArgumentParser argParser_;
    ConfigPresetManager presetManager_;
    
    // Enhanced initialization and execution
    void initializeEnhancedArgumentParser();
    void initializePresetManager();
    bool loadEnhancedConfiguration();
    bool saveEnhancedConfiguration();
    bool validateEnhancedConfiguration();
    void handleEnhancedCommandLineArguments();
    void handleEnhancedConfigurationOperations();
    void handleEnhancedPresetOperations();
    void handleEnhancedBackupOperations();
    void handleEnhancedHistoryOperations();
    void handleEnhancedValidation();
    int runEnhancedApplication();
};

// Global functions for enhanced configuration
EnhancedConfig& getGlobalEnhancedConfig();
AdvancedArgumentParser& getGlobalEnhancedArgumentParser();
ConfigPresetManager& getGlobalPresetManager();

// Enhanced application management
EnhancedNLMApplication& getEnhancedApplication();
void initializeEnhancedApplication(int argc, char** argv);
bool isEnhancedApplicationInitialized();
void runEnhancedApplication();
void shutdownEnhancedApplication();

// Enhanced configuration file operations
bool loadEnhancedConfigFile(const std::string& filepath,
                          const std::string& format = "auto",
                          bool inherit = true,
                          bool validate = true,
                          bool overrideWithEnv = true);
bool saveEnhancedConfigFile(const std::string& filepath,
                          const std::string& format = "simple");
bool validateEnhancedConfigFile(const std::string& filepath);
std::vector<std::string> compareEnhancedConfigFiles(const std::string& filepath1,
                                                  const std::string& filepath2);

// Enhanced configuration management
const std::vector<ConfigEntry>& getEnhancedConfigEntries();
ConfigEntry getEnhancedConfigEntry(const std::string& key);
bool hasEnhancedConfigEntry(const std::string& key);
void setEnhancedConfigEntry(const std::string& key, const ConfigValue& value,
                           const std::string& description = "",
                           ConfigSource source = ConfigSource::Runtime);
void removeEnhancedConfigEntry(const std::string& key);
std::vector<std::string> getEnhancedConfigKeys();
std::string getEnhancedConfigSummary();

// Enhanced configuration backup and history
bool createEnhancedConfigBackup(const std::string& name = "");
std::vector<std::string> listEnhancedConfigBackups();
bool restoreEnhancedConfigBackup(const std::string& name = "latest");
void addEnhancedConfigToHistory(const std::string& comment = "");
std::vector<std::pair<std::string, std::string>> listEnhancedConfigHistory();
bool restoreEnhancedConfigFromHistory(size_t index);

// Enhanced configuration presets
bool addEnhancedConfigPreset(const std::string& name,
                            const std::string& configDescription = "");
bool removeEnhancedConfigPreset(const std::string& name);
std::vector<std::string> listEnhancedConfigPresets();
bool applyEnhancedConfigPreset(const std::string& name,
                              const std::unordered_map<std::string, std::string>& parameters = {});

// Enhanced environment variable support
void enableEnhancedEnvVarOverride(bool enabled);
bool isEnhancedEnvVarOverrideEnabled();
void setEnhancedEnvVarPrefix(const std::string& prefix);
std::string getEnhancedEnvVarPrefix();

// Enhanced validation
void addEnhancedConfigValidationRule(const std::string& key,
                                   std::function<bool(const ConfigValue&)> validator,
                                   const std::string& errorMessage = "");
void addEnhancedConfigRangeValidation(const std::string& key, double min, double max,
                                     const std::string& errorMessage = "");
void addEnhancedConfigEnumValidation(const std::string& key,
                                    const std::vector<std::string>& allowedValues,
                                    const std::string& errorMessage = "");
std::vector<std::string> getEnhancedConfigValidationErrors();
bool isEnhancedConfigValid();

// Enhanced argument parsing
void addEnhancedArgumentOption(const std::string& longName, char shortName = '\0',
                              const std::string& description = "",
                              bool required = false,
                              const std::string& defaultValue = "",
                              std::function<bool(const std::string&)> validator = nullptr);
void addEnhancedArgumentFlag(const std::string& longName, char shortName = '\0',
                            const std::string& description = "",
                            bool defaultValue = false);
void addEnhancedArgumentPositional(const std::string& name,
                                  const std::string& description = "",
                                  bool required = false,
                                  std::function<bool(const std::string&)> validator = nullptr);
void addEnhancedArgumentMutuallyExclusiveGroup(const std::vector<std::string>& optionNames);
bool parseEnhancedArguments(int argc, char** argv);
std::string getEnhancedArgumentValue(const std::string& optionName);
bool getEnhancedArgumentFlag(const std::string& optionName);
std::string getEnhancedArgumentPositional(size_t index);
bool hasEnhancedArgument(const std::string& optionName);
std::string getEnhancedArgumentHelpText();
std::vector<std::string> getEnhancedArgumentErrors();

// Enhanced application management
void initializeEnhancedApplication(int argc, char** argv);
bool isEnhancedApplicationInitialized();
void runEnhancedApplication();
void shutdownEnhancedApplication();

// Global constants and error codes
namespace Defaults {
    const std::string ENHANCED_CONFIG_FILE = "configs/default.cfg";
    const std::string ENHANCED_CONFIG_FORMAT = "auto";
    const bool ENHANCED_CONFIG_INHERIT = true;
    const bool ENHANCED_CONFIG_VALIDATE = true;
    const bool ENHANCED_CONFIG_OVERRIDE_WITH_ENV = true;
    const std::string ENHANCED_ENV_PREFIX = "NLM_";
    const std::string ENHANCED_BACKUP_DIRECTORY = "./enhanced_backups";
    const size_t ENHANCED_MAX_BACKUPS = 20;
    const size_t ENHANCED_MAX_HISTORY_ENTRIES = 200;
    const std::string ENHANCED_LOG_FILENAME = "enhanced_nlm.log";
}

namespace ErrorCodes {
    const int ENHANCED_SUCCESS = 0;
    const int ENHANCED_CONFIG_FILE_NOT_FOUND = 1;
    const int ENHANCED_CONFIG_FILE_FORMAT_ERROR = 2;
    const int ENHANCED_CONFIG_VALIDATION_ERROR = 3;
    const int ENHANCED_CONFIG_INHERIT_ERROR = 4;
    const int ENHANCED_ENV_VAR_OVERRIDE_ERROR = 5;
    const int ENHANCED_CONFIG_BACKUP_ERROR = 6;
    const int ENHANCED_CONFIG_RESTORE_ERROR = 7;
    const int ENHANCED_CONFIG_HISTORY_ERROR = 8;
    const int ENHANCED_CONFIG_PRESET_ERROR = 9;
    const int ENHANCED_ARGUMENT_PARSE_ERROR = 10;
    const int ENHANCED_ARGUMENT_VALIDATION_ERROR = 11;
    const int ENHANCED_APPLICATION_INITIALIZATION_ERROR = 12;
    const int ENHANCED_APPLICATION_RUN_ERROR = 13;
}

// Enhanced application main function
int enhancedMain(int argc, char** argv);

// Backward compatibility functions
// These functions maintain the original Config interface
class Config {
public:
    Config();
    ~Config();
    
    // Original interface maintained for backward compatibility
    bool loadFromFile(const std::string& filepath);
    bool loadFromArgs(int argc, char** argv);
    bool saveToFile(const std::string& filepath) const;
    template<typename T> std::optional<T> get(const std::string& key) const;
    template<typename T> T getOr(const std::string& key, const T& defaultValue) const;
    void set(const std::string& key, const ConfigValue& value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, const std::string& value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, int value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, double value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, bool value, ConfigSource source = ConfigSource::Runtime);
    bool has(const std::string& key) const;
    void remove(const std::string& key);
    std::vector<std::string> getKeys() const;
    void clear();
    std::string summary() const;
    
    // Enhanced configuration access
    const std::vector<ConfigEntry>& getEntries() const { return enhancedConfig_.getEntries(); }
    void setEnhancedConfig(const EnhancedConfig& config) { enhancedConfig_ = config; }
    
private:
    EnhancedConfig enhancedConfig_;
    
    // Helper functions for backward compatibility
    std::string getStringFromConfigValue(const ConfigValue& value) const;
    std::string configValueToString(const ConfigValue& value) const;
};

// Global function for backward compatibility
Config& getConfig();

} // namespace nlm
