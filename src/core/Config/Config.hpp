#ifndef NLM_CONFIG_HPP
#define NLM_CONFIG_HPP

#include <string>
#include <memory>
#include <vector>
#include <variant>
#include <optional>
#include <fstream>
#include <sstream>
#include <iostream>
#include <algorithm>
#include <cctype>

#ifdef JSON_CONFIG_AVAILABLE
#include "nlohmann/json.hpp"
#endif

#ifdef YAML_CONFIG_AVAILABLE
#include "yaml-cpp/yaml.h"
#endif

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
    std::vector<std::string>
>;

// Configuration source
enum class ConfigSource {
    Default,
    File,
    CommandLine,
    Runtime
};

// Configuration validation errors
enum class ConfigValidationError {
    InvalidKey,
    InvalidType,
    InvalidValue,
    MissingRequiredField,
    DuplicateKey,
    FileParseError,
    FormatError
};

// Configuration validation result
struct ConfigValidationResult {
    bool success;
    std::vector<std::string> errors;
    std::vector<std::string> warnings;
    
    ConfigValidationResult() : success(true) {}
    
    void addError(const std::string& error) {
        success = false;
        errors.push_back(error);
    }
    
    void addWarning(const std::string& warning) {
        warnings.push_back(warning);
    }
};

// Configuration schema for validation
struct ConfigSchema {
    std::string key;
    ConfigSource source;
    std::string description;
    bool required;
    std::string type;  // "int", "double", "bool", "string", "vector"
    std::vector<std::string> allowedValues;
    
    ConfigSchema() : source(ConfigSource::Default), required(false) {}
    
    ConfigSchema(const std::string& k, const std::string& t, bool req = false, 
                 const std::string& desc = "")
        : key(k), source(ConfigSource::Default), description(desc), required(req), type(t) {}
};

// Configuration entry
struct ConfigEntry {
    std::string key;
    ConfigValue value;
    ConfigSource source;
    std::string description;
    std::string type;
    
    ConfigEntry() : key(), value(), source(ConfigSource::Default), description(), type() {}
    
    ConfigEntry(const std::string& k, const ConfigValue& v, ConfigSource s, 
                const std::string& desc = "", const std::string& t = "")
        : key(k), value(v), source(s), description(desc), type(t) {}
};

// JSON config parser
#ifdef JSON_CONFIG_AVAILABLE
class JSONConfigParser {
public:
    static ConfigValidationResult parse(const std::string& jsonStr, std::vector<ConfigEntry>& entries) {
        ConfigValidationResult result;
        try {
            auto j = nlohmann::json::parse(jsonStr);
            
            for (auto it = j.begin(); it != j.end(); ++it) {
                std::string key = it.key();
                
                // Determine type based on JSON value
                std::string type;
                ConfigValue value;
                
                if (it->is_number_integer()) {
                    type = "int";
                    value = it->get<int>();
                } else if (it->is_number_unsigned()) {
                    type = "uint64_t";
                    value = it->get<uint64_t>();
                } else if (it->is_number_float()) {
                    type = "double";
                    value = it->get<double>();
                } else if (it->is_boolean()) {
                    type = "bool";
                    value = it->get<bool>();
                } else if (it->is_string()) {
                    type = "string";
                    value = it->get<std::string>();
                } else if (it->is_array()) {
                    // Check array element type
                    auto arr = it->get<std::vector<nlohmann::json>>();
                    if (!arr.empty()) {
                        if (arr[0].is_number_integer()) {
                            type = "vector<int>";
                            value = arr.get<std::vector<int>>();
                        } else if (arr[0].is_number_float()) {
                            type = "vector<double>";
                            value = arr.get<std::vector<double>>();
                        } else if (arr[0].is_string()) {
                            type = "vector<string>";
                            value = arr.get<std::vector<std::string>>();
                        }
                    }
                }
                
                if (!type.empty()) {
                    entries.emplace_back(key, value, ConfigSource::File, "", type);
                } else {
                    result.addError("Unsupported JSON type for key: " + key);
                }
            }
            
        } catch (const nlohmann::json::parse_error& e) {
            result.addError("JSON parse error: " + std::string(e.what()));
        } catch (const std::exception& e) {
            result.addError("Error parsing JSON: " + std::string(e.what()));
        }
        
        return result;
    }
};
#endif

