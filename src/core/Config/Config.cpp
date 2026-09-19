#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <nlohmann/json.hpp>
#include <iostream>
#include <chrono>
#include <ctime>

namespace nlm {

struct Config::Impl {
    std::vector<ConfigEntry> entries;
    std::unordered_map<std::string, std::vector<ValidationFunc>> validators;
    std::unordered_map<std::string, std::string> descriptions;
};

Config::Config() : pImpl(std::make_unique<Impl>()) {
    // Initialize with default values
    set("seed", 42, ConfigSource::Default);
    set("max_neurons", 1000, ConfigSource::Default);
    set("learning_rate", 0.01, ConfigSource::Default);
    set("max_synapses", 10000, ConfigSource::Default);
    set("enable_stdp", true, ConfigSource::Default);
}

Config::~Config() = default;

Config::Config(Config&&) noexcept = default;

Config& Config::operator=(Config&&) noexcept = default;

bool Config::loadFromFile(const std::string& filepath) {
    // Try to parse as JSON first (preferred format)
    if (loadFromJSONFile(filepath)) {
        logInfo("Loaded configuration from JSON file: " + filepath);
        return true;
    }
    
    // Try YAML format
    if (loadFromYAMLFile(filepath)) {
        logInfo("Loaded configuration from YAML file: " + filepath);
        return true;
    }
    
    // Fall back to simple key=value format for backward compatibility
    std::ifstream file(filepath);
    if (!file.is_open()) {
        logWarning("Configuration file not found: " + filepath);
        return false;
    }
    
    std::string line;
    size_t lineNum = 0;
    while (std::getline(file, line)) {
        lineNum++;
        // Skip empty lines and comments
        line = trim(line);
        if (line.empty() || line[0] == '#' || line[0] == '/') {
            continue;
        }
        
        // Parse simple key=value pairs
        size_t pos = line.find('=');
        if (pos == std::string::npos) {
            logWarning("Malformed config line " + std::to_string(lineNum) + " in " + filepath + ": " + line);
            continue;
        }
        
        std::string key = trim(line.substr(0, pos));
        std::string value = trim(line.substr(pos + 1));
        
        if (key.empty()) {
            logWarning("Empty key found in config line " + std::to_string(lineNum) + " in " + filepath);
            continue;
        }
        
        // Remove quotes if present
        if (value.size() >= 2 && 
            ((value.front() == '"' && value.back() == '"') ||
             (value.front() == '\'' && value.back() == '\''))) {
            value = value.substr(1, value.size() - 2);
        }
        
        set(key, value, ConfigSource::File);
    }
    
    if (pImpl->entries.empty()) {
        logWarning("No valid configuration entries found in " + filepath);
    } else {
        logInfo("Loaded configuration from legacy file: " + filepath);
    }
    return true;
}

bool Config::loadFromJSONFile(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    try {
        nlohmann::json jsonConfig;
        file >> jsonConfig;
        
        // Clear existing entries and reset to defaults
        pImpl->entries.clear();
        
        // Parse JSON object
        parseJSON(jsonConfig, "", ConfigSource::File);
        
        return true;
    } catch (const std::exception& e) {
        logError("Failed to parse JSON config file: " + std::string(e.what()));
        return false;
    }
}

bool Config::loadFromYAMLFile(const std::string& filepath) {
    // For now, YAML is not implemented
    // In a real implementation, we would include yaml-cpp and parse YAML
    logWarning("YAML parsing not yet implemented, only JSON and legacy formats are supported");
    return false;
}

void Config::parseJSON(const nlohmann::json& json, const std::string& prefix, ConfigSource source) {
    if (json.is_object()) {
        for (const auto& [key, value] : json.items()) {
            std::string fullKey = prefix.empty() ? key : prefix + "." + key;
            parseJSON(value, fullKey, source);
        }
    } else if (json.is_array()) {
        // Handle arrays by converting to string representation
        std::vector<std::string> stringArray;
        for (const auto& item : json) {
            if (item.is_string()) {
                stringArray.push_back(item.get<std::string>());
            } else if (item.is_number_integer()) {
                stringArray.push_back(std::to_string(item.get<int64_t>()));
            } else if (item.is_number_float()) {
                stringArray.push_back(std::to_string(item.get<double>()));
            } else if (item.is_boolean()) {
                stringArray.push_back(item.get<bool>() ? "true" : "false");
            } else {
                stringArray.push_back(item.dump());
            }
        }
        
        set(prefix, ConfigValue(stringArray), source);
    } else if (json.is_string()) {
        set(prefix, json.get<std::string>(), source);
    } else if (json.is_number_integer()) {
        set(prefix, json.get<int64_t>(), source);
    } else if (json.is_number_float()) {
        set(prefix, json.get<double>(), source);
    } else if (json.is_boolean()) {
        set(prefix, json.get<bool>(), source);
    } else if (json.is_null()) {
        // Skip null values
    }
}

ConfigValue Config::jsonToConfigValue(const nlohmann::json& value) {
    if (value.is_string()) {
        return value.get<std::string>();
    } else if (value.is_number_integer()) {
        return value.get<int64_t>();
    } else if (value.is_number_float()) {
        return value.get<double>();
    } else if (value.is_boolean()) {
        return value.get<bool>();
    } else if (value.is_array()) {
        std::vector<std::string> result;
        for (const auto& item : value) {
            if (item.is_string()) {
                result.push_back(item.get<std::string>());
            } else if (item.is_number_integer()) {
                result.push_back(std::to_string(item.get<int64_t>()));
            } else if (item.is_number_float()) {
                result.push_back(std::to_string(item.get<double>()));
            } else if (item.is_boolean()) {
                result.push_back(item.get<bool>() ? "true" : "false");
            } else {
                result.push_back(item.dump());
            }
        }
        return result;
    } else {
        return std::string("null");
    }
}

std::string Config::configValueToString(const ConfigValue& value) const {
    return std::visit([](const auto& arg) -> std::string {
        using T = std::decay_t<decltype(arg)>;
        if constexpr (std::is_same_v<T, std::string>) {
            return arg;
        } else if constexpr (std::is_same_v<T, int> || std::is_same_v<T, int64_t>) {
            return std::to_string(arg);
        } else if constexpr (std::is_same_v<T, double>) {
            return std::to_string(arg);
        } else if constexpr (std::is_same_v<T, bool>) {
            return arg ? "true" : "false";
        } else if constexpr (std::is_same_v<T, std::vector<int>>) {
            std::string result = "[";
            for (size_t i = 0; i < arg.size(); ++i) {
                if (i > 0) result += ", ";
                result += std::to_string(arg[i]);
            }
            result += "]";
            return result;
        } else if constexpr (std::is_same_v<T, std::vector<double>>) {
            std::string result = "[";
            for (size_t i = 0; i < arg.size(); ++i) {
                if (i > 0) result += ", ";
                result += std::to_string(arg[i]);
            }
            result += "]";
            return result;
        } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
            std::string result = "[";
            for (size_t i = 0; i < arg.size(); ++i) {
                if (i > 0) result += ", ";
                result += "\"" + arg[i] + "\"";
            }
            result += "]";
            return result;
        } else {
            return "";
        }
    }, value);
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

void Config::overrideFromArgs(int argc, char** argv) {
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
}

bool Config::parseCommandLineArg(const std::string& arg) {
    if (arg.substr(0, 2) == "--") {
        size_t pos = arg.find('=');
        if (pos != std::string::npos) {
            std::string key = arg.substr(2, pos - 2);
            std::string value = arg.substr(pos + 1);
            set(key, value, ConfigSource::CommandLine);
            return true;
        }
    } else if (arg[0] == '-') {
        // This is a key without value (handled separately)
        return true;
    }
    return false;
}

bool Config::saveToFile(const std::string& filepath) const {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        logError("Failed to open configuration file for writing: " + filepath);
        return false;
    }
    
