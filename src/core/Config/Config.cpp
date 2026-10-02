#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <nlohmann/json.hpp>
#include <yaml-cpp/yaml.h>
#include <iostream>

namespace nlm {

// Forward declaration for JSON parsing helper
nlohmann::json configValueToJson(const ConfigValue& value);
ConfigValue jsonToConfigValue(const nlohmann::json& jsonValue);

struct Config::Impl {
    std::vector<ConfigEntry> entries;
    
    // Advanced features
    bool validationEnabled = true;
    bool caseInsensitive = false;
    std::string defaultFileFormat = "json";  // json, yaml, or ini
    
    // JSON schema support
    std::string schemaFile;
    bool strictValidation = false;
    
    // Configuration history
    std::vector<std::pair<std::string, ConfigEntry>> history;
    size_t maxHistorySize = 1000;
    
    // Performance optimizations
    mutable std::unordered_map<std::string, std::optional<ConfigValue>> cache;
};

// Helper function to convert ConfigValue to JSON
nlohmann::json configValueToJson(const ConfigValue& value) {
    nlohmann::json jsonValue;
    
    std::visit([&](auto&& arg) {
        using T = std::decay_t<decltype(arg)>;
        if constexpr (std::is_same_v<T, int>) {
            jsonValue = arg;
        } else if constexpr (std::is_same_v<T, int64_t>) {
            jsonValue = arg;
        } else if constexpr (std::is_same_v<T, double>) {
            jsonValue = arg;
        } else if constexpr (std::is_same_v<T, bool>) {
            jsonValue = arg;
        } else if constexpr (std::is_same_v<T, std::string>) {
            jsonValue = arg;
        } else if constexpr (std::is_same_v<T, std::vector<int>>) {
            jsonValue = nlohmann::json::array();
            for (const auto& item : arg) {
                jsonValue.push_back(item);
            }
        } else if constexpr (std::is_same_v<T, std::vector<double>>) {
            jsonValue = nlohmann::json::array();
            for (const auto& item : arg) {
                jsonValue.push_back(item);
            }
        } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
            jsonValue = nlohmann::json::array();
            for (const auto& item : arg) {
                jsonValue.push_back(item);
            }
        }
    }, value);
    
    return jsonValue;
}

// Helper function to convert JSON to ConfigValue
ConfigValue jsonToConfigValue(const nlohmann::json& jsonValue) {
    ConfigValue configValue;
    
    if (jsonValue.is_null()) {
        configValue = std::string("");
    } else if (jsonValue.is_boolean()) {
        configValue = jsonValue.get<bool>();
    } else if (jsonValue.is_number_integer()) {
        configValue = jsonValue.get<int>();
    } else if (jsonValue.is_number_unsigned()) {
        configValue = jsonValue.get<int64_t>();
    } else if (jsonValue.is_number_float()) {
        configValue = jsonValue.get<double>();
    } else if (jsonValue.is_string()) {
        configValue = jsonValue.get<std::string>();
    } else if (jsonValue.is_array()) {
        // Try to infer array type
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
            } else if (jsonValue[0].is_string()) {
                std::vector<std::string> stringArray;
                for (const auto& item : jsonValue) {
                    stringArray.push_back(item.get<std::string>());
                }
                configValue = stringArray;
            }
        }
        // If we couldn't determine the type, default to string array
        if (!std::holds_alternative<std::vector<std::string>>(configValue)) {
            std::vector<std::string> stringArray;
            for (const auto& item : jsonValue) {
                stringArray.push_back(item.get<std::string>());
            }
            configValue = stringArray;
        }
    }
    
    return configValue;
}

Config::Config() : pImpl(std::make_unique<Impl>()) {
    // Set default configuration format
    pImpl->defaultFileFormat = "json";
    pImpl->validationEnabled = true;
    pImpl->caseInsensitive = false;
    pImpl->strictValidation = false;
}

Config::~Config() = default;

Config::Config(Config&&) noexcept = default;
Config& Config::operator=(Config&&) noexcept = default;

