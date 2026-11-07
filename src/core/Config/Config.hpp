// NLM Configuration System
// Enhanced configuration management with better error handling and validation

#ifndef NLM_CONFIG_HPP
#define NLM_CONFIG_HPP

#include "core/ErrorHandling.hpp"
#include <string>
#include <map>
#include <memory>
#include <vector>
#include <variant>
#include <optional>
#include <chrono>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

namespace nlm {

class Config;

// Configuration source tracking
enum class ConfigSource {
    Default,
    File,
    CommandLine,
    Runtime,
    Derived
};

// Configuration value types
using ConfigValue = std::variant<
    int,
    int64_t,
    double,
    float,
    bool,
    std::string,
    std::vector<int>,
    std::vector<double>,
    std::vector<std::string>,
    std::vector<float>
>;

// Configuration entry with metadata
struct ConfigEntry {
    std::string key;
    ConfigValue value;
    ConfigSource source;
    std::string description;
    std::chrono::system_clock::time_point timestamp;
    
    ConfigEntry() : source(ConfigSource::Default), timestamp(std::chrono::system_clock::now()) {}
    
    ConfigEntry(const std::string& k, const ConfigValue& v, 
                ConfigSource s, const std::string& desc = "")
        : key(k), value(v), source(s), description(desc), 
          timestamp(std::chrono::system_clock::now()) {}
    
    // Type-safe getters
    template<typename T>
    std::optional<T> getValue() const {
        if (std::holds_alternative<T>(value)) {
            return std::get<T>(value);
        }
        return std::nullopt;
    }
    
    bool isDerivedFrom(const ConfigEntry& other) const {
        return source == ConfigSource::Derived && description.find(other.key) != std::string::npos;
    }
};

// Enhanced configuration system with validation
class Config {
private:
    std::map<std::string, ConfigEntry> entries_;
    std::map<std::string, std::function<bool(const ConfigValue&)>> validators_;
    
public:
    Config() {
        // Register built-in validators
        registerValidator<int>("int_range", [](const ConfigValue& v) {
            return std::get<int>(v) >= -1000000 && std::get<int>(v) <= 1000000;
        });
        registerValidator<double>("double_range", [](const ConfigValue& v) {
            double d = std::get<double>(v);
            return d >= -1000000.0 && d <= 1000000.0;
        });
        registerValidator<float>("float_range", [](const ConfigValue& v) {
            float f = std::get<float>(v);
            return f >= -1000000.0f && f <= 1000000.0f;
        });
        registerValidator<double>("positive", [](const ConfigValue& v) {
            return std::get<double>(v) >= 0.0;
        });
        registerValidator<float>("probability", [](const ConfigValue& v) {
            float f = std::get<float>(v);
            return f >= 0.0f && f <= 1.0f;
        });
    }
    
    // Add a configuration value with validation
    template<typename T>
    void set(const std::string& key, const T& value, ConfigSource source = ConfigSource::Runtime,
             const std::string& description = "", bool validate = true) {
        validateKey(key);
        
        ConfigEntry entry(key, ConfigValue(value), source, description);
        
        if (validate) {
            validateValue(key, entry.value);
        }
        
        entries_[key] = std::move(entry);
    }
    
    // Set string value (specialized to handle std::string)
    void set(const std::string& key, const std::string& value, ConfigSource source = ConfigSource::Runtime,
             const std::string& description = "", bool validate = true) {
        validateKey(key);
        
        ConfigEntry entry(key, ConfigValue(value), source, description);
        
        if (validate) {
            validateValue(key, entry.value);
        }
        
        entries_[key] = std::move(entry);
    }
    
    // Get configuration value with type safety
    template<typename T>
    std::optional<T> get(const std::string& key) const {
        validateKey(key);
        
        auto it = entries_.find(key);
        if (it == entries_.end()) {
            return std::nullopt;
        }
        
        return it->second.getValue<T>();
    }
    
    // Get with default value
    template<typename T>
    T getOr(const std::string& key, const T& defaultValue) const {
        auto value = get<T>(key);
        if (value.has_value()) {
            return *value;
        }
        return defaultValue;
    }
    
