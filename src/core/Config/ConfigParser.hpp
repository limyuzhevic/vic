#pragma once

#include <nlohmann/json.hpp>
#include <yaml-cpp/yaml.h>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include "Config.hpp"

namespace nlm {

class ConfigParser {
public:
    static bool parseJsonFile(const std::string& filepath, std::map<std::string, ConfigValue>& entries);
    static bool parseYamlFile(const std::string& filepath, std::map<std::string, ConfigValue>& entries);
    static bool parseKeyValueFile(const std::string& filepath, std::map<std::string, ConfigValue>& entries);
    
    static ConfigSource detectFileSource(const std::string& filepath);
    static bool isJsonFile(const std::string& filepath);
    static bool isYamlFile(const std::string& filepath);
    static bool isKeyValueFile(const std::string& filepath);
    
    static ConfigValue jsonToConfigValue(const nlohmann::json& jsonValue);
    static ConfigValue yamlToConfigValue(const YAML::Node& yamlValue);
    static std::string configValueToString(const ConfigValue& value);
    
    static std::string getFileExtension(const std::string& filepath);
    static void trim(std::string& str);
    static bool isEmptyOrComment(const std::string& line);
};

// Implementation of ConfigParser

inline bool ConfigParser::parseJsonFile(const std::string& filepath, std::map<std::string, ConfigValue>& entries) {
    try {
        std::ifstream file(filepath);
        if (!file.is_open()) {
            return false;
        }
        
        nlohmann::json jsonData;
        file >> jsonData;
        
        // Convert JSON to Config entries
        for (auto it = jsonData.begin(); it != jsonData.end(); ++it) {
            std::string key = it.key();
            ConfigValue value = jsonToConfigValue(*it);
            entries[key] = value;
        }
        
        return true;
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Failed to parse JSON config file " + filepath + ": " + std::string(e.what()));
        return false;
    }
}

inline bool ConfigParser::parseYamlFile(const std::string& filepath, std::map<std::string, ConfigValue>& entries) {
    try {
        YAML::Node yamlData = YAML::LoadFile(filepath);
        
        // Convert YAML to Config entries
        for (auto it = yamlData.begin(); it != yamlData.end(); ++it) {
            std::string key = it->first.as<std::string>();
            ConfigValue value = yamlToConfigValue(*it);
            entries[key] = value;
        }
        
        return true;
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Failed to parse YAML config file " + filepath + ": " + std::string(e.what()));
        return false;
    }
}

inline bool ConfigParser::parseKeyValueFile(const std::string& filepath, std::map<std::string, ConfigValue>& entries) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    std::string line;
    int lineNum = 0;
    while (std::getline(file, line)) {
        lineNum++;
        
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
                ((value.front() == '"' && value.back() == '"') ||
                 (value.front() == '\'' && value.back() == '\''))) {
                value = value.substr(1, value.size() - 2);
            }
            
            entries[key] = value;
        }
    }
    
    return true;
}

inline ConfigSource ConfigParser::detectFileSource(const std::string& filepath) {
    if (isJsonFile(filepath)) {
        return ConfigSource::File;  // JSON files are from file
    } else if (isYamlFile(filepath)) {
        return ConfigSource::File;  // YAML files are from file
    } else if (isKeyValueFile(filepath)) {
        return ConfigSource::File;  // Key-value files are from file
    } else {
        return ConfigSource::Default;
    }
}

inline bool ConfigParser::isJsonFile(const std::string& filepath) {
    std::string ext = toLower(getFileExtension(filepath));
    return ext == ".json";
}

inline bool ConfigParser::isYamlFile(const std::string& filepath) {
    std::string ext = toLower(getFileExtension(filepath));
    return ext == ".yaml" || ext == ".yml";
}

inline bool ConfigParser::isKeyValueFile(const std::string& filepath) {
    // Check if it's a config file by default pattern or content
    std::string filename = getFileExtension(filepath);
    toLower(filename);
    return filename == ".cfg" || filename == ".config" || filename.empty();
}