    // Save in JSON format for better readability
    nlohmann::json jsonConfig;
    
    for (const auto& entry : pImpl->entries) {
        // Convert entry to JSON
        const std::string& key = entry.key;
        const ConfigValue& value = entry.value;
        
        // Handle nested keys (e.g., "network.layer1.size")
        nlohmann::json& jsonNode = jsonConfig;
        size_t dotPos = key.find('.');
        std::string remainingKey = key;
        while (dotPos != std::string::npos) {
            std::string parentKey = remainingKey.substr(0, dotPos);
            if (!jsonNode.contains(parentKey)) {
                jsonNode[parentKey] = nlohmann::json::object();
            }
            jsonNode = jsonNode[parentKey];
            remainingKey = remainingKey.substr(dotPos + 1);
            dotPos = remainingKey.find('.');
        }
        
        jsonNode[key] = configValueToJSON(value);
    }
    
    // Try to write JSON file
    try {
        file << std::setw(2) << jsonConfig << std::endl;
        logInfo("Configuration saved to JSON file: " + filepath);
        return true;
    } catch (const std::exception& e) {
        logError("Failed to save configuration to file: " + std::string(e.what()));
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
        logError("Type mismatch for key: " + key + ". Expected type T but got different type.");
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
    
    // Validate the value if validators exist
    auto validatorIt = pImpl->validators.find(key);
    if (validatorIt != pImpl->validators.end()) {
        for (const auto& validator : validatorIt->second) {
            if (!validateValue(*it, validator)) {
                logWarning("Configuration validation failed for key: " + key);
            }
        }
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
    pImpl->validators.erase(key);
    pImpl->descriptions.erase(key);
}

std::vector<std::string> Config::getKeys() const {
    std::vector<std::string> keys;
    keys.reserve(pImpl->entries.size());
    for (const auto& entry : pImpl->entries) {
        keys.push_back(entry.key);
    }
    return keys;
}

std::vector<std::string> Config::getKeysBySource(ConfigSource source) const {
    std::vector<std::string> keys;
    for (const auto& entry : pImpl->entries) {
        if (entry.source == source) {
            keys.push_back(entry.key);
        }
    }
    return keys;
}

void Config::clear() {
    pImpl->entries.clear();
    pImpl->validators.clear();
    pImpl->descriptions.clear();
}

std::string Config::summary() const {
    std::ostringstream oss;
    oss << "Configuration Summary:" << std::endl;
    oss << "Total entries: " << pImpl->entries.size() << std::endl;
    oss << "Sources: " << std::endl;
    
    for (int i = 0; i < 4; ++i) {
        ConfigSource source = static_cast<ConfigSource>(i);
        auto keys = getKeysBySource(source);
        oss << "  " << static_cast<int>(source) << ": " << keys.size() << " keys" << std::endl;
    }
    
    oss << std::endl << "All entries:" << std::endl;
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
        oss << "] (Source: " << static_cast<int>(entry.source) << ")";
        if (!entry.description.empty()) {
            oss << " - " << entry.description;
        }
        oss << std::endl;
    }
    
    return oss.str();
}

bool Config::validate(const std::vector<ConfigSchema>& schema) const {
    bool allValid = true;
    
    for (const auto& schemaEntry : schema) {
        if (!has(schemaEntry.key)) {
            if (schemaEntry.required) {
                logError("Missing required configuration key: " + schemaEntry.key);
                allValid = false;
            } else if (!schemaEntry.defaultValue.empty()) {
                // Set default value
                set(schemaEntry.key, schemaEntry.defaultValue, ConfigSource::Default);
            }
            continue;
        }
        
        auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
            [&schemaEntry](const ConfigEntry& e) { return e.key == schemaEntry.key; });n
        if (it != pImpl->entries.end()) {
            if (!validateValue(*it, [schemaEntry](const ConfigValue& value) -> bool {
                // Basic type validation
                if (schemaEntry.type == "int") {
                    return std::holds_alternative<int>(value) || std::holds_alternative<int64_t>(value);
                } else if (schemaEntry.type == "double") {
                    return std::holds_alternative<double>(value);
                } else if (schemaEntry.type == "string") {
                    return std::holds_alternative<std::string>(value);
                } else if (schemaEntry.type == "bool") {
                    return std::holds_alternative<bool>(value);
                }
                return true;
            })) {
                logError("Invalid type for configuration key: " + schemaEntry.key);
                allValid = false;
            }
            
            if (schemaEntry.validator && !validateValue(*it, schemaEntry.validator)) {
                logError("Schema validation failed for key: " + schemaEntry.key);
                allValid = false;
            }
        }
    }
    
