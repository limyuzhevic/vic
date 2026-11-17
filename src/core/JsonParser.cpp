#include "JsonParser.hpp"

namespace nlm {

ConfigValue JsonParser::parseJsonToConfigValue(const std::string& jsonStr) {
    try {
        return parseJsonValue(jsonStr);
    } catch (const std::exception& e) {
        std::cerr << "JSON parsing error: " << e.what() << std::endl;
        return std::string("[JSON Parse Error: " + std::string(e.what()) + "]");
    }
}

ConfigValue JsonParser::parseJsonFileToConfigValue(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return std::string("[File not found: " + filepath + "]");
    }
    
    std::stringstream buffer;
    buffer << file.rdbuf();
    file.close();
    
    return parseJsonToConfigValue(buffer.str());
}

std::string JsonParser::configValueToJson(const ConfigValue& value) {
    try {
        return configValueToJsonValue(value);
    } catch (const std::exception& e) {
        std::cerr << "JSON serialization error: " << e.what() << std::endl;
        return "{\"error\": \"JSON serialization failed: " + std::string(e.what()) + "\"}";
    }
}

bool JsonParser::validateJson(const std::string& jsonStr) {
    try {
        size_t pos = 0;
        parseJsonValue(jsonStr);
        return true;
    } catch (const std::exception&) {
        return false;
    }
}

std::string JsonParser::getJsonSchema() {
    return R"(
{
  "$schema": "http://json-schema.org/draft-07/schema#",
  "title": "NLM Configuration",
  "type": "object",
  "properties": {
    "random_seed": {
      "type": "integer",
      "description": "Random seed for reproducible experiments",
      "default": 42
    },
    "simulation_timestep": {
      "type": "number",
      "description": "Simulation timestep in seconds",
      "default": 0.001
    },
    "neuron_count": {
      "type": "integer",
      "description": "Number of neurons in the brain",
      "default": 1000
    },
    "region_count": {
      "type": "integer",
      "description": "Number of brain regions",
      "default": 1
    },
    "connection_probability": {
      "type": "number",
      "description": "Connection probability for random connectivity",
      "default": 0.1
    },
    "plasticity_learning_rate": {
      "type": "number",
      "description": "Plasticity learning rate",
      "default": 0.01
    },
    subdivision of neural regions and cognitive system parameters.
  },
  "required": []
}
    )";
}

ConfigValue JsonParser::parseJsonValue(const std::string& jsonStr) {
    size_t pos = 0;
    skipWhitespace(jsonStr, pos);
    
    if (jsonStr.empty() || pos >= jsonStr.size()) {
        throw std::runtime_error("Invalid JSON: empty input");
    }
    
    char firstChar = jsonStr[pos];
    
    if (firstChar == '{') {
        return parseObject(jsonStr, pos);
    } else if (firstChar == '[') {
        return parseArray(jsonStr, pos);
    } else if (firstChar == '\"') {
        return extractString(jsonStr, pos);
    } else if (firstChar == 't' || firstChar == 'f') {
        return extractBoolean(jsonStr, pos);
    } else if (firstChar == 'n') {
        return extractNull(jsonStr, pos);
    } else {
        return extractNumber(jsonStr, pos);
    }
}

std::string JsonParser::configValueToJsonValue(const ConfigValue& value) {
    return std::visit([](auto&& arg) -> std::string {
        using T = std::decay_t<decltype(arg)>;
        if constexpr (std::is_same_v<T, int>) {
            return std::to_string(arg);
        } else if constexpr (std::is_same_v<T, int64_t>) {
            return std::to_string(arg);
        } else if constexpr (std::is_same_v<T, uint64_t>) {
            return std::to_string(arg);
        } else if constexpr (std::is_same_v<T, double>) {
            return std::to_string(arg);
        } else if constexpr (std::is_same_v<T, bool>) {
            return arg ? "true" : "false";
        } else if constexpr (std::is_same_v<T, std::string>) {
            return "\"" + escapeString(arg) + "\"";
        } else if constexpr (std::is_same_v<T, std::vector<int>>) {
            std::string result = "[";
            for (size_t i = 0; i < arg.size(); ++i) {
                if (i > 0) result += ", ";
                result += std::to_string(arg[i]);
            }
            result += "]";
            return result;
        } else if constexpr (std::is_same_v<T, std::vector<double>>) {
            std::string result = "[";
            for (size_t i = 0; i < arg.size(); ++i) {
                if (i > 0) result += ", ";
                result += std::to_string(arg[i]);
            }
            result += "]";
            return result;
        } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
            std::string result = "[";
            for (size_t i = 0; i < arg.size(); ++i) {
                if (i > 0) result += ", ";
                result += "\"" + escapeString(arg[i]) + "\"";
            }
            result += "]";
            return result;
        } else {
            return "[unknown]";
        }
    }, value);
}

