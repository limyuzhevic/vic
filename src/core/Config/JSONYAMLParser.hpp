#pragma once

#include <string>
#include <vector>
#include <memory>
#include <variant>
#include <unordered_map>
#include <functional>
#include <fstream>
#include <sstream>
#include <iostream>
#include <algorithm>
#include <cctype>

#include "ConfigValidation.hpp"

namespace nlm {

// JSON parsing implementation (simplified - in production would use a proper JSON library like nlohmann::json)
class JSONParser {
public:
    static std::unordered_map<std::string, ConfigValue> parse(const std::string& json) {
        std::unordered_map<std::string, ConfigValue> result;
        
        // Remove whitespace
        std::string cleanJson;
        for (char c : json) {
            if (!std::isspace(c)) {
                cleanJson += c;
            }
        }
        
        // Simple JSON parsing for key-value pairs (objects only)
        if (cleanJson.empty() || cleanJson[0] != '{' || cleanJson.back() != '}') {
            throw ConfigParseError("Invalid JSON format: must be an object", "");
        }
        
        cleanJson = cleanJson.substr(1, cleanJson.size() - 2);  // Remove braces
        
        size_t pos = 0;
        while (pos < cleanJson.size()) {
            // Find key
            size_t keyStart = cleanJson.find('"', pos);
            if (keyStart == std::string::npos) {
                throw ConfigParseError("Missing opening quote for key", cleanJson.substr(pos));
            }
            
            size_t keyEnd = cleanJson.find('"', keyStart + 1);
            if (keyEnd == std::string::npos) {
                throw ConfigParseError("Missing closing quote for key", cleanJson.substr(pos));
            }
            
            std::string key = cleanJson.substr(keyStart + 1, keyEnd - keyStart - 1);
            pos = keyEnd + 1;
            
            // Skip whitespace
            while (pos < cleanJson.size() && std::isspace(cleanJson[pos])) pos++;
            
            if (pos >= cleanJson.size() || cleanJson[pos] != ':') {
                throw ConfigParseError("Missing colon after key", cleanJson.substr(pos));
            }
            
            pos++;  // Skip colon
            while (pos < cleanJson.size() && std::isspace(cleanJson[pos])) pos++;
            
            // Find value
            ConfigValue value;
            if (pos < cleanJson.size() && cleanJson[pos] == '"') {
                // String value
                size_t valueStart = pos;
                size_t valueEnd = cleanJson.find('"', valueStart + 1);
                if (valueEnd == std::string::npos) {
                    throw ConfigParseError("Missing closing quote for string value", cleanJson.substr(pos));
                }
                value = cleanJson.substr(valueStart + 1, valueEnd - valueStart - 1);
                pos = valueEnd + 1;
            } else if (pos < cleanJson.size() && cleanJson[pos] == 't' && cleanJson.substr(pos, 4) == "true") {
                // Boolean true
                value = true;
                pos += 4;
            } else if (pos < cleanJson.size() && cleanJson[pos] == 'f' && cleanJson.substr(pos, 5) == "false") {
                // Boolean false
                value = false;
                pos += 5;
            } else if (pos < cleanJson.size() && (cleanJson[pos] == '-' || std::isdigit(cleanJson[pos]))) {
                // Number value
                size_t valueEnd = pos;
                while (valueEnd < cleanJson.size() && (std::isdigit(cleanJson[valueEnd]) || 
                                                      cleanJson[valueEnd] == '.' || 
                                                      cleanJson[valueEnd] == '-' ||
                                                      cleanJson[valueEnd] == 'e' ||
                                                      cleanJson[valueEnd] == 'E')) {
                    valueEnd++;
                }
                std::string numStr = cleanJson.substr(pos, valueEnd - pos);
                
                // Try to parse as double first
                try {
                    size_t posAfter;
                    double doubleVal = std::stod(numStr, &posAfter);
                    if (posAfter == numStr.size()) {
                        value = doubleVal;
                    } else {
                        // Try as int
                        size_t posAfterInt;
                        int64_t intVal = std::stoll(numStr, &posAfterInt);
                        if (posAfterInt == numStr.size()) {
                            value = intVal;
                        } else {
                            throw ConfigParseError("Invalid number format: " + numStr, numStr);
                        }
                    }
                } catch (...) {
                    throw ConfigParseError("Invalid number format: " + numStr, numStr);
                }
                
                pos = valueEnd;
            } else {
                throw ConfigParseError("Unsupported value type", cleanJson.substr(pos));
            }
            
            result[key] = value;
            
            // Skip whitespace
            while (pos < cleanJson.size() && std::isspace(cleanJson[pos])) pos++;
            
            if (pos < cleanJson.size() && cleanJson[pos] == ',') {
                pos++;  // Skip comma
                while (pos < cleanJson.size() && std::isspace(cleanJson[pos])) pos++;
            } else if (pos < cleanJson.size() && cleanJson[pos] != '}') {
                throw ConfigParseError("Expected ',' or '}'", cleanJson.substr(pos));
            }
        }
        
        return result;
    }
};

class YAMLParser {
public:
    static std::unordered_map<std::string, ConfigValue> parse(const std::string& yaml) {
        // Simplified YAML parsing for key-value pairs
        std::unordered_map<std::string, ConfigValue> result;
        
        std::istringstream stream(yaml);
        std::string line;
        
        while (std::getline(stream, line)) {
            // Remove whitespace and comments
            line.erase(0, line.find_first_not_of(" \t\r\n#"));
            line.erase(line.find_last_not_of(" \t\r\n") + 1);
            
            if (line.empty() || line.find(':') == std::string::npos) {
                continue;  // Skip empty lines and non-key-value pairs
            }
            
            size_t colonPos = line.find(':');
            std::string key = line.substr(0, colonPos);
            std::string value = line.substr(colonPos + 1);
            
            // Trim key and value
            key.erase(key.find_last_not_of(" \t\r\n") + 1);
            key.erase(0, key.find_first_not_of(" \t\r\n"));
            
            value.erase(0, value.find_first_not_of(" \t\r\n"));
            value.erase(value.find_last_not_of(" \t\r\n") + 1);
            
            // Remove quotes
            if (value.size() >= 2 && 
                ((value.front() == '"' && value.back() == '"') ||
                 (value.front() == '\'' && value.back() == '\''))) {
                value = value.substr(1, value.size() - 2);
            }
            
            // Parse value based on type
            ConfigValue configValue;
            if (value == "true" || value == "false") {
                configValue = (value == "true");
            } else {
                // Try numeric
                size_t pos;
                try {
                    double doubleVal = std::stod(value, &pos);
                    if (pos == value.size()) {
                        configValue = doubleVal;
                    } else {
                        int64_t intVal = std::stoll(value, &pos);
                        if (pos == value.size()) {
                            configValue = intVal;
                        } else {
                            configValue = value;  // Treat as string
                        }
                    }
                } catch (...) {
                    configValue = value;  // Treat as string
                }
            }
            
            result[key] = configValue;
        }
        
        return result;
    }
};

class JSONYAMLParser {
public:
    static std::unordered_map<std::string, ConfigValue> parse(const std::string& content, const std::string& filepath = "") {
        // Detect format
        if (content.find('{') != std::string::npos && content.find('}') != std::string::npos) {
            return JSONParser::parse(content);
        } else if (content.find(':') != std::string::npos && 
                   (content.find(' ') != std::string::npos || content.find('\t') != std::string::npos)) {
            return YAMLParser::parse(content);
        } else {
            throw ConfigParseError("Unable to determine file format. Expected JSON or YAML.", filepath);
        }
    }
};n
} // namespace nlm
