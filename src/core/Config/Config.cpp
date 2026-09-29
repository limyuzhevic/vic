#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <nlohmann/json.hpp>
#include <yaml-cpp/yaml.h>
#include <stdexcept>

namespace nlm {

struct Config::Impl {
    std::vector<ConfigEntry> entries;
    std::unordered_map<std::string, std::string> nestedKeys;
};

Config::Config() : pImpl(std::make_unique<Impl>()) {}

Config::~Config() = default;

Config::Config(Config&&) noexcept = default;

Config& Config::operator=(Config&&) noexcept = default;

bool Config::loadFromFile(const std::string& filepath) {
    // Check file extension to determine format
    std::filesystem::path path(filepath);
    std::string ext = path.extension().string();
    
    if (ext == ".json" || ext == ".JSON") {
        return loadFromJsonFile(filepath);
    } else if (ext == ".yaml" || ext == ".yml" || ext == ".YAML" || ext == ".YML") {
        return loadFromYamlFile(filepath);
    } else {
        // Fall back to legacy key=value format
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
}

bool Config::loadFromJson(const std::string& jsonString) {
    try {
        auto json = nlohmann::json::parse(jsonString);
        pImpl->entries.clear();
        
        // Process JSON object
        if (json.is_object()) {
            for (auto& [key, value] : json.items()) {
                ConfigValue configValue = jsonToConfigValue(value);
                set(key, configValue, ConfigSource::File);
            }
        } else {
            setLastError("JSON root must be an object");
            return false;
        }
        
        return true;
    } catch (const nlohmann::json::parse_error& e) {
        setLastError("JSON parse error: " + std::string(e.what()));
        return false;
    } catch (const std::exception& e) {
        setLastError("JSON error: " + std::string(e.what()));
        return false;
    }
}

bool Config::loadFromJsonFile(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        setLastError("Cannot open JSON file: " + filepath);
        return false;
    }
    
    std::stringstream buffer;
    buffer << file.rdbuf();
    
    return loadFromJson(buffer.str());
}

bool Config::loadFromYaml(const std::string& yamlString) {
    try {
        auto yaml = YAML::Load(yamlString);
        pImpl->entries.clear();
        
        // Process YAML mapping
        if (yaml.IsMap()) {
            for (auto it = yaml.begin(); it != yaml.end(); ++it) {
                std::string key = it->first.as<std::string>();
                ConfigValue configValue = yamlToConfigValue(*it);
                set(key, configValue, ConfigSource::File);
            }
        } else {
            setLastError("YAML root must be a mapping");
            return false;
        }
        
        return true;
    } catch (const YAML::ParserException& e) {
        setLastError("YAML parse error: " + std::string(e.what()));
        return false;
    } catch (const std::exception& e) {
        setLastError("YAML error: " + std::string(e.what()));
        return false;
    }
}

bool Config::loadFromYamlFile(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        setLastError("Cannot open YAML file: " + filepath);
        return false;
    }
    
    std::stringstream buffer;
    buffer << file.rdbuf();
    
    return loadFromYaml(buffer.str());
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
    // Check file extension to determine format
    std::filesystem::path path(filepath);
    std::string ext = path.extension().string();
    
    if (ext == ".json" || ext == ".JSON") {
        return saveToJsonFile(filepath);
    } else if (ext == ".yaml" || ext == ".yml" || ext == ".YAML" || ext == ".YML") {
        return saveToYamlFile(filepath);
    } else {
        // Fall back to legacy key=value format
        std::ofstream file(filepath);
        if (!file.is_open()) {
            return false;
        }
        
        for (const auto& entry : pImpl->entries) {
            file << "# " << entry.description << "\n";
            file << entry.key << " = ";
            
            std::visit([&file](auto&& arg) {
                using T = std::decay_t<decltype(arg)>;
                if constexpr (std::is_same_v<T, std::string>) {
                    file << "\"" << arg << "\"";
                } else {
                    file << arg;
                }
            }, entry.value);
            
            file << "\n\n";
        }
        
        return true;
    }
}

bool Config::saveToJsonFile(const std::string& filepath) const {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        setLastError("Cannot open JSON file for writing: " + filepath);
        return false;
    }
    
    nlohmann::json json = nlohmann::json::object();
    for (const auto& entry : pImpl->entries) {
        json[entry.key] = configValueToJson(entry.value);
    }
    
    file << json.dump(2);
    return true;
}

