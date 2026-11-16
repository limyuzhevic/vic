// Enhanced Config Implementation
// Implementation of the unified configuration system with validation

#include "Config.hpp"
#include <nlohmann/json.hpp>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <cctype>
#include <iostream>

namespace nlm {

// Config implementation
Config::Config(const ConfigSchema& schema) : schema(schema) {
    // Initialize with default values from schema
    for (const auto& field : schema.fields) {
        entries.push_back({field.key, field.defaultValue, Source::Default, field.description});
    }
}

Config::~Config() = default;

Config::Config(Config&& other) noexcept : 
    pImpl(std::move(other.pImpl)), 
    schema(std::move(other.schema)),
    validationLevel(other.validationLevel),
    entries(std::move(other.entries)) {
}

Config& Config::operator=(Config&& other) noexcept {
    if (this != &other) {
        pImpl = std::move(other.pImpl);
        schema = std::move(other.schema);
        validationLevel = other.validationLevel;
        entries = std::move(other.entries);
    }
    return *this;
}

bool Config::loadFromFile(const std::string& filepath, ValidationLevel validationLevel) {
    setValidationLevel(validationLevel);
    
    std::ifstream file(filepath);
    if (!file.is_open()) {
        throw ConfigError("Cannot open configuration file: " + filepath);
    }
    
    std::stringstream buffer;
    buffer << file.rdbuf();
    
    file.close();
    return loadFromJson(buffer.str(), validationLevel);
}

bool Config::loadFromJson(const std::string& jsonString, ValidationLevel validationLevel) {
    setValidationLevel(validationLevel);
    
    try {
        nlohmann::json jsonData = nlohmann::json::parse(jsonString);
        
        // Convert JSON to configuration entries
        for (auto it = jsonData.begin(); it != jsonData.end(); ++it) {
            std::string key = it.key();
            std::any value = jsonToAny(*it);
            
            // Find if key already exists
            auto entryIt = findEntry(key);
            if (entryIt != entries.end()) {
                entryIt->value = value;
                entryIt->source = Source::File;
                entryIt->validated = false;
            } else {
                entries.push_back({key, value, Source::File, "Loaded from JSON"});
            }
        }
        
        return validate(validationLevel);
    } catch (const nlohmann::json::exception& e) {
        throw ConfigError("Invalid JSON in configuration: " + std::string(e.what()));
    }
}

bool Config::loadFromConfig(const Config& other) {
    entries = other.entries;
    return validate(validationLevel);
}

bool Config::saveToFile(const std::string& filepath) const {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        throw ConfigError("Cannot write configuration file: " + filepath);
    }
    
    nlohmann::json jsonData;
    
    for (const auto& entry : entries) {
        jsonData[entry.key] = anyToJson(entry.value);
    }
    
    file << jsonData.dump(2); // Pretty print with 2-space indent
    
    return true;
}

template<typename T>
void Config::set(const std::string& key, const T& value, Source source,
                 const std::string& description) {
    // Validate type
    if (!std::is_same_v<T, int> && 
        !std::is_same_v<T, int64_t> && 
        !std::is_same_v<T, double> && 
        !std::is_same_v<T, bool> && 
        !std::is_same_v<T, std::string> &&
        !std::is_same_v<T, std::vector<int>> &&
        !std::is_same_v<T, std::vector<double>> &&
        !std::is_same_v<T, std::vector<std::string>>) {
        throw ConfigError("Unsupported type for configuration key: " + key);
    }
    
    auto entryIt = findEntry(key);
    if (entryIt != entries.end()) {
        entryIt->value = value;
        entryIt->source = source;
        entryIt->description = description.empty() ? entryIt->description : description;
        entryIt->validated = false;
    } else {
        entries.push_back({key, value, source, description});
    }
}

