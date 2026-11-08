#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <regex>
#include <unordered_map>
#include <memory>

namespace nlm {

// Simple JSON value parser
class SimpleJSONParser {
public:
    struct JSONValue {
        enum Type { JSON_NULL, JSON_BOOL, JSON_NUMBER, JSON_STRING, JSON_ARRAY, JSON_OBJECT };
        Type type;
        
        // Union for all value types
        bool bool_val;
        double number_val;
        std::string string_val;
        std::vector<JSONValue> array_val;
        std::unordered_map<std::string, JSONValue> object_val;
        
        JSONValue() : type(JSON_NULL) {}
        ~JSONValue() {}
    };
    
    static bool parseJSON(const std::string& json, JSONValue& result);
    static std::string toString(const JSONValue& value);
    static std::optional<ConfigValue> jsonValueToConfigValue(const JSONValue& jsonValue);
};

struct Config::Impl {
    std::vector<ConfigEntry> entries;
    // Schema validation
    std::unordered_map<std::string, std::string> expectedTypes;
    bool strictMode;
    
    Impl() : strictMode(false) {}
};

Config::Config() : pImpl(std::make_unique<Impl>()) {}

Config::~Config() = default;

Config::Config(Config&&) noexcept = default;

Config& Config::operator=(Config&&) noexcept = default;

bool Config::loadFromFile(const std::string& filepath) {
    // Try to detect file format by extension
    std::string ext = "." + std::filesystem::path(filepath).extension().string();
    std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);
    
    if (ext == ".json" || ext == ".yaml" || ext == ".yml") {
        // Parse JSON/YAML format
        std::ifstream file(filepath);
        if (!file.is_open()) {
            return false;
        }
        
        std::stringstream buffer;
        buffer << file.rdbuf();
        std::string content = buffer.str();
        
        SimpleJSONParser::JSONValue jsonData;
        if (!SimpleJSONParser::parseJSON(content, jsonData)) {
            return false;
        }
        
        // Convert JSON to Config entries
        return convertJSONToConfig(jsonData, ConfigSource::File);
    } else {
        // Parse simple key=value format (backward compatibility)
        return loadSimpleFormat(filepath);
    }
}

bool Config::loadSimpleFormat(const std::string& filepath) {
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
            
            // Validate and convert value
            ConfigValue configValue = parseConfigValue(value);
            if (configValue.has_value()) {
                set(key, configValue.value(), ConfigSource::File);
            } else {
                // Log warning for invalid value
                std::cerr << "Warning: Invalid config value for key '" << key << "' in file " << filepath << std::endl;
            }
        }
    }
    
    return true;
}

ConfigValue Config::parseConfigValue(const std::string& value) {
    // Try to parse as different types in order
    if (value == "true" || value == "false") {
        return value == "true";
    }
    
    // Try integer
    try {
        size_t pos;
        int intVal = std::stoi(value, &pos);
        if (pos == value.size()) return intVal;
        
        int64_t int64Val = std::stoll(value, &pos);
        if (pos == value.size()) return int64Val;
        
        float floatVal = std::stof(value, &pos);
        if (pos == value.size()) return floatVal;
        
        double doubleVal = std::stod(value, &pos);
        if (pos == value.size()) return doubleVal;
    } catch (...) {
        // Ignore parsing errors, continue to string
    }
    
    // Try boolean (case-insensitive)
    std::string lowerValue = toLower(value);
    if (lowerValue == "true") return true;
    if (lowerValue == "false") return false;
    
    // Return as string
    return value;
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
    
    // Try to detect preferred format by extension
    std::string ext = "." + std::filesystem::path(filepath).extension().string();
    std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);
    
    if (ext == ".json" || ext == ".yaml" || ext == ".yml") {
        // Save in JSON format
        SimpleJSONParser::JSONValue jsonData;
        convertConfigToJSON(jsonData);
        std::string jsonStr = SimpleJSONParser::toString(jsonData);
        file << jsonStr;
    } else {
        // Save in simple key=value format (backward compatibility)
        for (const auto& entry : pImpl->entries) {
            if (!entry.description.empty()) {
                file << "# " << entry.description << "\n";
            }
            file << entry.key << " = " << getConfigValueAsString(entry.value) << "\n\n";
        }
    }
    
    return true;
}

