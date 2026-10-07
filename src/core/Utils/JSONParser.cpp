#include "JSONParser.hpp"
#include <fstream>
#include <iostream>

namespace nlm {

bool JSONParser::parse(const std::string& input, JSONValue& output) {
    size_t pos = 0;
    skipWhitespace(input, pos);
    bool result = parseValue(input, pos, output);
    skipWhitespace(input, pos);
    return result && pos == input.size();
}

std::string JSONParser::serialize(const JSONValue& value, int indent) {
    std::stringstream ss;
    
    if (std::holds_alternative<std::monostate>(value)) {
        ss << "null";
    } else if (std::holds_alternative<bool>(value)) {
        ss << (std::get<bool>(value) ? "true" : "false");
    } else if (std::holds_alternative<double>(value)) {
        double val = std::get<double>(value);
        // Check if it's an integer
        if (val == static_cast<int64_t>(val)) {
            ss << static_cast<int64_t>(val);
        } else {
            ss << val;
        }
    } else if (std::holds_alternative<int64_t>(value)) {
        ss << std::get<int64_t>(value);
    } else if (std::holds_alternative<std::string>(value)) {
        ss << "\"" << std::get<std::string>(value) << "\"";
    } else if (std::holds_alternative<std::vector<JSONValue>>(value)) {
        const auto& array = std::get<std::vector<JSONValue>>(value);
        ss << '[';
        for (size_t i = 0; i < array.size(); ++i) {
            if (i > 0) ss << ', ';
            ss << serialize(array[i], indent + 1);
        }
        ss << ']';
    } else if (std::holds_alternative<std::map<std::string, JSONValue>>(value)) {
        const auto& obj = std::get<std::map<std::string, JSONValue>>(value);
        ss << '{';
        bool first = true;
        for (const auto& pair : obj) {
            if (!first) {
                ss << ', ';
            }
            first = false;
            ss << "\"" << pair.first << "\": " << serialize(pair.second, indent + 1);
        }
        ss << '}';
    }
    
    return ss.str();
}

std::optional<JSONValue> JSONParser::parseFile(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return std::nullopt;
    }
    
    std::stringstream buffer;
    buffer << file.rdbuf();
    
    JSONValue result;
    return parse(buffer.str(), result) ? std::make_optional(result) : std::nullopt;
}

bool JSONParser::parseValue(const std::string& input, size_t& pos, JSONValue& output) {
    skipWhitespace(input, pos);
    
    if (pos >= input.size()) {
        return false;
    }
    
    char ch = input[pos];
    
    switch (ch) {
        case '{':
            return parseObject(input, pos, std::get<std::map<std::string, JSONValue>>(output));
        case '[':
            return parseArray(input, pos, std::get<std::vector<JSONValue>>(output));
        case '"':
            return parseString(input, pos, std::get<std::string>(output));
        case 'n': // null
            if (startsWith(input.substr(pos), "null")) {
                pos += 4;
                return true;
            }
            return false;
        case 't': // true
            if (startsWith(input.substr(pos), "true")) {
                std::get<bool>(output) = true;
                pos += 4;
                return true;
            }
            return false;
        case 'f': // false
            if (startsWith(input.substr(pos), "false")) {
                std::get<bool>(output) = false;
                pos += 5;
                return true;
            }
            return false;
        default:
            // Try to parse number
            return parseNumber(input, pos, std::get<double>(output));
    }
}

bool JSONParser::parseObject(const std::string& input, size_t& pos, std::map<std::string, JSONValue>& output) {
    if (input[pos] != '{') return false;
    pos++;
    skipWhitespace(input, pos);
    
    if (input[pos] == '}') {
        pos++;
        return true;
    }
    
    while (true) {
        skipWhitespace(input, pos);
        
        // Parse key
        std::string key;
        if (!parseString(input, pos, key)) {
            return false;
        }
        
        skipWhitespace(input, pos);
        if (pos >= input.size() || input[pos] != ':') {
            return false;
        }
        pos++;
        
        // Parse value
        JSONValue value;
        if (!parseValue(input, pos, value)) {
            return false;
        }
        
        output[key] = std::move(value);
        
        skipWhitespace(input, pos);
        if (pos >= input.size()) {
            return false;
        }
        
        if (input[pos] == '}') {
            pos++;
            break;
        } else if (input[pos] == ',') {
            pos++;
            continue;
        } else {
            return false;
        }
    }
    
    return true;
}

