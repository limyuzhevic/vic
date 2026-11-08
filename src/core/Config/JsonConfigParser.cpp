// Implementation of JsonConfigParser

#include "JsonConfigParser.hpp"
#include <algorithm>
#include <cctype>
#include <sstream>

namespace nlm {

// Helper function for JSON to ConfigValue conversion
bool jsonToConfigValue(const json& j, ConfigValue& value) {
    if (j.is_boolean()) {
        value = j.get<bool>();
        return true;
    } else if (j.is_number_integer()) {
        value = j.get<int>();
        return true;
    } else if (j.is_number_float()) {
        value = j.get<double>();
        return true;
    } else if (j.is_string()) {
        value = j.get<std::string>();
        return true;
    } else if (j.is_array()) {
        // Convert JSON array to appropriate ConfigValue vector type
        if (j.empty()) {
            // Default to int vector
            value = std::vector<int>();
            return true;
        }
        
        // Try to determine the element type
        bool allInt = true;
        bool allDouble = true;
        bool allString = true;
        
        for (const auto& element : j) {
            if (!element.is_number_integer()) allInt = false;
            if (!element.is_number_float()) allDouble = false;
            if (!element.is_string()) allString = false;
        }
        
        if (allInt) {
            value = j.get<std::vector<int>>();
        } else if (allDouble) {
            value = j.get<std::vector<double>>();
        } else if (allString) {
            value = j.get<std::vector<std::string>>();
        } else {
            // Mixed types - fall back to string representation
            value = j.dump();
        }
        return true;
    }
    
    return false;
}

bool JsonConfigParser::parseJsonFile(const std::string& filepath, std::vector<ConfigEntry>& entries) {
    try {
        std::ifstream file(filepath);
        if (!file.is_open()) {
            return false;
        }
        
        json j;
        file >> j;
        return parseJsonObject(j, entries);
    } catch (const std::exception& e) {
        // Fallback to legacy parser
        return LegacyConfigParser::parseLegacyFile(filepath, entries);
    }
}

bool JsonConfigParser::parseJsonString(const std::string& jsonStr, std::vector<ConfigEntry>& entries) {
    try {
        json j = json::parse(jsonStr);
        return parseJsonObject(j, entries);
    } catch (const std::exception& e) {
        return false;
    }
}

bool JsonConfigParser::parseJsonObject(const json& obj, std::vector<ConfigEntry>& entries) {
    entries.clear();
    
    if (!obj.is_object()) {
        return false;
    }
    
    for (const auto& [key, value] : obj.items()) {
        ConfigEntry entry;
        entry.key = key;
        entry.source = ConfigSource::File;
        entry.description = "";
        
        if (jsonToConfigValue(value, entry.value)) {
            entries.push_back(entry);
        }
    }
    
    return true;
}

json JsonConfigParser::configEntryToJson(const ConfigEntry& entry) {
    json j;
    j[entry.key] = configValueToString(entry.value);
    return j;
}

bool JsonConfigParser::jsonToConfigEntry(const json& value, ConfigEntry& entry) {
    return jsonToConfigValue(value, entry.value);
}

bool JsonConfigParser::stringToConfigValue(const std::string& str, ConfigValue& value) {
    // Try to parse as boolean
    if (str == "true" || str == "false") {
        value = (str == "true");
        return true;
    }
    
    // Try to parse as integer
    try {
        size_t pos;
        long int_val = std::stol(str, &pos);
        if (pos == str.size()) {
            value = int_val;
            return true;
        }
    } catch (...) {}
    
    // Try to parse as floating point
    try {
        size_t pos;
        double double_val = std::stod(str, &pos);
        if (pos == str.size()) {
            value = double_val;
            return true;
        }
    } catch (...) {}
    
    // Try to parse as double
    try {
        size_t pos;
        double double_val = std::stod(str, &pos);
        if (pos == str.size()) {
            value = double_val;
            return true;
        }
    } catch (...) {}
    
    // If all else fails, treat as string
    value = str;
    return true;
}

std::string JsonConfigParser::configValueToString(const ConfigValue& value) {
    std::stringstream result;
    
    std::visit([&](auto&& arg) {
        using T = std::decay_t<decltype(arg)>;
        if constexpr (std::is_same_v<T, std::string>) {
            // Escape quotes and backslashes
            std::string escaped = arg;
            size_t pos = 0;
            while ((pos = escaped.find('"', pos)) != std::string::npos) {
                escaped.insert(pos, "\\");
                pos += 2;
            }
            pos = 0;
            while ((pos = escaped.find('\\', pos)) != std::string::npos) {
                escaped.insert(pos, "\\");
                pos += 2;
            }
            result << "\"" << escaped << "\"";
        } else if constexpr (std::is_same_v<T, bool>) {
            result << (arg ? "true" : "false");
        } else if constexpr (std::is_arithmetic_v<T>) {
            result << arg;
        } else if constexpr (std::is_same_v<T, std::vector<int>>) {
            // Implement array serialization
            result << "[";
            for (size_t i = 0; i < arg.size(); ++i) {
                if (i > 0) result << ", ";
                result << arg[i];
            }
            result << "]";
        } else if constexpr (std::is_same_v<T, std::vector<double>>) {
            // Implement array serialization
            result << "[";
            for (size_t i = 0; i < arg.size(); ++i) {
                if (i > 0) result << ", ";
                result << arg[i];
            }
            result << "]";
        } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
            // Implement array serialization
            result << "[";
            for (size_t i = 0; i < arg.size(); ++i) {
                if (i > 0) result << ", ";
                // Escape quotes and back 
    std::string escaped = arg[i];
                size_t pos = 0;
                while ((pos = escaped.find('"', pos)) != std::string::npos) {
                    escaped.insert(pos, "\\");
                    pos += 2;
                }
                pos = 0;
                while ((pos = escaped.find('\\', pos)) != std::string::npos) {
                    escaped.insert(pos, "\\");
                    pos += 2;
                }
                result << "\"" << escaped << "\"";
            }
            result << "]";
        }
    }, value);
    
    return result.str();
}

bool LegacyConfigParser::parseLegacyFile(const std::string& filepath, std::vector<ConfigEntry>& entries) {
    entries.clear();
    
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
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
            
            ConfigEntry entry;
            entry.key = key;
            entry.source = ConfigSource::File;
            entry.description = "";
            
            if (stringToConfigValue(value, entry.value)) {
                entries.push_back(entry);
            }
        }
    }
    
    return true;
}

std::string LegacyConfigParser::legacyConfigToString(const ConfigEntry& entry) {
    std::string result;
    if (entry.description.empty()) {
        result = "# ";
    }
    result += entry.key + " = " + configValueToString(entry.value) + "\n";
    return result;
}

// Helper function for string trimming
std::string trim(const std::string& str) {
    size_t start = str.find_first_not_of(" \t\r\n");
    if (start == std::string::npos) return "";
    size_t end = str.find_last_not_of(" \t\r\n");
    return str.substr(start, end - start + 1);
}

// Helper function for string to lower conversion
std::string toLower(const std::string& str) {
    std::string result = str;
    std::transform(result.begin(), result.end(), result.begin(), ::tolower);
    return result;
}