#include "JSONParser.hpp"
#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <cctype>
#include <algorithm>
#include <map>
#include <unordered_map>

namespace nlm {

bool JSONParser::parse(const std::string& json, ConfigValue& outValue) {
    if (!validateJSON(json)) {
        return false;
    }
    
    JSONTokenizer tokenizer(json);
    
    try {
        if (!parseValue(tokenizer, outValue)) {
            return false;
        }
        
        // Should be at end of input
        auto token = tokenizer.nextToken();
        if (token.type != JSONTokenizer::T_EOF) {
            return false;
        }
        
        return true;
    } catch (const std::exception&) {
        return false;
    }
}

bool JSONParser::parseFile(const std::string& filepath, ConfigValue& outValue) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    std::stringstream buffer;
    buffer << file.rdbuf();
    
    return parse(buffer.str(), outValue);
}

std::string JSONParser::toJSON(const ConfigValue& value) {
    std::stringstream result;
    
    std::visit([&](const auto& val) {
        using T = std::decay_t<decltype(val)>;
        
        if constexpr (std::is_same_v<T, int>) {
            result << val;
        } else if constexpr (std::is_same_v<T, int64_t>) {
            result << val;
        } else if constexpr (std::is_same_v<T, double>) {
            result << val;
        } else if constexpr (std::is_same_v<T, bool>) {
            result << std::boolalpha << val;
        } else if constexpr (std::is_same_v<T, std::string>) {
            result << "\"" << escapeString(val) << "\"";
        } else if constexpr (std::is_same_v<T, std::vector<int>>) {
            result << "[";
            for (size_t i = 0; i < val.size(); ++i) {
                if (i > 0) result << ",";
                result << val[i];
            }
            result << "]";
        } else if constexpr (std::is_same_v<T, std::vector<double>>) {
            result << "[";
            for (size_t i = 0; i < val.size(); ++i) {
                if (i > 0) result << ",";
                result << val[i];
            }
            result << "]";
        } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
            result << "[";
            for (size_t i = 0; i < val.size(); ++i) {
                if (i > 0) result << ",";
                result << "\"" << escapeString(val[i]) << "\"";
            }
            result << "]";
        }
    }, value);
    
    return result.str();
}

std::string JSONParser::toJSONPretty(const ConfigValue& value) {
    std::stringstream result;
    
    std::visit([&](const auto& val, int indent) {
        using T = std::decay_t<decltype(val)>;
        
        auto indentStr = std::string(indent * 2, ' ');
        
        if constexpr (std::is_same_v<T, int>) {
            result << val;
        } else if constexpr (std::is_same_v<T, int64_t>) {
            result << val;
        } else if constexpr (std::is_same_v<T, double>) {
            result << val;
        } else if constexpr (std::is_same_v<T, bool>) {
            result << std::boolalpha << val;
        } else if constexpr (std::is_same_v<T, std::string>) {
            result << "\"" << escapeString(val) << "\"";
        } else if constexpr (std::is_same_v<T, std::vector<int>>) {
            result << "[" << std::endl;
            for (size_t i = 0; i < val.size(); ++i) {
                result << indentStr << "  " << val[i];
                if (i < val.size() - 1) result << ",";
                result << std::endl;
            }
            result << indentStr << "]";
        } else if constexpr (std::is_same_v<T, std::vector<double>>) {
            result << "[" << std::endl;
            for (size_t i = 0; i < val.size(); ++i) {
                result << indentStr << "  " << val[i];
                if (i < val.size() - 1) result << ",";
                result << std::endl;
            }
            result << indentStr << "]";
        } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
            result << "[" << std::endl;
            for (size_t i = 0; i < val.size(); ++i) {
                result << indentStr << "  \"" << escapeString(val[i]) << "\"";
                if (i < val.size() - 1) result << ",";
                result << std::endl;
            }
            result << indentStr << "]";
        }
    }, value, 0);
    
    return result.str();
}

bool JSONParser::validateJSON(const std::string& json) {
    JSONTokenizer tokenizer(json);
    
    try {
        if (!parseValue(tokenizer, ConfigValue())) {
            return false;
        }
        
        // Should be at end of input
        auto token = tokenizer.nextToken();
        if (token.type != JSONTokenizer::T_EOF) {
            return false;
        }
        
        return true;
    } catch (const std::exception&) {
        return false;
    }
}

JSONParser::JSONTokenizer::JSONTokenizer(const std::string& json) 
    : json(json), pos(0), line(1), column(1) {}