std::string Config::getConfigValueAsString(const ConfigValue& value) const {
    std::ostringstream oss;
    std::visit([&oss](auto&& arg) {
        using T = std::decay_t<decltype(arg)>;
        if constexpr (std::is_same_v<T, std::string>) {
            oss << "\"" << arg << "\"";
        } else if constexpr (std::is_same_v<T, bool>) {
            oss << (arg ? "true" : "false");
        } else if constexpr (std::is_arithmetic_v<T>) {
            oss << arg;
        } else if constexpr (std::is_same_v<T, std::vector<int>> ||
                             std::is_same_v<T, std::vector<double>> ||
                             std::is_same_v<T, std::vector<std::string>>) {
            oss << "[";
            for (size_t i = 0; i < arg.size(); ++i) {
                if (i) oss << ", ";
                oss << arg[i];
            }
            oss << "]";
        } else {
            oss << "UNKNOWN";
        }
    }, value);
    return oss.str();
}

bool Config::validateConfig() const {
    if (!pImpl->strictMode) {
        return true;  // Skip validation in non-strict mode
    }
    
    for (const auto& entry : pImpl->entries) {
        // Check if key has expected type
        auto it = pImpl->expectedTypes.find(entry.key);
        if (it != pImpl->expectedTypes.end()) {
            std::string expectedType = it->second;
            if (!validateConfigValueType(entry.value, expectedType)) {
                std::cerr << "Error: Config key '" << entry.key << "' has wrong type. Expected " << expectedType << ".\n";
                return false;
            }
        }
    }
    return true;
}

bool Config::validateConfigValueType(const ConfigValue& value, const std::string& typeName) const {
    std::string type = toLower(typeName);
    
    return std::visit([&type](auto&& arg) -> bool {
        using T = std::decay_t<decltype(arg)>;
        if constexpr (std::is_same_v<T, int>) return type == "int" || type == "integer";
        else if constexpr (std::is_same_v<T, int64_t>) return type == "int64" || type == "long" || type == "integer";
        else if constexpr (std::is_same_v<T, double>) return type == "double" || type == "float" || type == "number";
        else if constexpr (std::is_same_v<T, float>) return type == "float" || type == "number";
        else if constexpr (std::is_same_v<T, bool>) return type == "bool" || type == "boolean";
        else if constexpr (std::is_same_v<T, std::string>) return type == "string" || type == "str";
        else if constexpr (std::is_same_v<T, std::vector<int>>) return type == "int_array" || type == "int_list";
        else if constexpr (std::is_same_v<T, std::vector<double>>) return type == "double_array" || type == "float_list";
        else if constexpr (std::is_same_v<T, std::vector<std::string>>) return type == "string_array" || type == "string_list";
        else return false;
    }, value);
}

void Config::setStrictMode(bool strict) {
    pImpl->strictMode = strict;
}

void Config::setExpectedType(const std::string& key, const std::string& typeName) {
    pImpl->expectedTypes[key] = typeName;
}

bool Config::convertJSONToConfig(const SimpleJSONParser::JSONValue& jsonData, ConfigSource source) {
    if (jsonData.type == SimpleJSONParser::JSONValue::JSON_OBJECT) {
        for (const auto& pair : jsonData.object_val) {
            std::optional<ConfigValue> configVal = SimpleJSONParser::jsonValueToConfigValue(pair.second);
            if (configVal.has_value()) {
                set(pair.first, configVal.value(), source);
            }
        }
        return true;
    }
    return false;
}

void Config::convertConfigToJSON(SimpleJSONParser::JSONValue& jsonData) const {
    jsonData.type = SimpleJSONParser::JSONValue::JSON_OBJECT;
    
    for (const auto& entry : pImpl->entries) {
        std::optional<SimpleJSONParser::JSONValue> jsonVal = configValueToJSONValue(entry.value);
        if (jsonVal.has_value()) {
            jsonData.object_val[entry.key] = std::move(jsonVal.value());
        }
    }
}

