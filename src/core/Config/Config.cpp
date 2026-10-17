#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <iostream>
#include <cstring>
#include <ctime>

namespace nlm {

struct Config::Impl {
    std::vector<ConfigEntry> entries;
    std::vector<ConfigSchema> schemas;
    std::unordered_map<std::vector<std::string>, ConfigValue> nestedValues;
    std::unordered_map<std::string, nlohmann::json> jsonCache;
    bool schemaLoaded = false;
};

Config::Config() : pImpl(std::make_unique<Impl>()) {}

Config::~Config() = default;

Config::Config(Config&&) noexcept = default;

Config& Config::operator=(Config&&) noexcept = default;

bool Config::loadFromFile(const std::string& filepath) {
    try {
        std::ifstream file(filepath);
        if (!file.is_open()) {
            return false;
        }
        
        // Parse JSON file
        nlohmann::json jsonData;
        file >> jsonData;
        
        // Convert JSON to Config entries
        flattenJsonToConfig(jsonData, "", ConfigSource::File);
        
        return true;
    } catch (const std::exception& e) {
        std::cerr << "Error loading config file " << filepath << ": " << e.what() << std::endl;
        return false;
    }
}

bool Config::loadFromArgs(int argc, char** argv) {
    bool success = true;
    for (int i = 1; i < argc; ++i) {
        std::string arg(argv[i]);
        
        // Handle --key=value format
        if (arg.substr(0, 2) == "--") {
            size_t pos = arg.find('=');
            if (pos != std::string::npos) {
                std::string key = arg.substr(2, pos - 2);
                std::string value = arg.substr(pos + 1);
                set(key, value, ConfigSource::CommandLine);
            } else {
                success = false;
            }
        }
        // Handle -key value format
        else if (arg[0] == '-' && i + 1 < argc) {
            std::string key = arg.substr(1);
            std::string value = argv[++i];
            set(key, value, ConfigSource::CommandLine);
        } else {
            success = false;
        }
    }
    return success;
}

bool Config::loadFromEnv(const std::string& prefix) {
    bool success = true;
    const char* envPtr = nullptr;
    size_t prefixLen = prefix.length();
    
    // Iterate through environment variables
    for (const char** env = environ; *env != nullptr; ++env) {
        std::string envStr(*env);
        size_t equalsPos = envStr.find('=');
        
        if (equalsPos != std::string::npos && envStr.size() > prefixLen &&
            envStr.substr(0, prefixLen) == prefix) {
            
            std::string key = envStr.substr(prefixLen, equalsPos - prefixLen);
            std::string value = envStr.substr(equalsPos + 1);
            
            // Convert underscore to dot for nested keys
            std::replace(key.begin(), key.end(), '_', '.');
            
            setNested(parsePath(key), ConfigValue(value), ConfigSource::Environment);
        }
    }
    
    return success;
}

bool Config::saveToFile(const std::string& filepath) const {
    try {
        std::ofstream file(filepath);
        if (!file.is_open()) {
            return false;
        }
        
        // Convert to JSON and save
        nlohmann::json jsonData = toJson();
        file << jsonData.dump(2); // Pretty print with 2-space indent
        
        return true;
    } catch (const std::exception& e) {
        std::cerr << "Error saving config file " << filepath << ": " << e.what() << std::endl;
        return false;
    }
}

template<typename T>
std::optional<T> Config::get(const std::string& key) const {
    return getOr<T>(key, T{});
}

template<typename T>
T Config::getOr(const std::string& key, const T& defaultValue) const {
    auto value = get<T>({key});
    return value.has_value() ? value.value() : defaultValue;
}

template<typename T>
std::optional<T> Config::get(const std::vector<std::string>& path) const {
    // First check if it's a direct key match
    auto value = getOr<T>(path, T{});
    
    // Try to find it in nested values
    auto nestedIt = pImpl->nestedValues.find(path);
    if (nestedIt != pImpl->nestedValues.end()) {
        try {
            T result = std::get<T>(nestedIt->second);
            return result;
        } catch (const std::bad_variant_access&) {
            return std::nullopt;
        }
    }
    
    // Also check if the flattened key matches
    std::string flatKey = joinPath(path);
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&flatKey](const ConfigEntry& e) { return e.key == flatKey; });
    
    if (it != pImpl->entries.end()) {
        try {
            T result = std::get<T>(it->value);
            return result;
        } catch (const std::bad_variant_access&) {
            return std::nullopt;
        }
    }
    
    return std::nullopt;
}

