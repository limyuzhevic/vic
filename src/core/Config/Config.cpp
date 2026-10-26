#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <nlohmann/json.hpp>
#include <yaml-cpp/yaml.h>

namespace nlm {

struct Config::Impl {
    std::vector<ConfigEntry> entries;
    std::vector<ConfigEntry> schemaEntries;
    std::vector<std::string> validationErrors;
};

Config::Config() : pImpl(std::make_unique<Impl>()) {}

Config::~Config() = default;

Config::Config(Config&&) noexcept = default;

Config& Config::operator=(Config&&) noexcept = default;

bool Config::loadFromFile(const std::string& filepath) {
    // Try different parsers in order
    std::string extension = "";
    size_t dotPos = filepath.find_last_of('.');
    if (dotPos != std::string::npos) {
        extension = filepath.substr(dotPos);
    }
    
    // JSON first (most common)
    if (extension == ".json" || extension == ".JSON" || filepath.find(".json") != std::string::npos) {
        if (parseJSONFile(filepath, pImpl->entries)) {
            return true;
        }
    }
    
    // YAML second
    if (extension == ".yaml" || extension == ".yml" || extension == ".YAML" || extension == ".YML" ||
        filepath.find(".yaml") != std::string::npos || filepath.find(".yml") != std::string::npos) {
        if (parseYAMLFile(filepath, pImpl->entries)) {
            return true;
        }
    }
    
    // Fallback to simple parser
    return parseSimpleFile(filepath, pImpl->entries);
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

// =============================================================================
// JSON and YAML Parsing Helper Functions
// =============================================================================

// JSON parsing implementation
bool parseJSONFile(const std::string& filepath, std::vector<ConfigEntry>& entries) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    try {
        nlohmann::json jsonConfig;
        file >> jsonConfig;
        
        for (auto it = jsonConfig.begin(); it != jsonConfig.end(); ++it) {
            std::string key = it.key();
            std::string description = "";
            
            // Check if this is a comment object (e.g., "key": {"value": 42, "description": "comment"})
            if (it->is_object() && it->contains("value") && it->contains("description")) {
                auto valueJson = (*it)["value"];
                description = (*it)["description"].get<std::string>();
                
                // Convert JSON value to ConfigValue
                ConfigValue configValue;
                if (valueJson.is_number_integer()) {
                    configValue = valueJson.get<int64_t>();
                } else if (valueJson.is_number_unsigned()) {
                    configValue = valueJson.get<uint64_t>();
                } else if (valueJson.is_number_float()) {
                    configValue = valueJson.get<double>();
                } else if (valueJson.is_boolean()) {
                    configValue = valueJson.get<bool>();
                } else if (valueJson.is_string()) {
                    configValue = valueJson.get<std::string>();
                } else if (valueJson.is_array()) {
                    std::vector<double> vec = valueJson.get<std::vector<double>>();
                    configValue = vec;
                }
                
                entries.emplace_back(key, configValue, ConfigSource::File, description);
            } else {
                // Simple key=value format
                ConfigValue configValue;
                if (it->is_number_integer()) {
                    configValue = it->get<int64_t>();
                } else if (it->is_number_unsigned()) {
                    configValue = it->get<uint64_t>();
                } else if (it->is_number_float()) {
                    configValue = it->get<double>();
                } else if (it->is_boolean()) {
                    configValue = it->get<bool>();
                } else if (it->is_string()) {
                    configValue = it->get<std::string>();
                } else if (it->is_array()) {
                    configValue = it->get<std::vector<double>>();
                }
                
                entries.emplace_back(key, configValue, ConfigSource::File, "");
            }
        }
        
        return true;
    } catch (const std::exception& e) {
        std::cerr << "JSON parsing error: " << e.what() << std::endl;
        return false;
    }
}

// YAML parsing implementation
bool parseYAMLFile(const std::string& filepath, std::vector<ConfigEntry>& entries) {
    try {
        YAML::Loader loader;
        YAML::Node doc = YAML::LoadFile(filepath);
        
        for (const auto& key : doc.keys()) {
            std::string keyName = key.as<std::string>();
            std::string description = "";
            
            // Handle YAML's scalar or mapping format
            if (doc[key].IsMap() && doc[key]["value"] && doc[key]["description"]) {
                auto valueNode = doc[key]["value"];
                description = doc[key]["description"].as<std::string>();
                
                ConfigValue configValue;
                if (valueNode.IsInt()) {
                    configValue = valueNode.as<int64_t>();
                } else if (valueNode.IsFloat()) {
                    configValue = valueNode.as<double>();
                } else if (valueNode.IsBool()) {
                    configValue = valueNode.as<bool>();
                } else if (valueNode.IsScalar()) {
                    configValue = valueNode.as<std::string>();
                } else if (valueNode.IsSequence()) {
                    std::vector<double> vec;
                    for (const auto& item : valueNode) {
                        vec.push_back(item.as<double>());
                    }
                    configValue = vec;
                }
                
                entries.emplace_back(keyName, configValue, ConfigSource::File, description);
            } else {
                // Simple YAML format
                ConfigValue configValue;
                if (doc[key].IsInt()) {
                    configValue = doc[key].as<int64_t>();
                } else if (doc[key].IsFloat()) {
                    configValue = doc[key].as<double>();
                } else if (doc[key].IsBool()) {
                    configValue = doc[key].as<bool>();
                } else if (doc[key].IsScalar()) {
                    configValue = doc[key].as<std::string>();
                } else if (doc[key].IsSequence()) {
                    std::vector<double> vec;
                    for (const auto& item : doc[key]) {
                        vec.push_back(item.as<double>());
                    }
                    configValue = vec;
                }
                
                entries.emplace_back(keyName, configValue, ConfigSource::File, "");
            }
        }
        
        return true;
    } catch (const YAML::Exception& e) {
        std::cerr << "YAML parsing error: " << e.what() << std::endl;
        return false;
    }
}

// Helper functions for type conversion
ConfigValue convertToConfigValue(const rapidjson::Value& value) {
    if (value.IsInt()) {
        return value.GetInt();
    } else if (value.IsInt64()) {
        return value.GetInt64();
    } else if (value.IsUint()) {
        return value.GetUint();
    } else if (value.IsUint64()) {
        return value.GetUint64();
    } else if (value.IsDouble()) {
        return value.GetDouble();
    } else if (value.IsBool()) {
        return value.GetBool();
    } else if (value.IsString()) {
        return value.GetString();
    } else if (value.IsArray()) {
        std::vector<double> vec;
        for (rapidjson::SizeType i = 0; i < value.Size(); ++i) {
            vec.push_back(value[i].GetDouble());
        }
        return vec;
    }
    return std::string("");
}

bool parseRapidJSONFile(const std::string& filepath, std::vector<ConfigEntry>& entries) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    try {
        rapidjson::IStreamWrapper isw(file);
        rapidjson::Document doc;
        doc.ParseStream(isw);
        
        if (doc.HasParseError()) {
            std::cerr << "RapidJSON parsing error at offset " 
                      << doc.GetParseErrorOffset() << ": "
                      << rapidjson::GetParseError_En(doc.GetParseError()) << std::endl;
            return false;
        }
        
        for (rapidjson::Value::ConstMemberIterator it = doc.MemberBegin(); 
             it != doc.MemberEnd(); ++it) {
            std::string key = it->name.GetString();
            std::string description = "";
            
            if (it->value.IsObject() && it->value.FindMember("value") != it->value.MemberEnd() &&
                it->value.FindMember("description") != it->value.MemberEnd()) {
                auto valueObj = it->value["value"];
                description = it->value["description"].GetString();
                ConfigValue configValue = convertToConfigValue(valueObj);
                entries.emplace_back(key, configValue, ConfigSource::File, description);
            } else {
                ConfigValue configValue = convertToConfigValue(it->value);
                entries.emplace_back(key, configValue, ConfigSource::File, "");
            }
        }
        
        return true;
    } catch (const std::exception& e) {
        std::cerr << "RapidJSON parsing error: " << e.what() << std::endl;
        return false;
    }
}

