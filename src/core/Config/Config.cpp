// Configuration parser implementation
#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <cmath>
#include <cassert>
#include <regex>
#include <iomanip>

namespace nlm {

namespace {

// Simple JSON parser implementation
class SimpleJsonParser {
public:
    struct JsonValue {
        enum class Type { Null, String, Number, Boolean, Object, Array };
        
        Type type;
        std::string stringValue;
        double numberValue;
        bool booleanValue;
        std::vector<std::pair<std::string, JsonValue>> objectValue;
        std::vector<JsonValue> arrayValue;
        
        JsonValue() : type(Type::Null) {}
        JsonValue(const std::string& s) : type(Type::String), stringValue(s) {}
        JsonValue(double n) : type(Type::Number), numberValue(n) {}
        JsonValue(bool b) : type(Type::Boolean), booleanValue(b) {}
        JsonValue(const std::vector<std::pair<std::string, JsonValue>>& obj) : type(Type::Object), objectValue(obj) {}
        JsonValue(const std::vector<JsonValue>& arr) : type(Type::Array), arrayValue(arr) {}
    };
    
    bool parse(const std::string& json, JsonValue& result) {
        skipWhitespace(json, 0);
        return parseValue(json, 0, result);
    }
    
private:
    bool parseValue(const std::string& json, size_t& pos, JsonValue& result) {
        skipWhitespace(json, pos);
        
        if (pos >= json.size()) return false;
        
        char c = json[pos];
        
        if (c == '{') {
            return parseObject(json, pos, result);
        } else if (c == '[') {
            return parseArray(json, pos, result);
        } else if (c == '"') {
            return parseString(json, pos, result);
        } else if (c == 't' || c == 'f') {
            return parseBoolean(json, pos, result);
        } else if (c == 'n') {
            return parseNull(json, pos, result);
        } else {
            return parseNumber(json, pos, result);
        }
    }
    
    bool parseObject(const std::string& json, size_t& pos, JsonValue& result) {
        if (json[pos] != '{') return false;
        pos++;
        skipWhitespace(json, pos);
        
        result.objectValue.clear();
        
        if (json[pos] == '}') {
            pos++;
            return true;
        }
        
        while (true) {
            skipWhitespace(json, pos);
            
            // Parse key
            if (json[pos] != '"') return false;
            JsonValue key;
            if (!parseString(json, pos, key)) return false;
            
            skipWhitespace(json, pos);
            if (pos >= json.size() || json[pos] != ':') return false;
            pos++;
            
            // Parse value
            JsonValue value;
            if (!parseValue(json, pos, value)) return false;
            
            result.objectValue.push_back({key.stringValue, value});
            
            skipWhitespace(json, pos);
            
            if (json[pos] == '}') {
                pos++;
                break;
            } else if (json[pos] == ',') {
                pos++;
            } else {
                return false;
            }
        }
        
        return true;
    }
    
    bool parseArray(const std::string& json, size_t& pos, JsonValue& result) {
        if (json[pos] != '[') return false;
        pos++;
        skipWhitespace(json, pos);
        
        result.arrayValue.clear();
        
        if (json[pos] == ']') {
            pos++;
            return true;
        }
        
        while (true) {
            JsonValue element;
            if (!parseValue(json, pos, element)) return false;
            
            result.arrayValue.push_back(element);
            
            skipWhitespace(json, pos);
            
            if (json[pos] == ']') {
                pos++;
                break;
            } else if (json[pos] == ',') {
                pos++;
            } else {
                return false;
            }
        }
        
        return true;
    }
    
    bool parseString(const std::string& json, size_t& pos, JsonValue& result) {
        if (json[pos] != '"') return false;
        pos++;
        
        size_t start = pos;
        while (pos < json.size() && json[pos] != '"') {
            if (json[pos] == '\\' && pos + 1 < json.size()) {
                pos += 2;  // Skip escaped character
            } else {
                pos++;
            }
        }
        
        if (pos >= json.size()) return false;
        
        result.stringValue = json.substr(start, pos - start);
        pos++; // Skip closing quote
        
        return true;
    }
    
