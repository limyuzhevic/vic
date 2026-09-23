// ConfigFileLoader implementation
// Provides JSON configuration file support for NLM experiments

#include "ConfigFileLoader.hpp"
#include <nlohmann/json.hpp>
#include <iostream>
#include <algorithm>
#include <cctype>

namespace nlm {

bool ConfigFileLoader::loadFromJSON(const std::string& filepath, Config& config) {
    try {
        // Check if nlohmann/json is available
        // For now, we'll use a simple parser
        std::ifstream file(filepath);
        if (!file.is_open()) {
            return false;
        }
        
        std::stringstream buffer;
        buffer << file.rdbuf();
        
        return parseJSONConfig(buffer.str(), config);
    } catch (const std::exception& e) {
        std::cerr << "Error loading JSON config from " << filepath << ": " << e.what() << std::endl;
        return false;
    }
}

bool ConfigFileLoader::saveToJSON(const std::string& filepath, const Config& config) {
    try {
        // Simple JSON implementation for now
        std::ofstream file(filepath);
        if (!file.is_open()) {
            return false;
        }
        
        file << "{\n";
        
        auto keys = config.getKeys();
        for (size_t i = 0; i < keys.size(); ++i) {
            file << "  \"" << keys[i] << "\": \"";
            
            auto value = config.get<std::string>(keys[i]);
            if (value) {
                file << *value;
            }
            
            if (i < keys.size() - 1) {
                file << "\",\n";
            } else {
                file << "\n";
            }
        }
        
        file << "}\n";
        file.close();
        
        return true;
    } catch (const std::exception& e) {
        std::cerr << "Error saving JSON config to " << filepath << ": " << e.what() << std::endl;
        return false;
    }
}

bool ConfigFileLoader::loadFromSimpleFile(const std::string& filepath, Config& config) {
    return config.loadFromFile(filepath);
}

bool ConfigFileLoader::validateFile(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    std::string line;
    bool hasValidContent = false;
    
    while (std::getline(file, line)) {
        line = trim(line);
        if (!line.empty() && !isComment(line) && hasValidKeyValuePair(line)) {
            hasValidContent = true;
            break;
        }
    }
    
    file.close();
    return hasValidContent;
}

bool ConfigFileLoader::parseJSONConfig(const std::string& configStr, Config& config) {
    // Simple JSON parser for basic key-value pairs
    std::istringstream stream(configStr);
    std::string line;
    bool inObject = false;
    bool firstProperty = true;
    
    while (std::getline(stream, line)) {
        line = trim(line);
        
        if (line.empty()) {
            continue;
        }
        
        if (line.front() == '{') {
            inObject = true;
            continue;
        } else if (line.front() == '}') {
            inObject = false;
            continue;
        }
        
        if (inObject) {
            // Parse "key": "value"
            size_t colonPos = line.find(':');
            if (colonPos != std::string::npos && !firstProperty) {
                std::string key = trim(line.substr(0, colonPos));
                std::string value = trim(line.substr(colonPos + 1));
                
                // Remove quotes
                if (value.size() >= 2 && value.front() == '"' && value.back() == '"') {
                    value = value.substr(1, value.size() - 2);
                }
                
                if (!key.empty() && !value.empty()) {
                    config.set(key, value, ConfigSource::File);
                }
            }
            
            firstProperty = false;
        }
    }
    
    return true;
}

std::string ConfigFileLoader::trim(const std::string& str) {
    size_t start = str.find_first_not_of(" \t\r\n");
    if (start == std::string::npos) return "";
    size_t end = str.find_last_not_of(" \t\r\n");
    return str.substr(start, end - start + 1);
}

bool ConfigFileLoader::isComment(const std::string& line) {
    return line.empty() || line[0] == '#' || line[0] == ';' || 
           (line.size() >= 2 && line[0] == '/' && line[1] == '/');
}

bool ConfigFileLoader::hasValidKeyValuePair(const std::string& line) {
    size_t pos = line.find('=');
    return pos != std::string::npos && pos > 0;
}

std::string ConfigFileLoader::toLower(const std::string& str) {
    std::string result = str;
    std::transform(result.begin(), result.end(), result.begin(), ::tolower);
    return result;
}

} // namespace nlm

