#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <optional>
#include <unordered_map>

// Include JSON library (nlohmann/json)
#include <nlohmann/json.hpp>
// Include YAML library (yaml-cpp)
#include <yaml-cpp/yaml.h>

namespace nlm {

struct Config::Impl {
    std::vector<ConfigEntry> entries;
};

Config::Config() : pImpl(std::make_unique<Impl>()) {}

Config::~Config() = default;

Config::Config(Config&&) noexcept = default;

Config& Config::operator=(Config&&) noexcept = default;

bool Config::loadFromFile(const std::string& filepath) {
    ConfigFormat format = detectFileFormat(filepath);
    
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    std::stringstream buffer;
    buffer << file.rdbuf();
    std::string content = buffer.str();
    
    file.close();
    
    clear();
    
    bool result = false;
    switch (format) {
        case ConfigFormat::SimpleKeyValue:
            result = parseSimpleKeyValue(content);
            break;
        case ConfigFormat::JSON:
            result = parseJSON(content);
            break;
        case ConfigFormat::YAML:
            result = parseYAML(content);
            break;
        case ConfigFormat::Unknown:
        default:
            return false;
    }
    
    return result;
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
    ConfigFormat format = detectFileFormat(filepath);
    
    std::ofstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    bool result = false;
    switch (format) {
        case ConfigFormat::SimpleKeyValue:
            result = saveSimpleKeyValue(file);
            break;
        case ConfigFormat::JSON:
            result = saveJSON(file);
            break;
        case ConfigFormat::YAML:
            result = saveYAML(file);
            break;
        case ConfigFormat::Unknown:
        default:
            return false;
    }
    
    return result;
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

// File format detection
ConfigFormat Config::detectFileFormat(const std::string& filepath) {
    std::string ext = getFileExtension(filepath);
    
    if (ext.empty()) {
        return ConfigFormat::Unknown;
    }
    
    ext = toLower(ext);
    
    if (ext == "cfg" || ext == "config" || ext == "txt") {
        return ConfigFormat::SimpleKeyValue;
    } else if (ext == "json") {
        return ConfigFormat::JSON;
    } else if (ext == "yaml" || ext == "yml") {
        return ConfigFormat::YAML;
    }
    
    return ConfigFormat::Unknown;
}

std::string Config::getFileExtension(const std::string& filepath) {
    size_t pos = filepath.find_last_of('.');
    if (pos == std::string::npos || pos == filepath.length() - 1) {
        return "";
    }
    return filepath.substr(pos + 1);
}

bool Config::endsWith(const std::string& str, const std::string& suffix) {
    return str.size() >= suffix.size() && str.compare(str.size() - suffix.size(), suffix.size(), suffix) == 0;
}

// Simple key=value format parsing
bool Config::parseSimpleKeyValue(const std::string& content) {
    std::istringstream stream(content);
    std::string line;
    
    while (std::getline(stream, line)) {
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
            
            try {
                ConfigValue parsedValue = parseSimpleValue(value);
                set(key, parsedValue, ConfigSource::File);
            } catch (const std::exception& e) {
                continue;
            }
        }
    }
    
    return true;
}

// JSON parsing
bool Config::parseJSON(const std::string& content) {
    try {
        auto jsonData = nlohmann::json::parse(content);
        
        // Convert JSON to flat key-value pairs
        std::unordered_map<std::string, ConfigValue> flatConfig;
        
        for (auto it = jsonData.begin(); it != jsonData.end(); ++it) {
            std::string key = it.key();
            
            // Convert JSON value to ConfigValue
            ConfigValue configValue;
            
            if (it->is_string()) {
                configValue = it->get<std::string>();
            } else if (it->is_number_integer()) {
                configValue = it->get<int>();
            } else if (it->is_number_unsigned()) {
                configValue = it->get<uint64_t>();
            } else if (it->is_number_float()) {
                configValue = it->get<double>();
            } else if (it->is_boolean()) {
                configValue = it->get<bool>();
            } else if (it->is_array()) {
                // Handle arrays
                std::vector<ConfigValue> arrayValue;
                for (const auto& item : *it) {
                    ConfigValue itemValue;
                    if (item.is_string()) {
                        itemValue = item.get<std::string>();
                    } else if (item.is_number_integer()) {
                        itemValue = item.get<int>();
                    } else if (item.is_number_unsigned()) {
                        itemValue = item.get<uint64_t>();
                    } else if (item.is_number_float()) {
                        itemValue = item.get<double>();
                    } else if (item.is_boolean()) {
                        itemValue = item.get<bool>();
                    } else if (item.is_object()) {
                        // Nested object - convert to flat key
                        std::string itemKey = key + "." + std::to_string(&item - &(*it));
                        std::unordered_map<std::string, ConfigValue> nestedFlat;
                        flattenConfig(itemKey + ".", item, nestedFlat);
                        for (const auto& pair : nestedFlat) {
                            arrayValue.push_back(pair.second);
                        }
                        continue;
                    }
                    arrayValue.push_back(itemValue);
                }
                configValue = arrayValue;
            } else if (it->is_object()) {
                // Nested object - flatten it
                std::unordered_map<std::string, ConfigValue> nestedFlat;
                flattenConfig(key + ".", it.value(), nestedFlat);
                
                for (const auto& pair : nestedFlat) {
                    configValue = pair.second;
                    set(pair.first, configValue, ConfigSource::File);
                }
                continue;
            }
            
            set(key, configValue, ConfigSource::File);
        }
        
        return true;
        
    } catch (const nlohmann::json::parse_error& e) {
        return false;
    } catch (const std::exception& e) {
        return false;
    }
}

// YAML parsing
bool Config::parseYAML(const std::string& content) {
    try {
        auto yamlData = YAML::Load(content);
        
        // Convert YAML to flat key-value pairs
        std::unordered_map<std::string, ConfigValue> flatConfig;
        
        flattenConfig("", yamlData, flatConfig);
        
        for (const auto& pair : flatConfig) {
            set(pair.first, pair.second, ConfigSource::File);
        }
        
        return true;
        
    } catch (const YAML::ParserException& e) {
        return false;
    } catch (const std::exception& e) {
        return false;
    }
}

// Helper: flatten nested structures
void Config::flattenConfig(const std::string& prefix, const ConfigValue& value, std::unordered_map<std::string, ConfigValue>& flatConfig) {
    if (std::holds_alternative<std::string>(value)) {
        flatConfig[prefix] = std::get<std::string>(value);
    } else if (std::holds_alternative<int>(value)) {
        flatConfig[prefix] = std::get<int>(value);
    } else if (std::holds_alternative<int64_t>(value)) {
        flatConfig[prefix] = std::get<int64_t>(value);
    } else if (std::holds_alternative<double>(value)) {
        flatConfig[prefix] = std::get<double>(value);
    } else if (std::holds_alternative<bool>(value)) {
        flatConfig[prefix] = std::get<bool>(value);
    } else if (std::holds_alternative<std::vector<int>>(value)) {
        flatConfig[prefix] = std::get<std::vector<int>>(value);
    } else if (std::holds_alternative<std::vector<double>>(value)) {
        flatConfig[prefix] = std::get<std::vector<double>>(value);
    } else if (std::holds_alternative<std::vector<std::string>>(value)) {
        flatConfig[prefix] = std::get<std::vector<std::string>>(value);
    } else if (std::holds_alternative<std::vector<ConfigValue>>(value)) {
        flatConfig[prefix] = std::get<std::vector<ConfigValue>>(value);
    } else if (std::holds_alternative<std::unordered_map<std::string, ConfigValue>>(value)) {
        const auto& obj = std::get<std::unordered_map<std::string, ConfigValue>>(value);
        for (const auto& pair : obj) {
            std::string newKey = prefix + pair.first;
            if (!prefix.empty()) {
                newKey = prefix + "." + pair.first;
            }
            flattenConfig(newKey, pair.second, flatConfig);
        }
    }
}

// Helper: flatten from nlohmann::json
void Config::flattenConfig(const std::string& prefix, const nlohmann::json& jsonData, std::unordered_map<std::string, ConfigValue>& flatConfig) {
    if (jsonData.is_string()) {
        flatConfig[prefix] = jsonData.get<std::string>();
    } else if (jsonData.is_number_integer()) {
        flatConfig[prefix] = jsonData.get<int>();
    } else if (jsonData.is_number_unsigned()) {
        flatConfig[prefix] = jsonData.get<uint64_t>();
    } else if (jsonData.is_number_float()) {
        flatConfig[prefix] = jsonData.get<double>();
    } else if (jsonData.is_boolean()) {
        flatConfig[prefix] = jsonData.get<bool>();
    } else if (jsonData.is_array()) {
        std::vector<ConfigValue> arrayValue;
        for (size_t i = 0; i < jsonData.size(); ++i) {
            ConfigValue itemValue;
            if (jsonData[i].is_string()) {
                itemValue = jsonData[i].get<std::string>();
            } else if (jsonData[i].is_number_integer()) {
                itemValue = jsonData[i].get<int>();
            } else if (jsonData[i].is_number_unsigned()) {
                itemValue = jsonData[i].get<uint64_t>();
            } else if (jsonData[i].is_number_float()) {
                itemValue = jsonData[i].get<double>();
            } else if (jsonData[i].is_boolean()) {
                itemValue = jsonData[i].get<bool>();
            } else if (jsonData[i].is_object()) {
                std::string itemKey = prefix + "." + std::to_string(i);
                flattenConfig(itemKey + ".", jsonData[i], flatConfig);
                continue;
            }
            arrayValue.push_back(itemValue);
        }
        flatConfig[prefix] = arrayValue;
    } else if (jsonData.is_object()) {
        for (auto it = jsonData.begin(); it != jsonData.end(); ++it) {
            std::string newKey = prefix + "." + it.key();
            flattenConfig(newKey, it.value(), flatConfig);
        }
    }
}

// Helper: flatten from YAML::Node
void Config::flattenConfig(const std::string& prefix, const YAML::Node& yamlData, std::unordered_map<std::string, ConfigValue>& flatConfig) {
    if (yamlData.IsScalar()) {
        std::string value = yamlData.Scalar();
        
        if (value == "true" || value == "false") {
            flatConfig[prefix] = (value == "true");
        } else if (std::all_of(value.begin(), value.end(), ::isdigit) || 
                   (value[0] == '-' && std::all_of(value.begin() + 1, value.end(), ::isdigit))) {
            try {
                if (value.find('.') != std::string::npos) {
                    flatConfig[prefix] = std::stod(value);
                } else {
                    flatConfig[prefix] = std::stoll(value);
                }
            } catch (...) {
                flatConfig[prefix] = value;
            }
        } else {
            flatConfig[prefix] = value;
        }
    } else if (yamlData.IsSequence()) {
        std::vector<ConfigValue> arrayValue;
        for (size_t i = 0; i < yamlData.size(); ++i) {
            if (yamlData[i].IsScalar()) {
                std::string itemValue = yamlData[i].Scalar();
                
                if (itemValue == "true" || itemValue == "false") {
                    arrayValue.push_back(itemValue == "true");
                } else if (std::all_of(itemValue.begin(), itemValue.end(), ::isdigit) || 
                           (itemValue[0] == '-' && std::all_of(itemValue.begin() + 1, itemValue.end(), ::isdigit))) {
                    try {
                        if (itemValue.find('.') != std::string::npos) {
                            arrayValue.push_back(std::stod(itemValue));
                        } else {
                            arrayValue.push_back(std::stoll(itemValue));
                        }
                    } catch (...) {
                        arrayValue.push_back(itemValue);
                    }
                } else {
                    arrayValue.push_back(itemValue);
                }
            }
        }
        flatConfig[prefix] = arrayValue;
    } else if (yamlData.IsMap()) {
        for (auto it = yamlData.begin(); it != yamlData.end(); ++it) {
            std::string newKey = prefix + "." + it->first.Scalar();
            flattenConfig(newKey, *it, flatConfig);
        }
    }
}

// Helper: parse simple values from string
ConfigValue Config::parseSimpleValue(const std::string& valueStr) {
    if (valueStr == "true") {
        return true;
    } else if (valueStr == "false") {
        return false;
    }
    
    // Try integer
    try {
        size_t pos;
        long long intVal = std::stoll(valueStr, &pos);
        if (pos == valueStr.length()) {
            return intVal;
        }
    } catch (...) {}
    
    // Try unsigned integer
    try {
        size_t pos;
        unsigned long long uintVal = std::stoull(valueStr, &pos);
        if (pos == valueStr.length()) {
            return static_cast<uint64_t>(uintVal);
        }
    } catch (...) {}
    
    // Try double
    try {
        size_t pos;
        double doubleVal = std::stod(valueStr, &pos);
        if (pos == valueStr.length()) {
            return doubleVal;
        }
    } catch (...) {}
    
    // Default to string
    return valueStr;
}

// Helper: convert ConfigValue to string
std::string Config::configValueToString(const ConfigValue& val) const {
    if (std::holds_alternative<std::string>(val)) {
        return std::get<std::string>(val);
    } else if (std::holds_alternative<int>(val)) {
        return std::to_string(std::get<int>(val));
    } else if (std::holds_alternative<int64_t>(val)) {
        return std::to_string(std::get<int64_t>(val));
    } else if (std::holds_alternative<double>(val)) {
        return std::to_string(std::get<double>(val));
    } else if (std::holds_alternative<bool>(val)) {
        return std::get<bool>(val) ? "true" : "false";
    }
    return "";
}

// Simple key=value save
bool Config::saveSimpleKeyValue(std::ofstream& file) const {
    for (const auto& entry : pImpl->entries) {
        file << "# " << entry.description << "\n";
        file << entry.key << " = " << configValueToString(entry.value) << "\n\n";
    }
    return true;
}

// JSON save
bool Config::saveJSON(std::ofstream& file) const {
    nlohmann::json jsonData;
    
    for (const auto& entry : pImpl->entries) {
        std::string key = entry.key;
        ConfigValue value = entry.value;
        
        // Handle nested keys (dots in key names)
        if (key.find('.') != std::string::npos) {
            // Create nested JSON structure
            std::istringstream keyStream(key);
            std::string segment;
            nlohmann::json current = jsonData;
            
            while (std::getline(keyStream, segment, '.')) {
                if (keyStream.eof()) {
                    // This is the final segment
                    if (std::holds_alternative<std::string>(value)) {
                        current[segment] = std::get<std::string>(value);
                    } else if (std::holds_alternative<int>(value)) {
                        current[segment] = std::get<int>(value);
                    } else if (std::holds_alternative<int64_t>(value)) {
                        current[segment] = std::get<int64_t>(value);
                    } else if (std::holds_alternative<double>(value)) {
                        current[segment] = std::get<double>(value);
                    } else if (std::holds_alternative<bool>(value)) {
                        current[segment] = std::get<bool>(value);
                    } else if (std::holds_alternative<std::vector<int>>(value)) {
                        current[segment] = std::get<std::vector<int>>(value);
                    } else if (std::holds_alternative<std::vector<double>>(value)) {
                        current[segment] = std::get<std::vector<double>>(value);
                    } else if (std::holds_alternative<std::vector<std::string>>(value)) {
                        current[segment] = std::get<std::vector<std::string>>(value);
                    }
                    break;
                }
                
                if (!current.contains(segment)) {
                    current[segment] = nlohmann::json::object();
                }
                current = current[segment];
            }
        } else {
            // Simple key
            if (std::holds_alternative<std::string>(value)) {
                jsonData[key] = std::get<std::string>(value);
            } else if (std::holds_alternative<int>(value)) {
                jsonData[key] = std::get<int>(value);
            } else if (std::holds_alternative<int64_t>(value)) {
                jsonData[key] = std::get<int64_t>(value);
            } else if (std::holds_alternative<double>(value)) {
                jsonData[key] = std::get<double>(value);
            } else if (std::holds_alternative<bool>(value)) {
                jsonData[key] = std::get<bool>(value);
            } else if (std::holds_alternative<std::vector<int>>(value)) {
                jsonData[key] = std::get<std::vector<int>>(value);
            } else if (std::holds_alternative<std::vector<double>>(value)) {
                jsonData[key] = std::get<std::vector<double>>(value);
            } else if (std::holds_alternative<std::vector<std::string>>(value)) {
                jsonData[key] = std::get<std::vector<std::string>>(value);
            }
        }
    }
    
    file << jsonData.dump(2);
    return true;
}

// YAML save
bool Config::saveYAML(std::ofstream& file) const {
    YAML::Emitter emitter;
    
    emitter << YAML::BeginMap;
    
    for (const auto& entry : pImpl->entries) {
        std::string key = entry.key;
        ConfigValue value = entry.value;
        
        if (std::holds_alternative<std::string>(value)) {
            emitter << YAML::Key << key << YAML::Value << std::get<std::string>(value);
        } else if (std::holds_alternative<int>(value)) {
            emitter << YAML::Key << key << YAML::Value << std::get<int>(value);
        } else if (std::holds_alternative<int64_t>(value)) {
            emitter << YAML::Key << key << YAML::Value << std::get<int64_t>(value);
        } else if (std::holds_alternative<double>(value)) {
            emitter << YAML::Key << key << YAML::Value << std::get<double>(value);
        } else if (std::holds_alternative<bool>(value)) {
            emitter << YAML::Key << key << YAML::Value << std::get<bool>(value);
        } else if (std::holds_alternative<std::vector<int>>(value)) {
            emitter << YAML::Key << key << YAML::Value << std::get<std::vector<int>>(value);
        } else if (std::holds_alternative<std::vector<double>>(value)) {
            emitter << YAML::Key << key << YAML::Value << std::get<std::vector<double>>(value);
        } else if (std::holds_alternative<std::vector<std::string>>(value)) {
            emitter << YAML::Key << key << YAML::Value << std::get<std::vector<std::string>>(value);
        }
    }
    
    emitter << YAML::EndMap;
    
    file << emitter.c_str();
    return true;
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
