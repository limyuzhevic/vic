// ConfigSchema.hpp - Configuration schema validation and type system
#pragma once

#include <string>
#include <unordered_map>
#include <vector>
#include <memory>
#include <variant>
#include <functional>
#include <optional>

namespace nlm {

typealias ConfigValue = std::variant<
    std::monostate, // null
    bool,
    int64_t,
    uint64_t,
    double,
    std::string,
    std::vector<bool>,
    std::vector<int64_t>,
    std::vector<uint64_t>,
    std::vector<double>,
    std::vector<std::string>
>;

struct ConfigField {
    std::string name;
    ConfigValue defaultValue;
    std::string type; // "bool", "int", "uint", "float", "double", "string", "bool_array", "int_array", "uint_array", "float_array", "string_array"
    bool required = false;
    std::string description;
    std::function<bool(const ConfigValue&)> validator;
    std::string category; // "brain", "development", "neuromodulation", "memory", "performance"
    int minValue = 0;
    int maxValue = 0;
    std::string displayName;
};

struct ConfigSchema {
    std::string version;
    std::string description;
    std::vector<ConfigField> fields;
    
    // Validation
    bool validate(const std::unordered_map<std::string, ConfigValue>& config) const;
    std::vector<std::string> getErrors(const std::unordered_map<std::string, ConfigValue>& config) const;
    
    // Field access
    const ConfigField* getField(const std::string& name) const;
    std::vector<ConfigField> getFieldsByCategory(const std::string& category) const;
    
    // Schema management
    void addField(const ConfigField& field);
    bool removeField(const std::string& name);
    void clear();
    
    // Schema serialization
    std::string toJSON() const;
    bool fromJSON(const std::string& json);
};

// Predefined configuration schemas
class ConfigSchemas {
public:
    static ConfigSchema getDefaultSchema();
    static ConfigSchema getDevelopmentSchema();
    static ConfigSchema getReinforcementLearningSchema();
    static ConfigSchema getPerformanceSchema();
    static ConfigSchema getMinimalSchema();
    
    // Schema validation utilities
    static bool validateConfig(const std::unordered_map<std::string, ConfigValue>& config, 
                              const std::string& schemaName = "default");
    static std::vector<std::string> getValidationErrors(
        const std::unordered_map<std::string, ConfigValue>& config,
        const std::string& schemaName = "default");
    
    // Configuration recommendations based on use case
    static std::unordered_map<std::string, ConfigValue> getRecommendedConfig(const std::string& useCase);
};

// Configuration template system
class ConfigTemplate {
public:
    ConfigTemplate() = default;
    ~ConfigTemplate() = default;
    
    // Template variables
    void setTemplateVariable(const std::string& name, const ConfigValue& value);
    ConfigValue getTemplateVariable(const std::string& name) const;
    
    // Substitutions
    void addSubstitution(const std::string& pattern, const std::string& replacement);
    std::string applySubstitutions(const std::string& input) const;
    
    // Template processing
    std::unordered_map<std::string, ConfigValue> generateConfig() const;
    
    // Template management
    void clear();
    bool isEmpty() const;
    
    // Template serialization
    std::string toJSON() const;
    bool fromJSON(const std::string& json);
    
private:
    std::unordered_map<std::string, ConfigValue> templateVariables_;
    std::vector<std::pair<std::string, std::string>> substitutions_;
};

// Configuration validator
class ConfigValidator {
public:
    static bool validate(const std::unordered_map<std::string, ConfigValue>& config,
                        const ConfigSchema& schema);
    
    static std::vector<std::string> getAllValidationErrors(
        const std::unordered_map<std::string, ConfigValue>& config,
        const ConfigSchema& schema);
    
    // Type conversion and validation helpers
    static bool validateType(const ConfigValue& value, const std::string& typeName);
    static std::string getValueTypeName(const ConfigValue& value);
    
    // Range validation
    static bool validateRange(const ConfigValue& value, int min, int max);
    static bool validateRange(const ConfigValue& value, double min, double max);
    
    // Array validation
    static bool validateArraySize(const ConfigValue& value, size_t expectedSize);
    static bool validateArrayType(const ConfigValue& value, const std::string& elementType);
};
}