std::string JsonParser::escapeString(const std::string& str) {
    std::string result;
    result.reserve(str.size() * 2);
    for (char c : str) {
        switch (c) {
            case '\"': result += "\\\""; break;
            case '\\': result += "\\\\"; break;
            case '\b': result += "\\b"; break;
            case '\f': result += "\\f"; break;
            case '\n': result += "\\n"; break;
            case '\r': result += "\\r"; break;
            case '\t': result += "\\t"; break;
            default:
                if (c < 0x20 || c == 0x7f) {
                    result += "\\u" + std::to_string(static_cast<int>(c));
                } else {
                    result += c;
                }
                break;
        }
    }
    return result;
}

std::string JsonParser::unescapeString(const std::string& str) {
    std::string result;
    result.reserve(str.size());
    for (size_t i = 0; i < str.size(); ++i) {
        if (str[i] == '\\' && i + 1 < str.size()) {
            switch (str[i + 1]) {
                case '\"': result += '\"'; i += 1; break;
                case '\\': result += '\\'; i += 1; break;
                case '/': result += '/'; i += 1; break;
                case 'b': result += '\b'; i += 1; break;
                case 'f': result += '\f'; i += 1; break;
                case 'n': result += '\n'; i += 1; break;
                case 'r': result += '\r'; i += 1; break;
                case 't': result += '\t'; i += 1; break;
                case 'u':
                    // Unicode escape - skip for now
                    i += 4;
                    break;
                default:
                    result += str[i];
                    break;
            }
        } else {
            result += str[i];
        }
    }
    return result;
}

std::string JsonParser::trim(const std::string& str) {
    size_t start = str.find_first_not_of(" \t\r\n");
    if (start == std::string::npos) return "";
    size_t end = str.find_last_not_of(" \t\r\n");
    return str.substr(start, end - start + 1);
}

std::string JsonParser::toLower(const std::string& str) {
    std::string result = str;
    std::transform(result.begin(), result.end(), result.begin(), ::tolower);
    return result;
}

std::string JsonParser::extractString(const std::string& jsonStr, size_t& pos) {
    if (pos >= jsonStr.size() || jsonStr[pos] != '\"') {
        throw std::runtime_error("Expected string starting with \"\"");
    }
    
    size_t start = ++pos;
    while (pos < jsonStr.size()) {
        if (jsonStr[pos] == '\"' && (pos == 0 || jsonStr[pos - 1] != '\\')) {
            break;
        }
        pos++;
    }
    
    if (pos >= jsonStr.size()) {
        throw std::runtime_error("Unterminated string literal");
    }
    
    std::string result = jsonStr.substr(start, pos - start);
    unescapeString(result); // This should actually do the unescaping
    pos++; // Skip closing quote
    return result;
}

std::string JsonParser::extractNumber(const std::string& jsonStr, size_t& pos) {
    size_t start = pos;
    bool isFloat = false;
    bool isNegative = false;
    
    if (jsonStr[pos] == '-') {
        isNegative = true;
        pos++;
    }
    
    while (pos < jsonStr.size() && (std::isdigit(jsonStr[pos]) || jsonStr[pos] == '.' || jsonStr[pos] == 'e' || jsonStr[pos] == 'E')) {
        if (jsonStr[pos] == '.') {
            isFloat = true;
        }
        pos++;
    }
    
    if (pos == start || (pos == start + 1 && jsonStr[start] == '-')) {
        throw std::runtime_error("Invalid number format");
    }
    
    std::string numStr = jsonStr.substr(start, pos - start);
    try {
        if (isFloat) {
            double val = std::stod(numStr);
            return std::to_string(val);
        } else {
            if (isNegative) {
                int64_t val = std::stoll(numStr);
                return std::to_string(val);
            } else {
                uint64_t val = std::stoull(numStr);
                return std::to_string(val);
            }
        }
    } catch (const std::exception&) {
        throw std::runtime_error("Invalid number: " + numStr);
    }
}

