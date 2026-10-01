// Configuration validation system for NLM
// Comprehensive validation with schema support

#ifndef NLM_CONFIG_VALIDATION_H
#define NLM_CONFIG_VALIDATION_H

#include <string>
#include <vector>
#include <map>
#include <variant>
#include <optional>
#include <memory>
#include <stdexcept>
#include <functional>
#include <type_traits>
#include <chrono>
#include <algorithm>
#include <sstream>

namespace nlm {
namespace config {

// Forward declarations
class Schema;
class Validator;
class ConfigurationValidator;

// Error types
enum class ValidationErrorType {
    NONE,
    TYPE_MISMATCH,
    RANGE_VIOLATION,
    MISSING_REQUIRED,
    CONDITION_VIOLATION,
    DEPENDENCY_VIOLATION,
    SCHEMA_VERSION_MISMATCH,
    CUSTOM_ERROR
};

struct ValidationError {
    ValidationErrorType type = ValidationErrorType::NONE;
    std::string path;
    std::string message;
    std::string expected;
    std::string actual;
    std::chrono::system_clock::time_point timestamp;
    
    ValidationError(ValidationErrorType t, const std::string& p, const std::string& m,
                   const std::string& e = "", const std::string& a = "")
        : type(t), path(p), message(m), expected(e), actual(a), 
          timestamp(std::chrono::system_clock::now()) {}
    
    std::string to_string() const {
        std::stringstream ss;
        ss << "[ValidationError] ";
        if (!path.empty()) ss << "Path: " << path << "; ";
        ss << message;
        if (!expected.empty()) ss << " (Expected: " << expected << ")";
        if (!actual.empty()) ss << " (Actual: " << actual << ")";
        return ss.str();
    }
};

// Type definitions for schema
using ConfigValue = std::variant<bool, int64_t, uint64_t, double, std::string,
                                 std::vector<bool>, std::vector<int64_t>, std::vector<uint64_t>,
                                 std::vector<double>, std::vector<std::string>, std::vector<ConfigValue>>;

// Schema element types
enum class SchemaElementType {
    BOOLEAN,
    INTEGER,
    INTEGER64,
    UNSIGNED_INTEGER,
    FLOAT,
    DOUBLE,
    STRING,
    ARRAY_BOOLEAN,
    ARRAY_INTEGER,
    ARRAY_UNSIGNED_INTEGER,
    ARRAY_FLOAT,
    ARRAY_DOUBLE,
    ARRAY_STRING,
    ARRAY_OBJECT,
    OBJECT,
    ENUM,
    NULL_VALUE
};

struct SchemaElement {
    std::string name;
    SchemaElementType type;
    std::optional<ConfigValue> default_value;
    std::optional<ConfigValue> min_value;
    std::optional<ConfigValue> max_value;
    std::vector<std::string> enum_values;
    std::vector<std::shared_ptr<SchemaElement>> children; // For objects
    bool required = false;
    std::function<bool(const ConfigValue&)> custom_validator;
    std::string description;
    std::map<std::string, std::shared_ptr<SchemaElement>> properties; // For named objects
    
    SchemaElement(const std::string& n, SchemaElementType t, bool req = false)
        : name(n), type(t), required(req) {}
    
    virtual ~SchemaElement() = default;
    
    virtual bool validate(const ConfigValue& value, std::vector<ValidationError>& errors, 
                         const std::string& path = "") const;
    