    // Check if key exists
    bool has(const std::string& key) const {
        validateKey(key);
        return entries_.find(key) != entries_.end();
    }
    
    // Remove configuration entry
    void remove(const std::string& key) {
        validateKey(key);
        entries_.erase(key);
    }
    
    // Load configuration from JSON file
    bool loadFromFile(const std::string& filepath) {
        try {
            std::ifstream file(filepath);
            if (!file.is_open()) {
                return false;
            }
            
            json configJson;
            file >> configJson;
            
            // Clear existing entries (except defaults)
            for (auto& [key, entry] : entries_) {
                if (entry.source == ConfigSource::Default) {
                    continue;  // Keep default values
                }
            }
            
            // Load from JSON
            for (auto& [key, value] : configJson.items()) {
                if (value.is_number_integer()) {
                    set(key, value.get<int>(), ConfigSource::File);
                } else if (value.is_number_unsigned()) {
                    set(key, static_cast<int64_t>(value.get<uint64_t>()), ConfigSource::File);
                } else if (value.is_number_float()) {
                    set(key, value.get<double>(), ConfigSource::File);
                } else if (value.is_boolean()) {
                    set(key, value.get<bool>(), ConfigSource::File);
                } else if (value.is_string()) {
                    set(key, value.get<std::string>(), ConfigSource::File);
                } else if (value.is_array()) {
                    // Handle array types
                    std::vector<double> doubleArray;
                    for (auto& item : value) {
                        doubleArray.push_back(item.get<double>());
                    }
                    set(key, doubleArray, ConfigSource::File);
                }
            }
            
            return true;
        } catch (const std::exception& e) {
            return false;
        }
    }
    
    // Load configuration from command line arguments
    bool loadFromArgs(int argc, char** argv) {
        bool foundConfig = false;
        
        for (int i = 1; i < argc; ++i) {
            std::string arg = argv[i];
            
            if (arg == "--config" && i + 1 < argc) {
                std::string configFile = argv[++i];
                if (loadFromFile(configFile)) {
                    foundConfig = true;
                }
            } else if (arg.find("--") == 0) {
                // Parse command line argument: --key=value or --key value
                size_t equalsPos = arg.find('=');
                std::string key, valueStr;
                
                if (equalsPos != std::string::npos) {
                    key = arg.substr(2, equalsPos - 2);
                    valueStr = arg.substr(equalsPos + 1);
                } else {
                    key = arg.substr(2);
                    if (i + 1 < argc && argv[i + 1][0] != '-') {
                        valueStr = argv[++i];
                    }
                }
                
                if (!key.empty() && !valueStr.empty()) {
                    parseAndSet(key, valueStr, ConfigSource::CommandLine);
                }
            }
        }
        
        return foundConfig;
    }
    
    // Get all configuration keys
    std::vector<std::string> getKeys() const {
        std::vector<std::string> keys;
        keys.reserve(entries_.size());
        
        for (const auto& [key, entry] : entries_) {
            keys.push_back(key);
        }
        
        return keys;
    }
    
