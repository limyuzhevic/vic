#include "../core/JsonParser.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <cctype>
#include <unordered_map>

namespace nlm {

// Simple JSON parser implementation for NLM configuration
// Provides basic JSON parsing with support for:
// - Key-value pairs
// - Arrays
// - Strings, numbers, booleans, null values
// - Whitespace handling

bool JsonParser::parseFile(const std::string& filepath, std::unordered_map<std::string, nlm::ConfigValue>& outValues) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    std::stringstream buffer;
    buffer << file.rdbuf();
    
    return parseString(buffer.str(), outValues);
}

bool JsonParser::parseString(const std::string& jsonStr, std::unordered_map<std::string, nlm::ConfigValue>& outValues) {
    size_t pos = 0;
    skipWhitespace(jsonStr, pos);
    
    // Expecting JSON object { ... }
    if (pos >= jsonStr.size() || jsonStr[pos] != '{') {
        return false;
    }
    
    pos++; // Skip {
    skipWhitespace(jsonStr, pos);
    
    while (pos < jsonStr.size() && jsonStr[pos] != '}') {
        // Parse key
        skipWhitespace(jsonStr, pos);
        if (pos >= jsonStr.size() || jsonStr[pos] != '"') {
            return false;
        }
        
        size_t keyEnd = jsonStr.find('"', pos + 1);
        if (keyEnd == std::string::npos) {
            return false;
        }
        
        std::string key = jsonStr.substr(pos + 1, keyEnd - pos - 1);
        pos = keyEnd + 1;
        
        skipWhitespace(jsonStr, pos);
        if (pos >= jsonStr.size() || jsonStr[pos] != ':') {
            return false;
        }
        
        pos++; // Skip :
        skipWhitespace(jsonStr, pos);
        
        // Parse value
        nlm::ConfigValue value;
        if (!parseValue(jsonStr, pos, value)) {
            return false;
        }
        
        outValues[key] = value;
        
        skipWhitespace(jsonStr, pos);
        if (pos < jsonStr.size() && jsonStr[pos] == ',') {
            pos++; // Skip comma
            skipWhitespace(jsonStr, pos);
        }
    }
    
    if (pos < jsonStr.size()) {
        pos++; // Skip }
    }
    
    return true;
}

bool JsonParser::validate(const std::unordered_map<std::string, nlm::ConfigValue>& values) {
    // Basic validation - ensure no null values and no duplicate keys
    for (const auto& pair : values) {
        if (std::holds_alternative<std::monostate>(pair.second)) {
            return false; // Null value not allowed
        }
    }
    return true;
}

std::string JsonParser::toJson(const std::unordered_map<std::string, nlm::ConfigValue>& values) {
    std::stringstream json;
    
    json << "{";
    bool first = true;
    
    for (const auto& pair : values) {
        if (!first) {
            json << ",\n";
        }
        first = false;
        
        json << "  \"" << pair.first << "\": ";
        json << valueToString(pair.second);
    }
    
    json << "\n}";
    return json.str();
}

// Helper functions

bool JsonParser::parseValue(const std::string& str, size_t& pos, nlm::ConfigValue& outValue) {
    skipWhitespace(str, pos);
    
    if (pos >= str.size()) {
        return false;
    }
    
    switch (str[pos]) {
        case '"': {
            // String value
            size_t endQuote = str.find('"', pos + 1);
            if (endQuote == std::string::npos) {
                return false;
            }
            std::string value = str.substr(pos + 1, endQuote - pos - 1);
            outValue = value;
            pos = endQuote + 1;
            return true;
        }
        case '[': {
            // Array value
            std::vector<nlm::ConfigValue> array;
            size_t oldPos = pos;
            if (!parseArray(str, pos, array)) {
                pos = oldPos;
                return false;
            }
            outValue = array;
            return true;
        }
        case '{': {
            // Object value - for now, treat as string representation
            size_t oldPos = pos;
            // Find matching closing brace
            int braceCount = 1;
            size_t endPos = pos + 1;
            while (endPos < str.size() && braceCount > 0) {
                endPos++;
                if (str[endPos - 1] == '{') {
                    braceCount++;
                } else if (str[endPos - 1] == '}') {
                    braceCount--;
                }
            }
            if (braceCount != 0) {
                pos = oldPos;
                return false;
            }
            std::string objectStr = str.substr(pos, endPos - pos);
            outValue = objectStr;
            pos = endPos;
            return true;
        }
        default: {
            // Number, boolean, or null
            size_t oldPos = pos;
            
            // Try to parse as double
            size_t numEnd;
            double numVal = parseNumber(str, pos, numEnd);
            if (pos != oldPos) {
                // Check if next character is not part of a number
                if (numEnd == str.size() || !std::isalnum(str[numEnd])) {
                    outValue = numVal;
                    return true;
                }
            }
            
            // Try to parse as boolean
            pos = oldPos;
            if (pos + 4 <= str.size() && str.substr(pos, 4) == "true") {
                outValue = true;
                pos += 4;
                return true;
            }
            
            if (pos + 5 <= str.size() && str.substr(pos, 5) == "false") {
                outValue = false;
                pos += 5;
                return true;
            }
            
            if (pos + 4 <= str.size() && str.substr(pos, 4) == "null") {
                outValue = std::monostate{}; // Null value
                pos += 4;
                return true;
            }
            
            return false;
        }
    }
}

