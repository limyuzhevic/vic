#pragma once

#include <string>
#include <vector>
#include <map>
#include <optional>
#include <variant>
#include <functional>
#include <regex>
#include <chrono>
#include <memory>
#include <unordered_map>
#include <set>

namespace nlm {

// Configuration value types for schema validation
enum class ConfigDataType {
    Integer,
    Float,
    Boolean,
    String,
    VectorInt,
    VectorFloat,
    VectorString,
    StringList,
    Object
};

// Validation rule types
enum class ValidationRule {
    Range,
    Pattern,
    Enum,
    Required,
    Custom,
    CrossKey,
    Exclusive
};

// Configuration source types
enum class ConfigSource {
    Default,
    File,
    CommandLine,
    Environment,
    Runtime
};

// Schema validation error
struct SchemaValidationError {
    std::string key;
    std::string message;
    ValidationRule rule;
    std::string expected;
    std::string actual;
    
    SchemaValidationError(const std::string& k, const std::string& msg, 
                         ValidationRule r, const std::string& exp = "", 
                         const std::string& act = "")
        : key(k), message(msg), rule(r), expected(exp), actual(act) {}
};

// Configuration schema entry
struct ConfigSchemaEntry {
    std::string key;
    ConfigDataType type;
    std::string description;
    std::string defaultValue;
    bool required;
    std::map<ValidationRule, std::string> validationRules;
    std::set<std::string> dependsOn;
    std::set<std::string> mutuallyExclusive;
    
    ConfigSchemaEntry(const std::string& k, ConfigDataType t, 
                     const std::string& desc = "", 
                     const std::string& def = "",
                     bool req = false)
        : key(k), type(t), description(desc), defaultValue(def), 
          required(req) {}
};

// Configuration schema
class ConfigSchema {
public:
    ConfigSchema() = default;
    
    // Add a schema entry
    void addEntry(const ConfigSchemaEntry& entry);
    
    // Get schema entry by key
    const ConfigSchemaEntry* getEntry(const std::string& key) const;
    
    // Check if key exists in schema
    bool hasEntry(const std::string& key) const;
    
    // Get all required keys
    std::vector<std::string> getRequiredKeys() const;
    
    // Get all keys
    std::vector<std::string> getAllKeys() const;
    
    // Validate a value against the schema
    std::vector<SchemaValidationError> validate(const std::string& key, 
                                               const std::string& value,
                                               const std::map<std::string, std::string>& config) const;
    
    // Validate entire configuration
    std::vector<SchemaValidationError> validateAll(const std::map<std::string, std::string>& config) const;
    
    // Get type for key
    ConfigDataType getType(const std::string& key) const;
    
    // Check if key is required
    bool isRequired(const std::string& key) const;
    
    // Get default value for key
    std::string getDefault(const std::string& key) const;
    
    // Get description for key
    std::string getDescription(const std::string& key) const;
    
    // Get validation rules for key
    std::map<ValidationRule, std::string> getValidationRules(const std::string& key) const;
    
    // Check dependencies
    std::vector<std::string> getDependencies(const std::string& key) const;
    
    // Check mutual exclusivity
    std::vector<std::string> getMutuallyExclusive(const std::string& key) const;
    
private:
    std::map<std::string, ConfigSchemaEntry> entries_;
};

// Configuration template
struct ConfigTemplate {
    std::string name;
    std::string description;
    std::map<std::string, std::string> values;
    
    ConfigTemplate(const std::string& n, const std::string& desc = "")
        : name(n), description(desc) {}
};

// Configuration preset
struct ConfigPreset {
    std::string name;
    std::string category; // development, production, research
    std::string description;
    std::map<std::string, std::string> values;
    
    ConfigPreset(const std::string& n, const std::string& cat, const std::string& desc = "")
        : name(n), category(cat), description(desc) {}
};

// Configuration comparison result
struct ConfigDiffEntry {
    std::string key;
    std::string oldValue;
    std::string newValue;
    ConfigSource source;
    bool isRemoved;
    bool isAdded;
    
    ConfigDiffEntry(const std::string& k, const std::string& old, 
                   const std::string& newVal, ConfigSource src)
        : key(k), oldValue(old), newValue(newVal), source(src),
          isRemoved(old != "" && newVal == ""),
          isAdded(old == "" && newVal != "") {}
};

class ConfigSchema; // Forward declaration

class EnhancedConfig {
public:
    EnhancedConfig(const ConfigSchema& schema = ConfigSchema());
    ~EnhancedConfig();
    
    // Disable copying
    EnhancedConfig(const EnhancedConfig&) = delete;
    EnhancedConfig& operator=(const EnhancedConfig&) = delete;
    
    // Enable moving
    EnhancedConfig(EnhancedConfig&&) noexcept;
    EnhancedConfig& operator=(EnhancedConfig&&) noexcept;
    
    // Load from file (JSON/YAML/INI formats)
    bool loadFromFile(const std::string& filepath);
    
    // Load from JSON string
    bool loadFromJson(const std::string& jsonStr);
    
    // Load from YAML string
    bool loadFromYaml(const std::string& yamlStr);
    
    // Load from environment variables (NLM_ prefix)
    void loadFromEnvironment(const std::string& prefix = "NLM_");
    
    // Load from command line arguments
    bool loadFromCommandLine(int argc, char** argv);
    
    // Save to file
    bool saveToFile(const std::string& filepath) const;
    
    // Export to JSON string
    std::string exportToJson() const;
    
    // Export to YAML string
    std::string exportToYaml() const;
    
    // Get configuration value with schema validation
    template<typename T>
    std::optional<T> get(const std::string& key) const;
    
    // Get or set with default
    template<typename T>
    T getOr(const std::string& key, const T& defaultValue) const;
    
    // Set configuration value with validation
    template<typename T>
    bool set(const std::string& key, const T& value, ConfigSource source = ConfigSource::Runtime);
    
    // Set from string with schema validation
    bool setString(const std::string& key, const std::string& value, ConfigSource source = ConfigSource::Runtime);
    
    // Check if key exists
    bool has(const std::string& key) const;
    
    // Get all keys
    std::vector<std::string> getKeys() const;
    
    // Get all values as strings
    std::map<std::string, std::string> getAllAsStrings() const;
    
    // Remove configuration value
    void remove(const std::string& key);
    
    // Clear all configuration
    void clear();
    
    // Validate configuration against schema
    std::vector<SchemaValidationError> validate() const;
    
    // Validate specific key
    std::vector<SchemaValidationError> validateKey(const std::string& key) const;
    
    // Check business rules
    std::vector<SchemaValidationError> validateBusinessRules() const;
    
    // Check cross-key dependencies
    std::vector<SchemaValidationError> validateCrossKeys() const;
    
    // Get configuration summary
    std::string summary() const;
    
    // Get configuration diff
    std::vector<ConfigDiffEntry> diff(const EnhancedConfig& other) const;
    
    // Apply configuration template
    bool applyTemplate(const ConfigTemplate& template_);
    
    // Apply configuration preset
    bool applyPreset(const ConfigPreset& preset);
    
    // Generate configuration from template name
    static bool generateFromTemplate(const std::string& templateName, 
                                     EnhancedConfig& config);
    
    // Get available templates
    static std::vector<std::string> getAvailableTemplates();
    
    // Get available presets
    static std::vector<ConfigPreset> getAvailablePresets(const std::string& category = "");
    
    // Merge with another configuration (layering)
    void merge(const EnhancedConfig& other, ConfigSource source = ConfigSource::Runtime);
    
    // Get schema
    const ConfigSchema& getSchema() const;
    
    // Set schema
    void setSchema(const ConfigSchema& schema);
    
    // Export configuration to file
    bool exportToFile(const std::string& filepath) const;
    
    // Import configuration from file
    static bool importFromFile(const std::string& filepath, EnhancedConfig& config);
    
    // Generate configuration documentation
    std::string generateDocumentation() const;
    
    // Compress configuration (reduce memory footprint)
    void compress();
    
    // Optimize lookups (build indexes)
    void optimize();
    
    // Get performance metrics
    struct PerformanceMetrics {
        size_t totalLookups;
        size_t cacheHits;
        double averageLookupTime;
        size_t memoryUsage;
        
        PerformanceMetrics() : totalLookups(0), cacheHits(0), 
                               averageLookupTime(0.0), memoryUsage(0) {}
    };
    
    PerformanceMetrics getPerformanceMetrics() const;
    
    // Reset performance metrics
    void resetPerformanceMetrics();
    
    // Get current configuration version
    std::string getVersion() const;
    
    // Set configuration version
    void setVersion(const std::string& version);
    
    // Migrate configuration from old version
    bool migrateFromVersion(const std::string& fromVersion);
    
    // Check if configuration has changed since last save
    bool hasChanged() const;
    
    // Get configuration change history
    std::vector<std::chrono::system_clock::time_point> getChangeHistory() const;
    
    // Get change history size
    size_t getChangeHistorySize() const;
    