    virtual std::string get_type_name() const {
        switch (type) {
            case SchemaElementType::BOOLEAN: return "boolean";
            case SchemaElementType::INTEGER: return "integer";
            case SchemaElementType::INTEGER64: return "int64";
            case SchemaElementType::UNSIGNED_INTEGER: return "unsigned_integer";
            case SchemaElementType::FLOAT: return "float";
            case SchemaElementType::DOUBLE: return "double";
            case SchemaElementType::STRING: return "string";
            case SchemaElementType::ARRAY_BOOLEAN: return "array<boolean>";
            case SchemaElementType::ARRAY_INTEGER: return "array<integer>";
            case SchemaElementType::ARRAY_UNSIGNED_INTEGER: return "array<unsigned_integer>";
            case SchemaElementType::ARRAY_FLOAT: return "array<float>";
            case SchemaElementType::ARRAY_DOUBLE: return "array<double>";
            case SchemaElementType::ARRAY_STRING: return "array<string>";
            case SchemaElementType::ARRAY_OBJECT: return "array<object>";
            case SchemaElementType::OBJECT: return "object";
            case SchemaElementType::ENUM: return "enum";
            case SchemaElementType::NULL_VALUE: return "null";
            default: return "unknown";
        }
    }
};

// Validator base class
class Validator {
public:
    virtual ~Validator() = default;
    virtual bool validate(const ConfigValue& value, std::vector<ValidationError>& errors,
                         const std::string& path = "") const = 0;
    virtual std::string get_name() const = 0;
};

// Range validator
class RangeValidator : public Validator {
private:
    ConfigValue min_value_;
    ConfigValue max_value_;
    
public:
    RangeValidator(const ConfigValue& min, const ConfigValue& max)
        : min_value_(min), max_value_(max) {}
    
    bool validate(const ConfigValue& value, std::vector<ValidationError>& errors,
                 const std::string& path = "") const override {
        if (value < min_value_ || value > max_value_) {
            errors.emplace_back(ValidationErrorType::RANGE_VIOLATION, path,
                             "Value outside valid range",
                             "Range: [" + config_value_to_string(min_value_) + 
                                 ", " + config_value_to_string(max_value_) + "]",
                             config_value_to_string(value));
            return false;
        }
        return true;
    }
    
    std::string get_name() const override { return "RangeValidator"; }
};

// Enum validator
class EnumValidator : public Validator {
private:
    std::vector<std::string> valid_values_;
    
public:
    EnumValidator(const std::vector<std::string>& values)
        : valid_values_(values) {}
    
    bool validate(const ConfigValue& value, std::vector<ValidationError>& errors,
                 const std::string& path = "") const override {
        std::string str_value;
        if (std::holds_alternative<std::string>(value)) {
            str_value = std::get<std::string>(value);
        } else {
            str_value = config_value_to_string(value);
        }
        
        if (std::find(valid_values_.begin(), valid_values_.end(), str_value) == valid_values_.end()) {
            std::string valid_list = "Valid values: ";
            for (size_t i = 0; i < valid_values_.size(); ++i) {
                if (i > 0) valid_list += ", ";
                valid_list += "\"" + valid_values_[i] + "\"";
            }
            errors.emplace_back(ValidationErrorType::RANGE_VIOLATION, path,
                             "Value not in allowed enum list",
                             valid_list,
                             "\"" + str_value + "\"");
            return false;
        }
        return true;
    }
    
    std::string get_name() const override { return "EnumValidator"; }
};

// Schema class
class Schema {
private:
    std::string name_;
    std::string version_;
    std::map<std::string, std::shared_ptr<SchemaElement>> properties_;
    std::map<std::string, std::shared_ptr<Validator>> validators_;
    std::shared_ptr<Schema> parent_schema_;
    std::vector<ValidationError> validation_errors_;
    
public:
    Schema(const std::string& name, const std::string& version = "1.0")
        : name_(name), version_(version) {}
    
    void add_property(const std::string& name, std::shared_ptr<SchemaElement> element) {
        properties_[name] = element;
    }
    
    void add_validator(const std::string& name, std::shared_ptr<Validator> validator) {
        validators_[name] = validator;
    }
    
