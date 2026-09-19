#include "ParameterResolver.hpp"
#include "ConfigError.hpp"
#include <any>
#include <typeinfo>
#include <sstream>
#include <algorithm>
#include <stdexcept>

namespace nlm {
    namespace ParameterResolverImpl {
        
        // Helper functions for type conversion
        inline std::string getTypeName(ParameterType type) {
            switch (type) {
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
        
        // Type conversion utilities
        inline std::optional<int> anyToInt(const std::any& value) {
            if (value.type() == typeid(int)) {
                return std::any_cast<int>(value);
            } else if (value.type() == typeid(int64_t)) {
                return static_cast<int>(std::any_cast<int64_t>(value));
            } else if (value.type() == typeid(double)) {
                double val = std::any_cast<double>(value);
                if (val >= std::numeric_limits<int>::lowest() && val <= std::numeric_limits<int>::max()) {
                    return static_cast<int>(val);
                }
            } else if (value.type() == typeid(bool)) {
                return std::any_cast<bool>(value) ? 1 : 0;
            }
            return std::nullopt;
        }
        
        inline std::optional<double> anyToDouble(const std::any& value) {
            if (value.type() == typeid(int)) {
                return static_cast<double>(std::any_cast<int>(value));
            } else if (value.type() == typeid(int64_t)) {
                return static_cast<double>(std::any_cast<int64_t>(value));
            } else if (value.type() == typeid(double)) {
                return std::any_cast<double>(value);
            } else if (value.type() == typeid(bool)) {
                return std::any_cast<bool>(value) ? 1.0 : 0.0;
            } else if (value.type() == typeid(std::string)) {
                double val;
                const std::string& str = std::any_cast<std::string>(value);
                try {
                    size_t pos;
                    val = std::stod(str, pos);
                    if (pos == str.length()) {
                        return val;
                    }
                } catch (...) {}
            }
            return std::nullopt;
        }
        
        inline std::optional<bool> anyToBool(const std::any& value) {
            if (value.type() == typeid(int)) {
                return std::any_cast<int>(value) != 0;
            } else if (value.type() == typeid(int64_t)) {
                return std::any_cast<int64_t>(value) != 0;
            } else if (value.type() == typeid(double)) {
                return std::any_cast<double>(value) != 0.0;
            } else if (value.type() == typeid(bool)) {
                return std::any_cast<bool>(value);
            } else if (value.type() == typeid(std::string)) {
                const std::string& str = std::any_cast<std::string>(value);
                return (str == "true" || str == "1" || str == "yes" || str == "on");
            }
            return std::nullopt;
        }
        
        inline std::optional<std::string> anyToString(const std::any& value) {
            if (value.type() == typeid(int)) {
                return std::to_string(std::any_cast<int>(value));
            } else if (value.type() == typeid(int64_t)) {
                return std::to_string(std::any_cast<int64_t>(value));
            } else if (value.type() == typeid(double)) {
                double val = std::any_cast<double>(value);
                return std::to_string(val);
            } else if (value.type() == typeid(bool)) {
                return std::any_cast<bool>(value) ? "true" : "false";
            } else if (value.type() == typeid(std::string)) {
                return std::any_cast<std::string>(value);
            } else if (value.type() == typeid(std::vector<int>)) {
                const auto& vec = std::any_cast<std::vector<int>>(value);
                std::ostringstream oss;
                oss << "[";
                for (size_t i = 0; i < vec.size(); ++i) {
                    if (i > 0) oss << ", ";
                    oss << vec[i];
                }
                oss << "]";
                return oss.str();
            } else if (value.type() == typeid(std::vector<double>)) {
                const auto& vec = std::any_cast<std::vector<double>>(value);
                std::ostringstream oss;
                oss << "[";
                for (size_t i = 0; i < vec.size(); ++i) {
                    if (i > 0) oss << ", ";
                    oss << vec[i];
                }
                oss << "]";
                return oss.str();
            } else if (value.type() == typeid(std::vector<std::string>)) {
                const auto& vec = std::any_cast<std::vector<std::string>>(value);
                std::ostringstream oss;
                oss << "[";
                for (size_t i = 0; i < vec.size(); ++i) {
                    if (i > 0) oss << ", ";
                    oss << "\"" << vec[i] << "\"";
                }
                oss << "]";
                return oss.str();
            }
            return std::nullopt;
        }
        
    } // namespace ParameterResolverImpl
    
    // Implementation of ParameterResolver methods
    
    ParameterResolver::ParameterResolver(std::shared_ptr<Config> config)
        : config_(config), validateOnAccess_(true) {
        initializeTypeConverters();
    }
    
    // Set configuration
    void ParameterResolver::setConfig(std::shared_ptr<Config> config) {
        config_ = config;
    }
    
    // Register parameter metadata
    void ParameterResolver::registerParameter(const ParameterMetadata& metadata) {
        parameterMetadata_[metadata.name] = metadata;
    }
    
    // Register parameter with type and default value
    void ParameterResolver::registerParameter(const std::string& name,
                                              ParameterType type,
                                              const std::any& defaultValue,
                                              const std::string& description,
                                              const std::string& unit,
                                              bool required,
                                              const std::vector<ValidationRules::ValidationFunction>& validators) {
        ParameterMetadata metadata(name, description, unit, type, defaultValue, validators, required);
        registerParameter(metadata);
    }
    
    // Check if parameter exists
    bool ParameterResolver::hasParameter(const std::string& name) const {
        return parameterMetadata_.find(name) != parameterMetadata_.end();
    }
    
    // Get parameter metadata
    const ParameterMetadata* ParameterResolver::getParameterMetadata(const std::string& name) const {
        auto it = parameterMetadata_.find(name);
        return (it != parameterMetadata_.end()) ? &it->second : nullptr;
    }
    
    // Get all parameter names
    std::vector<std::string> ParameterResolver::getParameterNames() const {
        std::vector<std::string> names;
        names.reserve(parameterMetadata_.size());
        for (const auto& pair : parameterMetadata_) {
            names.push_back(pair.first);
        }
        return names;
    }
    
    // Get parameter type name
    std::string ParameterResolver::getParameterTypeName(const std::string& name) const {
        auto it = parameterMetadata_.find(name);
        if (it == parameterMetadata_.end()) {
            return "";
        }
        return ParameterResolverImpl::getTypeName(it->second.type);
    }
    
    // Type-safe parameter access methods
    
    // Get int parameter
    ParameterConversionResult ParameterResolver::getInt(const std::string& name, int defaultValue) const {
        return getParameter<int>(name, defaultValue);
    }
    
    // Get int64_t parameter
    ParameterConversionResult ParameterResolver::getInt64(const std::string& name, int64_t defaultValue) const {
        return getParameter<int64_t>(name, defaultValue);
    }
    
    // Get double parameter
    ParameterConversionResult ParameterResolver::getDouble(const std::string& name, double defaultValue) const {
        return getParameter<double>(name, defaultValue);
    }
    
    // Get bool parameter
    ParameterConversionResult ParameterResolver::getBool(const std::string& name, bool defaultValue) const {
        return getParameter<bool>(name, defaultValue);
    }
    
    // Get string parameter
    ParameterConversionResult ParameterResolver::getString(const std::string& name, const std::string& defaultValue) const {
        return getParameter<std::string>(name, defaultValue);
    }
    
    // Get vector parameter
    ParameterConversionResult ParameterResolver::getIntVector(const std::string& name, const std::vector<int>& defaultValue) const {
        return getParameter<std::vector<int>>(name, defaultValue);
    }
    
    ParameterConversionResult ParameterResolver::getDoubleVector(const std::string& name, const std::vector<double>& defaultValue) const {
        return getParameter<std::vector<double>>(name, defaultValue);
    }
    
    ParameterConversionResult ParameterResolver::getStringVector(const std::string& name, const std::vector<std::string>& defaultValue) const {
        return getParameter<std::vector<std::string>>(name, defaultValue);
    }
    
    // Generic get parameter with type conversion
    template<typename T>
    ParameterConversionResult ParameterResolver::getParameter(const std::string& name, const T& defaultValue) const {
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
            // For now, return default value - actual implementation would need
            // proper conversion from ConfigValue to T
            return ParameterConversionResult::success(defaultValue);
        } else {
            // Use default value
            return ParameterConversionResult::success(defaultValue);
        }
    }
    
