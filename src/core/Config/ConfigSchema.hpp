#pragma once

#include <string>
#include <vector>
#include <unordered_map>
#include <memory>
#include <functional>
#include <optional>
#include <variant>

namespace nlm {

class ConfigValidator;

class ConfigSchema {
public:
    ConfigSchema(const std::string& name);
    ~ConfigSchema();
    
    // Schema name
    std::string getName() const { return name_; }
    
    // Field definition
    void addField(const std::string& key, const std::string& type, 
                  const std::string& description = "",
                  const std::optional<double>& min = std::nullopt,
                  const std::optional<double>& max = std::nullopt,
                  const std::string& pattern = "",
                  bool required = false,
                  const std::vector<std::string>& dependsOn = {});
    
    // Default values
    void setDefault(const std::string& key, const std::variant<int, int64_t, double, bool, std::string>& value);
    void setDefaults(const std::unordered_map<std::string, std::variant<int, int64_t, double, bool, std::string>>& defaults);
    
    // Validation rules
    void addValidationRule(const std::string& key, 
                          std::function<bool(const std::variant<int, int64_t, double, bool, std::string>&)> validator,
                          const std::string& errorMessage = "Validation failed");
    
    // Cross-parameter validation
    void addCrossValidation(const std::string& key,
                           std::function<bool(const std::unordered_map<std::string, std::variant<int, int64_t, double, bool, std::string>>&)> validator,
                           const std::string& errorMessage = "Cross-validation failed");
    
    // Custom formatter
    void addFormatter(const std::string& key,
                     std::function<std::string(const std::variant<int, int64_t, double, bool, std::string>&)> formatter);
    
    // Get field information
    std::string getFieldType(const std::string& key) const;
    std::string getFieldDescription(const std::string& key) const;
    bool isFieldRequired(const std::string& key) const;
    std::optional<double> getFieldMin(const std::string& key) const;
    std::optional<double> getFieldMax(const std::string& key) const;
    std::string getFieldPattern(const std::string& key) const;
    std::vector<std::string> getFieldDependencies(const std::string& key) const;
    
    // Get default value
    std::variant<int, int64_t, double, bool, std::string> getDefault(const std::string& key) const;
    bool hasDefault(const std::string& key) const;
    
    // Get validators
    std::vector<std::function<bool(const std::variant<int, int64_t, double, bool, std::string>&)>>
        getValidators(const std::string& key) const;
    
    // Get cross-validators
    std::vector<std::function<bool(const std::unordered_map<std::string, std::variant<int, int64_t, double, bool, std::string>>&)>>
        getCrossValidators() const;
    
    // Get formatters
    std::function<std::string(const std::variant<int, int64_t, double, bool, std::string>&)>
        getFormatter(const std::string& key) const;
    
    // Validate entire schema against config values
    std::vector<std::string> validate(const std::unordered_map<std::string, std::variant<int, int64_t, double, bool, std::string>>& values) const;
    
    // Validate individual field
    std::vector<std::string> validateField(const std::string& key, 
                                          const std::variant<int, int64_t, double, bool, std::string>& value) const;
    
    // Format value
    std::string formatValue(const std::string& key, const std::variant<int, int64_t, double, bool, std::string>& value) const;
    
    // Get all field keys
    std::vector<std::string> getFieldKeys() const;
    
    // Schema statistics
    size_t getFieldCount() const { return fields_.size(); }
    size_t getValidatorCount() const;
    size_t getCrossValidatorCount() const;
    
    // Schema info
    std::string getInfo() const;

private:
    struct FieldInfo {
        std::string type;
        std::string description;
        std::optional<double> min;
        std::optional<double> max;
        std::string pattern;
        bool required;
        std::vector<std::string> dependsOn;
        std::variant<int, int64_t, double, bool, std::string> defaultValue;
        bool hasDefault;
        std::vector<std::function<bool(const std::variant<int, int64_t, double, bool, std::string>&)>> validators;
        std::function<std::string(const std::variant<int, int64_t, double, bool, std::string>&)> formatter;
    };
    
    std::string name_;
    std::unordered_map<std::string, FieldInfo> fields_;
    std::vector<std::function<bool(const std::unordered_map<std::string, std::variant<int, int64_t, double, bool, std::string>>&)>> crossValidators_;
    std::vector<std::string> crossValidatorMessages_;
    std::unordered_map<std::string, std::function<std::string(const std::variant<int, int64_t, double, bool, std::string>&)>> formatters_;
};

class ConfigSchemaRegistry {
public:
    static ConfigSchemaRegistry& getInstance();
    
    // Register schema
    void registerSchema(const std::string& name, std::shared_ptr<ConfigSchema> schema);
    
    // Get schema
    std::shared_ptr<ConfigSchema> getSchema(const std::string& name) const;
    
    // Check existence
    bool hasSchema(const std::string& name) const;
    
    // Remove schema
    void removeSchema(const std::string& name);
    
    // Get all schema names
    std::vector<std::string> getSchemaNames() const;
    
    // Schema validation
    std::vector<std::string> validate(const std::string& schemaName,
                                      const std::unordered_map<std::string, std::variant<int, int64_t, double, bool, std::string>>& values) const;
    
    // Clear all schemas
    void clear();

private:
    ConfigSchemaRegistry() = default;
    std::unordered_map<std::string, std::shared_ptr<ConfigSchema>> schemas_;
};

} // namespace nlm