#include "core/SimpleJsonParser.hpp"
#include "core/Config/Config.hpp"
#include <stdexcept>
#include <cctype>

namespace nlm {

bool SimpleJsonParser::parseJson(const std::string& jsonStr, JsonValue& outValue) {
    size_t pos = 0;
    skipWhitespace(jsonStr, pos);
    return parseJsonInternal(jsonStr, pos, outValue);
}

bool SimpleJsonParser::parseJsonFile(const std::string& filepath, JsonValue& outValue) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    std::stringstream buffer;
    buffer << file.rdbuf();
    std::string content = buffer.str();
    return parseJson(content, outValue);
}

std::string SimpleJsonParser::toJsonString(const JsonValue& value, int indent) {
    std::string result;
    std::string indentStr(indent * 2, ' ');
    
    std::visit([&](auto&& val) {
        using T = std::decay_t<decltype(val)>;
        if constexpr (std::is_same_v<T, int> || std::is_same_v<T, int64_t>) {
            result += std::to_string(val);
        } else if constexpr (std::is_same_v<T, double>) {
            result += std::to_string(val);
        } else if constexpr (std::is_same_v<T, bool>) {
            result += val ? "true" : "false";
        } else if constexpr (std::is_same_v<T, std::string>) {
            result += "\"" + val + "\"";
        } else if constexpr (std::is_same_v<T, std::vector<JsonValue>>) {
            result += "[\n";
            for (size_t i = 0; i < val.size(); ++i) {
                if (i > 0) result += ",\n";
                result += toJsonString(val[i], indent + 1);
            }
            result += "\n" + indentStr + "]";
        } else if constexpr (std::is_same_v<T, std::map<std::string, JsonValue>>) {
            result += "{\n";
            size_t i = 0;
            for (const auto& pair : val) {
                if (i++ > 0) result += ",\n";
                result += indentStr + "\"" + pair.first + "\": " + toJsonString(pair.second, indent + 1);
            }
            result += "\n" + indentStr + "}";
        }
    }, value);
    
    return result;
}

bool SimpleJsonParser::fromJsonValue(const JsonValue& value, const std::string& key, std::string& outValue) {
    if (std::holds_alternative<std::map<std::string, JsonValue>>(value)) {
        const auto& obj = std::get<std::map<std::string, JsonValue>>(value);
        auto it = obj.find(key);
        if (it != obj.end() && std::holds_alternative<std::string>(it->second)) {
            outValue = std::get<std::string>(it->second);
            return true;
        }
    }
    return false;
}

bool SimpleJsonParser::fromJsonValue(const JsonValue& value, const std::string& key, int& outValue) {
    if (std::holds_alternative<std::map<std::string, JsonValue>>(value)) {
        const auto& obj = std::get<std::map<std::string, JsonValue>>(value);
        auto it = obj.find(key);
        if (it != obj.end()) {
            if (std::holds_alternative<int>(it->second)) {
                outValue = std::get<int>(it->second);
                return true;
            } else if (std::holds_alternative<int64_t>(it->second)) {
                outValue = static_cast<int>(std::get<int64_t>(it->second));
                return true;
            } else if (std::holds_alternative<double>(it->second)) {
                outValue = static_cast<int>(std::get<double>(it->second));
                return true;
            }
        }
    }
    return false;
}

bool SimpleJsonParser::fromJsonValue(const JsonValue& value, const std::string& key, double& outValue) {
    if (std::holds_alternative<std::map<std::string, JsonValue>>(value)) {
        const auto& obj = std::get<std::map<std::string, JsonValue>>(value);
        auto it = obj.find(key);
        if (it != obj.end()) {
            if (std::holds_alternative<int>(it->second)) {
                outValue = static_cast<double>(std::get<int>(it->second));
                return true;
            } else if (std::holds_alternative<int64_t>(it->second)) {
                outValue = static_cast<double>(std::get<int64_t>(it->second));
                return true;
            } else if (std::holds_alternative<double>(it->second)) {
                outValue = std::get<double>(it->second);
                return true;
            }
        }
    }
    return false;
}

bool SimpleJsonParser::fromJsonValue(const JsonValue& value, const std::string& key, bool& outValue) {
    if (std::holds_alternative<std::map<std::string, JsonValue>>(value)) {
        const auto& obj = std::get<std::map<std::string, JsonValue>>(value);
        auto it = obj.find(key);
        if (it != obj.end() && std::holds_alternative<bool>(it->second)) {
            outValue = std::get<bool>(it->second);
            return true;
        }
    }
    return false;
}

bool SimpleJsonParser::parseJsonInternal(const std::string& str, size_t& pos, JsonValue& outValue) {
    skipWhitespace(str, pos);
    
    if (pos >= str.length()) {
        return false;
    }
    
    char c = str[pos];
    
    if (c == '{') {
        return parseObject(str, pos, outValue);
    } else if (c == '[') {
        return parseArray(str, pos, outValue);
    } else if (c == '"') {
        std::string s;
        if (parseString(str, pos, s)) {
            outValue = s;
            return true;
        }
        return false;
    } else if (std::isdigit(c) || c == '-') {
        JsonValue num;
        if (parseNumber(str, pos, num)) {
            outValue = num;
            return true;
        }
        return false;
    } else if (c == 't' || c == 'f') {
        JsonValue b;
        if (parseBool(str, pos, b)) {
            outValue = b;
            return true;
        }
        return false;
    } else if (c == 'n') {
        if (parseNull(str, pos)) {
            outValue = nullptr;
            return true;
        }
        return false;
    }
    
    return false;
}

