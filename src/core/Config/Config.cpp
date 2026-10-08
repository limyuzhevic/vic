#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <regex>
#include <yaml-cpp/yaml.h>
#include <nlohmann/json.hpp>

namespace nlm {

struct Config::Impl {
    std::vector<ConfigEntry> entries;
    std::map<std::string, ConfigGroup> groups;
    std::map<std::string, std::vector<std::string>> keyToGroups;
    std::map<std::string, std::map<std::string, Config>> inheritance;
    std::map<std::string, std::vector<ValidationRule>> entryValidators;
};

Config::Config() : pImpl(std::make_unique<Impl>()), reloadRequired_(false) {}

Config::~Config() = default;

Config::Config(Config&&) noexcept = default;

Config& Config::operator=(Config&&) noexcept = default;

bool Config::loadFromFile(const std::string& filepath, ConfigSource source) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    std::string content((std::istreambuf_iterator<char>(file)), 
                        std::istreambuf_iterator<char>());
    return loadFromString(content, source);
}

bool Config::loadFromString(const std::string& content, ConfigSource source) {
    // Detect file format
    if (content.find("{") != std::string::npos && content.find("}") != std::string::npos) {
        return loadFromJSON(content, source);
    } else if (content.find(":") != std::string::npos) {
        return loadFromYAML(content, source);
    } else {
        // Try simple key=value format for backward compatibility
        return loadFromArgsInternal(content, source);
    }
}

bool Config::loadFromArgs(int argc, char** argv, ConfigSource source) {
    std::string argsContent;
    for (int i = 1; i < argc; ++i) {
        argsContent += std::string(argv[i]) + "\n";
    }
    return loadFromArgsInternal(argsContent, source);
}

bool Config::loadFromArgsInternal(const std::string& argsContent, ConfigSource source) {
    std::istringstream stream(argsContent);
    std::string line;
    
    while (std::getline(stream, line)) {
        line = trim(line);
        if (line.empty() || line[0] == '#') {
            continue;
        }
        
        // Parse simple key=value pairs
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
            
            set(key, value, source);
        }
    }
    return true;
}

bool Config::loadFromEnvironment(const std::map<std::string, std::string>& envVars, 
                                const std::string& prefix, ConfigSource source) {
    bool success = false;
    
    for (const auto& pair : envVars) {
        const std::string& key = pair.first;
        const std::string& value = pair.second;
        
        if (key.find(prefix) == 0) {
            std::string configKey = key.substr(prefix.size());
            // Convert underscore notation to dot notation for nested keys
            std::replace(configKey.begin(), configKey.end(), '_', '.');
            
            if (setFromEnvironmentString(configKey, value)) {
                success = true;
            }
        }
    }
    
    return success;
}

bool Config::loadFromPython(const std::map<std::string, std::string>& pythonVars,
                           ConfigSource source) {
    bool success = false;
    
    for (const auto& pair : pythonVars) {
        const std::string& key = pair.first;
        const std::string& value = pair.second;
        
        // Python variables typically use underscore notation
        std::string configKey = key;
        std::replace(configKey.begin(), configKey.end(), '_', '.');
        
        if (setFromPythonString(configKey, value)) {
            success = true;
        }
    }
    
    return success;
}

bool Config::saveToFile(const std::string& filepath, bool prettyPrint) const {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    // Choose format based on file extension
    if (isJSONFile(filepath)) {
        nlohmann::json jsonConfig = toJSON();
        if (prettyPrint) {
            file << jsonConfig.dump(2);
        } else {
            file << jsonConfig.dump();
        }
    } else if (isYAMLError(filepath)) {
        YAML::Emitter emitter;
        toYAML(emitter);
        file << emitter.c_str();
    } else {
        // Default to simple key=value format for backward compatibility
        for (const auto& entry : pImpl->entries) {
            file << "# " << entry.description << "\n";
            file << entry.key << " = " << getValueAsString(entry.value) << "\n";
            file << "\n";
        }
    }
    
    return true;
}

