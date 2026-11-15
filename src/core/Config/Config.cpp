#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <nlohmann/json.hpp>
#include <yaml-cpp/yaml.h>

namespace nlm {

struct Config::Impl {
    std::vector<ConfigEntry> entries;
    std::string configFormat; // Track the format for future operations
};

// Helper function to parse JSON values
detail::ConfigValue jsonToConfigValue(const nlohmann::json& j) {
    if (j.is_string()) {
        return j.get<std::string>();
    } else if (j.is_number_integer()) {
        return j.get<int64_t>();
    } else if (j.is_number_unsigned()) {
        return j.get<uint64_t>();
    } else if (j.is_number_float()) {
        return j.get<double>();
    } else if (j.is_boolean()) {
        return j.get<bool>();
    } else if (j.is_array()) {
        // For now, handle simple arrays of numbers or strings
        if (j.empty()) return std::vector<int>{};
        
        // Check if it's an array of numbers
        bool allNumbers = true;
        bool allStrings = true;
        for (const auto& item : j) {
            if (item.is_number()) allStrings = false;
            if (item.is_string()) allNumbers = false;
        }
        
        if (allNumbers) {
            std::vector<double> result;
            result.reserve(j.size());
            for (const auto& item : j) {
                result.push_back(item.get<double>());
            }
            return result;
        } else if (allStrings) {
            std::vector<std::string> result;
            result.reserve(j.size());
            for (const auto& item : j) {
                result.push_back(item.get<std::string>());
            }
            return result;
        }
    }
    
    // Default to empty string for unknown types
    return std::string();
}

// Helper function to parse YAML values
ConfigValue yamlToConfigValue(const YAML::Node& node) {
    if (node.IsScalar()) {
        std::string value = node.Scalar();
        // Try to parse as boolean
        if (value == "true" || value == "false") {
            return value == "true";
        }
        // Try to parse as integer
        try {
            if (value.find('.') == std::string::npos) {
                int64_t intVal = std::stoll(value);
                return intVal;
            }
        } catch (...) {}
        // Try to parse as float
        try {
            double floatVal = std::stod(value);
            return floatVal;
        } catch (...) {}
        return value;
    } else if (node.IsSequence()) {
        // Handle arrays
        if (node.size() == 0) return std::vector<double>{};
        
        // Determine if array contains numbers or strings
        bool allNumbers = true;
        bool allStrings = true;
        for (const auto& item : node) {
            if (item.IsScalar()) {
                std::string val = item.Scalar();
                // Check if it looks like a number
                bool looksLikeNumber = true;
                for (char c : val) {
                    if (!std::isdigit(c) && c != '.' && c != '-' && c != 'e' && c != 'E') {
                        looksLikeNumber = false;
                        break;
                    }
                }
                if (looksLikeNumber) {
                    allStrings = false;
                } else {
                    allNumbers = false;
                }
            }
        }
        
        if (allNumbers) {
            std::vector<double> result;
            result.reserve(node.size());
            for (const auto& item : node) {
                result.push_back(std::stod(item.Scalar()));
            }
            return result;
        } else if (allStrings) {
            std::vector<std::string> result;
            result.reserve(node.size());
            for (const auto& item : node) {
                result.push_back(item.Scalar());
            }
            return result;
        }
    }
    
    return std::string();
}

bool Config::loadFromFile(const std::string& filepath) {
    // Determine file format by extension
    std::string extension = std::filesystem::path(filepath).extension().string();
    std::transform(extension.begin(), extension.end(), extension.begin(), ::tolower);
    
    bool success = false;
    if (extension == ".json" || extension == ".json5" || extension == ".jsonc") {
        success = loadFromJSON(filepath);
        pImpl->configFormat = "JSON";
    } else if (extension == ".yaml" || extension == ".yml") {
        success = loadFromYAML(filepath);
        pImpl->configFormat = "YAML";
    } else {
        // Default to simple key=value format for backward compatibility
        success = loadFromSimpleFormat(filepath);
        pImpl->configFormat = "simple";
    }
    
    return success;
}

bool Config::loadFromJSON(const std::string& filepath) {
    try {
        std::ifstream file(filepath);
        if (!file.is_open()) {
            return false;
        }
        
        nlohmann::json j;
        file >> j;
        
        // Clear existing entries
        pImpl->entries.clear();
        
        // Recursively parse JSON
        parseJSONToConfig(j, "");
        
        return true;
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("JSON parsing error: " + std::string(e.what()));
        return false;
    }
}

bool Config::loadFromYAML(const std::string& filepath) {
    try {
        YAML::Node doc = YAML::LoadFile(filepath);
        
        // Clear existing entries
        pImpl->entries.clear();
        
        // Recursively parse YAML
        parseYAMLToConfig(doc, "");
        
        return true;
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("YAML parsing error: " + std::string(e.what()));
        return false;
    }
}

bool Config::loadFromSimpleFormat(const std::string& filepath) {
    // Original simple format implementation for backward compatibility
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    std::string line;
    while (std::getline(file, line)) {
        // Skip empty lines and comments
        line = trim(line);
        if (line.empty() || line[0] == '#' || line[0] == '/') {
            continue;
        }
        
        // Parse simple key=value pairs
        size_t pos = line.find('=');
        if (pos != std::string::npos) {
            std::string key = trim(line.substr(0, pos));
            std::string value = trim(line.substr(pos + 1));
            
            // Remove quotes if present
            if (value.size() >= 2 && 
                ((value.front() == '\"' && value.back() == '\"') ||
                 (value.front() == '\'' && value.back() == '\''))) {
                value = value.substr(1, value.size() - 2);
            }
            
            set(key, value, ConfigSource::File);
        }
    }
    
    return true;
}

void Config::parseJSONToConfig(const nlohmann::json& j, const std::string& prefix) {
    if (j.is_object()) {
        for (auto it = j.begin(); it != j.end(); ++it) {
            std::string key = prefix.empty() ? it.key() : (prefix + "." + it.key());
            parseJSONToConfig(*it, key);
        }
    } else {
        // Store the value
        set(keyToConfigKey(prefix), jsonToConfigValue(j), ConfigSource::File);
    }
}

void Config::parseYAMLToConfig(const YAML::Node& node, const std::string& prefix) {
    if (node.IsMap()) {
        for (auto it = node.begin(); it != node.end(); ++it) {
            std::string key = prefix.empty() ? it->first.as<std::string>() : 
                                     (prefix + "." + it->first.as<std::string>());
            parseYAMLToConfig(*it, key);
        }
    } else {
        // Store the value
        set(keyToConfigKey(prefix), yamlToConfigValue(node), ConfigSource::File);
    }
}

std::string Config::keyToConfigKey(const std::string& key) {
    // Convert dotted notation to a string suitable for config keys
    // For example, "brain.synapse.weight" becomes "brain.synapse.weight"
    return key;
}

bool Config::validate(const std::vector<ConfigValidationRule>& rules) const {
    std::vector<std::string> errors;
    
    for (const auto& rule : rules) {
        if (!has(rule.key)) {
            errors.push_back("Configuration key '" + rule.key + "' not found");
        } else {
            auto value = get(rule.key);
            if (!value) {
                errors.push_back("Configuration key '" + rule.key + "' has invalid type");
            } else if (!rule.validator(*value)) {
                errors.push_back("Configuration key '" + rule.key + "' fails validation");
            }
        }
    }
    
    if (!errors.empty()) {
        NLM_LOG_ERROR("Configuration validation failed:");
        for (const auto& error : errors) {
            NLM_LOG_ERROR("  - " + error);
        }
        return false;
    }
    
    NLM_LOG_INFO("Configuration validation passed");
    return true;
}

bool Config::validate(const std::string& key, std::function<bool(const ConfigValue&)> validator) const {
    if (!has(key)) {
        NLM_LOG_ERROR("Configuration validation failed: key '" + key + "' not found");
        return false;
    }
    
    auto value = get(key);
    if (!value) {
        NLM_LOG_ERROR("Configuration validation failed: key '" + key + "' has invalid type");
        return false;
    }
    
    if (!validator(*value)) {
        NLM_LOG_ERROR("Configuration validation failed: key '" + key + "' fails validation");
        return false;
    }
    
    NLM_LOG_INFO("Configuration validation passed for key: '" + key + "'");
    return true;
}

bool Config::validateRange(const std::string& key, double min, double max) const {
    return validate(key, [min, max](const ConfigValue& value) {
        if (std::holds_alternative<int64_t>(value)) {
            return static_cast<double>(std::get<int64_t>(value)) >= min && 
                   static_cast<double>(std::get<int64_t>(value)) <= max;
        } else if (std::holds_alternative<double>(value)) {
            return std::get<double>(value) >= min && std::get<double>(value) <= max;
        } else if (std::holds_alternative<float>(value)) {
            return static_cast<double>(std::get<float>(value)) >= min && 
                   static_cast<double>(std::get<float>(value)) <= max;
        }
        return false;
    });
}

bool Config::validatePositive(const std::string& key) const {
    return validate(key, [](const ConfigValue& value) {
        if (std::holds_alternative<int64_t>(value)) {
            return std::get<int64_t>(value) > 0;
        } else if (std::holds_alternative<uint64_t>(value)) {
            return std::get<uint64_t>(value) > 0;
        } else if (std::holds_alternative<double>(value)) {
            return std::get<double>(value) > 0;
        } else if (std::holds_alternative<float>(value)) {
            return std::get<float>(value) > 0;
        }
        return false;
    });
}

bool Config::validateNonNegative(const std::string& key) const {
    return validate(key, [](const ConfigValue& value) {
        if (std::holds_alternative<int64_t>(value)) {
            return std::get<int64_t>(value) >= 0;
        } else if (std::holds_alternative<uint64_t>(value)) {
            return std::get<uint64_t>(value) >= 0;
        } else if (std::holds_alternative<double>(value)) {
            return std::get<double>(value) >= 0;
        } else if (std::holds_alternative<float>(value)) {
            return std::get<float>(value) >= 0;
        }
        return false;
    });
}

bool Config::validateProbability(const std::string& key) const {
    return validate(key, [](const ConfigValue& value) {
        if (std::holds_alternative<double>(value)) {
            double val = std::get<double>(value);
            return val >= 0.0 && val <= 1.0;
        } else if (std::holds_alternative<float>(value)) {
            float val = std::get<float>(value);
            return val >= 0.0f && val <= 1.0f;
        }
        return false;
    });
}

std::string Config::getFormat() const {
    return pImpl->configFormat;
}

Config::ValidationRule::ValidationRule(std::string k, std::function<bool(const ConfigValue&)> v)
    : key(std::move(k)), validator(std::move(v)) {}

Config::ConfigValidationRule::ConfigValidationRule(std::string k, std::function<bool(const ConfigValue&)> v)
    : key(std::move(k)), validator(std::move(v)) {}

} // namespace nlm