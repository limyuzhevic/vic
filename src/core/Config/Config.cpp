#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <yaml-cpp/yaml.h>

namespace nlm {

struct Config::Impl {
    std::vector<ConfigEntry> entries;
    std::vector<ConfigValidationRule> schema;
    bool validateOnSet = false;
};

Config::Config() : pImpl(std::make_unique<Impl>()) {}

Config::~Config() = default;

Config::Config(Config&&) noexcept = default;

Config& Config::operator=(Config&&) noexcept = default;

bool Config::loadFromFile(const std::string& filepath) {
    // Detect file format based on extension
    std::string ext = filepath.substr(filepath.find_last_of('.') + 1);
    std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);
    
    if (ext == "json") {
        // Try to parse as JSON
        std::ifstream file(filepath);
        if (!file.is_open()) {
            return false;
        }
        
        json j;
        try {
            file >> j;
            return fromJson(j);
        } catch (const json::parse_error& e) {
            NLM_LOG_ERROR("JSON parse error in " + filepath + ": " + e.what());
            return false;
        }
    } else if (ext == "yaml" || ext == "yml") {
        // Try to parse as YAML
        try {
            YAML::Node y = YAML::LoadFile(filepath);
            json j;
            // Convert YAML to JSON
            if (y.IsMap()) {
                for (const auto& pair : y) {
                    j[pair.first.as<std::string>()] = pair.second;
                }
            }
            return fromJson(j);
        } catch (const YAML::Exception& e) {
            NLM_LOG_ERROR("YAML parse error in " + filepath + ": " + e.what());
            return false;
        }
    } else {
        // Fallback to simple key=value format for backward compatibility
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

bool Config::saveToFile(const std::string& filepath, bool pretty) const {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    json j;
    
    for (const auto& entry : pImpl->entries) {
        try {
            j[entry.key] = valueToJson(entry.value);
        } catch (const std::exception& e) {
            NLM_LOG_ERROR("Failed to serialize config value for key " + entry.key + ": " + e.what());
            continue;
        }
    }
    
    if (pretty) {
        file << j.dump(4);
    } else {
        file << j.dump();
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
    // Validate against schema if validation is enabled
    if (pImpl->validateOnSet && !pImpl->schema.empty()) {
        bool valid = true;
        for (const auto& rule : pImpl->schema) {
            if (rule.key == key) {
                if (!ConfigSchema::validateValue(value, rule.type)) {
                    NLM_LOG_ERROR("Configuration validation failed for key '" + key + "': " + 
                                "expected " + ConfigSchema::getTypeName(rule.type) + 
                                " but got invalid type");
                    valid = false;
                    break;
                }
                
                if (rule.allowedValues.has_value()) {
                    auto strValue = std::visit([](auto&& arg) -> std::string {
                        if constexpr (std::is_same_v<decltype(arg), std::string>) {
                            return arg;
                        } else {
                            return "";
                        }
                    }, value);
                    
                    if (std::find(rule.allowedValues->begin(), rule.allowedValues->end(), strValue) == 
                        rule.allowedValues->end()) {
                        NLM_LOG_ERROR("Configuration validation failed for key '" + key + 
                                    "': value '" + strValue + "' not in allowed values");
                        valid = false;
                        break;
                    }
                }
                
                if (rule.customValidator && !rule.customValidator(value)) {
                    NLM_LOG_ERROR("Configuration validation failed for key '" + key + ": " + 
                                "custom validator returned false");
                    valid = false;
                    break;
                }
                break;
            }
        }
        
        if (!valid) {
            return; // Skip setting invalid value
        }
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

json Config::toJson() const {
    json j;
    
    for (const auto& entry : pImpl->entries) {
        try {
            j[entry.key] = valueToJson(entry.value);
        } catch (const std::exception& e) {
            NLM_LOG_ERROR("Failed to serialize config value for key " + entry.key + ": " + e.what());
        }
    }
    
    return j;
}

bool Config::fromJson(const json& j) {
    if (!j.is_object()) {
        NLM_LOG_ERROR("JSON configuration must be an object");
        return false;
    }
    
    clear();
    
    for (const auto& [key, value] : j.items()) {
        ConfigValue configValue;
        if (jsonToValue(value, configValue)) {
            set(key, configValue, ConfigSource::File);
        } else {
            NLM_LOG_ERROR("Failed to parse JSON value for key " + key);
        }
    }
    
    return true;
}

bool Config::validateSchema(const std::vector<ConfigValidationRule>& schema) const {
    pImpl->schema = schema;
    pImpl->validateOnSet = true;
    
    // Check if all required keys exist
    for (const auto& rule : schema) {
        if (rule.defaultValue.has_value() && !has(rule.key)) {
            NLM_LOG_WARNING("Configuration missing required key: " + rule.key);
        }
    }
    
    return true;
}

void Config::setSchema(const std::vector<ConfigValidationRule>& schema) {
    pImpl->schema = schema;
    pImpl->validateOnSet = !schema.empty();
}

std::map<std::string, std::string> Config::parseSimpleFormat(const std::string& content) {
    std::map<std::string, std::string> result;
    
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
            
            // Remove quotes if present
            if (value.size() >= 2 && 
                ((value.front() == '"' && value.back() == '"') ||
                 (value.front() == '\'' && value.back() == '\''))) {
                value = value.substr(1, value.size() - 2);
            }
            
            result[key] = value;
        }
    }
    
    return result;
}

std::string Config::serializeSimpleFormat() const {
    std::ostringstream oss;
    oss << "# NLM Configuration File\n";
    
    for (const auto& entry : pImpl->entries) {
        oss << "# " << entry.description << "\n";
        std::visit([&oss](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, std::string>) {
                oss << entry.key << " = \"" << arg << "\"\n";
            } else if constexpr (std::is_same_v<T, bool>) {
                oss << entry.key << " = " << (arg ? "true" : "false") << "\n";
            } else {
                oss << entry.key << " = " << arg << "\n";
            }
        }, entry.value);
        oss << "\n";
    }
    
    return oss.str();
}

json Config::valueToJson(const ConfigValue& value) {
    json j;
    std::visit([&j](auto&& arg) {
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
        }
    }, value);
    return j;
}

bool Config::jsonToValue(const json& j, ConfigValue& value) {
    if (j.is_number_integer()) {
        value = j.get<int>();
        return true;
    } else if (j.is_number_unsigned()) {
        value = j.get<uint64_t>();
        return true;
    } else if (j.is_number()) {
        // Check if it's actually an integer
        if (j.is_number_integer()) {
            value = j.get<int64_t>();
        } else {
            value = j.get<double>();
        }
        return true;
    } else if (j.is_boolean()) {
        value = j.get<bool>();
        return true;
    } else if (j.is_string()) {
        value = j.get<std::string>();
        return true;
    } else if (j.is_array()) {
        // Check array element type
        if (!j.empty()) {
            const auto& first = j[0];
            if (first.is_number_integer()) {
                std::vector<int> vec = j.get<std::vector<int>>();
                value = vec;
                return true;
            } else if (first.is_number()) {
                std::vector<double> vec = j.get<std::vector<double>>();
                value = vec;
                return true;
            } else if (first.is_string()) {
                std::vector<std::string> vec = j.get<std::vector<std::string>>();
                value = vec;
                return true;
            }
        } else {
            // Empty array - use int vector as default
            value = std::vector<int>();
            return true;
        }
    }
    
    return false;
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

// ConfigSchema implementation
ConfigValidationRule ConfigSchema::createRule(const std::string& key, ConfigDataType type,
                                               const std::string& description,
                                               const std::optional<std::string>& defaultValue,
                                               const std::optional<std::vector<std::string>>& allowedValues) {
    ConfigValidationRule rule;
    rule.key = key;
    rule.type = type;
    rule.defaultValue = defaultValue;
    rule.allowedValues = allowedValues;
    rule.description = description;
    return rule;
}

bool ConfigSchema::validateValue(const ConfigValue& value, ConfigDataType type) {
    switch (type) {
        case ConfigDataType::Int:
            return std::holds_alternative<int>(value);
        case ConfigDataType::Int64:
            return std::holds_alternative<int64_t>(value);
        case ConfigDataType::Double:
            return std::holds_alternative<double>(value);
        case ConfigDataType::Bool:
            return std::holds_alternative<bool>(value);
        case ConfigDataType::String:
            return std::holds_alternative<std::string>(value);
        case ConfigDataType::IntArray:
            return std::holds_alternative<std::vector<int>>(value);
        case ConfigDataType::DoubleArray:
            return std::holds_alternative<std::vector<double>>(value);
        case ConfigDataType::StringArray:
            return std::holds_alternative<std::vector<std::string>>(value);
        default:
            return false;
    }
}

std::string ConfigSchema::getTypeName(ConfigDataType type) {
    switch (type) {
        case ConfigDataType::Int: return "int";
        case ConfigDataType::Int64: return "int64_t";
        case ConfigDataType::Double: return "double";
        case ConfigDataType::Bool: return "bool";
        case ConfigDataType::String: return "string";
        case ConfigDataType::IntArray: return "vector<int>";
        case ConfigDataType::DoubleArray: return "vector<double>";
        case ConfigDataType::StringArray: return "vector<string>";
        default: return "unknown";
    }
}

} // namespace nlm
