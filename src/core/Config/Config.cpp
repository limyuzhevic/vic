#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <cassert>
#include <cmath>
#include <iomanip>
#include <iostream>

namespace nlm {

// Implementation of Config class
struct Config::Impl {
    std::vector<ConfigEntry> entries;
    std::unique_ptr<ConfigSchema> schema;
};

Config::Config() : pImpl(std::make_unique<Impl>()) {
    // Initialize with default schema if none exists
    if (!schema_) {
        schema_ = std::make_unique<ConfigSchema>("Default", "Default configuration schema");
    }
}

Config::~Config() = default;

Config::Config(Config&&) noexcept = default;

Config& Config::operator=(Config&&) noexcept = default;

void Config::setSchema(const ConfigSchema& schema) {
    schema_ = std::make_unique<ConfigSchema>(schema);
    validationDirty_ = true;
}

const ConfigSchema* Config::getSchema() const {
    return schema_.get();
}

bool Config::loadFromFile(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    std::string line;
    while (std::getline(file, line)) {
        line = trim(line);
        if (line.empty() || line[0] == '#' || line[0] == '/') {
            continue;
        }
        
        size_t pos = line.find('=');
        if (pos != std::string::npos) {
            std::string key = trim(line.substr(0, pos));
            std::string value = trim(line.substr(pos + 1));
            
            if (value.size() >= 2 && 
                ((value.front() == '"' && value.back() == '"') ||
                 (value.front() == '\'' && value.back() == '\''))) {
                value = value.substr(1, value.size() - 2);
            }
            
            set(key, value, ConfigSource::File);
        }
    }
    
    validationDirty_ = true;
    return true;
}

bool Config::loadFromArgs(int argc, char** argv) {
    for (int i = 1; i < argc; ++i) {
        std::string arg(argv[i]);
        
        if (arg.substr(0, 2) == "--") {
            size_t pos = arg.find('=');
            if (pos != std::string::npos) {
                std::string key = arg.substr(2, pos - 2);
                std::string value = arg.substr(pos + 1);
                set(key, value, ConfigSource::CommandLine);
            }
        } else if (arg[0] == '-' && i + 1 < argc) {
            std::string key = arg.substr(1);
            std::string value = argv[++i];
            set(key, value, ConfigSource::CommandLine);
        }
    }
    validationDirty_ = true;
    return true;
}

bool Config::loadFromPythonDict(const std::map<std::string, std::any>& pythonDict) {
    bool success = true;
    for (const auto& pair : pythonDict) {
        if (!setFromPythonValue(pair.first, pair.second)) {
            success = false;
        }
    }
    if (success) {
        validationDirty_ = true;
    }
    return success;
}

bool Config::saveToFile(const std::string& filepath) const {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    for (const auto& entry : pImpl->entries) {
        file << "# " << entry.description << "\n";
        file << entry.key << " = " << formatValue(entry.value) << "\n";
    }
    
    return true;
}

template<typename T>
std::optional<T> Config::get(const std::string& key) const {
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    
    if (it == pImpl->entries.end()) {
        return std::nullopt;
    }
    
    try {
        return std::get<T>(it->value);
    } catch (const std::bad_variant_access&) {
        return std::nullopt;
    }
}

template<typename T>
T Config::getOr(const std::string& key, const T& defaultValue) const {
    auto val = get<T>(key);
    return val.has_value() ? val.value() : defaultValue;
}

void Config::set(const std::string& key, const ConfigValue& value, ConfigSource source) {
    setRaw(key, value, source);
    // Invalidate cached validation results
    if (pImpl->entries.empty()) {
        pImpl->entries.reserve(32); // Pre-allocate reasonable capacity
    }
    for (auto& entry : pImpl->entries) {
        if (entry.key == key) {
            entry.markValidationDirty();
            break;
        }
    }
    validationDirty_ = true;
}

void Config::set(const std::string& key, const std::string& value, ConfigSource source) {
    set(key, ConfigValue(value), source);
}

void Config::set(const std::string& key, int value, ConfigSource source) {
    set(key, ConfigValue(value), source);
}

void Config::set(const std::string& key, double value, ConfigSource source) {
    set(key, ConfigValue(value), source);
}

void Config::set(const std::string& key, bool value, ConfigSource source) {
    set(key, ConfigValue(value), source);
}

void Config::setRaw(const std::string& key, const ConfigValue& value, ConfigSource source) {
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    
    if (it != pImpl->entries.end()) {
        it->value = value;
        it->source = source;
    } else {
        pImpl->entries.emplace_back(key, value, source);
    }
}

ValidationResult Config::validateParameter(const std::string& key) const {
    ValidationResult result;
    
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    
    if (it == pImpl->entries.end()) {
        if (schema_ && schema_->hasParameter(key)) {
            const auto& param = schema_->getParameter(key);
            if (param.required) {
                result.addError(ConfigurationError(
                    "Required parameter '" + key + "' is missing",
                    ValidationErrorType::MissingRequired,
                    ValidationSeverity::Error,
                    key
                ));
            }
        }
        return result;
    }
    
    return validateConfigValue(*it);
}

ValidationResult Config::validateAll() const {
    ValidationResult result;
    
    for (const auto& entry : pImpl->entries) {
        auto paramResult = validateParameter(entry.key);
        for (const auto& error : paramResult.errors()) {
            result.addError(error);
        }
    }
    
    // Validate dependencies
    for (const auto& entry : pImpl->entries) {
        auto depResult = validateDependencies(entry);
        for (const auto& error : depResult.errors()) {
            result.addError(error);
        }
    }
    
    validationResults_ = result;
    validationDirty_ = false;
    
    return result;
}

bool Config::has(const std::string& key) const {
    return std::any_of(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
}

void Config::remove(const std::string& key) {
    pImpl->entries.erase(
        std::remove_if(pImpl->entries.begin(), pImpl->entries.end(),
            [&key](const ConfigEntry& e) { return e.key == key; }),
        pImpl->entries.end()
    );
    validationDirty_ = true;
}

std::vector<std::string> Config::getKeys() const {
    std::vector<std::string> keys;
    keys.reserve(pImpl->entries.size());
    for (const auto& entry : pImpl->entries) {
        keys.push_back(entry.key);
    }
    return keys;
}

std::vector<ParameterGroup> Config::getParameterGroups() const {
    std::vector<ParameterGroup> groups;
    
    if (schema_) {
        // Create groups from schema
        std::map<std::string, ParameterGroup> groupMap;
        
        // Add default group for parameters without group
        ParameterGroup defaultGroup("Default", "Default parameters without specific group");
        
        for (const auto& pair : schema_->getKeys()) {
            const auto& param = schema_->getParameter(pair);
            bool assigned = false;
            for (const auto& dep : param.dependencies) {
                // Find group containing dependency
                for (const auto& group : parameterGroups_) {
                    if (group.hasParameter(dep.parameter)) {
                        groupMap[group.name()].addParameter(pair);
                        assigned = true;
                        break;
                    }
                }
            }
            
            if (!assigned) {
                defaultGroup.addParameter(pair);
            }
        }
        
        groups.push_back(defaultGroup);
        for (const auto& pair : groupMap) {
            groups.push_back(pair.second);
        }
    }
    
    return groups;
}

void Config::clear() {
    pImpl->entries.clear();
    validationDirty_ = true;
}

std::string Config::summary() const {
    std::ostringstream oss;
    oss << "Configuration (" << pImpl->entries.size() << " entries):\n";
    
    for (const auto& entry : pImpl->entries) {
        oss << "  " << entry.key << " = [";
        oss << formatValue(entry.value);
        oss << "] (source: " << static_cast<int>(entry.source) << ")\n";
    }
    
    if (validationDirty_) {
        validateAll();
    }
    
    if (!validationResults_.errors().empty()) {
        oss << "\nValidation errors:";
        for (const auto& error : validationResults_.errors()) {
            oss << "\n  " << error.formatted();
        }
    }
    
    return oss.str();
}

ValidationResult Config::validateConfigValue(const ConfigEntry& entry) const {
    ValidationResult result;
    
    if (!schema_ || !schema_->hasParameter(entry.key)) {
        return result; // No schema validation if schema not set
    }
    
    const auto& param = schema_->getParameter(entry.key);
    
    // Type validation
    if (param.type == "int") {
        try {
            int val = std::any_cast<int>(entry.value);
            if (param.minValue.has_value()) {
                int minVal = std::any_cast<int>(param.minValue.value());
                if (val < minVal) {
                    result.addError(ConfigurationError(
                        "Value " + std::to_string(val) + " is below minimum " + std::to_string(minVal),
                        ValidationErrorType::RangeViolation,
                        ValidationSeverity::Error,
                        entry.key,
                        "Set to at least " + std::to_string(minVal)
                    ));
                }
            }
            if (param.maxValue.has_value()) {
                int maxVal = std::any_cast<int>(param.maxValue.value());
                if (val > maxVal) {
                    result.addError(ConfigurationError(
                        "Value " + std::to_string(val) + " exceeds maximum " + std::to_string(maxVal),
                        ValidationErrorType::RangeViolation,
                        ValidationSeverity::Error,
                        entry.key,
                        "Set to at most " + std::to_string(maxVal)
                    ));
                }
            }
        } catch (const std::bad_any_cast&) {
            result.addError(ConfigurationError(
                "Value type does not match parameter type (expected int)",
                ValidationErrorType::TypeMismatch,
                ValidationSeverity::Error,
                entry.key,
                "Parameter type is int"
            ));
        }
    } else if (param.type == "double") {
        try {
            double val = std::any_cast<double>(entry.value);
            if (param.minValue.has_value()) {
                double minVal = std::any_cast<double>(param.minValue.value());
                if (val < minVal) {
                    result.addError(ConfigurationError(
                        "Value " + std::to_string(val) + " is below minimum " + std::to_string(minVal),
                        ValidationErrorType::RangeViolation,
                        ValidationSeverity::Error,
                        entry.key,
                        "Set to at least " + std::to_string(minVal)
                    ));
                }
            }
            if (param.maxValue.has_value()) {
                double maxVal = std::any_cast<double>(param.maxValue.value());
                if (val > maxVal) {
                    result.addError(ConfigurationError(
                        "Value " + std::to_string(val) + " exceeds maximum " + std::to_string(maxVal),
                        ValidationErrorType::RangeViolation,
                        ValidationSeverity::Error,
                        entry.key,
                        "Set to at most " + std::to_string(maxVal)
                    ));
                }
            }
        } catch (const std::bad_any_cast&) {
            result.addError(ConfigurationError(
                "Value type does not match parameter type (expected double)",
                ValidationErrorType::TypeMismatch,
                ValidationSeverity::Error,
                entry.key,
                "Parameter type is double"
            ));
        }
    } else if (param.type == "string") {
        try {
            std::string val = std::any_cast<std::string>(entry.value);
            
            if (!param.pattern.empty()) {
                // Parse pattern (simplified)
                if (param.pattern.find("length:") == 0) {
                    // Format: length:min,max
                    size_t colon1 = param.pattern.find(':', 8);
                    size_t comma = param.pattern.find(',', colon1);
                    if (colon1 != std::string::npos && comma != std::string::npos) {
                        int minLen = std::stoi(param.pattern.substr(8, colon1 - 8));
                        int maxLen = std::stoi(param.pattern.substr(comma + 1));
                        
                        if ((int)val.length() < minLen) {
                            result.addError(ConfigurationError(
                                "String length " + std::to_string(val.length()) + " is below minimum " + std::to_string(minLen),
                                ValidationErrorType::RangeViolation,
                                ValidationSeverity::Error,
                                entry.key,
                                "String must have at least " + std::to_string(minLen) + " characters"
                            ));
                        }
                        
                        if ((int)val.length() > maxLen) {
                            result.addError(ConfigurationError(
                                "String length " + std::to_string(val.length()) + " exceeds maximum " + std::to_string(maxLen),
                                ValidationErrorType::RangeViolation,
                                ValidationSeverity::Error,
                                entry.key,
                                "String must have at most " + std::to_string(maxLen) + " characters"
                            ));
                        }
                    }
                } else {
                    std::regex regex(param.pattern);
                    if (!std::regex_match(val, regex)) {
                        result.addError(ConfigurationError(
                            "String does not match required pattern",
                            ValidationErrorType::FormatViolation,
                            ValidationSeverity::Error,
                            entry.key,
                            "String must match pattern: " + param.pattern
                        ));
                    }
                }
            }
        } catch (const std::bad_any_cast&) {
            result.addError(ConfigurationError(
                "Value type does not match parameter type (expected string)",
                ValidationErrorType::TypeMismatch,
                ValidationSeverity::Error,
                entry.key,
                "Parameter type is string"
            ));
        }
    } else if (param.type == "enum") {
        // Enum validation
        if (!param.allowedValues.empty()) {
            // Check if value matches one of the allowed values
            bool found = false;
            for (const auto& allowedVal : param.allowedValues) {
                if (ConfigUtils::valuesEqual(entry.value, allowedVal)) {
                    found = true;
                    break;
                }
            }
            if (!found) {
                result.addError(ConfigurationError(
                    "Value not in allowed enum values",
                    ValidationErrorType::EnumViolation,
                    ValidationSeverity::Error,
                    entry.key,
                    "Choose from: " + enumToString(param.allowedValues)
                ));
            }
        }
    }
    
    return result;
}

ValidationResult Config::validateDependencies(const ConfigEntry& entry) const {
    ValidationResult result;
    
    if (!schema_ || !schema_->hasParameter(entry.key)) {
        return result;
    }
    
    const auto& param = schema_->getParameter(entry.key);
    
    for (const auto& dep : param.dependencies) {
        // Find dependency entry
        auto depIt = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
            [&dep](const ConfigEntry& e) { return e.key == dep.parameter; });
        
        if (depIt == pImpl->entries.end()) {
            // Dependency not set
            if (dep.operation == "exists") {
                result.addError(ConfigurationError(
                    "Parameter '" + dep.parameter + "' is required but not set",
                    ValidationErrorType::DependencyViolation,
                    ValidationSeverity::Error,
                    entry.key,
                    "Set " + dep.parameter + " before using " + entry.key
                ));
            }
        } else {
            // Dependency is set, check operation
            if (dep.operation == "equals") {
                if (!ConfigUtils::valuesEqual(depIt->value, dep.value)) {
                    result.addError(ConfigurationError(
                        "Parameter '" + dep.parameter + "' must have value " + formatValue(dep.value) + " but has " + formatValue(depIt->value),
                        ValidationErrorType::DependencyViolation,
                        ValidationSeverity::Error,
                        entry.key,
                        "Set " + dep.parameter + " to " + formatValue(dep.value)
                    ));
                }
            } else if (dep.operation == "greater") {
                try {
                    double depVal = std::any_cast<double>(depIt->value);
                    double requiredVal = std::any_cast<double>(dep.value);
                    if (depVal <= requiredVal) {
                        result.addError(ConfigurationError(
                            "Parameter '" + dep.parameter + "' must be greater than " + std::to_string(requiredVal) + " but is " + std::to_string(depVal),
                            ValidationErrorType::DependencyViolation,
                            ValidationSeverity::Error,
                            entry.key,
                            "Increase " + dep.parameter + " to be greater than " + std::to_string(requiredVal)
                        ));
                    }
                } catch (const std::bad_any_cast&) {
                    result.addError(ConfigurationError(
                        "Parameter '" + dep.parameter + "' has invalid type for comparison",
                        ValidationErrorType::TypeMismatch,
                        ValidationSeverity::Error,
                        entry.key,
                        "Ensure " + dep.parameter + " is numeric"
                    ));
                }
            } else if (dep.operation == "less") {
                try {
                    double depVal = std::any_cast<double>(depIt->value);
                    double requiredVal = std::any_cast<double>(dep.value);
                    if (depVal >= requiredVal) {
                        result.addError(ConfigurationError(
                            "Parameter '" + dep.parameter + "' must be less than " + std::to_string(requiredVal) + " but is " + std::to_string(depVal),
                            ValidationErrorType::DependencyViolation,
                            ValidationSeverity::Error,
                            entry.key,
                            "Decrease " + dep.parameter + " to be less than " + std::to_string(requiredVal)
                        ));
                    }
                } catch (const std::bad_any_cast&) {
                    result.addError(ConfigurationError(
                        "Parameter '" + dep.parameter + "' has invalid type for comparison",
                        ValidationErrorType::TypeMismatch,
                        ValidationSeverity::Error,
                        entry.key,
                        "Ensure " + dep.parameter + " is numeric"
                    ));
                }
            }
        }
    }
    
    return result;
}

ValidationResult Config::validateConditional(const ConfigEntry& entry) const {
    ValidationResult result;
    
    if (!schema_ || !schema_->hasParameter(entry.key)) {
        return result;
    }
    
    const auto& param = schema_->getParameter(entry.key);
    
    // Check conditional dependencies
    // In a real implementation, this would check if(entry.value meets certain conditions)
    // and validate other parameters based on those conditions
    
    return result;
}

std::string Config::getParameterType(const std::string& key) const {
    if (!schema_ || !schema_->hasParameter(key)) {
        return "unknown";
    }
    return schema_->getParameter(key).type;
}

std::string Config::getParameterConstraints(const std::string& key) const {
    if (!schema_ || !schema_->hasParameter(key)) {
        return "No constraints defined (schema not set)";
    }
    
    const auto& param = schema_->getParameter(key);
    std::ostringstream oss;
    
    if (param.type == "int" || param.type == "double") {
        if (param.minValue.has_value() && param.maxValue.has_value()) {
            oss << "Range: [" << formatValue(param.minValue.value()) << ", " << formatValue(param.maxValue.value()) << "]";
        } else if (param.minValue.has_value()) {
            oss << "Min: " << formatValue(param.minValue.value());
        } else if (param.maxValue.has_value()) {
            oss << "Max: " << formatValue(param.maxValue.value());
        }
    } else if (param.type == "string" && !param.pattern.empty()) {
        oss << "Pattern: " << param.pattern;
    }
    
    return oss.str();
}

bool Config::hasPythonType(const std::string& key) const {
    if (!schema_ || !schema_->hasParameter(key)) {
        return false;
    }
    
    const auto& param = schema_->getParameter(key);
    return !param.pythonType.empty();
}

std::string Config::getPythonType(const std::string& key) const {
    if (!schema_ || !schema_->hasParameter(key)) {
        return "";
    }
    
    const auto& param = schema_->getParameter(key);
    return param.pythonType.empty() ? "" : param.pythonType;
}

std::string Config::formatValue(const ConfigValue& value) {
    std::ostringstream oss;
    std::visit([&oss](auto&& arg) {
        using T = std::decay_t<decltype(arg)>;
        if constexpr (std::is_same_v<T, std::string>) {
            oss << "\"" << arg << "\"";
        } else if constexpr (std::is_same_v<T, std::vector<int>>) {
            oss << "[";
            for (size_t i = 0; i < arg.size(); ++i) {
                if (i > 0) oss << ", ";
                oss << arg[i];
            }
            oss << "]";
        } else if constexpr (std::is_same_v<T, std::vector<double>>) {
            oss << "[";
            for (size_t i = 0; i < arg.size(); ++i) {
                if (i > 0) oss << ", ";
                oss << std::fixed << std::setprecision(2) << arg[i];
            }
            oss << "]";
        } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
            oss << "[";
            for (size_t i = 0; i < arg.size(); ++i) {
                if (i > 0) oss << ", ";
                oss << "\"" << arg[i] << "\"";
            }
            oss << "]";
        } else {
            oss << arg;
        }
    }, value);
    return oss.str();
}