bool Config::loadFromFile(const std::string& filepath) {
    std::string ext = std::filesystem::path(filepath).extension().string();
    std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);
    
    if (ext == ".json" || pImpl->defaultFileFormat == "json") {
        return loadFromJson(filepath);
    } else if (ext == ".yaml" || ext == ".yml" || pImpl->defaultFileFormat == "yaml") {
        return loadFromYaml(filepath);
    } else {
        // Fallback to simple key=value format
        return loadFromSimpleFile(filepath);
    }
}

bool Config::loadFromJson(const std::string& filepath) {
    try {
        std::ifstream file(filepath);
        if (!file.is_open()) {
            NLM_LOG_ERROR("Failed to open JSON config file: " + filepath);
            return false;
        }
        
        nlohmann::json jsonData;
        file >> jsonData;
        
        // Clear existing entries
        pImpl->entries.clear();
        
        // Parse JSON and add entries
        if (jsonData.is_object()) {
            for (auto it = jsonData.begin(); it != jsonData.end(); ++it) {
                std::string key = it.key();
                std::string description = "";
                
                // Add description if available
                if (jsonData[key].contains("_description")) {
                    description = jsonData[key]["_description"].get<std::string>();
                }
                
                ConfigValue value = jsonToConfigValue(it.value());
                set(key, value, ConfigSource::File);
            }
        }
        
        NLM_LOG_INFO("Successfully loaded JSON configuration from: " + filepath);
        return true;
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Error parsing JSON config file " + filepath + ": " + std::string(e.what()));
        return false;
    }
}

bool Config::loadFromYaml(const std::string& filepath) {
    try {
        YAML::Loader loader;
        YAML::Node yamlData = loader.GetFile(filepath);
        
        if (!yamlData) {
            NLM_LOG_ERROR("Failed to parse YAML config file: " + filepath);
            return false;
        }
        
        // Clear existing entries
        pImpl->entries.clear();
        
        // Parse YAML and add entries
        if (yamlData.IsMap()) {
            for (const auto& pair : yamlData) {
                std::string key = pair.first.as<std::string>();
                std::string description = "";
                
                // Add description if available
                if (pair.second["_description"]) {
                    description = pair.second["_description"].as<std::string>();
                }
                
                // Convert YAML value to ConfigValue
                ConfigValue value;
                if (pair.second.IsScalar()) {
                    if (pair.second.IsBoolean()) {
                        value = pair.second.as<bool>();
                    } else if (pair.second.IsInteger()) {
                        value = pair.second.as<int>();
                    } else if (pair.second.IsFloat()) {
                        value = pair.second.as<double>();
                    } else {
                        value = pair.second.as<std::string>();
                    }
                } else if (pair.second.IsSequence()) {
                    // Convert sequence to appropriate array type
                    if (!pair.second.empty()) {
                        if (pair.second[0].IsBoolean()) {
                            std::vector<bool> boolArray;
                            for (const auto& item : pair.second) {
                                boolArray.push_back(item.as<bool>());
                            }
                            value = boolArray;
                        } else if (pair.second[0].IsInteger()) {
                            std::vector<int> intArray;
                            for (const auto& item : pair.second) {
                                intArray.push_back(item.as<int>());
                            }
                            value = intArray;
                        } else if (pair.second[0].IsFloat()) {
                            std::vector<double> doubleArray;
                            for (const auto& item : pair.second) {
                                doubleArray.push_back(item.as<double>());
                            }
                            value = doubleArray;
                        } else {
                            std::vector<std::string> stringArray;
                            for (const auto& item : pair.second) {
                                stringArray.push_back(item.as<std::string>());
                            }
                            value = stringArray;
                        }
                    }
                }
                
                set(key, value, ConfigSource::File);
            }
        }
        
        NLM_LOG_INFO("Successfully loaded YAML configuration from: " + filepath);
        return true;
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Error parsing YAML config file " + filepath + ": " + std::string(e.what()));
        return false;
    }
}

bool Config::loadFromSimpleFile(const std::string& filepath) {
    // Original simple key=value format parsing
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
    std::string ext = std::filesystem::path(filepath).extension().string();
    std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);
    
    if (ext == ".json" || pImpl->defaultFileFormat == "json") {
        return saveToJson(filepath);
    } else if (ext == ".yaml" || ext == ".yml" || pImpl->defaultFileFormat == "yaml") {
        return saveToYaml(filepath);
    } else {
        // Fallback to simple key=value format
        return saveToSimpleFile(filepath);
    }
}