bool Config::saveToYamlFile(const std::string& filepath) const {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        setLastError("Cannot open YAML file for writing: " + filepath);
        return false;
    }
    
    YAML::Emitter yaml;
    yaml << YAML::BeginMap;
    
    for (const auto& entry : pImpl->entries) {
        yaml << YAML::Key << entry.key;
        yaml << YAML::Value << configValueToYaml(entry.value);
    }
    
    yaml << YAML::EndMap;
    
    file << yaml.c_str();
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

std::string Config::saveToJson() const {
    return configToJson(pImpl->entries);
}

bool Config::setValidated(const std::string& key, const ConfigValue& value, ConfigSource source) {
    ValidationResult validation = validateField(key, ConfigSchema("test"));
    if (!validation.isValid()) {
        setLastError("Validation failed for key " + key + ": " + validation.toString());
        return false;
    }
    
    set(key, value, source);
    return true;
}

template<typename T>
std::optional<T> Config::getNested(const std::vector<std::string>& keys) const {
    if (keys.empty()) return std::nullopt;
    
    std::string currentKey = keys[0];
    auto entry = getEntry(currentKey);
    if (!entry) return std::nullopt;
    
    if (keys.size() == 1) {
        return get<T>(currentKey);
    }
    
    // Check if value is a nested structure (we need to handle nested values)
    // For now, we'll just return the value if it's the right type
    if constexpr (std::is_same_v<T, std::string>) {
        return std::get<std::string>(entry->value);
    } else if constexpr (std::is_same_v<T, int>) {
        return std::get<int>(entry->value);
    } else if constexpr (std::is_same_v<T, double>) {
        return std::get<double>(entry->value);
    } else if constexpr (std::is_same_v<T, bool>) {
        return std::get<bool>(entry->value);
    }
    
    return std::nullopt;
}

