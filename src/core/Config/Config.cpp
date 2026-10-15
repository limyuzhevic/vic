#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <regex>
#include <unordered_map>
#include <nlohmann/json.hpp>
#include <yaml-cpp/yaml.h>

using json = nlohmann::json;
using YAML = YAML::Node;

namespace nlm {

struct Config::Impl {
    std::vector<ConfigEntry> entries;
    std::unordered_map<std::string, ConfigSchema> schemas;
    std::unordered_map<std::string, std::string> environmentPrefix;
};

// ConfigEntry helper implementations
bool ConfigEntry::isType(const std::string& expectedType) const {
    if (expectedType == "int" && std::holds_alternative<int>(value)) return true;
    if (expectedType == "int64" && std::holds_alternative<int64_t>(value)) return true;
    if (expectedType == "float" && std::holds_alternative<double>(value)) return true;
    if (expectedType == "bool" && std::holds_alternative<bool>(value)) return true;
    if (expectedType == "string" && std::holds_alternative<std::string>(value)) return true;
    if (expectedType == "int-array" && std::holds_alternative<std::vector<int>>(value)) return true;
    if (expectedType == "float-array" && std::holds_alternative<std::vector<double>>(value)) return true;
    if (expectedType == "string-array" && std::holds_alternative<std::vector<std::string>>(value)) return true;
    if (expectedType == "bool-array" && std::holds_alternative<std::vector<bool>>(value)) return true;
    if (expectedType == "object" && std::holds_alternative<std::unordered_map<std::string, std::string>>(value)) return true;
    return false;
}

bool ConfigEntry::validate() const {
    if (validationRegex.empty() && !minValue && !maxValue) {
        return true; // No validation rules
    }
    
    std::string strValue;
    
    if (std::holds_alternative<int>(value)) {
        strValue = std::to_string(std::get<int>(value));
    } else if (std::holds_alternative<int64_t>(value)) {
        strValue = std::to_string(std::get<int64_t>(value));
    } else if (std::holds_alternative<double>(value)) {
        strValue = std::to_string(std::get<double>(value));
    } else if (std::holds_alternative<bool>(value)) {
        strValue = std::get<bool>(value) ? "true" : "false";
    } else if (std::holds_alternative<std::string>(value)) {
        strValue = std::get<std::string>(value);
    } else {
        return false; // Can't validate complex types
    }
    
    // Check regex validation
    if (!validationRegex.empty()) {
        std::regex pattern(validationRegex);
        if (!std::regex_match(strValue, pattern)) {
            return false;
        }
    }
    
    // Check min/max validation for numeric types
    try {
        double numValue = 0.0;
        if (std::holds_alternative<int>(value)) {
            numValue = std::get<int>(value);
        } else if (std::holds_alternative<int64_t>(value)) {
            numValue = static_cast<double>(std::get<int64_t>(value));
        } else if (std::holds_alternative<double>(value)) {
            numValue = std::get<double>(value);
        }
        
        if (minValue && numValue < *minValue) return false;
        if (maxValue && numValue > *maxValue) return false;
    } catch (...) {
        return false;
    }
    
    return true;
}

template<typename T>
std::optional<T> ConfigEntry::getValueAs() const {
    try {
        if constexpr (std::is_same_v<T, int> && std::holds_alternative<int>(value)) {
            return std::get<int>(value);
        } else if constexpr (std::is_same_v<T, int64_t> && std::holds_alternative<int64_t>(value)) {
            return std::get<int64_t>(value);
        } else if constexpr (std::is_same_v<T, double> && std::holds_alternative<double>(value)) {
            return std::get<double>(value);
        } else if constexpr (std::is_same_v<T, bool> && std::holds_alternative<bool>(value)) {
            return std::get<bool>(value);
        } else if constexpr (std::is_same_v<T, std::string> && std::holds_alternative<std::string>(value)) {
            return std::get<std::string>(value);
        } else if constexpr (std::is_same_v<T, std::vector<int>> && std::holds_alternative<std::vector<int>>(value)) {
            return std::get<std::vector<int>>(value);
        } else if constexpr (std::is_same_v<T, std::vector<double>> && std::holds_alternative<std::vector<double>>(value)) {
            return std::get<std::vector<double>>(value);
        } else if constexpr (std::is_same_v<T, std::vector<std::string>> && std::holds_alternative<std::vector<std::string>>(value)) {
            return std::get<std::vector<std::string>>(value);
        } else if constexpr (std::is_same_v<T, std::vector<bool>> && std::holds_alternative<std::vector<bool>>(value)) {
            return std::get<std::vector<bool>>(value);
        } else if constexpr (std::is_same_v<T, std::unordered_map<std::string, std::string>> && std::holds_alternative<std::unordered_map<std::string, std::string>>(value)) {
            return std::get<std::unordered_map<std::string, std::string>>(value);
        }
    } catch (const std::bad_variant_access&) {}
    return std::nullopt;
}

void ConfigEntry::setValue(const ConfigValue& newValue, ConfigSource source) {
    value = newValue;
    this->source = source;
}

// Config helper implementations
Config::Config() : pImpl(std::make_unique<Impl>()) {
    // Set up default schema for common keys
    setupDefaultSchemas();
}

Config::~Config() = default;

Config::Config(Config&&) noexcept = default;

Config& Config::operator=(Config&&) noexcept = default;

void Config::setupDefaultSchemas() {
    // Add default schemas for common NLM configuration keys
    addSchema(ConfigSchema{"random_seed", "int", true, std::nullopt, std::nullopt, 
                          "^\\d+$", "Random seed for reproducible experiments", 
                          [](const ConfigValue& v) { return std::holds_alternative<int>(v); }});
    
    addSchema(ConfigSchema{"neuron_count", "int", true, 1, 10000,
                          "^[1-9]\\d*$", "Number of neurons in the brain",
                          [](const ConfigValue& v) { 
                              return std::holds_alternative<int>(v) && std::get<int>(v) > 0; }});
    
    addSchema(ConfigSchema{"region_count", "int", true, 1, 10,
                          "^[1-9]\\d*$", "Number of brain regions",
                          [](const ConfigValue& v) { 
                              return std::holds_alternative<int>(v) && std::get<int>(v) > 0; }});
    
    addSchema(ConfigSchema{"connection_probability", "float", true, 0.0, 1.0,
                          "^(0\\.\\d+|1\\.0?)$", "Connection probability (0.0-1.0)",
                          [](const ConfigValue& v) {
                              return std::holds_alternative<double>(v) && 
                                     std::get<double>(v) >= 0.0 && std::get<double>(v) <= 1.0; }});
    
    addSchema(ConfigSchema{"simulation_timestep", "float", true, 0.0, 1.0,
                          "^0\\.\\d+$", "Simulation timestep in seconds",
                          [](const ConfigValue& v) {
                              return std::holds_alternative<double>(v) && std::get<double>(v) > 0.0; }});
}

bool Config::loadFromFile(const std::string& filepath) {
    std::string extension = "." + std::filesystem::path(filepath).extension().string();
    std::transform(extension.begin(), extension.end(), extension.begin(), ::tolower);
    
    if (extension == ".json") {
        std::ifstream file(filepath);
        if (!file.is_open()) return false;
        json j;
        file >> j;
        return loadFromJSON(jsonToConfigValue(j));
    } else if (extension == ".yaml" || extension == ".yml") {
        std::ifstream file(filepath);
        if (!file.is_open()) return false;
        YAML yamlDoc = YAML::LoadFile(filepath);
        return loadFromYAML(yamlToConfigValue(yamlDoc));
    } else if (extension == ".toml") {
        // TODO: Implement TOML support
        NLM_LOG_ERROR("TOML configuration not yet implemented");
        return false;
    } else {
        // Legacy key=value format
        return loadFromLegacyFile(filepath);
    }
}

bool Config::loadFromJSON(const std::string& jsonString) {
    try {
        auto j = json::parse(jsonString);
        return loadFromJSON(j);
    } catch (const json::parse_error& e) {
        NLM_LOG_ERROR("JSON parse error: " + std::string(e.what()));
        return false;
    }
}

bool Config::loadFromJSON(const json& j) {
    clear();
    
    for (auto it = j.begin(); it != j.end(); ++it) {
        std::string key = it.key();
        ConfigValue value = jsonToConfigValue(*it);
        
        // Apply schema validation if available
        if (pImpl->schemas.find(key) != pImpl->schemas.end()) {
            ConfigEntry entry(key, value, ConfigSource::File, pImpl->schemas[key].description, 
                             pImpl->schemas[key].type, pImpl->schemas[key].validationRegex,
                             pImpl->schemas[key].minValue, pImpl->schemas[key].maxValue,
                             pImpl->schemas[key].required);
            
            if (entry.validate() && pImpl->schemas[key].validator(value)) {
                pImpl->entries.push_back(entry);
            } else {
                NLM_LOG_WARNING("Invalid value for config key " + key + ": " + jsonToString(*it));
            }
        } else {
            pImpl->entries.emplace_back(key, value, ConfigSource::File);
        }
    }
    
    return true;
}

bool Config::loadFromYAML(const std::string& yamlString) {
    try {
        auto yamlDoc = YAML::Load(yamlString);
        return loadFromYAML(yamlDoc);
    } catch (const YAML::Exception& e) {
        NLM_LOG_ERROR("YAML parse error: " + std::string(e.what()));
        return false;
    }
}

bool Config::loadFromYAML(const YAML& yamlDoc) {
    clear();
    
    for (const auto& it : yamlDoc) {
        std::string key = it.first.as<std::string>();
        ConfigValue value = yamlToConfigValue(it.second);
        
        // Apply schema validation if available
        if (pImpl->schemas.find(key) != pImpl->schemas.end()) {
            ConfigEntry entry(key, value, ConfigSource::File, pImpl->schemas[key].description,
                             pImpl->schemas[key].type, pImpl->schemas[key].validationRegex,
                             pImpl->schemas[key].minValue, pImpl->schemas[key].maxValue,
                             pImpl->schemas[key].required);
            
            if (entry.validate() && pImpl->schemas[key].validator(value)) {
                pImpl->entries.push_back(entry);
            } else {
                NLM_LOG_WARNING("Invalid value for config key " + key + ": " + yamlToString(it.second));
            }
        } else {
            pImpl->entries.emplace_back(key, value, ConfigSource::File);
        }
    }
    
    return true;
}

bool Config::loadFromTOML(const std::string& tomlString) {
    // TODO: Implement TOML support
    NLM_LOG_ERROR("TOML configuration not yet implemented");
    return false;
}

ConfigValue Config::jsonToConfigValue(const json& j) {
    if (j.is_string()) {
        return j.get<std::string>();
    } else if (j.is_number_integer()) {
        return j.get<int>();
    } else if (j.is_number_unsigned()) {
        return j.get<int64_t>();
    } else if (j.is_number_float()) {
        return j.get<double>();
    } else if (j.is_boolean()) {
        return j.get<bool>();
    } else if (j.is_array()) {
        std::vector<std::string> result;
        for (const auto& item : j) {
            if (item.is_string()) {
                result.push_back(item.get<std::string>());
            }
        }
        return result;
    } else if (j.is_object()) {
        std::unordered_map<std::string, std::string> result;
        for (auto it = j.begin(); it != j.end(); ++it) {
            result[it.key()] = it.value().get<std::string>();
        }
        return result;
    } else {
        throw std::runtime_error("Unsupported JSON type");
    }
}

ConfigValue Config::yamlToConfigValue(const YAML& y) {
    if (y.IsScalar()) {
        if (y.IsSequence()) {
            // This shouldn't happen since IsScalar() is true
            return std::vector<std::string>();
        }
        
        const std::string& value = y.Scalar();
        
        // Try to detect type
        if (value == "true" || value == "false") {
            return value == "true";
        } else if (std::all_of(value.begin(), value.end(), ::isdigit)) {
            try {
                return std::stoll(value);
            } catch (...) {
                return std::stod(value);
            }
        } else {
            return value;
        }
    } else if (y.IsSequence()) {
        std::vector<std::string> result;
        for (const auto& item : y) {
            if (item.IsScalar()) {
                result.push_back(item.Scalar());
            }
        }
        return result;
    } else if (y.IsMap()) {
        std::unordered_map<std::string, std::string> result;
        for (const auto& it : y) {
            if (it.first.IsScalar() && it.second.IsScalar()) {
                result[it.first.Scalar()] = it.second.Scalar();
            }
        }
        return result;
    } else {
        throw std::runtime_error("Unsupported YAML type");
    }
}

std::string Config::jsonToString(const json& j) {
    return j.dump();
}

std::string Config::yamlToString(const YAML& y) {
    std::stringstream stream;
    stream << y;
    return stream.str();
}

bool Config::loadFromLegacyFile(const std::string& filepath) {
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

bool Config::loadFromEnvironment(const std::string& prefix) {
    clear();
    
    for (const auto& pair : std::filesystem::environment()) {
        std::string key = pair.first;
        std::string value = pair.second;
        
        // Check if key starts with prefix
        if (key.size() >= prefix.size() && 
            std::equal(prefix.begin(), prefix.end(), key.begin())) {
            
            std::string configKey = key.substr(prefix.size());
            set(configKey, value, ConfigSource::Environment);
        }
    }
    
    return true;
}

bool Config::saveToFile(const std::string& filepath, const std::string& format) const {
    std::string extension = "." + std::filesystem::path(filepath).extension().string();
    std::transform(extension.begin(), extension.end(), extension.begin(), ::tolower);
    
    std::ofstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    if (extension == ".json") {
        return saveToJSON(filepath);
    } else if (extension == ".yaml" || extension == ".yml") {
        return saveToYAML(filepath);
    } else if (extension == ".toml") {
        return saveToTOML(filepath);
    } else {
        // Legacy format
        return saveToLegacyFile(filepath);
    }
}

bool Config::saveToJSON(const std::string& filepath) const {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    json j;
    for (const auto& entry : pImpl->entries) {
        j[entry.key] = configValueToJSON(entry.value);
    }
    
    file << j.dump(2);
    return true;
}

bool Config::saveToYAML(const std::string& filepath) const {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    YAML::Emitter emitter;
    emitter << YAML::BeginMap;
    for (const auto& entry : pImpl->entries) {
        emitter << YAML::Key << entry.key;
        emitter << YAML::Value << configValueToYAML(entry.value);
    }
    emitter << YAML::EndMap;
    
    file << emitter.c_str();
    return true;
}

bool Config::saveToTOML(const std::string& filepath) const {
    // TODO: Implement TOML support
    NLM_LOG_ERROR("TOML save not yet implemented");
    return false;
}

json Config::configValueToJSON(const ConfigValue& value) {
    std::visit([&](auto&& arg) {
        using T = std::decay_t<decltype(arg)>;
        if constexpr (std::is_same_v<T, int>) {
            j = arg;
        } else if constexpr (std::is_same_v<T, int64_t>) {
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
        } else if constexpr (std::is_same_v<T, std::vector<bool>>) {
            j = arg;
        } else if constexpr (std::is_same_v<T, std::unordered_map<std::string, std::string>>) {
            j = arg;
        }
    }, value);
    
    return j;
}

YAML::Node Config::configValueToYAML(const ConfigValue& value) {
    YAML::Node node;
    
    std::visit([&](auto&& arg) {
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
        } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
            for (const auto& item : arg) {
                node.push_back(item);
            }
        } else if constexpr (std::is_same_v<T, std::vector<bool>>) {
            for (const auto& item : arg) {
                node.push_back(static_cast<int>(item));
            }
        } else if constexpr (std::is_same_v<T, std::unordered_map<std::string, std::string>>) {
            for (const auto& pair : arg) {
                node[pair.first] = pair.second;
            }
        }
    }, value);
    
