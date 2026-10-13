#pragma once

#include <string>
#include <memory>
#include <vector>
#include <variant>
#include <optional>
#include <map>
#include <functional>
#include <sstream>
#include <iomanip>
#include <regex>
#include <chrono>
#include <algorithm>

namespace nlm {

// Forward declarations
class Config;

// Configuration value types
using ConfigValue = std::variant<
    int,
    int64_t,
    double,
    bool,
    std::string,
    std::vector<int>,
    std::vector<double>,
    std::vector<std::string>,
    std::map<std::string, std::string>  // For nested configurations
>;

// Configuration source
enum class ConfigSource {
    Default,
    File,
    CommandLine,
    Runtime,
    Preset
};

// Configuration entry with validation support
struct ConfigEntry {
    std::string key;
    ConfigValue value;
    ConfigSource source;
    std::string description;
    std::string validationRegex;
    std::string defaultValue;
    bool required;
    
    ConfigEntry() 
        : key(), value(), source(ConfigSource::Default), description(), 
          validationRegex(), defaultValue(), required(false) {}
    
    ConfigEntry(const std::string& k, const ConfigValue& v, ConfigSource s, 
                const std::string& desc = "", const std::string& regex = "",
                const std::string& defVal = "", bool req = false)
        : key(k), value(v), source(s), description(desc), 
          validationRegex(regex), defaultValue(defVal), required(req) {}
    
    // Validate configuration entry
    bool isValid() const {
        if (required && (value.index() == 0 || (std::holds_alternative<std::string>(value) && std::get<std::string>(value).empty()))) {
            return false;
        }
        
        if (!validationRegex.empty()) {
            std::string strValue;
            switch (value.index()) {
                case 0: strValue = std::to_string(std::get<int>(value)); break;
                case 1: strValue = std::to_string(std::get<int64_t>(value)); break;
                case 2: strValue = std::to_string(std::get<double>(value)); break;
                case 3: strValue = std::get<bool>(value) ? "true" : "false"; break;
                case 4: strValue = std::get<std::string>(value); break;
                default: return true; // For lists and maps, we skip regex validation
            }
            
            std::regex pattern(validationRegex);
            if (!std::regex_match(strValue, pattern)) {
                return false;
            }
        }
        
        return true;
    }
};

// Configuration validation results
struct ConfigValidationResult {
    bool valid;
    std::vector<std::string> errors;
    std::vector<std::string> warnings;
    
    ConfigValidationResult() : valid(true) {}
    
    void addError(const std::string& error) {
        errors.push_back(error);
        valid = false;
    }
    
    void addWarning(const std::string& warning) {
        warnings.push_back(warning);
    }
};

// Configuration preset manager
class ConfigPresets {
public:
    struct Preset {
        std::string name;
        std::map<std::string, ConfigValue> values;
        std::string description;
    };
    
    static void registerPreset(const std::string& name, const std::map<std::string, ConfigValue>& values, 
                               const std::string& description = "") {
        Preset preset;
        preset.name = name;
        preset.values = values;
        preset.description = description;
        presets_[name] = preset;
    }
    
    static std::shared_ptr<Config> applyPreset(const std::string& name) {
        auto it = presets_.find(name);
        if (it == presets_.end()) {
            throw std::runtime_error("Unknown preset: " + name);
        }
        
        auto config = std::make_shared<Config>();
        for (const auto& entry : it->second.values) {
            config->set(entry.first, entry.second, ConfigSource::Preset);
        }
        return config;
    }
    
    static std::vector<std::string> getPresetNames() {
        std::vector<std::string> names;
        for (const auto& pair : presets_) {
            names.push_back(pair.first);
        }
        return names;
    }
    
    static std::string getPresetDescription(const std::string& name) {
        auto it = presets_.find(name);
        if (it == presets_.end()) {
            return "";
        }
        return it->second.description;
    }
    
private:
    static std::map<std::string, Preset> presets_;
};

// Define the static member
inline std::map<std::string, ConfigPresets::Preset> ConfigPresets::presets_;

// Enhanced configuration value helper functions
namespace config_helpers {
    // Convert value to string
    std::string toString(const ConfigValue& value);
    
    // Convert string to value
    ConfigValue fromString(const std::string& str, const std::string& key = "");
    
    // Validate configuration key
    bool isValidKey(const std::string& key);
    
