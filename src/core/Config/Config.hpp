#pragma once

#include <string>
#include <memory>
#include <vector>
#include <variant>
#include <optional>
#include <any>

namespace nlm {

// Forward declarations
class Config;
class ConfigSchema;

// Configuration value types (extended)
using ConfigValue = std::variant<
    int,
    int64_t,
    double,
    bool,
    std::string,
    std::vector<int>,
    std::vector<double>,
    std::vector<std::string>
>;

// Configuration source
enum class ConfigSource {
    Default,
    File,
    CommandLine,
    Runtime
};

// Parameter types for enhanced schema
enum class ParamType {
    Int,
    Int64,
    Double,
    Float,
    Bool,
    String,
    VectorInt,
    VectorDouble,
    VectorString
};

// Parameter range definition
struct ParameterRange {
    ParamType type;
    double min;  // For continuous types
    double max;  // For continuous types
    std::vector<int> intOptions;  // For discrete integer options
    std::string unit;             // Unit of measurement
    
    ParameterRange() : type(ParamType::Double), min(0.0), max(1.0) {}
    
    ParameterRange(ParamType t, double minVal, double maxVal, const std::string& unit = "")
        : type(t), min(minVal), max(maxVal), unit(unit) {}
    
    ParameterRange(ParamType t, const std::vector<int>& options, const std::string& unit = "")
        : type(t), min(0.0), max(0.0), intOptions(options), unit(unit) {}
    
    bool contains(double value) const {
        if (type == ParamType::Int || type == ParamType::Int64) {
            int intVal = static_cast<int>(value);
            if (!intOptions.empty()) {
                return std::find(intOptions.begin(), intOptions.end(), intVal) != intOptions.end();
            }
            return intVal >= min && intVal <= max;
        }
        return value >= min && value <= max;
    }
    
    std::string getDescription() const {
        if (!intOptions.empty()) {
            std::string result = "One of: ";
            for (size_t i = 0; i < intOptions.size(); ++i) {
                if (i > 0) result += ", ";
                result += std::to_string(intOptions[i]);
            }
            return result;
        }
        return "Range: [" + std::to_string(min) + ", " + std::to_string(max) + "]";
    }
};

// Parameter metadata
struct ParameterMetadata {
    std::string name;
    std::string displayName;
    std::string description;
    ParamType type;
    std::any defaultValue;
    ParameterRange range;
    std::string group;
    bool required;
    int order;  // For UI ordering
    
    ParameterMetadata() : type(ParamType::Double), required(false), order(0) {}
    
    ParameterMetadata(const std::string& n, const std::string& d, const std::string& desc = "",
                      ParamType t = ParamType::Double, const std::any& def = std::any(),
                      const std::string& grp = "general", bool req = false, int ord = 0)
        : name(n), displayName(d), description(desc), type(t), defaultValue(def), group(grp),
          required(req), order(ord) {}
    
    std::string getTypeName() const {
        switch (type) {
            case ParamType::Int: return "int";
            case ParamType::Int64: return "int64";
            case ParamType::Double: return "double";
            case ParamType::Float: return "float";
            case ParamType::Bool: return "bool";
            case ParamType::String: return "string";
            case ParamType::VectorInt: return "vector<int>";
            case ParamType::VectorDouble: return "vector<double>";
            case ParamType::VectorString: return "vector<string>";
            default: return "unknown";
        }
    }
    
    std::string getDefaultValueString() const {
        if (!defaultValue.has_value()) return "";
        
        if (type == ParamType::String) {
            return "\"" + std::any_cast<std::string>(defaultValue) + "\"";
        } else if (type == ParamType::Bool) {
            return std::any_cast<bool>(defaultValue) ? "true" : "false";
        } else if (type == ParamType::VectorInt) {
            const auto& vec = std::any_cast<std::vector<int>>(defaultValue);
            std::string result = "[";
            for (size_t i = 0; i < vec.size(); ++i) {
                if (i > 0) result += ", ";
                result += std::to_string(vec[i]);
            }
            return result + "]";
        } else if (type == ParamType::VectorDouble) {
            const auto& vec = std::any_cast<std::vector<double>>(defaultValue);
            std::string result = "[";
            for (size_t i = 0; i < vec.size(); ++i) {
                if (i > 0) result += ", ";
                result += std::to_string(vec[i]);
            }
            return result + "]";
        } else if (type == ParamType::VectorString) {
            const auto& vec = std::any_cast<std::vector<std::string>>(defaultValue);
            std::string result = "[";
            for (size_t i = 0; i < vec.size(); ++i) {
                if (i > 0) result += ", ";
                result += "\"" + vec[i] + "\"";
            }
            return result + "]";
        } else {
            return std::to_string(std::any_cast<double>(defaultValue));
        }
    }
    