std::optional<SimpleJSONParser::JSONValue> Config::configValueToJSONValue(const ConfigValue& configValue) const {
    SimpleJSONParser::JSONValue jsonVal;
    
    return std::visit([](auto&& arg) -> std::optional<SimpleJSONParser::JSONValue> {
        using T = std::decay_t<decltype(arg)>;
        if constexpr (std::is_same_v<T, int>) {
            jsonVal.type = SimpleJSONParser::JSONValue::JSON_NUMBER;
            jsonVal.number_val = static_cast<double>(arg);
            return jsonVal;
        } else if constexpr (std::is_same_v<T, int64_t>) {
            jsonVal.type = SimpleJSONParser::JSONValue::JSON_NUMBER;
            jsonVal.number_val = static_cast<double>(arg);
            return jsonVal;
        } else if constexpr (std::is_same_v<T, double>) {
            jsonVal.type = SimpleJSONParser::JSONValue::JSON_NUMBER;
            jsonVal.number_val = arg;
            return jsonVal;
        } else if constexpr (std::is_same_v<T, float>) {
            jsonVal.type = SimpleJSONParser::JSONValue::JSON_NUMBER;
            jsonVal.number_val = static_cast<double>(arg);
            return jsonVal;
        } else if constexpr (std::is_same_v<T, bool>) {
            jsonVal.type = SimpleJSONParser::JSONValue::JSON_BOOL;
            jsonVal.bool_val = arg;
            return jsonVal;
        } else if constexpr (std::is_same_v<T, std::string>) {
            jsonVal.type = SimpleJSONParser::JSONValue::JSON_STRING;
            jsonVal.string_val = arg;
            return jsonVal;
        } else if constexpr (std::is_same_v<T, std::vector<int>>) {
            jsonVal.type = SimpleJSONParser::JSONValue::JSON_ARRAY;
            for (const auto& item : arg) {
                SimpleJSONParser::JSONValue itemVal;
                itemVal.type = SimpleJSONParser::JSONValue::JSON_NUMBER;
                itemVal.number_val = static_cast<double>(item);
                jsonVal.array_val.push_back(itemVal);
            }
            return jsonVal;
        } else if constexpr (std::is_same_v<T, std::vector<double>>) {
            jsonVal.type = SimpleJSONParser::JSONValue::JSON_ARRAY;
            for (const auto& item : arg) {
                SimpleJSONParser::JSONValue itemVal;
                itemVal.type = SimpleJSONParser::JSONValue::JSON_NUMBER;
                itemVal.number_val = item;
                jsonVal.array_val.push_back(itemVal);
            }
            return jsonVal;
        } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
            jsonVal.type = SimpleJSONParser::JSONValue::JSON_ARRAY;
            for (const auto& item : arg) {
                SimpleJSONParser::JSONValue itemVal;
                itemVal.type = SimpleJSONParser::JSONValue::JSON_STRING;
                itemVal.string_val = item;
                jsonVal.array_val.push_back(itemVal);
            }
            return jsonVal;
        } else {
            return std::nullopt;
        }
    }, configValue);
}

// Simple JSON parser implementation
bool SimpleJSONParser::parseJSON(const std::string& json, JSONValue& result) {
    size_t pos = 0;
    skipWhitespace(json, pos);
    bool parsed = parseValue(json, pos, result);
    skipWhitespace(json, pos);
    return parsed && pos == json.size();
}

bool SimpleJSONParser::parseValue(const std::string& json, size_t& pos, JSONValue& result) {
    skipWhitespace(json, pos);
    
    if (pos >= json.size()) return false;
    
    char ch = json[pos];
    
    if (ch == '{') {
        return parseObject(json, pos, result);
    } else if (ch == '[') {
        return parseArray(json, pos, result);
    } else if (ch == '"') {
        return parseString(json, pos, result);
    } else if (ch == 't' || ch == 'f') {
        return parseBoolean(json, pos, result);
    } else if (ch == 'n') {
        return parseNull(json, pos, result);
    } else if (ch == '-' || std::isdigit(ch)) {
        return parseNumber(json, pos, result);
    } else {
        return false;
    }
}