    bool parseBoolean(const std::string& json, size_t& pos, JsonValue& result) {
        if (json.substr(pos, 4) == "true") {
            result.booleanValue = true;
            pos += 4;
            return true;
        } else if (json.substr(pos, 5) == "false") {
            result.booleanValue = false;
            pos += 5;
            return true;
        }
        return false;
    }
    
    bool parseNull(const std::string& json, size_t& pos, JsonValue& result) {
        if (json.substr(pos, 4) == "null") {
            pos += 4;
            return true;
        }
        return false;
    }
    
    bool parseNumber(const std::string& json, size_t& pos, JsonValue& result) {
        size_t start = pos;
        bool isDecimal = false;
        bool isExponent = false;
        
        while (pos < json.size()) {
            char c = json[pos];
            if (c >= '0' && c <= '9') {
                pos++;
            } else if (c == '.' && !isDecimal && !isExponent) {
                isDecimal = true;
                pos++;
            } else if ((c == 'e' || c == 'E') && !isExponent) {
                isExponent = true;
                pos++;
                if (pos < json.size() && (json[pos] == '+' || json[pos] == '-')) {
                    pos++;
                }
            } else if (c == '-' && pos == start) {
                pos++;
            } else {
                break;
            }
        }
        
        if (pos == start) return false;
        
        try {
            result.numberValue = std::stod(json.substr(start, pos - start));
        } catch (...) {
            return false;
        }
        
        return true;
    }
    
    void skipWhitespace(const std::string& json, size_t& pos) {
        while (pos < json.size() && std::isspace(json[pos])) {
            pos++;
        }
    }
    
    void skipWhitespace(const std::string& json, size_t start) {
        (void)start;  // Unused parameter
    }
};

// Helper functions for Config
bool isValidKey(const std::string& key) {
    if (key.empty()) return false;
    for (char c : key) {
        if (!std::isalnum(c) && c != '_' && c != '.' && c != '-') {
            return false;
        }
    }
    return true;
}

std::string getTypeName(const ConfigValue& value) {
    return std::visit([](auto&& arg) -> std::string {
        using T = std::decay_t<decltype(arg)>;
        if constexpr (std::is_same_v<T, int>) return "int";
        else if constexpr (std::is_same_v<T, int64_t>) return "int64_t";
        else if constexpr (std::is_same_v<T, double>) return "double";
        else if constexpr (std::is_same_v<T, bool>) return "bool";
        else if constexpr (std::is_same_v<T, std::string>) return "string";
        else if constexpr (std::is_same_v<T, std::vector<int>>) return "list[int]";
        else if constexpr (std::is_same_v<T, std::vector<double>>) return "list[float]";
        else if constexpr (std::is_same_v<T, std::vector<std::string>>) return "list[str]";
        else return "unknown";
    }, value);
}

} // namespace

struct Config::Impl {
    std::vector<ConfigEntry> entries;
    SimpleJsonParser jsonParser;
    std::string lastError;
    
    // Forbid copying
    Impl(const Impl&) = delete;
    Impl& operator=(const Impl&) = delete;
    
    Impl() {}
    
    bool setFromJson(const SimpleJsonParser::JsonValue& json) {
        if (json.type != SimpleJsonParser::JsonValue::Type::Object) {
            lastError = "JSON root must be an object";
            return false;
        }
        
        clear();
        
        for (const auto& pair : json.objectValue) {
            const std::string& key = pair.first;
            const SimpleJsonParser::JsonValue& value = pair.second;
            
            if (!isValidKey(key)) {
                lastError = "Invalid key name: " + key;
                return false;
            }
            
            ConfigValue configValue;
            bool success = convertJsonToConfig(value, configValue);
            if (!success) {
                lastError = "Failed to convert JSON value for key: " + key;
                return false;
            }
            
            entries.emplace_back(key, configValue, ConfigSource::File);
        }
        
        return true;
    }
    
