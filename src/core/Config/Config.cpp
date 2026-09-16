// Config.cpp - Complete implementation with JSON/YAML support and improved error handling

#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <optional>
#include <iostream>

// Include for JSON/YAML parsing
#ifdef HAS_JSON
#include <nlohmann/json.hpp>
#endif

// Include for YAML parsing
#ifdef HAS_YAML
#include <yaml-cpp/yaml.h>
#endif

namespace nlm {

struct Config::Impl {
    std::vector<ConfigEntry> entries;
};

Config::Config() : pImpl(std::make_unique<Impl>()) {}

Config::~Config() = default;

Config::Config(Config&&) noexcept = default;

Config& Config::operator=(Config&&) noexcept = default;

// Helper function to parse key=value lines
bool Config::parseLine(const std::string& line) {
    std::string trimmed = trim(line);
    if (trimmed.empty() || trimmed[0] == '#' || trimmed[0] == '/') {
        return true; // Skip empty lines and comments
    }
    
    size_t pos = trimmed.find('=');
    if (pos == std::string::npos) {
        return false; // Invalid line format
    }
    
    std::string key = trim(trimmed.substr(0, pos));
    std::string value = trim(trimmed.substr(pos + 1));
    
    // Remove quotes if present
    if (value.size() >= 2 && 
        ((value.front() == '"' && value.back() == '"') ||
         (value.front() == '\'' && value.back() == '\''))) {
        value = value.substr(1, value.size() - 2);
    }
    
    // Convert boolean strings
    if (value == "true" || value == "false") {
        bool boolVal = (value == "true");
        set(key, boolVal, ConfigSource::File);
        return true;
    }
    
    // Convert numeric values
    try {
        if (value.find('.') != std::string::npos) {
            // Float
            float floatVal = std::stof(value);
            set(key, floatVal, ConfigSource::File);
        } else {
            // Integer
            int intVal = std::stoi(value);
            set(key, intVal, ConfigSource::File);
        }
        return true;
    } catch (...) {
        // Not a number, treat as string
        set(key, value, ConfigSource::File);
        return true;
    }
}

bool Config::loadFromFile(const std::string& filepath) {
    // Check file existence first
    if (!std::filesystem::exists(filepath)) {
        NLM_LOG_WARNING("Configuration file does not exist: " + filepath);
        return false;
    }
    
    std::ifstream file(filepath);
    if (!file.is_open()) {
        NLM_LOG_ERROR("Cannot open configuration file: " + filepath);
        return false;
    }
    
    // Try to determine file format
    std::string firstLine;
    std::getline(file, firstLine);
    file.clear();
    file.seekg(0);
    
    bool success;
    if (firstLine.find("{") == 0 || firstLine.find("[") == 0) {
        // JSON format
        success = loadFromJSONFile(filepath);
    } else if (firstLine.find("%") != std::string::npos && firstLine.find("---") != std::string::npos) {
        // YAML format
        success = loadFromYAMLFile(filepath);
    } else {
        // Legacy key=value format
        success = loadFromLegacyFormatFile(filepath);
    }
    
    if (!success) {
        NLM_LOG_ERROR("Failed to parse configuration file: " + filepath);
        return false;
    }
    
    NLM_LOG_INFO("Successfully loaded configuration from: " + filepath);
    return true;
}

bool Config::loadFromJSONFile(const std::string& filepath) {
#ifdef HAS_JSON
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    try {
        nlohmann::json jsonData;
        file >> jsonData;
        
        // Recursively parse JSON object
        parseJSON(jsonData, "");
        return true;
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("JSON parsing error: " + std::string(e.what()));
        return false;
    }
#else
    NLM_LOG_WARNING("JSON support not available - skipping JSON parsing");
    return false;
#endif
}

bool Config::loadFromYAMLFile(const std::string& filepath) {
#ifdef HAS_YAML
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    try {
        YAML::Node yamlData = YAML::LoadFile(filepath);
        
        // Recursively parse YAML node
        parseYAML(yamlData, "");
        return true;
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("YAML parsing error: " + std::string(e.what()));
        return false;
    }
#else
    NLM_LOG_WARNING("YAML support not available - skipping YAML parsing");
    return false;
#endif
}

void Config::parseJSON(const nlohmann::json& json, const std::string& prefix) {
    for (auto it = json.begin(); it != json.end(); ++it) {
        std::string key = prefix.empty() ? it.key() : prefix + "." + it.key();
        
        if (it->is_object()) {
            parseJSON(*it, key);
        } else if (it->is_array()) {
            // Handle arrays specially
            std::string arrayKey = key + "[]";
            parseJSONArray(*it, arrayKey);
        } else {
            setValueFromJSON(it.key(), *it, key, ConfigSource::File);
        }
    }
}

void Config::parseJSONArray(const nlohmann::json& array, const std::string& key) {
    // For now, just store as string representation
    // Better to support specific array types (int[], float[], string[])
    std::stringstream ss;
    ss << "[";
    for (size_t i = 0; i < array.size(); ++i) {
        if (i > 0) ss << ", ";
        ss << array[i];
    }
    ss << "]";
    
    set(key, ss.str(), ConfigSource::File);
}

void Config::setValueFromJSON(const std::string& jsonKey, const nlohmann::json& value, const std::string& fullKey, ConfigSource source) {
    if (value.is_string()) {
        set(fullKey, value.get<std::string>(), source);
    } else if (value.is_boolean()) {
        set(fullKey, value.get<bool>(), source);
    } else if (value.is_number_integer()) {
        // Try to detect if it should be int64_t or just int
        if (value.get<int64_t>() > INT_MAX) {
            set(fullKey, value.get<int64_t>(), source);
        } else {
            set(fullKey, value.get<int>(), source);
        }
    } else if (value.is_number_float()) {
        set(fullKey, value.get<double>(), source);
    } else {
        // Unknown type, store as string
        set(fullKey, value.dump(), source);
    }
}

void Config::parseYAML(const YAML::Node& yaml, const std::string& prefix) {
    for (auto it = yaml.begin(); it != yaml.end(); ++it) {
        std::string key = prefix.empty() ? it->first.as<std::string>() : prefix + "." + it->first.as<std::string>();
        
        if (it->second.IsMap()) {
            parseYAML(it->second, key);
        } else if (it->second.IsSequence()) {
            // Handle arrays specially
            std::string arrayKey = key + "[]";
            parseYAMLSequence(it->second, arrayKey);
        } else {
            setValueFromYAML(it->first.as<std::string>(), it->second, key, ConfigSource::File);
        }
    }
}

void Config::parseYAMLSequence(const YAML::Node& sequence, const std::string& key) {
    std::stringstream ss;
    ss << "[";
    for (size_t i = 0; i < sequence.size(); ++i) {
        if (i > 0) ss << ", ";
        ss << sequence[i];
    }
    ss << "]";
    
    set(key, ss.str(), ConfigSource::File);
}

void Config::setValueFromYAML(const std::string& yamlKey, const YAML::Node& value, const std::string& fullKey, ConfigSource source) {
    if (value.IsScalar()) {
        std::string strValue = value.as<std::string>();
        
        // Try to convert to appropriate type
        if (strValue == "true" || strValue == "false") {
            bool boolVal = (strValue == "true");
            set(fullKey, boolVal, source);
        } else {
            // Check if it's a number
            try {
                if (strValue.find('.') != std::string::npos) {
                    float floatVal = std::stof(strValue);
                    set(fullKey, floatVal, source);
                } else {
                    int intVal = std::stoi(strValue);
                    set(fullKey, intVal, source);
                }
            } catch (...) {
                // Not a number, keep as string
                set(fullKey, strValue, source);
            }
        }
    } else {
        set(fullKey, value.as<std::string>(), source);
    }
}

// Legacy key=value format parsing with improved error handling
bool Config::loadFromLegacyFormatFile(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    std::string line;
    size_t lineNum = 0;
    bool hasErrors = false;
    
    while (std::getline(file, line)) {
        ++lineNum;
        if (!parseLine(line)) {
            NLM_LOG_ERROR("Invalid configuration line " + std::to_string(lineNum) + ": " + line);
            hasErrors = true;
        }
    }
    
    if (hasErrors) {
        NLM_LOG_WARNING("Configuration file has parsing errors but was partially loaded");
    }
    
    return true;
}

// Enhanced summary with better formatting
std::string Config::summary() const {
    std::ostringstream oss;
    oss << "Configuration Summary (" << pImpl->entries.size() << " entries):\n";
    oss << "=" << std::string(60, '=') << "\n";
    
    for (const auto& entry : pImpl->entries) {
        oss << "Key: " << entry.key << "\n";
        oss << "  Source: " << static_cast<int>(entry.source) << "\n";
        oss << "  Description: " << entry.description << "\n";
        
        oss << "  Value: ";
        std::visit([&oss](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, std::string>) {
                oss << "\"" << arg << "\"";
            } else if constexpr (std::is_same_v<T, bool>) {
                oss << (arg ? "true" : "false");
            } else {
                oss << arg;
            }
        }, entry.value);
        oss << "\n";
        oss << "---" << std::string(60, '-') << "\n";
    }
    
    return oss.str();
}

// Explicit template instantiations
template std::optional<int> Config::get<int>(const std::string&) const;
template std::optional<int64_t> Config::get<int64_t>(const std::string&) const;
template std::optional<double> Config::get<double>(const std::string&) const;
template std::optional<bool> Config::get<bool>(const std::string&) const;
template std::optional<std::string> Config::get<std::string>(const std::string&) const;

template int Config::getOr<int>(const std::string&, const int&) const;
template int64_t Config::getOr<int64_t>(const std::string&, const int64_t&) const;
template double Config::getOr<double>(const std::string&, const double&) const;
template bool Config::getOr<bool>(const std::string&, const bool&) const;
template std::string Config::getOr<std::string>(const std::string&, const std::string&) const;

} // namespace nlm