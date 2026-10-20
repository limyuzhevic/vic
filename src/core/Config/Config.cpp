#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <regex>
#include <cctype>

// Include JSON and YAML libraries
#ifdef __GNUC__
    #include <nlohmann/json.hpp>
#else
    #include "json.hpp"
#endif

#ifdef __GNUC__
    #include <yaml-cpp/yaml.h>
#else
    #include "yaml-cpp/yaml.h"
#endif

namespace nlm {

struct Config::Impl {
    std::vector<ConfigEntry> entries;
};

Config::Config() : pImpl(std::make_unique<Impl>()) {}

Config::~Config() = default;

Config::Config(Config&&) noexcept = default;

Config& Config::operator=(Config&&) noexcept = default;

bool Config::loadFromFile(const std::string& filepath) {
    // TODO PHASE 2: Implement proper JSON/YAML parser
    // PLACEHOLDER - Phase 1 uses a simple key=value format
    
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

// Helper functions implementation
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

std::string Config::formatValue(const ConfigValue& value) const {
    std::ostringstream oss;
    std::visit([&oss](auto&& arg) {
        using T = std::decay_t<decltype(arg)>;
        if constexpr (std::is_same_v<T, std::string>) {
            oss << "\"" << arg << "\"";
        } else if constexpr (std::is_same_v<T, std::vector<int>>) {
            oss << "[";
            for (size_t i = 0; i < arg.size(); ++i) {
                if (i > 0) oss << ", ";
                oss << arg[i];
            }
            oss << "]";
        } else if constexpr (std::is_same_v<T, std::vector<double>>) {
            oss << "[";
            for (size_t i = 0; i < arg.size(); ++i) {
                if (i > 0) oss << ", ";
                oss << arg[i];
            }
            oss << "]";
        } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
            oss << "[";
            for (size_t i = 0; i < arg.size(); ++i) {
                if (i > 0) oss << ", ";
                oss << "\"" << arg[i] << "\"";
            }
            oss << "]";
        } else {
            oss << arg;
        }
    }, value);
    return oss.str();
}

ConfigValue Config::parseValue(const std::string& valueStr) const {
    if (valueStr.empty()) {
        return std::string();
    }
    
    if (valueStr == "true" || valueStr == "false") {
        return valueStr == "true";
    }
    
    try {
        size_t pos;
        int64_t intVal = std::stoll(valueStr, &pos);
        if (pos == valueStr.size()) {
            return intVal;
        }
    } catch (...) {}
    
    try {
        size_t pos;
        double doubleVal = std::stod(valueStr, &pos);
        if (pos == valueStr.size()) {
            return doubleVal;
        }
    } catch (...) {}
    
    if (valueStr.front() == '[' && valueStr.back() == ']') {
        std::string content = valueStr.substr(1, valueStr.size() - 2);
        std::string result = "[";
        std::string token;
        std::istringstream tokenStream(content);
        
        bool first = true;
        while (std::getline(tokenStream, token, ',')) {
            if (!first) result += ", ";
            result += token;
            first = false;
        }
        result += "]";
        return result;
    }
    
    return valueStr;
}

// JSON/YAML conversion implementations
ConfigValue Config::jsonToConfigValue(const nlohmann::json& jsonVal) {
    if (jsonVal.is_string()) {
        return jsonVal.get<std::string>();
    } else if (jsonVal.is_number_integer()) {
        return jsonVal.get<int64_t>();
    } else if (jsonVal.is_number_float()) {
        return jsonVal.get<double>();
    } else if (jsonVal.is_boolean()) {
        return jsonVal.get<bool>();
    } else if (jsonVal.is_array()) {
        std::vector<std::string> result;
        for (const auto& item : jsonVal) {
            result.push_back(formatValue(jsonToConfigValue(item)));
        }
        return result;
    } else if (jsonVal.is_object()) {
        std::ostringstream oss;
        oss << "{";
        bool first = true;
        for (auto it = jsonVal.begin(); it != jsonVal.end(); ++it) {
            if (!first) oss << ", ";
            oss << "\"" << it.key() << "\": " << it.value();
            first = false;
        }
        oss << "}";
        return oss.str();
    }
    return std::string();
}

nlohmann::json Config::configValueToJSON(const ConfigValue& configVal) const {
    nlohmann::json jsonVal;
    
    std::visit([&jsonVal](auto&& arg) {
        using T = std::decay_t<decltype(arg)>;
        if constexpr (std::is_same_v<T, std::string>) {
            jsonVal = arg;
        } else if constexpr (std::is_same_v<T, int> || std::is_same_v<T, int64_t>) {
            jsonVal = arg;
        } else if constexpr (std::is_same_v<T, double>) {
            jsonVal = arg;
        } else if constexpr (std::is_same_v<T, bool>) {
            jsonVal = arg;
        } else if constexpr (std::is_same_v<T, std::vector<int>>) {
            nlohmann::json array = nlohmann::json::array();
            for (const auto& item : arg) {
                array.push_back(item);
            }
            jsonVal = array;
        } else if constexpr (std::is_same_v<T, std::vector<double>>) {
            nlohmann::json array = nlohmann::json::array();
            for (const auto& item : arg) {
                array.push_back(item);
            }
            jsonVal = array;
        } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
            nlohmann::json array = nlohmann::json::array();
            for (const auto& item : arg) {
                array.push_back(item);
            }
            jsonVal = array;
        }
    }, configVal);
    