    return allValid;
}

void Config::addValidation(const std::string& key, const ValidationFunc& validator) {
    pImpl->validators[key].push_back(validator);
}

bool Config::hasSource(const std::string& key, ConfigSource source) const {
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key, source](const ConfigEntry& e) { return e.key == key && e.source == source; });
    return it != pImpl->entries.end();
}

bool Config::equals(const Config& other) const {
    if (pImpl->entries.size() != other.pImpl->entries.size()) {
        return false;
    }
    
    for (const auto& entry : pImpl->entries) {
        auto otherIt = std::find_if(other.pImpl->entries.begin(), other.pImpl->entries.end(),
            [&entry](const ConfigEntry& e) { return e.key == entry.key; });
        
        if (otherIt == other.pImpl->entries.end() || !compareValues(entry.value, otherIt->value)) {
            return false;
        }
    }
    
    return true;
}

std::string Config::getDiff(const Config& other) const {
    std::ostringstream oss;
    oss << "Configuration Differences:" << std::endl;
    
    // Find keys in this config but not in other
    for (const auto& entry : pImpl->entries) {
        auto otherIt = std::find_if(other.pImpl->entries.begin(), other.pImpl->entries.end(),
            [&entry](const ConfigEntry& e) { return e.key == entry.key; });
        
        if (otherIt == other.pImpl->entries.end()) {
            oss << "  Added key: " << entry.key << std::endl;
        } else if (!compareValues(entry.value, otherIt->value)) {
            oss << "  Modified key: " << entry.key << std::endl;
            oss << "    From: [" << configValueToString(entry.value) << "]" << std::endl;
            oss << "    To:   [" << configValueToString(otherIt->value) << "]" << std::endl;
        }
    }
    
    // Find keys in other config but not in this
    for (const auto& entry : other.pImpl->entries) {
        auto thisIt = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
            [&entry](const ConfigEntry& e) { return e.key == entry.key; });
        
        if (thisIt == pImpl->entries.end()) {
            oss << "  Removed key: " << entry.key << std::endl;
        }
    }
    
    if (oss.str() == "Configuration Differences:\n") {
        oss << "  No differences found" << std::endl;
    }
    
    return oss.str();
}

