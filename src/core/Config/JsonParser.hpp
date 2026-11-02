// JSON Parser for NLM Configuration
// Provides JSON parsing capabilities for the Config system

#pragma once

#include "Config.hpp"
#include <string>
#include <vector>
#include <map>
#include <optional>

namespace nlm {

class JsonParser {
public:
    // Parse JSON string into Config entries
    static bool parseJson(const std::string& jsonStr, std::vector<ConfigEntry>& entries);
    
    // Parse JSON file
    static bool parseJsonFile(const std::string& filepath, std::vector<ConfigEntry>& entries);
    
    // Convert Config entries to JSON string
    static std::string entriesToJson(const std::vector<ConfigEntry>& entries);
    
    // Validate JSON syntax
    static bool validateJson(const std::string& jsonStr);
    
    // Get list of supported JSON value types
    static std::vector<std::string> getSupportedTypes() {
        return {"int", "int64", "double", "string", "bool", "array"};
    }
    
private:
    // Helper functions for JSON parsing
    static std::optional<std::string> extractStringValue(const std::string& jsonStr, size_t& pos);
    static std::optional<int> extractIntValue(const std::string& jsonStr, size_t& pos);
    static std::optional<int64_t> extractInt64Value(const std::string& jsonStr, size_t& pos);
    static std::optional<double> extractDoubleValue(const std::string& jsonStr, size_t& pos);
    static std::optional<bool> extractBoolValue(const std::string& jsonStr, size_t& pos);
    static std::optional<std::vector<double>> extractArrayValue(const std::string& jsonStr, size_t& pos);
    static std::optional<std::vector<std::string>> extractStringArrayValue(const std::string& jsonStr, size_t& pos);
    
    static void skipWhitespace(const std::string& jsonStr, size_t& pos);
    static char peekChar(const std::string& jsonStr, size_t pos);
    static char getChar(const std::string& jsonStr, size_t& pos);
    
    // Helper functions for JSON generation
    static std::string jsonValueToString(const ConfigValue& value);
    static std::string jsonArrayToString(const std::vector<int>& arr);
    static std::string jsonArrayToString(const std::vector<double>& arr);
    static std::string jsonArrayToString(const std::vector<std::string>& arr);
};

} // namespace nlm