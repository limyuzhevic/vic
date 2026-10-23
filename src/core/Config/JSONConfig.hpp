// JSON Support for NLM Configuration
// Provides JSON serialization/deserialization for NLM configuration

#ifndef NLM_JSON_CONFIG_HPP
#define NLM_JSON_CONFIG_HPP

#include "EnhancedConfig.hpp"
#include <string>
#include <fstream>
#include <sstream>
#include <stdexcept>

namespace nlm {

// JSON configuration exception
class JSONConfigError : public std::runtime_error {
public:
    JSONConfigError(const std::string& message) : std::runtime_error(message) {}
};

// JSON configuration handler
class JSONConfigHandler {
public:
    // Parse JSON string
    static bool parseJSON(const std::string& json, Config& config) {
        try {
            // Simple JSON parsing for basic types
            // TODO: Replace with actual JSON library (e.g., nlohmann/json)
            
            std::istringstream iss(json);
            std::string line;
            
            while (std::getline(iss, line)) {
                // Simple parsing: "key": value
                size_t colonPos = line.find(':');
                if (colonPos == std::string::npos) continue;
                
                std::string key = trim(line.substr(0, colonPos));
                std::string valueStr = trim(line.substr(colonPos + 1));
                
                // Remove quotes from key and value if present
                if (key.size() >= 2 && key.front() == '"' && key.back() == '"') {
                    key = key.substr(1, key.size() - 2);
                }
                
                if (valueStr.size() >= 2 && valueStr.front() == '"' && valueStr.back() == '"') {
                    valueStr = valueStr.substr(1, valueStr.size() - 2);
                }
                
                // Parse value based on type
                ConfigValue configValue = parseJSONValue(valueStr);
                config.set(key, configValue, "Loaded from JSON");
            }
            
            return true;
        } catch (const std::exception& e) {
            NLM_LOG_ERROR("JSON parsing error: " + std::string(e.what()));
            return false;
        }
    }
    
    // Generate JSON string
    static std::string generateJSON(const Config& config) {
        std::ostringstream oss;
        oss << "{" << std::endl;
        
        const auto& entries = config.getEntries();
        for (size_t i = 0; i < entries.size(); ++i) {
            const ConfigEntry& entry = entries[i];
            
            oss << "  \"" << entry.key << "\": " << entry.getValueString();
            
            if (i < entries.size() - 1) {
                oss << ",";
            }
            oss << std::endl;
        }
        
        oss << "}" << std::endl;
        return oss.str();
    }
    
    // Save configuration to file as JSON
    static bool saveJSON(const Config& config, const std::string& filepath) {
        try {
            std::ofstream file(filepath);
            if (!file.is_open()) {
                throw JSONConfigError("Cannot open file for writing: " + filepath);
            }
            
            std::string json = generateJSON(config);
            file << json;
            
            file.close();
            NLM_LOG_INFO("Configuration saved as JSON to: " + filepath);
            return true;
        } catch (const std::exception& e) {
            NLM_LOG_ERROR("Failed to save JSON configuration: " + std::string(e.what()));
            return false;
        }
    }
    
    // Load configuration from file as JSON
    static bool loadJSON(const std::string& filepath, Config& config) {
        try {
            std::ifstream file(filepath);
            if (!file.is_open()) {
                throw JSONConfigError("Cannot open file for reading: " + filepath);
            }
            
            std::stringstream buffer;
            buffer << file.rdbuf();
            
            file.close();
            
            return parseJSON(buffer.str(), config);
        } catch (const std::exception& e) {
            NLM_LOG_ERROR("Failed to load JSON configuration: " + std::string(e.what()));
            return false;
        }
    }
    
    // Helper function to trim whitespace
    static std::string trim(const std::string& str) {
        size_t first = str.find_first_not_of(" \t\n\r");
        if (first == std::string::npos) return "";
        size_t last = str.find_last_not_of(" \t\n\r");
        return str.substr(first, (last - first + 1));
    }
    
    // Parse JSON value string
    static ConfigValue parseJSONValue(const std::string& valueStr) {
        // Try to parse as different types
        if (valueStr == "true" || valueStr == "false") {
            return valueStr == "true";
        }
        
        // Check for array format
        if (valueStr.size() >= 2 && valueStr.front() == '[' && valueStr.back() == ']') {
            std::string inner = valueStr.substr(1, valueStr.size() - 2);
            if (inner.empty()) {
                return std::vector<std::string>();
            }
            
            std::vector<std::string> items;
            size_t pos = 0;
            while (pos < inner.size()) {
                size_t commaPos = inner.find(',', pos);
                if (commaPos == std::string::npos) {
                    commaPos = inner.size();
                }
                
                std::string item = trim(inner.substr(pos, commaPos - pos));
                if (!item.empty()) {
                    items.push_back(item);
                }
                
                pos = commaPos + 1;
                if (pos < inner.size() && inner[pos] == ' ') {
                    pos++;
                }
            }
            
            // Try to determine if it's numeric or string array
            bool allNumbers = true;
            bool allFloats = true;
            for (const auto& item : items) {
                if (item.empty() || item == "null") {
                    allNumbers = false;
                    allFloats = false;
                    break;
                }
                
                // Check if integer
                bool isInt = true;
                for (char c : item) {
                    if (!std::isdigit(c)) {
                        isInt = false;
                        break;
                    }
                }
                if (isInt) {
                    try {
                        std::stoi(item);
                    } catch (...) {
                        isInt = false;
                    }
                }
                
                if (isInt) continue;
                
                // Check if float
                bool isFloat = true;
                size_t dotPos = item.find('.');
                if (dotPos != std::string::npos && dotPos < item.size() - 1) {
                    std::string intPart = item.substr(0, dotPos);
                    std::string fracPart = item.substr(dotPos + 1);
                    
                    // Check if integer parts are digits
                    bool intPartValid = true;
                    for (char c : intPart) {
                        if (!std::isdigit(c)) {
                            intPartValid = false;
                            break;
                        }
                    }
                    
                    bool fracPartValid = true;
                    for (char c : fracPart) {
                        if (!std::isdigit(c)) {
                            fracPartValid = false;
                            break;
                        }
                    }
                    
                    isFloat = intPartValid && fracPartValid;
                }
                
                if (!isFloat) {
                    allNumbers = false;
                    allFloats = false;
                    break;
                }
            }
            
            if (allNumbers && !allFloats) {
                std::vector<int> intArray;
                for (const auto& item : items) {
                    if (!item.empty() && item != "null") {
                        intArray.push_back(std::stoi(item));
                    }
                }
                return intArray;
            } else if (allFloats) {
                std::vector<float> floatArray;
                for (const auto& item : items) {
                    if (!item.empty() && item != "null") {
                        floatArray.push_back(std::stof(item));
                    }
                }
                return floatArray;
            } else {
                return items;
            }
        }
        
        // Check for null
        if (valueStr == "null") {
            return ConfigValue();
        }
        
        // Try to parse as integer
        try {
            int intValue = std::stoi(valueStr);
            return intValue;
        } catch (...) {}
        
        // Try to parse as float
        try {
            float floatValue = std::stof(valueStr);
            return floatValue;
        } catch (...) {}
        
        // Return as string
        return valueStr;
    }
};

} // namespace nlm

#endif // NLM_JSON_CONFIG_HPP
