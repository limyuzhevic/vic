#include "JsonConfig/JsonConfigParser.hpp"
#include <iostream>
#include <stdexcept>
#include <cctype>

namespace nlm {

bool JsonConfigParser::parseFromFile(const std::string& filepath, std::vector<ConfigEntry>& entries) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    std::stringstream buffer;
    buffer << file.rdbuf();
    std::string jsonString = buffer.str();
    
    return parseFromString(jsonString, entries);
}

bool JsonConfigParser::parseFromString(const std::string& jsonString, std::vector<ConfigEntry>& entries) {
    entries.clear();
    
    if (!validateJson(jsonString)) {
        return false;
    }
    
    size_t pos = 0;
    while (pos < jsonString.size()) {
        // Skip whitespace and comments
        while (pos < jsonString.size() && (std::isspace(jsonString[pos]) || jsonString[pos] == '/')) {
            if (jsonString[pos] == '/' && pos + 1 < jsonString.size() && jsonString[pos + 1] == '/') {
                // Skip line comment
                while (pos < jsonString.size() && jsonString[pos] != '\n') {
                    pos++;
                }
            } else {
                pos++;
            }
        }
        
        if (pos >= jsonString.size()) break;
        
        // Parse key
        size_t keyStart = pos;
        while (pos < jsonString.size() && jsonString[pos] != ':' && !std::isspace(jsonString[pos])) {
            pos++;
        }
        
        if (pos >= jsonString.size() || jsonString[pos] != ':') {
            break; // Invalid JSON
        }
        
        std::string key = trim(jsonString.substr(keyStart, pos - keyStart));
        pos++; // Skip colon
        
        // Skip whitespace after colon
        while (pos < jsonString.size() && std::isspace(jsonString[pos])) {
            pos++;
        }
        
        if (pos >= jsonString.size()) break;
        
        // Parse value
        std::string valueStr;
        ConfigSource source = ConfigSource::File;
        
        if (jsonString[pos] == '"') {
            // String value
            valueStr = parseJsonString(jsonString, pos);
            source = ConfigSource::File;
        } else if (std::isdigit(jsonString[pos]) || jsonString[pos] == '-') {
            // Number value
            valueStr = parseJsonNumber(jsonString, pos);
            source = ConfigSource::File;
        } else if (jsonString[pos] == 't' || jsonString[pos] == 'f') {
            // Boolean value
            bool boolValue = parseJsonBoolean(jsonString, pos);
            valueStr = boolValue ? "true" : "false";
            source = ConfigSource::File;
        } else if (jsonString[pos] == '{') {
            // Object - skip for now
            pos = jsonString.find('}', pos);
            if (pos == std::string::npos) {
                return false;
            }
            pos++;
            continue;
        } else if (jsonString[pos] == '[') {
            // Array - skip for now
            pos = jsonString.find(']', pos);
            if (pos == std::string::npos) {
                return false;
            }
            pos++;
            continue;
        } else {
            // Unknown type, skip
            while (pos < jsonString.size() && jsonString[pos] != ',' && jsonString[pos] != '}') {
                pos++;
            }
            continue;
        }
        
        // Add entry
        entries.emplace_back(key, ConfigValue(valueStr), source);
        
        // Skip comma if present
        while (pos < jsonString.size() && std::isspace(jsonString[pos])) {
            pos++;
        }
        if (pos < jsonString.size() && jsonString[pos] == ',') {
            pos++;
        }
    }
    
    return true;
}

std::string JsonConfigParser::toJsonString(const ConfigEntry& entry) {
    std::ostringstream oss;
    
    oss << "{\"key\": \"" << entry.key << "\", \"value\": ";
    
    std::visit([&oss](auto&& arg) {
        using T = std::decay_t<decltype(arg)>;
        if constexpr (std::is_same_v<T, int>) {
            oss << arg;
        } else if constexpr (std::is_same_v<T, int64_t>) {
            oss << arg;
        } else if constexpr (std::is_same_v<T, double>) {
            oss << std::fixed << std::setprecision(6) << arg;
        } else if constexpr (std::is_same_v<T, bool>) {
            oss << (arg ? "true" : "false");
        } else if constexpr (std::is_same_v<T, std::string>) {
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
                oss << std::fixed << std::setprecision(6) << arg[i];
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
            oss << "null";
        }
    }, entry.value);
    
    oss << "}";
    
    return oss.str();
}

