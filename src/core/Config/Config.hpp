#pragma once

#include <string>
#include <vector>
#include <unordered_map>
#include <stdexcept>
#include <sstream>
#include <algorithm>
#include <cctype>

namespace nlm {

// Simple JSON parsing implementation (no external dependencies)
class SimpleJSON {
public:
    struct Value {
        enum Type { Null, Bool, Number, String, Array, Object };
        Type type;
        std::string asString;
        double asNumber;
        bool asBool;
        std::vector<Value> asArray;
        std::unordered_map<std::string, Value> asObject;
        
        Value() : type(Null), asNumber(0.0), asBool(false) {}
    };
    
    static bool parse(const std::string& json, Value& result);
    static std::string serialize(const Value& value);
};

// Validation exception
class ConfigException : public std::runtime_error {
public:
    explicit ConfigException(const std::string& message) : std::runtime_error(message) {}
};

class ValidationException : public ConfigException {
public:
    explicit ValidationException(const std::string& message) : ConfigException(message) {}
};

// Configuration value types (same as before)
enum class ConfigSource {
    Default,
    File,
    CommandLine,
    Runtime
};

struct ConfigEntry {
    std::string key;
    std::variant<int, int64_t, double, bool, std::string> value;
    ConfigSource source;
    std::string description;
    
    ConfigEntry() : key(), value(), source(ConfigSource::Default), description() {}
    ConfigEntry(const std::string& k, const std::variant<int, int64_t, double, bool, std::string>& v, ConfigSource s, const std::string& desc = "")
        : key(k), value(v), source(s), description(desc) {}
};

class Config {
public:
    Config();
    ~Config();
    
    // Disable copying, enable moving
    Config(const Config&) = delete;
    Config& operator=(const Config&) = delete;
    Config(Config&&) noexcept;
    Config& operator=(Config&&) noexcept;
    
    // Load from file (JSON format)
    bool loadFromFile(const std::string& filepath);
    
    // Load from command line arguments
    bool loadFromArgs(int argc, char** argv);
    
    // Save to file
    bool saveToFile(const std::string& filepath) const;
    
    // Get values
    template<typename T>
    std::optional<T> get(const std::string& key) const;
    
    template<typename T>
    T getOr(const std::string& key, const T& defaultValue) const;
    
    // Set values
    void set(const std::string& key, const ConfigValue& value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, const std::string& value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, int value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, double value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, bool value, ConfigSource source = ConfigSource::Runtime);
    
    // Check existence
    bool has(const std::string& key) const;
    
    // Remove key
    void remove(const std::string& key);
    
    // Get all keys
    std::vector<std::string> getKeys() const;
    
    // Clear all
    void clear();
    
    // Get configuration summary
    std::string summary() const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Internal helpers
    static std::string trim(const std::string& str);
    static std::string toLower(const std::string& str);
    
    // Validation functions
    static bool isValidNumber(double value, double min, double max);
    static bool isValidCount(uint64_t value, uint64_t min, uint64_t max);
    static bool validateConfigData(const SimpleJSON::Value& jsonData, std::vector<std::string>& errors);
};

// Simple JSON implementation
namespace {
    bool SimpleJSON::parse(const std::string& json, Value& result) {
        // Very basic JSON parsing - only handles simple objects for demonstration
        // In a real implementation, you would use a proper JSON library or more robust parser
        
        // Remove whitespace
        std::string cleanJson;
        for (char c : json) {
            if (!std::isspace(c)) cleanJson += c;
        }
        
        if (cleanJson.empty() || cleanJson[0] != '{') {
            return false;
        }
        
        // Parse object
        result.type = Value::Object;
        size_t pos = 1;
        while (pos < cleanJson.size() && cleanJson[pos] != '}') {
            // Find key
            size_t keyStart = cleanJson.find('"', pos);
            if (keyStart == std::string::npos) break;
            size_t keyEnd = cleanJson.find('"', keyStart + 1);
            if (keyEnd == std::string::npos) break;
            
            std::string key = cleanJson.substr(keyStart + 1, keyEnd - keyStart - 1);
            
            // Find value start
            size_t valueStart = cleanJson.find(':', keyEnd);
            if (valueStart == std::string::npos) break;
            size_t valueEnd = cleanJson.find(',', valueStart);
            if (valueEnd == std::string::npos) {
                valueEnd = cleanJson.find('}', valueStart);
                if (valueEnd == std::string::npos) break;
            }
            
            std::string valueStr = cleanJson.substr(valueStart + 1, valueEnd - valueStart - 1);
            
            // Parse value based on content
            Value value;
            if (valueStr.front() == '"' && valueStr.back() == '"') {
                // String
                value.type = Value::String;
                value.asString = valueStr.substr(1, valueStr.size() - 2);
            } else if (valueStr == "true" || valueStr == "false") {
                // Boolean
                value.type = Value::Bool;
                value.asBool = (valueStr == "true");
            } else {
                // Number
                try {
                    value.type = Value::Number;
                    value.asNumber = std::stod(valueStr);
                } catch (...) {
                    value.type = Value::Null;
                }
            }
            
            result.asObject[key] = value;
            pos = valueEnd + 1;
        }
        
        return true;
    }
    
