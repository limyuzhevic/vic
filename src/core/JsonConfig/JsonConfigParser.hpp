#pragma once

#include <string>
#include <vector>
#include <map>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include "core/Config/Config.hpp"

namespace nlm {

class JsonConfigParser {
public:
    // Parse JSON configuration from file
    static bool parseFromFile(const std::string& filepath, std::vector<ConfigEntry>& entries);
    
    // Parse JSON configuration from string
    static bool parseFromString(const std::string& jsonString, std::vector<ConfigEntry>& entries);
    
    // Convert ConfigEntry to JSON string
    static std::string toJsonString(const ConfigEntry& entry);
    
    // Validate JSON format
    static bool validateJson(const std::string& jsonString);
    
private:
    // Helper functions for JSON parsing
    static std::string trim(const std::string& str);
    static std::string toLower(const std::string& str);
    static bool isJsonValue(const std::string& str);
    static std::string parseJsonValue(const std::string& str, size_t& pos);
    static std::string parseJsonString(const std::string& str, size_t& pos);
    static std::string parseJsonNumber(const std::string& str, size_t& pos);
    static bool parseJsonBoolean(const std::string& str, size_t& pos);
};

} // namespace nlm
