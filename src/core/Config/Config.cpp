// NLM Configuration System Enhancement
// This file implements the improved Config system with:
// - Proper JSON/YAML parsing support (nlohmann::json)
// - Configuration validation and schema support
// - Default values for common configuration keys
// - Robust error handling
// - Enhanced Python bindings integration

#pragma once

#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <map>
#include <unordered_set>
#include <stdexcept>

namespace nlm {

// Forward declarations for JSON support
namespace json = nlohmann;

// Configuration schema definition
struct ConfigSchema {
    std::string key;
    std::string type; // "int", "int64", "double", "bool", "string", "vector<int>", "vector<double>", "vector<string>"
    std::string description;
    std::optional<ConfigValue> defaultValue;
    std::function<bool(const ConfigValue&)> validator;
    bool required = false;
    
    ConfigSchema(std::string k, std::string t, std::string d, 
                 std::optional<ConfigValue> def = std::nullopt,
                 std::function<bool(const ConfigValue&)> val = nullptr,
                 bool req = false)
        : key(k), type(t), description(d), defaultValue(def), 
          validator(val), required(req) {}
};

// Default configuration values
const std::vector<ConfigSchema> DEFAULT_CONFIG_SCHEMA = {
    // Brain configuration
    ConfigSchema("brain.neuron_count", "int64", "Total number of neurons", static_cast<int64_t>(1000), 
                 [](const ConfigValue& v) { return std::holds_alternative<int64_t>(v); }, true),
    ConfigSchema("brain.synapse_density", "double", "Synapse connectivity density", 0.1,
                 [](const ConfigValue& v) { return std::holds_alternative<double>(v); }),
    ConfigSchema("brain.connection_probability", "double", "Probability of connection between neurons", 0.05,
                 [](const ConfigValue& v) { return std::holds_alternative<double>(v); }),
    
    // Plasticity configuration
    ConfigSchema("plasticity.stdp.enable", "bool", "Enable STDP plasticity", true,
                 [](const ConfigValue& v) { return std::holds_alternative<bool>(v); }),
    ConfigSchema("plasticity.stdp.learning_rate", "double", "STDP learning rate", 0.001,
                 [](const ConfigValue& v) { return std::holds_alternative<double>(v); }),
    ConfigSchema("plasticity.hebbian.enable", "bool", "Enable Hebbian plasticity", true,
                 [](const ConfigValue& v) { return std::holds_alternative<bool>(v); }),
    ConfigSchema("plasticity.structural.enable", "bool", "Enable structural plasticity", true,
                 [](const ConfigValue& v) { return std::holds_alternative<bool>(v); }),
    
    // Neuromodulation configuration
    ConfigSchema("neuromod.dopamine.scale", "double", "Dopamine modulation scale", 1.0,
                 [](const ConfigValue& v) { return std::holds_alternative<double>(v); }),
    ConfigSchema("neuromod.curiosity.enable", "bool", "Enable curiosity-driven exploration", true,
                 [](const ConfigValue& v) { return std::holds_alternative<bool>(v); }),
    ConfigSchema("neuromod.novelty.enable", "bool", "Enable novelty detection", true,
                 [](const ConfigValue& v) { return std::holds_alternative<bool>(v); }),
    
    // Simulation configuration
    ConfigSchema("simulation.timestep", "double", "Simulation timestep in seconds", 0.001,
                 [](const ConfigValue& v) { return std::holds_alternative<double>(v); }),
    ConfigSchema("simulation.max_steps", "int64", "Maximum simulation steps", 10000,
                 [](const ConfigValue& v) { return std::holds_alternative<int64_t>(v); }),
    
    // Logging configuration
    ConfigSchema("logging.level", "string", "Log level (DEBUG, INFO, WARNING, ERROR)", std::string("INFO"),
                 [](const ConfigValue& v) { return std::holds_alternative<std::string>(v); }),
    ConfigSchema("logging.to_file", "bool", "Log to file instead of console", false,
                 [](const ConfigValue& v) { return std::holds_alternative<bool>(v); }),
    ConfigSchema("logging.filename", "string", "Log filename", std::string("nlm.log"),
                 [](const ConfigValue& v) { return std::holds_alternative<std::string>(v); }),
    
    // Environment configuration
    ConfigSchema("environment.name", "string", "Environment type (GridWorld, etc.)", std::string("GridWorld"),
                 [](const ConfigValue& v) { return std::holds_alternative<std::string>(v); }),
    ConfigSchema("environment.width", "int", "Environment width in cells", 20,
                 [](const ConfigValue& v) { return std::holds_alternative<int>(v); }),
    ConfigSchema("environment.height", "int", "Environment height in cells", 20,
                 [](const ConfigValue& v) { return std::holds_alternative<int>(v); }),
};

// Validation helper functions
namespace Validation {
    bool validateRange(const ConfigValue& value, double min, double max) {
        if (std::holds_alternative<double>(value)) {
            double val = std::get<double>(value);
            return val >= min && val <= max;
        } else if (std::holds_alternative<int>(value)) {
            int val = std::get<int>(value);
            return val >= min && val <= max;
        } else if (std::holds_alternative<int64_t>(value)) {
            int64_t val = std::get<int64_t>(value);
            return val >= min && val <= max;
        }
        return false;
    }
    
