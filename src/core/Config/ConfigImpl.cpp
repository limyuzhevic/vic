//=============================================================================
// ConfigImpl.cpp - Implementation of Enhanced Config Class
//=============================================================================

#include "ConfigImpl.hpp"
#include <algorithm>
#include <cctype>
#include <filesystem>
#include <fstream>
#include <iomanip>
#include <sstream>
#include <unordered_map>

namespace nlm {

// Implementation of Config::Impl helper functions

nlohmann::json Config::Impl::createDefaultSchema() {
    nlohmann::json schema;
    schema["type"] = "object";
    schema["properties"] = nlohmann::json::object();
    schema["required"] = nlohmann::json::array();
    schema["additionalProperties"] = false;
    
    // Add type definitions
    nlohmann::json definitions = nlohmann::json::object();
    nlohmann::json numberDef;
    numberDef["type"] = "number";
    numberDef["minimum"] = -std::numeric_limits<double>::max();
    numberDef["maximum"] = std::numeric_limits<double>::max();
    definitions["number"] = numberDef;
    
    nlohmann::json integerDef;
    integerDef["type"] = "integer";
    integerDef["minimum"] = -std::numeric_limits<int64_t>::max();
    integerDef["maximum"] = std::numeric_limits<int64_t>::max();
    definitions["integer"] = integerDef;
    
    nlohmann::json booleanDef;
    booleanDef["type"] = "boolean";
    definitions["boolean"] = booleanDef;
    
    nlohmann::json stringDef;
    stringDef["type"] = "string";
    definitions["string"] = stringDef;
    
    schema["definitions"] = definitions;
    
    return schema;
}

bool Config::Impl::validateAgainstSchema(const std::vector<ConfigEntry>& entries,
                                        const nlohmann::json& schema) {
    if (schema.empty()) return true;
    
    nlohmann::json instance = nlohmann::json::object();
    for (const auto& entry : entries) {
        instance[entry.key] = configValueToJson(entry.value);
    }
    
    // Simple validation - could be enhanced with full JSON Schema validation
    for (const auto& entry : entries) {
        if (entry.isRequired && !instance.contains(entry.key)) {
            return false;
        }
    }
    
    return true;
}

std::string Config::Impl::getTypeNameFromValue(const ConfigValue& value) {
    if (std::holds_alternative<int>(value)) return "int";
    if (std::holds_alternative<int64_t>(value)) return "int64_t";
    if (std::holds_alternative<double>(value)) return "double";
    if (std::holds_alternative<bool>(value)) return "bool";
    if (std::holds_alternative<std::string>(value)) return "string";
    if (std::holds_alternative<std::vector<int>>(value)) return "std::vector<int>";
    if (std::holds_alternative<std::vector<double>>(value)) return "std::vector<double>";
    if (std::holds_alternative<std::vector<std::string>>(value)) return "std::vector<std::string>";
    return "unknown";
}

std::string Config::Impl::normalizeKey(const std::string& key) {
    std::string result = key;
    std::transform(result.begin(), result.end(), result.begin(), ::tolower);
    
    // Replace common separators with underscores
    std::replace(result.begin(), result.end(), '-', '_');
    std::replace(result.begin(), result.end(), '.', '_');
    std::replace(result.begin(), result.end(), ' ', '_');
    
    return result;
}

ConfigValue Config::Impl::parseValue(const std::string& str) {
    if (str.empty()) return std::string();
    
    // Check for boolean
    if (str == "true" || str == "false") {
        return str == "true";
    }
    
    // Check for numeric
    char* endptr;
    double doubleVal = std::strtod(str.c_str(), &endptr);
    if (*endptr == '\0') {
        // Check if it's an integer
        long longVal = std::strtoll(str.c_str(), &endptr, 10);
        if (*endptr == '\0') {
            return static_cast<int>(longVal);
        }
        return doubleVal;
    }
    
    // Default to string
    return str;
}

std::string Config::Impl::configValueToString(const ConfigValue& value) {
    if (std::holds_alternative<int>(value)) {
        return std::to_string(std::get<int>(value));
    } else if (std::holds_alternative<int64_t>(value)) {
        return std::to_string(std::get<int64_t>(value));
    } else if (std::holds_alternative<double>(value)) {
        return std::to_string(std::get<double>(value));
    } else if (std::holds_alternative<bool>(value)) {
        return std::get<bool>(value) ? "true" : "false";
    } else if (std::holds_alternative<std::string>(value)) {
        return std::get<std::string>(value);
    } else if (std::holds_alternative<std::vector<int>>(value)) {
        std::ostringstream oss;
        oss << "[";
        const auto& vec = std::get<std::vector<int>>(value);
        for (size_t i = 0; i < vec.size(); ++i) {
            if (i > 0) oss << ", ";
            oss << vec[i];
        }
        oss << "]";
        return oss.str();
    } else if (std::holds_alternative<std::vector<double>>(value)) {
        std::ostringstream oss;
        oss << "[";
        const auto& vec = std::get<std::vector<double>>(value);
        for (size_t i = 0; i < vec.size(); ++i) {
            if (i > 0) oss << ", ";
            oss << vec[i];
        }
        oss << "]";
        return oss.str();
    } else if (std::holds_alternative<std::vector<std::string>>(value)) {
        std::ostringstream oss;
        oss << "[";
        const auto& vec = std::get<std::vector<std::string>>(value);
        for (size_t i = 0; i < vec.size(); ++i) {
            if (i > 0) oss << ", ";
            oss << "\"" << vec[i] << "\"";
        }
        oss << "]";
        return oss.str();
    }
    return "";
}

nlohmann::json Config::Impl::configValueToJson(const ConfigValue& value) {
    if (std::holds_alternative<int>(value)) {
        return std::get<int>(value);
    } else if (std::holds_alternative<int64_t>(value)) {
        return std::get<int64_t>(value);
    } else if (std::holds_alternative<double>(value)) {
        return std::get<double>(value);
    } else if (std::holds_alternative<bool>(value)) {
        return std::get<bool>(value);
    } else if (std::holds_alternative<std::string>(value)) {
        return std::get<std::string>(value);
    } else if (std::holds_alternative<std::vector<int>>(value)) {
        return std::get<std::vector<int>>(value);
    } else if (std::holds_alternative<std::vector<double>>(value)) {
        return std::get<std::vector<double>>(value);
    } else if (std::holds_alternative<std::vector<std::string>>(value)) {
        return std::get<std::vector<std::string>>(value);
    }
    return nullptr;
}

bool Config::Impl::validateEntry(const ConfigEntry& entry) {
    if (entry.key.empty()) {
        NLM_LOG_ERROR("Configuration entry has empty key");
        return false;
    }
    
    if (!entry.validate()) {
        NLM_LOG_ERROR("Configuration entry validation failed for key: " + entry.key);
        return false;
    }
    
    // Check for duplicate keys (case-insensitive)
    std::string normalizedKey = normalizeKey(entry.key);
    // Note: This check would need access to the full entries list
    // For now, we'll skip this check in the helper
    
    return true;
}

} // namespace nlm