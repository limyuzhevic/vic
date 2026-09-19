#pragma once

#include <string>
#include <memory>
#include <vector>
#include <map>
#include <set>
#include <unordered_map>
#include <functional>
#include <variant>
#include <mutex>
#include <atomic>
#include <chrono>
#include <thread>
#include <sstream>
#include "ConfigError.hpp"
#include "ValidationRules.hpp"
#include "ConfigSchema.hpp"
#include "../Config/Config.hpp"

namespace nlm {

// Parameter type for type-safe access
enum class ParameterType {
    Integer,
    Integer64,
    Double,
    Bool,
    String,
    IntVector,
    DoubleVector,
    StringVector,
    Unknown
};

// Parameter conversion result
struct ParameterConversionResult {
    bool success = false;
    std::variant<int, int64_t, double, bool, std::string, std::vector<int>, std::vector<double>, std::vector<std::string>, std::monostate> value;
    std::string errorMessage;
    
    static ParameterConversionResult success(int value) {
        return {true, value, "", ""};
    }
    
    static ParameterConversionResult success(int64_t value) {
        return {true, value, "", ""};
    }
    
    static ParameterConversionResult success(double value) {
        return {true, value, "", ""};
    }
    
    static ParameterConversionResult success(bool value) {
        return {true, value, "", ""};
    }
    
    static ParameterConversionResult success(const std::string& value) {
        return {true, value, "", ""};
    }
    
    static ParameterConversionResult success(const std::vector<int>& value) {
        return {true, value, "", ""};
    }
    
    static ParameterConversionResult success(const std::vector<double>& value) {
        return {true, value, "", ""};
    }
    
    static ParameterConversionResult success(const std::vector<std::string>& value) {
        return {true, value, "", ""};
    }
    
    static ParameterConversionResult failure(const std::string& error) {
        return {false, std::monostate{}, error, ""};
    }
};

// Parameter metadata
struct ParameterMetadata {
    std::string name;
    std::string description;
    std::string unit;
    ParameterType type;
    std::any defaultValue;
    std::vector<ValidationRules::ValidationFunction> validators;
    bool required = false;
    bool isDynamic = false; // Can be changed at runtime
    std::string pluginName;
    std::string componentType;
    
    ParameterMetadata() = default;
    
    ParameterMetadata(const std::string& name,
                     const std::string& description = "",
                     const std::string& unit = "",
                     ParameterType type = ParameterType::Unknown,
                     const std::any& defaultValue = std::monostate{},
                     const std::vector<ValidationRules::ValidationFunction>& validators = {},
                     bool required = false,
                     bool isDynamic = false,
                     const std::string& pluginName = "",
                     const std::string& componentType = "")
        : name(name), description(description), unit(unit), type(type),
          defaultValue(defaultValue), validators(validators), required(required),
          isDynamic(isDynamic), pluginName(pluginName), componentType(componentType) {}
};

// Parameter resolver for type-safe parameter access
class ParameterResolver {
private:
    std::shared_ptr<Config> config_;
    std::map<std::string, ParameterMetadata> parameterMetadata_;
    std::map<std::string, std::function<ParameterConversionResult(const ConfigValue&)>> typeConverters_;
    bool validateOnAccess_ = true;
    
public:
    ParameterResolver(std::shared_ptr<Config> config = nullptr)
        : config_(config), validateOnAccess_(true) {
        initializeTypeConverters();
    }
    
    // Set configuration
    void setConfig(std::shared_ptr<Config> config) {
        config_ = config;
    }
    
    // Register parameter metadata
    void registerParameter(const ParameterMetadata& metadata) {
        parameterMetadata_[metadata.name] = metadata;
    }
    
    // Register parameter with type and default value
    void registerParameter(const std::string& name,
                          ParameterType type,
                          const std::any& defaultValue,
                          const std::string& description = "",
                          const std::string& unit = "",
                          bool required = false,
                          const std::vector<ValidationRules::ValidationFunction>& validators = {}) {
        ParameterMetadata metadata(name, description, unit, type, defaultValue, validators, required);
        registerParameter(metadata);
    }
    
    // Check if parameter exists
    bool hasParameter(const std::string& name) const {
        return parameterMetadata_.find(name) != parameterMetadata_.end();
    }
    