    return node;
}

bool Config::saveToLegacyFile(const std::string& filepath) const {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    for (const auto& entry : pImpl->entries) {
        file << "# " << entry.description << "\n";
        file << entry.key << " = " << valueToString(entry.value) << "\n\n";
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
    // Check schema validation if available
    if (pImpl->schemas.find(key) != pImpl->schemas.end()) {
        ConfigSchema& schema = pImpl->schemas[key];
        
        // Type check
        if (!schema.validator(value)) {
            NLM_LOG_WARNING("Value type mismatch for key " + key + ": expected " + schema.type);
            return;
        }
        
        // Create entry with schema information
        ConfigEntry entry(key, value, source, schema.description, schema.type,
                         schema.validationRegex, schema.minValue, schema.maxValue,
                         schema.required);
        
        if (entry.validate()) {
            // Update or add entry
            auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
                [&key](const ConfigEntry& e) { return e.key == key; });
            
            if (it != pImpl->entries.end()) {
                *it = entry;
            } else {
                pImpl->entries.push_back(entry);
            }
        } else {
            NLM_LOG_ERROR("Configuration validation failed for key " + key);
        }
    } else {
        // No schema validation, add entry directly
        auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
            [&key](const ConfigEntry& e) { return e.key == key; });
        
        if (it != pImpl->entries.end()) {
            it->value = value;
            it->source = source;
        } else {
            pImpl->entries.emplace_back(key, value, source);
        }
    }
}

