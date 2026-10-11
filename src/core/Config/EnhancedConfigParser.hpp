#pragma once

#include <string>
#include <vector>
#include <memory>
#include <variant>
#include <unordered_map>
#include <functional>
#include <fstream>
#include <iostream>
#include <algorithm>
#include <cctype>
#include <stdexcept>
#include <numeric>

#include "ConfigValidation.hpp"

namespace nlm {

class ConfigParser {
public:
    static std::unordered_map<std::string, ConfigValue> parse(const std::string& content, const std::string& filepath = "") {
        // Simple key=value parser for backward compatibility
        std::unordered_map<std::string, ConfigValue> result;
        
        std::istringstream stream(content);
        std::string line;
        
        while (std::getline(stream, line)) {
            // Skip empty lines and comments
            line.erase(0, line.find_first_not_of(" \t\r\n#"));
            line.erase(line.find_last_not_of(" \t\r\n") + 1);
            
            if (line.empty() || line[0] == '#') {
                continue;
            }
            
            // Parse key=value pairs
            size_t pos = line.find('=');
            if (pos == std::string::npos) {
                continue;  // Skip malformed lines
            }
            
            std::string key = line.substr(0, pos);
            std::string value = line.substr(pos + 1);
            
            // Trim whitespace
            key.erase(0, key.find_first_not_of(" \t"));
            key.erase(key.find_last_not_of(" \t") + 1);
            
            value.erase(0, value.find_first_not_of(" \t"));
            value.erase(value.find_last_not_of(" \t") + 1);
            
            // Remove quotes
            if (value.size() >= 2 && 
                ((value.front() == '"' && value.back() == '"') ||
                 (value.front() == '\'' && value.back() == '\''))) {
                value = value.substr(1, value.size() - 2);
            }
            
            // Parse value based on type
            ConfigValue configValue;
            if (value == "true") {
                configValue = true;
            } else if (value == "false") {
                configValue = false;
            } else {
                // Try numeric parsing
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
    
    static bool isJSON(const std::string& content) {
        // Simple check for JSON object format
        return content.find('{') != std::string::npos && content.find('}') != std::string::npos;
    }
    
    static bool isYAML(const std::string& content) {
        // Simple check for YAML format (key: value pairs)
        return content.find(':') != std::string::npos && 
               (content.find('\n') != std::string::npos || content.find('\r') != std::string::npos);
    }
};

class EnhancedConfigParser {
public:
    static std::unordered_map<std::string, ConfigValue> parseFile(const std::string& filepath) {
        std::ifstream file(filepath);
        if (!file.is_open()) {
            throw ConfigParseError("Cannot open file: " + filepath, filepath);
        }
        
        std::string content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
        
        try {
            return JSONYAMLParser::parse(content, filepath);
        } catch (const ConfigParseError& e) {
            // If JSON/YAML parsing fails, try legacy key=value format
            std::cerr << "Warning: JSON/YAML parsing failed, falling back to key=value format" << std::endl;
            std::cerr << "Error: " << e.what() << std::endl;
            
            return ConfigParser::parse(content, filepath);
        }
    }
    
    static std::unordered_map<std::string, ConfigValue> parseContent(const std::string& content, const std::string& source = "") {
        // Auto-detect format
        if (ConfigParser::isJSON(content)) {
            return JSONYAMLParser::parse(content, source);
        } else if (ConfigParser::isYAML(content)) {
            return JSONYAMLParser::parse(content, source);
        } else {
            // Assume key=value format for backward compatibility
            return ConfigParser::parse(content, source);
        }
    }
};

} // namespace nlm