std::string Config::trim(const std::string& str) {
    size_t start = str.find_first_not_of(" \t\r\n");
    if (start == std::string::npos) return "";
    size_t end = str.find_last_not_of(" \t\r\n");
    return str.substr(start, end - start + 1);
}

std::string Config::toLower(const std::string& str) {
    std::string result = str;
    std::transform(result.begin(), result.end(), result.begin(), ::tolower);
    return result;
}

// Helper functions for Python integration
bool Config::setFromPythonValue(const std::string& key, const std::any& value) {
    // Convert Python value to ConfigValue
    if (value.type() == typeid(int)) {
        set(key, std::any_cast<int>(value), ConfigSource::Runtime);
        return true;
    } else if (value.type() == typeid(double)) {
        set(key, std::any_cast<double>(value), ConfigSource::Runtime);
        return true;
    } else if (value.type() == typeid(bool)) {
        set(key, std::any_cast<bool>(value), ConfigSource::Runtime);
        return true;
    } else if (value.type() == typeid(std::string)) {
        set(key, std::any_cast<std::string>(value), ConfigSource::Runtime);
        return true;
    } else if (value.type() == typeid(std::vector<int>)) {
        set(key, std::any_cast<std::vector<int>>(value), ConfigSource::Runtime);
        return true;
    } else if (value.type() == typeid(std::vector<double>)) {
        set(key, std::any_cast<std::vector<double>>(value), ConfigSource::Runtime);
        return true;
    } else if (value.type() == typeid(std::vector<std::string>)) {
        set(key, std::any_cast<std::vector<std::string>>(value), ConfigSource::Runtime);
        return true;
    }
    return false;
}

