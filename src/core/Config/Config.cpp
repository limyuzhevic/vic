#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <string>
#include <memory>
#include <map>

// JSON and YAML parsing libraries
#include <nlohmann/json.hpp>
#include <yaml-cpp/yaml.h>

namespace nlm {

// Profile storage for snapshot/restore
std::map<std::string, nlm::Config> Config::profileStore;

// Helper functions
namespace {

static ConfigValue jsonToConfigValue(const nlohmann::json& j) {
    if (j.is_null()) {
        return std::string("");
    } else if (j.is_boolean()) {
        return j.get<bool>();
    } else if (j.is_number_integer()) {
        return j.get<int>();
    } else if (j.is_number_unsigned()) {
        return j.get<uint64_t>();
    } else if (j.is_number()) {
        return j.get<double>();
    } else if (j.is_string()) {
        return j.get<std::string>();
    } else if (j.is_array()) {
        // Determine vector type based on first element
        if (!j.empty()) {
            const auto& firstElement = j[0];
            if (firstElement.is_number_integer()) {
                std::vector<int> vec = j.get<std::vector<int>>();
                return vec;
            } else if (firstElement.is_number()) {
                std::vector<double> vec = j.get<std::vector<double>>();
                return vec;
            } else if (firstElement.is_string()) {
                std::vector<std::string> vec = j.get<std::vector<std::string>>();
                return vec;
            } else if (firstElement.is_boolean()) {
                std::vector<bool> vec = j.get<std::vector<bool>>();
                return vec;
            }
        }
        // Fallback to string representation
        return j.dump();
    } else if (j.is_object()) {
        // For nested objects, convert to JSON string
        return j.dump();
    } else {
        return j.dump();
    }
}

static nlohmann::json configValueToJson(const ConfigValue& value) {
    nlohmann::json j;
    std::visit([&](auto&& arg) {
        using T = std::decay_t<decltype(arg)>;
        if constexpr (std::is_same_v<T, int>) {
            j = arg;
        } else if constexpr (std::is_same_v<T, int64_t>) {
            j = arg;
        } else if constexpr (std::is_same_v<T, uint64_t>) {
            j = arg;
        } else if constexpr (std::is_same_v<T, double>) {
            j = arg;
        } else if constexpr (std::is_same_v<T, bool>) {
            j = arg;
        } else if constexpr (std::is_same_v<T, std::string>) {
            j = arg;
        } else if constexpr (std::is_same_v<T, std::vector<int>>) {
            j = arg;
        } else if constexpr (std::is_same_v<T, std::vector<double>>) {
            j = arg;
        } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
            j = arg;
        } else {
            j = std::string("");
        }
    }, value);
    return j;
}

static ConfigValue yamlToConfigValue(const YAML::Node& node) {
    if (!node) {
        return std::string("");
    } else if (node.IsScalar()) {
        const std::string& value = node.Scalar();
        
        // Try to determine type
        if (value == "true" || value == "false") {
            return value == "true";
        } else if (value.find('.') != std::string::npos) {
            try {
                return std::stod(value);
            } catch (...) {
                return value;
            }
        } else {
            try {
                return std::stoll(value);
            } catch (...) {
                return value;
            }
        }
    } else if (node.IsSequence()) {
        // Determine vector type based on first element
        if (!node.empty()) {
            const YAML::Node& firstElement = node[0];
            if (firstElement.IsScalar()) {
                const std::string& value = firstElement.Scalar();
                if (value == "true" || value == "false") {
                    std::vector<bool> vec;
                    for (const auto& elem : node) {
                        vec.push_back(elem.as<bool>());
                    }
                    return vec;
                } else if (value.find('.') != std::string::npos) {
                    try {
                        std::vector<double> vec;
                        for (const auto& elem : node) {
                            vec.push_back(elem.as<double>());
                        }
                        return vec;
                    } catch (...) {
                        std::vector<std::string> vec;
                        for (const auto& elem : node) {
                            vec.push_back(elem.as<std::string>());
                        }
                        return vec;
                    }
                } else {
                    try {
                        std::vector<int64_t> vec;
                        for (const auto& elem : node) {
                            vec.push_back(elem.as<int64_t>());
                        }
                        return vec;
                    } catch (...) {
                        std::vector<std::string> vec;
                        for (const auto& elem : node) {
                            vec.push_back(elem.as<std::string>());
                        }
                        return vec;
                    }
                }
            }
        }
        // Fallback to string representation
        return node.as<std::string>();
    } else if (node.IsMap()) {
        // For nested objects, convert to YAML string
        std::stringstream stream;
        stream << node;
        return stream.str();
    } else {
        return node.as<std::string>();
    }
}

static YAML::Node configValueToYaml(const ConfigValue& value) {
    YAML::Node node;
    std::visit([&](auto&& arg) {
        using T = std::decay_t<decltype(arg)>;
        if constexpr (std::is_same_v<T, int>) {
            node = arg;
        } else if constexpr (std::is_same_v<T, int64_t>) {
            node = arg;
        } else if constexpr (std::is_same_v<T, uint64_t>) {
            node = arg;
        } else if constexpr (std::is_same_v<T, double>) {
            node = arg;
        } else if constexpr (std::is_same_v<T, bool>) {
            node = arg;
        } else if constexpr (std::is_same_v<T, std::string>) {
            node = arg;
        } else if constexpr (std::is_same_v<T, std::vector<int>>) {
            node = arg;
        } else if constexpr (std::is_same_v<T, std::vector<double>>) {
            node = arg;
        } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
            node = arg;
        } else {
            node = std::string("");
        }
    }, value);
    return node;
}