template<typename T>
bool Config::setNested(const std::vector<std::string>& keys, const T& value, ConfigSource source) {
    if (keys.empty()) return false;
    
    std::string key = keys[0];
    
    if (keys.size() == 1) {
        set(key, value, source);
        return true;
    }
    
    // For nested keys, we'll treat them as a single key with dot notation
    // This is the legacy format support
    setNestedValue(key, value, source);
    return true;
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

std::vector<ConfigEntry> Config::getEntries() const {
    return pImpl->entries;
}

void Config::clear() {
    pImpl->entries.clear();
    pImpl->nestedKeys.clear();
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

ValidationResult Config::validateSchema(const ConfigSchema& schema) const {
    return schema.validate(pImpl->entries);
}

bool Config::validateField(const std::string& key, const ConfigSchema& schema) const {
    auto entry = getEntry(key);
    if (!entry) return false;
    
    std::vector<std::string> errors;
    errors = validateFieldType(*entry);
    if (!errors.empty()) return false;
    
    errors = validateFieldRange(*entry);
    if (!errors.empty()) return false;
    
    return true;
}

bool Config::isFieldOfType(const std::string& key, const std::string& typeName) const {
    auto entry = getEntry(key);
    if (!entry) return false;
    
    std::string expectedType = getFieldType(key);
    return expectedType == typeName;
}

std::string Config::getFieldType(const std::string& key) const {
    auto entry = getEntry(key);
    if (!entry) return "unknown";
    
    return getTypeNameFromValue(entry->value);
}

bool Config::isFieldInRange(const std::string& key, double min, double max) const {
    auto entry = getEntry(key);
    if (!entry) return false;
    
    std::visit([this, key, min, max](auto&& arg) {
        using T = std::decay_t<decltype(arg)>;
        if constexpr (std::is_same_v<T, double> || std::is_same_v<T, int>) {
            if (arg < min || arg > max) {
                setLastError("Value out of range for key " + key + ": " + std::to_string(arg) + 
                           " is not in range [" + std::to_string(min) + ", " + std::to_string(max) + "]");
            }
        }
    }, entry->value);
    
    return !getLastError().empty();
}

std::optional<ConfigEntry> Config::getEntry(const std::string& key) const {
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    
    if (it == pImpl->entries.end()) {
        return std::nullopt;
    }
    
    return *it;
}

bool Config::mergeFrom(const Config& other, bool override) {
    bool success = true;
    
    for (const auto& entry : other.getEntries()) {
        if (has(entry.key) && !override) {
            continue;
        }
        
        set(entry.key, entry.value, ConfigSource::File);
    }
    
    return success;
}

bool Config::inheritFrom(const Config& parent, const std::vector<std::string>& allowedKeys) {
    bool success = true;
    
    for (const auto& entry : parent.getEntries()) {
        if (!allowedKeys.empty() && 
            std::find(allowedKeys.begin(), allowedKeys.end(), entry.key) == allowedKeys.end()) {
            continue;
        }
        
        set(entry.key, entry.value, ConfigSource::File);
    }
    
    return success;
}

std::string Config::toJsonString(const ConfigValue& value) {
    return configValueToJson(value).dump();
}

ConfigValue Config::fromJsonValue(const std::string& jsonString) {
    auto json = nlohmann::json::parse(jsonString);
    return jsonToConfigValue(json);
}

nlohmann::json Config::toNlohmannJson(const ConfigValue& value) {
    return configValueToJson(value);
}

std::string Config::configToJson(const std::vector<ConfigEntry>& entries) {
    nlohmann::json json = nlohmann::json::object();
    
    for (const auto& entry : entries) {
        json[entry.key] = configValueToJson(entry.value);
    }
    
    return json.dump(2);
}

std::vector<ConfigEntry> Config::jsonToConfig(const std::string& jsonString, ConfigSource source) {
    std::vector<ConfigEntry> entries;
    
    try {
        auto json = nlohmann::json::parse(jsonString);
        
        if (json.is_object()) {
            for (auto& [key, value] : json.items()) {
                ConfigValue configValue = jsonToConfigValue(value);
                entries.emplace_back(key, configValue, source);
            }
        }
    } catch (const nlohmann::json::parse_error& e) {
        setLastError("JSON parse error: " + std::string(e.what()));
    } catch (const std::exception& e) {
        setLastError("JSON error: " + std::string(e.what()));
    }
    
    return entries;
}

std::vector<ConfigEntry> Config::yamlToConfig(const std::string& yamlString, ConfigSource source) {
    std::vector<ConfigEntry> entries;
    
    try {
        auto yaml = YAML::Load(yamlString);
        
        if (yaml.IsMap()) {
            for (auto it = yaml.begin(); it != yaml.end(); ++it) {
                std::string key = it->first.as<std::string>();
                ConfigValue configValue = yamlToConfigValue(*it);
                entries.emplace_back(key, configValue, source);
            }
        }
    } catch (const YAML::ParserException& e) {
        setLastError("YAML parse error: " + std::string(e.what()));
    } catch (const std::exception& e) {
        setLastError("YAML error: " + std::string(e.what()));
    }
    
    return entries;
}

std::string Config::schemaToJson(const ConfigSchema& schema) {
    nlohmann::json json = nlohmann::json::object();
    
    json["name"] = schema.name;
    json["validateRequired"] = schema.validateRequired;
    json["minVersion"] = schema.minVersion;
    json["maxVersion"] = schema.maxVersion;
    
    nlohmann::json fields = nlohmann::json::object();
    for (const auto& [key, type] : schema.fieldTypes) {
        nlohmann::json field = nlohmann::json::object();
        field["type"] = type;
        field["description"] = schema.fieldDescriptions.at(key);
        field["range"] = schema.fieldRanges.at(key);
        fields[key] = field;
    }
    
    json["fields"] = fields;
    
    return json.dump(2);
}

ConfigSchema Config::schemaFromJson(const std::string& jsonString) {
    ConfigSchema schema("");
    
    try {
        auto json = nlohmann::json::parse(jsonString);
        
        schema.name = json.value("name", "");
        schema.validateRequired = json.value("validateRequired", true);
        schema.minVersion = json.value("minVersion", 0.0);
        schema.maxVersion = json.value("maxVersion", 999.0);
        
        if (json.contains("fields")) {
            for (auto& [key, field] : json["fields"].items()) {
                std::string type = field.value("type", "unknown");
                std::string description = field.value("description", "");
                std::string range = field.value("range", "");
                schema.addField(key, type, description, range);
            }
        }
    } catch (const nlohmann::json::parse_error& e) {
        setLastError("JSON parse error: " + std::string(e.what()));
    } catch (const std::exception& e) {
        setLastError("JSON error: " + std::string(e.what()));
    }
    
    return schema;
}

std::string Config::lastError = "";

std::string Config::getLastError() {
    return lastError;
}

void Config::clearLastError() {
    lastError.clear();
}

void Config::setLastError(const std::string& error) {
    lastError = error;
}

nlohmann::json Config::configValueToJson(const ConfigValue& value) {
    nlohmann::json json;
    
    std::visit([&json](auto&& arg) {
        using T = std::decay_t<decltype(arg)>;
        if constexpr (std::is_same_v<T, int>) {
            json = arg;
        } else if constexpr (std::is_same_v<T, int64_t>) {
            json = arg;
        } else if constexpr (std::is_same_v<T, double>) {
            json = arg;
        } else if constexpr (std::is_same_v<T, bool>) {
            json = arg;
        } else if constexpr (std::is_same_v<T, std::string>) {
            json = arg;
        } else if constexpr (std::is_same_v<T, std::vector<int>>) {
            json = arg;
        } else if constexpr (std::is_same_v<T, std::vector<double>>) {
            json = arg;
        } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
            json = arg;
        }
    }, value);
    
    return json;
}

ConfigValue Config::jsonToConfigValue(const nlohmann::json& jsonValue) {
    ConfigValue configValue;
    
    if (jsonValue.is_number_integer()) {
        int64_t val = jsonValue.get<int64_t>();
        configValue = val;
    } else if (jsonValue.is_number_float()) {
        double val = jsonValue.get<double>();
        configValue = val;
    } else if (jsonValue.is_boolean()) {
        bool val = jsonValue.get<bool>();
        configValue = val;
    } else if (jsonValue.is_string()) {
        std::string val = jsonValue.get<std::string>();
        configValue = val;
    } else if (jsonValue.is_array()) {
        if (!jsonValue.empty()) {
            auto firstType = jsonValue[0].type();
            
            if (firstType == nlohmann::json::value_t::number_integer) {
                std::vector<int> vec;
                for (const auto& item : jsonValue) {
                    vec.push_back(item.get<int>());
                }
                configValue = vec;
            } else if (firstType == nlohmann::json::value_t::number_float) {
                std::vector<double> vec;
                for (const auto& item : jsonValue) {
                    vec.push_back(item.get<double>());
                }
                configValue = vec;
            } else if (firstType == nlohmann::json::value_t::string) {
                std::vector<std::string> vec;
                for (const auto& item : jsonValue) {
                    vec.push_back(item.get<std::string>());
                }
                configValue = vec;
            }
        }
    }
    
    return configValue;
}

YAML::Node Config::configValueToYaml(const ConfigValue& value) {
    YAML::Node yamlNode;
    
    std::visit([&yamlNode](auto&& arg) {
        using T = std::decay_t<decltype(arg)>;
        if constexpr (std::is_same_v<T, int>) {
            yamlNode = arg;
        } else if constexpr (std::is_same_v<T, int64_t>) {
            yamlNode = arg;
        } else if constexpr (std::is_same_v<T, double>) {
            yamlNode = arg;
        } else if constexpr (std::is_same_v<T, bool>) {
            yamlNode = arg;
        } else if constexpr (std::is_same_v<T, std::string>) {
            yamlNode = arg;
        } else if constexpr (std::is_same_v<T, std::vector<int>>) {
            yamlNode = arg;
        } else if constexpr (std::is_same_v<T, std::vector<double>>) {
            yamlNode = arg;
        } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
            yamlNode = arg;
        }
    }, value);
    
    return yamlNode;
}

