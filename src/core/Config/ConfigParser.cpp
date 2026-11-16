#include "ConfigParser.hpp"
#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>

// Use nlohmann::json namespace
using json = nlohmann::json;

std::unordered_map<std::string, json> ConfigParser::parseJSON(const std::string& jsonString) {
    std::unordered_map<std::string, json> config;
    try {
        auto parsed = json::parse(jsonString);
        config = parsed.get<std::unordered_map<std::string, json>>();
    } catch (const json::parse_error& e) {
        // TODO: Log error
    }
    return config;
}

bool ConfigParser::parseJSONFile(const std::string& filepath, std::unordered_map<std::string, json>& config) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    std::stringstream buffer;
    buffer << file.rdbuf();
    
    config = parseJSON(buffer.str());
    return true;
}

std::unordered_map<std::string, json> ConfigParser::parseYAML(const std::string& yamlString) {
    // Simplified YAML parsing - in production, would use a proper YAML library
    std::unordered_map<std::string, json> config;
    
    // For now, treat YAML as JSON for simplicity
    // TODO: Implement proper YAML parsing
    return parseJSON(yamlString);
}

nlm::ConfigValue ConfigParser::jsonToConfigValue(const json& jsonValue) {
    if (jsonValue.is_string()) {
        return jsonValue.get<std::string>();
    } else if (jsonValue.is_number_integer()) {
        return jsonValue.get<int>();
    } else if (jsonValue.is_number_unsigned()) {
        return jsonValue.get<int64_t>();
    } else if (jsonValue.is_number()) {
        return jsonValue.get<double>();
    } else if (jsonValue.is_boolean()) {
        return jsonValue.get<bool>();
    } else if (jsonValue.is_array()) {
        // Determine array type
        if (!jsonValue.empty()) {
            if (jsonValue[0].is_number_integer()) {
                std::vector<int> vec = jsonValue.get<std::vector<int>>();
                return vec;
            } else if (jsonValue[0].is_number()) {
                std::vector<double> vec = jsonValue.get<std::vector<double>>();
                return vec;
            } else if (jsonValue[0].is_string()) {
                std::vector<std::string> vec = jsonValue.get<std::vector<std::string>>();
                return vec;
            }
        }
    }
    
    // Default to string for unknown types
    return jsonValue.dump();
}

json ConfigParser::configValueToJSON(const nlm::ConfigValue& configValue) {
    return std::visit([](auto&& arg) -> json {
        using T = std::decay_t<decltype(arg)>;
        if constexpr (std::is_same_v<T, std::string>) {
            return arg;
        } else if constexpr (std::is_same_v<T, int>) {
            return arg;
        } else if constexpr (std::is_same_v<T, int64_t>) {
            return arg;
        } else if constexpr (std::is_same_v<T, double>) {
            return arg;
        } else if constexpr (std::is_same_v<T, bool>) {
            return arg;
        } else if constexpr (std::is_same_v<T, std::vector<int>>) {
            return json(arg);
        } else if constexpr (std::is_same_v<T, std::vector<double>>) {
            return json(arg);
        } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
            return json(arg);
        } else {
            return "";
        }
    }, configValue);
}

bool ConfigParser::loadConfigFromFile(nlm::Config& config, const std::string& filepath) {
    // Determine file format
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    std::stringstream buffer;
    buffer << file.rdbuf();
    std::string content = buffer.str();
    
    file.close();
    
    std::unordered_map<std::string, json> jsonConfig;
    
    if (isJSON(content)) {
        jsonConfig = parseJSON(content);
    } else if (isYAML(content)) {
        jsonConfig = parseYAML(content);
    } else {
        // Try legacy format
        return config.loadFromFile(filepath);
    }
    
    // Clear existing config
    config.clear();
    
    // Load JSON config into Config object
    for (const auto& [key, jsonValue] : jsonConfig) {
        nlm::ConfigValue configValue = jsonToConfigValue(jsonValue);
        config.set(key, configValue, nlm::ConfigSource::File);
    }
    
    return true;
}

bool ConfigParser::saveConfigToFile(const nlm::Config& config, const std::string& filepath) {
    json jsonConfig;
    
    for (const auto& key : config.getKeys()) {
        // TODO: Get the actual value from config
        // This is a simplified version
        jsonConfig[key] = "[value]";
    }
    
    std::ofstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    file << std::setw(2) << jsonConfig << std::endl;
    return true;
}

std::vector<std::string> ConfigParser::validateConfig(const nlm::Config& config) {
    std::vector<std::string> errors;
    
    // Basic validation - check for empty config
    if (config.getKeys().empty()) {
        errors.push_back("Configuration is empty");
    }
    
    // Check for valid key-value pairs
    for (const auto& key : config.getKeys()) {
        // TODO: Add more specific validation logic
        if (key.empty()) {
            errors.push_back("Empty key found");
        }
    }
    
    return errors;
}

void ConfigParser::mergeConfig(nlm::Config& target, const nlm::Config& source) {
    for (const auto& key : source.getKeys()) {
        // TODO: Get the actual value from source
        // This is a simplified version
        target.set(key, "[value]", nlm::ConfigSource::File);
    }
}

std::string ConfigParser::toLower(const std::string& str) {
    std::string result = str;
    std::transform(result.begin(), result.end(), result.begin(), ::tolower);
    return result;
}

bool ConfigParser::isJSON(const std::string& content) {
    // Simple heuristic - check for { } and , patterns
    size_t braceCount = std::count(content.begin(), content.end(), '{');
    size_t braceCloseCount = std::count(content.begin(), content.end(), '}');
    size_t commaCount = std::count(content.begin(), content.end(), ',');
    
    return braceCount > 0 && braceCloseCount > 0 && commaCount > 0;
}

bool ConfigParser::isYAML(const std::string& content) {
    // Simple heuristic - check for key: value patterns
    size_t colonCount = std::count(content.begin(), content.end(), ':');
    size_t dashCount = std::count(content.begin(), content.end(), '-');
    
    return colonCount > 0 && dashCount > 0;
}