    bool validatePositive(const ConfigValue& value) {
        if (std::holds_alternative<double>(value)) {
            return std::get<double>(value) > 0.0;
        } else if (std::holds_alternative<int>(value)) {
            return std::get<int>(value) > 0;
        } else if (std::holds_alternative<int64_t>(value)) {
            return std::get<int64_t>(value) > 0;
        }
        return false;
    }
    
    bool validateLogLevel(const ConfigValue& value) {
        if (const auto* str = std::get_if<std::string>(&value)) {
            static const std::unordered_set<std::string> validLevels = {
                "DEBUG", "INFO", "WARNING", "ERROR", "FATAL"
            };
            return validLevels.find(*str) != validLevels.end();
        }
        return false;
    }
}

struct Config::Impl {
    std::vector<ConfigEntry> entries;
    std::vector<ConfigSchema> schema;
    
    Impl() : entries(), schema(DEFAULT_CONFIG_SCHEMA.begin(), DEFAULT_CONFIG_SCHEMA.end()) {}
};

Config::Config() : pImpl(std::make_unique<Impl>()) {
    // Initialize with default values
    for (const auto& schema : pImpl->schema) {
        if (schema.defaultValue) {
            // Convert default value to proper type and set it
            set(schema.key, *schema.defaultValue, ConfigSource::Default);
        }
    }
}

Config::~Config() = default;

Config::Config(Config&&) noexcept = default;
Config& Config::operator=(Config&&) noexcept = default;

bool Config::loadFromFile(const std::string& filepath) {
    try {
        // Try JSON parsing first (modern approach)
        auto jsonResult = loadFromJSON(filepath);
        if (jsonResult) {
            return true;
        }
        
        // Fallback to simple key=value format (legacy)
        std::ifstream file(filepath);
        if (!file.is_open()) {
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
                
                // Remove quotes if present
                if (value.size() >= 2 && 
                    ((value.front() == '"' && value.back() == '"') ||
                     (value.front() == '\'' && value.back() == '\''))) {
                    value = value.substr(1, value.size() - 2);
                }
                
                set(key, value, ConfigSource::File);
            }
        }
        return true;
        
    } catch (const std::exception& e) {
        std::cerr << "Error loading config from file " << filepath << ": " << e.what() << std::endl;
        return false;
    }
}