bool SimpleJSONParser::parseObject(const std::string& json, size_t& pos, JSONValue& result) {
    if (json[pos] != '{') return false;
    
    size_t startPos = pos++;
    skipWhitespace(json, pos);
    
    result.type = JSONValue::JSON_OBJECT;
    
    if (json[pos] == '}') {
        pos++;
        return true;
    }
    
    while (true) {
        skipWhitespace(json, pos);
        
        // Parse key
        JSONValue keyVal;
        if (!parseString(json, pos, keyVal) || keyVal.type != JSONValue::JSON_STRING) {
            return false;
        }
        
        skipWhitespace(json, pos);
        if (pos >= json.size() || json[pos] != ':') {
            return false;
        }
        pos++;
        
        // Parse value
        JSONValue valueVal;
        if (!parseValue(json, pos, valueVal)) {
            return false;
        }
        
        // Store in object
        result.object_val[keyVal.string_val] = std::move(valueVal);
        
        skipWhitespace(json, pos);
        if (pos >= json.size()) {
            return false;
        }
        
        if (json[pos] == '}') {
            pos++;
            break;
        } else if (json[pos] == ',') {
            pos++;
            continue;
        } else {
            return false;
        }
    }
    
    return true;
}

bool SimpleJSONParser::parseArray(const std::string& json, size_t& pos, JSONValue& result) {
    if (json[pos] != '[') return false;
    
    size_t startPos = pos++;
    skipWhitespace(json, pos);
    
    result.type = JSONValue::JSON_ARRAY;
    
    if (json[pos] == ']') {
        pos++;
        return true;
    }
    
    while (true) {
        JSONValue element;
        if (!parseValue(json, pos, element)) {
            return false;
        }
        
        result.array_val.push_back(std::move(element));
        
        skipWhitespace(json, pos);
        if (pos >= json.size()) {
            return false;
        }
        
        if (json[pos] == ']') {
            pos++;
            break;
        } else if (json[pos] == ',') {
            pos++;
            continue;
        } else {
            return false;
        }
    }
    
    return true;
}

bool SimpleJSONParser::parseString(const std::string& json, size_t& pos, JSONValue& result) {
    if (json[pos] != '"') return false;
    
    size_t startPos = ++pos;  // Skip opening quote
    
    while (pos < json.size()) {
        if (json[pos] == '\\') {
            pos++;  // Skip escape character
            if (pos >= json.size()) return false;
        } else if (json[pos] == '"') {
            break;
        }
        pos++;
    }
    
    if (pos >= json.size() || json[pos] != '"') {
        return false;
    }
    
    result.type = JSONValue::JSON_STRING;
    result.string_val = json.substr(startPos, pos - startPos);
    pos++;  // Skip closing quote
    
    return true;
}

bool SimpleJSONParser::parseBoolean(const std::string& json, size_t& pos, JSONValue& result) {
    if (json.compare(pos, 4, "true") == 0) {
        result.type = JSONValue::JSON_BOOL;
        result.bool_val = true;
        pos += 4;
        return true;
    } else if (json.compare(pos, 5, "false") == 0) {
        result.type = JSONValue::JSON_BOOL;
        result.bool_val = false;
        pos += 5;
        return true;
    }
    return false;
}

bool SimpleJSONParser::parseNull(const std::string& json, size_t& pos, JSONValue& result) {
    if (json.compare(pos, 4, "null") == 0) {
        result.type = JSONValue::JSON_NULL;
        pos += 4;
        return true;
    }
    return false;
}

