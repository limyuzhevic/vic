#pragma once

#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>

namespace nlm {

// Simple JSON parser implementation for configuration
class SimpleJSONParser {
public:
    bool parse(const std::string& json, std::vector<ConfigEntry>& outEntries) {
        outEntries.clear();
        
        // Skip whitespace
        auto skipWhitespace = [](const std::string& str, size_t& pos) {
            while (pos < str.size() && std::isspace(str[pos])) {
                pos++;
            }
        };
        
        size_t pos = 0;
        skipWhitespace(json, pos);
        
        if (pos >= json.size() || json[pos] != '{') {
            return false;
        }
        
        pos++; // skip {
        skipWhitespace(json, pos);
        
        while (pos < json.size() && json[pos] != '}') {
            // Parse key
            skipWhitespace(json, pos);
            if (pos >= json.size() || json[pos] != '"') {
                return false;
            }
            
            size_t keyStart = ++pos;
            while (pos < json.size() && json[pos] != '"') {
                pos++;
            }
            
            if (pos >= json.size()) {
                return false;
            }
            
            std::string key = json.substr(keyStart, pos - keyStart);
            pos++; // skip closing "
            
            skipWhitespace(json, pos);
            if (pos >= json.size() || json[pos] != ':') {
                return false;
            }
            
            pos++; // skip :
            skipWhitespace(json, pos);
            
            // Parse value
            ConfigValue value;
            if (pos < json.size() && json[pos] == '"') {
                // String value
                pos++; // skip opening "
                size_t valueStart = pos;
                while (pos < json.size() && json[pos] != '"') {
                    pos++;
                }
                
                if (pos >= json.size()) {
                    return false;
                }
                
                value = json.substr(valueStart, pos - valueStart);
                pos++; // skip closing "
            } else if (pos < json.size() && (json[pos] == '-' || std::isdigit(json[pos]))) {
                // Number value
                size_t valueStart = pos;
                while (pos < json.size() && (std::isdigit(json[pos]) || json[pos] == '.' || json[pos] == '-')) {
                    pos++;
                }
                
                std::string numStr = json.substr(valueStart, pos - valueStart);
                
                // Try to parse as int
                try {
                    int intVal = std::stoi(numStr);
                    value = intVal;
                } catch (...) {
                    try {
                        double doubleVal = std::stod(numStr);
                        value = doubleVal;
                    } catch (...) {
                        value = numStr; // fall back to string
                    }
                }
            } else if (pos < json.size() && json[pos] == 't' || json[pos] == 'f') {
                // Boolean value
                if (pos + 4 <= json.size() && json.substr(pos, 4) == "true") {
                    value = true;
                    pos += 4;
                } else if (pos + 5 <= json.size() && json.substr(pos, 5) == "false") {
                    value = false;
                    pos += 5;
                } else {
                    return false;
                }
            } else {
                return false;
            }
            
            skipWhitespace(json, pos);
            
            // Add to entries
            outEntries.push_back(ConfigEntry{key, value, ConfigSource::File});
            
            skipWhitespace(json, pos);
            if (pos < json.size() && json[pos] == ',') {
                pos++; // skip comma
                skipWhitespace(json, pos);
            }
        }
        
        return true;
    }
};

bool Config::loadFromFile(const std::string& filepath) {
    // Try to parse as JSON first
    std::ifstream file(filepath);
    if (!file.is_open()) {
        NLM_LOG_ERROR("Failed to open config file: " + filepath + " - file may not exist or permissions may be insufficient");
        return false;
    }
    
    std::stringstream buffer;
    buffer << file.rdbuf();
    std::string content = buffer.str();
    file.close();
    
    // Try JSON parsing
    SimpleJSONParser jsonParser;
    std::vector<ConfigEntry> jsonEntries;
    if (jsonParser.parse(content, jsonEntries)) {
        // Successfully parsed as JSON
        pImpl->entries = std::move(jsonEntries);
        NLM_LOG_INFO("Configuration loaded as JSON: " + std::to_string(pImpl->entries.size()) + " entries from " + filepath);
        return true;
    }
    
    // Fall back to simple key=value format
    file.open(filepath);
    if (!file.is_open()) {
        NLM_LOG_ERROR("Failed to open config file: " + filepath);
        return false;
    }
    
    std::string line;
    size_t successCount = 0;
    size_t errorCount = 0;
    
    while (std::getline(file, line)) {
        // Skip empty lines and comments
        line = trim(line);
        if (line.empty() || line[0] == '#' || line[0] == '/') {
            continue;
        }
        
        // Parse simple key=value pairs
        size_t pos = line.find('=');
        if (pos != std::string::npos) {
            std::string key = trim(line.substr(0, pos));
            std::string value = trim(line.substr(pos + 1));
            
            // Remove quotes if present
            if (value.size() >= 2 && 
                ((value.front() == '"' && value.back() == '"') ||
                 (value.front() == '\'' && value.back() == '\''))) {
                value = value.substr(1, value.size() - 2);
            }
            
            set(key, value, ConfigSource::File);
            successCount++;
        } else {
            NLM_LOG_WARNING("Invalid config line (no '=' found): " + line);
            errorCount++;
        }
    }
    
    if (successCount == 0 && errorCount > 0) {
        NLM_LOG_ERROR("Config file parsing failed: " + std::to_string(errorCount) + " invalid lines in " + filepath);
    } else {
        NLM_LOG_INFO("Config file loaded: " + std::to_string(successCount) + " entries from " + filepath);
    }
    
    return !file.bad();
}