JSONParser::JSONTokenizer::Token JSONParser::JSONTokenizer::nextToken() {
    Token token;
    token.line = line;
    token.column = column;
    
    skipWhitespace();
    
    if (pos >= json.size()) {
        token.type = T_EOF;
        return token;
    }
    
    char c = json[pos];
    
    switch (c) {
        case '{':
            token.type = T_OBJECT_BEGIN;
            advance();
            break;
        case '}':
            token.type = T_OBJECT_END;
            advance();
            break;
        case '[':
            token.type = T_ARRAY_BEGIN;
            advance();
            break;
        case ']':
            token.type = T_ARRAY_END;
            advance();
            break;
        case ',':
            token.type = T_COMMA;
            advance();
            break;
        case ':':
            token.type = T_COLON;
            advance();
            break;
        case '"':
            token.type = T_STRING;
            token.value = parseStringValue();
            break;
        case 't':
            if (json.substr(pos, 4) == "true") {
                token.type = T_BOOL_TRUE;
                advance(); advance(); advance(); advance();
                token.value = "true";
            } else {
                token.type = T_ERROR;
            }
            break;
        case 'f':
            if (json.substr(pos, 5) == "false") {
                token.type = T_BOOL_FALSE;
                advance(); advance(); advance(); advance(); advance();
                token.value = "false";
            } else {
                token.type = T_ERROR;
            }
            break;
        case 'n':
            if (json.substr(pos, 4) == "null") {
                token.type = T_NULL;
                advance(); advance(); advance(); advance();
                token.value = "null";
            } else {
                token.type = T_ERROR;
            }
            break;
        case '-':
        case '0': case '1': case '2': case '3': case '4':
        case '5': case '6': case '7': case '8': case '9':
            token.type = T_NUMBER;
            token.value = parseNumberValue();
            break;
        default:
            token.type = T_ERROR;
            break;
    }
    
    return token;
}

void JSONParser::JSONTokenizer::skipWhitespace() {
    while (pos < json.size()) {
        char c = json[pos];
        if (std::isspace(c)) {
            if (c == '\n') {
                line++;
                column = 1;
            } else {
                column++;
            }
            pos++;
        } else if (c == '/' && pos + 1 < json.size()) {
            if (json[pos + 1] == '/') {
                // Line comment
                pos += 2;
                while (pos < json.size() && json[pos] != '\n') {
                    pos++;
                    column++;
                }
            } else if (json[pos + 1] == '*') {
                // Block comment
                pos += 2;
                while (pos + 1 < json.size() && !(json[pos] == '*' && json[pos + 1] == '/')) {
                    pos++;
                    column++;
                    if (json[pos] == '\n') {
                        line++;
                        column = 1;
                    }
                }
                if (pos + 1 < json.size()) {
                    pos += 2; // Skip */
                    column += 2;
                }
            } else {
                break;
            }
        } else {
            break;
        }
    }
}

char JSONParser::JSONTokenizer::peek() const {
    if (pos < json.size()) {
        return json[pos];
    }
    return '\0';
}

char JSONParser::JSONTokenizer::advance() {
    if (pos < json.size()) {
        char c = json[pos];
        pos++;
        column++;
        return c;
    }
    return '\0';
}

void JSONParser::JSONTokenizer::skipLineComment() {
    while (pos < json.size() && json[pos] != '\n') {
        pos++;
        column++;
    }
}

void JSONParser::JSONTokenizer::skipBlockComment() {
    while (pos + 1 < json.size() && !(json[pos] == '*' && json[pos + 1] == '/')) {
        pos++;
        column++;
        if (json[pos] == '\n') {
            line++;
            column = 1;
        }
    }
    if (pos + 1 < json.size()) {
        pos += 2; // Skip */
        column += 2;
    }
}

std::string JSONParser::JSONTokenizer::parseStringValue() {
    advance(); // Skip opening quote
    
    std::string result;
    while (pos < json.size() && json[pos] != '"') {
        if (json[pos] == '\\') {
            advance(); // Skip backslash
            if (pos >= json.size()) {
                throw std::runtime_error("Unterminated escape sequence");
            }
            char escaped = json[pos];
            switch (escaped) {
                case '"': result += '"'; break;
                case '\\': result += '\\'; break;
                case '/': result += '/'; break;
                case 'b': result += '\b'; break;
                case 'f': result += '\f'; break;
                case 'n': result += '\n'; break;
                case 'r': result += '\r'; break;
                case 't': result += '\t'; break;
                case 'u': {
                    if (pos + 4 >= json.size()) {
                        throw std::runtime_error("Invalid Unicode escape sequence");
                    }
                    // Skip 4 hex digits (simplified)
                    advance(); advance(); advance(); advance();
                    break;
                }
                default:
                    result += escaped;
                    break;
            }
            advance(); // Skip escaped character
        } else {
            if (json[pos] == '\n') {
                line++;
                column = 1;
            } else {
                column++;
            }
            result += json[pos];
            advance();
        }
    }
    
    if (pos >= json.size() || json[pos] != '"') {
        throw std::runtime_error("Unterminated string literal");
    }
    
    advance(); // Skip closing quote
    
    return result;
}

