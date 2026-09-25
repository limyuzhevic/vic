#include "Config.hpp"
#include "ConfigValidator.hpp"
#include <nlohmann/json.hpp>
#include <yaml-cpp/yaml.h>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <regex>

namespace nlm {

// Helper functions for parsing
static std::string trim(const std::string& str);
static bool parseValueFromString(const std::string& str, ConfigValue& value);
static std::string valueToString(const ConfigValue& value);

struct Config::Impl {
    std::vector<ConfigEntry> entries;
    std::string fileFormat;  // "json", "yaml", "ini", or ""
};

Config::Config() : pImpl(std::make_unique<Impl>()) {
    pImpl->fileFormat = "json";  // Default format
}

Config::~Config() = default;

Config::Config(Config&&) noexcept = default;

Config& Config::operator=(Config&&) noexcept = default;

bool Config::loadFromFile(const std::string& filepath) {
    // Validate file format
    std::string extension = filepath.substr(filepath.find_last_of('.') + 1);
    std::transform(extension.begin(), extension.end(), extension.begin(), ::tolower);
    
    bool isValidFormat = (extension == "json" || extension == "yaml" || extension == "yml" || extension == "ini");
    if (!isValidFormat) {
        NLM_LOG_WARNING("Unsupported configuration file format: " + extension);
        return false;
    }
    
    pImpl->fileFormat = extension;
    
    // Try to parse based on file format
    if (extension == "json") {
        return JSONConfigLoader::loadFromJSON(filepath, pImpl->entries);
    } else if (extension == "yaml" || extension == "yml") {
        return JSONConfigLoader::loadFromYAML(filepath, pImpl->entries);
    } else {  // ini format
        return loadFromINI(filepath);
    }
}

bool Config::loadFromINI(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    std::string line;
    std::string currentSection = "";
    while (std::getline(file, line)) {
        // Skip empty lines and comments
        line = trim(line);
        if (line.empty() || line[0] == ';' || line[0] == '#') {
            continue;
        }
        
        // Section header
        if (line[0] == '[' && line.back() == ']') {
            currentSection = line.substr(1, line.size() - 2);
            continue;
        }
        
        // Key=value pair
        size_t pos = line.find('=');
        if (pos != std::string::npos) {
            std::string key = trim(line.substr(0, pos));
            std::string value = trim(line.substr(pos + 1));
            
            // Add section prefix to key if section exists
            if (!currentSection.empty()) {
                key = currentSection + "." + key;
            }
            
            set(key, value, ConfigSource::File);
        }
    }
    
    return true;
}

bool JSONConfigLoader::loadFromJSON(const std::string& filepath, std::vector<ConfigEntry>& entries) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    try {
        nlohmann::json jsonData;
        file >> jsonData;
        
        // Clear existing entries
        entries.clear();
        
        // Parse JSON recursively
        parseJSON(jsonData, "", entries);
        
        return true;
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Failed to parse JSON config file '" + filepath + "': " + std::string(e.what()));
        return false;
    }
}

bool JSONConfigLoader::loadFromYAML(const std::string& filepath, std::vector<ConfigEntry>& entries) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    try {
        YAML::Node yamlData = YAML::Load(file);
        
        // Clear existing entries
        entries.clear();
        
        // Parse YAML recursively
        parseYAML(yamlData, "", entries);
        
        return true;
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Failed to parse YAML config file '" + filepath + "': " + std::string(e.what()));
        return false;
    }
}

bool JSONConfigLoader::saveToJSON(const std::string& filepath, const std::vector<ConfigEntry>& entries) {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    try {
        nlohmann::json jsonData;
        
        // Convert entries to JSON
        for (const auto& entry : entries) {
            setJSONValue(jsonData, entry.key, entry.value);
        }
        
        file << jsonData.dump(2);  // Pretty print with 2-space indentation
        return true;
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Failed to write JSON config file '" + filepath + "': " + std::string(e.what()));
        return false;
    }
}

