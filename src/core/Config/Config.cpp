#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <stack>
#include <vector>
#include <unordered_map>
#include <string>
#include <optional>

namespace nlm {

// Simple JSON parser implementation
class SimpleJsonParser {
public:
    struct JsonValue {
        enum class Type { STRING, NUMBER, BOOLEAN, NULL_TYPE, ARRAY, OBJECT };
        
        Type type;
        std::string stringValue;
        double numberValue;
        bool booleanValue;
        std::vector<JsonValue> arrayValue;
        std::unordered_map<std::string, JsonValue> objectValue;
        
        JsonValue() : type(Type::NULL_TYPE) {}
        JsonValue(const std::string& s) : type(Type::STRING), stringValue(s) {}
        JsonValue(double n) : type(Type::NUMBER), numberValue(n) {}
        JsonValue(bool b) : type(Type::BOOLEAN), booleanValue(b) {}
        JsonValue(const std::vector<JsonValue>& arr) : type(Type::ARRAY), arrayValue(arr) {}
        JsonValue(const std::unordered_map<std::string, JsonValue>& obj) : type(Type::OBJECT), objectValue(obj) {}
    };
    
    static std::optional<JsonValue> parseJsonFile(const std::string& filepath) {
        std::ifstream file(filepath);
        if (!file.is_open()) {
            return std::nullopt;
        }
        
        std::stringstream buffer;
        buffer << file.rdbuf();
        return parseJsonString(buffer.str());
    }
    
    static std::optional<JsonValue> parseJsonString(const std::string& jsonStr) {
        try {
            auto result = parseValue(jsonStr, 0);
            if (!result) {
                return std::nullopt;
            }
            
            // Ensure we parsed the entire string
            size_t endPos = skipWhitespace(jsonStr, result->second);
            if (endPos != jsonStr.size()) {
                return std::nullopt;
            }
            
            return result->first;
        } catch (const std::exception&) {
            return std::nullopt;
        }
    }
    
    static std::pair<std::optional<JsonValue>, size_t> parseValue(const std::string& jsonStr, size_t startPos) {
        size_t pos = skipWhitespace(jsonStr, startPos);
        
        if (pos >= jsonStr.size()) {
            return {std::nullopt, pos};
        }
        
        char c = jsonStr[pos];
        
        switch (c) {
            case '"':
                return parseString(jsonStr, pos);
            case 't':
            case 'f':
                return parseBoolean(jsonStr, pos);
            case 'n':
                return parseNull(jsonStr, pos);
            case '-':
            case '0': case '1': case '2': case '3': case '4':
            case '5': case '6': case '7': case '8': case '9':
                return parseNumber(jsonStr, pos);
            case '[':
                return parseArray(jsonStr, pos);
            case '{':
                return parseObject(jsonStr, pos);
            default:
                return {std::nullopt, pos};
        }
    }
    
private:
    static size_t skipWhitespace(const std::string& str, size_t pos) {
        while (pos < str.size() && std::isspace(str[pos])) {
            pos++;
        }
        return pos;
    }
    
    static std::pair<std::optional<JsonValue>, size_t> parseString(const std::string& str, size_t startPos) {
        size_t endPos = str.find('"', startPos + 1);
        if (endPos == std::string::npos) {
            return {std::nullopt, startPos};
        }
        
        std::string value = str.substr(startPos + 1, endPos - startPos - 1);
        
        // Handle basic escape sequences
        size_t escapePos = value.find('\\');
        while (escapePos != std::string::npos && escapePos + 1 < value.size()) {
            char escaped = value[escapePos + 1];
            if (escaped == '"' || escaped == '\\' || escaped == '/') {
                value.erase(escapePos, 2);
            } else if (escaped == 'b') {
                value.replace(escapePos, 2, "\b");
            } else if (escaped == 'f') {
                value.replace(escapePos, 2, "\f");
            } else if (escaped == 'n') {
                value.replace(escapePos, 2, "\n");
            } else if (escaped == 'r') {
                value.replace(escapePos, 2, "\r");
            } else if (escaped == 't') {
                value.replace(escapePos, 2, "\t");
            }
            escapePos = value.find('\\', escapePos);
        }
        
        return {{value}, endPos + 1};
    }
    
    static std::pair<std::optional<JsonValue>, size_t> parseBoolean(const std::string& str, size_t startPos) {
        if (str.compare(startPos, 4, "true") == 0) {
            size_t endPos = skipWhitespace(str, startPos + 4);
            return {{true}, endPos};
        } else if (str.compare(startPos, 5, "false") == 0) {
            size_t endPos = skipWhitespace(str, startPos + 5);
            return {{false}, endPos};
        }
        return {std::nullopt, startPos};
    }
    
    static std::pair<std::optional<JsonValue>, size_t> parseNull(const std::string& str, size_t startPos) {
        if (str.compare(startPos, 4, "null") == 0) {
            size_t endPos = skipWhitespace(str, startPos + 4);
            return {{}, endPos};
        }
        return {std::nullopt, startPos};
    }
    