bool ConfigUtils::valuesEqual(const ConfigValue& a, const ConfigValue& b) {
    return std::visit([&b](auto&& argA) -> bool {
        using T = std::decay_t<decltype(argA)>;
        if constexpr (std::is_same_v<T, std::string>) {
            return std::any_cast<const std::string&>(b) == argA;
        } else if constexpr (std::is_same_v<T, std::vector<int>>) {
            return std::any_cast<const std::vector<int>&>(b) == argA;
        } else if constexpr (std::is_same_v<T, std::vector<double>>) {
            return std::any_cast<const std::vector<double>&>(b) == argA;
        } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
            return std::any_cast<const std::vector<std::string>&>(b) == argA;
        } else {
            return std::any_cast<T>(b) == argA;
        }
    }, a);
}

std::string ConfigUtils::formatError(const std::string& templateStr, 
                                    const std::map<std::string, std::string>& placeholders) {
    std::string result = templateStr;
    for (const auto& pair : placeholders) {
        size_t pos = result.find("%" + pair.first + "%");
        while (pos != std::string::npos) {
            result.replace(pos, pair.first.length() + 2, pair.second);
            pos = result.find("%" + pair.first + "%", pos + pair.second.length());
        }
    }
    return result;
}

size_t ConfigUtils::hashValue(const ConfigValue& value) {
    return std::visit([](auto&& arg) -> size_t {
        using T = std::decay_t<decltype(arg)>;
        if constexpr (std::is_same_v<T, std::string>) {
            return std::hash<std::string>{}(arg);
        } else if constexpr (std::is_same_v<T, std::vector<int>>) {
            size_t seed = std::hash<size_t>{}(arg.size());
            for (const auto& item : arg) {
                seed ^= std::hash<int>{}(item) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
            }
            return seed;
        } else if constexpr (std::is_same_v<T, std::vector<double>>) {
            size_t seed = std::hash<size_t>{}(arg.size());
            for (const auto& item : arg) {
                seed ^= std::hash<double>{}(item) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
            }
            return seed;
        } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
            size_t seed = std::hash<size_t>{}(arg.size());
            for (const auto& item : arg) {
                seed ^= std::hash<std::string>{}(item) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
            }
            return seed;
        } else {
            return std::hash<T>{}(arg);
        }
    }, value);
}