void Config::set(const std::string& key, const ConfigValue& value, 
                 ConfigSource source, const std::string& description,
                 const std::vector<std::string>& tags) {
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    
    if (it != pImpl->entries.end()) {
        it->value = value;
        it->source = source;
        if (!description.empty()) it->description = description;
        it->tags = tags;
    } else {
        ConfigEntry newEntry(key, value, source, description, tags);
        pImpl->entries.push_back(newEntry);
    }
    
    // Mark for reload if this is a configuration change
    reloadRequired_ = true;
}

bool Config::has(const std::string& key) const {
    return std::any_of(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
}

bool Config::hasType(const std::string& key, const std::type_info& type) const {
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    
    if (it == pImpl->entries.end()) {
        return false;
    }
    
    return typeMatches(it->value, type);
}

void Config::remove(const std::string& key) {
    pImpl->entries.erase(
        std::remove_if(pImpl->entries.begin(), pImpl->entries.end(),
            [&key](const ConfigEntry& e) { return e.key == key; }),
        pImpl->entries.end()
    );
    
    // Clean up group associations
    for (auto& pair : pImpl->keyToGroups) {
        auto& groups = pair.second;
        groups.erase(std::remove(groups.begin(), groups.end(), key), groups.end());
    }
    
    // Mark for reload
    reloadRequired_ = true;
}

std::vector<std::string> Config::getKeys() const {
    std::vector<std::string> keys;
    keys.reserve(pImpl->entries.size());
    for (const auto& entry : pImpl->entries) {
        keys.push_back(entry.key);
    }
    return keys;
}

std::vector<std::string> Config::getKeys(const std::string& tag) const {
    std::vector<std::string> keys;
    for (const auto& entry : pImpl->entries) {
        if (std::find(entry.tags.begin(), entry.tags.end(), tag) != entry.tags.end()) {
            keys.push_back(entry.key);
        }
    }
    return keys;
}

void Config::createGroup(const std::string& groupName, const std::string& description) {
    ConfigGroup group(groupName, description);
    pImpl->groups[groupName] = group;
}

bool Config::hasGroup(const std::string& groupName) const {
    return pImpl->groups.find(groupName) != pImpl->groups.end();
}

void Config::addToGroup(const std::string& key, const std::string& groupName) {
    if (hasGroup(groupName)) {
        pImpl->keyToGroups[key].push_back(groupName);
    }
}

std::vector<std::string> Config::getGroups() const {
    std::vector<std::string> groups;
    groups.reserve(pImpl->groups.size());
    for (const auto& pair : pImpl->groups) {
        groups.push_back(pair.first);
    }
    return groups;
}

std::vector<std::string> Config::getKeysInGroup(const std::string& groupName) const {
    std::vector<std::string> keys;
    if (!hasGroup(groupName)) {
        return keys;
    }
    
    for (const auto& pair : pImpl->keyToGroups) {
        const auto& groups = pair.second;
        if (std::find(groups.begin(), groups.end(), groupName) != groups.end()) {
            keys.push_back(pair.first);
        }
    }
    return keys;
}

bool Config::validateAll() const {
    for (const auto& entry : pImpl->entries) {
        if (!entry.validate()) {
            return false;
        }
    }
    return true;
}

std::vector<std::string> Config::getValidationErrors() const {
    std::vector<std::string> errors;
    for (const auto& entry : pImpl->entries) {
        std::string error = entry.getValidationError();
        if (!error.empty()) {
            errors.push_back(error);
        }
    }
    return errors;
}

void Config::addValidator(const std::string& key, const ValidationRule& validator) {
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });n    
    if (it != pImpl->entries.end()) {
        it->validators.push_back(validator);
        pImpl->entryValidators[key].push_back(validator);
    }
}

