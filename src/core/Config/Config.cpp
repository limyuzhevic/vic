// Helper functions for JSON parsing
static std::string trim(const std::string& str) {
    size_t start = str.find_first_not_of(" \t\r\n");
    if (start == std::string::npos) return "";
    size_t end = str.find_last_not_of(" \t\r\n");
    return str.substr(start, end - start + 1);
}

static bool isDigit(char c) {
    return c >= '0' && c <= '9';
}

static bool isWhitespace(char c) {
    return c == ' ' || c == '\t' || c == '\n' || c == '\r';
}

static std::string parseString(const std::string& str, size_t& pos) {
    std::string result;
    pos++; // Skip opening quote
    
    while (pos < str.size() && str[pos] != '"') {
        if (str[pos] == '\\' && pos + 1 < str.size()) {
            pos++; // Skip backslash
            if (pos < str.size()) {
                result += str[pos];
                pos++;
            }
        } else {
            result += str[pos];
            pos++;
        }
    }
    
    if (pos < str.size() && str[pos] == '"') {
        pos++; // Skip closing quote
    }
    
    return result;
}

static std::string parseNumber(const std::string& str, size_t& pos) {
    size_t start = pos;
    if (str[pos] == '-') {
        pos++;
    }
    
    // Parse integer part
    while (pos < str.size() && isDigit(str[pos])) {
        pos++;
    }
    
    // Parse decimal part if present
    if (pos < str.size() && str[pos] == '.') {
        pos++;
        while (pos < str.size() && isDigit(str[pos])) {
            pos++;
        }
    }
    
    // Parse exponent if present
    if (pos < str.size() && (str[pos] == 'e' || str[pos] == 'E')) {
        pos++;
        if (pos < str.size() && (str[pos] == '+' || str[pos] == '-')) {
            pos++;
        }
        while (pos < str.size() && isDigit(str[pos])) {
            pos++;
        }
    }
    
    return str.substr(start, pos - start);
}

static std::string parseArray(const std::string& str, size_t& pos) {
    std::string result = "[";
    pos++; // Skip opening bracket
    
    while (pos < str.size() && str[pos] != ']') {
        size_t start = pos;
        parseValue(str, pos);
        result += str.substr(start, pos - start);
        
        while (pos < str.size() && isWhitespace(str[pos])) {
            pos++;
        }
        if (pos < str.size() && str[pos] == ',') {
            result += ", ";
            pos++;
        }
    }
    
    if (pos < str.size() && str[pos] == ']') {
        result += "]";
        pos++;
    }
    
    return result;
}

static std::string parseValue(const std::string& str, size_t& pos) {
    // Skip whitespace
    while (pos < str.size() && isWhitespace(str[pos])) {
        pos++;
    }
    
    if (pos >= str.size()) return "";
    
    char ch = str[pos];
    
    if (ch == '"') {
        return parseString(str, pos);
    } else if (ch == '[') {
        return parseArray(str, pos);
    } else if (ch == '{') {
        // For simplicity, just return empty for objects
        return "{}";
    } else if (ch == '-' || isDigit(ch)) {
        return parseNumber(str, pos);
    } else if (ch == 't' || ch == 'f') {
        if (str.substr(pos, 4) == "true") {
            pos += 4;
            return "true";
        } else if (str.substr(pos, 5) == "false") {
            pos += 5;
            return "false";
        }
    } else if (ch == 'n') {
        if (str.substr(pos, 4) == "null") {
            pos += 4;
            return "null";
        }
    }
    
    return "";
}

void Config::parseJsonValue(const std::string& json, size_t& pos) {
    // Skip whitespace
    while (pos < json.size() && isWhitespace(json[pos])) {
        pos++;
    }
    
    if (pos >= json.size()) return;
    
    char ch = json[pos];
    
    if (ch == '{') {
        // Object - for now, we don't handle nested objects
        pos++;
        while (pos < json.size() && json[pos] != '}') {
            // Skip key
            while (pos < json.size() && json[pos] != '"') {
                pos++;
            }
            if (pos >= json.size()) break;
            
            pos++; // Skip opening quote
            size_t keyStart = pos;
            while (pos < json.size() && json[pos] != '"') {
                pos++;
            }
            if (pos >= json.size()) break;
            
            std::string key = json.substr(keyStart, pos - keyStart);
            pos++; // Skip closing quote
            
            // Skip whitespace
            while (pos < json.size() && isWhitespace(json[pos])) {
                pos++;
            }
            
            if (pos < json.size() && json[pos] == ':') {
                pos++; // Skip colon
                parseJsonValue(json, pos); // Parse value
            }
            
            // Skip whitespace and comma
            while (pos < json.size() && isWhitespace(json[pos])) {
                pos++;
            }
            if (pos < json.size() && json[pos] == ',') {
                pos++;
            }
        }
        if (pos < json.size() && json[pos] == '}') {
            pos++;
        }
    } else if (ch == '[') {
        // Array - parse elements
        pos++;
        while (pos < json.size() && json[pos] != ']') {
            parseJsonValue(json, pos);
            
            while (pos < json.size() && isWhitespace(json[pos])) {
                pos++;
            }
            if (pos < json.size() && json[pos] == ',') {
                pos++;
            }
        }
        if (pos < json.size() && json[pos] == ']') {
            pos++;
        }
    } else if (ch == '"') {
        // String
        pos++;
        size_t start = pos;
        while (pos < json.size() && json[pos] != '"') {
            if (json[pos] == '\\' && pos + 1 < json.size()) {
                pos += 2; // Skip escaped character
            } else {
                pos++;
            }
        }
        if (pos < json.size() && json[pos] == '"') {
            pos++; // Skip closing quote
        }
    } else if (ch == '-' || isDigit(ch)) {
        // Number (positive or negative)
        parseNumber(json, pos);
    } else if (ch == 't' || ch == 'f') {
        // Boolean (true/false)
        if (json.substr(pos, 4) == "true") {
            pos += 4;
        } else if (json.substr(pos, 5) == "false") {
            pos += 5;
        }
    } else if (ch == 'n') {
        // Null
        if (json.substr(pos, 4) == "null") {
            pos += 4;
        }
    }
    
    // Skip trailing whitespace
    while (pos < json.size() && isWhitespace(json[pos])) {
        pos++;
    }
}

// Explicit template instantiations
// Add missing template instantiations for the config methods

namespace nlm {
    // Helper function implementations
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
}

// Function to check if string is JSON
static bool isJsonFile(const std::string& content) {
    size_t firstNonWhitespace = content.find_first_not_of(" \t\n\r");
    if (firstNonWhitespace != std::string::npos) {
        char firstChar = content[firstNonWhitespace];
        return (firstChar == '{' || firstChar == '[');
    }
    return false;
}