static bool isJsonFile(const std::string& filepath) {
    const std::string ext = std::filesystem::path(filepath).extension().string();
    std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);
    return ext == ".json" || ext == ".jsn";
}

static bool isYamlFile(const std::string& filepath) {
    const std::string ext = std::filesystem::path(filepath).extension().string();
    std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);
    return ext == ".yaml" || ext == ".yml";
}

static bool validateJsonSchema(const nlohmann::json& jsonConfig) {
    // Basic validation - check for valid types
    for (const auto& [key, value] : jsonConfig.items()) {
        if (key == "_descriptions") continue;  // Skip metadata
        
        if (!value.is_null() && !value.is_boolean() && 
            !value.is_number_integer() && !value.is_number_unsigned() && 
            !value.is_number() && !value.is_string() && 
            !value.is_array() && !value.is_object()) {
            return false;
        }
    }
    return true;
}

static bool validateYamlSchema(const YAML::Node& yamlConfig) {
    // Basic validation - check for valid types
    for (const auto& yamlPair : yamlConfig) {
        std::string key = yamlPair.first.as<std::string>();
        if (key == "_descriptions") continue;  // Skip metadata
        
        const YAML::Node& node = yamlPair.second;
        if (!node.IsNull() && !node.IsScalar() && !node.IsSequence() && !node.IsMap()) {
            return false;
        }
    }
    return true;
}

} // namespace

struct Config::Impl {
    std::vector<ConfigEntry> entries;
    std::string version;
};

Config::Config() : pImpl(std::make_unique<Impl>()) {
    pImpl->version = "1.0.0";
}

Config::~Config() = default;

Config::Config(Config&&) noexcept = default;

Config& Config::operator=(Config&&) noexcept = default;