bool Config::loadFromJSON(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    try {
        auto jsonData = json::json::parse(file);
        
        // Recursively process JSON to set config values
        processJSONConfig(jsonData, "");
        
        return true;
        
    } catch (const json::json::parse_error& e) {
        std::cerr << "JSON parse error in " << filepath << ": " << e.what() << std::endl;
        return false;
    } catch (const std::exception& e) {
        std::cerr << "Error loading JSON config from " << filepath << ": " << e.what() << std::endl;
        return false;
    }
}

void Config::processJSONConfig(const json::json& jsonData, const std::string& prefix) {
    for (auto it = jsonData.begin(); it != jsonData.end(); ++it) {
        std::string key = prefix.empty() ? it.key() : prefix + "." + it.key();
        
        if (it->is_object()) {
            // Recursively process nested objects
            processJSONConfig(*it, key);
        } else if (it->is_array()) {
            // Handle array types
            setArrayValue(key, *it);
        } else {
            // Handle scalar values
            setScalarValue(key, *it);
        }
    }
}

void Config::setArrayValue(const std::string& key, const json::json& jsonValue) {
    if (jsonValue.empty()) return;
    
    std::string firstType;
    std::vector<ConfigValue> values;
    
    for (const auto& item : jsonValue) {
        if (!firstType.empty()) {
            // Ensure all array elements are of the same type
            if (item.type_name() != firstType) {
                std::cerr << "Array elements in key '" << key << "' have inconsistent types" << std::endl;
                return;
            }
        } else {
            firstType = item.type_name();
        }
        
        values.push_back(jsonToConfigValue(item));
    }
    
    // Create appropriate vector type
    ConfigValue configValue;
    if (firstType == "number_integer" || firstType == "number_unsigned") {
        std::vector<int> intVector;
        intVector.reserve(values.size());
        for (const auto& val : values) {
            if (const auto* intVal = std::get_if<int>(&val)) {
                intVector.push_back(*intVal);
            }
        }
        configValue = intVector;
    } else if (firstType == "number_float") {
        std::vector<double> doubleVector;
        doubleVector.reserve(values.size());
        for (const auto& val : values) {
            if (const auto* doubleVal = std::get_if<double>(&val)) {
                doubleVector.push_back(*doubleVal);
            }
        }
        configValue = doubleVector;
    } else if (firstType == "string") {
        std::vector<std::string> stringVector;
        stringVector.reserve(values.size());
        for (const auto& val : values) {
            if (const auto* strVal = std::get_if<std::string>(&val)) {
                stringVector.push_back(*strVal);
            }
        }
        configValue = stringVector;
    }
    
    if (!values.empty()) {
        set(key, configValue, ConfigSource::File);
    }
}

void Config::setScalarValue(const std::string& key, const json::json& jsonValue) {
    ConfigValue configValue = jsonToConfigValue(jsonValue);
    if (configValue.index() != -1) {
        set(key, configValue, ConfigSource::File);
    }
}

ConfigValue Config::jsonToConfigValue(const json::json& jsonValue) {
    if (jsonValue.is_null()) {
        return std::string("null");
    } else if (jsonValue.is_boolean()) {
        return jsonValue.get<bool>();
    } else if (jsonValue.is_number_integer()) {
        return jsonValue.get<int64_t>();
    } else if (jsonValue.is_number_unsigned()) {
        return jsonValue.get<uint64_t>();
    } else if (jsonValue.is_number_float()) {
        return jsonValue.get<double>();
    } else if (jsonValue.is_string()) {
        return jsonValue.get<std::string>();
    }
    
    return std::string("");
}

bool Config::saveToFile(const std::string& filepath) const {
    try {
        // Try JSON format first (preferred)
        std::ofstream file(filepath);
        if (!file.is_open()) {
            return false;
        }
        
        // Create nested JSON structure from flattened keys
        auto jsonData = configToJSON();
        
        // Write pretty-printed JSON
        file << jsonData.dump(2);
        
        return true;
        
    } catch (const std::exception& e) {
        std::cerr << "Error saving config to " << filepath << ": " << e.what() << std::endl;
        return false;
    }
}