bool JSONConfigLoader::saveToYAML(const std::string& filepath, const std::vector<ConfigEntry>& entries) {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    try {
        YAML::Emitter emitter;
        emitter << YAML::BeginMap;
        
        // Convert entries to YAML
        for (const auto& entry : entries) {
            setYAMLEmitter(emitter, entry.key, entry.value);
        }
        
        emitter << YAML::EndMap;
        file << emitter.c_str();
        return true;
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Failed to write YAML config file '" + filepath + "': " + std::string(e.what()));
        return false;
    }
}

void JSONConfigLoader::parseJSON(const nlohmann::json& jsonData, const std::string& prefix, std::vector<ConfigEntry>& entries) {
    if (jsonData.is_object()) {
        for (auto it = jsonData.begin(); it != jsonData.end(); ++it) {
            std::string key = prefix.empty() ? it.key() : prefix + "." + it.key();
            parseJSON(*it, key, entries);
        }
    } else if (jsonData.is_array()) {
        for (size_t i = 0; i < jsonData.size(); ++i) {
            std::string key = prefix.empty() ? "[" + std::to_string(i) + "]" : prefix + ".[" + std::to_string(i) + "]";
            parseJSON(jsonData[i], key, entries);
        }
    } else {
        // Leaf node - store as config entry
        ConfigValue value;
        if (parseJSONValue(jsonData, value)) {
            entries.emplace_back(prefix, value, ConfigSource::File);
        }
    }
}

void JSONConfigLoader::parseYAML(const YAML::Node& yamlData, const std::string& prefix, std::vector<ConfigEntry>& entries) {
    if (yamlData.IsMap()) {
        for (auto it = yamlData.begin(); it != yamlData.end(); ++it) {
            std::string key = prefix.empty() ? it->first.as<std::string>() : prefix + "." + it->first.as<std::string>();
            parseYAML(*it, key, entries);
        }
    } else if (yamlData.IsSequence()) {
        for (size_t i = 0; i < yamlData.size(); ++i) {
            std::string key = prefix.empty() ? "[" + std::to_string(i) + "]" : prefix + ".[" + std::to_string(i) + "]";
            parseYAML(yamlData[i], key, entries);
        }
    } else {
        // Leaf node - store as config entry
        ConfigValue value;
        if (parseYAMLValue(yamlData, value)) {
            entries.emplace_back(prefix, value, ConfigSource::File);
        }
    }
}

bool JSONConfigLoader::parseJSONValue(const nlohmann::json& jsonData, ConfigValue& value) {
    if (jsonData.is_number_integer()) {
        value = jsonData.get<int>();
        return true;
    } else if (jsonData.is_number_unsigned()) {
        value = static_cast<int64_t>(jsonData.get<uint64_t>());
        return true;
    } else if (jsonData.is_number_float()) {
        value = jsonData.get<double>();
        return true;
    } else if (jsonData.is_boolean()) {
        value = jsonData.get<bool>();
        return true;
    } else if (jsonData.is_string()) {
        value = jsonData.get<std::string>();
        return true;
    } else if (jsonData.is_array()) {
        std::vector<double> doubleArray;
        for (const auto& item : jsonData) {
            if (item.is_number()) {
                doubleArray.push_back(item.get<double>());
            } else {
                return false;  // Only support numeric arrays for now
            }
        }
        value = doubleArray;
        return true;
    }
    
    return false;
}