    // Get configuration summary
    std::string summary() const {
        std::stringstream ss;
        ss << "Configuration Summary:" << std::endl;
        
        for (const auto& [key, entry] : entries_) {
            ss << "  " << key << ": ";
            
            std::visit([&ss](const auto& value) {
                if constexpr (std::is_same_v<decltype(value), int>) {
                    ss << value;
                } else if constexpr (std::is_same_v<decltype(value), double>) {
                    ss << value;
                } else if constexpr (std::is_same_v<decltype(value), float>) {
                    ss << value;
                } else if constexpr (std::is_same_v<decltype(value), bool>) {
                    ss << (value ? "true" : "false");
                } else if constexpr (std::is_same_v<decltype(value), std::string>) {
                    ss << "\"" << value << "\"";
                } else if constexpr (std::is_same_v<decltype(value), std::vector<int>>) {
                    ss << "[";
                    for (size_t i = 0; i < value.size(); ++i) {
                        if (i > 0) ss << ", ";
                        ss << value[i];
                    }
                    ss << "]";
                } else if constexpr (std::is_same_v<decltype(value), std::vector<double>>) {
                    ss << "[";
                    for (size_t i = 0; i < value.size(); ++i) {
                        if (i > 0) ss << ", ";
                        ss << value[i];
                    }
                    ss << "]";
                } else if constexpr (std::is_same_v<decltype(value), std::vector<std::string>>) {
                    ss << "[";
                    for (size_t i = 0; i < value.size(); ++i) {
                        if (i > 0) ss << ", ";
                        ss << "\"" << value[i] << "\"";
                    }
                    ss << "]";
                } else if constexpr (std::is_same_v<decltype(value), std::vector<float>>) {
                    ss << "[";
                    for (size_t i = 0; i < value.size(); ++i) {
                        if (i > 0) ss << ", ";
                        ss << value[i];
                    }
                    ss << "]";
                }
            }, entry.value);
            
            ss << " (source: " << static_cast<int>(entry.source) << ")" << std::endl;
        }
        
        return ss.str();
    }
    
    // Clear all non-default configuration
    void clear() {
        std::map<std::string, ConfigEntry> defaults;
        
        for (const auto& [key, entry] : entries_) {
            if (entry.source == ConfigSource::Default) {
                defaults[key] = entry;
            }
        }
        
        entries_ = std::move(defaults);
    }
    
    // Register a custom validator
    template<typename T>
    void registerValidator(const std::string& name, std::function<bool(const ConfigValue&)> validator) {
        std::string fullName = typeid(T).name() + "_" + name;
        validators_[fullName] = std::move(validator);
    }
    
    // Get configuration entry (internal use only)
    const ConfigEntry* getEntry(const std::string& key) const {
        validateKey(key);
        auto it = entries_.find(key);
        if (it == entries_.end()) {
            return nullptr;
        }
        return &it->second;
    }
    
private:
    // Validate key format
    void validateKey(const std::string& key) const {
        if (key.empty()) {
            throw ConfigurationError("Configuration key cannot be empty");
        }
        if (key.find('.') != std::string::npos) {
            throw ConfigurationError("Configuration key cannot contain dots");
        }
    }
    
    // Validate configuration value using registered validators
    void validateValue(const std::string& key, const ConfigValue& value) {
        std::string typeName;
        
        if (std::holds_alternative<int>(value)) {
            typeName = typeid(int).name() + "_range";
        } else if (std::holds_alternative<int64_t>(value)) {
            typeName = typeid(int64_t).name() + "_range";
        } else if (std::holds_alternative<double>(value)) {
            typeName = typeid(double).name() + "_range";
        } else if (std::holds_alternative<float>(value)) {
            typeName = typeid(float).name() + "_range";
        } else if (std::holds_alternative<bool>(value)) {
            typeName = typeid(bool).name() + "_range";
        } else if (std::holds_alternative<std::string>(value)) {
            typeName = typeid(std::string).name() + "_range";
        }
        
        auto it = validators_.find(typeName);
        if (it != validators_.end() && !it->second(value)) {
            throw ConfigurationError("Validation failed for key " + key);
        }
    }
    
    // Parse string value and set configuration
    void parseAndSet(const std::string& key, const std::string& valueStr, ConfigSource source) {
        // Try to parse as int
        try {
            size_t pos;
            int intValue = std::stoi(valueStr, &pos);
            if (pos == valueStr.length()) {
                set(key, intValue, source);
                return;
            }
        } catch (...) {}
        
        // Try to parse as double
        try {
            size_t pos;
            double doubleValue = std::stod(valueStr, &pos);
            if (pos == valueStr.length()) {
                set(key, doubleValue, source);
                return;
            }
        } catch (...) {}
        
        // Try to parse as bool
        if (valueStr == "true" || valueStr == "false") {
            bool boolValue = (valueStr == "true");
            set(key, boolValue, source);
            return;
        }
        
        // Otherwise treat as string
        set(key, valueStr, source);
    }
};

} // namespace nlm

#endif // NLM_CONFIG_HPP
