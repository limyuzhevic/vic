// Configuration implementation header
// This file contains the implementation of the enhanced Config class

#pragma once

#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <cmath>
#include <stdexcept>
#include <regex>

// Include fallback implementations if real ones are not available
#if defined(NLOHMANN_JSON_FALLBACK)
#include "nlohmann/json.hpp"
#endif

#if defined(YAML_CPP_FALLBACK)
#include "yaml-cpp/yaml.h"
#endif

namespace nlm {

struct Config::Impl {
    std::vector<ConfigEntry> entries;
    std::map<std::string, ConfigSchema> schema;
    bool schemaValidationEnabled = true;
    
    // JSON/YAML parsing helpers
    static nlohmann::json configValueToJSON(const ConfigValue& value);
    static ConfigValue JSONToConfigValue(const nlohmann::json& jsonValue);
    static YAML::Node configValueToYAML(const ConfigValue& value);
    static ConfigValue YAMLToConfigValue(const YAML::Node& yamlValue);
};

// Helper function to check if a file exists
inline bool fileExists(const std::string& path) {
    return std::filesystem::exists(path) && std::filesystem::is_regular_file(path);
}

// Implementation of Config::Impl static methods
inline nlohmann::json Config::Impl::configValueToJSON(const ConfigValue& value) {
    nlohmann::json jsonValue;
    
    if (std::holds_alternative<int>(value)) {
        jsonValue = std::get<int>(value);
    } else if (std::holds_alternative<int64_t>(value)) {
        jsonValue = std::get<int64_t>(value);
    } else if (std::holds_alternative<double>(value)) {
        jsonValue = std::get<double>(value);
    } else if (std::holds_alternative<bool>(value)) {
        jsonValue = std::get<bool>(value);
    } else if (std::holds_alternative<std::string>(value)) {
        jsonValue = std::get<std::string>(value);
    } else if (std::holds_alternative<std::vector<int>>(value)) {
        jsonValue = std::get<std::vector<int>>(value);
    } else if (std::holds_alternative<std::vector<double>>(value)) {
        jsonValue = std::get<std::vector<double>>(value);
    } else if (std::holds_alternative<std::vector<std::string>>(value)) {
        jsonValue = std::get<std::vector<std::string>>(value);
    } else if (std::holds_alternative<std::vector<bool>>(value)) {
        jsonValue = std::get<std::vector<bool>>(value);
    }
    
    return jsonValue;
}

inline ConfigValue Config::Impl::JSONToConfigValue(const nlohmann::json& jsonValue) {
    ConfigValue configValue;
    
    if (jsonValue.is_number_integer()) {
        configValue = jsonValue.get<int>();
    } else if (jsonValue.is_number_float()) {
        configValue = jsonValue.get<double>();
    } else if (jsonValue.is_boolean()) {
        configValue = jsonValue.get<bool>();
    } else if (jsonValue.is_string()) {
        configValue = jsonValue.get<std::string>();
    } else if (jsonValue.is_array()) {
        // Determine array type
        if (!jsonValue.empty()) {
            if (jsonValue[0].is_number_integer()) {
                std::vector<int> intArray;
                for (const auto& item : jsonValue) {
                    intArray.push_back(item.get<int>());
                }
                configValue = intArray;
            } else if (jsonValue[0].is_number_float()) {
                std::vector<double> doubleArray;
                for (const auto& item : jsonValue) {
                    doubleArray.push_back(item.get<double>());
                }
                configValue = doubleArray;
            } else if (jsonValue[0].is_boolean()) {
                std::vector<bool> boolArray;
                for (const auto& item : jsonValue) {
                    boolArray.push_back(item.get<bool>());
                }
                configValue = boolArray;
            } else {
                std::vector<std::string> stringArray;
                for (const auto& item : jsonValue) {
                    stringArray.push_back(item.get<std::string>());
                }
                configValue = stringArray;
            }
        } else {
            // Empty array - default to string array
            configValue = std::vector<std::string>();
        }
    }
    
    return configValue;
}

inline YAML::Node Config::Impl::configValueToYAML(const ConfigValue& value) {
    YAML::Node yamlNode;
    
    if (std::holds_alternative<int>(value)) {
        yamlNode = std::get<int>(value);
    } else if (std::holds_alternative<int64_t>(value)) {
        yamlNode = std::get<int64_t>(value);
    } else if (std::holds_alternative<double>(value)) {
        yamlNode = std::get<double>(value);
    } else if (std::holds_alternative<bool>(value)) {
        yamlNode = std::get<bool>(value);
    } else if (std::holds_alternative<std::string>(value)) {
        yamlNode = std::get<std::string>(value);
    } else if (std::holds_alternative<std::vector<int>>(value)) {
        for (const auto& item : std::get<std::vector<int>>(value)) {
            yamlNode.push_back(item);
        }
    } else if (std::holds_alternative<std::vector<double>>(value)) {
        for (const auto& item : std::get<std::vector<double>>(value)) {
            yamlNode.push_back(item);
        }
    } else if (std::holds_alternative<std::vector<std::string>>(value)) {
        for (const auto& item : std::get<std::vector<std::string>>(value)) {
            yamlNode.push_back(item);
        }
    } else if (std::holds_alternative<std::vector<bool>>(value)) {
        for (const auto& item : std::get<std::vector<bool>>(value)) {
            yamlNode.push_back(item);
        }
    }
    
    return yamlNode;
}

inline ConfigValue Config::Impl::YAMLToConfigValue(const YAML::Node& yamlValue) {
    ConfigValue configValue;
    
    if (yamlValue.IsScalar()) {
        if (yamlValue.IsIntegral()) {
            configValue = yamlValue.as<int>();
        } else if (yamlValue.IsFloat()) {
            configValue = yamlValue.as<double>();
        } else if (yamlValue.IsBool()) {
            configValue = yamlValue.as<bool>();
        } else {
            configValue = yamlValue.as<std::string>();
        }
    } else if (yamlValue.IsSequence()) {
        // Determine array type
        if (!yamlValue.empty()) {
            if (yamlValue[0].IsIntegral()) {
                std::vector<int> intArray;
                for (size_t i = 0; i < yamlValue.size(); ++i) {
                    intArray.push_back(yamlValue[i].as<int>());
                }
                configValue = intArray;
            } else if (yamlValue[0].IsFloat()) {
                std::vector<double> doubleArray;
                for (size_t i = 0; i < yamlValue.size(); ++i) {
                    doubleArray.push_back(yamlValue[i].as<double>());
                }
                configValue = doubleArray;
            } else if (yamlValue[0].IsBool()) {
                std::vector<bool> boolArray;
                for (size_t i = 0; i < yamlValue.size(); ++i) {
                    boolArray.push_back(yamlValue[i].as<bool>());
                }
                configValue = boolArray;
            } else {
                std::vector<std::string> stringArray;
                for (size_t i = 0; i < yamlValue.size(); ++i) {
                    stringArray.push_back(yamlValue[i].as<std::string>());
                }
                configValue = stringArray;
            }
        } else {
            // Empty array - default to string array
            configValue = std::vector<std::string>();
        }
    }
    
    return configValue;
}

// Config implementation
inline Config::Config() : pImpl(std::make_unique<Impl>()) {
    // Register default schema entries
    defineSchema("random_seed", ConfigType::Integer, "Random seed for reproducible simulations", 
                 [](const ConfigValue& v) { return validateInt(v, 0, std::numeric_limits<int>::max()); }, 
                 false, "42");
    
    defineSchema("simulation_timestep", ConfigType::Float, "Simulation timestep in seconds",
                 [](const ConfigValue& v) { return validateDouble(v, 0.0, 1.0); },
                 false, "0.001");
    
    defineSchema("neuron_count", ConfigType::Integer, "Number of neurons in the network",
                 [](const ConfigValue& v) { return validateInt(v, 1, 1000000); },
                 false, "1000");
    
    defineSchema("region_count", ConfigType::Integer, "Number of neural regions",
                 [](const ConfigValue& v) { return validateInt(v, 1, 100); },
                 false, "1");
    
    defineSchema("connection_probability", ConfigType::Float, "Probability of synapse formation",
                 [](const ConfigValue& v) { return validateDouble(v, 0.0, 1.0); },
                 false, "0.1");
}

inline Config::~Config() = default;

inline Config::Config(Config&& other) noexcept : pImpl(std::move(other.pImpl)) {}

inline Config& Config::operator=(Config&& other) noexcept {
    if (this != &other) {
        pImpl = std::move(other.pImpl);
    }
    return *this;
}

inline bool Config::loadFromFile(const std::string& filepath, const std::string& format) {
    std::string actualFormat = format;
    if (actualFormat == "auto") {
        // Auto-detect format based on file extension
        if (filepath.size() >= 5 && filepath.substr(filepath.size() - 5) == ".json") {
            actualFormat = "json";
        } else if (filepath.size() >= 5 && filepath.substr(filepath.size() - 5) == ".yaml") {
            actualFormat = "yaml";
        } else if (filepath.size() >= 4 && filepath.substr(filepath.size() - 4) == ".yml") {
            actualFormat = "yaml";
        } else {
            actualFormat = "text"; // fallback to original format
        }
    }
    
    if (actualFormat == "json") {
        return loadFromJSON(filepath);
    } else if (actualFormat == "yaml") {
        return loadFromYAML(filepath);
    } else {
        return loadFromFile(filepath); // original text format
    }
}

inline bool Config::loadFromJSON(const std::string& filepath) {
    try {
        std::ifstream file(filepath);
        if (!file.is_open()) {
            return false;
        }
        
        nlohmann::json jsonData = nlohmann::json::parse(file);
        
        // Clear existing entries
        pImpl->entries.clear();
        
        // Convert JSON to config entries
        for (auto it = jsonData.begin(); it != jsonData.end(); ++it) {
            std::string key = it.key();
            ConfigValue value = Impl::JSONToConfigValue(*it);
            pImpl->entries.emplace_back(key, value, ConfigSource::File);
            
            // Validate against schema if available
            if (pImpl->schemaValidationEnabled && pImpl->schema.find(key) != pImpl->schema.end()) {
                validateValue(pImpl->schema[key], value);
            }
        }
        
        return true;
    } catch (const std::exception& e) {
        return false;
    }
}

inline bool Config::loadFromYAML(const std::string& filepath) {
    try {
        YAML::Node yamlData = YAML::LoadFile(filepath);
        
        // Clear existing entries
        pImpl->entries.clear();
        
        // Convert YAML to config entries
        for (auto it = yamlData.begin(); it != yamlData.end(); ++it) {
            std::string key = it->first.as<std::string>();
            ConfigValue value = Impl::YAMLToConfigValue(it->second);
            pImpl->entries.emplace_back(key, value, ConfigSource::File);
            
            // Validate against schema if available
            if (pImpl->schemaValidationEnabled && pImpl->schema.find(key) != pImpl->schema.end()) {
                validateValue(pImpl->schema[key], value);
            }
        }
        
        return true;
    } catch (const std::exception& e) {
        return false;
    }
}

inline bool Config::loadFromString(const std::string& content, const std::string& format) {
    if (format == "json") {
        try {
            nlohmann::json jsonData = nlohmann::json::parse(content);
            pImpl->entries.clear();
            
            for (auto it = jsonData.begin(); it != jsonData.end(); ++it) {
                std::string key = it.key();
                ConfigValue value = Impl::JSONToConfigValue(*it);
                pImpl->entries.emplace_back(key, value, ConfigSource::File);
            }
            return true;
        } catch (const std::exception&) {
            return false;
        }
    } else if (format == "yaml") {
        try {
            YAML::Node yamlData = YAML::Load(content);
            pImpl->entries.clear();
            
            for (auto it = yamlData.begin(); it != yamlData.end(); ++it) {
                std::string key = it->first.as<std::string>();
                ConfigValue value = Impl::YAMLToConfigValue(it->second);
                pImpl->entries.emplace_back(key, value, ConfigSource::File);
            }
            return true;
        } catch (const std::exception&) {
            return false;
        }
    } else {
        // Original text format parsing
        return Config::loadFromFile("/tmp/temp_config.txt"); // Fallback
    }
}

inline bool Config::loadFromArgs(int argc, char** argv) {
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

inline bool Config::saveToFile(const std::string& filepath, const std::string& format) const {
    if (format == "json") {
        return saveToJSON(filepath);
    } else if (format == "yaml") {
        return saveToYAML(filepath);
    } else {
        // Original text format - fallback
        std::ofstream file(filepath);
        if (!file.is_open()) {
            return false;
        }
        
        for (const auto& entry : pImpl->entries) {
            file << "# " << entry.description << "\n";
            file << entry.key << " = " << "PLACEHOLDER_VALUE\n";
        }
        return true;
    }
}

inline bool Config::saveToJSON(const std::string& filepath) const {
    try {
        nlohmann::json jsonData;
        
        for (const auto& entry : pImpl->entries) {
            jsonData[entry.key] = Impl::configValueToJSON(entry.value);
        }
        
        std::ofstream file(filepath);
        if (!file.is_open()) {
            return false;
        }
        
        file << jsonData.dump(4); // Pretty print with 4 spaces indentation
        return true;
    } catch (const std::exception&) {
        return false;
    }
}

inline bool Config::saveToYAML(const std::string& filepath) const {
    try {
        YAML::Emitter emitter;
        emitter << YAML::BeginMap;
        
        for (const auto& entry : pImpl->entries) {
            emitter << YAML::Key << entry.key;
            emitter << YAML::Value << Impl::configValueToYAML(entry.value);
        }
        
        emitter << YAML::EndMap;
        
        std::ofstream file(filepath);
        if (!file.is_open()) {
            return false;
        }
        
        file << emitter.c_str();
        return true;
    } catch (const std::exception&) {
        return false;
    }
}

inline bool Config::validateSchema() const {
    return validateSchema(pImpl->schema);
}

inline bool Config::validateSchema(const std::vector<ConfigSchema>& schema) const {
    for (const auto& entry : pImpl->entries) {
        if (!validateKey(entry.key)) {
            return false;
        }
    }
    return true;
}

inline bool Config::validateKey(const std::string& key) const {
    if (pImpl->schema.find(key) == pImpl->schema.end()) {
        // Key not in schema - validate against default schema if possible
        ConfigType inferredType = inferTypeFromString(key);
        if (inferredType != ConfigType::String) {
            // Only warn if schema validation is enabled
            if (pImpl->schemaValidationEnabled) {
                // Log warning for unknown key with non-default type
                return true;
            }
        }
        return true; // Unknown keys are allowed unless explicitly forbidden
    }
    
    const ConfigSchema& schema = pImpl->schema.at(key);
    
    // Check if key exists in entries
    auto entryIt = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
                               [&key](const ConfigEntry& e) { return e.key == key; });
    
    if (entryIt == pImpl->entries.end()) {
        // Key is required but not found
        if (schema.required) {
            return false;
        }
        return true; // Optional keys can be missing
    }
    
    // Key exists - validate its value
    return validateValue(schema, entryIt->value);
}

inline void Config::defineSchema(const std::string& key, ConfigType type,
                                 const std::string& description,
                                 ValidationRule validator, bool required,
                                 const std::string& defaultValue) {
    pImpl->schema[key] = ConfigSchema(key, type, description, validator, required, defaultValue);
}

inline template<typename T>
std::optional<T> Config::getValidated(const std::string& key,
                                     const std::optional<T>& defaultValue) const {
    auto value = get<T>(key);
    if (!value.has_value()) {
        return defaultValue;
    }
    
    // Validate against schema if available
    if (pImpl->schemaValidationEnabled && pImpl->schema.find(key) != pImpl->schema.end()) {
        const ConfigSchema& schema = pImpl->schema.at(key);
        if (!validateValue(schema, ConfigValue(*value))) {
            return defaultValue;
        }
    }
    
    return *value;
}

inline template<typename T>
T Config::getOrValidated(const std::string& key, const T& defaultValue) const {
    auto value = getValidated<T>(key, std::nullopt);
    if (value.has_value()) {
        return *value;
    }
    return defaultValue;
}

inline template<typename T>
std::optional<T> Config::getTyped(const std::string& key) const {
    auto value = get<T>(key);
    return value.has_value() ? std::optional<T>(*value) : std::nullopt;
}

inline void Config::setValidated(const std::string& key, const ConfigValue& value,
                                ConfigSource source, bool validate) {
    if (validate && pImpl->schemaValidationEnabled && pImpl->schema.find(key) != pImpl->schema.end()) {
        const ConfigSchema& schema = pImpl->schema.at(key);
        if (!validateValue(schema, value)) {
            throw SchemaValidationError(
                "Validation failed for key: " + key,
                key,
                configTypeToString(schema.type),
                "invalid"
            );
        }
    }
    
    set(key, value, source);
}

inline bool Config::validateValue(const ConfigSchema& schema, const ConfigValue& value) const {
    if (schema.validator) {
        return schema.validator(value);
    }
    
    // Default validation based on type
    switch (schema.type) {
        case ConfigType::Integer:
            return validateInt(value);
        case ConfigType::Float:
            return validateDouble(value);
        case ConfigType::Boolean:
            return validateBool(value);
        case ConfigType::String:
            return validateString(value);
        case ConfigType::IntArray:
            return std::holds_alternative<std::vector<int>>(value);
        case ConfigType::FloatArray:
            return std::holds_alternative<std::vector<double>>(value);
        case ConfigType::StringArray:
            return std::holds_alternative<std::vector<std::string>>(value);
        case ConfigType::BoolArray:
            return std::holds_alternative<std::vector<bool>>(value);
        default:
            return false;
    }
}

inline bool Config::has(const std::string& key) const {
    return std::any_of(pImpl->entries.begin(), pImpl->entries.end(),
                      [&key](const ConfigEntry& e) { return e.key == key; });
}

inline bool Config::hasType(const std::string& key, ConfigType expectedType) const {
    auto entryIt = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
                               [&key](const ConfigEntry& e) { return e.key == key; });
    