bool Config::saveToJson(const std::string& filepath) const {
    try {
        nlohmann::json jsonData;
        
        for (const auto& entry : pImpl->entries) {
            nlohmann::json jsonValue = configValueToJson(entry.value);
            
            // Add metadata for special keys
            if (entry.description.empty() && entry.source != ConfigSource::Default) {
                jsonData[entry.key] = jsonValue;
            } else {
                // Use object format with metadata
                jsonData[entry.key] = {
                    {"_description", entry.description},
                    {"_source", static_cast<int>(entry.source)}
                };
                if (jsonValue.is_object()) {
                    jsonData[entry.key].update(jsonValue);
                } else {
                    jsonData[entry.key]["_value"] = jsonValue;
                }
            }
        }
        
        std::ofstream file(filepath);
        if (!file.is_open()) {
            NLM_LOG_ERROR("Failed to create JSON config file: " + filepath);
            return false;
        }
        
        file << std::setw(2) << jsonData;
        NLM_LOG_INFO("Successfully saved JSON configuration to: " + filepath);
        return true;
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Error saving JSON config file " + filepath + ": " + std::string(e.what()));
        return false;
    }
}

bool Config::saveToYaml(const std::string& filepath) const {
    try {
        YAML::Emitter emitter;
        emitter << YAML::BeginMap;
        
        for (const auto& entry : pImpl->entries) {
            emitter << YAML::Key << entry.key << YAML::Value;
            
            // Handle different value types
            std::visit([&](auto&& arg) {
                using T = std::decay_t<decltype(arg)>;
                if constexpr (std::is_same_v<T, int>) {
                    emitter << arg;
                } else if constexpr (std::is_same_v<T, int64_t>) {
                    emitter << static_cast<int>(arg);
                } else if constexpr (std::is_same_v<T, double>) {
                    emitter << arg;
                } else if constexpr (std::is_same_v<T, bool>) {
                    emitter << arg;
                } else if constexpr (std::is_same_v<T, std::string>) {
                    emitter << arg;
                } else if constexpr (std::is_same_v<T, std::vector<int>>) {
                    emitter << YAML::BeginSeq;
                    for (const auto& item : arg) {
                        emitter << item;
                    }
                    emitter << YAML::EndSeq;
                } else if constexpr (std::is_same_v<T, std::vector<double>>) {
                    emitter << YAML::BeginSeq;
                    for (const auto& item : arg) {
                        emitter << item;
                    }
                    emitter << YAML::EndSeq;
                } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
                    emitter << YAML::BeginSeq;
                    for (const auto& item : arg) {
                        emitter << item;
                    }
                    emitter << YAML::EndSeq;
                }
            }, entry.value);
        }
        
        emitter << YAML::EndMap;
        
        std::ofstream file(filepath);
        if (!file.is_open()) {
            NLM_LOG_ERROR("Failed to create YAML config file: " + filepath);
            return false;
        }
        
        file << emitter.c_str();
        NLM_LOG_INFO("Successfully saved YAML configuration to: " + filepath);
        return true;
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Error saving YAML config file " + filepath + ": " + std::string(e.what()));
        return false;
    }
}

bool Config::saveToSimpleFile(const std::string& filepath) const {
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
    std::string lookupKey = key;
    if (pImpl->caseInsensitive) {
        lookupKey = toLower(key);
    }
    
    // Check cache first
    auto cacheIt = pImpl->cache.find(lookupKey);
    if (cacheIt != pImpl->cache.end()) {
        if (cacheIt->second.has_value()) {
            try {
                return std::get<T>(cacheIt->second.value());
            } catch (const std::bad_variant_access&) {}
        }
        return std::nullopt;
    }
    
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&lookupKey](const ConfigEntry& e) { 
            std::string eKey = e.key;
            if (pImpl->caseInsensitive) {
                eKey = toLower(eKey);
            }
            return eKey == lookupKey; 
        });
    
    if (it == pImpl->entries.end()) {
        pImpl->cache[lookupKey] = std::nullopt;
        return std::nullopt;
    }
    
    try {
        T value = std::get<T>(it->value);
        pImpl->cache[lookupKey] = value;
        return value;
    } catch (const std::bad_variant_access&) {
        pImpl->cache[lookupKey] = std::nullopt;
        return std::nullopt;
    }
}

