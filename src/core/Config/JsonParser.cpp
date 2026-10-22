#include "JsonParser.hpp"

namespace nlm {

std::optional<JsonValue> JsonParser::parse(const std::string& json) {
    size_t pos = 0;
    skipWhitespace(json, pos);
    auto result = parseValue(json, pos);
    skipWhitespace(json, pos);
    
    if (pos != json.size()) {
        return std::nullopt; // Extra data after JSON
    }
    
    return result;
}

std::string JsonParser::serialize(const JsonValue& json) {
    std::ostringstream oss;
    
    std::visit([&oss](auto&& arg) {
        using T = std::decay_t<decltype(arg)>;
        if constexpr (std::is_same_v<T, std::string>) {
            oss << "\"" << arg << "\"";
        } else if constexpr (std::is_same_v<T, int>) {
            oss << arg;
        } else if constexpr (std::is_same_v<T, double>) {
            oss << arg;
        } else if constexpr (std::is_same_v<T, bool>) {
            oss << (arg ? "true" : "false");
        } else if constexpr (std::is_same_v<T, std::nullptr_t>) {
            oss << "null";
        } else if constexpr (std::is_same_v<T, std::vector<JsonValue>>) {
            oss << "[";
            for (size_t i = 0; i < arg.size(); ++i) {
                if (i > 0) oss << ", ";
                oss << serialize(arg[i]);
            }
            oss << "]";
        } else if constexpr (std::is_same_v<T, std::unordered_map<std::string, JsonValue>>) {
            oss << "{";
            for (auto it = arg.begin(); it != arg.end(); ++it) {
                if (it != arg.begin()) oss << ", ";
                oss << "\"" << it->first << "\": " << serialize(it->second);
            }
            oss << "}";
        }
    }, json);
    
    return oss.str();
}

void JsonParser::skipWhitespace(const std::string& json, size_t& pos) {
    while (pos < json.size() && std::isspace(json[pos])) {
        ++pos;
    }
}

char JsonParser::getNextChar(const std::string& json, size_t& pos) {
    if (pos < json.size()) {
        return json[pos];
    }
    return '\0';
}

std::optional<JsonValue> JsonParser::parseValue(const std::string& json, size_t& pos) {
    skipWhitespace(json, pos);
    char ch = getNextChar(json, pos);
    
    switch (ch) {
        case '"': return parseString(json, pos);
        case 't': case 'f': return parseBool(json, pos);
        case 'n': return nullptr; // null
        case '[': return parseArray(json, pos);
        case '{': return parseObject(json, pos);
        default: return parseNumber(json, pos);
    }
}

std::optional<std::string> JsonParser::parseString(const std::string& json, size_t& pos) {
    if (getNextChar(json, pos) != '"') return std::nullopt;
    ++pos; // Skip opening quote
    
    std::string result;
    while (pos < json.size()) {
        char ch = json[pos];
        if (ch == '\"') {
            ++pos; // Skip closing quote
            return result;
        } else if (ch == '\\') {
            ++pos; // Skip backslash
            if (pos >= json.size()) return std::nullopt;
            
            char escaped = json[pos];
            switch (escaped) {
                case '\\': result += '\\'; break;
                case '\"': result += '\"'; break;
                case '/': result += '/'; break;
                case 'b': result += '\b'; break;
                case 'f': result += '\f'; break;
                case 'n': result += '\n'; break;
                case 'r': result += '\r'; break;
                case 't': result += '\t'; break;
                default: return std::nullopt; // Invalid escape
            }
        } else {
            result += ch;
        }
        ++pos;
    }
    
    return std::nullopt; // Unterminated string
}

std::optional<double> JsonParser::parseNumber(const std::string& json, size_t& pos) {
    size_t startPos = pos;
    bool isDecimal = false;
    bool isExponent = false;
    
    // Sign
    if (pos < json.size() && (json[pos] == '+' || json[pos] == '-')) {
        ++pos;
    }
    
    // Integer part
    while (pos < json.size() && std::isdigit(json[pos])) {
        ++pos;
    }
    
    if (pos == startPos) return std::nullopt; // No digits
    
    // Decimal part
    if (pos < json.size() && json[pos] == '.') {
        isDecimal = true;
        ++pos;
        if (!std::isdigit(json[pos])) return std::nullopt;
        while (pos < json.size() && std::isdigit(json[pos])) {
            ++pos;
        }
    }
    
    // Exponent part
    if (pos < json.size() && (json[pos] == 'e' || json[pos] == 'E')) {
        isExponent = true;
        ++pos;
        if (pos < json.size() && (json[pos] == '+' || json[pos] == '-')) {
            ++pos;
        }
        if (pos >= json.size() || !std::isdigit(json[pos])) return std::nullopt;
        while (pos < json.size() && std::isdigit(json[pos])) {
            ++pos;
        }
    }
    
    std::string numberStr = json.substr(startPos, pos - startPos);
    try {
        return std::stod(numberStr);
    } catch (...) {
        return std::nullopt;
    }
}

std::optional<bool> JsonParser::parseBool(const std::string& json, size_t& pos) {
    size_t startPos = pos;
    
    if (pos + 4 <= json.size() && json.substr(pos, 4) == "true") {
        pos += 4;
        return true;
    } else if (pos + 5 <= json.size() && json.substr(pos, 5) == "false") {
        pos += 5;
        return false;
    }
    
    return std::nullopt;
}

std::optional<JsonValue> JsonParser::parseArray(const std::string& json, size_t& pos) {
    if (getNextChar(json, pos) != '[') return std::nullopt;
    ++pos; // Skip opening bracket
    
    std::vector<JsonValue> array;
    
    skipWhitespace(json, pos);
    
    if (getNextChar(json, pos) == ']') {
        ++pos; // Skip closing bracket
        return array;
    }
    
    while (true) {
        auto value = parseValue(json, pos);
        if (!value) return std::nullopt;
        
        array.push_back(*value);
        
        skipWhitespace(json, pos);
        char ch = getNextChar(json, pos);
        
        if (ch == ']') {
            ++pos; // Skip closing bracket
            return array;
        } else if (ch == ',') {
            ++pos; // Skip comma
            skipWhitespace(json, pos);
        } else {
            return std::nullopt; // Expected comma or closing bracket
        }
    }
}

std::optional<JsonValue> JsonParser::parseObject(const std::string& json, size_t& pos) {
    if (getNextChar(json, pos) != '{') return std::nullopt;
    ++pos; // Skip opening brace
    
    std::unordered_map<std::string, JsonValue> object;
    
    skipWhitespace(json, pos);
    
    if (getNextChar(json, pos) == '}') {
        ++pos; // Skip closing brace
        return object;
    }
    
    while (true) {
        // Parse key
        auto key = parseString(json, pos);
        if (!key) return std::nullopt;
        
        skipWhitespace(json, pos);
        
        if (getNextChar(json, pos) != ':') return std::nullopt;
        ++pos; // Skip colon
        
        skipWhitespace(json, pos);
        
        // Parse value
        auto value = parseValue(json, pos);
        if (!value) return std::nullopt;
        
        object[*key] = *value;
        
        skipWhitespace(json, pos);
        char ch = getNextChar(json, pos);
        
        if (ch == '}') {
            ++pos; // Skip closing brace
            return object;
        } else if (ch == ',') {
            ++pos; // Skip comma
            skipWhitespace(json, pos);
        } else {
            return std::nullopt; // Expected comma or closing brace
        }
    }
}

} // namespace nlm