    return jsonVal;
}

ConfigValue Config::yamlToConfigValue(const YAML::Node& yamlNode) {
    if (yamlNode.IsScalar()) {
        std::string value = yamlNode.Scalar();
        if (value == "true" || value == "false") {
            return value == "true";
        }
        try {
            size_t pos;
            int64_t intVal = std::stoll(value, &pos);
            if (pos == value.size()) {
                return intVal;
            }
        } catch (...) {}
        try {
            size_t pos;
            double doubleVal = std::stod(value, &pos);
            if (pos == value.size()) {
                return doubleVal;
            }
        } catch (...) {}
        return value;
    } else if (yamlNode.IsSequence()) {
        std::vector<std::string> result;
        for (const auto& item : yamlNode) {
            result.push_back(formatValue(yamlToConfigValue(item)));
        }
        return result;
    } else if (yamlNode.IsMap()) {
        std::ostringstream oss;
        oss << "{";
        bool first = true;
        for (const auto& pair : yamlNode) {
            if (!first) oss << ", ";
            oss << "\"" << pair.first.Scalar() << "\": " << pair.second;
            first = false;
        }
        oss << "}";
        return oss.str();
    }
    return std::string();
}

YAML::Node Config::configValueToYAML(const ConfigValue& configVal) const {
    YAML::Node yamlNode;
    
    std::visit([&yamlNode](auto&& arg) {
        using T = std::decay_t<decltype(arg)>;
        if constexpr (std::is_same_v<T, std::string>) {
            yamlNode = arg;
        } else if constexpr (std::is_same_v<T, int> || std::is_same_v<T, int64_t>) {
            yamlNode = arg;
        } else if constexpr (std::is_same_v<T, double>) {
            yamlNode = arg;
        } else if constexpr (std::is_same_v<T, bool>) {
            yamlNode = arg;
        } else if constexpr (std::is_same_v<T, std::vector<int>>) {
            YAML::Node array = YAML::Node(YAML::NodeType::Sequence);
            for (const auto& item : arg) {
                array.push_back(item);
            }
            yamlNode = array;
        } else if constexpr (std::is_same_v<T, std::vector<double>>) {
            YAML::Node array = YAML::Node(YAML::NodeType::Sequence);
            for (const auto& item : arg) {
                array.push_back(item);
            }
            yamlNode = array;
        } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
            YAML::Node array = YAML::Node(YAML::NodeType::Sequence);
            for (const auto& item : arg) {
                array.push_back(item);
            }
            yamlNode = array;
        }
    }, configVal);
    
    return yamlNode;
}