std::string JsonParser::extractValue(const std::string& jsonStr, size_t& pos) {
    skipWhitespace(jsonStr, pos);
    
    if (pos >= jsonStr.size()) {
        throw std::runtime_error("Invalid JSON: end of input");
    }
    
    char firstChar = jsonStr[pos];
    
    if (firstChar == '{') {
        return parseObject(jsonStr, pos);
    } else if (firstChar == '[') {
        return parseArray(jsonStr, pos);
    } else if (firstChar == '\"') {
        return extractString(jsonStr, pos);
    } else if (firstChar == 't' || firstChar == 'f') {
        return extractBoolean(jsonStr, pos);
    } else if (firstChar == 'n') {
        return extractNull(jsonStr, pos);
    } else {
        return extractNumber(jsonStr, pos);
    }
}

void JsonParser::skipWhitespace(const std::string& jsonStr, size_t& pos) {
    while (pos < jsonStr.size() && std::isspace(jsonStr[pos])) {
        pos++;
    }
}

ConfigValue JsonParser::parseObject(const std::string& jsonStr, size_t& pos) {
    if (pos >= jsonStr.size() || jsonStr[pos] != '{') {
        throw std::runtime_error("Expected object starting with {\");");
    }
    
    pos++; // Skip opening brace
    skipWhitespace(jsonStr, pos);
    
    std::map<std::string, std::string> object;
    
    while (pos < jsonStr.size() && jsonStr[pos] != '}') {
        skipWhitespace(jsonStr, pos);
        
        if (jsonStr[pos] == '\"') {
            std::string key = extractString(jsonStr, pos);
            skipWhitespace(jsonStr, pos);
            
            if (pos >= jsonStr.size() || jsonStr[pos] != ':') {
                throw std::runtime_error("Expected colon after object key\");");
            }
            
            pos++; // Skip colon
            skipWhitespace(jsonStr, pos);
            
            std::string value = extractValue(jsonStr, pos);
            object[key] = value;
            
            skipWhitespace(jsonStr, pos);
            
            if (pos < jsonStr.size() && jsonStr[pos] == ',') {
                pos++; // Skip comma
            }
        } else {
            throw std::runtime_error("Expected string key in object\");");
        }
    }
    
    if (pos >= jsonStr.size() || jsonStr[pos] != '}') {
        throw std::runtime_error("Unterminated object\");");
    }
    
    pos++; // Skip closing brace
    return object; // For now, return as string representation
}

ConfigValue JsonParser::parseArray(const std::string& jsonStr, size_t& pos) {
    if (pos >= jsonStr.size() || jsonStr[pos] != '[') {
        throw std::runtime_error("Expected array starting with [\");");
    }
    
    pos++; // Skip opening bracket
    skipWhitespace(jsonStr, pos);
    
    std::vector<std::string> array;
    
    while (pos < jsonStr.size() && jsonStr[pos] != ']') {
        std::string value = extractValue(jsonStr, pos);
        array.push_back(value);
        
        skipWhitespace(jsonStr, pos);
        
        if (pos < jsonStr.size() && jsonStr[pos] == ',') {
            pos++; // Skip comma
        }
    }
    
    if (pos >= jsonStr.size() || jsonStr[pos] != ']') {
        throw std::runtime_error("Unterminated array\");");
    }
    
    pos++; // Skip closing bracket
    return array; // For now, return as string representation
}

ConfigValue JsonParser::extractBoolean(const std::string& jsonStr, size_t& pos) {
    if (jsonStr.substr(pos, 4) == "true") {
        pos += 4;
        return true;
    } else if (jsonStr.substr(pos, 5) == "false") {
        pos += 5;
        return false;
    } else {
        throw std::runtime_error("Invalid boolean value\");");
    }
}

ConfigValue JsonParser::extractNull(const std::string& jsonStr, size_t& pos) {
    if (jsonStr.substr(pos, 4) == "null") {
        pos += 4;
        return std::string("null");
    } else {
        throw std::runtime_error("Invalid null value\");");
    }
}

} // namespace nlm