    std::string SimpleJSON::serialize(const Value& value) {
        std::ostringstream oss;
        switch (value.type) {
            case Value::Null:
                oss << "null";
                break;
            case Value::Bool:
                oss << (value.asBool ? "true" : "false");
                break;
            case Value::Number:
                oss << value.asNumber;
                break;
            case Value::String:\n                oss << "\"" << value.asString << "\"";
                break;
            case Value::Array:
                oss << "[";
                for (size_t i = 0; i < value.asArray.size(); ++i) {
                    if (i > 0) oss << ", ";
                    oss << serialize(value.asArray[i]);
                }
                oss << "]";
                break;
            case Value::Object:
                oss << "{";
                size_t i = 0;
                for (const auto& pair : value.asObject) {
                    if (i > 0) oss << ", ";
                    oss << "\"" << pair.first << "\": " << serialize(pair.second);
                    ++i;
                }
                oss << "}";
                break;
        }
        return oss.str();
    }
}

struct Config::Impl {
    std::vector<ConfigEntry> entries;
    int version = 1;
};

Config::Config() : pImpl(std::make_unique<Impl>()) {}

Config::~Config() = default;

Config::Config(Config&&) noexcept = default;

Config& Config::operator=(Config&&) noexcept = default;

bool Config::loadFromFile(const std::string& filepath) {
    try {
        // Read file
        std::ifstream file(filepath);
        if (!file.is_open()) {
            throw ConfigException("Cannot open configuration file: " + filepath);
        }
        
        // Read entire file
        std::stringstream buffer;
        buffer << file.rdbuf();
        std::string content = buffer.str();
        
        // Parse JSON
        SimpleJSON::Value jsonData;
        if (!SimpleJSON::parse(content, jsonData)) {
            throw ConfigException("Invalid JSON in configuration file: " + filepath);
        }
        
        // Check version
        auto versionIt = jsonData.asObject.find("__config_version");
        if (versionIt != jsonData.asObject.end() && versionIt->second.type == SimpleJSON::Value::Number) {
            int fileVersion = static_cast<int>(versionIt->second.asNumber);
            if (fileVersion != pImpl->version) {
                throw ValidationException("Configuration version mismatch: file version " + 
                                        std::to_string(fileVersion) + ", expected " + 
                                        std::to_string(pImpl->version));
            }
        }
        
        // Clear existing entries
        clear();
        
        // Convert JSON to Config entries (simplified for demonstration)
        auto entriesIt = jsonData.asObject.find("entries");
        if (entriesIt != jsonData.asObject.end()) {
            const SimpleJSON::Value& entriesValue = entriesIt->second;
            // In a real implementation, you would properly parse the entries
            // For now, just add the basic entries
            if (entriesValue.type == SimpleJSON::Value::Array) {
                // Simplified: just use basic parameters from the main object
                auto neuronCountIt = jsonData.asObject.find("neuron_count");
                if (neuronCountIt != jsonData.asObject.end() && neuronCountIt->second.type == SimpleJSON::Value::Number) {
                    set("neuron_count", static_cast<int>(neuronCountIt->second.asNumber), ConfigSource::File);
                }
                
                auto regionCountIt = jsonData.asObject.find("region_count");
                if (regionCountIt != jsonData.asObject.end() && regionCountIt->second.type == SimpleJSON::Value::Number) {
                    set("region_count", static_cast<int>(regionCountIt->second.asNumber), ConfigSource::File);
                }
                
                auto populationIt = jsonData.asObject.find("population_per_region");
                if (populationIt != jsonData.asObject.end() && populationIt->second.type == SimpleJSON::Value::Number) {
                    set("population_per_region", static_cast<int>(populationIt->second.asNumber), ConfigSource::File);
                }
            }
        }
        
        // Validate configuration
        std::vector<std::string> errors;
        if (!validateConfigData(jsonData, errors)) {
            std::string errorMsg = "Configuration validation failed:\n";
            for (const auto& error : errors) {
                errorMsg += "  - " + error + "\n";
            }
            throw ValidationException(errorMsg);
        }
        
        return true;
        
    } catch (const ConfigException& e) {
        // Re-throw configuration-specific exceptions
        throw;
    } catch (const std::exception& e) {
        throw ConfigException("Error loading configuration: " + std::string(e.what()));
    }
}

bool Config::loadFromArgs(int argc, char** argv) {
    for (int i = 1; i < argc; ++i) {
        std::string arg(argv[i]);
        
        // Handle --key=value format
        if (arg.substr(0, 2) == "--") {
            size_t pos = arg.find('=');
            if (pos != std::string::npos) {
                std::string key = arg.substr(2, pos - 2);
                std::string value = arg.substr(pos + 1);
                set(key, value, ConfigSource::CommandLine);
            }
        }
        // Handle -key value format
        else if (arg[0] == '-' && i + 1 < argc) {
            std::string key = arg.substr(1);
            std::string value = argv[++i];
            set(key, value, ConfigSource::CommandLine);
        }
    }
    return true;
}

bool Config::saveToFile(const std::string& filepath) const {
    try {
        std::ofstream file(filepath);
        if (!file.is_open()) {
            return false;
        }
        
        // Create simple JSON structure
        std::ostringstream oss;
        oss << "{\n";
        oss << "  \"__config_version\": " << pImpl->version << ",\n";
        oss << "  \"entries\": [\n";
        
        for (size_t i = 0; i < pImpl->entries.size(); ++i) {
            const auto& entry = pImpl->entries[i];
            oss << "    {\n";
            oss << "      \"key\": \"" << entry.key << "\",\n";
            oss << "      \"description\": \"" << entry.description << "\"\n";
            
            // Add source
            oss << "    }";
            if (i < pImpl->entries.size() - 1) oss << ",\n";
            else oss << "\n";
        }
        
        oss << "  ],\n";
        oss << "  \"neuron_count\": 1000,\n";
        oss << "  \"region_count\": 1,\n";
        oss << "  \"population_per_region\": 100\n";
        oss << "}\n";
        
        file << oss.str();
        
        return true;
        
    } catch (const std::exception& e) {
        return false;
    }
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
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    
    if (it != pImpl->entries.end()) {
        it->value = value;
        it->source = source;
    } else {
        pImpl->entries.emplace_back(key, value, source);
    }
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
}

std::vector<std::string> Config::getKeys() const {
    std::vector<std::string> keys;
    keys.reserve(pImpl->entries.size());
    for (const auto& entry : pImpl->entries) {
        keys.push_back(entry.key);
    }
    return keys;
}

void Config::clear() {
    pImpl->entries.clear();
}

// Validation implementation
bool Config::isValidNumber(double value, double min, double max) {
    return !std::isnan(value) && value >= min && value <= max;
}

bool Config::isValidCount(uint64_t value, uint64_t min, uint64_t max) {
    return value >= min && value <= max;
}

bool Config::validateConfigData(const SimpleJSON::Value& jsonData, std::vector<std::string>& errors) {
    // Check required parameters
    auto it = jsonData.asObject.find("neuron_count");
    if (it == jsonData.asObject.end()) {
        errors.push_back("Missing required parameter: neuron_count");
    }
    
    it = jsonData.asObject.find("region_count");
    if (it == jsonData.asObject.end()) {
        errors.push_back("Missing required parameter: region_count");
    }
    
    it = jsonData.asObject.find("population_per_region");
    if (it == jsonData.asObject.end()) {
        errors.push_back("Missing required parameter: population_per_region");
    }
    
    // Validate if parameters are present
    it = jsonData.asObject.find("neuron_count");
    if (it != jsonData.asObject.end() && it->second.type == SimpleJSON::Value::Number) {
        if (!isValidCount(static_cast<uint64_t>(it->second.asNumber), 1, 1000000)) {
            errors.push_back("Invalid neuron_count: must be between 1 and 1,000,000");
        }
    }
    
    it = jsonData.asObject.find("region_count");
    if (it != jsonData.asObject.end() && it->second.type == SimpleJSON::Value::Number) {
        if (!isValidCount(static_cast<uint64_t>(it->second.asNumber), 1, 100)) {
            errors.push_back("Invalid region_count: must be between 1 and 100");
        }
    }
    
    it = jsonData.asObject.find("population_per_region");
    if (it != jsonData.asObject.end() && it->second.type == SimpleJSON::Value::Number) {
        if (!isValidCount(static_cast<uint64_t>(it->second.asNumber), 1, 10000)) {
            errors.push_back("Invalid population_per_region: must be between 1 and 10,000");
        }
    }
    
    return errors.empty();
}

std::string Config::summary() const {
    std::ostringstream oss;
    oss << "Configuration (version " << pImpl->version << ") (" << pImpl->entries.size() << " entries):\n";
    for (const auto& entry : pImpl->entries) {
        oss << "  " << entry.key << " = [";
        std::visit([&oss](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, std::string>) {
                oss << "\"" << arg << "\"";
            } else {
                oss << arg;
            }
        }, entry.value);
        oss << "] (" << static_cast<int>(entry.source) << ")\n";
    }
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