ConfigValue Config::yamlToConfigValue(const YAML::Node& yamlNode) {
    ConfigValue configValue;
    
    if (yamlNode.IsScalar()) {
        if (yamlNode.IsInt()) {
            int val = yamlNode.as<int>();
            configValue = val;
        } else if (yamlNode.IsFloat()) {
            double val = yamlNode.as<double>();
            configValue = val;
        } else if (yamlNode.IsBool()) {
            bool val = yamlNode.as<bool>();
            configValue = val;
        } else if (yamlNode.IsDouble()) {
            double val = yamlNode.as<double>();
            configValue = val;
        } else if (yamlNode.IsString()) {
            std::string val = yamlNode.as<std::string>();
            configValue = val;
        }
    } else if (yamlNode.IsSequence()) {
        if (!yamlNode.empty()) {
            auto firstType = yamlNode[0].Type();
            
            if (firstType == YAML::NodeType::Int) {
                std::vector<int> vec;
                for (const auto& item : yamlNode) {
                    vec.push_back(item.as<int>());
                }
                configValue = vec;
            } else if (firstType == YAML::NodeType::Float) {
                std::vector<double> vec;
                for (const auto& item : yamlNode) {
                    vec.push_back(item.as<double>());
                }
                configValue = vec;
            } else if (firstType == YAML::NodeType::Scalar) {
                std::vector<std::string> vec;
                for (const auto& item : yamlNode) {
                    vec.push_back(item.as<std::string>());
                }
                configValue = vec;
            }
        }
    }
    
    return configValue;
}

