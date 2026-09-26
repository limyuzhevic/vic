#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <optional>
#include <string>
#include <stdexcept>
#include <cctype>

namespace nlm {

// JSON parsing implementation
class JSONParser {
public:
    struct JSONValue {
        enum Type {
            NULL_TYPE,
            BOOLEAN,
            NUMBER,
            STRING,
            ARRAY,
            OBJECT
        };
        
        Type type;
        bool boolValue;
        double numberValue;
        std::string stringValue;
        std::vector<JSONValue> arrayValue;
        std::vector<std::pair<std::string, JSONValue>> objectValue;
        
        JSONValue() : type(NULL_TYPE) {}
        JSONValue(bool value) : type(BOOLEAN), boolValue(value) {}
        JSONValue(double value) : type(NUMBER), numberValue(value) {}
        JSONValue(int value) : type(NUMBER), numberValue(static_cast<double>(value)) {}
        JSONValue(const std::string& value) : type(STRING), stringValue(value) {}
        JSONValue(const char* value) : type(STRING), stringValue(value) {}
        JSONValue(const std::vector<JSONValue>& value) : type(ARRAY), arrayValue(value) {}
        JSONValue(const std::vector<std::pair<std::string, JSONValue>>& value) : type(OBJECT), objectValue(value) {}
    };
    