template<typename T>
T Config::getOr(const std::vector<std::string>& path, const T& defaultValue) const {
    auto value = get<T>(path);
    return value.has_value() ? value.value() : defaultValue;
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

bool Config::setNested(const std::vector<std::string>& path, const ConfigValue& value, ConfigSource source) {
    if (path.empty()) return false;
    
    std::string flatKey = joinPath(path);
    
    // Check if key already exists
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&flatKey](const ConfigEntry& e) { return e.key == flatKey; });
    
    ConfigEntry entry(flatKey, value, source);
    entry.configPath = flatKey;
    
    if (it != pImpl->entries.end()) {
        *it = entry;
    } else {
        pImpl->entries.push_back(entry);
    }
    
    // Update nested values map
    pImpl->nestedValues[path] = value;
    
    return true;
}

bool Config::has(const std::string& key) const {
    // Check direct entries
    bool hasDirect = std::any_of(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    
    if (hasDirect) return true;
    
    // Check if key matches any nested path
    std::vector<std::string> path = parsePath(key);
    bool hasNested = pImpl->nestedValues.find(path) != pImpl->nestedValues.end();
    
    return hasNested;
}

bool Config::hasNested(const std::vector<std::string>& path) const {
    if (path.empty()) return false;
    
    // Check if the exact path exists
    bool hasExact = pImpl->nestedValues.find(path) != pImpl->nestedValues.end();
    
    if (hasExact) return true;
    
    // Check if path is a prefix of any existing nested value
    for (const auto& nestedPath : pImpl->nestedValues) {
        bool isPrefix = true;
        if (nestedPath.first.size() < path.size()) {
            isPrefix = false;
        } else {
            for (size_t i = 0; i < path.size(); ++i) {
                if (nestedPath.first[i] != path[i]) {
                    isPrefix = false;
                    break;
                }
            }
        }
        if (isPrefix) return true;
    }
    
    return false;
}

void Config::remove(const std::string& key) {
    // Remove from direct entries
    pImpl->entries.erase(
        std::remove_if(pImpl->entries.begin(), pImpl->entries.end(),
            [&key](const ConfigEntry& e) { return e.key == key; }),
        pImpl->entries.end()
    );
    
    // Remove from nested values
    std::vector<std::string> path = parsePath(key);
    pImpl->nestedValues.erase(path);
}

void Config::removeNested(const std::vector<std::string>& path) {
    if (path.empty()) return;
    
    std::string flatKey = joinPath(path);
    
    // Remove from direct entries
    pImpl->entries.erase(
        std::remove_if(pImpl->entries.begin(), pImpl->entries.end(),
            [&flatKey](const ConfigEntry& e) { return e.key == flatKey; }),
        pImpl->entries.end()
    );
    
    // Remove from nested values
    pImpl->nestedValues.erase(path);
}

std::vector<std::string> Config::getKeys() const {
    std::vector<std::string> keys;
    keys.reserve(pImpl->entries.size());
    for (const auto& entry : pImpl->entries) {
        keys.push_back(entry.key);
    }
    return keys;
}

std::vector<std::vector<std::string>> Config::getNestedPaths() const {
    std::vector<std::vector<std::string>> paths;
    paths.reserve(pImpl->nestedValues.size());
    for (const auto& pair : pImpl->nestedValues) {
        paths.push_back(pair.first);
    }
    return paths;
}

void Config::clear() {
    pImpl->entries.clear();
    pImpl->nestedValues.clear();
    pImpl->schemaLoaded = false;
}

std::string Config::summary() const {
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
        oss << "] (" << static_cast<int>(entry.source) << ")\n";
    }
    
    if (!pImpl->nestedValues.empty()) {
        oss << "\nNested values:";
        for (const auto& pair : pImpl->nestedValues) {
            oss << "\n  [";
            for (size_t i = 0; i < pair.first.size(); ++i) {
                if (i > 0) oss << ".";
                oss << pair.first[i];
            }
            oss << "] = [";
            std::visit([&oss](auto&& arg) {
                using T = std::decay_t<decltype(arg)>;
                if constexpr (std::is_same_v<T, std::string>) {
                    oss << "\"" << arg << "\"";
                } else {
                    oss << arg;
                }
            }, pair.second);
            oss << "]";
        }
    }
    
    return oss.str();
}