// Simple key=value parser (fallback)
bool parseSimpleFile(const std::string& filepath, std::vector<ConfigEntry>& entries) {
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
            
            if (value.size() >= 2 && 
                ((value.front() == '\"' && value.back() == '\"') ||
                 (value.front() == '\'' && value.back() == '\''))) {
                value = value.substr(1, value.size() - 2);
            }
            
            set(key, value, ConfigSource::File);
        }
    }
    
    return true;
}

// Save as JSON implementation
bool saveAsJSONFile(const std::vector<ConfigEntry>& entries, const std::string& filepath) {
    nlohmann::json jsonConfig;
    
    for (const auto& entry : entries) {
        if (!entry.description.empty()) {
            // Rich entry with description
            nlohmann::json valueJson;
            std::visit([&valueJson](auto&& arg) {
                using T = std::decay_t<decltype(arg)>;
                if constexpr (std::is_same_v<T, int> || std::is_same_v<T, int64_t> || 
                             std::is_same_v<T, uint64_t>) {
                    valueJson = arg;
                } else if constexpr (std::is_same_v<T, double>) {
                    valueJson = arg;
                } else if constexpr (std::is_same_v<T, bool>) {
                    valueJson = arg;
                } else if constexpr (std::is_same_v<T, std::string>) {
                    valueJson = arg;
                } else if constexpr (std::is_same_v<T, std::vector<double>>) {
                    valueJson = arg;
                } else if constexpr (std::is_same_v<T, std::vector<int>>) {
                    std::vector<double> vec(arg.begin(), arg.end());
                    valueJson = vec;
                } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
                    valueJson = arg;
                }
            }, entry.value);
            
            jsonConfig[entry.key] = {
                {"value", valueJson},
                {"description", entry.description},
                {"source", static_cast<int>(entry.source)}
            };
        } else {
            // Simple entry
            nlohmann::json valueJson;
            std::visit([&valueJson](auto&& arg) {
                using T = std::decay_t<decltype(arg)>;
                if constexpr (std::is_same_v<T, int> || std::is_same_v<T, int64_t> || 
                             std::is_same_v<T, uint64_t>) {
                    valueJson = arg;
                } else if constexpr (std::is_same_v<T, double>) {
                    valueJson = arg;
                } else if constexpr (std::is_same_v<T, bool>) {
                    valueJson = arg;
                } else if constexpr (std::is_same_v<T, std::string>) {
                    valueJson = arg;
                } else if constexpr (std::is_same_v<T, std::vector<double>>) {
                    valueJson = arg;
                } else if constexpr (std::is_same_v<T, std::vector<int>>) {
                    std::vector<double> vec(arg.begin(), arg.end());
                    valueJson = vec;
                } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
                    valueJson = arg;
                }
            }, entry.value);
            
            jsonConfig[entry.key] = valueJson;
        }
    }
    
    std::ofstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    file << std::setw(2) << jsonConfig << std::endl;
    return true;
}

