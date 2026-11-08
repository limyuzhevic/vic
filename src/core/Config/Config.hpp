#pragma once

#include <string>
#include <memory>
#include <vector>
#include <variant>
#include <optional>
#include <algorithm>

namespace nlm {

// Forward declarations
class Config;
// Configuration schema validation
class ConfigSchema {
public:
    enum class ValidationResult {
        Valid,
        InvalidType,
        MissingRequired,
        OutOfRange,
        InvalidValue,
        UnknownField
    };
    
    struct SchemaField {
        std::string name;
        std::string type;  // "int", "double", "string", "bool", "array"
        bool required;
        std::string description;
        std::variant<int64_t, double, std::string, bool> defaultValue;
        std::pair<double, double> range;  // for numeric types: min, max
        std::vector<std::string> allowedValues;  // for string types
        
        SchemaField(const std::string& n, const std::string& t, bool r = false, const std::string& d = "")
            : name(n), type(t), required(r), description(d) {}
    };
    
    ConfigSchema();
    ~ConfigSchema();
    
    // Add a field definition
    void addField(const SchemaField& field);
    
    // Validate configuration against schema
    ValidationResult validate(const Config& config) const;
    
    // Get validation errors
    std::vector<std::string> getValidationErrors(const Config& config) const;
    
    // Check if field exists in schema
    bool hasField(const std::string& name) const;
    
    // Get field definition
    SchemaField getField(const std::string& name) const;
    
    // Get all field names
    std::vector<std::string> getFieldNames() const;
    
    // Clear schema
    void clear();
    
    // Check if schema is empty
    bool empty() const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Configuration schema manager (singleton)
class ConfigSchemaManager {
public:
    static ConfigSchemaManager& getInstance();
    
    // Register a schema for a configuration category
    void registerSchema(const std::string& category, const ConfigSchema& schema);
    
    // Get schema for a category
    ConfigSchema getSchema(const std::string& category) const;
    
    // Check if schema exists for category
    bool hasSchema(const std::string& category) const;
    
    // Validate configuration against registered schema
    bool validateConfiguration(const std::string& category, const Config& config) const;
    
    // Get all registered schema categories
    std::vector<std::string> getSchemaCategories() const;
    
    // Clear all schemas
    void clear();
    
private:
    ConfigSchemaManager();
    ~ConfigSchemaManager();
    
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Schema convenience functions
namespace config_schema {

// Register common NLM configuration schemas
void registerNMConfigurationSchema(ConfigSchemaManager& manager);

// Validate configuration with common NLM defaults
bool validateNMConfiguration(const Config& config);

// Get validation error details for NLM configuration
std::vector<std::string> getNMConfigurationValidationErrors(const Config& config);

// Reset to default NLM configuration schema
void resetNMConfigurationSchema(ConfigSchemaManager& manager);

} // namespace config_schema

} // namespace nlm