    static JSONValue parse(const std::string& jsonStr);
    static std::string toString(const JSONValue& value);
    static bool tryGetString(const JSONValue& value, std::string& result);
    static bool tryGetDouble(const JSONValue& value, double& result);
    static bool tryGetInt64(const JSONValue& value, int64_t& result);
    static bool tryGetBool(const JSONValue& value, bool& result);
    static bool tryGetArray(const JSONValue& value, const std::vector<JSONValue>&& result);
};

// JSON Parser Implementation
JSONParser::JSONValue JSONParser::parse(const std::string& jsonStr) {
    JSONValue result;
    
    if (jsonStr.empty()) {
        return result;
    }
    
    const char* str = jsonStr.c_str();
    size_t pos = 0;
    
    auto trim = [](const std::string& s) {
        size_t start = s.find_first_not_of(" \t\n\r");
        if (start == std::string::npos) return std::string();
        size_t end = s.find_last_not_of(" \t\n\r");
        return s.substr(start, end - start + 1);
    };
    
    auto skipWhitespace = [&]() {
        while (pos < jsonStr.length() && std::isspace(str[pos])) {
            pos++;
        }
    };
    
    auto parseString = [&]() -> JSONValue {
        skipWhitespace();
        if (pos >= jsonStr.length() || str[pos] != '"') {
            throw std::runtime_error("Expected string start\"");
        }
        pos++;
        std::string result;
        while (pos < jsonStr.length() && str[pos] != '"') {
            if (str[pos] == '\\') {
                pos++;
                if (pos >= jsonStr.length()) {
                    throw std::runtime_error("Invalid escape sequence");
                }
                switch (str[pos]) {
                    case '\\': result += '\\'; break;
                    case '\"': result += '"'; break;
                    case '/': result += '/'; break;
                    case 'b': result += '\b'; break;
                    case 'f': result += '\f'; break;
                    case 'n': result += '\n'; break;
                    case 'r': result += '\r'; break;
                    case 't': result += '\t'; break;
                    case 'u': {
                        if (pos + 4 >= jsonStr.length()) {
                            throw std::runtime_error("Invalid Unicode escape");
                        }
                        // Skip hex digits
                        pos += 4;
                        break;
                    }
                    default:
                        throw std::runtime_error("Invalid escape sequence");
                }
            } else {
                result += str[pos];
            }
            pos++;
        }
        if (pos >= jsonStr.length() || str[pos] != '"') {
            throw std::runtime_error("Unterminated string");
        }
        pos++;
        return JSONValue(result);
    };
    
    auto parseNumber = [&]() -> JSONValue {
        skipWhitespace();
        size_t start = pos;
        bool negative = false;
        if (str[pos] == '-') {
            negative = true;
            pos++;
        }
        if (pos >= jsonStr.length() || !std::isdigit(str[pos])) {
            throw std::runtime_error("Expected digit in number");
        }
        while (pos < jsonStr.length() && (std::isdigit(str[pos]) || str[pos] == '.' || str[pos] == 'e' || str[pos] == 'E' || str[pos] == '+' || str[pos] == '-')) {
            pos++;
        }
        std::string numStr = jsonStr.substr(start, pos - start);
        try {
            size_t idx;
            double value = std::stod(numStr, &idx);
            if (idx != numStr.length()) {
                throw std::runtime_error("Invalid number format");
            }
            return JSONValue(value);
        } catch (...) {
            throw std::runtime_error("Invalid number format: " + numStr);
        }
    };
    
    auto parseBool = [&]() -> JSONValue {
        skipWhitespace();
        if (pos + 4 <= jsonStr.length() && strncmp(str + pos, "true", 4) == 0) {
            pos += 4;
            return JSONValue(true);
        } else if (pos + 5 <= jsonStr.length() && strncmp(str + pos, "false", 5) == 0) {
            pos += 5;
            return JSONValue(false);
        }
        throw std::runtime_error("Expected true or false");
    };
    
    auto parseNull = [&]() -> JSONValue {
        skipWhitespace();
        if (pos + 4 <= jsonStr.length() && strncmp(str + pos, "null", 4) == 0) {
            pos += 4;
            return JSONValue();
        }
        throw std::runtime_error("Expected null");
    };
    
    auto parseArray = [&]() -> JSONValue {
        skipWhitespace();
        if (pos >= jsonStr.length() || str[pos] != '[') {
            throw std::runtime_error("Expected array start['");
        }
        pos++;
        std::vector<JSONValue> array;
        skipWhitespace();
        if (str[pos] != ']') {
            while (true) {
                array.push_back(parse(jsonStr.substr(pos)));
                skipWhitespace();
                if (str[pos] == ']') {
                    pos++;
                    break;
                } else if (str[pos] == ',') {
                    pos++;
                    skipWhitespace();
                    if (str[pos] == ']') {
                        throw std::runtime_error("Trailing comma in array");
                    }
                } else {
                    throw std::runtime_error("Expected , or ] in array");
                }
            }
        } else {
            pos++;
        }
        return JSONValue(array);
    };
    
    auto parseObject = [&]() -> JSONValue {
        skipWhitespace();
        if (pos >= jsonStr.length() || str[pos] != '{') {
            throw std::runtime_error("Expected object start{");
        }
        pos++;
        std::vector<std::pair<std::string, JSONValue>> object;
        skipWhitespace();
        if (str[pos] != '}') {
            while (true) {
                std::string key = parseString().stringValue;
                skipWhitespace();
                if (str[pos] != ':') {
                    throw std::runtime_error("Expected : in object");
                }
                pos++;
                object.push_back(std::make_pair(key, parse(jsonStr.substr(pos))));
                skipWhitespace();
                if (str[pos] == '}') {
                    pos++;
                    break;
                } else if (str[pos] == ',') {
                    pos++;
                    skipWhitespace();
                    if (str[pos] == '}') {
                        throw std::runtime_error("Trailing comma in object");
                    }
                } else {
                    throw std::runtime_error("Expected , or } in object");
                }
            }
        } else {
            pos++;
        }
        return JSONValue(object);
    };
    
    skipWhitespace();
    if (pos >= jsonStr.length()) {
        throw std::runtime_error("Empty JSON string");
    }
    
    switch (str[pos]) {
        case '{': result = parseObject(); break;
        case '[': result = parseArray(); break;
        case '"': result = parseString(); break;
        case 't': case 'f': result = parseBool(); break;
        case 'n': result = parseNull(); break;
        default: result = parseNumber(); break;
    }
    
    skipWhitespace();
    if (pos != jsonStr.length()) {
        throw std::runtime_error("Extra data after JSON");
    }
    
    return result;
}

std::string JSONParser::toString(const JSONParser::JSONValue& value) {
    std::ostringstream oss;
    switch (value.type) {
        case JSONValue::NULL_TYPE:
            oss << "null";
            break;
        case JSONValue::BOOLEAN:
            oss << (value.boolValue ? "true" : "false");
            break;
        case JSONValue::NUMBER:
            oss << value.numberValue;
            break;
        case JSONValue::STRING:
            oss << '"' << value.stringValue << '"';
            break;
        case JSONValue::ARRAY: {
            oss << '[';
            for (size_t i = 0; i < value.arrayValue.size(); ++i) {
                if (i > 0) oss << ', ';
                oss << toString(value.arrayValue[i]);
            }
            oss << ']';
            break;
        }
        case JSONValue::OBJECT: {
            oss << '{';
            for (size_t i = 0; i < value.objectValue.size(); ++i) {
                if (i > 0) oss << ', ';
                oss << '"' << value.objectValue[i].first << "": " << toString(value.objectValue[i].second);
            }
            oss << '}';
            break;
        }
    }
    return oss.str();
}

bool JSONParser::tryGetString(const JSONValue& value, std::string& result) {
    if (value.type == STRING) {
        result = value.stringValue;
        return true;
    }
    return false;
}

bool JSONParser::tryGetDouble(const JSONValue& value, double& result) {
    if (value.type == NUMBER) {
        result = value.numberValue;
        return true;
    }
    return false;
}

bool JSONParser::tryGetInt64(const JSONValue& value, int64_t& result) {
    if (value.type == NUMBER) {
        result = static_cast<int64_t>(value.numberValue);
        return true;
    }
    return false;
}

bool JSONParser::tryGetBool(const JSONValue& value, bool& result) {
    if (value.type == BOOLEAN) {
        result = value.boolValue;
        return true;
    }
    return false;
}

bool JSONParser::tryGetArray(const JSONValue& value, const std::vector<JSONValue>&& result) {
    if (value.type == ARRAY) {
        // This is a helper for external use
        return true;
    }
    return false;
}

// Helper functions to convert JSON to Config
void convertJSONToConfig(const JSONParser::JSONValue& jsonValue, Config& config, const std::string& prefix = "") {
    switch (jsonValue.type) {
        case JSONParser::JSONValue::NULL_TYPE:
            // Skip null values
            break;
        case JSONParser::JSONValue::BOOLEAN:
            config.set(prefix, jsonValue.boolValue, ConfigSource::File);
            break;
        case JSONParser::JSONValue::NUMBER: {
            // Try to determine if it's an int or double
            bool isInt = std::floor(jsonValue.numberValue) == jsonValue.numberValue;
            if (isInt) {
                config.set(prefix, static_cast<int64_t>(jsonValue.numberValue), ConfigSource::File);
            } else {
                config.set(prefix, jsonValue.numberValue, ConfigSource::File);
            }
            break;
        }
        case JSONParser::JSONValue::STRING:
            config.set(prefix, jsonValue.stringValue, ConfigSource::File);
            break;
        case JSONParser::JSONValue::ARRAY:
            // Convert array to string representation for now
            // TODO: Implement proper array handling
            config.set(prefix, JSONParser::toString(jsonValue), ConfigSource::File);
            break;
        case JSONParser::JSONValue::OBJECT:
            for (const auto& pair : jsonValue.objectValue) {
                std::string newKey = prefix.empty() ? pair.first : prefix + "." + pair.first;
                convertJSONToConfig(pair.second, config, newKey);
            }
            break;
    }
}

bool Config::loadFromFile(const std::string& filepath) {
    try {
        // Determine file format based on extension
        std::string extension = "." + std::filesystem::path(filepath).extension().string();
        std::transform(extension.begin(), extension.end(), extension.begin(), ::tolower);
        
        std::ifstream file(filepath);
        if (!file.is_open()) {
            throw std::runtime_error("Cannot open file: " + filepath);
        }
        
        std::string content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
        
        // Parse based on file format
        JSONParser::JSONValue jsonData;
        if (extension == ".json") {
            jsonData = JSONParser::parse(content);
        } else if (extension == ".yaml" || extension == ".yml") {
            // TODO: Add YAML parsing support
            throw std::runtime_error("YAML parsing not yet implemented");
        } else {
            // Try to parse as JSON first, if fails, fall back to simple key=value format
            try {
                jsonData = JSONParser::parse(content);
            } catch (...) {
                // Fall back to simple format parsing
                file.clear();
                file.seekg(0);
                return parseSimpleFormat(file);
            }
        }
        
        // Convert JSON to configuration
        convertJSONToConfig(jsonData, *this);
        
        return true;
        
    } catch (const std::exception& e) {
        // Log error (using simple error handling since logger may not be initialized yet)
        std::cerr << "Error loading configuration from " << filepath << ": " << e.what() << std::endl;
        return false;
    }
}

bool Config::parseSimpleFormat(std::ifstream& file) {
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

bool Config::saveToFile(const std::string& filepath) const {
    try {
        // Determine file format based on extension
        std::string extension = "." + std::filesystem::path(filepath).extension().string();
        std::transform(extension.begin(), extension.end(), extension.begin(), ::tolower);
        
        std::ofstream file(filepath);
        if (!file.is_open()) {
            throw std::runtime_error("Cannot open file for writing: " + filepath);
        }
        
        // Convert configuration to JSON structure
        JSONParser::JSONValue jsonValue;
        std::vector<std::pair<std::string, JSONParser::JSONValue>> jsonObject;
        
        for (const auto& entry : pImpl->entries) {
            // Convert ConfigEntry to JSON
            JSONParser::JSONValue jsonConfigValue;
            
            // Convert based on value type
            std::visit([&](const auto& val) {
                using T = std::decay_t<decltype(val)>;
                if constexpr (std::is_same_v<T, int>) {
                    jsonConfigValue = JSONParser::JSONValue(static_cast<double>(val));
                } else if constexpr (std::is_same_v<T, int64_t>) {
                    jsonConfigValue = JSONParser::JSONValue(static_cast<double>(val));
                } else if constexpr (std::is_same_v<T, double>) {
                    jsonConfigValue = JSONParser::JSONValue(val);
                } else if constexpr (std::is_same_v<T, bool>) {
                    jsonConfigValue = JSONParser::JSONValue(val);
                } else if constexpr (std::is_same_v<T, std::string>) {
                    jsonConfigValue = JSONParser::JSONValue(val);
                } else if constexpr (std::is_same_v<T, std::vector<int>>) {
                    // Convert int vector to double vector
                    std::vector<JSONParser::JSONValue> arr;
                    for (auto& item : val) {
                        arr.push_back(JSONParser::JSONValue(static_cast<double>(item)));
                    }
                    jsonConfigValue = JSONParser::JSONValue(arr);
                } else if constexpr (std::is_same_v<T, std::vector<double>>) {
                    // Convert double vector to JSON array
                    std::vector<JSONParser::JSONValue> arr;
                    for (auto& item : val) {
                        arr.push_back(JSONParser::JSONValue(item));
                    }
                    jsonConfigValue = JSONParser::JSONValue(arr);
                } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
                    // Convert string vector to JSON array
                    std::vector<JSONParser::JSONValue> arr;
                    for (auto& item : val) {
                        arr.push_back(JSONParser::JSONValue(item));
                    }
                    jsonConfigValue = JSONParser::JSONValue(arr);
                }
            }, entry.value);
            
            jsonObject.push_back(std::make_pair(entry.key, jsonConfigValue));
        }
        
        jsonValue = JSONParser::JSONValue(jsonObject);
        
        // Write JSON to file
        std::string jsonStr;
        if (extension == ".json") {
            jsonStr = JSONParser::toString(jsonValue);
            file << jsonStr;
        } else {
            // For other formats, write in simple format as fallback
            for (const auto& entry : pImpl->entries) {
                file << "# " << entry.description << "\n";
                std::visit([&](const auto& val) {
                    using T = std::decay_t<decltype(val)>;
                    if constexpr (std::is_same_v<T, std::string>) {
                        file << entry.key << " = \"" << val << "\"\n";
                    } else if constexpr (std::is_same_v<T, bool>) {
                        file << entry.key << " = " << (val ? "true" : "false") << "\n";
                    } else if constexpr (std::is_same_v<T, double>) {
                        file << entry.key << " = " << val << "\n";
                    } else if constexpr (std::is_same_v<T, int> || std::is_same_v<T, int64_t>) {
                        file << entry.key << " = " << val << "\n";
                    }
                }, entry.value);
            }
        }
        
        file.flush();
        return true;
        
    } catch (const std::exception& e) {
        std::cerr << "Error saving configuration to " << filepath << ": " << e.what() << std::endl;
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