bool SimpleJSONParser::parseNumber(const std::string& json, size_t& pos, JSONValue& result) {
    size_t startPos = pos;
    
    if (json[pos] == '-') pos++;
    
    // Parse integer part
    if (pos < json.size() && std::isdigit(json[pos])) {
        while (pos < json.size() && std::isdigit(json[pos])) pos++;
    }
    
    // Parse decimal part
    if (pos < json.size() && json[pos] == '.') {
        pos++;
        if (pos < json.size() && std::isdigit(json[pos])) {
            while (pos < json.size() && std::isdigit(json[pos])) pos++;
        } else {
            return false;
        }
    }
    
    // Parse exponent
    if (pos < json.size() && (json[pos] == 'e' || json[pos] == 'E')) {
        pos++;
        if (pos < json.size() && (json[pos] == '+' || json[pos] == '-')) pos++;
        if (pos < json.size() && std::isdigit(json[pos])) {
            while (pos < json.size() && std::isdigit(json[pos])) pos++;
        } else {
            return false;
        }
    }
    
    std::string numStr = json.substr(startPos, pos - startPos);
    try {
        // Try to parse as double first (to preserve precision)
        size_t posEnd;
        double val = std::stod(numStr, &posEnd);
        if (posEnd == numStr.size()) {
            result.type = JSONValue::JSON_NUMBER;
            result.number_val = val;
            return true;
        }
        
        // If not a valid double, try as int
        size_t posEnd2;
        long long intVal = std::stoll(numStr, &posEnd2);
        if (posEnd2 == numStr.size()) {
            result.type = JSONValue::JSON_NUMBER;
            result.number_val = static_cast<double>(intVal);
            return true;
        }
    } catch (...) {
        return false;
    }
    
    return false;
}

std::string SimpleJSONParser::toString(const JSONValue& value) {
    std::ostringstream oss;
    
    switch (value.type) {
        case JSONValue::JSON_NULL:
            oss << "null";
            break;
        case JSONValue::JSON_BOOL:
            oss << (value.bool_val ? "true" : "false");
            break;
        case JSONValue::JSON_NUMBER:
            oss << value.number_val;
            break;
        case JSONValue::JSON_STRING:
            oss << "\"" << escapeString(value.string_val) << "\"";
            break;
        case JSONValue::JSON_ARRAY:
            oss << "[";
            for (size_t i = 0; i < value.array_val.size(); ++i) {
                if (i > 0) oss << ", ";
                oss << toString(value.array_val[i]);
            }
            oss << "]";
            break;
        case JSONValue::JSON_OBJECT:
            oss << "{";
            for (auto it = value.object_val.begin(); it != value.object_val.end(); ++it) {
                if (it != value.object_val.begin()) oss << ", ";
                oss << "\"" << escapeString(it->first) << "\": " << toString(it->second);
            }
            oss << "}";
            break;
    }
    
    return oss.str();
}

std::string SimpleJSONParser::escapeString(const std::string& str) {
    std::ostringstream oss;
    for (char ch : str) {
        switch (ch) {
            case '\\': oss << "\\\\"; break;
            case '"': oss << "\\\""; break;
            case '\b': oss << "\\b"; break;
            case '\f': oss << "\\f"; break;
            case '\n': oss << "\\n"; break;
            case '\r': oss << "\\r"; break;
            case '\t': oss << "\\t"; break;
            default:
                if (ch < 32 || ch > 126) {
                    oss << "\\u" << std::hex << std::setw(4) << std::setfill('0') << static_cast<int>(ch);
                } else {
                    oss << ch;
                }
                break;
        }
    }
    return oss.str();
}