// Python integration namespace implementation
namespace PythonConfig {
    std::map<std::string, std::any> toPythonDict(const Config& config) {
        std::map<std::string, std::any> result;
        for (const auto& key : config.getKeys()) {
            result[key] = convertToPythonValue(config.getOr<int>(key, 0));
        }
        return result;
    }
    
    bool loadFromPythonDict(Config& config, const std::map<std::string, std::any>& pythonDict) {
        config.clear();
        for (const auto& pair : pythonDict) {
            if (!config.setFromPythonValue(pair.first, pair.second)) {
                return false;
            }
        }
        config.validationDirty_ = true;
        return true;
    }
    
    ValidationResult validatePythonType(const std::string& key, const std::any& value, const std::string& pythonType) {
        ValidationResult result;
        
        if (pythonType == "int") {
            if (value.type() != typeid(int)) {
                result.addError(ConfigurationError(
                    "Expected Python int type, got " + std::to_string(value.type().hash_code()),
                    ValidationErrorType::TypeMismatch,
                    ValidationSeverity::Error,
                    key,
                    "Convert to int"
                ));
            }
        } else if (pythonType == "float") {
            if (value.type() != typeid(double)) {
                result.addError(ConfigurationError(
                    "Expected Python float type, got " + std::to_string(value.type().hash_code()),
                    ValidationErrorType::TypeMismatch,
                    ValidationSeverity::Error,
                    key,
                    "Convert to float"
                ));
            }
        } else if (pythonType == "bool") {
            if (value.type() != typeid(bool)) {
                result.addError(ConfigurationError(
                    "Expected Python bool type, got " + std::to_string(value.type().hash_code()),
                    ValidationErrorType::TypeMismatch,
                    ValidationSeverity::Error,
                    key,
                    "Convert to bool"
                ));
            }
        } else if (pythonType == "str") {
            if (value.type() != typeid(std::string)) {
                result.addError(ConfigurationError(
                    "Expected Python str type, got " + std::to_string(value.type().hash_code()),
                    ValidationErrorType::TypeMismatch,
                    ValidationSeverity::Error,
                    key,
                    "Convert to str"
                ));
            }
        }
        
        return result;
    }
    
