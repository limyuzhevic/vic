// ConfigJsonHelper.cpp - Implementation of JSON helper functions

#include "ConfigJsonHelper.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <cctype>

namespace nlm {

// Static constants
const std::string ConfigJsonHelper::JSON_EXT = ".json";
const std::string ConfigJsonHelper::KV_EXT = ".cfg";

// Helper function to flatten nested JSON objects to key=value pairs
static void flattenJsonRecursive(const nlohmann::json& json, std::string prefix,
                                 std::unordered_map<std::string, std::string>& result) {
    if (json.is_object()) {
        for (auto it = json.begin(); it != json.end(); ++it) {
            std::string key = prefix.empty() ? it.key() : prefix + "." + it.key();
            flattenJsonRecursive(it.value(), key, result);
        }
    } else if (json.is_array()) {
        // Handle arrays - treat them as single string values
        std::stringstream ss;
        ss << json.dump();
        result[prefix] = ss.str();
    } else {
        // Convert to string based on type
        std::stringstream ss;
        if (json.is_string()) {
            ss << "\"" << json.get<std::string>() << "\"";
        } else {
            ss << json.dump();
        }
        result[prefix] = ss.str();
    }
}

bool ConfigJsonHelper::isJsonFile(const std::string& filepath) {
    std::string ext = getFileExtension(filepath);
    if (ext == JSON_EXT) {
        return true;
    }
    
    // Also try to parse as JSON to determine format
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    try {
        nlohmann::json jsonData;
        file >> jsonData;
        return true;
    } catch (const std::exception&) {
        return false;
    }
}

bool ConfigJsonHelper::isKeyValueFile(const std::string& filepath) {
    std::string ext = getFileExtension(filepath);
    if (ext == KV_EXT) {
        return true;
    }
    
    // Try to parse as key=value format
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    std::string line;
    while (std::getline(file, line)) {
        line = trim(line);
        if (line.empty() || line[0] == '#' || line[0] == '/') {
            continue;
        }
        
        size_t pos = line.find('=');
        if (pos != std::string::npos) {
            return true;
        }
    }
    
    return false;
}

std::string ConfigJsonHelper::jsonToString(const nlohmann::json& value) {
    std::stringstream ss;
    if (value.is_string()) {
        ss << "\"" << value.get<std::string>() << "\"";
    } else if (value.is_boolean()) {
        ss << (value.get<bool>() ? "true" : "false");
    } else if (value.is_number_integer()) {
        ss << value.get<int64_t>();
    } else if (value.is_number_unsigned()) {
        ss << value.get<uint64_t>();
    } else if (value.is_number_float()) {
        ss << value.get<double>();
    } else if (value.is_array()) {
        ss << value.dump();
    } else if (value.is_object()) {
        ss << value.dump();
    } else {
        ss << value.dump();
    }
    return ss.str();
}

ConfigValue ConfigJsonHelper::convertStringToConfigValue(const std::string& str) {
    // Try to parse as boolean
    if (isBooleanString(str)) {
        bool val = (str == "true");
        return ConfigValue(val);
    }
    
    // Try to parse as integer
    if (isIntegerString(str)) {
        try {
            int64_t val = std::stoll(str);
            return ConfigValue(val);
        } catch (...) {}
    }
    
    // Try to parse as double
    if (isDoubleString(str)) {
        try {
            double val = std::stod(str);
            return ConfigValue(val);
        } catch (...) {}
    }
    
    // Return as string
    return ConfigValue(str);
}

void ConfigJsonHelper::flattenJson(const nlohmann::json& json, std::string prefix,
                                   std::unordered_map<std::string, std::string>& result) {
    flattenJsonRecursive(json, prefix, result);
}

std::string ConfigJsonHelper::getFileExtension(const std::string& filepath) {
    size_t pos = filepath.rfind('.');
    if (pos == std::string::npos || pos == 0) {
        return "";
    }
    return filepath.substr(pos);
}

std::string ConfigJsonHelper::trim(const std::string& str) {
    size_t start = str.find_first_not_of(" \t\r\n");
    if (start == std::string::npos) return "";
    size_t end = str.find_last_not_of(" \t\r\n");
    return str.substr(start, end - start + 1);
}

bool ConfigJsonHelper::isBooleanString(const std::string& str) {
    return str == "true" || str == "false";
}

bool ConfigJsonHelper::isIntegerString(const std::string& str) {
    if (str.empty()) return false;
    
    size_t start = 0;
    if (str[0] == '+' || str[0] == '-') {
        start = 1;
    }
    
    for (size_t i = start; i < str.size(); ++i) {
        if (!std::isdigit(str[i])) {
            return false;
        }
    }
    
    return true;
}

bool ConfigJsonHelper::isDoubleString(const std::string& str) {
    if (str.empty()) return false;
    
    bool hasDecimal = false;
    bool hasExp = false;
    bool valid = false;
    
    size_t i = 0;
    if (str[0] == '+' || str[0] == '-') {
        i = 1;
    }
    
    for (; i < str.size(); ++i) {
        if (std::isdigit(str[i])) {
            valid = true;
        } else if (str[i] == '.') {
            if (hasDecimal || hasExp) return false;
            hasDecimal = true;
        } else if (str[i] == 'e' || str[i] == 'E') {
            if (hasExp || !valid) return false;
            hasExp = true;
            valid = false; // Need digit after e
        } else if (str[i] == '+' || str[i] == '-') {
            if (i == 0 || str[i-1] != 'e' && str[i-1] != 'E') return false;
        } else {
            return false;
        }
    }
    
    return valid;
}

} // namespace nlm
