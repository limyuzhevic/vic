// JsonParser implementation

std::string JsonParser::unescapeString(const std::string& str) {
    std::string result;
    result.reserve(str.size());
    for (size_t i = 0; i < str.size(); ++i) {
        if (str[i] == '\\' && i + 1 < str.size()) {
            switch (str[i + 1]) {
                case '"': result.push_back('"'); break;
                case '\\': result.push_back('\\'); break;
                case '/': result.push_back('/'); break;
                case 'b': result.push_back('\b'); break;
                case 'f': result.push_back('\f'); break;
                case 'n': result.push_back('\n'); break;
                case 'r': result.push_back('\r'); break;
                case 't': result.push_back('\t'); break;
                case 'u': {
                    if (i + 5 < str.size()) {
                        std::string hex = str.substr(i + 2, 4);
                        try {
                            unsigned long code = std::stoul(hex, nullptr, 16);
                            result.push_back(static_cast<char>(code));
                        } catch (...) {
                            result.push_back('?');
                        }
                        i += 5;
                        break;
                    }
                    result.push_back('?');
                    break;
                }
                default: result.push_back(str[i + 1]); break;
            }
            i++;
        } else {
            result.push_back(str[i]);
        }
    }
    return result;
}

std::string JsonParser::trim(const std::string& str) {
    size_t start = 0;
    size_t end = str.size();
    
    while (start < end && std::isspace(str[start])) start++;
    while (end > start && std::isspace(str[end - 1])) end--;
    
    return str.substr(start, end - start);
}

bool JsonParser::parse(const std::string& json, JsonValue& result) {
    size_t pos = 0;
    
    auto skipWhitespace = [&]() {
        while (pos < json.size() && std::isspace(json[pos])) {
            pos++;
        }
    };
    
    auto parseString = [&]() -> bool {
        if (pos >= json.size() || json[pos] != '"') return false;
        pos++;
        size_t start = pos;
        while (pos < json.size() && json[pos] != '"') {
            if (json[pos] == '\\' && pos + 1 < json.size()) {
                pos += 2;
                continue;
            }
            pos++;
        }
        if (pos >= json.size()) return false;
        result.stringValue = unescapeString(json.substr(start, pos - start));
        pos++;
        return true;
    };
    
    auto parseNumber = [&]() -> bool {
        size_t start = pos;
        bool hasDecimal = false;
        bool hasExp = false;
        
        if (json[pos] == '-') {
            pos++;
        }
        
        while (pos < json.size() && (std::isdigit(json[pos]) || 
                                      (!hasDecimal && json[pos] == '.') ||
                                      (!hasExp && (json[pos] == 'e' || json[pos] == 'E')))) {
            if (json[pos] == '.') {
                hasDecimal = true;
            } else if (json[pos] == 'e' || json[pos] == 'E') {
                hasExp = true;
            }
            pos++;
        }
        
        if (pos == start) return false;
        
        try {
            result.numberValue = std::stod(json.substr(start, pos - start));
        } catch (...) {
            return false;
        }
        return true;
    };
    
    auto parseBool = [&]() -> bool {
        if (pos + 4 <= json.size() && json.substr(pos, 4) == "true") {
            result.boolValue = true;
            pos += 4;
            return true;
        } else if (pos + 5 <= json.size() && json.substr(pos, 5) == "false") {
            result.boolValue = false;
            pos += 5;
            return true;
        }
        return false;
    };
    
    auto parseNull = [&]() -> bool {
        if (pos + 4 <= json.size() && json.substr(pos, 4) == "null") {
            pos += 4;
            return true;
        }
        return false;
    };
    
    auto parseArray = [&]() -> bool {
        if (pos >= json.size() || json[pos] != '[') return false;
        pos++;
        
        skipWhitespace();
        if (json[pos] == ']') {
            pos++;
            result.arrayValue.clear();
            return true;
        }
        
        while (true) {
            JsonValue element;
            if (!parse(element)) return false;
            result.arrayValue.push_back(element);
            
            skipWhitespace();
            if (pos >= json.size()) return false;
            
            if (json[pos] == ']') {
                pos++;
                break;
            } else if (json[pos] == ',') {
                pos++;
                skipWhitespace();
                if (pos >= json.size() || json[pos] == ']') return false;
            } else {
                return false;
            }
        }
        return true;
    };
    
    auto parseObject = [&]() -> bool {
        if (pos >= json.size() || json[pos] != '{') return false;
        pos++;
        
        skipWhitespace();
        if (json[pos] == '}') {
            pos++;
            result.objectValue.clear();
            return true;
        }
        
        while (true) {
            skipWhitespace();
            
            if (pos >= json.size() || json[pos] != '"') return false;
            JsonValue key;
            if (!parseString()) return false;
            
            skipWhitespace();
            if (pos >= json.size() || json[pos] != ':') return false;
            pos++;
            
            skipWhitespace();
            JsonValue value;
            if (!parse(value)) return false;
            
            result.objectValue[key.stringValue] = value;
            
            skipWhitespace();
            if (pos >= json.size()) return false;
            
            if (json[pos] == '}') {
                pos++;
                break;
            } else if (json[pos] == ',') {
                pos++;
                skipWhitespace();
                if (pos >= json.size() || json[pos] == '}') return false;
            } else {
                return false;
            }
        }
        return true;
    };
    
    skipWhitespace();
    if (pos >= json.size()) return false;
    
    if (parseString()) {
        return true;
    } else if (parseNumber()) {
        return true;
    } else if (parseBool()) {
        return true;
    } else if (parseNull()) {
        return true;
    } else if (parseArray()) {
        return true;
    } else if (parseObject()) {
        return true;
    }
    
    return false;
}