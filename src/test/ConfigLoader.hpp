#pragma once

#include <string>
#include <unordered_map>
#include <memory>
#include <fstream>
#include <sstream>
#include <iostream>
#include <algorithm>
#include <cctype>

// Simple configuration parser for tests
class TestConfig {
public:
    TestConfig() = default;
    
    // Load configuration from file
    bool loadFromFile(const std::string& filename) {
        std::ifstream file(filename);
        if (!file.is_open()) {
            return false;
        }
        
        std::string line;
        while (std::getline(file, line)) {
            parseLine(line);
        }
        
        return true;
    }
    
    // Parse a configuration line
    void parseLine(const std::string& line) {
        // Trim whitespace
        std::string trimmed = line;
        trimmed.erase(0, trimmed.find_first_not_of(" \t\n\r"));
        trimmed.erase(trimmed.find_last_not_of(" \t\n\r") + 1);
        
        if (trimmed.empty() || trimmed[0] == '#') {
            return; // Skip empty lines and comments
        }
        
        // Parse key-value pairs
        size_t equalsPos = trimmed.find('=');
        if (equalsPos != std::string::npos) {
            std::string key = trimmed.substr(0, equalsPos);
            std::string value = trimmed.substr(equalsPos + 1);
            
            // Trim key and value
            key.erase(0, key.find_first_not_of(" \t\n\r"));
            key.erase(key.find_last_not_of(" \t\n\r") + 1);
            value.erase(0, value.find_first_not_of(" \t\n\r"));
            value.erase(value.find_last_not_of(" \t\n\r") + 1);
            
            setValue(key, value);
        }
    }
    
    // Set a configuration value
    void setValue(const std::string& key, const std::string& value) {
        values[key] = value;
        
        // Handle section headers
        if (key.find('{') != std::string::npos) {
            currentSection = key.substr(0, key.find('{') - 1);
            currentSection.erase(currentSection.find_last_not_of(" \t") + 1);
        } else if (key.find('}') != std::string::npos) {
            currentSection.clear();
        } else if (!currentSection.empty()) {
            nestedValues[currentSection][key] = value;
        }
    }
    
    // Get a configuration value
    std::string getValue(const std::string& key, const std::string& defaultValue = "") {
        auto it = values.find(key);
        if (it != values.end()) {
            return it->second;
        }
        return defaultValue;
    }
    
    // Get a boolean configuration value
    bool getBool(const std::string& key, bool defaultValue = false) {
        std::string value = getValue(key);
        std::transform(value.begin(), value.end(), value.begin(), ::tolower);
        return value == "true" || value == "1" || value == "yes";
    }
    
    // Get an integer configuration value
    int getInt(const std::string& key, int defaultValue = 0) {
        std::string value = getValue(key);
        try {
            return std::stoi(value);
        } catch (...) {
            return defaultValue;
        }
    }
    
    // Get a floating-point configuration value
    double getDouble(const std::string& key, double defaultValue = 0.0) {
        std::string value = getValue(key);
        try {
            return std::stod(value);
        } catch (...) {
            return defaultValue;
        }
    }
    
    // Get nested configuration values
    std::string getNestedValue(const std::string& section, const std::string& key, 
                              const std::string& defaultValue = "") {
        auto sectionIt = nestedValues.find(section);
        if (sectionIt != nestedValues.end()) {
            auto keyIt = sectionIt->second.find(key);
            if (keyIt != sectionIt->second.end()) {
                return keyIt->second;
            }
        }
        return defaultValue;
    }
    
    // Get nested boolean configuration value
    bool getNestedBool(const std::string& section, const std::string& key, bool defaultValue = false) {
        std::string value = getNestedValue(section, key);
        std::transform(value.begin(), value.end(), value.begin(), ::tolower);
        return value == "true" || value == "1" || value == "yes";
    }
    
    // Check if a configuration key exists
    bool hasKey(const std::string& key) {
        return values.find(key) != values.end();
    }
    
    // Check if a section has a key
    bool hasSectionKey(const std::string& section, const std::string& key) {
        auto sectionIt = nestedValues.find(section);
        if (sectionIt != nestedValues.end()) {
            return sectionIt->second.find(key) != sectionIt->second.end();
        }
        return false;
    }
    
private:
    std::unordered_map<std::string, std::string> values;
    std::unordered_map<std::string, std::unordered_map<std::string, std::string>> nestedValues;
    std::string currentSection;
};