void Config::set(const std::string& key, const std::any& value, Source source,
                 const std::string& description) {
    auto entryIt = findEntry(key);
    if (entryIt != entries.end()) {
        entryIt->value = value;
        entryIt->source = source;
        entryIt->description = description.empty() ? entryIt->description : description;
        entryIt->validated = false;
    } else {
        entries.push_back({key, value, source, description});
    }
}

bool Config::has(const std::string& key) const {
    return findEntry(key) != entries.end();
}

void Config::remove(const std::string& key) {
    auto it = std::remove_if(entries.begin(), entries.end(),
                            [&key](const Entry& entry) { return entry.key == key; });
    entries.erase(it, entries.end());
}

std::vector<std::string> Config::getKeys() const {
    std::vector<std::string> keys;
    for (const auto& entry : entries) {
        keys.push_back(entry.key);
    }
    return keys;
}

void Config::clear() {
    entries.clear();
}

std::string Config::summary() const {
    std::stringstream ss;
    ss << "Configuration Summary (" << entries.size() << " entries):" << std::endl;
    
    for (const auto& entry : entries) {
        ss << "  " << entry.key << " (" << sourceToString(entry.source) << "):";
        
        if (entry.value.has_value()) {
            if (entry.value.type() == typeid(int)) {
                ss << " " << std::any_cast<int>(entry.value);
            } else if (entry.value.type() == typeid(int64_t)) {
                ss << " " << std::any_cast<int64_t>(entry.value);
            } else if (entry.value.type() == typeid(double)) {
                ss << " " << std::any_cast<double>(entry.value);
            } else if (entry.value.type() == typeid(bool)) {
                ss << " " << (std::any_cast<bool>(entry.value) ? "true" : "false");
            } else if (entry.value.type() == typeid(std::string)) {
                ss << " '" << std::any_cast<std::string>(entry.value) << "'";
            } else {
                ss << " <value>";
            }
        }
        
        ss << " - " << entry.description << std::endl;
    }
    
    return ss.str();
}

bool Config::validate(ValidationLevel level) const {
    // Basic validation
    for (const auto& entry : entries) {
        // Type validation based on value type
        if (!entry.value.has_value()) {
            continue;
        }
        
        // Check for any type-specific validation rules
        // This would be extended with actual validation logic
    }
    
    return true;
}

void Config::merge(const Config& other) {
    for (const auto& entry : other.entries) {
        auto existingIt = findEntry(entry.key);
        if (existingIt != entries.end()) {
            // Prefer runtime values over file values
            if (entry.source == Source::Runtime || 
                (existingIt->source != Source::Runtime && entry.source != Source::Default)) {
                *existingIt = entry;
            }
        } else {
            entries.push_back(entry);
        }
    }
}

Config Config::clone() const {
    Config cloneConfig(schema);
    cloneConfig.entries = entries;
    cloneConfig.validationLevel = validationLevel;
    return cloneConfig;
}

std::map<std::string, std::string> Config::loadEnvironmentVariables(const std::string& prefix) {
    std::map<std::string, std::string> envVars;
    
    // This would be implemented with actual environment variable reading
    // For now, return empty map
    return envVars;
}

bool Config::loadFromCommandLine(int argc, char** argv, const std::string& prefix) {
    // This would be implemented with actual command line argument parsing
    // For now, return false
    return false;
}

// Helper methods
Config::Entry* Config::findEntry(const std::string& key) {
    for (auto& entry : entries) {
        if (entry.key == key) {
            return &entry;
        }
    }
    return nullptr;
}

const Config::Entry* Config::findEntry(const std::string& key) const {
    for (const auto& entry : entries) {
        if (entry.key == key) {
            return &entry;
        }
    }
    return nullptr;
}

std::string Config::sourceToString(Source source) const {
    switch (source) {
        case Source::Default: return "Default";
        case Source::File: return "File";
        case Source::CommandLine: return "CommandLine";
        case Source::Runtime: return "Runtime";
        case Source::Environment: return "Environment";
        default: return "Unknown";
    }
}