    double getNumericValue(const std::any& value) const {
        if (type == ParamType::Int) return static_cast<double>(std::any_cast<int>(value));
        if (type == ParamType::Int64) return static_cast<double>(std::any_cast<int64_t>(value));
        if (type == ParamType::Double) return std::any_cast<double>(value);
        if (type == ParamType::Float) return static_cast<double>(std::any_cast<float>(value));
        if (type == ParamType::Bool) return std::any_cast<bool>(value) ? 1.0 : 0.0;
        return 0.0;
    }
};

// Validation result
struct ValidationResult {
    bool isValid;
    std::vector<std::string> errors;
    std::vector<std::string> warnings;
    
    ValidationResult() : isValid(true) {}
    
    void addError(const std::string& error) {
        isValid = false;
        errors.push_back(error);
    }
    
    void addWarning(const std::string& warning) {
        warnings.push_back(warning);
    }
    
    std::string getErrors() const {
        std::string result;
        for (const auto& error : errors) {
            if (!result.empty()) result += "; ";
            result += error;
        }
        return result;
    }
    
    std::string getWarnings() const {
        std::string result;
        for (const auto& warning : warnings) {
            if (!result.empty()) result += "; ";
            result += warning;
        }
        return result;
    }
};

// Configuration schema class
class ConfigSchema {
private:
    std::map<std::string, ParameterMetadata> parameters;
    std::map<std::string, std::vector<std::string>> groups;
    
public:
    ConfigSchema() {}
    
    void addParameter(const ParameterMetadata& metadata) {
        parameters[metadata.name] = metadata;
        groups[metadata.group].push_back(metadata.name);
    }
    
    bool hasParameter(const std::string& name) const {
        return parameters.find(name) != parameters.end();
    }
    
    const ParameterMetadata* getParameter(const std::string& name) const {
        auto it = parameters.find(name);
        if (it != parameters.end()) {
            return &it->second;
        }
        return nullptr;
    }
    
    std::vector<std::string> getGroupNames() const {
        std::vector<std::string> result;
        for (const auto& group : groups) {
            result.push_back(group.first);
        }
        return result;
    }
    
    std::vector<std::string> getParametersInGroup(const std::string& group) const {
        auto it = groups.find(group);
        if (it != groups.end()) {
            return it->second;
        }
        return {};
    }
    
    std::vector<ParameterMetadata> getAllParameters() const {
        std::vector<ParameterMetadata> result;
        for (const auto& pair : parameters) {
            result.push_back(pair.second);
        }
        return result;
    }
    
    ValidationResult validateParameter(const std::string& name, const std::any& value) const {
        ValidationResult result;
        
        const auto* param = getParameter(name);
        if (!param) {
            result.addError("Unknown parameter: " + name);
            return result;
        }
        
        // Type validation
        if (!validateType(value, param->type)) {
            result.addError("Parameter '" + name + "' has wrong type. Expected " + param->getTypeName() + ".");
        }
        
        // Range validation
        if (param->type != ParamType::String && param->type != ParamType::Bool) {
            double numericValue = param->getNumericValue(value);
            if (!param->range.contains(numericValue)) {
                result.addError("Parameter '" + name + "' has value " + std::to_string(numericValue) +
                              " which is outside allowed range. " + param->range.getDescription());
            }
        }
        
        return result;
    }
    
    ValidationResult validateConfiguration(const std::map<std::string, std::any>& config) const {
        ValidationResult result;
        
        for (const auto& param : parameters) {
            const auto& name = param.first;
            const auto& metadata = param.second;
            
            auto it = config.find(name);
            if (it == config.end()) {
                if (metadata.required) {
                    result.addError("Required parameter '" + name + "' is missing.");
                }
                continue;
            }
            
            ValidationResult paramResult = validateParameter(name, it->second);
            if (!paramResult.isValid) {
                for (const auto& error : paramResult.errors) {
                    result.addError(error);
                }
            }
            for (const auto& warning : paramResult.warnings) {
                result.addWarning(warning);
            }
        }
        
        return result;
    }
    
