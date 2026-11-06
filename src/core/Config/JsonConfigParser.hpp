// Simple JSON Parser for NLM Configuration
// Provides JSON parsing capabilities for config files

#pragma once

#include <string>
#include <unordered_map>
#include <vector>
#include <nlohmann/json.hpp>

namespace nlm {

class JsonConfigParser {
public:
    // Parse JSON from a string
    static bool parseFromString(const std::string& jsonString, 
                               std::unordered_map<std::string, std::string>& outConfig);
    
    // Parse JSON from a file
    static bool parseFromFile(const std::string& filepath,
                             std::unordered_map<std::string, std::string>& outConfig);
    
    // Convert config map to JSON string
    static std::string toJson(const std::unordered_map<std::string, std::string>& config);
    
    // Get supported file extensions
    static std::vector<std::string> getSupportedExtensions();
    
private:
    static bool validateConfigValue(const std::string& value);
    static std::string escapeJsonString(const std::string& str);
};

} // namespace nlm