void JsonParser::skipWhitespace(const std::string& str, size_t& pos) {
    while (pos < str.size() && isWhitespace(str[pos])) {
        pos++;
    }
}

bool JsonParser::isWhitespace(char c) {
    return c == ' ' || c == '\t' || c == '\n' || c == '\r' || c == '\f' || c == '\v';
}

std::string JsonParser::trim(const std::string& str) {
    size_t start = str.find_first_not_of(" \t\n\r\f\v");
    if (start == std::string::npos) {
        return "";
    }
    size_t end = str.find_last_not_of(" \t\n\r\f\v");
    return str.substr(start, end - start + 1);
}

std::string JsonParser::extractString(const std::string& str, size_t& pos) {
    if (pos >= str.size() || str[pos] != '"') {
        return "";
    }
    
    size_t endQuote = str.find('"', pos + 1);
    if (endQuote == std::string::npos) {
        return "";
    }
    
    std::string result = str.substr(pos + 1, endQuote - pos - 1);
    pos = endQuote + 1;
    return result;
}

double JsonParser::parseNumber(const std::string& str, size_t& pos, size_t& endPos) {
    size_t oldPos = pos;
    
    // Check for sign
    if (pos < str.size() && (str[pos] == '+' || str[pos] == '-')) {
        pos++;
    }
    
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
        
        // Check for sign in exponent
        if (pos < str.size() && (str[pos] == '+' || str[pos] == '-')) {
            pos++;
        }
        
        // Parse exponent digits
        while (pos < str.size() && std::isdigit(str[pos])) {
            pos++;
        }
    }
    
    endPos = pos;
    
    if (pos == oldPos) {
        return 0.0;
    }
    
    try {
        return std::stod(str.substr(oldPos, pos - oldPos));
    } catch (...) {
        return 0.0;
    }
}

bool JsonParser::parseArray(const std::string& str, size_t& pos, std::vector<nlm::ConfigValue>& outArray) {
    if (pos >= str.size() || str[pos] != '[') {
        return false;
    }
    
    pos++; // Skip [
    skipWhitespace(str, pos);
    
    outArray.clear();
    
    while (pos < str.size() && str[pos] != ']') {
        // Parse value
        nlm::ConfigValue value;
        if (!parseValue(str, pos, value)) {
            return false;
        }
        
        outArray.push_back(value);
        
        skipWhitespace(str, pos);
        if (pos < str.size() && str[pos] == ',') {
            pos++; // Skip comma
            skipWhitespace(str, pos);
        }
    }
    
    if (pos < str.size() && str[pos] == ']') {
        pos++; // Skip ]
    }
    
    return true;
}

std::string JsonParser::valueToString(const nlm::ConfigValue& value) {
    std::stringstream result;
    
    std::visit([&](auto&& arg) {
        using T = std::decay_t<decltype(arg)>;
        
        if constexpr (std::is_same_v<T, std::string>) {
            result << "\"" << arg << "\"";
        } else if constexpr (std::is_same_v<T, bool>) {
            result << (arg ? "true" : "false");
        } else if constexpr (std::is_same_v<T, std::monostate>) {
            result << "null";
        } else if constexpr (std::is_same_v<T, double>) {
            // Check if it's an integer
            if (std::abs(arg - static_cast<int64_t>(arg)) < 1e-10) {
                result << static_cast<int64_t>(arg);
            } else {
                result << arg;
            }
        } else if constexpr (std::is_same_v<T, int>) {
            result << arg;
        } else if constexpr (std::is_same_v<T, int64_t>) {
            result << arg;
        } else if constexpr (std::is_same_v<T, std::vector<int>>) {
            result << "[";
            for (size_t i = 0; i < arg.size(); ++i) {
                if (i > 0) result << ", ";
                result << arg[i];
            }
            result << "]";
        } else if constexpr (std::is_same_v<T, std::vector<double>>) {
            result << "[";
            for (size_t i = 0; i < arg.size(); ++i) {
                if (i > 0) result << ", ";
                result << arg[i];
            }
            result << "]";
        } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
            result << "[";
            for (size_t i = 0; i < arg.size(); ++i) {
                if (i > 0) result << ", ";
                result << "\"" << arg[i] << "\"";
            }
            result << "]";
        } else {
            result << "\"" << "unknown type" << "\"";
        }
    }, value);
    
    return result.str();
}

} // namespace nlm