template<typename T>
T Config::getOr(const std::string& key, const T& defaultValue) const {
    auto val = get<T>(key);
    return val.has_value() ? val.value() : defaultValue;
}

void Config::set(const std::string& key, const ConfigValue& value, ConfigSource source) {
    std::string lookupKey = key;
    if (pImpl->caseInsensitive) {
        lookupKey = toLower(key);
    }
    
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&lookupKey](const ConfigEntry& e) {
            std::string eKey = e.key;
            if (pImpl->caseInsensitive) {
                eKey = toLower(eKey);
            }
            return eKey == lookupKey;
        });
    
    if (it != pImpl->entries.end()) {
        // Update existing entry
        it->value = value;
        it->source = source;
        
        // Update cache
        pImpl->cache[lookupKey] = value;
        
        // Add to history if not too large
        if (pImpl->history.size() < pImpl->maxHistorySize) {
            pImpl->history.emplace_back(key, *it);
        }
    } else {
        // Add new entry
        pImpl->entries.emplace_back(key, value, source);
        
        // Update cache
        pImpl->cache[lookupKey] = value;
        
        // Add to history if not too large
        if (pImpl->history.size() < pImpl->maxHistorySize) {
            pImpl->history.emplace_back(key, pImpl->entries.back());
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
    std::string lookupKey = key;
    if (pImpl->caseInsensitive) {
        lookupKey = toLower(key);
    }
    
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&lookupKey](const ConfigEntry& e) {
            std::string eKey = e.key;
            if (pImpl->caseInsensitive) {
                eKey = toLower(eKey);
            }
            return eKey == lookupKey;
        });
    
    return it != pImpl->entries.end();
}

void Config::remove(const std::string& key) {
    std::string lookupKey = key;
    if (pImpl->caseInsensitive) {
        lookupKey = toLower(key);
    }
    
    pImpl->entries.erase(
        std::remove_if(pImpl->entries.begin(),
                      pImpl->entries.end(),
                      [&lookupKey](const ConfigEntry& e) {
                          std::string eKey = e.key;
                          if (pImpl->caseInsensitive) {
                              eKey = toLower(eKey);
                          }
                          return eKey == lookupKey;
                      }),
        pImpl->entries.end()
    );
    
    // Clear cache for this key
    pImpl->cache.erase(lookupKey);
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
    pImpl->cache.clear();
    pImpl->history.clear();
}

std::string Config::summary() const {
    std::ostringstream oss;
    oss << "Advanced Configuration (" << pImpl->entries.size() << " entries):\n";
    
    // Configuration metadata
    oss << "Metadata:\n";
    oss << "  File format: " << pImpl->defaultFileFormat << "\n";
    oss << "  Case insensitive: " << (pImpl->caseInsensitive ? "enabled" : "disabled") << "\n";
    oss << "  Validation enabled: " << (pImpl->validationEnabled ? "enabled" : "disabled") << "\n";
    oss << "  Strict validation: " << (pImpl->strictValidation ? "enabled" : "disabled") << "\n";
    oss << "  Cache entries: " << pImpl->cache.size() << "\n";
    oss << "  History entries: " << pImpl->history.size() << "\n\n";
    
    // Configuration entries
    oss << "Configuration Entries:\n";
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
        oss << "] (" << static_cast<int>(entry.source) << ") - " << entry.description << "\n";
    }
    
    return oss.str();
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

// Add helper functions for JSON and YAML library includes
namespace nlohmann {
    // Helper to convert ConfigValue to JSON
    void to_json(nlohmann::json& j, const nlm::ConfigEntry& entry) {
        j = nlohmann::json{
            {"key", entry.key},
            {"value", nlm::configValueToJson(entry.value)},
            {"source", static_cast<int>(entry.source)},
            {"description", entry.description}
        };
    }
    
    // Helper to convert JSON to ConfigEntry
    void from_json(const nlohmann::json& j, nlm::ConfigEntry& entry) {
        j.at("key").get_to(entry.key);
        entry.value = nlm::jsonToConfigValue(j.at("value"));
        j.at("source").get_to(entry.source);
        if (j.contains("description")) {
            j.at("description").get_to(entry.description);
        }
    }
}