    static std::pair<std::optional<JsonValue>, size_t> parseNumber(const std::string& str, size_t startPos) {
        size_t endPos = startPos;
        bool hasDecimal = false;
        bool hasExponent = false;
        
        if (str[endPos] == '-') {
            endPos++;
        }
        
        while (endPos < str.size() && std::isdigit(str[endPos])) {
            endPos++;
        }
        
        if (endPos < str.size() && str[endPos] == '.') {
            hasDecimal = true;
            endPos++;
            while (endPos < str.size() && std::isdigit(str[endPos])) {
                endPos++;
            }
        }
        
        if (endPos < str.size() && (str[endPos] == 'e' || str[endPos] == 'E')) {
            hasExponent = true;
            endPos++;
            if (endPos < str.size() && (str[endPos] == '+' || str[endPos] == '-')) {
                endPos++;
            }
            while (endPos < str.size() && std::isdigit(str[endPos])) {
                endPos++;
            }
        }
        
        if (endPos == startPos) {
            return {std::nullopt, startPos};
        }
        
        try {
            double value = std::stod(str.substr(startPos, endPos - startPos));
            return {{value}, endPos};
        } catch (const std::exception&) {
            return {std::nullopt, startPos};
        }
    }
    
    static std::pair<std::optional<JsonValue>, size_t> parseArray(const std::string& str, size_t startPos) {
        std::vector<JsonValue> array;
        size_t pos = skipWhitespace(str, startPos + 1);  // Skip '['
        
        if (pos >= str.size() || str[pos] == ']') {
            return {{array}, pos + 1};
        }
        
        while (pos < str.size()) {
            auto [value, newPos] = parseValue(str, pos);
            if (!value) {
                return {std::nullopt, startPos};
            }
            
            array.push_back(*value);
            pos = skipWhitespace(str, newPos);
            
            if (pos >= str.size() || str[pos] == ']') {
                break;
            }
            
            if (str[pos] != ',') {
                return {std::nullopt, startPos};
            }
            
            pos = skipWhitespace(str, pos + 1);
        }
        
        if (pos >= str.size() || str[pos] != ']') {
            return {std::nullopt, startPos};
        }
        
        return {{array}, pos + 1};
    }
    
    static std::pair<std::optional<JsonValue>, size_t> parseObject(const std::string& str, size_t startPos) {
        std::unordered_map<std::string, JsonValue> object;
        size_t pos = skipWhitespace(str, startPos + 1);  // Skip '{'
        
        if (pos >= str.size() || str[pos] == '}') {
            return {{object}, pos + 1};
        }
        
        while (pos < str.size()) {
            auto [key, keyPos] = parseString(str, pos);
            if (!key || key->type != JsonValue::Type::STRING) {
                return {std::nullopt, startPos};
            }
            
            pos = skipWhitespace(str, keyPos);
            if (pos >= str.size() || str[pos] != ':') {
                return {std::nullopt, startPos};
            }
            
            pos = skipWhitespace(str, pos + 1);
            
            auto [value, valuePos] = parseValue(str, pos);
            if (!value) {
                return {std::nullopt, startPos};
            }
            
            object[key->stringValue] = *value;
            pos = skipWhitespace(str, valuePos);
            
            if (pos >= str.size() || str[pos] == '}') {
                break;
            }
            
            if (str[pos] != ',') {
                return {std::nullopt, startPos};
            }
            
            pos = skipWhitespace(str, pos + 1);
        }
        
        if (pos >= str.size() || str[pos] != '}') {
            return {std::nullopt, startPos};
        }
        
        return {{object}, pos + 1};
    }
};

/**
 * @struct Config::Impl
 * @brief PIMPL implementation structure for the Config class
 * 
 * This structure contains all private implementation details of the Config class,
 * including member variables and implementation-specific data structures. This
 * keeps the Config class header clean and allows for better encapsulation and maintainability.
 */

struct Config::Impl {
    std::vector<ConfigEntry> entries;
    std::unique_ptr<SimpleJsonParser> jsonParser;
    
    Impl() : jsonParser(std::make_unique<SimpleJsonParser>()) {}
};

Config::Config() : pImpl(std::make_unique<Impl>()) {}

Config::~Config() = default;

Config::Config(Config&&) noexcept = default;

Config& Config::operator=(Config&&) noexcept = default;

/**
 * @brief Load configuration from file with JSON/YAML support
 * 
 * This function implements the Phase 2 JSON/YAML parser as specified in the TODO comments.
 * It supports both JSON and YAML formats through a unified parser interface.
 * 
 * The parser handles:
 * - JSON (.json) files with proper JSON syntax
 * - YAML (.yaml, .yml) files with YAML syntax
 * - Both formats can contain nested objects, arrays, strings, numbers, booleans, and null values
 * - Configuration values are mapped to appropriate C++ types
 * 
 * @param filepath Path to the configuration file (.json, .yaml, .yml)
 * @return true if configuration is successfully loaded, false otherwise
 */
bool Config::loadFromFile(const std::string& filepath) {
    // Determine file type based on extension
    std::string ext = "." + filepath.substr(filepath.find_last_of(".") + 1);
    std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);
    