    // Set parameter with type checking
    bool ParameterResolver::setParameter(const std::string& name, const ConfigValue& value, bool validate) {
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
    ValidationResult ParameterResolver::validateParameterValue(const std::string& name, const ConfigValue& value) const {
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
    bool ParameterResolver::isParameterValid(const std::string& name) const {
        if (!config_ || !config_->has(name)) {
            return false;
        }
        
        auto configVal = config_->get<const ConfigValue&>(name);
        ValidationResult result = validateParameterValue(name, configVal);
        return result.isValid;
    }
    
    // Enable/disable validation on access
    void ParameterResolver::setValidateOnAccess(bool validate) {
        validateOnAccess_ = validate;
    }
    
    bool ParameterResolver::isValidateOnAccess() const {
        return validateOnAccess_;
    }
    
    // Get configuration summary
    std::string ParameterResolver::getParameterSummary() const {
        std::ostringstream oss;
        oss << "ParameterResolver Summary:" << std::endl;
        oss << "  Registered parameters: " << parameterMetadata_.size() << std::endl;
        oss << "  Validation on access: " << (validateOnAccess_ ? "Enabled" : "Disabled") << std::endl;
        oss << "  Parameters:" << std::endl;
        
        for (const auto& pair : parameterMetadata_) {
            const auto& metadata = pair.second;
            oss << "    - " << metadata.name << " (" << ParameterResolverImpl::getTypeName(metadata.type) << ")";
            if (metadata.required) oss << " [REQUIRED]";
            if (metadata.isDynamic) oss << " [DYNAMIC]";
            oss << " - " << metadata.description;
            if (!metadata.unit.empty()) oss << " [" << metadata.unit << "]";
            oss << std::endl;
        }
        
        return oss.str();
    }
    
    // Clear all parameter metadata
    void ParameterResolver::clearParameters() {
        parameterMetadata_.clear();
    }
    
    // Initialize type converters
    void ParameterResolver::initializeTypeConverters() {
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
    
} // namespace nlm