// File-based load - parse JSON or YAML configuration files
bool Config::loadFromFile(const std::string& filepath) {
    // Determine file type based on extension
    if (!std::filesystem::exists(filepath)) {
        return false;
    }
    
    bool success = false;
    
    if (isJsonFile(filepath)) {
        // Parse JSON file
        std::ifstream file(filepath);
        if (file.is_open()) {
            try {
                nlohmann::json jsonConfig;
                file >> jsonConfig;
                
                // Validate JSON schema
                if (validateJsonSchema(jsonConfig)) {
                    // Convert JSON to ConfigValue
                    for (const auto& [key, value] : jsonConfig.items()) {
                        ConfigValue configValue = jsonToConfigValue(value);
                        // Find description in the JSON object if available
                        std::string description = "";
                        if (jsonConfig.contains("_descriptions") && 
                            jsonConfig["_descriptions"].is_object() && 
                            jsonConfig["_descriptions"].contains(key)) {
                            description = jsonConfig["_descriptions"][key].get<std::string>();
                        }
                        set(key, configValue, ConfigSource::File);
                    }
                    success = true;
                }
            } catch (const std::exception& e) {
                // Ignore parsing errors
            }
        }
    } else if (isYamlFile(filepath)) {
        // Parse YAML file
        try {
            YAML::Node yamlConfig = YAML::LoadFile(filepath);
            
            // Validate YAML structure
            if (validateYamlSchema(yamlConfig)) {
                // Convert YAML to ConfigValue
                for (const auto& yamlPair : yamlConfig) {
                    std::string key = yamlPair.first.as<std::string>();
                    ConfigValue configValue = yamlToConfigValue(yamlPair.second);
                    
                    // Find description in the YAML node if available
                    std::string description = "";
                    if (yamlConfig["_descriptions"] && 
                        yamlConfig["_descriptions"].IsMap() && 
                        yamlConfig["_descriptions"].IsDefined() && 
                        yamlConfig["_descriptions"].IsSequence()) {
                        for (const auto& descNode : yamlConfig["_descriptions"].as<std::vector<YAML::Node>>()) {
                            if (descNode.IsMap() && descNode.Contains(key)) {
                                description = descNode[key].as<std::string>();
                                break;
                            }
                        }
                    }
                    set(key, configValue, ConfigSource::File);
                }
                success = true;
            }
        } catch (const std::exception& e) {
            // Ignore parsing errors
        }
    } else {
        // Fallback to simple key=value format
        std::ifstream file(filepath);
        if (!file.is_open()) {
            return false;
        }
        
        std::string line;
        while (std::getline(file, line)) {
            // Skip empty lines and comments
            line = trim(line);
            if (line.empty() || line[0] == '#' || line[0] == '/') {
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
                
                set(key, value, ConfigSource::File);
            }
        }
        
        success = true;
    }
    
    return success;
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

// Save to file with format support
bool Config::saveToFile(const std::string& filepath) const {
    return saveToFile(filepath, "auto");
}

bool Config::saveToFile(const std::string& filepath, const std::string& format) const {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    // Determine output format
    std::string actualFormat = format;
    if (actualFormat == "auto") {
        actualFormat = isJsonFile(filepath) ? "json" : 
                      isYamlFile(filepath) ? "yaml" : "simple";
    }
    
    bool success = false;
    
    if (actualFormat == "json") {
        // Save as JSON
        nlohmann::json jsonConfig;
        
        // Add descriptions as separate object
        nlohmann::json descriptions;
        for (const auto& entry : pImpl->entries) {
            if (!entry.description.empty()) {
                descriptions[entry.key] = entry.description;
            }
        }
        if (!descriptions.empty()) {
            jsonConfig["_descriptions"] = descriptions;
        }
        
        // Add all configuration values
        for (const auto& entry : pImpl->entries) {
            jsonConfig[entry.key] = configValueToJson(entry.value);
        }
        
        try {
            file << jsonConfig.dump(2);  // Pretty print with indentation
            success = true;
        } catch (const std::exception& e) {
            // Ignore serialization errors
        }
    } else if (actualFormat == "yaml") {
        // Save as YAML
        try {
            // Create root YAML node
            YAML::Emitter yamlConfig;
            
            // Add configuration values
            for (const auto& entry : pImpl->entries) {
                yamlConfig << YAML::Key << entry.key << YAML::Value << configValueToYaml(entry.value);
            }
            
            // Add descriptions if any
            bool hasDescriptions = false;
            for (const auto& entry : pImpl->entries) {
                if (!entry.description.empty()) {
                    hasDescriptions = true;
                    break;
                }
            }
            
            if (hasDescriptions) {
                yamlConfig << YAML::Key << "_descriptions" << YAML::Value;
                yamlConfig << YAML::BeginMap;
                for (const auto& entry : pImpl->entries) {
                    if (!entry.description.empty()) {
                        yamlConfig << YAML::Key << entry.key << YAML::Value << entry.description;
                    }
                }
                yamlConfig << YAML::EndMap;
            }
            
            file << yamlConfig.c_str();
            success = true;
        } catch (const std::exception& e) {
            // Ignore serialization errors
        }
    } else {
        // Simple key=value format (backward compatibility)
        for (const auto& entry : pImpl->entries) {
            file << "# " << entry.description << "\n";
            
            // Format value based on type
            std::string valueStr;
            std::visit([&](auto&& arg) {
                using T = std::decay_t<decltype(arg)>;
                if constexpr (std::is_same_v<T, std::string>) {
                    valueStr = "\"" + arg + "\"";
                } else if constexpr (std::is_same_v<T, bool>) {
                    valueStr = arg ? "true" : "false";
                } else {
                    valueStr = std::to_string(arg);
                }
            }, entry.value);
            
            file << entry.key << " = " << valueStr << "\n";
        }
        
        success = true;
    }
    
    return success;
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

// Configuration validation implementation
bool Config::validate() const {
    for (const auto& entry : pImpl->entries) {
        try {
            // Try to retrieve the value to verify it's accessible
            std::visit([](auto&& arg) { /* Just access the value */ }, entry.value);
        } catch (const std::bad_variant_access&) {
            return false;
        }
    }
    return true;
}

// Profile/scenario management
bool Config::loadProfile(const std::string& profileName) {
    // Check if profile exists
    auto it = profileStore.find(profileName);
    if (it == profileStore.end()) {
        return false;
    }
    
    // Copy profile data to current config
    clear();
    for (const auto& entry : it->second.pImpl->entries) {
        pImpl->entries.push_back(entry);
    }
    pImpl->version = it->second.pImpl->version;
    
    return true;
}

bool Config::saveProfile(const std::string& profileName) const {
    profileStore[profileName] = *this;
    return true;
}

std::vector<std::string> Config::getProfiles() const {
    std::vector<std::string> profiles;
    for (const auto& pair : profileStore) {
        profiles.push_back(pair.first);
    }
    return profiles;
}

std::string Config::summary() const {
    std::ostringstream oss;
    oss << "Configuration (version " << pImpl->version << ", " << pImpl->entries.size() << " entries):\n";
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