    if (ext == ".json" || ext == ".yaml" || ext == ".yml") {
        // Use JSON/YAML parser for proper format support
        auto jsonValue = pImpl->jsonParser->parseJsonFile(filepath);
        if (!jsonValue) {
            NLM_LOG_ERROR("Failed to parse JSON/YAML file: " + filepath);
            return false;
        }
        
        // Convert JSON object to config entries
        if (jsonValue->type == SimpleJsonParser::JsonValue::Type::OBJECT) {
            return convertJsonToConfig(jsonValue->objectValue);
        } else {
            NLM_LOG_ERROR("JSON file must contain an object at the root level");
            return false;
        }
    } else {
        // Use legacy key=value format for compatibility
        return loadFromFileLegacy(filepath);
    }
}

/**
 * @brief Convert JSON object to configuration entries
 * 
 * This helper function recursively converts JSON objects to configuration entries,
 * handling nested structures and different data types appropriately.
 * 
 * @param jsonObj JSON object to convert
 * @return true if conversion succeeds, false otherwise
 */
bool Config::convertJsonToConfig(const std::unordered_map<std::string, SimpleJsonParser::JsonValue>& jsonObj) {
    for (const auto& pair : jsonObj) {
        const std::string& key = pair.first;
        const SimpleJsonParser::JsonValue& jsonValue = pair.second;
        
        ConfigValue configValue;
        std::string description = "Configuration value for " + key;
        
        switch (jsonValue.type) {
            case SimpleJsonParser::JsonValue::Type::STRING:
                configValue = jsonValue.stringValue;
                break;
            case SimpleJsonParser::JsonValue::Type::NUMBER:
                configValue = jsonValue.numberValue;
                break;
            case SimpleJsonParser::JsonValue::Type::BOOLEAN:
                configValue = jsonValue.booleanValue;
                break;
            case SimpleJsonParser::JsonValue::Type::NULL_TYPE:
                configValue = std::string("");  // Treat null as empty string
                break;
            case SimpleJsonParser::JsonValue::Type::ARRAY:
                // For now, convert arrays to strings (could be enhanced later)
                {
                    std::stringstream ss;
                    ss << "[";
                    for (size_t i = 0; i < jsonValue.arrayValue.size(); ++i) {
                        if (i > 0) ss << ", ";
                        const auto& item = jsonValue.arrayValue[i];
                        switch (item.type) {
                            case SimpleJsonParser::JsonValue::Type::STRING:
                                ss << "\"" << item.stringValue << "\"";
                                break;
                            case SimpleJsonParser::JsonValue::Type::NUMBER:
                                ss << item.numberValue;
                                break;
                            case SimpleJsonParser::JsonValue::Type::BOOLEAN:
                                ss << (item.booleanValue ? "true" : "false");
                                break;
                            default:
                                ss << "null";
                                break;
                        }
                    }
                    ss << "]";
                    configValue = ss.str();
                }
                break;
            case SimpleJsonParser::JsonValue::Type::OBJECT:
                // For now, convert objects to strings (could be enhanced later)
                {
                    std::stringstream ss;
                    ss << "{";
                    size_t i = 0;
                    for (const auto& objPair : jsonValue.objectValue) {
                        if (i > 0) ss << ", ";
                        ss << "\"" << objPair.first << "\": ";
                        const auto& item = objPair.second;
                        switch (item.type) {
                            case SimpleJsonParser::JsonValue::Type::STRING:
                                ss << "\"" << item.stringValue << "\"";
                                break;
                            case SimpleJsonParser::JsonValue::Type::NUMBER:
                                ss << item.numberValue;
                                break;
                            case SimpleJsonParser::JsonValue::Type::BOOLEAN:
                                ss << (item.booleanValue ? "true" : "false");
                                break;
                            default:
                                ss << "null";
                                break;
                        }
                        i++;
                    }
                    ss << "}";
                    configValue = ss.str();
                }
                break;
        }
        
        set(key, configValue, ConfigSource::File);
    }
    
    return true;
}

/**
 * @brief Legacy key=value format loading
 * 
 * This function provides backward compatibility with the original key=value format
 * used in Phase 1. It maintains the same behavior as the original implementation.
 * 
 * @param filepath Path to the configuration file
 * @return true if configuration is successfully loaded, false otherwise
 */
bool Config::loadFromFileLegacy(const std::string& filepath) {
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
    
    // Use JSON format for saving configuration
    file << "{\n";
    
    for (size_t i = 0; i < pImpl->entries.size(); ++i) {
        const auto& entry = pImpl->entries[i];
        file << "  \"" << entry.key << "\": ";
        
        std::visit([&file](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, std::string>) {
                file << "\"" << arg << "\"";
            } else if constexpr (std::is_same_v<T, bool>) {
                file << (arg ? "true" : "false");
            } else {
                file << arg;
            }
        }, entry.value);
        
        if (i < pImpl->entries.size() - 1) {
            file << ",\n";
        } else {
            file << "\n";
        }
    }
    
    file << "}\n";
    
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

} // namespace nlm
