// Configuration Validation System implementation

#include "ConfigValidation.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <iostream>

namespace nlm {

// Additional helper functions implementation
bool ConfigSchema::isTypeCompatible(const std::string& value, const std::string& type) const {
    if (type == "int64_t" || type == "int" || type == "integer") {
        return isInteger(value);
    } else if (type == "float" || type == "double" || type == "number") {
        return isNumber(value);
    } else if (type == "bool" || type == "boolean") {
        return isBoolean(value);
    } else if (type == "string") {
        return true; // Any string is valid
    } else if (type == "array") {
        return isArray(value);
    }
    return false; // Unknown type
}

bool ConfigSchema::isInteger(const std::string& value) const {
    try {
        std::stoll(value);
        return true;
    } catch (...) {
        return false;
    }
}

bool ConfigSchema::isNumber(const std::string& value) const {
    try {
        std::stod(value);
        return true;
    } catch (...) {
        return false;
    }
}

bool ConfigSchema::isBoolean(const std::string& value) const {
    std::string lower = value;
    std::transform(lower.begin(), lower.end(), lower.begin(), ::tolower);
    return lower == "true" || lower == "false" || lower == "1" || lower == "0";
}

bool ConfigSchema::isArray(const std::string& value) const {
    // Simple array detection: starts with '[' and ends with ']'
    return value.size() >= 2 && value.front() == '[' && value.back() == ']';
}

bool ConfigSchema::isValueInRange(const std::string& value, const ConfigField& field) const {
    if (field.allowedValues.empty()) {
        return true; // No constraints
    }
    
    // Check against allowed values
    for (const auto& allowed : field.allowedValues) {
        if (value == allowed.first) {
            return true;
        }
    }
    return false;
}

bool ConfigSchema::isValidArray(const std::string& value, const ConfigField& field) const {
    if (!field.isArray) {
        return true;
    }
    
    if (!isArray(value)) {
        return false;
    }
    
    // Count elements in array (naive implementation)
    size_t elementCount = 0;
    size_t start = 1; // Skip opening bracket
    size_t end = value.size() - 1; // Skip closing bracket
    
    for (size_t i = start; i <= end; ++i) {
        if (value[i] == ',') {
            elementCount++;
        }
    }
    elementCount++; // Account for last element
    
    return elementCount >= field.minArraySize && 
           (field.maxArraySize == 0 || elementCount <= field.maxArraySize);
}

void ConfigValidator::parseConfigFile(const std::string& filename, std::shared_ptr<Config> config) {
    if (!config) {
        throw std::invalid_argument("Config is null");
    }
    
    std::ifstream file(filename);
    if (!file.is_open()) {
        throw std::runtime_error("Could not open config file: " + filename);
    }
    
    std::string line;
    int lineNumber = 0;
    while (std::getline(file, line)) {
        lineNumber++;
        // Skip empty lines and comments
        if (line.empty() || line.find('#') == 0) {
            continue;
        }
        
        // Parse key-value pair
        size_t pos = line.find('=');
        if (pos == std::string::npos) {
            throw std::runtime_error("Invalid config line " + std::to_string(lineNumber) +
                                    ": expected 'key = value' format");
        }
        
        std::string key = line.substr(0, pos);
        std::string value = line.substr(pos + 1);
        
        // Trim whitespace
        key.erase(0, key.find_first_not_of(" \t"));
        key.erase(key.find_last_not_of(" \t") + 1);
        value.erase(0, value.find_first_not_of(" \t"));
        value.erase(value.find_last_not_of(" \t") + 1);
        
        if (key.empty() || value.empty()) {
            throw std::runtime_error("Invalid config line " + std::to_string(lineNumber) +
                                    ": key or value is empty");
        }
        
        // Set configuration value
        try {
            // Try to parse as different types
            if (value.find('.') != std::string::npos) {
                // Try float
                try {
                    float floatValue = std::stof(value);
                    config->set(key, floatValue);
                    continue;
                } catch (...) {}
            }
            
            // Try int64_t
            try {
                int64_t intValue = std::stoll(value);
                config->set(key, intValue);
                continue;
            } catch (...) {}
            
            // Try boolean
            if (value == "true" || value == "false") {
                bool boolValue = (value == "true");
                config->set(key, boolValue);
                continue;
            }
            
            // Default to string
            config->set(key, value);
        } catch (const std::exception& e) {
            throw std::runtime_error("Failed to set config value for key '" + key + "': " + e.what());
        }
    }
}

} // namespace nlm
