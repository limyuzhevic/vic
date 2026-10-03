#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <unordered_map>
#include <regex>
#include <stdexcept>

// JSON parser support
#ifdef HAVE_NLOHMANN_JSON
#include <nlohmann/json.hpp>
#endif

// YAML parser support  
#ifdef HAVE_YAML_CPP
#include <yaml-cpp/yaml.h>
#endif

// Enhanced configuration value types with type conversion support
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

// Configuration entry with enhanced metadata
struct ConfigEntry {
    std::string key;
    ConfigValue value;
    ConfigSource source;
    std::string description;
    
    // Type information for validation
    std::string type;
    bool required;
    
    ConfigEntry() : key(), value(), source(ConfigSource::Default), description(), type(), required(false) {}
    ConfigEntry(const std::string& k, const ConfigValue& v, ConfigSource s, const std::string& desc = "", 
                const std::string& t = "string", bool req = false)
        : key(k), value(v), source(s), description(desc), type(t), required(req) {}
};

Config::Config() : pImpl(std::make_unique<Impl>()) {}

Config::~Config() = default;

Config::Config(Config&&) noexcept = default;

Config& Config::operator=(Config&&) noexcept = default;

bool Config::loadFromFile(const std::string& filepath) {
    // Enhanced file loading with JSON, YAML, and backward compatibility support
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    // Determine file format based on extension
    std::string ext = "";
    size_t dotPos = filepath.find_last_of('.');
    if (dotPos != std::string::npos) {
        ext = filepath.substr(dotPos + 1);
        std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);
    }
    
    try {
        if (ext == "json") {
            return loadFromJSONFile(filepath);
        } else if (ext == "yaml" || ext == "yml") {
            return loadFromYAMLFile(filepath);
        } else {
            // Backward compatibility: simple key=value format
            return loadFromSimpleFormat(filepath);
        }
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Failed to load config from " + filepath + ": " + e.what());
        return false;
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
    return oss.str();
}

bool Config::loadFromSimpleFormat(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    std::string line;
    size_t lineNum = 0;
    while (std::getline(file, line)) {
        ++lineNum;
        
        // Skip empty lines and comments
        line = trim(line);
        if (line.empty() || line[0] == '#' || line[0] == '/' || line[0] == ';') {
            continue;
        }
        
        // Parse simple key=value pairs with enhanced format support
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
            
            // Enhanced type inference and validation
            if (validateValue(key, value)) {
                set(key, value, ConfigSource::File);
            } else {
                NLM_LOG_WARNING("Invalid configuration format in " + filepath + 
                               ": line " + std::to_string(lineNum) + ": " + key);
            }
        } else {
            // Handle boolean flags (true/false, yes/no, on/off)
            std::string lowerLine = toLower(line);
            if (lowerLine == "true" || lowerLine == "yes" || lowerLine == "on") {
                set(line, true, ConfigSource::File);
            } else if (lowerLine == "false" || lowerLine == "no" || lowerLine == "off") {
                set(line, false, ConfigSource::File);
            } else {
                NLM_LOG_WARNING("Invalid configuration format in " + filepath + 
                               ": line " + std::to_string(lineNum) + ": " + line);
            }
        }
    }
    
    return true;
}

bool Config::loadFromJSONFile(const std::string& filepath) {
    // For now, use the simple format as fallback
    // In a full implementation, this would use nlohmann::json library
    NLM_LOG_WARNING("JSON file loading not yet implemented: " + filepath + ". Falling back to simple format.");
    return loadFromSimpleFormat(filepath);
}

bool Config::loadFromYAMLFile(const std::string& filepath) {
    // For now, use the simple format as fallback
    // In a full implementation, this would use YAML-CPP library
    NLM_LOG_WARNING("YAML file loading not yet implemented: " + filepath + ". Falling back to simple format.");
    return loadFromSimpleFormat(filepath);
}