bool JsonConfigParser::validateJson(const std::string& jsonString) {
    if (jsonString.empty()) return false;
    
    if (jsonString[0] != '{') {
        return false;
    }
    
    size_t pos = 1;
    size_t braceCount = 1;
    bool inString = false;
    char quoteChar = 0;
    
    while (pos < jsonString.size() && braceCount > 0) {
        char c = jsonString[pos];
        
        if (!inString) {
            if (c == '{') braceCount++;
            else if (c == '}') braceCount--;
            else if (c == '"') {
                inString = true;
                quoteChar = c;
            } else if (c == '\'') {
                inString = true;
                quoteChar = c;
            }
        } else {
            if (c == quoteChar) {
                inString = false;
                quoteChar = 0;
            } else if (c == '\\' && pos + 1 < jsonString.size()) {
                pos++; // Skip escaped character
            }
        }
        
        pos++;
    }
    
    return braceCount == 0;
}

std::string JsonConfigParser::trim(const std::string& str) {
    size_t start = str.find_first_not_of(" \t\r\n");
    if (start == std::string::npos) return "";
    size_t end = str.find_last_not_of(" \t\r\n");
    return str.substr(start, end - start + 1);
}

std::string JsonConfigParser::toLower(const std::string& str) {
    std::string result = str;
    std::transform(result.begin(), result.end(), result.begin(), ::tolower);
    return result;
}

bool JsonConfigParser::isJsonValue(const std::string& str) {
    if (str.empty()) return false;
    
    char c = str[0];
    return (c == '"') || (c == '{') || (c == '[') || std::isdigit(c) || 
           c == '-' || c == 't' || c == 'f' || c == 'n';
}

std::string JsonConfigParser::parseJsonString(const std::string& str, size_t& pos) {
    if (pos >= str.size() || (str[pos] != '"' && str[pos] != '\'')) {
        return "";
    }
    
    char quoteChar = str[pos];
    pos++; // Skip opening quote
    
    std::string result;
    while (pos < str.size()) {
        if (str[pos] == quoteChar) {
            pos++; // Skip closing quote
            return result;
        } else if (str[pos] == '\\' && pos + 1 < str.size()) {
            pos++; // Skip escape character
            if (pos < str.size()) {
                result += str[pos];
            }
        } else {
            result += str[pos];
        }
        pos++;
    }
    
    return result;
}

std::string JsonConfigParser::parseJsonNumber(const std::string& str, size_t& pos) {
    size_t start = pos;
    
    if (str[pos] == '-') pos++;
    
    // Parse integer part
    while (pos < str.size() && std::isdigit(str[pos])) {
        pos++;
    }
    
    // Parse decimal part
    if (pos < str.size() && str[pos] == '.') {
        pos++; // Skip decimal point
        while (pos < str.size() && std::isdigit(str[pos])) {
            pos++;
        }
    }
    
    // Parse exponent part
    if (pos < str.size() && (str[pos] == 'e' || str[pos] == 'E')) {
        pos++; // Skip exponent indicator
        if (pos < str.size() && (str[pos] == '+' || str[pos] == '-')) {
            pos++; // Skip exponent sign
        }
        while (pos < str.size() && std::isdigit(str[pos])) {
            pos++;
        }
    }
    
    return str.substr(start, pos - start);
}

bool JsonConfigParser::parseJsonBoolean(const std::string& str, size_t& pos) {
    if (pos + 3 < str.size() && str.substr(pos, 4) == "true") {
        pos += 4;
        return true;
    } else if (pos + 4 < str.size() && str.substr(pos, 5) == "false") {
        pos += 5;
        return false;
    }
    
    return false;
}

} // namespace nlm