bool JSONConfigLoader::parseYAMLValue(const YAML::Node& yamlData, ConfigValue& value) {
    if (yamlData.IsScalar()) {
        if (yamlData.IsInteger()) {
            value = yamlData.as<int>();
            return true;
        } else if (yamlData.IsDouble()) {
            value = yamlData.as<double>();
            return true;
        } else if (yamlData.IsBoolean()) {
            value = yamlData.as<bool>();
            return true;
        } else {
            value = yamlData.as<std::string>();
            return true;
        }
    } else if (yamlData.IsSequence()) {
        std::vector<double> doubleArray;
        for (const auto& item : yamlData) {
            if (item.IsScalar()) {
                if (item.IsInteger()) {
                    doubleArray.push_back(item.as<int>());
                } else if (item.IsDouble()) {
                    doubleArray.push_back(item.as<double>());
                } else if (item.IsBoolean()) {
                    doubleArray.push_back(item.as<bool>() ? 1.0 : 0.0);
                } else {
                    return false;  // Only support simple types in arrays
                }
            } else {
                return false;
            }
        }
        value = doubleArray;
        return true;
    }
    
    return false;
}

void JSONConfigLoader::setJSONValue(nlohmann::json& jsonData, const std::string& key, const ConfigValue& value) {
    std::vector<std::string> keyPath = splitKey(key);
    nlohmann::json* current = &jsonData;
    
    for (size_t i = 0; i < keyPath.size() - 1; ++i) {
        if (!current->contains(keyPath[i])) {
            (*current)[keyPath[i]] = nlohmann::json::object();
        }
        current = &(*current)[keyPath[i]];
    }
    
    // Set the value
    setJSONValue(*current, keyPath.back(), value);
}

void JSONConfigLoader::setYAMLEmitter(YAML::Emitter& emitter, const std::string& key, const ConfigValue& value) {
    std::vector<std::string> keyPath = splitKey(key);
    
    // For simplicity, just emit as flat key-value pairs
    // In a real implementation, you'd want proper nesting
    emitter << YAML::Key << key << YAML::Value;
    emitYAMLValue(emitter, value);
}

void JSONConfigLoader::setJSONValue(nlohmann::json& jsonData, const std::string& key, const ConfigValue& value) {
    if (std::holds_alternative<int>(value)) {
        jsonData[key] = std::get<int>(value);
    } else if (std::holds_alternative<int64_t>(value)) {
        jsonData[key] = std::get<int64_t>(value);
    } else if (std::holds_alternative<double>(value)) {
        jsonData[key] = std::get<double>(value);
    } else if (std::holds_alternative<bool>(value)) {
        jsonData[key] = std::get<bool>(value);
    } else if (std::holds_alternative<std::string>(value)) {
        jsonData[key] = std::get<std::string>(value);
    } else if (std::holds_alternative<std::vector<int>>(value)) {
        jsonData[key] = std::get<std::vector<int>>(value);
    } else if (std::holds_alternative<std::vector<double>>(value)) {
        jsonData[key] = std::get<std::vector<double>>(value);
    } else if (std::holds_alternative<std::vector<std::string>>(value)) {
        jsonData[key] = std::get<std::vector<std::string>>(value);
    }
}

void JSONConfigLoader::emitYAMLValue(YAML::Emitter& emitter, const ConfigValue& value) {
    if (std::holds_alternative<int>(value)) {
        emitter << std::get<int>(value);
    } else if (std::holds_alternative<int64_t>(value)) {
        emitter << std::get<int64_t>(value);
    } else if (std::holds_alternative<double>(value)) {
        emitter << std::get<double>(value);
    } else if (std::holds_alternative<bool>(value)) {
        emitter << std::get<bool>(value);
    } else if (std::holds_alternative<std::string>(value)) {
        emitter << std::get<std::string>(value);
    } else if (std::holds_alternative<std::vector<int>>(value)) {
        emitter << YAML::Flow << std::get<std::vector<int>>(value);
    } else if (std::holds_alternative<std::vector<double>>(value)) {
        emitter << YAML::Flow << std::get<std::vector<double>>(value);
    } else if (std::holds_alternative<std::vector<std::string>>(value)) {
        emitter << YAML::Flow << std::get<std::vector<std::string>>(value);
    }
}