    // Get parameter metadata
    const ParameterMetadata* getParameterMetadata(const std::string& name) const {
        auto it = parameterMetadata_.find(name);
        return (it != parameterMetadata_.end()) ? &it->second : nullptr;
    }
    
    // Get all parameter names
    std::vector<std::string> getParameterNames() const {
        std::vector<std::string> names;
        names.reserve(parameterMetadata_.size());
        for (const auto& pair : parameterMetadata_) {
            names.push_back(pair.first);
        }
        return names;
    }
    
    // Get parameter type name
    std::string getParameterTypeName(const std::string& name) const {
        auto it = parameterMetadata_.find(name);
        if (it == parameterMetadata_.end()) {
            return "";
        }
        switch (it->second.type) {
            case ParameterType::Integer: return "int";
            case ParameterType::Integer64: return "int64_t";
            case ParameterType::Double: return "double";
            case ParameterType::Bool: return "bool";
            case ParameterType::String: return "std::string";
            case ParameterType::IntVector: return "std::vector<int>";
            case ParameterType::DoubleVector: return "std::vector<double>";
            case ParameterType::StringVector: return "std::vector<std::string>";
            case ParameterType::Unknown: default: return "unknown";
        }
        return "unknown";
    }
    
    // Type-safe parameter access methods
    
    // Get int parameter
    ParameterConversionResult getInt(const std::string& name, int defaultValue = 0) const {
        return getParameter<int>(name, defaultValue);
    }
    
    // Get int64_t parameter
    ParameterConversionResult getInt64(const std::string& name, int64_t defaultValue = 0) const {
        return getParameter<int64_t>(name, defaultValue);
    }
    
    // Get double parameter
    ParameterConversionResult getDouble(const std::string& name, double defaultValue = 0.0) const {
        return getParameter<double>(name, defaultValue);
    }
    
    // Get bool parameter
    ParameterConversionResult getBool(const std::string& name, bool defaultValue = false) const {
        return getParameter<bool>(name, defaultValue);
    }
    
    // Get string parameter
    ParameterConversionResult getString(const std::string& name, const std::string& defaultValue = "") const {
        return getParameter<std::string>(name, defaultValue);
    }
    
    // Get vector parameter
    ParameterConversionResult getIntVector(const std::string& name, const std::vector<int>& defaultValue = {}) const {
        return getParameter<std::vector<int>>(name, defaultValue);
    }
    
    ParameterConversionResult getDoubleVector(const std::string& name, const std::vector<double>& defaultValue = {}) const {
        return getParameter<std::vector<double>>(name, defaultValue);
    }
    
    ParameterConversionResult getStringVector(const std::string& name, const std::vector<std::string>& defaultValue = {}) const {
        return getParameter<std::vector<std::string>>(name, defaultValue);
    }
    
    // Generic get parameter with type conversion
    template<typename T>
    ParameterConversionResult getParameter(const std::string& name, const T& defaultValue) const {
        if (!config_) {
            return ParameterConversionResult::failure("Configuration is not set");
        }
        
        auto it = parameterMetadata_.find(name);
        if (it == parameterMetadata_.end()) {
            // Check if config has the key
            if (config_->has(name)) {
                // Try to convert existing value to requested type
                try {
                    std::optional<T> val = config_->get<T>(name);
                    if (val.has_value()) {
                        return ParameterConversionResult::success(val.value());
                    } else {
                        return ParameterConversionResult::success(defaultValue);
                    }
                } catch (...) {
                    return ParameterConversionResult::failure("Failed to convert existing value for parameter: " + name);
                }
            } else {
                // Parameter not found, use default
                return ParameterConversionResult::success(defaultValue);
            }
        }
        
        const auto& metadata = it->second;
        
        // Check if config has the key
        if (config_->has(name)) {
            auto configVal = config_->get<const ConfigValue&>(name);
            // Note: This is a simplified version - actual implementation would need
            // to handle the variant type properly
            return ParameterConversionResult::success(defaultValue);
        } else {
            // Use default value
            return ParameterConversionResult::success(defaultValue);
        }
    }
    