json::json Config::configToJSON() const {
    json::json jsonData;
    
    for (const auto& entry : pImpl->entries) {
        // Split key into nested path
        size_t dotPos = entry.key.find('.');
        if (dotPos != std::string::npos) {
            // Create nested structure
            std::string prefix = entry.key.substr(0, dotPos);
            std::string remaining = entry.key.substr(dotPos + 1);
            
            auto current = &jsonData;
            size_t start = 0;
            while (true) {
                size_t nextDot = remaining.find('.', start);
                std::string segment = (nextDot == std::string::npos) ? remaining : remaining.substr(start, nextDot - start);
                
                if (nextDot == std::string::npos) {
                    // Last segment - set value
                    (*current)[segment] = configValueToJSON(entry.value);
                    break;
                }
                
                // Navigate or create nested object
                if (!current->contains(segment)) {
                    (*current)[segment] = json::json::object();
                }
                current = &((*current)[segment]);
                
                start = nextDot + 1;
            }
        } else {
            // Top-level key
            jsonData[entry.key] = configValueToJSON(entry.value);
        }
    }
    
    return jsonData;
}

json::json Config::configValueToJSON(const ConfigValue& configValue) const {
    if (std::holds_alternative<int>(configValue)) {
        return std::get<int>(configValue);
    } else if (std::holds_alternative<int64_t>(configValue)) {
        return std::get<int64_t>(configValue);
    } else if (std::holds_alternative<double>(configValue)) {
        return std::get<double>(configValue);
    } else if (std::holds_alternative<bool>(configValue)) {
        return std::get<bool>(configValue);
    } else if (std::holds_alternative<std::string>(configValue)) {
        return std::get<std::string>(configValue);
    } else if (std::holds_alternative<std::vector<int>>(configValue)) {
        return std::get<std::vector<int>>(configValue);
    } else if (std::holds_alternative<std::vector<double>>(configValue)) {
        return std::get<std::vector<double>>(configValue);
    } else if (std::holds_alternative<std::vector<std::string>>(configValue)) {
        return std::get<std::vector<std::string>>(configValue);
    }
    
    return "";
}

bool Config::validate() const {
    bool allValid = true;
    
    for (const auto& entry : pImpl->entries) {
        // Find schema entry
        bool schemaFound = false;
        for (const auto& schema : pImpl->schema) {
            if (schema.key == entry.key) {
                schemaFound = true;
                
                // Check if value type matches schema
                if (!validateValueType(entry.value, schema.type)) {
                    std::cerr << "Config validation error: Key '" << entry.key 
                              << "' has incorrect type. Expected: " << schema.type << std::endl;
                    allValid = false;
                }
                
                // Run custom validator if provided
                if (schema.validator && !schema.validator(entry.value)) {
                    std::cerr << "Config validation error: Key '" << entry.key 
                              << "' failed custom validation" << std::endl;
                    allValid = false;
                }
                
                // Check if value is within reasonable range
                if (!validateValueRange(entry.value, schema.type)) {
                    std::cerr << "Config validation error: Key '" << entry.key 
                              << "' has value outside reasonable range" << std::endl;
                    allValid = false;
                }
                
                break;
            }
        }
        
        if (!schemaFound && entry.source != ConfigSource::Default) {
            std::cerr << "Config validation warning: Key '" << entry.key 
                      << "' has no schema definition" << std::endl;
        }
    }
    
    return allValid;
}

bool Config::validateValueType(const ConfigValue& value, const std::string& expectedType) const {
    if (expectedType == "int" && std::holds_alternative<int>(value)) return true;
    if (expectedType == "int64" && std::holds_alternative<int64_t>(value)) return true;
    if (expectedType == "double" && std::holds_alternative<double>(value)) return true;
    if (expectedType == "bool" && std::holds_alternative<bool>(value)) return true;
    if (expectedType == "string" && std::holds_alternative<std::string>(value)) return true;
    if (expectedType == "vector<int>" && std::holds_alternative<std::vector<int>>(value)) return true;
    if (expectedType == "vector<double>" && std::holds_alternative<std::vector<double>>(value)) return true;
    if (expectedType == "vector<string>" && std::holds_alternative<std::vector<std::string>>(value)) return true;
    return false;
}