std::vector<std::string> JSONConfigLoader::splitKey(const std::string& key) {
    std::vector<std::string> result;
    std::string current;
    
    for (char c : key) {
        if (c == '.') {
            if (!current.empty()) {
                result.push_back(current);
                current.clear();
            }
        } else {
            current.push_back(c);
        }
    }
    
    if (!current.empty()) {
        result.push_back(current);
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
                
                // Validate before setting
                ConfigValue configValue;
                if (parseValueFromString(value, configValue)) {
                    if (ConfigValidator::validate(key, configValue)) {
                        set(key, configValue, ConfigSource::CommandLine);
                    } else {
                        NLM_LOG_WARNING("Invalid value for command line argument '--" + key + "': '" + value + "'");
                    }
                } else {
                    NLM_LOG_WARNING("Cannot parse value for command line argument '--" + key + "': '" + value + "'");
                }
            }
        }
        // Handle -key value format
        else if (arg[0] == '-' && i + 1 < argc) {
            std::string key = arg.substr(1);
            std::string value = argv[++i];
            
            // Validate before setting
            ConfigValue configValue;
            if (parseValueFromString(value, configValue)) {
                if (ConfigValidator::validate(key, configValue)) {
                    set(key, configValue, ConfigSource::CommandLine);
                } else {
                    NLM_LOG_WARNING("Invalid value for command line argument '-" + key + "': '" + value + "'");
                }
            } else {
                NLM_LOG_WARNING("Cannot parse value for command line argument '-" + key + "': '" + value + "'");
            }
        }
    }
    
    return true;
}

bool Config::saveToFile(const std::string& filepath) const {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    // Determine file format from extension
    std::string extension = filepath.substr(filepath.find_last_of('.') + 1);
    std::transform(extension.begin(), extension.end(), extension.begin(), ::tolower);
    
    if (extension == "json") {
        return JSONConfigLoader::saveToJSON(filepath, pImpl->entries);
    } else if (extension == "yaml" || extension == "yml") {
        return JSONConfigLoader::saveToYAML(filepath, pImpl->entries);
    } else {
        NLM_LOG_WARNING("Unsupported config file format for saving: " + extension);
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
        return std::nullopt;
    }
}

template<typename T>
T Config::getOr(const std::string& key, const T& defaultValue) const {
    auto val = get<T>(key);
    return val.has_value() ? val.value() : defaultValue;
}

void Config::set(const std::string& key, const ConfigValue& value, ConfigSource source) {
    // Validate before setting
    if (!ConfigValidator::validate(key, value)) {
        std::string error = ConfigValidator::getValidationError(key, value);
        NLM_LOG_ERROR("Configuration validation failed for key '" + key + "': " + error);
        return;
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
    ConfigValue configValue;
    if (parseValueFromString(value, configValue)) {
        set(key, configValue, source);
    } else {
        NLM_LOG_WARNING("Cannot parse string value for key '" + key + "': '" + value + "'");
    }
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
        std::remove_if(pImpl->entries.begin(),
                      pImpl->entries.end(),
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

// Helper functions
bool parseValueFromString(const std::string& str, ConfigValue& value) {
    // Try to parse as boolean
    if (str == "true" || str == "1" || str == "yes" || str == "on") {
        value = true;
        return true;
    } else if (str == "false" || str == "0" || str == "no" || str == "off") {
        value = false;
        return true;
    }
    
    // Try to parse as integer
    try {
        size_t pos;
        int intVal = std::stoi(str, &pos);
        if (pos == str.size()) {
            value = intVal;
            return true;
        }
    } catch (const std::exception&) {}
    
    // Try to parse as int64_t
    try {
        size_t pos;
        int64_t int64Val = std::stoll(str, &pos);
        if (pos == str.size()) {
            value = int64Val;
            return true;
        }
    } catch (const std::exception&) {}
    
    // Try to parse as double
    try {
        size_t pos;
        double doubleVal = std::stod(str, &pos);
        if (pos == str.size()) {
            value = doubleVal;
            return true;
        }
    } catch (const std::exception&) {}
    
    // Try to parse as string (already a string)
    value = str;
    return true;
}

std::string valueToString(const ConfigValue& value) {
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