std::string Config::toJSON() const {
    nlohmann::json jsonConfig;
    
    for (const auto& entry : pImpl->entries) {
        // Convert nested keys (e.g., "network.layer1.size") to nested JSON
        nlohmann::json& jsonNode = jsonConfig;
        size_t dotPos = entry.key.find('.');
        while (dotPos != std::string::npos) {
            std::string parentKey = entry.key.substr(0, dotPos);
            if (!jsonNode.contains(parentKey)) {
                jsonNode[parentKey] = nlohmann::json::object();
            }
            jsonNode = jsonNode[parentKey];
            entry.key.substr(dotPos + 1);
            dotPos = entry.key.find('.', dotPos + 1);
        }
        
        jsonNode[entry.key] = configValueToJSON(entry.value);
    }
    
    return jsonConfig.dump(2);
}

bool Config::fromJSON(const std::string& jsonStr) {
    try {
        nlohmann::json jsonConfig = nlohmann::json::parse(jsonStr);
        
        clear();
        
        parseJSON(jsonConfig, "", ConfigSource::File);
        
        return true;
    } catch (const std::exception& e) {
        logError("Failed to parse JSON string: " + std::string(e.what()));
        return false;
    }
}

std::vector<ConfigEntry> Config::getAllEntries() const {
    return pImpl->entries;
}

void Config::setDescription(const std::string& key, const std::string& description) {
    pImpl->descriptions[key] = description;
}

std::string Config::getDescription(const std::string& key) const {
    auto it = pImpl->descriptions.find(key);
    if (it != pImpl->descriptions.end()) {
        return it->second;
    }
    return "";
}

bool Config::validateValue(const ConfigEntry& entry, const ValidationFunc& validator) const {
    return validator(entry.value);
}