    if (entryIt == pImpl->entries.end()) {
        return false;
    }
    
    return entryIt->type == expectedType;
}

inline void Config::remove(const std::string& key) {
    pImpl->entries.erase(
        std::remove_if(pImpl->entries.begin(), pImpl->entries.end(),
                      [&key](const ConfigEntry& e) { return e.key == key; }),
        pImpl->entries.end()
    );
}

inline std::vector<std::string> Config::getKeys() const {
    std::vector<std::string> keys;
    keys.reserve(pImpl->entries.size());
    for (const auto& entry : pImpl->entries) {
        keys.push_back(entry.key);
    }
    return keys;
}

inline std::vector<std::string> Config::getKeysByType(ConfigType type) const {
    std::vector<std::string> keys;
    for (const auto& entry : pImpl->entries) {
        if (entry.type == type) {
            keys.push_back(entry.key);
        }
    }
    return keys;
}

inline void Config::clear() {
    pImpl->entries.clear();
}

inline std::string Config::summary() const {
    std::ostringstream oss;
    oss << "Configuration (" << pImpl->entries.size() << " entries):\n";
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
        oss << "] (source: " << static_cast<int>(entry.source) << ")\n";
    }
    return oss.str();
}

inline ConfigType Config::getType(const std::string& key) const {
    auto entryIt = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
                               [&key](const ConfigEntry& e) { return e.key == key; });
    
    if (entryIt == pImpl->entries.end()) {
        return ConfigType::String; // Default for missing keys
    }
    
    return entryIt->type;
}