// YAML config parser
#ifdef YAML_CONFIG_AVAILABLE
class YAMLConfigParser {
public:
    static ConfigValidationResult parse(const std::string& yamlStr, std::vector<ConfigEntry>& entries) {
        ConfigValidationResult result;
        try {
            auto root = YAML::Load(yamlStr);
            
            for (auto it = root.begin(); it != root.end(); ++it) {
                std::string key = it->first.as<std::string>();
                
                // Determine type based on YAML value
                std::string type;
                ConfigValue value;
                
                if (it->second.IsScalar()) {
                    std::string strVal = it->second.as<std::string>();
                    
                    // Try to determine type from string
                    if (strVal == "true" || strVal == "false") {
                        type = "bool";
                        value = (strVal == "true");
                    } else {
                        // Try numeric parsing
                        try {
                            if (strVal.find('.') != std::string::npos) {
                                type = "double";
                                value = std::stod(strVal);
                            } else {
                                type = "int";
                                value = std::stoi(strVal);
                            }
                        } catch (...) {
                            type = "string";
                            value = strVal;
                        }
                    }
                } else if (it->second.IsSequence()) {
                    auto seq = it->second.as<std::vector<YAML::Node>>();
                    if (!seq.empty()) {
                        if (seq[0].IsScalar()) {
                            std::string firstVal = seq[0].as<std::string>();
                            if (firstVal.find('.') != std::string::npos) {
                                type = "vector<double>";
                                std::vector<double> doubleVec;
                                for (const auto& node : seq) {
                                    doubleVec.push_back(node.as<double>());
                                }
                                value = doubleVec;
                            } else {
                                type = "vector<int>";
                                std::vector<int> intVec;
                                for (const auto& node : seq) {
                                    intVec.push_back(node.as<int>());
                                }
                                value = intVec;
                            }
                        }
                    }
                }
                
                if (!type.empty()) {
                    entries.emplace_back(key, value, ConfigSource::File, "", type);
                } else {
                    result.addError("Unsupported YAML type for key: " + key);
                }
            }
            
        } catch (const YAML::Exception& e) {
            result.addError("YAML parse error: " + std::string(e.what()));
        } catch (const std::exception& e) {
            result.addError("Error parsing YAML: " + std::string(e.what()));
        }
        
        return result;
    }
};
#endif

// Config file format
enum class ConfigFormat {
    Unknown,
    JSON,
    YAML,
    Ini
};

class ConfigFormatDetector {
public:
    static ConfigFormat detectFormat(const std::string& content) {
        if (content.empty()) return ConfigFormat::Unknown;
        
        // Check for JSON (starts with { or [)
        if ((content.find('{') == 0 && content.find('}') != std::string::npos) ||
            (content.find('[') == 0 && content.find(']') != std::string::npos)) {
            return ConfigFormat::JSON;
        }
        
        // Check for YAML (common YAML patterns)
        if (content.find("key:") != std::string::npos || 
            content.find("  - ") != std::string::npos ||
            content.find(": ") != std::string::npos) {
            return ConfigFormat::YAML;
        }
        
        // Check for INI (key = value format)
        if (content.find(" = ") != std::string::npos ||
            content.find("\n[Section]") != std::string::npos) {
            return ConfigFormat::Ini;
        }
        
        return ConfigFormat::Unknown;
    }
};

// Main configuration class
class Config {
public:
    Config();
    ~Config();
    
    // Disable copying, enable moving
    Config(const Config&) = delete;
    Config& operator=(const Config&) = delete;
    Config(Config&&) noexcept;
    Config& operator=(Config&&) noexcept;
    
    // Load from file (supports JSON, YAML, INI)
    bool loadFromFile(const std::string& filepath);
    
    // Load from string (auto-detect format)
    bool loadFromString(const std::string& content);
    
    // Load from command line arguments
    bool loadFromArgs(int argc, char** argv);
    
    // Save to file (default format: JSON)
    bool saveToFile(const std::string& filepath, ConfigFormat format = ConfigFormat::JSON) const;
    
    // Save to string
    std::string saveToString(ConfigFormat format = ConfigFormat::JSON) const;
    
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
    
    // Validation
    ConfigValidationResult validate(const ConfigSchema& schema = ConfigSchema()) const;
    
    // Check existence
    bool has(const std::string& key) const;
    
    // Remove key
    void remove(const std::string& key);
    
    // Get all keys
    std::vector<std::string> getKeys() const;
    
    // Get all entries
    std::vector<ConfigEntry> getEntries() const;
    
    // Clear all
    void clear();
    
    // Get configuration summary
    std::string summary() const;
    
    // Merge with another config
    void merge(const Config& other, ConfigSource source = ConfigSource::File);
    
    // Get statistics
    size_t getEntryCount() const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Internal helpers
    static std::string trim(const std::string& str);
    static std::string toLower(const std::string& str);
    static std::string getFileExtension(const std::string& filename);
    
    // Validation helpers
    static bool validateValueType(const ConfigValue& value, const std::string& type) noexcept;
    static bool validateValueRange(const ConfigValue& value, const std::string& type) const noexcept;
    
    // File I/O helpers
    static bool readFile(const std::string& filepath, std::string& content);
    static bool writeFile(const std::string& filepath, const std::string& content) noexcept;
    
    // Format conversion helpers
    static std::string toJSON(const std::vector<ConfigEntry>& entries) noexcept;
    static std::string toYAML(const std::vector<ConfigEntry>& entries) noexcept;
    static std::string toINI(const std::vector<ConfigEntry>& entries) noexcept;
    
    static std::vector<ConfigEntry> fromJSON(const std::string& jsonStr, ConfigValidationResult& result);
    static std::vector<ConfigEntry> fromYAML(const std::string& yamlStr, ConfigValidationResult& result);
    static std::vector<ConfigEntry> fromINI(const std::string& iniStr, ConfigValidationResult& result);
};

} // namespace nlm

#endif // NLM_CONFIG_HPP