    bool validate(const std::map<std::string, ConfigValue>& config,
                  std::vector<ValidationError>& errors) const {
        errors.clear();
        
        // Check required properties
        for (const auto& pair : properties_) {
            if (pair.second->required) {
                auto it = config.find(pair.second->name);
                if (it == config.end()) {
                    errors.emplace_back(ValidationErrorType::MISSING_REQUIRED,
                                     pair.second->name, "Required property is missing");
                }
            }
        }
        
        if (!errors.empty()) {
            return false;
        }
        
        // Validate each property
        for (const auto& pair : properties_) {
            auto it = config.find(pair.second->name);
            if (it != config.end()) {
                if (!pair.second->validate(it->second, errors, pair.second->name)) {
                    continue;
                }
            }
        }
        
        // Run custom validators
        for (const auto& pair : validators_) {
            if (!pair.second->validate(ConfigValue(), errors)) {
                continue;
            }
        }
        
        validation_errors_ = errors;
        return errors.empty();
    }
    
    const std::vector<ValidationError>& get_validation_errors() const {
        return validation_errors_;
    }
    
    std::string get_name() const { return name_; }
    std::string get_version() const { return version_; }
    
    bool has_property(const std::string& name) const {
        return properties_.find(name) != properties_.end();
    }
    
    std::shared_ptr<SchemaElement> get_property(const std::string& name) const {
        auto it = properties_.find(name);
        if (it != properties_.end()) {
            return it->second;
        }
        return nullptr;
    }
};

// Configuration validator class
class ConfigurationValidator {
private:
    std::map<std::string, std::shared_ptr<Schema>> schemas_;
    std::map<std::string, std::shared_ptr<Validator>> global_validators_;
    bool auto_fix_enabled_ = false;
    
public:
    void register_schema(const std::string& name, std::shared_ptr<Schema> schema) {
        schemas_[name] = schema;
    }
    
    void register_validator(const std::string& name, std::shared_ptr<Validator> validator) {
        global_validators_[name] = validator;
    }
    
    ValidationResult validate(const std::map<std::string, ConfigValue>& config,
                             const std::string& schema_name = "default") const {
        ValidationResult result;
        
        auto schema_it = schemas_.find(schema_name);
        if (schema_it != schemas_.end()) {
            std::vector<ValidationError> errors;
            if (!schema_it->second->validate(config, errors)) {
                result.errors = errors;
                result.valid = false;
            } else {
                result.valid = true;
            }
        } else {
            // No schema validation, just run global validators
            for (const auto& pair : global_validators_) {
                pair.second->validate(ConfigValue(), result.errors);
            }
            result.valid = result.errors.empty();
        }
        
        return result;
    }
    
    bool auto_fix(const std::map<std::string, ConfigValue>& config,
                  const std::string& schema_name = "default",
                  std::map<std::string, ConfigValue>& fixed_config) const {
        if (!auto_fix_enabled_) {
            return false;
        }
        
        fixed_config = config;
        auto schema_it = schemas_.find(schema_name);
        
        if (schema_it != schemas_.end()) {
            std::vector<ValidationError> errors;
            schema_it->second->validate(fixed_config, errors);
            
            // Try to fix common errors
            for (auto& error : errors) {
                if (error.type == ValidationErrorType::MISSING_REQUIRED && 
                    schema_it->second->has_property(error.path)) {
                    auto element = schema_it->second->get_property(error.path);
                    if (element && element->default_value.has_value()) {
                        fixed_config[error.path] = *element->default_value;
                    }
                }
            }
            
            // Validate again
            std::vector<ValidationError> remaining_errors;
            return schema_it->second->validate(fixed_config, remaining_errors) && 
                   remaining_errors.empty();
        }
        
        return true;
    }
    
    void enable_auto_fix(bool enabled) {
        auto_fix_enabled_ = enabled;
    }
    