    std::string getPythonTypeForKey(const Config& config, const std::string& key) {
        return config.getPythonType(key);
    }
    
    bool hasPythonKey(const std::map<std::string, std::any>& pythonDict, const std::string& key) {
        return pythonDict.find(key) != pythonDict.end();
    }
    
    template<typename T>
    T PythonConfig::getFromPythonDict(const std::map<std::string, std::any>& pythonDict, const std::string& key) {
        auto it = pythonDict.find(key);
        if (it == pythonDict.end()) {
            throw std::runtime_error("Key not found in Python dict: " + key);
        }
        return std::any_cast<T>(it->second);
    }
    
    std::any convertToPythonValue(const ConfigValue& value) {
        if (std::holds_alternative<int>(value)) {
            return std::get<int>(value);
        } else if (std::holds_alternative<double>(value)) {
            return std::get<double>(value);
        } else if (std::holds_alternative<bool>(value)) {
            return std::get<bool>(value);
        } else if (std::holds_alternative<std::string>(value)) {
            return std::get<std::string>(value);
        }
        return std::any();
    }
}

// Global schema helper functions
namespace ConfigSchema {
    ParameterInfo makeIntParameter(const std::string& key, int defaultValue, 
                                   int min = std::numeric_limits<int>::lowest(),
                                   int max = std::numeric_limits<int>::max(),
                                   const std::string& description = "",
                                   bool required = false) {
        ParameterInfo param;
        param.key = key;
        param.type = "int";
        param.defaultValue = defaultValue;
        param.description = description;
        param.required = required;
        param.minValue = min;
        param.maxValue = max;
        param.pythonType = "int";
        return param;
    }
    