    bool convertJsonToConfig(const SimpleJsonParser::JsonValue& json, ConfigValue& configValue) {
        switch (json.type) {
            case SimpleJsonParser::JsonValue::Type::Null:
                configValue = std::string("");
                return true;
            case SimpleJsonParser::JsonValue::Type::String:
                configValue = json.stringValue;
                return true;
            case SimpleJsonParser::JsonValue::Type::Number: {
                // Determine if it's an integer or double
                if (std::floor(json.numberValue) == json.numberValue && 
                    json.numberValue >= std::numeric_limits<int>::min() && 
                    json.numberValue <= std::numeric_limits<int>::max()) {
                    configValue = static_cast<int>(json.numberValue);
                } else {
                    configValue = json.numberValue;
                }
                return true;
            }
            case SimpleJsonParser::JsonValue::Type::Boolean:
                configValue = json.booleanValue;
                return true;
            case SimpleJsonParser::JsonValue::Type::Array:
                return convertJsonArrayToConfig(json.arrayValue, configValue);
            case SimpleJsonParser::JsonValue::Type::Object:
                // For now, convert object to string representation
                configValue = jsonToString(json);
                return true;
        }
        return false;
    }
    
    bool convertJsonArrayToConfig(const std::vector<SimpleJsonParser::JsonValue>& jsonArray, ConfigValue& configValue) {
        if (jsonArray.empty()) {
            configValue = std::vector<std::string>{};
            return true;
        }
        
        // Check if all elements are strings
        bool allStrings = true;
        for (const auto& elem : jsonArray) {
            if (elem.type != SimpleJsonParser::JsonValue::Type::String) {
                allStrings = false;
                break;
            }
        }
        
        if (allStrings) {
            std::vector<std::string> strArray;
            strArray.reserve(jsonArray.size());
            for (const auto& elem : jsonArray) {
                strArray.push_back(elem.stringValue);
            }
            configValue = strArray;
            return true;
        }
        
        // Check if all elements are numbers
        bool allNumbers = true;
        for (const auto& elem : jsonArray) {
            if (elem.type != SimpleJsonParser::JsonValue::Type::Number) {
                allNumbers = false;
                break;
            }
        }
        
        if (allNumbers) {
            // Try to determine if they're integers or doubles
            bool allIntegers = true;
            for (const auto& elem : jsonArray) {
                if (std::floor(elem.numberValue) != elem.numberValue) {
                    allIntegers = false;
                    break;
                }
            }
            
            if (allIntegers) {
                std::vector<int> intArray;
                intArray.reserve(jsonArray.size());
                for (const auto& elem : jsonArray) {
                    intArray.push_back(static_cast<int>(elem.numberValue));
                }
                configValue = intArray;
            } else {
                std::vector<double> doubleArray;
                doubleArray.reserve(jsonArray.size());
                for (const auto& elem : jsonArray) {
                    doubleArray.push_back(elem.numberValue);
                }
                configValue = doubleArray;
            }
            return true;
        }
        
        // Mixed types or unsupported - use string representation
        configValue = jsonToString(jsonArray);
        return true;
    }
    
    std::string jsonToString(const SimpleJsonParser::JsonValue& json) {
        std::ostringstream oss;
        
        switch (json.type) {
            case SimpleJsonParser::JsonValue::Type::Null:
                oss << "null";
                break;
            case SimpleJsonParser::JsonValue::Type::String:
                oss << '"' << json.stringValue << '"';
                break;
            case SimpleJsonParser::JsonValue::Type::Number:
                oss << json.numberValue;
                break;
            case SimpleJsonParser::JsonValue::Type::Boolean:
                oss << (json.booleanValue ? "true" : "false");
                break;
            case SimpleJsonParser::JsonValue::Type::Array:
                oss << '[';
                for (size_t i = 0; i < json.arrayValue.size(); ++i) {
                    if (i > 0) oss << ', ';
                    oss << jsonToString(json.arrayValue[i]);
                }
                oss << ']';
                break;
            case SimpleJsonParser::JsonValue::Type::Object:
                oss << '{';
                for (size_t i = 0; i < json.objectValue.size(); ++i) {
                    if (i > 0) oss << ', ';
                    oss << '"' << json.objectValue[i].first << "": jsonToString(json.objectValue[i].second);
                }
                oss << '}';
                break;
        }
        
        return oss.str();
    }
    
    std::string jsonToString(const std::vector<SimpleJsonParser::JsonValue>& jsonArray) {
        std::ostringstream oss;
        oss << '[';
        for (size_t i = 0; i < jsonArray.size(); ++i) {
            if (i > 0) oss << ', ';
            oss << jsonToString(jsonArray[i]);
        }
        oss << ']';
        return oss.str();
    }
    
    void clear() {
        entries.clear();
    }
};