// Schema validation implementations
bool Config::validateSchema(const JSONSchema& schema, const std::string& key, const ConfigValue& value, std::vector<std::string>& errors) {
    bool typeMatches = false;
    
    if (schema.type == "string") {
        typeMatches = std::holds_alternative<std::string>(value);
    } else if (schema.type == "integer") {
        typeMatches = std::holds_alternative<int>(value) || std::holds_alternative<int64_t>(value);
    } else if (schema.type == "number") {
        typeMatches = std::holds_alternative<double>(value);
    } else if (schema.type == "boolean") {
        typeMatches = std::holds_alternative<bool>(value);
    } else if (schema.type == "array") {
        typeMatches = std::holds_alternative<std::vector<int>>(value) ||
                     std::holds_alternative<std::vector<double>>(value) ||
                     std::holds_alternative<std::vector<std::string>>(value);
    } else if (schema.type == "object") {
        typeMatches = std::holds_alternative<std::string>(value);
    }
    
    if (!typeMatches) {
        errors.push_back("Key '" + key + "': expected type '" + schema.type + "', got different type");
        return false;
    }
    
    std::visit([&](auto&& arg) {
        using T = std::decay_t<decltype(arg)>;
        if constexpr (std::is_same_v<T, std::string>) {
            if (schema.pattern) {
                std::regex regex(*schema.pattern);
                if (!std::regex_match(arg, regex)) {
                    errors.push_back("Key '" + key + "': value '" + arg + "' does not match pattern '" + *schema.pattern + "'");
                }
            }
            if (schema.minLength && arg.length() < *schema.minLength) {
                errors.push_back("Key '" + key + "': string length '" + std::to_string(arg.length()) + "' is less than minimum '" + std::to_string(*schema.minLength) + "'");
            }
            if (schema.maxLength && arg.length() > *schema.maxLength) {
                errors.push_back("Key '" + key + "': string length '" + std::to_string(arg.length()) + "' exceeds maximum '" + std::to_string(*schema.maxLength) + "'");
            }
        } else if constexpr (std::is_same_v<T, int> || std::is_same_v<T, int64_t>) {
            if (schema.min && (double)arg < *schema.min) {
                errors.push_back("Key '" + key + "': value '" + std::to_string(arg) + "' is less than minimum '" + std::to_string(*schema.min) + "'");
            }
            if (schema.max && (double)arg > *schema.max) {
                errors.push_back("Key '" + key + "': value '" + std::to_string(arg) + "' exceeds maximum '" + std::to_string(*schema.max) + "'");
            }
        } else if constexpr (std::is_same_v<T, double>) {
            if (schema.min && arg < *schema.min) {
                errors.push_back("Key '" + key + "': value '" + std::to_string(arg) + "' is less than minimum '" + std::to_string(*schema.min) + "'");
            }
            if (schema.max && arg > *schema.max) {
                errors.push_back("Key '" + key + "': value '" + std::to_string(arg) + "' exceeds maximum '" + std::to_string(*schema.max) + "'");
            }
        } else if constexpr (std::is_same_v<T, bool>) {
        } else if constexpr (std::is_same_v<T, std::vector<int>> || 
                           std::is_same_v<T, std::vector<double>> || 
                           std::is_same_v<T, std::vector<std::string>>) {
        }
    }, value);
    
    return errors.empty();
}

// Environment variable substitution implementations
std::string Config::substituteEnvironmentVariables(const std::string& str) {
    std::regex envVarPattern(R"(\$\{?([A-Z_][A-Z0-9_]*)(:?\+)?\}?)");
    std::string result = str;
    std::smatch matches;
    
    while (std::regex_search(result, matches, envVarPattern)) {
        for (size_t i = 0; i < matches.size(); ++i) {
            std::string varName = matches[i];
            if (varName.find('${') == 0 && varName.back() == '}') {
                varName = varName.substr(2, varName.size() - 3);
            } else if (varName.front() == '$') {
                varName = varName.substr(1);
            }
            
            const char* envValue = std::getenv(varName.c_str());
            if (envValue) {
                std::string replacement = envValue;
                size_t plusPos = varName.find('+');
                if (plusPos != std::string::npos) {
                    std::string defaultValue = varName.substr(plusPos + 1);
                    if (replacement.empty()) {
                        replacement = defaultValue;
                    }
                }
                
                size_t pos = result.find(matches[i]);
                if (pos != std::string::npos) {
                    result.replace(pos, matches[i].length(), replacement);
                }
            }
        }
    }
    
    return result;
}

bool Config::isEnvironmentVariable(const std::string& str) {
    return str.find('$') == 0;
}

std::string Config::getEnvironmentVariableValue(const std::string& varName) {
    const char* envValue = std::getenv(varName.c_str());
    return envValue ? envValue : "";
}

// Configuration loading implementations
bool Config::loadFromFile(const std::string& filepath) {
    std::string format = detectFormat(filepath);
    
    if (format == "JSON") {
        return loadFromJSONWithValidation(filepath, JSONSchema{});
    } else if (format == "YAML") {
        return loadFromYAMLWithValidation(filepath, JSONSchema{});
    } else {
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
                
                value = substituteEnvironmentVariables(value);
                
                set(key, value, ConfigSource::File);
            }
        }
        return true;
    }
}

