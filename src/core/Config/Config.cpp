#include <nlohmann/json.hpp>
#include <yaml-cpp/yaml.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <string>
#include <vector>
#include <map>
#include <variant>
#include <optional>

namespace nlm {

// Forward declarations for nlohmann::json and YAML::Node
using json = nlohmann::json;
using YAML::Node;

// Update Config class to support JSON/YAML

bool Config::loadFromFile(const std::string& filepath) {
    std::string extension = getFileExtension(filepath);
    
    if (extension == ".json") {
        return loadFromJSON(filepath);
    } else if (extension == ".yaml" || extension == ".yml") {
        return loadFromYAML(filepath);
    } else {
        // Fallback to simple key=value format for compatibility
        return loadFromSimpleFormat(filepath);
    }
}

bool Config::loadFromJSON(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        NLM_LOG_ERROR("Failed to open JSON file: " + filepath);
        return false;
    }
    
    try {
        json jsonData;
        file >> jsonData;
        clear();
        
        parseJSON(jsonData);
        
        NLM_LOG_INFO("Successfully loaded configuration from JSON: " + filepath);
        return true;
        
    } catch (const json::parse_error& e) {
        NLM_LOG_ERROR("JSON parse error in " + filepath + ": " + std::string(e.what()));
        return false;
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Error reading JSON file " + filepath + ": " + std::string(e.what()));
        return false;
    }
}

bool Config::loadFromYAML(const std::string& filepath) {
    try {
        YAML::Node yamlData = YAML::LoadFile(filepath);
        clear();
        
        parseYAML(yamlData);
        
        NLM_LOG_INFO("Successfully loaded configuration from YAML: " + filepath);
        return true;
        
    } catch (const YAML::Exception& e) {
        NLM_LOG_ERROR("YAML parse error in " + filepath + ": " + std::string(e.what()));
        return false;
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Error reading YAML file " + filepath + ": " + std::string(e.what()));
        return false;
    }
}

void Config::parseJSON(const json& jsonData) {
    if (!jsonData.is_object()) {
        NLM_LOG_WARN("JSON root element is not an object, treating as single key-value pair");
        // If it's a string, use it as the only key
        if (jsonData.is_string()) {
            set("value", jsonData.get<std::string>());
        }
        return;
    }
    
    for (const auto& [key, value] : jsonData.items()) {
        setFromJSONValue(key, value);
    }
}

void Config::parseYAML(const Node& yamlData) {
    if (!yamlData.IsMap()) {
        NLM_LOG_WARN("YAML root element is not a map, treating as single key-value pair");
        // If it's a scalar, use it as the only key
        if (yamlData.IsScalar()) {
            set("value", yamlData.Scalar());
        }
        return;
    }
    
    for (const auto& pair : yamlData) {
        std::string key = pair.first.as<std::string>();
        YAML::Node value = pair.second;
        setFromYAMLValue(key, value);
    }
}

void Config::setFromJSONValue(const std::string& key, const json& value) {
    ConfigValue configValue;
    
    if (value.is_string()) {
        configValue = value.get<std::string>();
    } else if (value.is_number_integer()) {
        configValue = value.get<int64_t>();
    } else if (value.is_number_unsigned()) {
        configValue = value.get<uint64_t>();
    } else if (value.is_number_float()) {
        configValue = value.get<double>();
    } else if (value.is_boolean()) {
        configValue = value.get<bool>();
    } else if (value.is_array()) {
        // Convert JSON array to string representation for now
        // TODO: Implement proper array support
        std::vector<std::string> arrayStr;
        for (const auto& item : value) {
            arrayStr.push_back(item.dump());
        }
        configValue = arrayToString(arrayStr);
    } else if (value.is_object()) {
        // Convert JSON object to string representation
        // TODO: Implement proper object support
        configValue = value.dump();
    } else {
        configValue = "";
    }
    
    set(key, configValue, ConfigSource::File);
}