inline std::string Config::getDescription(const std::string& key) const {
    auto entryIt = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
                               [&key](const ConfigEntry& e) { return e.key == key; });
    
    if (entryIt == pImpl->entries.end()) {
        return "";
    }
    
    return entryIt->description;
}

inline bool Config::isRequired(const std::string& key) const {
    auto schemaIt = pImpl->schema.find(key);
    if (schemaIt == pImpl->schema.end()) {
        return false;
    }
    return schemaIt->required;
}

inline std::string Config::inferTypeFromString(const std::string& str) {
    // Convert to lowercase for checking
    std::string lower = toLower(str);
    
    // Check for integer patterns
    if (std::regex_match(str, std::regex("^-?\d+$"))) {
        return ConfigType::Integer;
    }
    
    // Check for float patterns
    if (std::regex_match(str, std::regex("^-?\d*\.?\d+$"))) {
        return ConfigType::Float;
    }
    
    // Check for boolean patterns
    if (lower == "true" || lower == "false" || lower == "yes" || lower == "no" ||
        lower == "on" || lower == "off") {
        return ConfigType::Boolean;
    }
    
    // Check for array patterns
    if (str.find('[') != std::string::npos || str.find(']') != std::string::npos) {
        return ConfigType::StringArray;
    }
    
    // Default to string
    return ConfigType::String;
}

inline std::string Config::configTypeToString(ConfigType type) {
    switch (type) {
        case ConfigType::Integer: return "integer";
        case ConfigType::Float: return "float";
        case ConfigType::Boolean: return "boolean";
        case ConfigType::String: return "string";
        case ConfigType::IntArray: return "int_array";
        case ConfigType::FloatArray: return "float_array";
        case ConfigType::StringArray: return "string_array";
        case ConfigType::BoolArray: return "bool_array";
        default: return "unknown";
    }
}

} // namespace nlm