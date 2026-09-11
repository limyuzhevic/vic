#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <optional>
#include <cstring>
#include <cctype>

namespace nlm {

struct Config::Impl {
    std::vector<ConfigEntry> entries;
};

Config::Config() : pImpl(std::make_unique<Impl>()) {}

Config::~Config() = default;

Config::Config(Config&&) noexcept = default;

Config& Config::operator=(Config&&) noexcept = default;

bool Config::loadFromFile(const std::string& filepath) {
    // Implement proper JSON/YAML parser
    // Supports JSON (.json) and YAML (.yaml, .yml) formats with basic parsing
    
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    // Detect file format by extension
    std::string ext = getFileExtension(filepath);
    std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);
    
    bool loaded = false;
    
    // Try JSON parsing
    if (ext == "json") {
        loaded = parseJsonFile(file);
    } else if (ext == "yaml" || ext == "yml") {
        loaded = parseYamlFile(file);
    } else {
        // Unknown format, try simple format
        file.clear();
        file.seekg(0);
        return loadFromSimpleFormat(file);
    }
    
    if (!loaded) {
        // Fallback to simple format if parsing failed
        file.clear();
        file.seekg(0);
        return loadFromSimpleFormat(file);
    }
    
    return true;
}

bool Config::parseJsonFile(std::ifstream& file) {
    std::string content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
    
    // Basic JSON parsing - looks for key-value pairs
    // This is a simplified implementation for demonstration
    size_t pos = 0;
    while ((pos = content.find('"', pos)) != std::string::npos) {
        size_t keyStart = pos;
        size_t keyEnd = content.find('"', keyStart + 1);
        if (keyEnd == std::string::npos) break;
        
        std::string key = content.substr(keyStart + 1, keyEnd - keyStart - 1);
        
        // Skip to value
        pos = content.find(':', keyEnd);
        if (pos == std::string::npos) break;
        
        // Find value start
        size_t valueStart = content.find_first_not_of(' ', pos + 1);
        if (valueStart == std::string::npos) break;
        
        // Parse value based on type
        char nextChar = content[valueStart];
        if (nextChar == '"') {
            // String value
            size_t valueEnd = content.find('"', valueStart + 1);
            if (valueEnd == std::string::npos) break;
            
            std::string value = content.substr(valueStart + 1, valueEnd - valueStart - 1);
            set(key, value, ConfigSource::File);
            pos = valueEnd + 1;
        } else if (nextChar == '{') {
            // Object - skip for now
            pos = content.find('}', valueStart);
            if (pos == std::string::npos) break;
            pos++;
        } else if (nextChar == '[') {
            // Array - parse as string for now
            size_t valueEnd = findMatchingBracket(content, valueStart, '[');
            if (valueEnd == std::string::npos) break;
            
            std::string arrayContent = content.substr(valueStart + 1, valueEnd - valueStart - 1);
            // Convert array to string representation
            set(key, "[" + arrayContent + "]", ConfigSource::File);
            pos = valueEnd + 1;
        } else if (isdigit(nextChar) || nextChar == '-') {
            // Number value
            size_t valueEnd = content.find_first_of(",}", valueStart);
            if (valueEnd == std::string::npos) break;
            
            std::string valueStr = content.substr(valueStart, valueEnd - valueStart);
            try {
                if (valueStr.find('.') != std::string::npos) {
                    set(key, std::stod(valueStr), ConfigSource::File);
                } else {
                    set(key, std::stoi(valueStr), ConfigSource::File);
                }
            } catch (...) {
                set(key, valueStr, ConfigSource::File);
            }
            pos = valueEnd;
        } else if (nextChar == 't' || nextChar == 'f') {
            // Boolean value
            if (content.substr(valueStart, 4) == "true") {
                set(key, true, ConfigSource::File);
                pos = valueStart + 4;
            } else if (content.substr(valueStart, 5) == "false") {
                set(key, false, ConfigSource::File);
                pos = valueStart + 5;
            }
        }
    }
    
    return !pImpl->entries.empty();
}

bool Config::parseYamlFile(std::ifstream& file) {
    std::string content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
    
    // Basic YAML parsing - looks for key-value pairs
    size_t pos = 0;
    while ((pos = content.find_first_of('\n', pos)) != std::string::npos) {
        std::string line = content.substr(pos);
        // Remove leading whitespace
        size_t lineStart = line.find_first_not_of(" \t");
        if (lineStart == std::string::npos) {
            pos++;
            continue;
        }
        
        line = line.substr(lineStart);
        
        // Skip comments
        if (line.find("//") != std::string::npos) {
            pos++;
            continue;
        }
        
        // Find key-value separator
        size_t colonPos = line.find(':');
        if (colonPos == std::string::npos) {
            pos++;
            continue;
        }
        
        std::string key = trim(line.substr(0, colonPos));
        std::string valueStr = trim(line.substr(colonPos + 1));
        
        // Remove quotes from value
        if (valueStr.size() >= 2 && 
            ((valueStr.front() == '"' && valueStr.back() == '"') ||
             (valueStr.front() == '\'' && valueStr.back() == '\''))) {
            valueStr = valueStr.substr(1, valueStr.size() - 2);
        }
        
        // Parse value based on content
        if (valueStr.empty()) {
            // Empty value
            set(key, std::string(""), ConfigSource::File);
        } else if (valueStr == "true" || valueStr == "false") {
            set(key, valueStr == "true", ConfigSource::File);
        } else if (isdigit(valueStr[0]) || (valueStr[0] == '-' && valueStr.size() > 1 && isdigit(valueStr[1]))) {
            // Number
            try {
                if (valueStr.find('.') != std::string::npos) {
                    set(key, std::stod(valueStr), ConfigSource::File);
                } else {
                    set(key, std::stoi(valueStr), ConfigSource::File);
                }
            } catch (...) {
                set(key, valueStr, ConfigSource::File);
            }
        } else if (valueStr[0] == '[' && valueStr.back() == ']') {
            // Array - parse as string representation
            std::string arrayContent = valueStr.substr(1, valueStr.size() - 2);
            set(key, "[" + arrayContent + "]", ConfigSource::File);
        } else {
            // String value
            set(key, valueStr, ConfigSource::File);
        }
        
        pos++;
    }
    
    return !pImpl->entries.empty();
}

bool Config::loadFromSimpleFormat(std::ifstream& file) {
    std::string line;
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
        }
    }
    
    return !pImpl->entries.empty();
}

std::string Config::getFileExtension(const std::string& filepath) {
    size_t pos = filepath.find_last_of('.');
    if (pos != std::string::npos) {
        return filepath.substr(pos + 1);
    }
    return "";
}

size_t Config::findMatchingBracket(const std::string& str, size_t start, char openBracket) {
    int depth = 1;
    size_t pos = start + 1;
    
    while (pos < str.size() && depth > 0) {
        char c = str[pos];
        if (c == '[' || c == '{') {
            depth++;
        } else if (c == ']' || c == '}') {
            depth--;
        }
        pos++;
    }
    
    return (depth == 0) ? pos - 1 : std::string::npos;
}

bool Config::isspace(int c) {
    return c == ' ' || c == '\t' || c == '\n' || c == '\r';
}

bool Config::isdigit(int c) {
    return c >= '0' && c <= '9';
}