void Config::set(const std::string& key, const std::string& value, ConfigSource source) {
    set(key, ConfigValue(value), source);
}

void Config::set(const std::string& key, int value, ConfigSource source) {
    setInt(key, value, source);
}

void Config::set(const std::string& key, int64_t value, ConfigSource source) {
    setInt(key, value, source);
}

void Config::set(const std::string& key, double value, ConfigSource source) {
    setDouble(key, value, source);
}

void Config::set(const std::string& key, bool value, ConfigSource source) {
    setBool(key, value, source);
}

void Config::setInt(const std::string& key, int value, ConfigSource source) {
    ConfigValue configValue = value;
    set(key, configValue, source);
}

void Config::setDouble(const std::string& key, double value, ConfigSource source) {
    ConfigValue configValue = value;
    set(key, configValue, source);
}

void Config::setString(const std::string& key, const std::string& value, ConfigSource source) {
    ConfigValue configValue = value;
    set(key, configValue, source);
}

void Config::setBool(const std::string& key, bool value, ConfigSource source) {
    ConfigValue configValue = value;
    set(key, configValue, source);
}

void Config::setArray(const std::string& key, const std::vector<std::string>& value, ConfigSource source) {
    ConfigValue configValue = value;
    set(key, configValue, source);
}

bool Config::has(const std::string& key) const {
    return std::any_of(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
}

std::string Config::getType(const std::string& key) const {
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    
    if (it == pImpl->entries.end()) {
        return "unknown";
    }
    
    if (std::holds_alternative<int>(it->value)) return "int";
    if (std::holds_alternative<int64_t>(it->value)) return "int64";
    if (std::holds_alternative<double>(it->value)) return "float";
    if (std::holds_alternative<bool>(it->value)) return "bool";
    if (std::holds_alternative<std::string>(it->value)) return "string";
    if (std::holds_alternative<std::vector<int>>(it->value)) return "int-array";
    if (std::holds_alternative<std::vector<double>>(it->value)) return "float-array";
    if (std::holds_alternative<std::vector<std::string>>(it->value)) return "string-array";
    if (std::holds_alternative<std::vector<bool>>(it->value)) return "bool-array";
    if (std::holds_alternative<std::unordered_map<std::string, std::string>>(it->value)) return "object";
    
    return "unknown";
}

void Config::remove(const std::string& key) {
    pImpl->entries.erase(
        std::remove_if(pImpl->entries.begin(), pImpl->entries.end(),
            [&key](const ConfigEntry& e) { return e.key == key; }),
        pImpl->entries.end()
    );
    
    // Also remove schema if present
    pImpl->schemas.erase(key);
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

bool Config::validate() const {
    for (const auto& entry : pImpl->entries) {
        if (!entry.validate()) {
            NLM_LOG_ERROR("Validation failed for key: " + entry.key);
            return false;
        }
    }
    return validateSchema();
}

bool Config::validateSchema() const {
    for (const auto& schema : pImpl->schemas) {
        if (schema.second.required && !has(schema.first)) {
            NLM_LOG_ERROR("Required config key missing: " + schema.first);
            return false;
        }
        
        if (has(schema.first)) {
            auto entry = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
                [&schema](const ConfigEntry& e) { return e.key == schema.first; });
            
            if (entry != pImpl->entries.end() && !schema.second.validator(entry->value)) {
                NLM_LOG_ERROR("Schema validation failed for key: " + schema.first);
                return false;
            }
        }
    }
    return true;
}

std::string Config::summary() const {
    std::ostringstream oss;
    oss << "Configuration (" << pImpl->entries.size() << " entries, " << pImpl->schemas.size() << " schemas):\n";
    oss << "\nEntries:";
    for (const auto& entry : pImpl->entries) {
        oss << "\n  " << entry.key << " = [";
        oss << valueToString(entry.value);
        oss << "] (source: " << static_cast<int>(entry.source) << ")";
        if (!entry.description.empty()) {
            oss << " - " << entry.description;
        }
    }
    
    oss << "\n\nSchemas:";
    for (const auto& schema : pImpl->schemas) {
        oss << "\n  " << schema.first << ": type=" << schema.second.type;
        if (schema.second.required) oss << ", required";
        if (!schema.second.description.empty()) oss << " - " << schema.second.description;
    }
    
    return oss.str();
}

std::vector<ConfigEntry> Config::getEntriesBySource(ConfigSource source) const {
    std::vector<ConfigEntry> result;
    for (const auto& entry : pImpl->entries) {
        if (entry.source == source) {
            result.push_back(entry);
        }
    }
    return result;
}

void Config::merge(const Config& other, ConfigSource source) {
    for (const auto& entry : other.pImpl->entries) {
        // Create new entry with merged source
        ConfigEntry mergedEntry(entry.key, entry.value, source, entry.description,
                               entry.typeHint, entry.validationRegex,
                               entry.minValue, entry.maxValue, entry.required);
        
        // Update if key already exists, otherwise add new
        auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
            [&entry](const ConfigEntry& e) { return e.key == entry.key; });
        
        if (it != pImpl->entries.end()) {
            *it = mergedEntry;
        } else {
            pImpl->entries.push_back(mergedEntry);
        }
    }
}