    // Set parameter with type checking
    bool setParameter(const std::string& name, const ConfigValue& value, bool validate = true) {
        if (!config_) {
            return false;
        }
        
        auto it = parameterMetadata_.find(name);
        if (it == parameterMetadata_.end()) {
            // Allow setting of non-registered parameters if validation is off
            if (!validateOnAccess_) {
                config_->set(name, value);
                return true;
            }
            return false;
        }
        
        const auto& metadata = it->second;
        
        // Validate against registered validators
        if (validate && validateOnAccess_) {
            ValidationResult validationResult = validateParameterValue(name, value);
            if (!validationResult.isValid) {
                return false;
            }
        }
        
        config_->set(name, value);
        return true;
    }
    
    // Validate parameter value
    ValidationResult validateParameterValue(const std::string& name, const ConfigValue& value) const {
        auto it = parameterMetadata_.find(name);
        if (it == parameterMetadata_.end()) {
            return ValidationResult::failure("Unknown parameter: " + name);
        }
        
        const auto& metadata = it->second;
        
        // Run validators
        for (const auto& validator : metadata.validators) {
            ValidationResult result = validator(value, nullptr);
            if (!result.isValid) {
                return result;
            }
        }
        
        return ValidationResult::success();
    }
    
    // Check if parameter value is valid
    bool isParameterValid(const std::string& name) const {
        if (!config_ || !config_->has(name)) {
            return false;
        }
        
        auto configVal = config_->get<const ConfigValue&>(name);
        ValidationResult result = validateParameterValue(name, configVal);
        return result.isValid;
    }
    
    // Enable/disable validation on access
    void setValidateOnAccess(bool validate) {
        validateOnAccess_ = validate;
    }
    
    bool isValidateOnAccess() const {
        return validateOnAccess_;
    }
    
    // Get configuration summary
    std::string getParameterSummary() const {
        std::ostringstream oss;
        oss << "ParameterResolver Summary:" << std::endl;
        oss << "  Registered parameters: " << parameterMetadata_.size() << std::endl;
        oss << "  Validation on access: " << (validateOnAccess_ ? "Enabled" : "Disabled") << std::endl;
        oss << "  Parameters:" << std::endl;
        
        for (const auto& pair : parameterMetadata_) {
            const auto& metadata = pair.second;
            oss << "    - " << metadata.name << " (" << getParameterTypeName(metadata.name) << ")";
            if (metadata.required) oss << " [REQUIRED]";
            if (metadata.isDynamic) oss << " [DYNAMIC]";
            oss << " - " << metadata.description;
            if (!metadata.unit.empty()) oss << " [" << metadata.unit << "]";
            oss << std::endl;
        }
        
        return oss.str();
    }
    
    // Clear all parameter metadata
    void clearParameters() {
        parameterMetadata_.clear();
    }
    
private:
    void initializeTypeConverters() {
        // Initialize type converters for proper type conversion
        typeConverters_["int"] = [this](const ConfigValue& value) -> ParameterConversionResult {
            try {
                std::optional<int> val = config_->get<int>(name);
                if (val.has_value()) {
                    return ParameterConversionResult::success(val.value());
                }
            } catch (...) {}
            return ParameterConversionResult::failure("Cannot convert to int");
        };
        
        typeConverters_["int64_t"] = [this](const ConfigValue& value) -> ParameterConversionResult {
            try {
                std::optional<int64_t> val = config_->get<int64_t>(name);
                if (val.has_value()) {
                    return ParameterConversionResult::success(val.value());
                }
            } catch (...) {}
            return ParameterConversionResult::failure("Cannot convert to int64_t");
        };
        
        typeConverters_["double"] = [this](const ConfigValue& value) -> ParameterConversionResult {
            try {
                std::optional<double> val = config_->get<double>(name);
                if (val.has_value()) {
                    return ParameterConversionResult::success(val.value());
                }
            } catch (...) {}
            return ParameterConversionResult::failure("Cannot convert to double");
        };
        
        typeConverters_["bool"] = [this](const ConfigValue& value) -> ParameterConversionResult {
            try {
                std::optional<bool> val = config_->get<bool>(name);
                if (val.has_value()) {
                    return ParameterConversionResult::success(val.value());
                }
            } catch (...) {}
            return ParameterConversionResult::failure("Cannot convert to bool");
        };
        
        typeConverters_["std::string"] = [this](const ConfigValue& value) -> ParameterConversionResult {
            try {
                std::optional<std::string> val = config_->get<std::string>(name);
                if (val.has_value()) {
                    return ParameterConversionResult::success(val.value());
                }
            } catch (...) {}
            return ParameterConversionResult::failure("Cannot convert to std::string");
        };
    }
};