// Implementation of all other Config methods remains the same as before

// Config method implementations
bool Config::validate() const {
    validateSchema();
    return pImpl->validationErrors.empty();
}

std::vector<std::string> Config::getValidationErrors() const {
    return pImpl->validationErrors;
}

void Config::addSchemaEntry(const std::string& key, const std::string& type,
                           const ConfigValue& defaultValue, const std::string& description) {
    pImpl->schemaEntries.emplace_back(key, defaultValue, ConfigSource::Default, description);
}

void Config::setSchema(const std::string& key, const std::string& type,
                      const ConfigValue& defaultValue, const std::string& description) {
    pImpl->schemaEntries.emplace_back(key, defaultValue, ConfigSource::Default, description);
}

void Config::batchSet(const std::vector<std::pair<std::string, ConfigValue>>& updates) {
    for (const auto& update : updates) {
        set(update.first, update.second);
    }
}

bool Config::mergeFrom(const Config& other, ConfigSource source) {
    bool changed = false;
    for (const auto& entry : other.pImpl->entries) {
        bool exists = false;
        for (auto& thisEntry : pImpl->entries) {
            if (thisEntry.key == entry.key) {
                thisEntry.value = entry.value;
                thisEntry.source = source;
                exists = true;
                break;
            }
        }
        if (!exists) {
            pImpl->entries.push_back(entry);
            pImpl->entries.back().source = source;
            changed = true;
        }
    }
    return changed;
}

