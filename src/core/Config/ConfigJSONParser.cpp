#include "ConfigJSONParser.hpp"
#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <string>
#include <nlohmann/json.hpp>

namespace nlm {

std::unordered_map<std::string, std::string> ConfigJSONParser::parseJSON(const std::string& jsonStr) {
    std::unordered_map<std::string, std::string> configMap;
    
    try {
        auto json = nlohmann::json::parse(jsonStr);
        
        for (const auto& [key, value] : json.items()) {
            // Convert JSON value to string representation
            configMap[key] = value.dump();
        }
    } catch (const nlohmann::json::parse_error& e) {
        // Fall back to simple parser if JSON parsing fails
        return parseSimpleFormat(jsonStr);
    }
    
    return configMap;
}

std::unordered_map<std::string, std::string> ConfigJSONParser::parseJSONFile(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return std::unordered_map<std::string, std::string>();
    }
    
    std::stringstream buffer;
    buffer << file.rdbuf();
    
    return parseJSON(buffer.str());
}

template<typename T>
std::string ConfigJSONParser::valueToString(const T& value) {
    std::ostringstream oss;
    oss << value;
    return oss.str();
}

template<typename T>
T ConfigJSONParser::stringToValue(const std::string& str) {
    std::istringstream iss(str);
    T value;
    iss >> value;
    return value;
}

// Fallback simple parser for non-JSON format
std::unordered_map<std::string, std::string> ConfigJSONParser::parseSimpleFormat(const std::string& content) {
    std::unordered_map<std::string, std::string> configMap;
    std::stringstream ss(content);
    std::string line;
    
    while (std::getline(ss, line)) {
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
            
            configMap[key] = value;
        }
    }
    
    return configMap;
}

} // namespace nlm