std::optional<ConfigValue> SimpleJSONParser::jsonValueToConfigValue(const JSONValue& jsonValue) {
    switch (jsonValue.type) {
        case JSONValue::JSON_NULL:
            return std::string("");  // Treat null as empty string
        case JSONValue::JSON_BOOL:
            return jsonValue.bool_val;
        case JSONValue::JSON_NUMBER:
            return jsonValue.number_val;
        case JSONValue::JSON_STRING:
            return jsonValue.string_val;
        case JSONValue::JSON_ARRAY: {
            // Determine array type based on first element
            if (jsonValue.array_val.empty()) {
                return std::vector<std::string>{};
            }
            
            // Check if all elements are numbers (int or double)
            bool allNumbers = true;
            for (const auto& item : jsonValue.array_val) {
                if (item.type != JSONValue::JSON_NUMBER) {
                    allNumbers = false;
                    break;
                }
            }
            
            if (allNumbers) {
                std::vector<double> doubleArray;
                for (const auto& item : jsonValue.array_val) {
                    doubleArray.push_back(item.number_val);
                }
                return doubleArray;
            } else {
                // Try to parse as integers first, then fall back to strings
                bool allIntegers = true;
                for (const auto& item : jsonValue.array_val) {
                    if (item.type != JSONValue::JSON_NUMBER || 
                        !std::floor(item.number_val) == item.number_val) {
                        allIntegers = false;
                        break;
                    }
                }
                
                if (allIntegers) {
                    std::vector<int> intArray;
                    for (const auto& item : jsonValue.array_val) {
                        intArray.push_back(static_cast<int>(item.number_val));
                    }
                    return intArray;
                } else {
                    // Return as string array
                    std::vector<std::string> stringArray;
                    for (const auto& item : jsonValue.array_val) {
                        if (item.type == JSONValue::JSON_STRING) {
                            stringArray.push_back(item.string_val);
                        } else {
                            stringArray.push_back(toString(item));
                        }
                    }
                    return stringArray;
                }
            }
        }
        case JSONValue::JSON_OBJECT:
            // For objects, return as JSON string representation
            return toString(jsonValue);
        default:
            return std::nullopt;
    }
}

void SimpleJSONParser::skipWhitespace(const std::string& json, size_t& pos) {
    while (pos < json.size() && std::isspace(json[pos])) {
        pos++;
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

void Config::set(const std::string& key, int64_t value, ConfigSource source) {
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
            } else if constexpr (std::is_same_v<T, std::vector<int>> ||
                                 std::is_same_v<T, std::vector<double>> ||
                                 std::is_same_v<T, std::vector<std::string>>) {
                oss << "[";
                for (size_t i = 0; i < arg.size(); ++i) {
                    if (i) oss << ", ";
                    oss << arg[i];
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

template<typename T>
std::optional<T> Config::get(const std::string& key) const {
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    if (it == pImpl->entries.end()) {
        return std::nullopt;
    }
    // Numeric values convert between int/int64/double/float/etc, so a
    // value stored as double can be read as float and vice versa.
    return std::visit([](auto&& v) -> std::optional<T> {
        using V = std::decay_t<decltype(v)>;
        if constexpr (std::is_same_v<V, T>) {
            return v;
        } else if constexpr (std::is_arithmetic_v<V> && std::is_arithmetic_v<T>) {
            return static_cast<T>(v);
        } else {
            return std::nullopt;
        }
    }, it->value);
}

template<typename T>
T Config::getOr(const std::string& key, const T& defaultValue) const {
    auto val = get<T>(key);
    return val.has_value() ? val.value() : defaultValue;
}

// Explicit template instantiations
// (uint64_t and size_t are the same type on this platform, so only one is listed)
template std::optional<int> Config::get<int>(const std::string&) const;
template std::optional<int64_t> Config::get<int64_t>(const std::string&) const;
template std::optional<uint64_t> Config::get<uint64_t>(const std::string&) const;
template std::optional<float> Config::get<float>(const std::string&) const;
template std::optional<double> Config::get<double>(const std::string&) const;
template std::optional<bool> Config::get<bool>(const std::string&) const;
template std::optional<std::string> Config::get<std::string>(const std::string&) const;

template int Config::getOr<int>(const std::string&, const int&) const;
template int64_t Config::getOr<int64_t>(const std::string&, const int64_t&) const;
template uint64_t Config::getOr<uint64_t>(const std::string&, const uint64_t&) const;
template float Config::getOr<float>(const std::string&, const float&) const;
template double Config::getOr<double>(const std::string&, const double&) const;
template bool Config::getOr<bool>(const std::string&, const bool&) const;
template std::string Config::getOr<std::string>(const std::string&, const std::string&) const;

} // namespace nlm