    ParameterInfo makeDoubleParameter(const std::string& key, double defaultValue,
                                       double min = -std::numeric_limits<double>::max(),
                                       double max = std::numeric_limits<double>::max(),
                                       const std::string& description = "",
                                       bool required = false) {
        ParameterInfo param;
        param.key = key;
        param.type = "double";
        param.defaultValue = defaultValue;
        param.description = description;
        param.required = required;
        param.minValue = min;
        param.maxValue = max;
        param.pythonType = "float";
        return param;
    }
    
    ParameterInfo makeStringParameter(const std::string& key, const std::string& defaultValue,
                                       size_t minLength = 0,
                                       size_t maxLength = std::numeric_limits<size_t>::max(),
                                       const std::string& pattern = "",
                                       const std::string& description = "",
                                       bool required = false) {
        ParameterInfo param;
        param.key = key;
        param.type = "string";
        param.defaultValue = defaultValue;
        param.description = description;
        param.required = required;
        if (minLength > 0 || maxLength < std::numeric_limits<size_t>::max()) {
            param.pattern = "length:" + std::to_string(minLength) + "," + std::to_string(maxLength);
        }
        if (!pattern.empty()) {
            if (!param.pattern.empty()) param.pattern += ";";
            param.pattern += pattern;
        }
        param.pythonType = "str";
        return param;
    }
    
