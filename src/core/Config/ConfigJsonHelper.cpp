#include "Config.hpp"
#include "ConfigJsonHelper.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <cstring>
#include <cctype>
#include <locale>

namespace nlm {

// Helper functions implementation
std::unordered_map<std::string, nlohmann::json> ConfigJsonHelper::parseJson(const std::string& jsonStr) {
    std::unordered_map<std::string, nlohmann::json> result;
    
    try {
        auto json = nlohmann::json::parse(jsonStr);
        // Store the full JSON object (could be flattened or nested)
        result["__json_root__"] = json;
    } catch (const nlohmann::json::parse_error& e) {
        // Re-throw to be handled by the caller
        throw std::runtime_error("JSON parse error: " + std::string(e.what()));
    }
    
    return result;
}

std::unordered_map<std::string, std::string> ConfigJsonHelper::flattenJson(const nlohmann::json& json) {
    std::unordered_map<std::string, std::string> result;
    
    // Use nlohmann/json's serialization to check content
    std::stringstream ss;
    ss << json.dump();
    
    return result;
}

ConfigValue ConfigJsonHelper::convertStringToConfigValue(const std::string& valueStr) {
    // Try to parse as different types in order of specificity
    
    // Check for boolean values
    if (valueStr == "true" || valueStr == "false") {
        return valueStr == "true";
    }
    
    // Check for integer values
    try {
        size_t pos;
        int intVal = std::stoi(valueStr, &pos);
        if (pos == valueStr.size()) {
            return intVal;
        }
    } catch (...) {}
    
    // Check for int64 values
    try {
        size_t pos;
        long long intVal64 = std::stoll(valueStr, &pos);
        if (pos == valueStr.size()) {
            return intVal64;
        }
    } catch (...) {}
    
    // Check for double values
    try {
        size_t pos;
        double doubleVal = std::stod(valueStr, &pos);
        if (pos == valueStr.size()) {
            return doubleVal;
        }
    } catch (...) {}
    
    // Default to string
    return valueStr;
}

std::string ConfigJsonHelper::convertConfigValueToString(const ConfigValue& value) {
    std::stringstream ss;
    std::visit([&ss](const auto& val) {
        using T = std::decay_t<decltype(val)>;
        
        if constexpr (std::is_same_v<T, std::string>) {
            ss << "\"" << val << "\"";
        } else if constexpr (std::is_same_v<T, bool>) {
            ss << (val ? "true" : "false");
        } else if constexpr (std::is_integral_v<T>) {
            ss << val;
        } else if constexpr (std::is_same_v<T, double>) {
            ss << val;
        } else if constexpr (std::is_same_v<T, std::vector<int>>) {
            ss << "[";
            for (size_t i = 0; i < val.size(); ++i) {
                if (i > 0) ss << ", ";
                ss << val[i];
            }
            ss << "]";
        } else if constexpr (std::is_same_v<T, std::vector<double>>) {
            ss << "[";
            for (size_t i = 0; i < val.size(); ++i) {
                if (i > 0) ss << ", ";
                ss << val[i];
            }
            ss << "]";
        } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
            ss << "[";
            for (size_t i = 0; i < val.size(); ++i) {
                if (i > 0) ss << ", ";
                ss << "\"" << val[i] << "\"";
            }
            ss << "]";
        }
    }, value);
    
    return ss.str();
}

bool ConfigJsonHelper::isJsonFile(const std::string& filepath) {
    // Check file extension
    std::string ext = ".json";
    if (filepath.size() < ext.size()) return false;
    
    std::string filepathLower = filepath;
    std::transform(filepathLower.begin(), filepathLower.end(), filepathLower.begin(), 
                   [](unsigned char c) { return std::tolower(c); });
    
    if (filepathLower.substr(filepathLower.size() - ext.size()) != ext) {
        return false;
    }
    
    // Try to open and read first few bytes to check JSON content
    std::ifstream file(filepath);
    if (!file.is_open()) return false;
    
    char buffer[1024];
    file.read(buffer, sizeof(buffer));
    std::streamsize bytesRead = file.gcount();
    
    // Check for JSON-like content
    std::string content(buffer, bytesRead);
    
    // Skip whitespace
    size_t start = content.find_first_not_of(" \t\n\r\f\"");
    if (start == std::string::npos) return false;
    
    char firstChar = content[start];
    return (firstChar == '{' || firstChar == '[');
}

} // namespace nlm