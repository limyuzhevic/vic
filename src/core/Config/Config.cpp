#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <iostream>
#include <vector>
#include <map>

namespace nlm {

struct Config::Impl {
    std::vector<ConfigEntry> entries;
    ParserOptions parserOptions;
    std::vector<ConfigSchema> schema;
};

Config::Config() : pImpl(std::make_unique<Impl>()) {
    // Initialize default parser options
    pImpl->parserOptions.validateSchema = true;
    pImpl->parserOptions.supportNestedKeys = true;
    pImpl->parserOptions.preserveOrder = true;
    pImpl->parserOptions.configSource = "JSON";
}

Config::~Config() = default;

Config::Config(Config&&) noexcept = default;

Config& Config::operator=(Config&&) noexcept = default;

bool Config::loadFromFile(const std::string& filepath) {
    ParserOptions options = pImpl->parserOptions;
    options.configSource = "File";
    return loadFromJSON(filepath, options);
}

// Backward compatibility for key=value format
bool Config::loadFromFileLegacy(const std::string& filepath) {
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
    std::ofstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    saveToJSON(filepath);
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

// Private methods implementation
bool Config::loadFromJSON(const std::string& filepath, const ParserOptions& options) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    std::stringstream buffer;
    buffer << file.rdbuf();
    std::string jsonString = buffer.str();
    
    return loadFromJSONString(jsonString, options);
}

bool Config::loadFromJSONString(const std::string& jsonString, const ParserOptions& options) {
    try {
        nlohmann::json jsonData = nlohmann::json::parse(jsonString);
        
        if (options.validateSchema && !validate(jsonData, pImpl->schema)) {
            std::cerr << "Configuration schema validation failed!" << std::endl;
            return false;
        }
        
        clear();
        flattenAndLoadJSON(jsonData, ConfigSource::File, options);
        
        return true;
    } catch (const nlohmann::json::parse_error& e) {
        std::cerr << "JSON parse error: " << e.what() << std::endl;
        return false;
    } catch (const std::exception& e) {
        std::cerr << "Error loading configuration: " << e.what() << std::endl;
        return false;
    }
}

void Config::flattenAndLoadJSON(const nlohmann::json& json, ConfigSource source, const ParserOptions& options) {
    if (options.supportNestedKeys) {
        std::map<std::string, nlohmann::json> flattened;
        flatten(json, "", flattened);
        for (const auto& pair : flattened) {
            setFromJSONValue(pair.first, pair.second, source);
        }
    } else {
        for (const auto& item : json.items()) {
            std::string key = item.key();
            setFromJSONValue(key, item.value(), source);
        }
    }
}

std::string Config::flattenKey(const std::string& key, const nlohmann::json& parent) const {
    return key;
}

void Config::setFromJSONValue(const std::string& key, const nlohmann::json& value, ConfigSource source) {
    ConfigValue configValue;
    
    if (value.is_string()) {
        configValue = value.get<std::string>();
    } else if (value.is_number_integer()) {
        configValue = value.get<int>();
    } else if (value.is_number_unsigned()) {
        configValue = value.get<uint64_t>();
    } else if (value.is_number()) {
        configValue = value.get<double>();
    } else if (value.is_boolean()) {
        configValue = value.get<bool>();
    } else if (value.is_array()) {
        if (value.empty()) {
            configValue = std::vector<int>{};
        } else if (value.front().is_number_integer()) {
            configValue = value.get<std::vector<int>>();
        } else if (value.front().is_number()) {
            configValue = value.get<std::vector<double>>();
        } else {
            configValue = value.get<std::vector<std::string>>();
        }
    } else if (value.is_object()) {
        std::string jsonStr = value.dump();
        configValue = jsonStr;
    } else {
        throw std::runtime_error("Unsupported JSON value type for key: " + key);
    }
    
    set(key, configValue, source);
}

// Schema validation
bool Config::validate(const nlohmann::json& config, std::vector<ConfigValidationError>& errors) {
    // This would be expanded with actual schema validation logic
    return true;
}

// JSON conversion
bool Config::jsonToConfigValue(const nlohmann::json& jsonValue, ConfigValue& configValue) {
    if (jsonValue.is_string()) {
        configValue = jsonValue.get<std::string>();
        return true;
    } else if (jsonValue.is_number_integer()) {
        configValue = jsonValue.get<int>();
        return true;
    } else if (jsonValue.is_number_unsigned()) {
        configValue = jsonValue.get<uint64_t>();
        return true;
    } else if (jsonValue.is_number()) {
        configValue = jsonValue.get<double>();
        return true;
    } else if (jsonValue.is_boolean()) {
        configValue = jsonValue.get<bool>();
        return true;
    } else if (jsonValue.is_array()) {
        if (jsonValue.empty()) {
            configValue = std::vector<int>{};
            return true;
        } else if (jsonValue.front().is_number_integer()) {
            configValue = jsonValue.get<std::vector<int>>();
            return true;
        } else if (jsonValue.front().is_number()) {
            configValue = jsonValue.get<std::vector<double>>();
            return true;
        } else {
            configValue = jsonValue.get<std::vector<std::string>>();
            return true;
        }
    }
    
    return false;
}

nlohmann::json Config::configValueToJson(const ConfigValue& configValue) {
    nlohmann::json jsonValue;
    
    std::visit([&jsonValue](auto&& arg) {
        using T = std::decay_t<decltype(arg)>;
        if constexpr (std::is_same_v<T, int> || std::is_same_v<T, int64_t>) {
            jsonValue = arg;
        } else if constexpr (std::is_same_v<T, double>) {
            jsonValue = arg;
        } else if constexpr (std::is_same_v<T, bool>) {
            jsonValue = arg;
        } else if constexpr (std::is_same_v<T, std::string>) {
            jsonValue = arg;
        } else if constexpr (std::is_same_v<T, std::vector<int>>) {
            jsonValue = arg;
        } else if constexpr (std::is_same_v<T, std::vector<double>>) {
            jsonValue = arg;
        } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
            jsonValue = arg;
        }
    }, configValue);
    