bool Config::loadFromJSON(const std::string& jsonStr) {
    try {
        auto json = nlohmann::json::parse(jsonStr);
        
        clear();
        
        if (json.is_object()) {
            for (auto it = json.begin(); it != json.end(); ++it) {
                std::string key = it.key();
                ConfigValue value = jsonToConfigValue(it.value());
                set(key, value, ConfigSource::File);
            }
        }
        
        return true;
    } catch (const nlohmann::json::parse_error& e) {
        throw std::runtime_error("JSON parse error: " + std::string(e.what()));
    }
}

bool Config::loadFromYAML(const std::string& yamlStr) {
    try {
        auto yaml = YAML::Load(yamlStr);
        
        clear();
        
        if (yaml.IsMap()) {
            for (const auto& pair : yaml) {
                std::string key = pair.first.Scalar();
                ConfigValue value = yamlToConfigValue(pair.second);
                set(key, value, ConfigSource::File);
            }
        }
        
        return true;
    } catch (const YAML::Exception& e) {
        throw std::runtime_error("YAML parse error: " + std::string(e.what()));
    }
}

bool Config::loadFromJSONWithValidation(const std::string& filepath, const JSONSchema& schema) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    std::stringstream buffer;
    buffer << file.rdbuf();
    std::string jsonStr = buffer.str();
    
    return loadFromJSONWithValidation(jsonStr, schema);
}

bool Config::loadFromJSONWithValidation(const std::string& jsonStr, const JSONSchema& schema) {
    try {
        auto json = nlohmann::json::parse(jsonStr);
        
        clear();
        std::vector<std::string> errors;
        
        if (json.is_object()) {
            for (auto it = json.begin(); it != json.end(); ++it) {
                std::string key = it.key();
                ConfigValue value = jsonToConfigValue(it.value());
                
                JSONSchema effectiveSchema = schema;
                if (!schema.properties.empty() && schema.properties->find(key) != schema.properties->end()) {
                    effectiveSchema = schema.properties->at(key);
                }
                
                if (!validateSchema(effectiveSchema, key, value, errors)) {
                    throw std::runtime_error("Schema validation failed for key '" + key + "': " + errors.back());
                }
                
                set(key, value, ConfigSource::File);
            }
        }
        
        return true;
    } catch (const nlohmann::json::parse_error& e) {
        throw std::runtime_error("JSON parse error: " + std::string(e.what()));
    }
}

bool Config::loadFromYAMLWithValidation(const std::string& filepath, const JSONSchema& schema) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    std::stringstream buffer;
    buffer << file.rdbuf();
    std::string yamlStr = buffer.str();
    
    return loadFromYAMLWithValidation(yamlStr, schema);
}

bool Config::loadFromYAMLWithValidation(const std::string& yamlStr, const JSONSchema& schema) {
    try {
        auto yaml = YAML::Load(yamlStr);
        
        clear();
        std::vector<std::string> errors;
        
        if (yaml.IsMap()) {
            for (const auto& pair : yaml) {
                std::string key = pair.first.Scalar();
                ConfigValue value = yamlToConfigValue(pair.second);
                
                if (!schema.type.empty()) {
                    if (!validateSchema(schema, key, value, errors)) {
                        throw std::runtime_error("Schema validation failed for key '" + key + "': " + errors.back());
                    }
                }
                
                set(key, value, ConfigSource::File);
            }
        }
        
        return true;
    } catch (const YAML::Exception& e) {
        throw std::runtime_error("YAML parse error: " + std::string(e.what()));
    }
}

