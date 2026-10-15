// Include the implementation header
#include "ConfigImpl.hpp"

namespace nlm {

// ConfigSchema implementation
ConfigSchema& ConfigSchema::addInt(const std::string& key, int defaultValue,
                                   const std::string& description, int min, int max) {
    auto entry = parameters.find(key);
    if (entry == parameters.end()) {
        parameters[key] = ConfigEntry(key, defaultValue, ConfigSource::Default, description);
        entry = parameters.find(key);
    }
    
    auto validator = [min, max](const ConfigValue& v) {
        try {
            int val = std::get<int>(v);
            return val >= min && val <= max;
        } catch (...) { return false; }
    };
    
    entry->second.metadata.addValidation(
        "Value must be between " + std::to_string(min) + " and " + std::to_string(max),
        validator);
    return *this;
}

ConfigSchema& ConfigSchema::addDouble(const std::string& key, double defaultValue,
                                      const std::string& description, double min, double max) {
    auto entry = parameters.find(key);
    if (entry == parameters.end()) {
        parameters[key] = ConfigEntry(key, defaultValue, ConfigSource::Default, description);
        entry = parameters.find(key);
    }
    
    auto validator = [min, max](const ConfigValue& v) {
        try {
            double val = std::get<double>(v);
            return val >= min && val <= max;
        } catch (...) { return false; }
    };
    
    entry->second.metadata.addValidation(
        "Value must be between " + std::to_string(min) + " and " + std::to_string(max),
        validator);
    return *this;
}

ConfigSchema& ConfigSchema::addBool(const std::string& key, bool defaultValue,
                                    const std::string& description) {
    auto entry = parameters.find(key);
    if (entry == parameters.end()) {
        parameters[key] = ConfigEntry(key, defaultValue, ConfigSource::Default, description);
    }
    
    auto validator = [](const ConfigValue& v) {
        return std::holds_alternative<bool>(v);
    };
    
    entry->second.metadata.addValidation("Value must be a boolean", validator);
    return *this;
}

ConfigSchema& ConfigSchema::addString(const std::string& key, const std::string& defaultValue,
                                     const std::string& description) {
    auto entry = parameters.find(key);
    if (entry == parameters.end()) {
        parameters[key] = ConfigEntry(key, defaultValue, ConfigSource::Default, description);
    }
    
    auto validator = [](const ConfigValue& v) {
        return std::holds_alternative<std::string>(v);
    };
    
    entry->second.metadata.addValidation("Value must be a string", validator);
    return *this;
}

ConfigSchema& ConfigSchema::addRangeValidation(const std::string& key, double min, double max,
                                              const std::string& errorMsg) {
    auto entry = parameters.find(key);
    if (entry == parameters.end()) return *this;
    
    auto validator = [min, max](const ConfigValue& v) {
        try {
            double val = std::get<double>(v);
            return val >= min && val <= max;
        } catch (...) { return false; }
    };
    
    entry->second.metadata.addValidation(errorMsg, validator);
    return *this;
}

ConfigSchema& ConfigSchema::addPositiveValidation(const std::string& key,
                                                 const std::string& errorMsg) {
    auto entry = parameters.find(key);
    if (entry == parameters.end()) return *this;
    
    auto validator = [](const ConfigValue& v) {
        try {
            double val = std::get<double>(v);
            return val >= 0.0;
        } catch (...) { return false; }
    };
    
    entry->second.metadata.addValidation(errorMsg, validator);
    return *this;
}

ConfigSchema& ConfigSchema::addNonEmptyValidation(const std::string& key,
                                                 const std::string& errorMsg) {
    auto entry = parameters.find(key);
    if (entry == parameters.end()) return *this;
    
    auto validator = [](const ConfigValue& v) {
        try {
            std::string val = std::get<std::string>(v);
            return !val.empty();
        } catch (...) { return false; }
    };
    
    entry->second.metadata.addValidation(errorMsg, validator);
    return *this;
}

const ConfigMetadata& ConfigSchema::getMetadata(const std::string& key) const {
    static const ConfigMetadata empty;
    auto it = parameters.find(key);
    return (it != parameters.end()) ? it->second.metadata : empty;
}

bool ConfigSchema::hasParameter(const std::string& key) const {
    return parameters.find(key) != parameters.end();
}

std::vector<std::string> ConfigSchema::getKeys() const {
    std::vector<std::string> keys;
    keys.reserve(parameters.size());
    for (const auto& param : parameters) {
        keys.push_back(param.first);
    }
    return keys;
}

nlohmann::json ConfigSchema::toJson() const {
    nlohmann::json json;
    for (const auto& param : parameters) {
        json[param.first] = valueToString(param.second.value);
    }
    return json;
}

void ConfigSchema::fromJson(const nlohmann::json& json) {
    for (auto it = json.begin(); it != json.end(); ++it) {
        std::string key = it.key();
        if (parameters.find(key) != parameters.end()) {
            parameters[key].value = stringToValue(it.value().dump());
        }
    }
}

std::string ConfigSchema::generateHelp() const {
    std::ostringstream oss;
    oss << "Configuration Parameters:" << std::endl;
    for (const auto& param : parameters) {
        const auto& metadata = param.second.metadata;
        oss << "\n" << param.first << ":" << std::endl;
        oss << "  Description: " << metadata.description << std::endl;
        oss << "  Type: " << getValueTypeName(param.second.value) << std::endl;
        if (metadata.required) {
            oss << "  Required: Yes" << std::endl;
        }
        if (!metadata.validationRules.empty()) {
            oss << "  Validation:" << std::endl;
            for (const auto& rule : metadata.validationRules) {
                oss << "    - " << rule.errorMessage << std::endl;
            }
        }
        oss << "  Source: " << static_cast<int>(param.second.source) << std::endl;
    }
    return oss.str();
}

std::string ConfigSchema::getValueTypeName(const ConfigValue& value) const {
    if (std::holds_alternative<int>(value)) return "int";
    if (std::holds_alternative<int64_t>(value)) return "int64_t";
    if (std::holds_alternative<double>(value)) return "double";
    if (std::holds_alternative<bool>(value)) return "bool";
    if (std::holds_alternative<std::string>(value)) return "string";
    if (std::holds_alternative<std::vector<int>>(value)) return "int[]";
    if (std::holds_alternative<std::vector<double>>(value)) return "double[]";
    if (std::holds_alternative<std::vector<std::string>>(value)) return "string[]";
    if (std::holds_alternative<std::vector<bool>>(value)) return "bool[]";
    return "unknown";
}

// ConfigSchema implementation
template std::optional<int> Config::get<int>(const std::string&) const;
template std::optional<double> Config::get<double>(const std::string&) const;
template std::optional<bool> Config::get<bool>(const std::string&) const;
template std::optional<std::string> Config::get<std::string>(const std::string&) const;

template bool Config::set<int>(const std::string&, const int&, ConfigSource);
template bool Config::set<double>(const std::string&, const double&, ConfigSource);
template bool Config::set<bool>(const std::string&, const bool&, ConfigSource);
template bool Config::set<std::string>(const std::string&, const std::string&, ConfigSource);

} // namespace nlm