Config Config::filter(const std::vector<std::string>& keys) const {
    Config filteredConfig;
    for (const auto& key : keys) {
        auto value = getAsVariant(key);
        if (value) {
            filteredConfig.set(key, value.value(), ConfigSource::Runtime);
        }
    }
    return filteredConfig;
}

Config Config::transform(const std::function<ConfigValue(ConfigValue)>& transform) const {
    Config transformedConfig;
    for (const auto& entry : pImpl->entries) {
        ConfigValue newValue = transform(entry.value);
        transformedConfig.set(entry.key, newValue, entry.source);
    }
    return transformedConfig;
}

std::optional<ConfigValue> Config::getAsVariant(const std::string& key) const {
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    
    if (it == pImpl->entries.end()) {
        return std::nullopt;
    }
    
    return it->value;
}

// Save as JSON implementation
bool saveAsJSONFile(const std::vector<ConfigEntry>& entries, const std::string& filepath) {
    nlohmann::json jsonConfig;
    
    for (const auto& entry : entries) {
        if (!entry.description.empty()) {
            // Rich entry with description
            nlohmann::json valueJson;
            std::visit([&valueJson](auto&& arg) {
                using T = std::decay_t<decltype(arg)>;
                if constexpr (std::is_same_v<T, int> || std::is_same_v<T, int64_t> || 
                             std::is_same_v<T, uint64_t>) {
                    valueJson = arg;
                } else if constexpr (std::is_same_v<T, double>) {
                    valueJson = arg;
                } else if constexpr (std::is_same_v<T, bool>) {
                    valueJson = arg;
                } else if constexpr (std::is_same_v<T, std::string>) {
                    valueJson = arg;
                } else if constexpr (std::is_same_v<T, std::vector<double>>) {
                    valueJson = arg;
                } else if constexpr (std::is_same_v<T, std::vector<int>>) {
                    std::vector<double> vec(arg.begin(), arg.end());
                    valueJson = vec;
                } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
                    valueJson = arg;
                }
            }, entry.value);
            
            jsonConfig[entry.key] = {
                {"value", valueJson},
                {"description", entry.description},
                {"source", static_cast<int>(entry.source)}
            };
        } else {
            // Simple entry
            nlohmann::json valueJson;
            std::visit([&valueJson](auto&& arg) {
                using T = std::decay_t<decltype(arg)>;
                if constexpr (std::is_same_v<T, int> || std::is_same_v<T, int64_t> || 
                             std::is_same_v<T, uint64_t>) {
                    valueJson = arg;
                } else if constexpr (std::is_same_v<T, double>) {
                    valueJson = arg;
                } else if constexpr (std::is_same_v<T, bool>) {
                    valueJson = arg;
                } else if constexpr (std::is_same_v<T, std::string>) {
                    valueJson = arg;
                } else if constexpr (std::is_same_v<T, std::vector<double>>) {
                    valueJson = arg;
                } else if constexpr (std::is_same_v<T, std::vector<int>>) {
                    std::vector<double> vec(arg.begin(), arg.end());
                    valueJson = vec;
                } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
                    valueJson = arg;
                }
            }, entry.value);
            
            jsonConfig[entry.key] = valueJson;
        }
    }
    
    std::ofstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    file << std::setw(2) << jsonConfig << std::endl;
    return true;
}

} // namespace nlm