void SimpleJsonParser::skipWhitespace(const std::string& str, size_t& pos) {
    while (pos < str.length() && std::isspace(str[pos])) {
        pos++;
    }
}

bool SimpleJsonParser::parseString(const std::string& str, size_t& pos, std::string& outValue) {
    if (pos >= str.length() || str[pos] != '"') {
        return false;
    }
    
    pos++; // Skip opening quote
    size_t start = pos;
    
    while (pos < str.length() && str[pos] != '"') {
        if (str[pos] == '\\' && pos + 1 < str.length()) {
            pos += 2; // Skip escaped character
        } else {
            pos++;
        }
    }
    
    if (pos >= str.length()) {
        return false; // Unterminated string
    }
    
    outValue = str.substr(start, pos - start);
    pos++; // Skip closing quote
    return true;
}

bool SimpleJsonParser::parseNumber(const std::string& str, size_t& pos, JsonValue& outValue) {
    size_t start = pos;
    bool hasDecimal = false;
    bool hasExp = false;
    
    if (str[pos] == '-') {
        pos++;
    }
    
    while (pos < str.length()) {
        char c = str[pos];
        if (std::isdigit(c)) {
            pos++;
        } else if (c == '.' && !hasDecimal) {
            hasDecimal = true;
            pos++;
        } else if ((c == 'e' || c == 'E') && !hasExp) {
            hasExp = true;
            pos++;
            if (pos < str.length() && (str[pos] == '+' || str[pos] == '-')) {
                pos++;
            }
        } else {
            break;
        }
    }
    
    if (pos == start) {
        return false;
    }
    
    std::string numStr = str.substr(start, pos - start);
    
    if (hasDecimal || hasExp) {
        try {
            double val = std::stod(numStr);
            outValue = val;
        } catch (...) {
            return false;
        }
    } else {
        try {
            if (numStr.find('.') != std::string::npos) {
                outValue = std::stoll(numStr);
            } else {
                outValue = std::stoll(numStr);
            }
        } catch (...) {
            return false;
        }
    }
    
    return true;
}

bool SimpleJsonParser::parseBool(const std::string& str, size_t& pos, JsonValue& outValue) {
    if (str.substr(pos, 4) == "true" && (pos + 4 == str.length() || !std::isalnum(str[pos + 4]))) {
        outValue = true;
        pos += 4;
        return true;
    } else if (str.substr(pos, 5) == "false" && (pos + 5 == str.length() || !std::isalnum(str[pos + 5]))) {
        outValue = false;
        pos += 5;
        return true;
    }
    return false;
}

bool SimpleJsonParser::parseNull(const std::string& str, size_t& pos) {
    if (str.substr(pos, 4) == "null" && (pos + 4 == str.length() || !std::isalnum(str[pos + 4]))) {
        pos += 4;
        return true;
    }
    return false;
}

bool SimpleJsonParser::parseArray(const std::string& str, size_t& pos, JsonValue& outValue) {
    if (pos >= str.length() || str[pos] != '[') {
        return false;
    }
    
    pos++; // Skip '['
    skipWhitespace(str, pos);
    
    std::vector<JsonValue> array;
    
    if (pos < str.length() && str[pos] == ']') {
        pos++; // Skip ']'
        outValue = array;
        return true;
    }
    
    while (pos < str.length()) {
        skipWhitespace(str, pos);
        
        JsonValue element;
        if (!parseJsonInternal(str, pos, element)) {
            return false;
        }
        
        array.push_back(std::move(element));
        
        skipWhitespace(str, pos);
        
        if (pos >= str.length()) {
            return false;
        }
        
        if (str[pos] == ']') {
            pos++; // Skip ']'
            outValue = array;
            return true;
        } else if (str[pos] == ',') {
            pos++; // Skip ','
            continue;
        } else {
            return false;
        }
    }
    
    return false;
}

bool SimpleJsonParser::parseObject(const std::string& str, size_t& pos, JsonValue& outValue) {
    if (pos >= str.length() || str[pos] != '{') {
        return false;
    }
    
    pos++; // Skip '{'
    skipWhitespace(str, pos);
    
    std::map<std::string, JsonValue> object;
    
    if (pos < str.length() && str[pos] == '}') {
        pos++; // Skip '}'
        outValue = object;
        return true;
    }
    
    while (pos < str.length()) {
        skipWhitespace(str, pos);
        
        // Parse key
        std::string key;
        if (!parseString(str, pos, key)) {
            return false;
        }
        
        skipWhitespace(str, pos);
        
        if (pos >= str.length() || str[pos] != ':') {
            return false;
        }
        
        pos++; // Skip ':'
        skipWhitespace(str, pos);
        
        // Parse value
        JsonValue value;
        if (!parseJsonInternal(str, pos, value)) {
            return false;
        }
        
        object[key] = std::move(value);
        
        skipWhitespace(str, pos);
        
        if (pos >= str.length()) {
            return false;
        }
        
        if (str[pos] == '}') {
            pos++; // Skip '}'
            outValue = object;
            return true;
        } else if (str[pos] == ',') {
            pos++; // Skip ','
            continue;
        } else {
            return false;
        }
    }
    
    return false;
}

} // namespace nlm