void Config::setRangeValidator(const std::string& key, double min, double max,
                               const std::string& desc) {
    ValidationRule rule = rangeValidator(key + "_range", min, max, desc);
    addValidator(key, rule);
}

void Config::setEnumValidator(const std::string& key, const std::vector<std::string>& values,
                              const std::string& desc) {
    ValidationRule rule = enumValidator(key + "_enum", values, desc);
    addValidator(key, rule);
}

void Config::setPatternValidator(const std::string& key, const std::string& pattern,
                                 const std::string& desc) {
    ValidationRule rule = patternValidator(key + "_pattern", pattern, desc);
    addValidator(key, rule);
}

void Config::setInheritance(const std::string& parentKey, const Config& parentConfig) {
    pImpl->inheritance[parentKey] = parentConfig;
}

void Config::merge(const Config& other, ConfigSource source) {
    for (const auto& entry : other.pImpl->entries) {
        // Check if key already exists
        bool exists = false;
        for (const auto& existingEntry : pImpl->entries) {
            if (existingEntry.key == entry.key) {
                exists = true;
                break;
            }
        }
        
        if (!exists) {
            ConfigEntry newEntry = entry;
            newEntry.source = source;
            pImpl->entries.push_back(newEntry);
        }
    }
}

void Config::reload() {
    // Implementation for reloading configuration
    reloadRequired_ = false;
}

std::string Config::summary() const {
    std::ostringstream oss;
    oss << "Configuration (" << pImpl->entries.size() << " entries, " << pImpl->groups.size() << " groups):\n";
    
    // Summary by group
    for (const auto& groupPair : pImpl->groups) {
        oss << "  Group: " << groupPair.first << " (" << groupPair.second.description << ")\n";
        auto keys = getKeysInGroup(groupPair.first);
        for (const auto& key : keys) {
            auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
                [&key](const ConfigEntry& e) { return e.key == key; });n            if (it != pImpl->entries.end()) {
                oss << "    " << it->key << " = [";
                oss << getValueAsString(it->value) << "] (source: " << static_cast<int>(it->source) << ")\n";
            }
        }
    }
    
    // Ungrouped entries
    auto allKeys = getKeys();
    std::vector<std::string> groupedKeys;
    for (const auto& pair : pImpl->keyToGroups) {
        groupedKeys.push_back(pair.first);
    }
    
    std::vector<std::string> ungroupedKeys;
    for (const auto& key : allKeys) {
        if (std::find(groupedKeys.begin(), groupedKeys.end(), key) == groupedKeys.end()) {
            ungroupedKeys.push_back(key);
        }
    }
    
    if (!ungroupedKeys.empty()) {
        oss << "  Ungrouped:\n";
        for (const auto& key : ungroupedKeys) {
            auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
                [&key](const ConfigEntry& e) { return e.key == key; });n            if (it != pImpl->entries.end()) {
                oss << "    " << it->key << " = [";
                oss << getValueAsString(it->value) << "] (source: " << static_cast<int>(it->source) << ")\n";
            }
        }
    }
    
    return oss.str();
}

std::string Config::summary(const std::string& group) const {
    std::ostringstream oss;
    if (!hasGroup(group)) {
        return "Group not found: " + group + "\n";
    }
    
    oss << "Configuration summary for group: " << group << "\n";
    auto keys = getKeysInGroup(group);
    
    for (const auto& key : keys) {
        auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
            [&key](const ConfigEntry& e) { return e.key == key; });n        if (it != pImpl->entries.end()) {
            oss << "  " << it->key << " = [";
            oss << getValueAsString(it->value) << "] (source: " << static_cast<int>(it->source) << ")\n";
            if (!it->description.empty()) {
                oss << "    Description: " << it->description << "\n";
            }
            if (!it->tags.empty()) {
                oss << "    Tags: ";
                for (size_t i = 0; i < it->tags.size(); ++i) {
                    if (i > 0) oss << ", ";
                    oss << it->tags[i];
                }
                oss << "\n";
            }
        }
    }
    
    return oss.str();
}