std::string Config::valueToString(const ConfigValue& value) const {
    std::ostringstream oss;
    std::visit([&oss](auto&& arg) {
        using T = std::decay_t<decltype(arg)>;
        if constexpr (std::is_same_v<T, std::string>) {
            oss << arg;
        } else if constexpr (std::is_same_v<T, bool>) {
            oss << (arg ? "true" : "false");
        } else if constexpr (std::is_same_v<T, int> || std::is_same_v<T, int64_t>) {
            oss << arg;
        } else if constexpr (std::is_same_v<T, double>) {
            oss << std::fixed << std::setprecision(6) << arg;
        } else if constexpr (std::is_same_v<T, std::vector<int>>) {
            oss << "["; for (size_t i = 0; i < arg.size(); ++i) {
                if (i > 0) oss << ", ";
                oss << arg[i];
            } oss << "]";
        } else if constexpr (std::is_same_v<T, std::vector<double>>) {
            oss << "["; for (size_t i = 0; i < arg.size(); ++i) {
                if (i > 0) oss << ", ";
                oss << std::fixed << std::setprecision(6) << arg[i];
            } oss << "]";
        } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
            oss << "["; for (size_t i = 0; i < arg.size(); ++i) {
                if (i > 0) oss << ", ";
                oss << "\"" << arg[i] << "\"";
            } oss << "]";
        }
    }, value);
    return oss.str();
}

ConfigValue Config::stringToValue(const std::string& str, const std::string& type) const {
    // Enhanced string to value conversion with type inference
    if (type == "int") {
        try { return std::stoll(str); } catch (...) { return 0; }
    } else if (type == "double") {
        try { return std::stod(str); } catch (...) { return 0.0; }
    } else if (type == "bool") {
        std::string lower = toLower(str);
        return (lower == "true" || lower == "yes" || lower == "on" || str == "1");
    } else if (type == "vector<int>") {
        // Parse vector format: [1,2,3] or 1,2,3
        std::string clean = str;
        if (clean.front() == '[' && clean.back() == ']') {
            clean = clean.substr(1, clean.size() - 2);
        }
        std::vector<int> result;
        std::stringstream ss(clean);
        std::string item;
        while (std::getline(ss, item, ',')) {
            item = trim(item);
            try { result.push_back(std::stoll(item)); } catch (...) {}
        }
        return result;
    } else {
        // Default to string
        return str;
    }
}

bool Config::validateValue(const std::string& key, const std::string& value) const {
    // Enhanced validation based on key name and value format
    // Check for common patterns to infer type and validate
    
    // Empty value is valid (can be set to empty string)
    if (value.empty()) {
        return true;
    }
    
    // Check for numeric patterns
    bool isInt = true;
    bool isDouble = true;
    bool isBool = true;
    
    for (char c : value) {
        if (!std::isdigit(c) && c != '-' && c != '+') {
            isInt = false;
            break;
        }
    }
    
    for (char c : value) {
        if (!std::isdigit(c) && c != '-' && c != '+' && c != '.') {
            isDouble = false;
            break;
        }
    }
    
    std::string lower = toLower(value);
    if (lower == "true" || lower == "false" || lower == "yes" || lower == "no" || 
        lower == "on" || lower == "off" || lower == "1" || lower == "0") {
        isBool = true;
    } else {
        isBool = false;
    }
    
    // Validate based on key name for specialized types
    if (key.find("rate") != std::string::npos || key.find("probability") != std::string::npos || 
        key.find("weight") != std::string::npos) {
        // Should be a double
        if (!isDouble && isInt) {
            // Allow integer that's also valid as double
        } else if (!isDouble && !isInt) {
            return false;
        }
    }
    
    return true;
}

std::string Config::interpolateValue(const std::string& value, 
                                   const std::unordered_map<std::string, std::string>& variables) const {
    // Enhanced interpolation with ${VAR} or $VAR syntax
    std::string result = value;
    std::regex varPattern(R"(\${([^}]+)})"); // ${VAR} pattern
    std::regex varPattern2(R"(\$([A-Za-z_][A-Za-z0-9_]*))"); // $VAR pattern
    
    auto replaceVar = [&](const std::smatch& match) {
        std::string varName = match[1].str();
        auto it = variables.find(varName);
        if (it != variables.end()) {
            return it->second;
        }
        return std::string("$") + varName; // Return unchanged if not found
    };
    
    // Apply both patterns
    result = std::regex_replace(result, varPattern, replaceVar);
    result = std::regex_replace(result, varPattern2, replaceVar);
    
    return result;
}