    const std::any& getDefaultValue(const std::string& name) const {
        const auto* param = getParameter(name);
        static const std::any empty;
        if (param) {
            return param->defaultValue;
        }
        return empty;
    }
    
    std::string getParameterDescription(const std::string& name) const {
        const auto* param = getParameter(name);
        if (!param) return "";
        
        std::string result = "Name: " + param->name + "\n";
        result += "Display Name: " + param->displayName + "\n";
        result += "Description: " + param->description + "\n";
        result += "Type: " + param->getTypeName() + "\n";
        result += "Default: " + param->getDefaultValueString() + "\n";
        result += "Range: " + param->range.getDescription() + "\n";
        result += "Group: " + param->group + "\n";
        return result;
    }
private:
    bool validateType(const std::any& value, ParamType expectedType) const {
        if (!value.has_value()) return false;
        
        if (expectedType == ParamType::Int) return value.type() == typeid(int);
        if (expectedType == ParamType::Int64) return value.type() == typeid(int64_t);
        if (expectedType == ParamType::Double) return value.type() == typeid(double);
        if (expectedType == ParamType::Float) return value.type() == typeid(float);
        if (expectedType == ParamType::Bool) return value.type() == typeid(bool);
        if (expectedType == ParamType::String) return value.type() == typeid(std::string);
        if (expectedType == ParamType::VectorInt) return value.type() == typeid(std::vector<int>);
        if (expectedType == ParamType::VectorDouble) return value.type() == typeid(std::vector<double>);
        if (expectedType == ParamType::VectorString) return value.type() == typeid(std::vector<std::string>);
        
        return false;
    }
};

// Main configuration class
class Config {
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
public:
    Config();
    ~Config();
    
    // Disable copying, enable moving
    Config(const Config&) = delete;
    Config& operator=(const Config&) = delete;
    Config(Config&&) noexcept;
    Config& operator=(Config&&) noexcept;
    
    // Core configuration methods (backward compatible)
    bool loadFromFile(const std::string& filepath);
    bool loadFromArgs(int argc, char** argv);
    bool saveToFile(const std::string& filepath) const;
    
    // Get values (backward compatible)
    template<typename T>
    std::optional<T> get(const std::string& key) const;
    
    template<typename T>
    T getOr(const std::string& key, const T& defaultValue) const;
    
    // Set values (backward compatible)
    void set(const std::string& key, const ConfigValue& value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, const std::string& value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, int value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, double value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, bool value, ConfigSource source = ConfigSource::Runtime);
    
    // Check existence
    bool has(const std::string& key) const;
    
    // Remove key
    void remove(const std::string& key);
    
    // Get all keys
    std::vector<std::string> getKeys() const;
    
    // Clear all
    void clear();
    
    // Get configuration summary
    std::string summary() const;
    
    // Enhanced configuration methods (NEW)
    bool set(const std::string& key, const std::any& value);
    bool isValid(const std::string& key) const;
    ValidationResult validate() const;
    ValidationResult validate(const std::string& key) const;
    std::string getValidationErrors(const std::string& key) const;
    std::string getValidationWarnings(const std::string& key) const;
    
    // Schema and group methods
    void setSchema(const std::shared_ptr<ConfigSchema>& schema);
    const std::shared_ptr<ConfigSchema>& getSchema() const;
    
    std::vector<std::string> getGroups() const;
    std::vector<std::string> getParametersInGroup(const std::string& group) const;
    std::string getParameterDescription(const std::string& key) const;
    
    // Template configuration loading
    bool loadFromTemplate(const std::string& templateName);
    static std::vector<std::string> getAvailableTemplates() {
        return {"simple_brain", "agent_learning", "research", "production"};
    }
    
    // Python convenience methods
    template<typename T>
    T getOr(const std::string& key, const T& defaultValue) const { return getOr<T>(key, defaultValue); }
private:
    void initializeDefaults();
    std::string anyToString(const std::any& value) const;
    bool validateType(const std::string& key, const std::any& value) const;
};

} // namespace nlm