std::string Config::debugInfo() const {
    std::ostringstream oss;
    oss << "=== Configuration Debug Info ===\n";
    oss << "Total entries: " << pImpl->entries.size() << "\n";
    oss << "Total schemas: " << pImpl->schemas.size() << "\n\n";
    
    oss << "Entries by source:";
    for (ConfigSource src : {ConfigSource::Default, ConfigSource::File, ConfigSource::CommandLine, ConfigSource::Runtime, ConfigSource::Environment}) {
        auto entries = getEntriesBySource(src);
        if (!entries.empty()) {
            oss << "\n  " << static_cast<int>(src) << ": " << entries.size() << " entries";
        }
    }
    
    return oss.str();
}

void Config::setDefault(const std::string& key, const ConfigValue& value, const std::string& description, const std::string& type) {
    set(key, value, ConfigSource::Default);
    
    // Add schema if type is specified
    if (!type.empty()) {
        ConfigSchema schema;
        schema.key = key;
        schema.type = type;
        schema.required = false;
        schema.description = description;
        schema.validator = [type](const ConfigValue& v) {
            if (type == "int" && std::holds_alternative<int>(v)) return true;
            if (type == "float" && std::holds_alternative<double>(v)) return true;
            if (type == "string" && std::holds_alternative<std::string>(v)) return true;
            if (type == "bool" && std::holds_alternative<bool>(v)) return true;
            return false;
        };
        
        pImpl->schemas[key] = schema;
    }
}