bool Config::validateSchema(const std::vector<ConfigSchema>& schema) const {
    bool valid = true;
    
    for (const auto& schemaEntry : schema) {
        if (!validateSchemaEntry(schemaEntry)) {
            valid = false;
        }
    }
    
    return valid;
}

void Config::addSchema(const std::vector<ConfigSchema>& schema) {
    pImpl->schemas.insert(pImpl->schemas.end(), schema.begin(), schema.end());
    pImpl->schemaLoaded = true;
}

bool Config::validateValue(const ConfigSchema& schema, const ConfigValue& value) const {
    // Check type
    std::string typeStr;
    if (std::holds_alternative<int>(value)) typeStr = "int";
    else if (std::holds_alternative<int64_t>(value)) typeStr = "int64";
    else if (std::holds_alternative<double>(value)) typeStr = "double";
    else if (std::holds_alternative<bool>(value)) typeStr = "bool";
    else if (std::holds_alternative<std::string>(value)) typeStr = "string";
    else if (std::holds_alternative<std::vector<int>>(value)) typeStr = "array_int";
    else if (std::holds_alternative<std::vector<double>>(value)) typeStr = "array_double";
    else if (std::holds_alternative<std::vector<std::string>>(value)) typeStr = "array_string";
    else typeStr = "unknown";
    
    if (typeStr != schema.type) {
        return false;
    }
    
    // Check allowed values
    if (!schema.allowedValues.empty()) {
        std::optional<std::string> strValue;
        if (std::holds_alternative<std::string>(value)) {
            strValue = std::get<std::string>(value);
        }
        
        bool found = false;
        for (const auto& allowed : schema.allowedValues) {
            if (strValue && *strValue == allowed) {
                found = true;
                break;
            }
        }
        
        if (!found && schema.allowedValues.size() > 0) {
            return false;
        }
    }
    
    // Check pattern validation for strings
    if (schema.type == "string" && !schema.validationPattern.empty()) {
        if (std::holds_alternative<std::string>(value)) {
            const std::string& strVal = std::get<std::string>(value);
            // Simple pattern matching - could use regex for more complex patterns
            if (strVal.find(schema.validationPattern) == std::string::npos) {
                return false;
            }
        } else {
            return false;
        }
    }
    
    return true;
}

std::vector<std::string> Config::parsePath(const std::string& pathStr) {
    std::vector<std::string> path;
    std::string current;
    bool inQuotes = false;
    
    for (char c : pathStr) {
        if (c == '\"' || c == '\'') {
            inQuotes = !inQuotes;
            current += c;
        } else if (c == '.' && !inQuotes) {
            if (!current.empty()) {
                path.push_back(current);
                current.clear();
            }
        } else {
            current += c;
        }
    }
    
    if (!current.empty()) {
        path.push_back(current);
    }
    
    return path;
}

std::string Config::joinPath(const std::vector<std::string>& path) {
    if (path.empty()) return "";
    
    std::string result = path[0];
    for (size_t i = 1; i < path.size(); ++i) {
        result += "." + path[i];
    }
    
    return result;
}

nlohmann::json Config::toJson() const {
    nlohmann::json result;
    
    // Add flat entries
    for (const auto& entry : pImpl->entries) {
        result[entry.key] = configValueToJson(entry.value);
    }
    
    // Add nested values
    for (const auto& pair : pImpl->nestedValues) {
        std::string key = joinPath(pair.first);
        result[key] = configValueToJson(pair.second);
    }
    
    return result;
}

bool Config::loadFromJsonString(const std::string& jsonStr) {
    try {
        nlohmann::json jsonData = nlohmann::json::parse(jsonStr);
        flattenJsonToConfig(jsonData, "", ConfigSource::File);
        return true;
    } catch (const std::exception& e) {
        std::cerr << "Error parsing JSON config: " << e.what() << std::endl;
        return false;
    }
}

std::vector<ConfigEntry> Config::getAllEntries() const {
    return pImpl->entries;
}

