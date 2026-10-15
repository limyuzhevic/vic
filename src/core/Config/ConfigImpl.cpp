#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <limits>
#include <numeric>
#include <iomanip>
#include <iostream>
#include <cctype>

namespace nlm {

// Simple JSON parser for configuration files
class SimpleJSONParser {
public:
    static bool parseFile(const std::string& filepath, std::map<std::string, std::string>& result);
    static bool parseString(const std::string& jsonStr, std::map<std::string, std::string>& result);
    static std::string toJSON(const std::map<std::string, std::string>& data);
    
private:
    static std::string trim(const std::string& str);
    static std::string unescape(const std::string& str);
    static bool isWhitespace(char c);
    static bool isDigit(char c);
    static bool isAlpha(char c);
    static std::string parseString(const std::string& json, size_t& pos);
    static double parseNumber(const std::string& json, size_t& pos);
    static bool parseBool(const std::string& json, size_t& pos);
    static std::map<std::string, std::string> parseObject(const std::string& json, size_t& pos);
    static std::vector<std::map<std::string, std::string>> parseArray(const std::string& json, size_t& pos);
};

// Implementation of Config methods
Config::Config() : pImpl(std::make_unique<Impl>()) {}

Config::~Config() = default;

Config::Config(Config&&) noexcept = default;

Config& Config::operator=(Config&&) noexcept = default;

bool Config::loadFromFile(const std::string& filepath) {
    if (!fileExists(filepath)) {
        return false;
    }
    
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    // Try JSON first, then fallback to key=value format
    std::map<std::string, std::string> jsonData;
    if (SimpleJSONParser::parseFile(filepath, jsonData)) {
        for (const auto& pair : jsonData) {
            set(pair.first, pair.second, ConfigSource::File);
        }
        return true;
    }
    
    // Fallback to simple key=value format
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

bool Config::loadFromArgs(int argc, char** argv, const ConfigSchema& schema) {
    // Parse command line arguments
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

bool Config::loadFromEnvironment(const std::string& prefix, const ConfigSchema& schema) {
    for (const auto& key : pImpl->schemaLoaded ? pImpl->schema.getKeys() : getKeys()) {
        std::string envKey = prefix + toUpper(key);
        
        const char* envValue = std::getenv(envKey.c_str());
        if (envValue) {
            std::string value = envValue;
            set(key, value, ConfigSource::Environment);
        }
    }
    return true;
}

bool Config::saveToFile(const std::string& filepath) const {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    file << "{\n";
    bool first = true;
    for (const auto& entry : pImpl->entries) {
        if (!first) file << ",\n";
        first = false;
        
        file << "  \"" << entry.first << "\": " << valueToString(entry.second.value);
    }
    file << "\n}\n";
    return true;
}

template<typename T>
std::optional<T> Config::get(const std::string& key) const {
    auto it = pImpl->entries.find(key);
    if (it == pImpl->entries.end()) {
        return std::nullopt;
    }
    
    try {
        return std::get<T>(it->second.value);
    } catch (const std::bad_variant_access&) {
        return std::nullopt;
    }
}

template<typename T>
T Config::getOr(const std::string& key, const T& defaultValue) const {
    auto val = get<T>(key);
    return val.has_value() ? val.value() : defaultValue;
}

template<typename T>
bool Config::set(const std::string& key, const T& value, ConfigSource source) {
    if (pImpl->schemaLoaded && !pImpl->schema.hasParameter(key)) {
        return false;
    }
    
    ConfigEntry entry;
    entry.key = key;
    entry.value = ConfigValue(value);
    entry.source = source;
    
    if (pImpl->schemaLoaded) {
        const auto& metadata = pImpl->schema.getMetadata(key);
        entry.metadata = metadata;
    }
    
    if (!entry.metadata.validate(entry.value)) {
        return false;
    }
    
    pImpl->entries[key] = entry;
    return true;
}

bool Config::set(const std::string& key, int value, ConfigSource source) {
    return set<int>(key, value, source);
}

bool Config::set(const std::string& key, double value, ConfigSource source) {
    return set<double>(key, value, source);
}

bool Config::set(const std::string& key, bool value, ConfigSource source) {
    return set<bool>(key, value, source);
}

bool Config::set(const std::string& key, const std::string& value, ConfigSource source) {
    return set<std::string>(key, value, source);
}

bool Config::has(const std::string& key) const {
    return pImpl->entries.find(key) != pImpl->entries.end();
}

bool Config::isValid(const std::string& key) const {
    auto it = pImpl->entries.find(key);
    if (it == pImpl->entries.end()) {
        return false;
    }
    return it->second.validate();
}

std::string Config::getValidationError(const std::string& key) const {
    auto it = pImpl->entries.find(key);
    if (it == pImpl->entries.end()) {
        return "Key not found in configuration";
    }
    return it->second.metadata.getError(it->second.value);
}

std::vector<std::string> Config::getKeys() const {
    std::vector<std::string> keys;
    keys.reserve(pImpl->entries.size());
    for (const auto& entry : pImpl->entries) {
        keys.push_back(entry.first);
    }
    return keys;
}

std::vector<std::string> Config::getValidKeys() const {
    std::vector<std::string> keys;
    for (const auto& entry : pImpl->entries) {
        if (entry.second.validate()) {
            keys.push_back(entry.first);
        }
    }
    return keys;
}

void Config::setSchema(const ConfigSchema& schema) {
    pImpl->schema = schema;
    pImpl->schemaLoaded = true;
}

const ConfigSchema& Config::getSchema() const {
    return pImpl->schema;
}

std::string Config::summary() const {
    std::ostringstream oss;
    oss << "Configuration (" << pImpl->entries.size() << " entries, " 
        << getValidKeys().size() << " valid):\n";
    for (const auto& entry : pImpl->entries) {
        oss << "  " << entry.first << " = [";
        std::visit([&oss](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, std::string>) {
                oss << "\"" << arg << "\"";
            } else if constexpr (std::is_same_v<T, bool>) {
                oss << (arg ? "true" : "false");
            } else {
                oss << arg;
            }
        }, entry.second.value);
        oss << "] (" << static_cast<int>(entry.second.source) << ")";
        
        if (!entry.second.validate()) {
            oss << " [INVALID: " << entry.second.metadata.getError(entry.second.value) << "]";
        }
        oss << "\n";
    }
    return oss.str();
}

std::string Config::getHelp(const std::string& key) const {
    if (pImpl->schemaLoaded) {
        const auto& metadata = pImpl->schema.getMetadata(key);
        std::ostringstream oss;
        oss << metadata.description;
        if (!metadata.validationRules.empty()) {
            oss << "\n\nValidation rules:\n";
            for (const auto& rule : metadata.validationRules) {
                oss << "  - " << rule.errorMessage << "\n";
            }
        }
        return oss.str();
    }
    return "No help available (schema not loaded)";
}

std::string Config::generateHelp() const {
    if (!pImpl->schemaLoaded) {
        return "No schema available to generate help.";
    }
    return pImpl->schema.generateHelp();
}

void Config::remove(const std::string& key) {
    pImpl->entries.erase(key);
}

void Config::clear() {
    pImpl->entries.clear();
}

void Config::resetToDefaults() {
    if (!pImpl->schemaLoaded) {
        clear();
        return;
    }
    
    pImpl->entries.clear();
}

std::string Config::expandPath(const std::string& path) {
    std::filesystem::path fsPath(path);
    if (fsPath.is_relative()) {
        fsPath = std::filesystem::current_path() / fsPath;
    }
    return fsPath.string();
}

bool Config::fileExists(const std::string& path) {
    return std::filesystem::exists(path);
}

std::string Config::trim(const std::string& str) {
    size_t start = str.find_first_not_of(" \t\r\n");
    if (start == std::string::npos) return "";
    size_t end = str.find_last_not_of(" \t\r\n");
    return str.substr(start, end - start + 1);
}

bool Config::isNumber(const std::string& str) {
    if (str.empty()) return false;
    size_t start = 0;
    if (str[0] == '-') start = 1;
    if (start >= str.size()) return false;
    
    bool hasDecimal = false;
    for (size_t i = start; i < str.size(); ++i) {
        if (str[i] == '.') {
            if (hasDecimal) return false;
            hasDecimal = true;
        } else if (!std::isdigit(str[i])) {
            return false;
        }
    }
    return true;
}

ConfigValue Config::stringToValue(const std::string& str) {
    if (str == "true") return true;
    if (str == "false") return false;
    
    if (isNumber(str)) {
        try {
            size_t pos;
            int64_t intVal = std::stoll(str, &pos);
            if (pos == str.size()) {
                return intVal;
            }
        } catch (...) {}
        
        try {
            size_t pos;
            double doubleVal = std::stod(str, &pos);
            if (pos == str.size()) {
                return doubleVal;
            }
        } catch (...) {}
    }
    return str;
}

std::string Config::valueToString(const ConfigValue& value) {
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

// ConfigSchema implementation
ConfigSchema& ConfigSchema::addParameter(const std::string& key, 
                                        const ConfigValue& defaultValue,
                                        const std::string& description,
                                        ConfigSource source,
                                        bool required) {
    parameters[key] = ConfigEntry(key, defaultValue, source, description);
    parameters[key].metadata.required = required;
    parameters[key].metadata.defaultSource = source;
    return *this;
}

ConfigSchema& ConfigSchema::addInt(const std::string& key, int defaultValue,
                                   const std::string& description, int min, int max) {
    auto entry = parameters.find(key);
    if (entry == parameters.end()) {
        parameters[key] = ConfigEntry(key, defaultValue, ConfigSource::Default, description);
        entry = parameters.find(key);
    }
    
    auto validator = [min, max](const ConfigValue& v) {
        try {
            int val = std::get<int>(v);
            return val >= min && val <= max;
        } catch (...) { return false; }
    };
    
    entry->second.metadata.addValidation(
        "Value must be between " + std::to_string(min) + " and " + std::to_string(max),
        validator);
    return *this;
}

ConfigSchema& ConfigSchema::addDouble(const std::string& key, double defaultValue,
                                      const std::string& description, double min, double max) {
    auto entry = parameters.find(key);
    if (entry == parameters.end()) {
        parameters[key] = ConfigEntry(key, defaultValue, ConfigSource::Default, description);
        entry = parameters.find(key);
    }
    
    auto validator = [min, max](const ConfigValue& v) {
        try {
            double val = std::get<double>(v);
            return val >= min && val <= max;
        } catch (...) { return false; }
    };
    
    entry->second.metadata.addValidation(
        "Value must be between " + std::to_string(min) + " and " + std::to_string(max),
        validator);
    return *this;
}

ConfigSchema& ConfigSchema::addBool(const std::string& key, bool defaultValue,
                                    const std::string& description) {
    auto entry = parameters.find(key);
    if (entry == parameters.end()) {
        parameters[key] = ConfigEntry(key, defaultValue, ConfigSource::Default, description);
    }
    
    auto validator = [](const ConfigValue& v) {
        return std::holds_alternative<bool>(v);
    };
    
    entry->second.metadata.addValidation("Value must be a boolean", validator);
    return *this;
}

ConfigSchema& ConfigSchema::addString(const std::string& key, const std::string& defaultValue,
                                     const std::string& description) {
    auto entry = parameters.find(key);
    if (entry == parameters.end()) {
        parameters[key] = ConfigEntry(key, defaultValue, ConfigSource::Default, description);
    }
    
    auto validator = [](const ConfigValue& v) {
        return std::holds_alternative<std::string>(v);
    };
    
    entry->second.metadata.addValidation("Value must be a string", validator);
    return *this;
}

ConfigSchema& ConfigSchema::addRangeValidation(const std::string& key, double min, double max,
                                              const std::string& errorMsg) {
    auto entry = parameters.find(key);
    if (entry == parameters.end()) return *this;
    
    auto validator = [min, max](const ConfigValue& v) {
        try {
            double val = std::get<double>(v);
            return val >= min && val <= max;
        } catch (...) { return false; }
    };
    
    entry->second.metadata.addValidation(errorMsg, validator);
    return *this;
}

ConfigSchema& ConfigSchema::addPositiveValidation(const std::string& key,
                                                 const std::string& errorMsg) {
    auto entry = parameters.find(key);
    if (entry == parameters.end()) return *this;
    
    auto validator = [](const ConfigValue& v) {
        try {
            double val = std::get<double>(v);
            return val >= 0.0;
        } catch (...) { return false; }
    };
    
    entry->second.metadata.addValidation(errorMsg, validator);
    return *this;
}

ConfigSchema& ConfigSchema::addNonEmptyValidation(const std::string& key,
                                                 const std::string& errorMsg) {
    auto entry = parameters.find(key);
    if (entry == parameters.end()) return *this;
    
    auto validator = [](const ConfigValue& v) {
        try {
            std::string val = std::get<std::string>(v);
            return !val.empty();
        } catch (...) { return false; }
    };
    
    entry->second.metadata.addValidation(errorMsg, validator);
    return *this;
}

const ConfigMetadata& ConfigSchema::getMetadata(const std::string& key) const {
    static const ConfigMetadata empty;
    auto it = parameters.find(key);
    return (it != parameters.end()) ? it->second.metadata : empty;
}

bool ConfigSchema::hasParameter(const std::string& key) const {
    return parameters.find(key) != parameters.end();
}

std::vector<std::string> ConfigSchema::getKeys() const {
    std::vector<std::string> keys;
    keys.reserve(parameters.size());
    for (const auto& param : parameters) {
        keys.push_back(param.first);
    }
    return keys;
}

nlohmann::json ConfigSchema::toJson() const {
    nlohmann::json json;
    for (const auto& param : parameters) {
        json[param.first] = valueToString(param.second.value);
    }
    return json;
}

void ConfigSchema::fromJson(const nlohmann::json& json) {
    for (auto it = json.begin(); it != json.end(); ++it) {
        std::string key = it.key();
        if (parameters.find(key) != parameters.end()) {
            parameters[key].value = stringToValue(it.value().dump());
        }
    }
}

std::string ConfigSchema::generateHelp() const {
    std::ostringstream oss;
    oss << "Configuration Parameters:" << std::endl;
    for (const auto& param : parameters) {
        const auto& metadata = param.second.metadata;
        oss << "\n" << param.first << ":" << std::endl;
        oss << "  Description: " << metadata.description << std::endl;
        oss << "  Type: " << getValueTypeName(param.second.value) << std::endl;
        if (metadata.required) {
            oss << "  Required: Yes" << std::endl;
        }
        if (!metadata.validationRules.empty()) {
            oss << "  Validation:" << std::endl;
            for (const auto& rule : metadata.validationRules) {
                oss << "    - " << rule.errorMessage << std::endl;
            }
        }
        oss << "  Source: " << static_cast<int>(param.second.source) << std::endl;
    }
    return oss.str();
}

std::string ConfigSchema::getValueTypeName(const ConfigValue& value) const {
    if (std::holds_alternative<int>(value)) return "int";
    if (std::holds_alternative<int64_t>(value)) return "int64_t";
    if (std::holds_alternative<double>(value)) return "double";
    if (std::holds_alternative<bool>(value)) return "bool";
    if (std::holds_alternative<std::string>(value)) return "string";
    if (std::holds_alternative<std::vector<int>>(value)) return "int[]";
    if (std::holds_alternative<std::vector<double>>(value)) return "double[]";
    if (std::holds_alternative<std::vector<std::string>>(value)) return "string[]";
    if (std::holds_alternative<std::vector<bool>>(value)) return "bool[]";
    return "unknown";
}

// SimpleJSONParser implementation
bool SimpleJSONParser::parseFile(const std::string& filepath, std::map<std::string, std::string>& result) {
    std::ifstream file(filepath);
    if (!file.is_open()) return false;
    
    std::string content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
    return parseString(content, result);
}

bool SimpleJSONParser::parseString(const std::string& jsonStr, std::map<std::string, std::string>& result) {
    size_t pos = 0;
    char ch;
    
    while (pos < jsonStr.size()) {
        ch = jsonStr[pos];
        
        if (isWhitespace(ch)) {
            pos++;
            continue;
        }
        
        if (ch == '{') {
            result = parseObject(jsonStr, pos);
            return true;
        }
        
        if (ch == '}') {
            return true;
        }
        
        if (ch == '"') {
            size_t keyPos = ++pos;
            while (pos < jsonStr.size() && jsonStr[pos] != '"') {
                if (jsonStr[pos] == '\\') pos++; // Skip escaped character
                pos++;
            }
            
            std::string key = jsonStr.substr(keyPos, pos - keyPos);
            pos++; // Skip closing quote
            
            // Skip whitespace
            while (pos < jsonStr.size() && isWhitespace(jsonStr[pos])) pos++;
            
            if (pos >= jsonStr.size() || jsonStr[pos] != ':') return false;
            pos++; // Skip colon
            
            // Skip whitespace
            while (pos < jsonStr.size() && isWhitespace(jsonStr[pos])) pos++;
            
            std::string valueStr;
            if (jsonStr[pos] == '"') {
                size_t valPos = ++pos;
                while (pos < jsonStr.size() && jsonStr[pos] != '"') {
                    if (jsonStr[pos] == '\\') pos++; // Skip escaped character
                    pos++;
                }
                valueStr = jsonStr.substr(valPos, pos - valPos);
                pos++; // Skip closing quote
            } else if (isDigit(jsonStr[pos]) || (jsonStr[pos] == '-' && pos + 1 < jsonStr.size() && isDigit(jsonStr[pos + 1]))) {
                size_t valPos = pos;
                while (pos < jsonStr.size() && (isDigit(jsonStr[pos]) || jsonStr[pos] == '.' || jsonStr[pos] == '-')) {
                    pos++;
                }
                valueStr = jsonStr.substr(valPos, pos - valPos);
            } else if (jsonStr[pos] == 't' || jsonStr[pos] == 'f') {
                size_t valPos = pos;
                if (jsonStr.substr(valPos, 4) == "true") {
                    valueStr = "true";
                    pos += 4;
                } else if (jsonStr.substr(valPos, 5) == "false") {
                    valueStr = "false";
                    pos += 5;
                } else {
                    return false;
                }
            } else if (jsonStr[pos] == 'n' && jsonStr.substr(pos, 4) == "null") {
                valueStr = "null";
                pos += 4;
            }
            
            result[key] = valueStr;
            
            // Skip whitespace
            while (pos < jsonStr.size() && isWhitespace(jsonStr[pos])) pos++;
            
            if (pos < jsonStr.size() && jsonStr[pos] == ',') {
                pos++; // Skip comma
                while (pos < jsonStr.size() && isWhitespace(jsonStr[pos])) pos++;
            }
        }
    }
    
    return true;
}

std::string SimpleJSONParser::toJSON(const std::map<std::string, std::string>& data) {
    std::ostringstream oss;
    oss << "{\n";
    bool first = true;
    for (const auto& pair : data) {
        if (!first) oss << ",\n";
        first = false;
        oss << "  \"" << pair.first << "\": " << pair.second;
    }
    oss << "\n}";
    return oss.str();
}

std::string SimpleJSONParser::trim(const std::string& str) {
    size_t start = str.find_first_not_of(" \t\r\n");
    if (start == std::string::npos) return "";
    size_t end = str.find_last_not_of(" \t\r\n");
    return str.substr(start, end - start + 1);
}

std::string SimpleJSONParser::unescape(const std::string& str) {
    std::string result;
    result.reserve(str.size());
    
    for (size_t i = 0; i < str.size(); ++i) {
        if (str[i] == '\\' && i + 1 < str.size()) {
            result += str[++i]; // Skip escape character and add the escaped character
        } else {
            result += str[i];
        }
    }
    
    return result;
}

bool SimpleJSONParser::isWhitespace(char c) {
    return c == ' ' || c == '\t' || c == '\n' || c == '\r' || c == '\f' || c == '\v';
}

bool SimpleJSONParser::isDigit(char c) {
    return c >= '0' && c <= '9';
}

bool SimpleJSONParser::isAlpha(char c) {
    return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || c == '_';
}

std::string SimpleJSONParser::parseString(const std::string& json, size_t& pos) {
    size_t start = ++pos; // Skip opening quote
    while (pos < json.size() && json[pos] != '"') {
        if (json[pos] == '\\' && pos + 1 < json.size()) {
            pos++; // Skip escape character
        }
        pos++;
    }
    std::string result = json.substr(start, pos - start);
    pos++; // Skip closing quote
    return unescape(result);
}

double SimpleJSONParser::parseNumber(const std::string& json, size_t& pos) {
    size_t start = pos;
    if (json[pos] == '-') pos++;
    while (pos < json.size() && isDigit(json[pos])) pos++;
    if (pos < json.size() && json[pos] == '.') {
        pos++;
        while (pos < json.size() && isDigit(json[pos])) pos++;
    }
    return std::stod(json.substr(start, pos - start));
}

bool SimpleJSONParser::parseBool(const std::string& json, size_t& pos) {
    if (json.substr(pos, 4) == "true") {
        pos += 4;
        return true;
    } else if (json.substr(pos, 5) == "false") {
        pos += 5;
        return false;
    }
    return false;
}

std::map<std::string, std::string> SimpleJSONParser::parseObject(const std::string& json, size_t& pos) {
    std::map<std::string, std::string> result;
    pos++; // Skip opening brace
    
    while (pos < json.size() && json[pos] != '}') {
        // Parse key
        std::string key;
        while (pos < json.size() && isWhitespace(json[pos])) pos++;
        
        if (json[pos] == '"') {
            key = parseString(json, pos);
        } else {
            return result;
        }
        
        // Skip whitespace
        while (pos < json.size() && isWhitespace(json[pos])) pos++;
        
        if (pos >= json.size() || json[pos] != ':') return result;
        pos++; // Skip colon
        
        // Skip whitespace
        while (pos < json.size() && isWhitespace(json[pos])) pos++;
        
        // Parse value
        std::string valueStr;
        if (json[pos] == '"') {
            valueStr = parseString(json, pos);
        } else if (isDigit(json[pos]) || (json[pos] == '-' && pos + 1 < json.size() && isDigit(json[pos + 1]))) {
            size_t start = pos;
            if (json[pos] == '-') pos++;
            while (pos < json.size() && isDigit(json[pos])) pos++;
            if (pos < json.size() && json[pos] == '.') {
                pos++;
                while (pos < json.size() && isDigit(json[pos])) pos++;
            }
            valueStr = json.substr(start, pos - start);
        } else if (json.substr(pos, 4) == "true") {
            valueStr = "true";
            pos += 4;
        } else if (json.substr(pos, 5) == "false") {
            valueStr = "false";
            pos += 5;
        } else if (json.substr(pos, 4) == "null") {
            valueStr = "null";
            pos += 4;
        }
        
        result[key] = valueStr;
        
        // Skip whitespace
        while (pos < json.size() && isWhitespace(json[pos])) pos++;
        
        if (pos < json.size() && json[pos] == ',') {
            pos++; // Skip comma
            while (pos < json.size() && isWhitespace(json[pos])) pos++;
        }
    }
    
    if (pos < json.size() && json[pos] == '}') {
        pos++; // Skip closing brace
    }
    
    return result;
}

std::vector<std::map<std::string, std::string>> SimpleJSONParser::parseArray(const std::string& json, size_t& pos) {
    std::vector<std::map<std::string, std::string>> result;
    pos++; // Skip opening bracket
    
    while (pos < json.size() && json[pos] != ']') {
        // Parse object
        std::map<std::string, std::string> obj;
        
        if (json[pos] == '{') {
            obj = parseObject(json, pos);
        }
        
        result.push_back(obj);
        
        // Skip whitespace
        while (pos < json.size() && isWhitespace(json[pos])) pos++;
        
        if (pos < json.size() && json[pos] == ',') {
            pos++; // Skip comma
            while (pos < json.size() && isWhitespace(json[pos])) pos++;
        }
    }
    
    if (pos < json.size() && json[pos] == ']') {
        pos++; // Skip closing bracket
    }
    
    return result;
}

} // namespace nlm
