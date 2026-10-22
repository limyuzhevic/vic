// src/core/json/JSONParser.cpp
#include "JSONValue.hpp"
#include <sstream>
#include <algorithm>
#include <cctype>

namespace nlm {
namespace json {

// Helper functions
static void trim(std::string& str) {
    str.erase(str.begin(), std::find_if(str.begin(), str.end(), [](unsigned char ch) {
        return !std::isspace(ch);
    }));
    str.erase(std::find_if(str.rbegin(), str.rend(), [](unsigned char ch) {
        return !std::isspace(ch);
    }).base(), str.end());
}

static bool isWhitespace(char c) {
    return c == ' ' || c == '\t' || c == '\n' || c == '\r';
}

static std::string trim(const std::string& str) {
    size_t start = 0;
    size_t end = str.size();
    
    while (start < end && isWhitespace(str[start])) start++;
    while (end > start && isWhitespace(str[end - 1])) end--;
    
    return str.substr(start, end - start);
}

JSONValue parseJSON(const std::string& input) {
    std::string trimmed = trim(input);
    if (trimmed.empty()) {
        return nullptr;
    }
    
    // Check for simple types
    if (trimmed[0] == 'n' && trimmed.substr(0, 4) == "null") {
        return nullptr;
    }
    if (trimmed[0] == 't' && trimmed.substr(0, 4) == "true") {
        return true;
    }
    if (trimmed[0] == 'f' && trimmed.substr(0, 5) == "false") {
        return false;
    }
    if (trimmed[0] == '"') {
        // Parse string
        if (trimmed.back() != '"') {
            throw JSONParseError("Unterminated string literal");
        }
        std::string str = trimmed.substr(1, trimmed.size() - 2);
        // TODO: Handle escape sequences
        return str;
    }
    
    // Check for array
    if (trimmed[0] == '[') {
        std::vector<JSONValue> array;
        size_t pos = 1;
        while (pos < trimmed.size()) {
            size_t endPos = findJSONElementEnd(trimmed, pos);
            std::string elementStr = trimmed.substr(pos, endPos - pos);
            array.push_back(parseJSON(elementStr));
            
            pos = endPos + 1;
            while (pos < trimmed.size() && isWhitespace(trimmed[pos])) pos++;
            
            if (pos < trimmed.size() && trimmed[pos] == ',') {
                pos++;
                while (pos < trimmed.size() && isWhitespace(trimmed[pos])) pos++;
            }
        }
        
        return array;
    }
    
    // Check for object
    if (trimmed[0] == '{') {
        std::map<std::string, JSONValue> object;
        size_t pos = 1;
        while (pos < trimmed.size()) {
            // Skip whitespace
            while (pos < trimmed.size() && isWhitespace(trimmed[pos])) pos++;
            
            if (pos >= trimmed.size() || trimmed[pos] == '}') break;
            
            // Find key
            size_t keyEnd = findJSONElementEnd(trimmed, pos);
            std::string keyStr = trimmed.substr(pos, keyEnd - pos);
            std::string key = parseJSON(keyStr).asString();
            
            pos = keyEnd + 1;
            while (pos < trimmed.size() && isWhitespace(trimmed[pos])) pos++;
            
            if (pos >= trimmed.size() || trimmed[pos] != ':') {
                throw JSONParseError("Missing colon after key in object");
            }
            
            pos++;
            while (pos < trimmed.size() && isWhitespace(trimmed[pos])) pos++;
            
            // Find value
            size_t valueEnd = findJSONElementEnd(trimmed, pos);
            std::string valueStr = trimmed.substr(pos, valueEnd - pos);
            object[key] = parseJSON(valueStr);
            
            pos = valueEnd + 1;
            while (pos < trimmed.size() && isWhitespace(trimmed[pos])) pos++;
            
            if (pos < trimmed.size() && trimmed[pos] == ',') {
                pos++;
            } else if (pos < trimmed.size() && trimmed[pos] != '}') {
                throw JSONParseError("Unexpected character in object parsing");
            }
        }
        
        return object;
    }
    
    // Try to parse as number
    try {
        size_t pos;
        double value = std::stod(trimmed, &pos);
        if (pos == trimmed.size()) {
            return value;
        }
    } catch (const std::exception&) {}
    
    throw JSONParseError("Unable to parse JSON: " + input);
}

static size_t findJSONElementEnd(const std::string& str, size_t start) {
    if (start >= str.size()) return start;
    
    char current = str[start];
    if (current == '"') {
        // String literal
        size_t end = start + 1;
        while (end < str.size()) {
            if (str[end] == '\\') {
                end += 2; // Skip escape sequence
                continue;
            }
            if (str[end] == '"') {
                return end + 1;
            }
            end++;
        }
        return str.size();
    } else if (current == '[') {
        // Array
        int depth = 1;
        size_t end = start + 1;
        while (end < str.size() && depth > 0) {
            if (str[end] == '[') depth++;
            else if (str[end] == ']') depth--;
            else if (str[end] == '\\') end += 2; // Skip escape sequence
            end++;
        }
        return end;
    } else if (current == '{') {
        // Object
        int depth = 1;
        size_t end = start + 1;
        while (end < str.size() && depth > 0) {
            if (str[end] == '{') depth++;
            else if (str[end] == '}') depth--;
            else if (str[end] == '\\') end += 2; // Skip escape sequence
            end++;
        }
        return end;
    } else {
        // Number or literal
        size_t end = start;
        while (end < str.size() && !isWhitespace(str[end]) && str[end] != ',' && str[end] != ']' && str[end] != '}') {
            end++;
        }
        return end;
    }
}

std::string toJSON(const JSONValue& value, int indent) {
    std::string result;
    std::string indentStr(indent * 2, ' ');
    
    if (isNull(value)) {
        return "null";
    } else if (isBoolean(value)) {
        return asBoolean(value) ? "true" : "false";
    } else if (isNumber(value)) {
        return std::to_string(asNumber(value));
    } else if (isString(value)) {
        std::string str = asString(value);
        // Escape special characters
        size_t pos = 0;
        while ((pos = str.find('\"', pos)) != std::string::npos) {
            str.replace(pos, 1, "\\\"");
            pos += 2;
        }
        pos = 0;
        while ((pos = str.find('\\', pos)) != std::string::npos) {
            str.replace(pos, 1, "\\\\");
            pos += 2;
        }
        pos = 0;
        while ((pos = str.find('\n', pos)) != std::string::npos) {
            str.replace(pos, 1, "\\n");
            pos += 2;
        }
        pos = 0;
        while ((pos = str.find('\t', pos)) != std::string::npos) {
            str.replace(pos, 1, "\\t");
            pos += 2;
        }
        return "\"" + str + "\"";
    } else if (isArray(value)) {
        const auto& array = asArray(value);
        if (array.empty()) {
            return "[]";
        }
        
        result = "[\n";
        for (size_t i = 0; i < array.size(); ++i) {
            result += indentStr + "  " + toJSON(array[i], indent + 1);
            if (i < array.size() - 1) {
                result += ",\n";
            }
        }
        result += "\n" + indentStr + "]";
        return result;
    } else if (isObject(value)) {
        const auto& object = asObject(value);
        if (object.empty()) {
            return "{}";
        }
        
        result = "{\n";
        size_t i = 0;
        for (const auto& pair : object) {
            result += indentStr + "  \"" + pair.first + "\": " + toJSON(pair.second, indent + 1);
            if (i < object.size() - 1) {
                result += ",\n";
            }
            i++;
        }
        result += "\n" + indentStr + "}";
        return result;
    }
    
    throw JSONParseError("Unknown JSON value type");
}

} // namespace json
} // namespace nlm