std::vector<std::string> Config::getSources() const {
    std::vector<std::string> sources;
    for (const auto& entry : pImpl->entries) {
        std::string sourceStr;
        switch (entry.source) {
            case ConfigSource::Default: sourceStr = "Default"; break;
            case ConfigSource::File: sourceStr = "File"; break;
            case ConfigSource::CommandLine: sourceStr = "CommandLine"; break;
            case ConfigSource::Environment: sourceStr = "Environment"; break;
            case ConfigSource::Runtime: sourceStr = "Runtime"; break;
            case ConfigSource::Python: sourceStr = "Python"; break;
        }
        sources.push_back(sourceStr);
    }
    return sources;
}

bool Config::isJSONFile(const std::string& filepath) {
    std::string extension = filepath.substr(filepath.find_last_of('.') + 1);
    return extension == "json" || extension == "JSON";
}

bool Config::isYAMLError(const std::string& filepath) {
    std::string extension = filepath.substr(filepath.find_last_of('.') + 1);
    return extension == "yaml" || extension == "yml" || extension == "YAML" || extension == "YML";
}

// Helper function implementations
std::string Config::getValueAsString(const ConfigValue& value) {
    std::ostringstream oss;
    std::visit([&oss](auto&& arg) {
        using T = std::decay_t<decltype(arg)>;
        if constexpr (std::is_same_v<T, std::string>) {
            oss << "\"" << arg << "\"";
        } else if constexpr (std::is_same_v<T, bool>) {
            oss << (arg ? "true" : "false");
        } else {
            oss << arg;
        }
    }, value);
    return oss.str();
}

bool Config::typeMatches(const ConfigValue& value, const std::type_info& type) {
    return std::any_of(typeList.begin(), typeList.end(),
        [&value, &type](const std::type_info* t) {
            return *t == type;
        });
}

nlohmann::json Config::toJSON() const {
    nlohmann::json json;
    
    for (const auto& entry : pImpl->entries) {
        std::string key = entry.key;
        std::replace(key.begin(), key.end(), '.', '_');  // JSON keys typically use underscores
        
        json[key] = configValueToJSON(entry.value);
    }
    
    return json;
}

void Config::toYAML(YAML::Emitter& emitter) const {
    emitter << YAML::BeginMap;
    
    for (const auto& entry : pImpl->entries) {
        emitter << YAML::Key << entry.key;
        emitter << configValueToYAML(entry.value);
    }
    
    emitter << YAML::EndMap;
}

ConfigValue Config::jsonToConfigValue(const nlohmann::json& json) {
    if (json.is_number_integer()) {
        return json.get<int>();
    } else if (json.is_number_unsigned()) {
        return json.get<int64_t>();
    } else if (json.is_number()) {
        return json.get<double>();
    } else if (json.is_boolean()) {
        return json.get<bool>();
    } else if (json.is_string()) {
        return json.get<std::string>();
    } else if (json.is_array()) {
        if (json.empty()) {
            return std::vector<int>();
        }
        
        if (json[0].is_number_integer()) {
            std::vector<int> vec;
            for (const auto& item : json) {
                vec.push_back(item.get<int>());
            }
            return vec;
        } else if (json[0].is_number()) {
            std::vector<double> vec;
            for (const auto& item : json) {
                vec.push_back(item.get<double>());
            }
            return vec;
        } else if (json[0].is_boolean()) {
            std::vector<bool> vec;
            for (const auto& item : json) {
                vec.push_back(item.get<bool>());
            }
            return vec;
        } else {
            std::vector<std::string> vec;
            for (const auto& item : json) {
                vec.push_back(item.get<std::string>());
            }
            return vec;
        }
    }
    
    return std::string(); // Default to empty string
}