// Private helper methods
void Config::flattenJsonToConfig(const nlohmann::json& json, const std::string& prefix, ConfigSource source) {
    if (json.is_object()) {
        for (const auto& item : json.items()) {
            std::string key = prefix.empty() ? item.key() : prefix + "." + item.key();
            if (item.value().is_object()) {
                // Recursively process nested objects
                flattenJsonToConfig(item.value(), key, source);
            } else {
                // Convert JSON value to ConfigValue and store
                std::optional<ConfigValue> configVal = jsonToConfigValue(item.value(), "");
                if (configVal.has_value()) {
                    setNested(parsePath(key), configVal.value(), source);
                }
            }
        }
    } else if (json.is_array()) {
        // Handle arrays (store as string representation for now)
        std::string key = prefix.empty() ? "array" : prefix + ".array";
        setNested(parsePath(key), ConfigValue(json.dump()), source);
    } else {
        // Handle primitive values
        if (!prefix.empty()) {
            std::optional<ConfigValue> configVal = jsonToConfigValue(json, "");
            if (configVal.has_value()) {
                setNested(parsePath(prefix), configVal.value(), source);
            }
        }
    }
}

nlohmann::json Config::configValueToJson(const ConfigValue& value) {
    nlohmann::json result;
    
    std::visit([&result](auto&& arg) {
        using T = std::decay_t<decltype(arg)>;
        if constexpr (std::is_same_v<T, int>) {
            result = arg;
        } else if constexpr (std::is_same_v<T, int64_t>) {
            result = arg;
        } else if constexpr (std::is_same_v<T, double>) {
            result = arg;
        } else if constexpr (std::is_same_v<T, bool>) {
            result = arg;
        } else if constexpr (std::is_same_v<T, std::string>) {
            result = arg;
        } else if constexpr (std::is_same_v<T, std::vector<int>>) {
            result = arg;
        } else if constexpr (std::is_same_v<T, std::vector<double>>) {
            result = arg;
        } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
            result = arg;
        }
    }, value);
    
    return result;
}

std::optional<ConfigValue> Config::jsonToConfigValue(const nlohmann::json& json, const std::string& expectedType) {
    try {
        if (json.is_number_integer()) {
            return json.get<int>();
        } else if (json.is_number_float()) {
            return json.get<double>();
        } else if (json.is_boolean()) {
            return json.get<bool>();
        } else if (json.is_string()) {
            return json.get<std::string>();
        } else if (json.is_array()) {
            // Determine array element type
            if (!json.empty()) {
                if (json[0].is_number_integer()) {
                    std::vector<int> intArray;
                    for (const auto& item : json) {
                        intArray.push_back(item.get<int>());
                    }
                    return intArray;
                } else if (json[0].is_number_float()) {
                    std::vector<double> doubleArray;
                    for (const auto& item : json) {
                        doubleArray.push_back(item.get<double>());
                    }
                    return doubleArray;
                } else if (json[0].is_string()) {
                    std::vector<std::string> stringArray;
                    for (const auto& item : json) {
                        stringArray.push_back(item.get<std::string>());
                    }
                    return stringArray;
                }
            }
            // Empty array - default to string array
            return std::vector<std::string>();
        }
    } catch (const std::exception& e) {
        std::cerr << "Error converting JSON to ConfigValue: " << e.what() << std::endl;
    }
    
    return std::nullopt;
}

// Validation helper
bool Config::validateSchemaEntry(const ConfigSchema& schema) const {
    // Check if schema key exists in configuration
    bool keyExists = has(schema.key);
    
    if (schema.required && !keyExists) {
        return false;
    }
    
    if (keyExists && !schema.allowedValues.empty()) {
        // Additional validation logic for allowed values
        // This would need to be implemented based on specific requirements
    }
    
    return true;
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

// Template implementation for nested access
template<typename T>
std::optional<T> Config::get(const std::vector<std::string>& path) const {
    // First check if it's a direct key match
    auto value = getOr<T>(path, T{});
    
    // Try to find it in nested values
    auto nestedIt = pImpl->nestedValues.find(path);
    if (nestedIt != pImpl->nestedValues.end()) {
        try {
            T result = std::get<T>(nestedIt->second);
            return result;
        } catch (const std::bad_variant_access&) {
            return std::nullopt;
        }
    }
    
    // Also check if the flattened key matches
    std::string flatKey = joinPath(path);
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&flatKey](const ConfigEntry& e) { return e.key == flatKey; });
    
    if (it != pImpl->entries.end()) {
        try {
            T result = std::get<T>(it->value);
            return result;
        } catch (const std::bad_variant_access&) {
            return std::nullopt;
        }
    }
    
    return std::nullopt;
}

template<typename T>
T Config::getOr(const std::vector<std::string>& path, const T& defaultValue) const {
    auto value = get<T>(path);
    return value.has_value() ? value.value() : defaultValue;
}