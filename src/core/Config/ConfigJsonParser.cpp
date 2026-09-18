#include "ConfigJsonParser.hpp"
#include <nlohmann/json.hpp>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <stdexcept>
#include <cctype>

namespace nlm {

namespace {
    // Helper function to trim string
    std::string trim(const std::string& str) {
        size_t start = str.find_first_not_of(" \t\r\n");
        if (start == std::string::npos) return "";
        size_t end = str.find_last_not_of(" \t\r\n");
        return str.substr(start, end - start + 1);
    }
    
    // Helper function to convert string to lowercase
    std::string toLower(const std::string& str) {
        std::string result = str;
        std::transform(result.begin(), result.end(), result.begin(), 
                      [](unsigned char c) { return std::tolower(c); });
        return result;
    }
    
    // Helper function to parse simple key=value from line (backward compatibility)
    bool parseKeyValueLine(const std::string& line, std::string& key, std::string& value) {
        size_t pos = line.find('=');
        if (pos != std::string::npos) {
            key = trim(line.substr(0, pos));
            value = trim(line.substr(pos + 1));
            
            // Remove quotes if present
            if (value.size() >= 2 && 
                ((value.front() == '"' && value.back() == '"') ||
                 (value.front() == '\'' && value.back() == '\''))) {
                value = value.substr(1, value.size() - 2);
            }
            return !key.empty();
        }
        return false;
    }
    
    // Helper function to determine if a string ends with specific extension
    bool hasExtension(const std::string& filename, const std::string& ext) {
        if (filename.length() < ext.length()) return false;
        return toLower(filename.substr(filename.length() - ext.length())) == toLower(ext);
    }
}

// JSON Configuration Parser Implementation
ConfigJsonParser::ConfigJsonParser(Config& config) : config_(config) {}

bool ConfigJsonParser::parseFile(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    try {
        nlohmann::json jsonData;
        file >> jsonData;
        
        // Parse JSON and load into config
        parseJsonObject(jsonData, "", ConfigSource::File);
        return true;
    } catch (const std::exception& e) {
        return false;
    }
}

bool ConfigJsonParser::parseString(const std::string& jsonStr, ConfigSource source) {
    try {
        nlohmann::json jsonData = nlohmann::json::parse(jsonStr);
        parseJsonObject(jsonData, "", source);
        return true;
    } catch (const std::exception& e) {
        return false;
    }
}

void ConfigJsonParser::parseJsonObject(const nlohmann::json& jsonData, 
                       const std::string& prefix, 
                       ConfigSource source) {
    if (jsonData.is_object()) {
        for (const auto& [key, value] : jsonData.items()) {
            std::string fullKey = prefix.empty() ? key : (prefix + "." + key);
            parseJsonValue(value, fullKey, source);
        }
    } else if (!prefix.empty()) {
        // If we have a prefix but jsonData is not an object, treat it as a value
        parseJsonValue(jsonData, prefix, source);
    }
}

void ConfigJsonParser::parseJsonValue(const nlohmann::json& value, 
                       const std::string& key, 
                       ConfigSource source) {
    if (value.is_string()) {
        config_.set(key, value.get<std::string>(), source);
    } else if (value.is_number_integer()) {
        config_.set(key, value.get<int>(), source);
    } else if (value.is_number_unsigned()) {
        config_.set(key, static_cast<int64_t>(value.get<uint64_t>()), source);
    } else if (value.is_number_float()) {
        config_.set(key, value.get<double>(), source);
    } else if (value.is_boolean()) {
        config_.set(key, value.get<bool>(), source);
    } else if (value.is_array()) {
        // For arrays, we'll convert to a string representation for now
        // This could be enhanced to support actual vector types
        std::string arrayStr = "[";
        for (size_t i = 0; i < value.size(); ++i) {
            if (i > 0) arrayStr += ",";
            if (value[i].is_string()) {
                arrayStr += "\"" + value[i].get<std::string>() + "\"";
            } else {
                arrayStr += std::to_string(value[i].get<double>());
            }
        }
        arrayStr += "]";
        config_.set(key, arrayStr, source);
    } else if (value.is_null()) {
        config_.set(key, std::string("null"), source);
    } else {
        // Unknown type, convert to string representation
        config_.set(key, value.dump(), source);
    }
}

bool ConfigJsonParser::supportsExtension(const std::string& filename) {
    return hasExtension(filename, ".json");
}

} // namespace nlm