    ParameterInfo makeEnumParameter(const std::string& key, const std::any& defaultValue,
                                     const std::vector<std::any>& allowedValues,
                                     const std::string& description = "",
                                     bool required = false) {
        ParameterInfo param;
        param.key = key;
        param.type = "enum";
        param.defaultValue = defaultValue;
        param.description = description;
        param.required = required;
        param.allowedValues = allowedValues;
        param.pythonType = "enum";
        return param;
    }
}

// Forward declare helper for accessing ParameterInfo from ConfigSchema
const std::map<std::string, ConfigSchema::ParameterInfo>& ConfigSchema::getParameterInfo() const {
    // This is a placeholder - would need a method to access parameters in ConfigSchema
    static const std::map<std::string, ParameterInfo> emptyMap;
    return emptyMap;
}

// Helper function for enum string conversion
std::string enumToString(const std::vector<std::any>& values) {
    std::ostringstream oss;
    bool first = true;
    for (const auto& val : values) {
        if (!first) oss << ", ";
        if (val.type() == typeid(int)) {
            oss << std::any_cast<int>(val);
        } else if (val.type() == typeid(double)) {
            oss << std::any_cast<double>(val);
        } else if (val.type() == typeid(bool)) {
            oss << (std::any_cast<bool>(val) ? "true" : "false");
        } else if (val.type() == typeid(std::string)) {
            oss << "\"" << std::any_cast<std::string>(val) << "\"";
        } else {
            oss << "?";
        }
        first = false;
    }
    return oss.str();
}

} // namespace nlm