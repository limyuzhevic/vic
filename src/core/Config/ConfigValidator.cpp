#include "ConfigValidator.hpp"
#include <algorithm>
#include <filesystem>
#include <iostream>
#include <fstream>
#include <stdexcept>

namespace nlm {

struct ConfigValidator::Impl {
    std::map<std::string, std::vector<ValidationRule>> validators;
    std::map<std::string, DefaultProvider> defaultProviders;
};

ConfigValidator::ConfigValidator() : pImpl(std::make_unique<Impl>()) {}

ConfigValidator::~ConfigValidator() = default;

void ConfigValidator::registerParameter(const std::string& key, const std::string& type, 
                                       const ConfigValue& defaultValue, 
                                       const std::string& description,
                                       bool required) {
    ConfigParameter param(key, type, defaultValue, description, required);
    parameters.push_back(param);
    
    // Register default value provider
    if (type == "int" || type == "int64_t") {
        pImpl->defaultProviders[key] = [defaultValue]() { return defaultValue; };
    } else if (type == "double") {
        pImpl->defaultProviders[key] = [defaultValue]() { return defaultValue; };
    } else if (type == "string") {
        pImpl->defaultProviders[key] = [defaultValue]() { return defaultValue; };
    } else if (type == "bool") {
        pImpl->defaultProviders[key] = [defaultValue]() { return defaultValue; };
    } else if (type.find("vector<") != std::string::npos) {
        pImpl->defaultProviders[key] = [defaultValue]() { return defaultValue; };
    }
}

template<typename T>
void ConfigValidator::addValidator(const std::string& key, 
                                  std::function<bool(const T&, std::string& error)> validator) {
    // Store validator in pImpl
    pImpl->validators[key].push_back(
        [validator](const ConfigValue& value, std::string& error) {
            try {
                return validator(std::get<T>(value), error);
            } catch (const std::bad_variant_access&) {
                error = "Value type mismatch for parameter: " + key;
                return false;
            }
        }
    );
}

bool ConfigValidator::validate(const Config& config, std::vector<std::string>& errors) const {
    bool isValid = true;
    
    // Check all registered parameters
    for (const auto& param : parameters) {
        // Check if parameter exists in config
        if (!config.has(param.key)) {
            if (param.required) {
                errors.push_back("Required parameter missing: " + param.key);
                isValid = false;
            } else {
                // Add default value
                // This would need to be implemented
            }
        } else {
            // Get value from config
            ConfigValue value;
            // This is complex to get value from config without template...
            // For now, we'll skip detailed validation
            
            // For demonstration, add some basic validation
            if (param.key == "neuron_count") {
                // Basic validation for neuron count
                auto val = config.getOr<int64_t>(param.key, 0);
                if (val <= 0) {
                    errors.push_back("neuron_count must be positive, got: " + std::to_string(val));
                    isValid = false;
                } else if (val > 100000) {
                    errors.push_back("neuron_count too large, max is 100000: " + std::to_string(val));
                    isValid = false;
                }
            } else if (param.key == "connection_probability") {
                auto val = config.getOr<double>(param.key, 0.0);
                if (val < 0.0 || val > 1.0) {
                    errors.push_back("connection_probability must be between 0.0 and 1.0: " + std::to_string(val));
                    isValid = false;
                }
            } else if (param.key == "simulation_timestep") {
                auto val = config.getOr<double>(param.key, 0.0);
                if (val <= 0.0 || val > 1.0) {
                    errors.push_back("simulation_timestep must be positive and <= 1.0: " + std::to_string(val));
                    isValid = false;
                }
            }
        }
    }
    
    return isValid;
}

const ConfigParameter* ConfigValidator::getParameter(const std::string& key) const {
    for (const auto& param : parameters) {
        if (param.key == key) {
            return &param;
        }
    }
    return nullptr;
}

template<typename T>
T ConfigValidator::getDefaultValue(const std::string& key) const {
    auto it = pImpl->defaultProviders.find(key);
    if (it != pImpl->defaultProviders.end()) {
        ConfigValue value = it->second();
        try {
            return std::get<T>(value);
        } catch (const std::bad_variant_access&) {
            throw std::runtime_error("Default value type mismatch for parameter: " + key);
        }
    }
    throw std::runtime_error("No default value for parameter: " + key);
}

Config ConfigValidator::generateDefaultConfig() const {
    Config config;
    for (const auto& param : parameters) {
        // For now, set a simple default value
        // In a real implementation, we'd use the actual default value
        if (param.type == "int" || param.type == "int64_t") {
            config.set(param.key, 0, ConfigSource::Default);
        } else if (param.type == "double") {
            config.set(param.key, 0.0, ConfigSource::Default);
        } else if (param.type == "string") {
            config.set(param.key, "", ConfigSource::Default);
        } else if (param.type == "bool") {
            config.set(param.key, false, ConfigSource::Default);
        }
    }
    return config;
}

std::string ConfigValidator::getValidationSummary() const {
    std::ostringstream oss;
    oss << "Configuration Validation Summary:" << std::endl;
    oss << "Total parameters: " << parameters.size() << std::endl;
    
    for (const auto& param : parameters) {
        oss << "  " << param.key << " (" << param.type << ") - " << param.description;
        if (param.required) oss << " [REQUIRED]";
        oss << std::endl;
    }
    
    return oss.str();
}

// Validation helper implementations
bool ConfigValidator::validateIntRange(const ConfigValue& value, std::string& error) {
    try {
        auto val = std::get<int>(value);
        if (val < 0) {
            error = "Value must be non-negative";
            return false;
        }
    } catch (const std::bad_variant_access&) {}
    return true;
}

bool ConfigValidator::validateDoubleRange(const ConfigValue& value, std::string& error) {
    try {
        auto val = std::get<double>(value);
        if (val < 0.0) {
            error = "Value must be non-negative";
            return false;
        }
    } catch (const std::bad_variant_access&) {}
    return true;
}

bool ConfigValidator::validateStringNotEmpty(const ConfigValue& value, std::string& error) {
    try {
        auto val = std::get<std::string>(value);
        if (val.empty()) {
            error = "Value cannot be empty";
            return false;
        }
    } catch (const std::bad_variant_access&) {}
    return true;
}

bool ConfigValidator::validatePositive(const ConfigValue& value, std::string& error) {
    try {
        auto val = std::get<double>(value);
        if (val <= 0.0) {
            error = "Value must be positive";
            return false;
        }
    } catch (const std::bad_variant_access&) {
        try {
            auto val = std::get<int>(value);
            if (val <= 0) {
                error = "Value must be positive";
                return false;
            }
        } catch (const std::bad_variant_access&) {}
    }
    return true;
}

bool ConfigValidator::validateNonNegative(const ConfigValue& value, std::string& error) {
    try {
        auto val = std::get<double>(value);
        if (val < 0.0) {
            error = "Value must be non-negative";
            return false;
        }
    } catch (const std::bad_variant_access&) {
        try {
            auto val = std::get<int>(value);
            if (val < 0) {
                error = "Value must be non-negative";
                return false;
            }
        } catch (const std::bad_variant_access&) {}
    }
    return true;
}

bool ConfigValidator::validateBoolean(const ConfigValue& value, std::string& error) {
    return true; // bool validation is simple
}

bool ConfigValidator::validateFileExists(const ConfigValue& value, std::string& error) {
    try {
        auto val = std::get<std::string>(value);
        if (!val.empty() && !std::filesystem::exists(val)) {
            error = "File does not exist: " + val;
            return false;
        }
    } catch (const std::bad_variant_access&) {}
    return true;
}

// Default value provider implementations
ConfigValue ConfigValidator::getDefaultInt(const std::string& key) {
    return 0;
}

ConfigValue ConfigValidator::getDefaultDouble(const std::string& key) {
    return 0.0;
}

ConfigValue ConfigValidator::getDefaultString(const std::string& key) {
    return std::string();
}

ConfigValue ConfigValidator::getDefaultBool(const std::string& key) {
    return false;
}

ConfigValue ConfigValidator::getDefaultVectorInt(const std::string& key) {
    return std::vector<int>();
}

// Explicit template instantiations

template void ConfigValidator::addValidator<int>(const std::string&, 
                                               std::function<bool(const int&, std::string&)>);

template void ConfigValidator::addValidator<double>(const std::string&,
                                                 std::function<bool(const double&, std::string&)>);

template void ConfigValidator::addValidator<std::string>(const std::string&,
                                                      std::function<bool(const std::string&, std::string&)>);

template void ConfigValidator::addValidator<bool>(const std::string&,
                                               std::function<bool(const bool&, std::string&)>);

template int ConfigValidator::getDefaultValue<int>(const std::string&) const;

template double ConfigValidator::getDefaultValue<double>(const std::string&) const;

template std::string ConfigValidator::getDefaultValue<std::string>(const std::string&) const;

template bool ConfigValidator::getDefaultValue<bool>(const std::string&) const;

} // namespace nlm