bool Config::validateValueRange(const ConfigValue& value, const std::string& type) const {
    if (type == "double" && std::holds_alternative<double>(value)) {
        double val = std::get<double>(value);
        return val >= -1e10 && val <= 1e10; // Reasonable range
    } else if (type == "int" && std::holds_alternative<int>(value)) {
        int val = std::get<int>(value);
        return val >= -1e6 && val <= 1e6;
    } else if (type == "int64" && std::holds_alternative<int64_t>(value)) {
        int64_t val = std::get<int64_t>(value);
        return val >= -1e6 && val <= 1e6;
    }
    return true; // Other types have no range constraints
}

std::string Config::getValidationErrors() const {
    std::ostringstream oss;
    bool hasErrors = false;
    
    for (const auto& entry : pImpl->entries) {
        bool schemaFound = false;
        for (const auto& schema : pImpl->schema) {
            if (schema.key == entry.key) {
                schemaFound = true;
                
                if (!validateValueType(entry.value, schema.type)) {
                    oss << "ERROR: Key '" << entry.key << "' has incorrect type. Expected: " << schema.type << "\n";
                    hasErrors = true;
                }
                
                if (schema.validator && !schema.validator(entry.value)) {
                    oss << "ERROR: Key '" << entry.key << "' failed custom validation\n";
                    hasErrors = true;
                }
                
                if (!validateValueRange(entry.value, schema.type)) {
                    oss << "ERROR: Key '" << entry.key << "' has value outside reasonable range\n";
                    hasErrors = true;
                }
                
                break;
            }
        }
        
        if (!schemaFound && entry.source != ConfigSource::Default) {
            oss << "WARNING: Key '" << entry.key << "' has no schema definition\n";
        }
    }
    
    return hasErrors ? oss.str() : "All configuration values are valid.";
}

void Config::set(const std::string& key, const ConfigValue& value, ConfigSource source) {
    // Validate against schema first
    bool valid = true;
    std::string expectedType;
    
    for (const auto& schema : pImpl->schema) {
        if (schema.key == key) {
            expectedType = schema.type;
            if (schema.validator) {
                valid = schema.validator(value);
            }
            break;
        }
    }
    
    if (!valid) {
        std::cerr << "Warning: Config key '" << key << "' value failed validation" << std::endl;
    }
    
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

// Template method specializations are in Config.hpp
// Instance methods in Config.cpp for validation and error handling

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

std::string Config::summary() const {
    std::ostringstream oss;
    oss << "Configuration (" << pImpl->entries.size() << " entries):\n";
    
    // Sort entries by source priority (File > CommandLine > Default > Runtime)
    std::vector<const ConfigEntry*> sortedEntries;
    for (const auto& entry : pImpl->entries) {
        sortedEntries.push_back(&entry);
    }
    
    std::sort(sortedEntries.begin(), sortedEntries.end(),
        [](const ConfigEntry* a, const ConfigEntry* b) {
            return static_cast<int>(a->source) < static_cast<int>(b->source);
        });
    
    for (const auto* entry : sortedEntries) {
        oss << "  " << entry->key << " = [";
        std::visit([&oss](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, std::string>) {
                oss << "\"" << arg << "\"";
            } else {
                oss << arg;
            }
        }, entry->value);
        oss << "] (" << static_cast<int>(entry->source) << ")\n";
    }
    
    // Add validation status
    bool isValid = validate();
    oss << "\nValidation: " << (isValid ? "PASS" : "FAIL");
    if (!isValid) {
        oss << "\nValidation Errors:\n" << getValidationErrors();
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