bool Config::loadFromArgs(int argc, char** argv) {
    for (int i = 1; i < argc; ++i) {
        std::string arg(argv[i]);
        
        if (arg.substr(0, 2) == "--") {
            size_t pos = arg.find('=');
            if (pos != std::string::npos) {
                std::string key = arg.substr(2, pos - 2);
                std::string value = arg.substr(pos + 1);
                set(key, value, ConfigSource::CommandLine);
            }
        } else if (arg[0] == '-' && i + 1 < argc) {
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
        file << entry.key << " = " << formatValue(entry.value) << "\n";
    }
    
    return true;
}

std::string Config::saveToJSON() const {
    nlohmann::json json = nlohmann::json::object();
    
    for (const auto& entry : pImpl->entries) {
        json[entry.key] = configValueToJSON(entry.value);
    }
    
    return json.dump(2);
}

std::string Config::saveToYAML() const {
    YAML::Emitter emitter;
    emitter << YAML::BeginMap;
    
    for (const auto& entry : pImpl->entries) {
        emitter << YAML::Key << entry.key;
        emitter << YAML::Value << configValueToYAML(entry.value);
    }
    
    emitter << YAML::EndMap;
    return emitter.c_str();
}

bool Config::resolveEnvironmentVariables() {
    bool changed = false;
    
    for (auto& entry : pImpl->entries) {
        std::string originalValue = std::get<std::string>(entry.value);
        std::string newValue = substituteEnvironmentVariables(originalValue);
        
        if (originalValue != newValue) {
            set(entry.key, newValue, entry.source);
            changed = true;
        }
    }
    
    return changed;
}

bool Config::inheritFrom(const std::string& parentConfigPath) {
    Config parentConfig;
    if (!parentConfig.loadFromFile(parentConfigPath)) {
        return false;
    }
    
    bool inherited = false;
    for (const auto& parentEntry : parentConfig.pImpl->entries) {
        if (!has(parentEntry.key)) {
            set(parentEntry.key, parentEntry.value, ConfigSource::Inherited);
            inherited = true;
        }
    }
    
    return inherited;
}

bool Config::validate(const JSONSchema& schema, std::vector<std::string>& errors) const {
    errors.clear();
    
    for (const auto& entry : pImpl->entries) {
        if (!validateSchema(schema, entry.key, entry.value, errors)) {
            return false;
        }
    }
    
    return errors.empty();
}

bool Config::has(const std::string& key) const {
    return pImpl->keyToIndex.find(key) != pImpl->keyToIndex.end();
}

void Config::remove(const std::string& key) {
    auto it = pImpl->keyToIndex.find(key);
    if (it != pImpl->keyToIndex.end()) {
        size_t index = it->second;
        pImpl->entries.erase(pImpl->entries.begin() + index);
        pImpl->keyToIndex.erase(it);
        
        for (auto& pair : pImpl->keyToIndex) {
            if (pair.second > index) {
                pair.second--;
            }
        }
    }
}

std::vector<std::string> Config::getKeys() const {
    std::vector<std::string> keys;
    keys.reserve(pImpl->entries.size());
    
    for (const auto& entry : pImpl->entries) {
        keys.push_back(entry.key);
    }
    
    return keys;
}

std::vector<ConfigEntry> Config::getEntriesBySource(ConfigSource source) const {
    std::vector<ConfigEntry> result;
    result.reserve(pImpl->entries.size());
    
    for (const auto& entry : pImpl->entries) {
        if (entry.source == source) {
            result.push_back(entry);
        }
    }
    
    return result;
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
            } else if constexpr (std::is_same_v<T, std::vector<int>>) {
                oss << "[";
                for (size_t i = 0; i < arg.size(); ++i) {
                    if (i > 0) oss << ", ";
                    oss << arg[i];
                }
                oss << "]";
            } else if constexpr (std::is_same_v<T, std::vector<double>>) {
                oss << "[";
                for (size_t i = 0; i < arg.size(); ++i) {
                    if (i > 0) oss << ", ";
                    oss << arg[i];
                }
                oss << "]";
            } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
                oss << "[";
                for (size_t i = 0; i < arg.size(); ++i) {
                    if (i > 0) oss << ", ";
                    oss << "\"" << arg[i] << "\"";
                }
                oss << "]";
            } else {
                oss << arg;
            }
        }, entry.value);
        oss << "] (" << static_cast<int>(entry.source) << ")\n";
    }
    return oss.str();
}

std::string Config::detectFormat(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return "Unknown";
    }
    
    std::string firstLine;
    if (!std::getline(file, firstLine)) {
        return "Unknown";
    }
    
    if (firstLine.find('{') != std::string::npos || 
        firstLine.find('[') != std::string::npos ||
        firstLine.find(':') != std::string::npos) {
        return "JSON";
    }
    
    if (firstLine.find('-') != std::string::npos && 
        (firstLine.find('-') != std::string::npos || 
         firstLine.find(':') != std::string::npos)) {
        return "YAML";
    }
    
    return "KeyValue";
}

bool Config::isJSON(const std::string& content) {
    try {
        auto json = nlohmann::json::parse(content);
        return true;
    } catch (...) {
        return false;
    }
}

bool Config::isYAML(const std::string& content) {
    try {
        auto yaml = YAML::Load(content);
        return true;
    } catch (...) {
        return false;
    }
}
