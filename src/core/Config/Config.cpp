// src/core/Config/Config.cpp
#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <nlohmann/json.hpp>
#include <unordered_map>

namespace nlm {

struct Config::Impl {
    std::vector<ConfigEntry> entries;
    std::unordered_map<std::string, nlohmann::json> nestedConfig;
};

Config::Config() : pImpl(std::make_unique<Impl>()) {}

Config::~Config() = default;

Config::Config(Config&&) noexcept = default;

Config& Config::operator=(Config&&) noexcept = default;

bool Config::loadFromFile(const std::string& filepath, bool validate) {
    nlohmann::json config;
    if (!parseConfigFile(filepath, config)) {
        return false;
    }
    
    // Convert from JSON
    fromJSON(config);
    
    if (validate) {
        std::string error;
        if (!validator.validate(config)) {
            validator.getValidationReport(config); // This includes error details
            return false;
        }
    }
    
    return true;
}

bool Config::loadFromJSON(const std::string& jsonString, bool validate) {
    nlohmann::json config;
    try {
        config = nlohmann::json::parse(jsonString);
    } catch (const nlohmann::json::parse_error& e) {
        NLM_LOG_ERROR("JSON parse error: " + std::string(e.what()));
        return false;
    }
    
    fromJSON(config);
    
    if (validate) {
        std::string error;
        if (!validator.validate(config)) {
            validator.getValidationReport(config);
            return false;
        }
    }
    
    return true;
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
                
                // Try to parse value based on key prefixes
                bool parsed = false;
                if (key.find("int64") != std::string::npos) {
                    try { set(key, std::stoll(value), ConfigSource::CommandLine); parsed = true; }
                    catch (...) {}
                } else if (key.find("float") != std::string::npos || key.find("double") != std::string::npos || key.find("rate") != std::string::npos) {
                    try { set(key, std::stod(value), ConfigSource::CommandLine); parsed = true; }
                    catch (...) {}
                } else if (key.find("bool") != std::string::npos) {
                    if (value == "true" || value == "1") set(key, true, ConfigSource::CommandLine);
                    else if (value == "false" || value == "0") set(key, false, ConfigSource::CommandLine);
                    parsed = true;
                }
                
                if (!parsed) {
                    set(key, value, ConfigSource::CommandLine);
                }
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
    nlohmann::json config;
    toJSON(config);
    
    std::ofstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    file << config.dump(4) << std::endl;
    
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

std::optional<nlohmann::json> Config::getNested(const std::string& section, const std::string& key) const {
    std::string fullKey = section + "." + key;
    auto it = pImpl->entries.begin();
    while (it != pImpl->entries.end()) {
        if (it->key == fullKey) {
            return configValueToJSON(it->value);
        }
        ++it;
    }
    return std::nullopt;
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

void Config::setNested(const std::string& section, const std::string& key, const nlohmann::json& value) {
    std::string fullKey = section + "." + key;
    
    if (pImpl->nestedConfig.find(section) == pImpl->nestedConfig.end()) {
        pImpl->nestedConfig[section] = nlohmann::json::object();
    }
    
    pImpl->nestedConfig[section][key] = value;
}

bool Config::has(const std::string& key) const {
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    if (it != pImpl->entries.end()) {
        return true;
    }
    
    // Check nested config
    size_t dotPos = key.find('.');
    if (dotPos != std::string::npos) {
        std::string section = key.substr(0, dotPos);
        std::string nestedKey = key.substr(dotPos + 1);
        auto sectionIt = pImpl->nestedConfig.find(section);
        if (sectionIt != pImpl->nestedConfig.end()) {
            return sectionIt->second.contains(nestedKey);
        }
    }
    
    return false;
}

void Config::remove(const std::string& key) {
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    if (it != pImpl->entries.end()) {
        pImpl->entries.erase(it);
    }
    
    // Also remove from nested config
    size_t dotPos = key.find('.');
    if (dotPos != std::string::npos) {
        std::string section = key.substr(0, dotPos);
        std::string nestedKey = key.substr(dotPos + 1);
        auto sectionIt = pImpl->nestedConfig.find(section);
        if (sectionIt != pImpl->nestedConfig.end()) {
            sectionIt->second.erase(nestedKey);
            if (sectionIt->second.empty()) {
                pImpl->nestedConfig.erase(sectionIt);
            }
        }
    }
}

std::vector<std::string> Config::getKeys() const {
    std::vector<std::string> keys;
    keys.reserve(pImpl->entries.size() + 10000); // Estimate for nested keys
    for (const auto& entry : pImpl->entries) {
        keys.push_back(entry.key);
    }
    
    // Add nested keys
    for (const auto& [section, config] : pImpl->nestedConfig) {
        for (const auto& [key, _] : config.items()) {
            keys.push_back(section + "." + key);
        }
    }
    
    return keys;
}

std::vector<std::string> Config::getKeys(const std::string& section) const {
    std::vector<std::string> keys;
    
    // Get flat keys in this section
    for (const auto& entry : pImpl->entries) {
        size_t dotPos = entry.key.find('.');
        if (dotPos != std::string::npos) {
            std::string keySection = entry.key.substr(0, dotPos);
            if (keySection == section) {
                keys.push_back(entry.key);
            }
        }
    }
    
    // Get nested keys
    auto sectionIt = pImpl->nestedConfig.find(section);
    if (sectionIt != pImpl->nestedConfig.end()) {
        for (const auto& [key, _] : sectionIt->second.items()) {
            keys.push_back(section + "." + key);
        }
    }
    
    return keys;
}

void Config::clear() {
    pImpl->entries.clear();
    pImpl->nestedConfig.clear();
}

std::string Config::summary() const {
    std::ostringstream oss;
    oss << "Enhanced Configuration:\n";
    oss << "  Flat entries: " << pImpl->entries.size() << "\n";
    oss << "  Nested sections: " << pImpl->nestedConfig.size() << "\n";
    oss << "  Total keys: " << getKeys().size() << "\n\n";
    
    oss << "Flat entries:\n";
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
    
    if (!pImpl->nestedConfig.empty()) {
        oss << "\nNested sections:\n";
        for (const auto& [section, config] : pImpl->nestedConfig) {
            oss << "  Section: " << section << "\n";
            for (const auto& [key, value] : config.items()) {
                oss << "    " << key << " = " << value.dump() << "\n";
            }
        }
    }
    
    return oss.str();
}

std::string Config::getValidationReport() const {
    nlohmann::json config;
    toJSON(config);
    return validator.getValidationReport(config);
}

bool Config::validate(std::string& errorMessage) const {
    nlohmann::json config;
    toJSON(config);
    
    auto result = validator.validate(config);
    errorMessage = result.errorMessage;
    return result.success;
}

void Config::mergeConfig(const Config& other) {
    // Merge flat entries
    for (const auto& entry : other.pImpl->entries) {
        bool exists = false;
        for (auto& existingEntry : pImpl->entries) {
            if (existingEntry.key == entry.key) {
                existingEntry.value = entry.value;
                existingEntry.source = entry.source;
                exists = true;
                break;
            }
        }
        if (!exists) {
            pImpl->entries.push_back(entry);
        }
    }
    
    // Merge nested config
    for (const auto& [section, nestedConfig] : other.pImpl->nestedConfig) {
        if (pImpl->nestedConfig.find(section) == pImpl->nestedConfig.end()) {
            pImpl->nestedConfig[section] = nestedConfig;
        } else {
            for (const auto& [key, value] : nestedConfig.items()) {
                pImpl->nestedConfig[section][key] = value;
            }
        }
    }
}

void Config::patchConfig(const nlohmann::json& patch) {
    for (auto it = patch.begin(); it != patch.end(); ++it) {
        std::string key = it.key();
        const nlohmann::json& value = it.value();
        
        // Check if it's a nested key
        size_t dotPos = key.find('.');
        if (dotPos != std::string::npos) {
            std::string section = key.substr(0, dotPos);
            std::string nestedKey = key.substr(dotPos + 1);
            
            if (pImpl->nestedConfig.find(section) == pImpl->nestedConfig.end()) {
                pImpl->nestedConfig[section] = nlohmann::json::object();
            }
            
            pImpl->nestedConfig[section][nestedKey] = value;
        } else {
            // Try to convert JSON value to ConfigValue
            auto configValue = jsonToConfigValue(value);
            if (configValue) {
                set(key, *configValue, ConfigSource::Runtime);
            }
        }
    }
}

// Static helper methods
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

nlohmann::json Config::configValueToJSON(const ConfigValue& value) {
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

std::optional<ConfigValue> Config::jsonToConfigValue(const nlohmann::json& value) {
    if (value.is_number_integer()) {
        return value.get<int>();
    } else if (value.is_number()) {
        return value.get<double>();
    } else if (value.is_boolean()) {
        return value.get<bool>();
    } else if (value.is_string()) {
        return value.get<std::string>();
    } else if (value.is_array()) {
        // Determine array type based on elements
        if (!value.empty()) {
            if (value[0].is_number_integer()) {
                std::vector<int> arr = value.get<std::vector<int>>();
                return arr;
            } else if (value[0].is_number()) {
                std::vector<double> arr = value.get<std::vector<double>>();
                return arr;
            } else if (value[0].is_string()) {
                std::vector<std::string> arr = value.get<std::vector<std::string>>();
                return arr;
            }
        }
    }
    return std::nullopt;
}

bool Config::parseConfigFile(const std::string& filepath, nlohmann::json& config) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    // Try to parse as JSON first
    try {
        file >> config;
        return true;
    } catch (...) {
        file.clear();
        file.seekg(0, std::ios::beg);
    }
    
    // Try to parse as simple key=value format
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
            
            if (value.size() >= 2 && 
                ((value.front() == '"' && value.back() == '"') ||
                 (value.front() == '\'' && value.back() == '\''))) {
                value = value.substr(1, value.size() - 2);
            }
            
            config[key] = value;
        }
    }
    
    return true;
}

bool Config::parseConfigString(const std::string& content, nlohmann::json& config) {
    try {
        config = nlohmann::json::parse(content);
        return true;
    } catch (const nlohmann::json::parse_error& e) {
        NLM_LOG_ERROR("JSON parse error: " + std::string(e.what()));
        return false;
    }
}

void Config::fromJSON(const nlohmann::json& config) {
    // Clear existing config
    clear();
    
    // Convert JSON to Config entries
    for (auto it = config.begin(); it != config.end(); ++it) {
        std::string key = it.key();
        const nlohmann::json& value = it.value();
        
        // Check if it's a nested structure (object with section key)
        if (value.is_object()) {
            // This is a section, extract all key-value pairs
            for (auto vit = value.begin(); vit != value.end(); ++vit) {
                std::string nestedKey = key + "." + vit.key();
                set(nestedKey, value[vit.key()].dump(), ConfigSource::File);
            }
        } else {
            // Convert JSON value to ConfigValue
            auto configValue = jsonToConfigValue(value);
            if (configValue) {
                set(key, *configValue, ConfigSource::File);
            }
        }
    }
}

void Config::toJSON(nlohmann::json& config) const {
    // Convert flat entries to JSON
    for (const auto& entry : pImpl->entries) {
        config[entry.key] = configValueToJSON(entry.value);
    }
    
    // Add nested config
    for (const auto& [section, nestedConfig] : pImpl->nestedConfig) {
        for (const auto& [key, value] : nestedConfig.items()) {
            config[section + "." + key] = value;
        }
    }
}

// JSON serialization support
void to_json(nlohmann::json& j, const ConfigEntry& entry) {
    j = nlohmann::json{
        {"key", entry.key},
        {"value", Config::configValueToJSON(entry.value)},
        {"source", static_cast<int>(entry.source)},
        {"description", entry.description}
    };
}

void from_json(const nlohmann::json& j, ConfigEntry& entry) {
    j.at("key").get_to(entry.key);
    entry.value = Config::jsonToConfigValue(j.at("value")).value_or(ConfigValue(""));
    j.at("source").get_to(entry.source);
    if (j.contains("description")) {
        j.at("description").get_to(entry.description);
    }
}

} // namespace nlm