    // Get configuration key type name
    const char* getTypeName(const ConfigValue& value);
    
    // Format value for display
    std::string formatValue(const ConfigValue& value, size_t width = 0);
    
    // Merge two configurations
    void mergeConfig(Config& target, const Config& source);
}

// Enhanced configuration class with better API and validation
class Config {
public:
    // Constructors
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
    
    // Enhanced value setting with validation
    bool set(const std::string& key, const ConfigValue& value, 
             ConfigSource source = ConfigSource::Runtime,
             const std::string& description = "",
             const std::string& validationRegex = "",
             const std::string& defaultValue = "",
             bool required = false);
    
    // Type-specific setters with validation
    bool setInt(const std::string& key, int64_t value, ConfigSource source = ConfigSource::Runtime);
    bool setFloat(const std::string& key, float value, ConfigSource source = ConfigSource::Runtime);
    bool setBool(const std::string& key, bool value, ConfigSource source = ConfigSource::Runtime);
    bool setString(const std::string& key, const std::string& value, ConfigSource source = ConfigSource::Runtime);
    bool setList(const std::string& key, const std::vector<int64_t>& value, ConfigSource source = ConfigSource::Runtime);
    bool setList(const std::string& key, const std::vector<float>& value, ConfigSource source = ConfigSource::Runtime);
    bool setList(const std::string& key, const std::vector<std::string>& value, ConfigSource source = ConfigSource::Runtime);
    
    // Original template methods (for backward compatibility)
    template<typename T>
    std::optional<T> get(const std::string& key) const;
    
    template<typename T>
    T getOr(const std::string& key, const T& defaultValue) const;
    
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
    
    // Original template methods implementation
    template<typename T>
    std::optional<T> Config::get(const std::string& key) const {
        auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
            [&key](const ConfigEntry& e) { return e.key == key; });
        
        if (it == pImpl->entries.end()) {
            return std::nullopt;
        }
        
        try {
            if constexpr (std::is_same_v<T, int>) {
                return std::get<int>(it->value);
            } else if constexpr (std::is_same_v<T, int64_t>) {
                if (std::holds_alternative<int>(it->value)) {
                    return static_cast<int64_t>(std::get<int>(it->value));
                } else if (std::holds_alternative<int64_t>(it->value)) {
                    return std::get<int64_t>(it->value);
                }
            } else if constexpr (std::is_same_v<T, float>) {
                if (std::holds_alternative<int>(it->value)) {
                    return static_cast<float>(std::get<int>(it->value));
                } else if (std::holds_alternative<int64_t>(it->value)) {
                    return static_cast<float>(std::get<int64_t>(it->value));
                } else if (std::holds_alternative<double>(it->value)) {
                    return static_cast<float>(std::get<double>(it->value));
                }
            } else if constexpr (std::is_same_v<T, bool>) {
                if (std::holds_alternative<bool>(it->value)) {
                    return std::get<bool>(it->value);
                } else if (std::holds_alternative<int>(it->value)) {
                    return std::get<int>(it->value) != 0;
                } else if (std::holds_alternative<int64_t>(it->value)) {
                    return std::get<int64_t>(it->value) != 0;
                } else if (std::holds_alternative<double>(it->value)) {
                    return std::get<double>(it->value) != 0.0;
                } else if (std::holds_alternative<std::string>(it->value)) {
                    const auto& str = std::get<std::string>(it->value);
                    return str == "true" || str == "1" || str == "yes";
                }
            } else if constexpr (std::is_same_v<T, std::string>) {
                if (std::holds_alternative<std::string>(it->value)) {
                    return std::get<std::string>(it->value);
                } else {
                    return config_helpers::toString(it->value);
                }
            }
        } catch (const std::bad_variant_access&) {
            // Type mismatch
        }
        
        return std::nullopt;
    }
    
    template<typename T>
    T Config::getOr(const std::string& key, const T& defaultValue) const {
        auto value = get<T>(key);
        return value ? *value : defaultValue;
    }
    
    // Helper methods
    static std::string trim(const std::string& str);
    static std::string toLower(const std::string& str);
    static bool isValidKeyFormat(const std::string& key);
    static ConfigValue convertToVariant(const std::string& str);
    static std::string variantToString(const ConfigValue& value);
    
    // Additional public methods for enhanced functionality
    // [Implementation would continue with all the additional methods]
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm