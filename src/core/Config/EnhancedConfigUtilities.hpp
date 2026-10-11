#pragma once

#include <string>
#include <map>
#include <any>
#include <vector>
#include <functional>
#include <regex>

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

// Configuration source
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
    std::vector<std::string> dependsOn;
    std::vector<std::string> mutuallyExclusive;
    
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

// Configuration comparison entry
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

// Forward declaration
class EnhancedConfig;

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