nlohmann::json Config::configValueToJSON(const ConfigValue& value) {
    return std::visit([](auto&& arg) -> nlohmann::json {
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
            return arg;
        } else if constexpr (std::is_same_v<T, std::vector<double>>) {
            return arg;
        } else if constexpr (std::is_same_v<T, std::vector<bool>>) {
            return arg;
        } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
            return arg;
        }
        return nlohmann::json();
    }, value);
}

YAML::Node Config::configValueToYAML(const ConfigValue& value) {
    YAML::Node node;
    
    std::visit([&node](auto&& arg) {
        using T = std::decay_t<decltype(arg)>;
        if constexpr (std::is_same_v<T, int>) {
            node = arg;
        } else if constexpr (std::is_same_v<T, int64_t>) {
            node = arg;
        } else if constexpr (std::is_same_v<T, double>) {
            node = arg;
        } else if constexpr (std::is_same_v<T, bool>) {
            node = arg;
        } else if constexpr (std::is_same_v<T, std::string>) {
            node = arg;
        } else if constexpr (std::is_same_v<T, std::vector<int>>) {
            for (const auto& item : arg) {
                node.push_back(item);
            }
        } else if constexpr (std::is_same_v<T, std::vector<double>>) {
            for (const auto& item : arg) {
                node.push_back(item);
            }
        } else if constexpr (std::is_same_v<T, std::vector<bool>>) {
            for (const auto& item : arg) {
                node.push_back(item);
            }
        } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
            for (const auto& item : arg) {
                node.push_back(item);
            }
        }
    }, value);
    
    return node;
}

bool Config::setFromEnvironmentString(const std::string& key, const std::string& valueStr) {
    // Simple parsing for environment variable values
    ConfigValue configValue;
    
    // Try to parse as boolean
    if (valueStr == "true" || valueStr == "1" || valueStr == "yes") {
        configValue = true;
    } else if (valueStr == "false" || valueStr == "0" || valueStr == "no") {
        configValue = false;
    } else if (valueStr.find('.') != std::string::npos) {
        // Try to parse as double
        try {
            configValue = std::stod(valueStr);
        } catch (...) {
            // Fall back to string
            configValue = valueStr;
        }
    } else {
        // Try to parse as int
        try {
            configValue = std::stoll(valueStr);
        } catch (...) {
            // Fall back to string
            configValue = valueStr;
        }
    }
    
    set(key, configValue, ConfigSource::Environment);
    return true;
}

bool Config::setFromPythonString(const std::string& key, const std::string& valueStr) {
    // Similar to environment variable parsing but with Python-specific types
    return setFromEnvironmentString(key, valueStr);
}

bool Config::validateRange(const ConfigValue& value, double min, double max) {
    return std::visit([min, max](auto&& arg) -> bool {
        using T = std::decay_t<decltype(arg)>;
        if constexpr (std::is_same_v<T, int> || std::is_same_v<T, int64_t> ||
                     std::is_same_v<T, double>) {
            return static_cast<double>(arg) >= min && static_cast<double>(arg) <= max;
        }
        return false; // Non-numeric types cannot be range-validated
    }, value);
}

bool Config::validateEnum(const ConfigValue& value, const std::vector<std::string>& allowedValues) {
    return std::visit([&allowedValues](auto&& arg) -> bool {
        using T = std::decay_t<decltype(arg)>;
        if constexpr (std::is_same_v<T, std::string>) {
            return std::find(allowedValues.begin(), allowedValues.end(), arg) != allowedValues.end();
        }
        return false; // Non-string types cannot be enum-validated
    }, value);
}

bool Config::validatePattern(const ConfigValue& value, const std::string& pattern) {
    return std::visit([&pattern](auto&& arg) -> bool {
        using T = std::decay_t<decltype(arg)>;
        if constexpr (std::is_same_v<T, std::string>) {
            std::regex regex(pattern);
            return std::regex_match(arg, regex);
        }
        return false; // Non-string types cannot be pattern-validated
    }, value);
}

} // namespace nlm
