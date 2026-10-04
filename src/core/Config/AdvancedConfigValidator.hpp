// Configuration validation header
#pragma once

#include <string>
#include <vector>
#include <unordered_map>
#include <functional>
#include "Config.hpp"

namespace nlm {

// Configuration validation errors
struct ConfigValidationError {
    enum class ErrorType {
        INVALID_TYPE,
        OUT_OF_RANGE,
        MISSING_REQUIRED_FIELD,
        INVALID_VALUE,
        CONFLICTING_VALUES,
        SCHEMA_MISMATCH
    };
    
    ErrorType type;
    std::string field;
    std::string message;
    std::string suggestion;
    
    ConfigValidationError(ErrorType t, const std::string& f, const std::string& m, const std::string& s = "")
        : type(t), field(f), message(m), suggestion(s) {}
};

// Configuration schema definition
struct ConfigSchemaField {
    std::string name;
    std::string type; // "int", "double", "string", "bool"
    bool required = false;
    std::string description;
    std::vector<std::string> allowedValues;
    
    // Range constraints
    std::optional<double> minValue;
    std::optional<double> maxValue;
    
    // Validation function
    std::function<bool(const ConfigValue&)> validator;
    
    ConfigSchemaField(const std::string& n, const std::string& t, bool r = false, 
                     const std::string& desc = "", const std::vector<std::string>& allowed = {})
        : name(n), type(t), required(r), description(desc), allowedValues(allowed) {}
};

// Advanced configuration validation system
class ConfigValidator {
public:
    ConfigValidator();
    ~ConfigValidator();
    
    // Add validation rule for a field
    void addField(const ConfigSchemaField& field);
    
    // Add cross-field validation
    void addCrossFieldValidation(const std::string& field1, const std::string& field2,
                                std::function<bool(const ConfigValue&, const ConfigValue&)> validator,
                                const std::string& errorMessage);
    
    // Validate configuration against schema
    std::vector<ConfigValidationError> validate(const Config& config) const;
    
    // Validate configuration file
    std::vector<ConfigValidationError> validateFile(const std::string& filepath) const;
    
    // Get validation summary
    std::string getValidationSummary(const std::vector<ConfigValidationError>& errors) const;
    
    // Get field descriptions for help
    std::string getFieldHelp(const std::string& fieldName) const;
    
    // Check if configuration is valid
    bool isValid(const Config& config) const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Helper methods
    std::vector<ConfigValidationError> validateValue(const ConfigSchemaField& field, 
                                                     const ConfigValue& value) const;
    bool compareTypes(const std::string& type1, const std::string& type2) const;
    std::string typeToString(const ConfigValue& value) const;
};

// Advanced CLI option parser
class AdvancedCLIOptionParser {
public:
    AdvancedCLIOptionParser();
    ~AdvancedCLIOptionParser();
    
    // Add option with validation
    void addOption(const std::string& longName, const std::string& shortName = "",
                   const std::string& description = "", const std::string& type = "string",
                   const std::string& defaultValue = "",
                   std::function<bool(const std::string&)> validator = nullptr);
    
    // Add positional argument
    void addPositionalArgument(const std::string& name, const std::string& description,
                              const std::string& type = "string");
    
    // Parse command line arguments
    bool parse(int argc, char** argv, std::unordered_map<std::string, std::string>& parsedArgs) const;
    
    // Get help text
    std::string getHelp(const std::string& programName) const;
    
    // Get error messages
    std::string getErrorMessages() const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Configuration comparison and diff tools
class ConfigComparator {
public:
    ConfigComparator();
    ~ConfigComparator();
    
    // Compare two configurations
    struct ConfigDiff {
        enum class DiffType {
            ADDED,
            REMOVED,
            MODIFIED,
            UNCHANGED
        };
        
        std::string fieldName;
        DiffType type;
        std::string oldValue;
        std::string newValue;
        std::string description;
        
        ConfigDiff(const std::string& name, DiffType t, const std::string& old,
                  const std::string& newVal, const std::string& desc = "")
            : fieldName(name), type(t), oldValue(old), newValue(newVal), description(desc) {}
    };
    
    std::vector<ConfigDiff> compare(const Config& config1, const Config& config2) const;
    
    // Get human-readable diff
    std::string getHumanReadableDiff(const std::vector<ConfigDiff>& diffs) const;
    
    // Get unified diff format
    std::string getUnifiedDiff(const std::vector<ConfigDiff>& diffs) const;
};

// Configuration migration and upgrade tools
class ConfigMigrator {
public:
    ConfigMigrator();
    ~ConfigMigrator();
    
    // Add migration rule
    void addMigration(const std::string& fromVersion, const std::string& toVersion,
                     std::function<Config(Config&)> migrationFunction,
                     const std::string& description);
    
    // Apply migrations
    Config applyMigrations(Config config, const std::string& fromVersion, const std::string& toVersion) const;
    
    // Get migration history
    std::vector<std::string> getMigrationHistory() const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Configuration export and import utilities
class ConfigExporter {
public:
    ConfigExporter();
    ~ConfigExporter();
    
    // Export to JSON
    std::string exportToJSON(const Config& config) const;
    
    // Export to YAML (if YAML support is available)
    // std::string exportToYAML(const Config& config) const;
    
    // Export to TOML (if TOML support is available)
    // std::string exportToTOML(const Config& config) const;
    
    // Export to human-readable format
    std::string exportToHumanReadable(const Config& config) const;
    
    // Import from JSON
    bool importFromJSON(const std::string& json, Config& config) const;
    
    // Import from human-readable format
    bool importFromHumanReadable(const std::string& text, Config& config) const;
};

// Configuration summary and statistics
class ConfigStats {
public:
    ConfigStats();
    ~ConfigStats();
    
    // Get configuration statistics
    struct ConfigStatistics {
        size_t totalFields;
        size_t totalBytes;
        size_t stringFields;
        size_t numericFields;
        size_t booleanFields;
        size_t arrayFields;
        double averageValueLength;
        std::vector<std::string> duplicateValues;
    };
    
    ConfigStatistics computeStats(const Config& config) const;
    
    // Get field type distribution
    std::unordered_map<std::string, size_t> getFieldTypeDistribution(const Config& config) const;
    
    // Get source distribution
    std::unordered_map<std::string, size_t> getSourceDistribution(const Config& config) const;
};

} // namespace nlm