void Config::setFromYAMLValue(const std::string& key, const Node& value) {
    ConfigValue configValue;
    
    if (value.IsScalar()) {
        std::string scalarValue = value.as<std::string>();
        
        // Try to detect type
        if (isNumericString(scalarValue)) {
            if (hasDecimalPoint(scalarValue)) {
                configValue = std::stod(scalarValue);
            } else {
                try {
                    configValue = std::stoll(scalarValue);
                } catch (...) {
                    configValue = std::stod(scalarValue);
                }
            }
        } else if (scalarValue == "true" || scalarValue == "false") {
            configValue = (scalarValue == "true");
        } else {
            configValue = scalarValue;
        }
    } else if (value.IsSequence()) {
        // Convert YAML sequence to string representation
        std::vector<std::string> arrayStr;
        for (const auto& item : value) {
            if (item.IsScalar()) {
                arrayStr.push_back(item.as<std::string>());
            }
        }
        configValue = arrayToString(arrayStr);
    } else if (value.IsMap()) {
        // Convert YAML map to string representation
        std::vector<std::string> mapStr;
        for (const auto& pair : value) {
            std::string item = pair.first.as<std::string>() + ": " + (pair.second.IsScalar() ? pair.second.as<std::string>() : pair.second.as<std::string>());
            mapStr.push_back(item);
        }
        configValue = arrayToString(mapStr);
    } else {
        configValue = "";
    }
    
    set(key, configValue, ConfigSource::File);
}

std::string Config::getFileExtension(const std::string& filepath) {
    size_t pos = filepath.find_last_of('.');
    if (pos == std::string::npos) {
        return "";
    }
    std::string extension = filepath.substr(pos + 1);
    for (char& c : extension) {
        c = std::tolower(c);
    }
    return "." + extension;
}

bool Config::isNumericString(const std::string& str) {
    if (str.empty()) return false;
    
    size_t start = 0;
    if (str[0] == '-' || str[0] == '+') {
        start = 1;
    }
    
    for (size_t i = start; i < str.size(); ++i) {
        if (!std::isdigit(str[i])) {
            if (str[i] == '.' && i > start && i < str.size() - 1) {
                continue; // Allow decimal point
            }
            return false;
        }
    }
    return true;
}

bool Config::hasDecimalPoint(const std::string& str) {
    return str.find('.') != std::string::npos;
}

std::string Config::arrayToString(const std::vector<std::string>& items) {
    std::ostringstream oss;
    oss << "[";
    for (size_t i = 0; i < items.size(); ++i) {
        if (i > 0) oss << ", ";
        oss << "\"" << items[i] << "\"";
    }
    oss << "]";
    return oss.str();
}

bool Config::saveToJSON(const std::string& filepath) const {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        NLM_LOG_ERROR("Failed to create JSON file: " + filepath);
        return false;
    }
    
    try {
        json jsonData;
        jsonData["config"] = "NLM Configuration";
        jsonData["version"] = "2.0";
        
        for (const auto& entry : pImpl->entries) {
            jsonData[entry.key] = getValueAsString(entry.value);
        }
        
        file << jsonData.dump(2); // Pretty print with 2-space indent
        
        NLM_LOG_INFO("Configuration saved to JSON: " + filepath);
        return true;
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Error writing JSON file " + filepath + ": " + std::string(e.what()));
        return false;
    }
}

bool Config::saveToYAML(const std::string& filepath) const {
    try {
        YAML::Emitter emitter;
        emitter << YAML::BeginMap;
        emitter << YAML::Key << "config" << YAML::Value << "NLM Configuration";
        emitter << YAML::Key << "version" << YAML::Value << "2.0";
        
        for (const auto& entry : pImpl->entries) {
            emitter << YAML::Key << entry.key;
            emitter << YAML::Value << getValueAsString(entry.value);
        }
        
        emitter << YAML::EndMap;
        
        std::ofstream file(filepath);
        if (!file.is_open()) {
            NLM_LOG_ERROR("Failed to create YAML file: " + filepath);
            return false;
        }
        
        file << emitter.c_str();
        
        NLM_LOG_INFO("Configuration saved to YAML: " + filepath);
        return true;
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Error writing YAML file " + filepath + ": " + std::string(e.what()));
        return false;
    }
}