    return jsonValue;
}

// Error handling
std::string Config::getErrorString(ConfigError::Type type) {
    switch (type) {
        case ConfigError::Type::ParseError:
            return "Configuration parse error";
        case ConfigError::Type::ValidationError:
            return "Configuration validation error";
        case ConfigError::Type::ConversionError:
            return "Configuration conversion error";
        default:
            return "Unknown configuration error";
    }
}

// Parser configuration
void Config::setParserOptions(const ParserOptions& options) {
    pImpl->parserOptions = options;
}

ParserOptions Config::getParserOptions() const {
    return pImpl->parserOptions;
}

// Validation functions
bool Config::validateParameter(const std::string& key, const ConfigValue& value) const {
    return validateType(key, value) && validateRange(key, value);
}

bool Config::validateType(const std::string& key, const ConfigValue& value) const {
    // Implement type validation based on expected types for specific keys
    return true;
}

bool Config::validateRange(const std::string& key, const ConfigValue& value) const {
    // Implement range validation for critical parameters
    return true;
}

// Save helpers
void Config::saveToJSON(const std::string& filepath) const {
    nlohmann::json jsonConfig;
    
    if (pImpl->parserOptions.preserveOrder) {
        for (const auto& entry : pImpl->entries) {
            jsonConfig[entry.key] = configValueToJson(entry.value);
        }
    } else {
        for (const auto& entry : pImpl->entries) {
            jsonConfig[entry.key] = configValueToJson(entry.value);
        }
    }
    
    std::ofstream file(filepath);
    if (file.is_open()) {
        file << jsonConfig.dump(4);
    }
}

// Private helpers
void Config::flatten(const nlohmann::json& json, const std::string& prefix, std::map<std::string, nlohmann::json>& flattened) {
    for (const auto& item : json.items()) {
        std::string key = prefix.empty() ? item.key() : prefix + "." + item.key();
        if (item.value().is_object()) {
            flatten(item.value(), key, flattened);
        } else if (item.value().is_array()) {
            flattened[key] = item.value();
        } else {
            flattened[key] = item.value();
        }
    }
}