bool Config::compareValues(const ConfigValue& a, const ConfigValue& b) {
    return std::visit([](const auto& argA, const auto& argB) -> bool {
        using TA = std::decay_t<decltype(argA)>;
        using TB = std::decay_t<decltype(argB)>;
        
        if constexpr (std::is_same_v<TA, TB>) {
            return argA == argB;
        } else {
            // Try to convert to string for comparison
            std::string strA = std::visit([](const auto& arg) -> std::string {
                using T = std::decay_t<decltype(arg)>;
                if constexpr (std::is_same_v<T, int> || std::is_same_v<T, int64_t>) {
                    return std::to_string(arg);
                } else if constexpr (std::is_same_v<T, double>) {
                    return std::to_string(arg);
                } else if constexpr (std::is_same_v<T, bool>) {
                    return arg ? "true" : "false";
                } else if constexpr (std::is_same_v<T, std::string>) {
                    return arg;
                } else if constexpr (std::is_same_v<T, std::vector<int>>) {
                    std::string result = "[";
                    for (size_t i = 0; i < arg.size(); ++i) {
                        if (i > 0) result += ", ";
                        result += std::to_string(arg[i]);
                    }
                    result += "]";
                    return result;
                } else if constexpr (std::is_same_v<T, std::vector<double>>) {
                    std::string result = "[";
                    for (size_t i = 0; i < arg.size(); ++i) {
                        if (i > 0) result += ", ";
                        result += std::to_string(arg[i]);
                    }
                    result += "]";
                    return result;
                } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
                    std::string result = "[";
                    for (size_t i = 0; i < arg.size(); ++i) {
                        if (i > 0) result += ", ";
                        result += "\"" + arg[i] + "\"";
                    }
                    result += "]";
                    return result;
                } else {
                    return "";
                }
            }, argA);
            
            std::string strB = std::visit([](const auto& arg) -> std::string {
                using T = std::decay_t<decltype(arg)>;
                if constexpr (std::is_same_v<T, int> || std::is_same_v<T, int64_t>) {
                    return std::to_string(arg);
                } else if constexpr (std::is_same_v<T, double>) {
                    return std::to_string(arg);
                } else if constexpr (std::is_same_v<T, bool>) {
                    return arg ? "true" : "false";
                } else if constexpr (std::is_same_v<T, std::string>) {
                    return arg;
                } else if constexpr (std::is_same_v<T, std::vector<int>>) {
                    std::string result = "[";
                    for (size_t i = 0; i < arg.size(); ++i) {
                        if (i > 0) result += ", ";
                        result += std::to_string(arg[i]);
                    }
                    result += "]";
                    return result;
                } else if constexpr (std::is_same_v<T, std::vector<double>>) {
                    std::string result = "[";
                    for (size_t i = 0; i < arg.size(); ++i) {
                        if (i > 0) result += ", ";
                        result += std::to_string(arg[i]);
                    }
                    result += "]";
                    return result;
                } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
                    std::string result = "[";
                    for (size_t i = 0; i < arg.size(); ++i) {
                        if (i > 0) result += ", ";
                        result += "\"" + arg[i] + "\"";
                    }
                    result += "]";
                    return result;
                } else {
                    return "";
                }
            }, argB);
            
            return strA == strB;
        }
    }, a, b);
}

nlohmann::json Config::configValueToJSON(const ConfigValue& value) const {
    return std::visit([](const auto& arg) -> nlohmann::json {
        using T = std::decay_t<decltype(arg)>;
        if constexpr (std::is_same_v<T, int>) {
            return arg;
        } else if constexpr (std::is_same_v<T, int64_t>) {
            return arg;
        } else if constexpr (std::is_same_v<T, double>) {
            return arg;
        } else if constexpr (std::is_same_v<T, bool>) {
            return arg;
        } else if constexpr (std::is_same_v<T, std::string>) {
            return arg;
        } else if constexpr (std::is_same_v<T, std::vector<int>>) {
            nlohmann::json result = nlohmann::json::array();
            for (const auto& item : arg) {
                result.push_back(item);
            }
            return result;
        } else if constexpr (std::is_same_v<T, std::vector<double>>) {
            nlohmann::json result = nlohmann::json::array();
            for (const auto& item : arg) {
                result.push_back(item);
            }
            return result;
        } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
            nlohmann::json result = nlohmann::json::array();
            for (const auto& item : arg) {
                result.push_back(item);
            }
            return result;
        } else {
            return nullptr;
        }
    }, value);
}

ConfigValue Config::jsonToConfigValue(const nlohmann::json& value) {
    if (value.is_string()) {
        return value.get<std::string>();
    } else if (value.is_number_integer()) {
        return value.get<int64_t>();
    } else if (value.is_number_float()) {
        return value.get<double>();
    } else if (value.is_boolean()) {
        return value.get<bool>();
    } else if (value.is_array()) {
        std::vector<std::string> result;
        for (const auto& item : value) {
            if (item.is_string()) {
                result.push_back(item.get<std::string>());
            } else if (item.is_number_integer()) {
                result.push_back(std::to_string(item.get<int64_t>()));
            } else if (item.is_number_float()) {
                result.push_back(std::to_string(item.get<double>()));
            } else if (item.is_boolean()) {
                result.push_back(item.get<bool>() ? "true" : "false");
            } else {
                result.push_back(item.dump());
            }
        }
        return result;
    } else {
        return std::string("null");
    }
}

void Config::logError(const std::string& message) const {
    std::cerr << "[ERROR] " << message << std::endl;
}

void Config::logWarning(const std::string& message) const {
    std::cerr << "[WARNING] " << message << std::endl;
}

void Config::logInfo(const std::string& message) const {
    std::cout << "[INFO] " << message << std::endl;
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