std::string Config::trim(const std::string& str) {
    size_t first = str.find_first_not_of(" \t\n\r");
    if (first == std::string::npos) return "";
    
    size_t last = str.find_last_not_of(" \t\n\r");
    return str.substr(first, (last - first + 1));
}

std::string Config::toLower(const std::string& str) {
    std::string result = str;
    for (char& c : result) {
        c = std::tolower(static_cast<unsigned char>(c));
    }
    return result;
}

std::string Config::toUpper(const std::string& str) {
    std::string result = str;
    for (char& c : result) {
        c = std::toupper(static_cast<unsigned char>(c));
    }
    return result;
}

bool Config::stringMatch(const std::string& str, const std::string& pattern) {
    // Simple pattern matching for configuration validation
    // This could be enhanced with regex support
    if (pattern.empty()) return true;
    if (pattern == "*") return true;
    
    return str == pattern;
}

// JSON conversion methods
std::any Config::jsonToAny(const nlohmann::json& json) {
    if (json.is_null()) {
        return std::any();
    } else if (json.is_boolean()) {
        return json.get<bool>();
    } else if (json.is_number_integer()) {
        return json.get<int64_t>();
    } else if (json.is_number()) {
        return json.get<double>();
    } else if (json.is_string()) {
        return json.get<std::string>();
    } else if (json.is_array()) {
        std::vector<std::any> result;
        for (const auto& item : json) {
            result.push_back(jsonToAny(item));
        }
        return result;
    } else if (json.is_object()) {
        std::map<std::string, std::any> result;
        for (auto it = json.begin(); it != json.end(); ++it) {
            result[it.key()] = jsonToAny(*it);
        }
        return result;
    }
    
    throw ConfigError("Unsupported JSON type for configuration conversion");
}

nlohmann::json Config::anyToJson(const std::any& value) {
    if (!value.has_value()) {
        return nullptr;
    } else if (value.type() == typeid(bool)) {
        return std::any_cast<bool>(value);
    } else if (value.type() == typeid(int)) {
        return std::any_cast<int>(value);
    } else if (value.type() == typeid(int64_t)) {
        return std::any_cast<int64_t>(value);
    } else if (value.type() == typeid(double)) {
        return std::any_cast<double>(value);
    } else if (value.type() == typeid(std::string)) {
        return std::any_cast<std::string>(value);
    } else if (value.type() == typeid(std::vector<int>)) {
        return std::any_cast<std::vector<int>>(value);
    } else if (value.type() == typeid(std::vector<double>)) {
        return std::any_cast<std::vector<double>>(value);
    } else if (value.type() == typeid(std::vector<std::string>)) {
        return std::any_cast<std::vector<std::string>>(value);
    } else if (value.type() == typeid(std::vector<std::any>)) {
        nlohmann::json result = nlohmann::json::array();
        const auto& array = std::any_cast<std::vector<std::any>>(value);
        for (const auto& item : array) {
            result.push_back(anyToJson(item));
        }
        return result;
    } else if (value.type() == typeid(std::map<std::string, std::any>)) {
        nlohmann::json result = nlohmann::json::object();
        const auto& object = std::any_cast<std::map<std::string, std::any>>(value);
        for (const auto& item : object) {
            result[item.first] = anyToJson(item.second);
        }
        return result;
    } else if (value.type() == typeid(ConfigTypes::Duration)) {
        const auto& duration = std::any_cast<ConfigTypes::Duration>(value);
        return duration.seconds;
    } else if (value.type() == typeid(ConfigTypes::Percentage)) {
        const auto& percentage = std::any_cast<ConfigTypes::Percentage>(value);
        return percentage.value;
    } else if (value.type() == typeid(ConfigTypes::Probability)) {
        const auto& probability = std::any_cast<ConfigTypes::Probability>(value);
        return probability.value;
    }
    
    throw ConfigError("Unsupported type for JSON conversion");
}

} // namespace nlm