std::string Config::getValueAsString(const ConfigValue& value) const {
    std::ostringstream oss;
    std::visit([&oss](auto&& arg) {
        using T = std::decay_t<decltype(arg)>;
        if constexpr (std::is_same_v<T, std::string>) {
            oss << arg;
        } else if constexpr (std::is_same_v<T, int64_t> || std::is_same_v<T, uint64_t>) {
            oss << arg;
        } else if constexpr (std::is_same_v<T, double>) {
            oss << std::fixed << std::setprecision(6) << arg;
        } else if constexpr (std::is_same_v<T, bool>) {
            oss << (arg ? "true" : "false");
        } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
            oss << "[";
            for (size_t i = 0; i < arg.size(); ++i) {
                if (i > 0) oss << ", ";
                oss << "\"" << arg[i] << "\"";
            }
            oss << "]";
        }
    }, value);
    return oss.str();
}

// Legacy function for backward compatibility
bool Config::loadFromSimpleFormat(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        NLM_LOG_WARN("Failed to open config file for simple parsing: " + filepath);
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
    
    NLM_LOG_INFO("Loaded configuration from simple format: " + filepath);
    return true;
}

bool Config::saveToFile(const std::string& filepath) const {
    std::string extension = getFileExtension(filepath);
    
    if (extension == ".json") {
        return saveToJSON(filepath);
    } else if (extension == ".yaml" || extension == ".yml") {
        return saveToYAML(filepath);
    } else {
        // Fallback to simple format for compatibility
        return saveToSimpleFormat(filepath);
    }
}

bool Config::saveToSimpleFormat(const std::string& filepath) const {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        NLM_LOG_ERROR("Failed to create config file: " + filepath);
        return false;
    }
    
    file << "# NLM Configuration File\n";
    file << "# Generated by NLM Config System\n\n";
    
    for (const auto& entry : pImpl->entries) {
        file << "# " << entry.description << "\n";
        file << entry.key << " = " << getValueAsString(entry.value) << "\n\n";
    }
    
    NLM_LOG_INFO("Configuration saved to simple format: " + filepath);
    return true;
}

// Additional utility functions
void Config::setDescription(const std::string& key, const std::string& description) {
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    
    if (it != pImpl->entries.end()) {
        it->description = description;
    }
}

std::string Config::getDescription(const std::string& key) const {
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    
    if (it != pImpl->entries.end()) {
        return it->description;
    }
    
    return "";
}

// Validation functions
bool Config::validate() const {
    // Basic validation - can be extended with specific validation rules
    for (const auto& entry : pImpl->entries) {
        if (entry.key.empty()) {
            NLM_LOG_WARN("Config entry has empty key");
            continue;
        }
        
        // Check for duplicate keys
        size_t count = std::count_if(pImpl->entries.begin(), pImpl->entries.end(),
            [&entry](const ConfigEntry& e) { return e.key == entry.key; });
        
        if (count > 1) {
            NLM_LOG_WARN("Duplicate config key: " + entry.key);
        }
    }
    
    return true;
}

bool Config::merge(const Config& other) {
    bool changed = false;
    
    for (const auto& entry : other.pImpl->entries) {
        // Check if key already exists
        auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
            [&entry](const ConfigEntry& e) { return e.key == entry.key; });
        
        if (it == pImpl->entries.end()) {
            // New key, add it
            pImpl->entries.push_back(entry);
            changed = true;
        } else if (it->source == ConfigSource::File && entry.source != ConfigSource::File) {
            // Override non-file entry with file entry
            *it = entry;
            changed = true;
        } else if (entry.source == ConfigSource::CommandLine) {
            // Always override with command line
            *it = entry;
            changed = true;
        }
    }
    
    return changed;
}

void Config::dump() const {
    NLM_LOG_INFO("=== Configuration Dump ===");
    for (const auto& entry : pImpl->entries) {
        NLM_LOG_INFO("Key: " + entry.key + ", Value: " + getValueAsString(entry.value) + 
                    ", Source: " + std::to_string(static_cast<int>(entry.source)));
    }
    NLM_LOG_INFO("=== End Configuration Dump ===");
}

}} // namespace nlm