    void set_schema_version(const std::string& schema_name, const std::string& version) {
        auto it = schemas_.find(schema_name);
        if (it != schemas_.end()) {
            it->second->version_ = version;
        }
    }
};

// Utility functions for configuration validation
namespace validation {
    inline std::string config_value_to_string(const ConfigValue& value) {
        std::stringstream ss;
        std::visit([&ss](const auto& val) {
            using T = std::decay_t<decltype(val)>;
            if constexpr (std::is_same_v<T, std::string>) {
                ss << "\"" << val << "\"";
            } else if constexpr (std::is_same_v<T, bool>) {
                ss << (val ? "true" : "false");
            } else {
                ss << val;
            }
        }, value);
        return ss.str();
    }
    
    inline std::shared_ptr<SchemaElement> boolean_element(const std::string& name, bool required = false) {
        return std::make_shared<SchemaElement>(name, SchemaElementType::BOOLEAN, required);
    }
    
    inline std::shared_ptr<SchemaElement> integer_element(const std::string& name, 
                                                         bool required = false,
                                                         int64_t min = std::numeric_limits<int64_t>::min(),
                                                         int64_t max = std::numeric_limits<int64_t>::max()) {
        auto element = std::make_shared<SchemaElement>(name, SchemaElementType::INTEGER, required);
        if (min != std::numeric_limits<int64::min()) {
            element->min_value = min;
        }
        if (max != std::numeric_limits<int64_t>::max()) {
            element->max_value = max;
        }
        return element;
    }
    
    inline std::shared_ptr<SchemaElement> double_element(const std::string& name, 
                                                         bool required = false,
                                                         double min = std::numeric_limits<double>::min(),
                                                         double max = std::numeric_limits<double>::max()) {
        auto element = std::make_shared<SchemaElement>(name, SchemaElementType::DOUBLE, required);
        if (min != std::numeric_limits<double>::min()) {
            element->min_value = min;
        }
        if (max != std::numeric_limits<double>::max()) {
            element->max_value = max;
        }
        return element;
    }
    
    inline std::shared_ptr<SchemaElement> string_element(const std::string& name, 
                                                         bool required = false,
                                                         size_t min_length = 0,
                                                         size_t max_length = std::numeric_limits<size_t>::max()) {
        auto element = std::make_shared<SchemaElement>(name, SchemaElementType::STRING, required);
        if (min_length > 0) {
            element->min_value = static_cast<int64_t>(min_length);
        }
        if (max_length != std::numeric_limits<size_t>::max()) {
            element->max_value = static_cast<int64_t>(max_length);
        }
        return element;
    }
    
    inline std::shared_ptr<SchemaElement> enum_element(const std::string& name,
                                                      const std::vector<std::string>& values,
                                                      bool required = false) {
        auto element = std::make_shared<SchemaElement>(name, SchemaElementType::ENUM, required);
        element->enum_values = values;
        return element;
    }
    
    inline std::shared_ptr<Validator> range_validator(const ConfigValue& min, const ConfigValue& max) {
        return std::make_shared<RangeValidator>(min, max);
    }
    
    inline std::shared_ptr<Validator> enum_validator(const std::vector<std::string>& values) {
        return std::make_shared<EnumValidator>(values);
    }
}

// Configuration validation result structure
struct ValidationResult {
    bool valid = true;
    std::vector<ValidationError> errors;
    std::map<std::string, ConfigValue> fixed_values;
    
    std::string get_summary() const {
        std::stringstream ss;
        ss << "ValidationResult: " << (valid ? "PASS" : "FAIL") << "; "
           << errors.size() << " errors";
        if (!fixed_values.empty()) {
            ss << "; " << fixed_values.size() << " values fixed";
        }
        return ss.str();
    }
    
    bool has_errors() const {
        return !valid && !errors.empty();
    }
    
    std::string get_error_string() const {
        std::stringstream ss;
        for (const auto& error : errors) {
            ss << error.to_string() << "\n";
        }
        return ss.str();
    }
};

} // namespace config
} // namespace nlm

#endif // NLM_CONFIG_VALIDATION_H