void Config::addSchema(const ConfigSchema& schema) {
    pImpl->schemas[schema.key] = schema;
}

void Config::removeSchema(const std::string& key) {
    pImpl->schemas.erase(key);
}

bool Config::hasSchema(const std::string& key) const {
    return pImpl->schemas.find(key) != pImpl->schemas.end();
}

bool Config::validateEntry(const ConfigEntry& entry) const {
    return entry.validate();
}

std::string Config::getDefaultDescription(const std::string& key) const {
    // Return a default description based on key
    if (key == "random_seed") return "Random seed for reproducible experiments";
    if (key == "neuron_count") return "Number of neurons in the brain";
    if (key == "region_count") return "Number of brain regions";
    if (key == "connection_probability") return "Connection probability (0.0-1.0)";
    if (key == "simulation_timestep") return "Simulation timestep in seconds";
    return "Configuration parameter";
}

std::string Config::valueToString(const ConfigValue& value) const {
    std::ostringstream oss;
    
    if (std::holds_alternative<int>(value)) {
        oss << std::get<int>(value);
    } else if (std::holds_alternative<int64_t>(value)) {
        oss << std::get<int64_t>(value);
    } else if (std::holds_alternative<double>(value)) {
        oss << std::get<double>(value);
    } else if (std::holds_alternative<bool>(value)) {
        oss << (std::get<bool>(value) ? "true" : "false");
    } else if (std::holds_alternative<std::string>(value)) {
        oss << "\"" << std::get<std::string>(value) << "\"";
    } else if (std::holds_alternative<std::vector<int>>(value)) {
        oss << "[";
        const auto& vec = std::get<std::vector<int>>(value);
        for (size_t i = 0; i < vec.size(); ++i) {
            if (i > 0) oss << ", ";
            oss << vec[i];
        }
        oss << "]";
    } else if (std::holds_alternative<std::vector<double>>(value)) {
        oss << "[";
        const auto& vec = std::get<std::vector<double>>(value);
        for (size_t i = 0; i < vec.size(); ++i) {
            if (i > 0) oss << ", ";
            oss << vec[i];
        }
        oss << "]";
    } else if (std::holds_alternative<std::vector<std::string>>(value)) {
        oss << "[";
        const auto& vec = std::get<std::vector<std::string>>(value);
        for (size_t i = 0; i < vec.size(); ++i) {
            if (i > 0) oss << ", ";
            oss << "\"" << vec[i] << "\"";
        }
        oss << "]";
    } else if (std::holds_alternative<std::vector<bool>>(value)) {
        oss << "[";
        const auto& vec = std::get<std::vector<bool>>(value);
        for (size_t i = 0; i < vec.size(); ++i) {
            if (i > 0) oss << ", ";
            oss << (vec[i] ? "true" : "false");
        }
        oss << "]";
    } else if (std::holds_alternative<std::unordered_map<std::string, std::string>>(value)) {
        oss << "{";
        const auto& map = std::get<std::unordered_map<std::string, std::string>>(value);
        for (auto it = map.begin(); it != map.end(); ++it) {
            if (it != map.begin()) oss << ", ";
            oss << "\"" << it->first << "\": \"" << it->second << "\"";
        }
        oss << "}";
    }
    
    return oss.str();
}

// Explicit template instantiations

template std::optional<int> Config::get<int>(const std::string&) const;
template std::optional<int64_t> Config::get<int64_t>(const std::string&) const;
template std::optional<double> Config::get<double>(const std::string&) const;
template std::optional<bool> Config::get<bool>(const std::string&) const;
template std::optional<std::string> Config::get<std::string>(const std::string&) const;
template std::optional<std::vector<int>> Config::get<std::vector<int>>(const std::string&) const;
template std::optional<std::vector<double>> Config::get<std::vector<double>>(const std::string&) const;
template std::optional<std::vector<std::string>> Config::get<std::vector<std::string>>(const std::string&) const;
template std::optional<std::vector<bool>> Config::get<std::vector<bool>>(const std::string&) const;
template std::optional<std::unordered_map<std::string, std::string>> Config::get<std::unordered_map<std::string, std::string>>(const std::string&) const;

template int Config::getOr<int>(const std::string&, const int&) const;
template int64_t Config::getOr<int64_t>(const std::string&, const int64_t&) const;
template double Config::getOr<double>(const std::string&, const double&) const;
template bool Config::getOr<bool>(const std::string&, const bool&) const;
template std::string Config::getOr<std::string>(const std::string&, const std::string&) const;

} // namespace nlm