bool JSONParser::parseArray(const std::string& input, size_t& pos, std::vector<JSONValue>& output) {
    if (input[pos] != '[') return false;
    pos++;
    skipWhitespace(input, pos);
    
    if (input[pos] == ']') {
        pos++;
        return true;
    }
    
    while (true) {
        JSONValue element;
        if (!parseValue(input, pos, element)) {
            return false;
        }
        
        output.push_back(std::move(element));
        
        skipWhitespace(input, pos);
        if (pos >= input.size()) {
            return false;
        }
        
        if (input[pos] == ']') {
            pos++;
            break;
        } else if (input[pos] == ',') {
            pos++;
            continue;
        } else {
            return false;
        }
    }
    
    return true;
}

bool JSONParser::parseString(const std::string& input, size_t& pos, std::string& output) {
    if (pos >= input.size() || input[pos] != '"') {
        return false;
    }
    
    pos++;
    size_t start = pos;
    
    while (pos < input.size() && input[pos] != '"') {
        if (input[pos] == '\\' && pos + 1 < input.size()) {
            pos += 2; // Skip escaped character
        } else {
            pos++;
        }
    }
    
    if (pos >= input.size() || input[pos] != '"') {
        return false;
    }
    
    output = input.substr(start, pos - start);
    pos++; // Skip closing quote
    return true;
}

bool JSONParser::parseNumber(const std::string& input, size_t& pos, double& output) {
    size_t start = pos;
    bool hasDecimal = false;
    bool hasExponent = false;
    
    // Check for sign
    if (input[pos] == '+' || input[pos] == '-') {
        pos++;
    }
    
    // Parse integer part
    while (pos < input.size() && std::isdigit(input[pos])) {
        pos++;
    }
    
    // Check for decimal point
    if (pos < input.size() && input[pos] == '.') {
        hasDecimal = true;
        pos++;
    }
    
    // Parse fractional part
    while (pos < input.size() && std::isdigit(input[pos])) {
        pos++;
    }
    
    // Check for exponent
    if (pos < input.size() && (input[pos] == 'e' || input[pos] == 'E')) {
        hasExponent = true;
        pos++;
        
        // Check for sign in exponent
        if (pos < input.size() && (input[pos] == '+' || input[pos] == '-')) {
            pos++;
        }
        
        // Parse exponent digits
        while (pos < input.size() && std::isdigit(input[pos])) {
            pos++;
        }
    }
    
    if (pos == start) {
        return false; // No digits were parsed
    }
    
    try {
        output = std::stod(input.substr(start, pos - start));
        return true;
    } catch (const std::exception&) {
        return false;
    }
}

bool JSONParser::parseBoolean(const std::string& input, size_t& pos, bool& output) {
    if (startsWith(input.substr(pos), "true")) {
        output = true;
        pos += 4;
        return true;
    } else if (startsWith(input.substr(pos), "false")) {
        output = false;
        pos += 5;
        return true;
    }
    return false;
}

bool JSONParser::parseNull(const std::string& input, size_t& pos) {
    if (startsWith(input.substr(pos), "null")) {
        pos += 4;
        return true;
    }
    return false;
}

void JSONParser::skipWhitespace(const std::string& input, size_t& pos) {
    while (pos < input.size() && std::isspace(input[pos])) {
        pos++;
    }
}

char JSONParser::getNextChar(const std::string& input, size_t& pos) {
    if (pos < input.size()) {
        return input[pos++];
    }
    return '\0';
}

} // namespace nlm