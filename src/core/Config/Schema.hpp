#pragma once

#include <string>
#include <vector>
#include <unordered_map>
#include <functional>
#include <memory>
#include <optional>
#include <variant>

namespace nlm {

// Configuration parameter type definitions
enum class ParameterType {
    INT,
    INT64,
    DOUBLE,
    BOOL,
    STRING,
    VECTOR_INT,
    VECTOR_DOUBLE,
    VECTOR_STRING,
    OBJECT,
    ARRAY
};

// Validation result
struct ValidationResult {
    bool is_valid;
    std::vector<std::string> errors;
    std::vector<std::string> warnings;
    
    ValidationResult() : is_valid(true) {}
    ValidationResult(bool valid, const std::vector<std::string>& errs, const std::vector<std::string>& warns = {})
        : is_valid(valid), errors(errs), warnings(warns) {}
    
    void addError(const std::string& error) { errors.push_back(error); }
    void addWarning(const std::string& warning) { warnings.push_back(warning); }
    bool hasErrors() const { return !errors.empty(); }
    bool hasWarnings() const { return !warnings.empty(); }
};

// Parameter metadata
struct ParameterMeta {
    std::string name;
    ParameterType type;
    std::string description;
    std::string category;
    std::string group;
    std::vector<std::string> tags;
    
    // Default values
    std::optional<int> default_int;
    std::optional<int64_t> default_int64;
    std::optional<double> default_double;
    std::optional<bool> default_bool;
    std::optional<std::string> default_string;
    std::optional<std::vector<int>> default_vector_int;
    std::optional<std::vector<double>> default_vector_double;
    std::optional<std::vector<std::string>> default_vector_string;
    
    // Constraints
    std::optional<int> min_int;
    std::optional<int64_t> min_int64;
    std::optional<double> min_double;
    std::optional<int> max_int;
    std::optional<int64_t> max_int64;
    std::optional<double> max_double;
    std::optional<size_t> min_length;
    std::optional<size_t> max_length;
    std::optional<size_t> min_size;
    std::optional<size_t> max_size;
    
    // Validation rules
    std::function<ValidationResult(const std::string&, const nlm::ConfigValue&)> validation_rule;
    
    // Options for enum-like parameters
    std::vector<std::string> allowed_values;
    
    ParameterMeta(const std::string& name, ParameterType type, const std::string& description = "")
        : name(name), type(type), description(description), category("general"), group("basic") {}
};

// Configuration schema
class ConfigurationSchema {
private:
    std::unordered_map<std::string, ParameterMeta> parameters;
    std::unordered_map<std::string, std::vector<std::string>> groups;
    std::unordered_map<std::string, std::vector<std::string>> categories;
    
public:
    void addParameter(const ParameterMeta& param);
    
    bool hasParameter(const std::string& name) const;
    const ParameterMeta* getParameter(const std::string& name) const;
    
    const std::unordered_map<std::string, ParameterMeta>& getAllParameters() const { return parameters; }
    
    // Group and category management
    void addGroup(const std::string& name, const std::vector<std::string>& parameter_names);
    void addCategory(const std::string& name, const std::vector<std::string>& group_names);
    
    // Validation
    ValidationResult validate(const std::string& key, const nlm::ConfigValue& value) const;
    ValidationResult validateAll(const std::vector<std::pair<std::string, nlm::ConfigValue>>& key_values) const;
    
    // Schema utilities
    std::vector<std::string> getAllKeys() const;
    std::vector<std::string> getKeysByGroup(const std::string& group) const;
    std::vector<std::string> getKeysByCategory(const std::string& category) const;
    std::vector<std::string> getGroups() const;
    std::vector<std::string> getCategories() const;
    
    // Documentation generation
    std::string generateDocumentation() const;
    std::string generateCategoryDocumentation(const std::string& category) const;
    
    // Export/import schema
    std::string exportSchema() const;
    bool importSchema(const std::string& schema_json);
}; 

// Global schema instance
extern ConfigurationSchema global_schema;

// Parameter macros for easy schema definition
#define CONFIG_PARAM_INT(name, default_value, description) \
    global_schema.addParameter(ParameterMeta(name, ParameterType::INT, description) \
        .default_int = default_value)

#define CONFIG_PARAM_DOUBLE(name, default_value, description) \
    global_schema.addParameter(ParameterMeta(name, ParameterType::DOUBLE, description) \
        .default_double = default_value)

#define CONFIG_PARAM_BOOL(name, default_value, description) \
    global_schema.addParameter(ParameterMeta(name, ParameterType::BOOL, description) \
        .default_bool = default_value)

#define CONFIG_PARAM_STRING(name, default_value, description) \
    global_schema.addParameter(ParameterMeta(name, ParameterType::STRING, description) \
        .default_string = default_value)

#define CONFIG_PARAM_VECTOR_INT(name, default_value, description) \
    global_schema.addParameter(ParameterMeta(name, ParameterType::VECTOR_INT, description) \
        .default_vector_int = default_value)

#define CONFIG_PARAM_VECTOR_DOUBLE(name, default_value, description) \
    global_schema.addParameter(ParameterMeta(name, ParameterType::VECTOR_DOUBLE, description) \
        .default_vector_double = default_value)

#define CONFIG_PARAM_VECTOR_STRING(name, default_value, description) \
    global_schema.addParameter(ParameterMeta(name, ParameterType::VECTOR_STRING, description) \
        .default_vector_string = default_value)

// Constraint macros
#define CONFIG_PARAM_RANGE_INT(name, min, max) \
    global_schema.getParameter(name)->min_int = min; \
    global_schema.getParameter(name)->max_int = max

#define CONFIG_PARAM_RANGE_DOUBLE(name, min, max) \
    global_schema.getParameter(name)->min_double = min; \
    global_schema.getParameter(name)->max_double = max

#define CONFIG_PARAM_LENGTH(name, min_len, max_len) \
    global_schema.getParameter(name)->min_length = min_len; \
    global_schema.getParameter(name)->max_length = max_len

#define CONFIG_PARAM_SIZE(name, min_size, max_size) \
    global_schema.getParameter(name)->min_size = min_size; \
    global_schema.getParameter(name)->max_size = max_size

#define CONFIG_PARAM_ALLOWED_VALUES(name, values) \
    global_schema.getParameter(name)->allowed_values = values

// Group and category macros
#define CONFIG_GROUP(name, parameters) \
    global_schema.addGroup(name, {parameters})

#define CONFIG_CATEGORY(name, groups) \
    global_schema.addCategory(name, {groups})

} // namespace nlm