std::string JSONParser::JSONTokenizer::parseNumberValue() {
    size_t start = pos;
    
    if (json[pos] == '-') {
        advance();
    }
    
    while (pos < json.size() && std::isdigit(json[pos])) {
        advance();
    }
    
    if (pos < json.size() && json[pos] == '.') {
        advance();
        while (pos < json.size() && std::isdigit(json[pos])) {
            advance();
        }
    }
    
    if (pos < json.size() && (json[pos] == 'e' || json[pos] == 'E')) {
        advance();
        if (pos < json.size() && (json[pos] == '+' || json[pos] == '-')) {
            advance();
        }
        while (pos < json.size() && std::isdigit(json[pos])) {
            advance();
        }
    }
    
    return json.substr(start, pos - start);
}

bool JSONParser::parseValue(JSONTokenizer& tokenizer, ConfigValue& outValue) {
    auto token = tokenizer.nextToken();
    
    switch (token.type) {
        case JSONTokenizer::T_OBJECT_BEGIN:
            return parseObject(tokenizer, outValue);
        case JSONTokenizer::T_ARRAY_BEGIN:
            return parseArray(tokenizer, outValue);
        case JSONTokenizer::T_STRING:
            outValue = token.value;
            return true;
        case JSONTokenizer::T_NUMBER:
            return isInteger(token.value, std::get<long long>(outValue)) ||
                   isNumber(token.value, std::get<double>(outValue));
        case JSONTokenizer::T_BOOL_TRUE:
            outValue = true;
            return true;
        case JSONTokenizer::T_BOOL_FALSE:
            outValue = false;
            return true;
        case JSONTokenizer::T_NULL:
            // Null is not supported in ConfigValue, treat as empty string
            outValue = std::string();
            return true;
        default:
            return false;
    }
}

bool JSONParser::parseObject(JSONTokenizer& tokenizer, ConfigValue& outValue) {
    auto token = tokenizer.nextToken();
    if (token.type != JSONTokenizer::T_OBJECT_END) {
        while (true) {
            // Expect string key
            if (token.type != JSONTokenizer::T_STRING) {
                return false;
            }
            
            std::string key = token.value;
            
            token = tokenizer.nextToken();
            if (token.type != JSONTokenizer::T_COLON) {
                return false;
            }
            
            // Parse value
            ConfigValue value;
            if (!parseValue(tokenizer, value)) {
                return false;
            }
            
            // Add to map
            if (auto* map = std::get_if<std::map<std::string, ConfigValue>>(&outValue)) {
                (*map)[key] = value;
            } else {
                outValue = std::map<std::string, ConfigValue>();
                if (auto* map = std::get_if<std::map<std::string, ConfigValue>>(&outValue)) {
                    (*map)[key] = value;
                } else {
                    return false;
                }
            }
            
            token = tokenizer.nextToken();
            if (token.type == JSONTokenizer::T_OBJECT_END) {
                break;
            } else if (token.type != JSONTokenizer::T_COMMA) {
                return false;
            }
            
            token = tokenizer.nextToken();
        }
    }
    
    return true;
}

bool JSONParser::parseArray(JSONTokenizer& tokenizer, ConfigValue& outValue) {
    auto token = tokenizer.nextToken();
    if (token.type != JSONTokenizer::T_ARRAY_END) {
        std::vector<ConfigValue> array;
        
        while (true) {
            ConfigValue element;
            if (!parseValue(tokenizer, element)) {
                return false;
            }
            
            array.push_back(std::move(element));
            
            token = tokenizer.nextToken();
            if (token.type == JSONTokenizer::T_ARRAY_END) {
                break;
            } else if (token.type != JSONTokenizer::T_COMMA) {
                return false;
            }
            
            token = tokenizer.nextToken();
        }
        
        outValue = array;
    }
    
    return true;
}

std::string JSONParser::escapeString(const std::string& str) {
    std::string result;
    result.reserve(str.size());
    
    for (char c : str) {
        switch (c) {
            case '"': result += "\\\""; break;
            case '\\': result += "\\\\"; break;
            case '\b': result += "\\b"; break;
            case '\f': result += "\\f"; break;
            case '\n': result += "\\n"; break;
            case '\r': result += "\\r"; break;
            case '\t': result += "\\t"; break;
            default:
                if (c < 32 || c == 127) {
                    // Control characters - escape as hex
                    result += "\\u";
                    char hex[] = "0123456789ABCDEF";
                    result += hex[(c >> 4) & 0xF];
                    result += hex[c & 0xF];
                } else {
                    result += c;
                }
                break;
        }
    }
    
    return result;
}

bool JSONParser::isNumber(const std::string& str, double& outValue) {
    try {
        size_t pos;
        outValue = std::stod(str, pos);
        return pos == str.size();
    } catch (const std::exception&) {
        return false;
    }
}

bool JSONParser::isInteger(const std::string& str, long long& outValue) {
    try {
        size_t pos;
        outValue = std::stoll(str, pos);
        return pos == str.size();
    } catch (const std::exception&) {
        return false;
    }
}

} // namespace nlm