    // Enable/disable history tracking
    void setHistoryEnabled(bool enabled);
    
    // Export configuration with history
    bool exportWithHistory(const std::string& filepath) const;
    
    // Import configuration with history
    static bool importWithHistory(const std::string& filepath, EnhancedConfig& config);
    
    // Create backup of current configuration
    std::string createBackup(const std::string& prefix = "backup") const;
    
    // Restore configuration from backup
    static bool restoreFromBackup(const std::string& backupPath, EnhancedConfig& config);
    
    // Get all backup files
    static std::vector<std::string> getBackupFiles(const std::string& pattern = "*.backup");
    
    // Clean up old backup files
    static void cleanupBackups(size_t keepCount = 10);
    
    // Generate performance recommendations
    std::vector<std::string> generatePerformanceRecommendations() const;
    
    // Apply performance recommendations
    bool applyPerformanceRecommendations();
    
    // Get configuration size
    size_t getSize() const;
    
    // Get memory usage
    size_t getMemoryUsage() const;
    
    // Get configuration entry count
    size_t getEntryCount() const;
    
    // Validate key pattern (basic validation)
    static bool validateKeyPattern(const std::string& key);
    
    // Validate value range (basic validation)
    static bool validateValueRange(const std::string& key, const std::string& value);
    
    // Get configuration source for key
    ConfigSource getSource(const std::string& key) const;
    
    // Get all entries with their sources
    std::map<std::string, ConfigSource> getAllSources() const;
    
    // Get schema validation errors for documentation
    std::vector<std::string> getSchemaValidationErrors() const;
    
    // Generate schema documentation
    std::string generateSchemaDocumentation() const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Internal helpers
    bool isValidType(const std::string& key, const std::string& value) const;
    std::string convertToString(const std::any& value) const;
    template<typename T>
    std::optional<T> getFromAny(const std::any& value) const;
};

// Global default schema
ConfigSchema createDefaultSchema();

// Configuration templates
namespace Templates {
    ConfigTemplate createDevelopmentTemplate();
    ConfigTemplate createProductionTemplate();
    ConfigTemplate createResearchTemplate();
    ConfigTemplate createMinimalTemplate();
    ConfigTemplate createPerformanceTemplate();
    ConfigTemplate createMemoryTemplate();
    ConfigTemplate createDebugTemplate();
}

// Configuration presets
namespace Presets {
    ConfigPreset createDevelopment();
    ConfigPreset createProduction();
    ConfigPreset createResearch();
    ConfigPreset createHighPerformance();
    ConfigPreset createMinimal();
    ConfigPreset createDebug();
    ConfigPreset createCustom(const std::string& name, const std::map<std::string, std::string>& values);
}

// Configuration comparison utilities
namespace Comparison {
    std::vector<ConfigDiffEntry> compare(const EnhancedConfig& config1, 
                                        const EnhancedConfig& config2);
    std::string diffToString(const std::vector<ConfigDiffEntry>& diff);
    bool exportDiff(const std::vector<ConfigDiffEntry>& diff, 
                   const std::string& filepath);
}

// Configuration validation utilities
namespace Validation {
    std::vector<SchemaValidationError> validateSchema(const EnhancedConfig& config);
    std::vector<SchemaValidationError> validateBusinessRules(const EnhancedConfig& config);
    std::string validationErrorsToString(const std::vector<SchemaValidationError>& errors);
}

// Configuration migration utilities
namespace Migration {
    bool migrateConfiguration(EnhancedConfig& config, const std::string& fromVersion);
    std::string getMigrationSteps(const std::string& fromVersion, const std::string& toVersion);
}

// Configuration export/import utilities
namespace ExportImport {
    bool exportConfiguration(const EnhancedConfig& config, 
                            const std::string& filepath,
                            const std::string& format = "json");
    bool importConfiguration(EnhancedConfig& config,
                            const std::string& filepath,
                            const std::string& format = "json");
    std::string configurationToString(const EnhancedConfig& config,
                                      const std::string& format = "json");
    bool stringToConfiguration(const std::string& str, 
                              EnhancedConfig& config,
                              const std::string& format = "json");
}

// Configuration history utilities
namespace History {
    void logChange(EnhancedConfig& config, const std::string& key, 
                   const std::string& oldValue, const std::string& newValue);
    std::vector<std::string> getChangeLog(const EnhancedConfig& config);
    bool exportHistory(EnhancedConfig& config, const std::string& filepath);
    bool importHistory(EnhancedConfig& config, const std::string& filepath);
}

} // namespace nlm