inline ConfigValue ConfigParser::jsonToConfigValue(const nlohmann::json& jsonValue) {
    if (jsonValue.is_string()) {
        return jsonValue.get<std::string>();
    } else if (jsonValue.is_number_integer()) {
        return jsonValue.get<int64_t>();
    } else if (jsonValue.is_number_float()) {
        return jsonValue.get<double>();
    } else if (jsonValue.is_boolean()) {
        return jsonValue.get<bool>();
    } else if (jsonValue.is_array()) {
        // Handle arrays - determine element type
        std::vector<ConfigValue> result;
        for (const auto& item : jsonValue) {
            result.push_back(jsonToConfigValue(item));
        }
        
        // Try to determine array element type
        if (!result.empty()) {
            auto firstType = result[0].index();
            bool allSameType = true;
            for (const auto& item : result) {
                if (item.index() != firstType) {
                    allSameType = false;
                    break;
                }
            }
            
            if (allSameType) {
                if (firstType == typeid(int) || firstType == typeid(int64_t)) {
                    std::vector<int> intArray;
                    for (const auto& item : result) {
                        intArray.push_back(std::get<int>(item));
                    }
                    return intArray;
                } else if (firstType == typeid(double)) {
                    std::vector<double> doubleArray;
                    for (const auto& item : result) {
                        doubleArray.push_back(std::get<double>(item));
                    }
                    return doubleArray;
                } else if (firstType == typeid(std::string)) {
                    std::vector<std::string> stringArray;
                    for (const auto& item : result) {
                        stringArray.push_back(std::get<std::string>(item));
                    }
                    return stringArray;
                }
            }
        }
        
        // Fallback to generic vector<ConfigValue>
        return result;
    } else {
        // Default to string for unknown types
        return jsonValue.get<std::string>();
    }
}

inline ConfigValue ConfigParser::yamlToConfigValue(const YAML::Node& yamlValue) {
    if (yamlValue.IsScalar()) {
        if (yamlValue.IsIntegral()) {
            return yamlValue.as<int64_t>();
        } else if (yamlValue.IsDouble()) {
            return yamlValue.as<double>();
        } else if (yamlValue.IsBoolean()) {
            return yamlValue.as<bool>();
        } else {
            return yamlValue.as<std::string>();
        }
    } else if (yamlValue.IsSequence()) {
        std::vector<ConfigValue> result;
        for (const auto& item : yamlValue) {
            result.push_back(yamlToConfigValue(item));
        }
        
        // Try to determine array element type (similar to JSON)
        if (!result.empty()) {
            auto firstType = result[0].index();
            bool allSameType = true;
            for (const auto& item : result) {
                if (item.index() != firstType) {
                    allSameType = false;
                    break;
                }
            }
            
            if (allSameType) {
                if (firstType == typeid(int) || firstType == typeid(int64_t)) {
                    std::vector<int> intArray;
                    for (const auto& item : result) {
                        intArray.push_back(std::get<int>(item));
                    }
                    return intArray;
                } else if (firstType == typeid(double)) {
                    std::vector<double> doubleArray;
                    for (const auto& item : result) {
                        doubleArray.push_back(std::get<double>(item));
                    }
                    return doubleArray;
                } else if (firstType == typeid(std::string)) {
                    std::vector<std::string> stringArray;
                    for (const auto& item : result) {
                        stringArray.push_back(std::get<std::string>(item));
                    }
                    return stringArray;
                }
            }
        }
        
        return result;
    } else {
        return "";
    }
}

inline std::string ConfigParser::configValueToString(const ConfigValue& value) {
    std::ostringstream oss;
    std::visit([&oss](auto&& arg) {
        using T = std::decay_t<decltype(arg)>;
        if constexpr (std::is_same_v<T, std::string>) {
            oss << "\"" << arg << "\"";
        } else {
            oss << arg;
        }
    }, value);
    return oss.str();
}

inline std::string ConfigParser::getFileExtension(const std::string& filepath) {
    size_t pos = filepath.rfind('.');
    if (pos == std::string::npos) return "";
    return filepath.substr(pos);
}

inline void ConfigParser::trim(std::string& str) {
    size_t start = str.find_first_not_of(" \t\r\n");
    if (start == std::string::npos) {
        str.clear();
        return;
    }
    size_t end = str.find_last_not_of(" \t\r\n");
    str = str.substr(start, end - start + 1);
}

inline bool ConfigParser::isEmptyOrComment(const std::string& line) {
    std::string trimmed = line;
    trim(trimmed);
    return trimmed.empty() || trimmed[0] == '#' || trimmed[0] == '/';
}

} // namespace nlm