std::vector<std::string> Config::validateFieldType(const ConfigEntry& entry) const {
    std::vector<std::string> errors;
    std::string expectedType = getFieldType(entry.key);
    std::string actualType = getTypeNameFromValue(entry.value);
    
    if (expectedType != "unknown" && expectedType != actualType) {
        errors.push_back("Type mismatch for key " + entry.key + ": expected " + expectedType + 
                       ", got " + actualType);
    }
    
    return errors;
}

std::vector<std::string> Config::validateFieldRange(const ConfigEntry& entry) const {
    std::vector<std::string> errors;
    std::string range = getFieldRange(entry.key);
    
    if (!range.empty()) {
        // Simple range parsing (e.g., "0-100" or "0.0-1.0")
        size_t dashPos = range.find('-');
        if (dashPos != std::string::npos) {
            double min = std::stod(range.substr(0, dashPos));
            double max = std::stod(range.substr(dashPos + 1));
            
            std::visit([&errors, entry, min, max](auto&& arg) {
                using T = std::decay_t<decltype(arg)>;
                if constexpr (std::is_same_v<T, double> || std::is_same_v<T, int>) {
                    if (arg < min || arg > max) {
                        errors.push_back("Value out of range for key " + entry.key + ": " + 
                                       std::to_string(arg) + " is not in range [" + 
                                       std::to_string(min) + ", " + std::to_string(max) + "]");
                    }
                }
            }, entry.value);
        }
    }
    
    return errors;
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

std::vector<std::string> Config::parseKey(const std::string& key) const {
    std::vector<std::string> keys;
    
    // Handle dot notation for nested keys
    size_t pos = 0;
    std::string token;
    
    while ((pos = key.find('.')) != std::string::npos) {
        token = key.substr(0, pos);
        keys.push_back(token);
        key.erase(0, pos + 1);
    }
    
    keys.push_back(key);
    return keys;
}

std::string Config::joinKey(const std::vector<std::string>& keys) const {
    if (keys.empty()) return "";
    
    std::string result = keys[0];
    for (size_t i = 1; i < keys.size(); ++i) {
        result += "." + keys[i];
    }
    
    return result;
}

std::string Config::getTypeNameFromValue(const ConfigValue& value) {
    if (std::holds_alternative<int>(value)) return "int";
    if (std::holds_alternative<int64_t>(value)) return "int64_t";
    if (std::holds_alternative<double>(value)) return "double";
    if (std::holds_alternative<bool>(value)) return "bool";
    if (std::holds_alternative<std::string>(value)) return "string";
    if (std::holds_alternative<std::vector<int>>(value)) return "vector<int>";
    if (std::holds_alternative<std::vector<double>>(value)) return "vector<double>";
    if (std::holds_alternative<std::vector<std::string>>(value)) return "vector<string>";
    return "unknown";
}

void Config::setNestedValue(const std::string& key, const ConfigValue& value, ConfigSource source) {
    // For nested keys, we'll store them as-is
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    
    if (it != pImpl->entries.end()) {
        it->value = value;
        it->source = source;
    } else {
        pImpl->entries.emplace_back(key, value, source);
    }
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