#include "core/JSONConfig.hpp"
#include <sstream>
#include <fstream>
#include <algorithm>
#include <cctype>
#include <iomanip>
#include <cmath>

namespace nlm {

// Parse JSON string to ConfigValue map
std::unordered_map<std::string, ConfigValue> JSONConfig::parseJSON(const std::string& jsonString) {
    try {
        auto jsonData = nlohmann::json::parse(jsonString);
        return flattenJSON(jsonData);
    } catch (const nlohmann::json::parse_error& e) {
        throw JSONConfigException(JSONConfigException::JSONErrorCode::InvalidJSON,
            std::string("JSON parse error: ") + e.what());
    }
}

// Parse JSON file to ConfigValue map
std::unordered_map<std::string, ConfigValue> JSONConfig::parseJSONFile(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        throw ConfigException(ConfigException::ErrorCode::FileNotFound,
            std::string("Cannot open file: ") + filepath);
    }
    
    std::stringstream buffer;
    buffer << file.rdbuf();
    
    return parseJSON(buffer.str());
}

// Convert ConfigValue map to JSON string
std::string JSONConfig::toJSON(const std::unordered_map<std::string, ConfigValue>& configMap) {
    nlohmann::json jsonData;
    
    for (const auto& pair : configMap) {
        jsonData[pair.first] = configValueToJSON(pair.second);
    }
    
    return jsonData.dump();
}

// Convert ConfigValue map to formatted JSON string
std::string JSONConfig::toFormattedJSON(const std::unordered_map<std::string, ConfigValue>& configMap, int indent) {
    nlohmann::json jsonData;
    
    for (const auto& pair : configMap) {
        jsonData[pair.first] = configValueToJSON(pair.second);
    }
    
    return jsonData.dump(indent);
}

// Convert single ConfigValue to JSON
nlohmann::json JSONConfig::configValueToJSON(const ConfigValue& value) {
    std::visit(nlohmann::json_overloader{
        [](int val) { return nlohmann::json(val); },
        [](int64_t val) { return nlohmann::json(val); },
        [](double val) { return nlohmann::json(val); },
        [](bool val) { return nlohmann::json(val); },
        [](const std::string& val) { return nlohmann::json(val); },
        [](const std::vector<int>& val) {
            nlohmann::json array = nlohmann::json::array();
            for (const auto& item : val) {
                array.push_back(item);
            }
            return array;
        },
        [](const std::vector<double>& val) {
            nlohmann::json array = nlohmann::json::array();
            for (const auto& item : val) {
                array.push_back(item);
            }
            return array;
        },
        [](const std::vector<std::string>& val) {
            nlohmann::json array = nlohmann::json::array();
            for (const auto& item : val) {
                array.push_back(item);
            }
            return array;
        }
    }, value);
}

// Convert JSON to ConfigValue
ConfigValue JSONConfig::jsonToConfigValue(const nlohmann::json& value) {
    if (value.is_string()) {
        return value.get<std::string>();
    } else if (value.is_number_integer()) {
        return value.get<int>();
    } else if (value.is_number_unsigned()) {
        return value.get<uint64_t>();
    } else if (value.is_number()) {
        return value.get<double>();
    } else if (value.is_boolean()) {
        return value.get<bool>();
    } else if (value.is_array()) {
        return jsonArrayToConfigVector(value);
    } else if (value.is_object()) {
        throw JSONConfigException(JSONConfigException::JSONErrorCode::NestedObjectError,
            "Nested JSON objects need to be flattened");
    }
    
    return std::string();
}

// Validate JSON against schema
bool JSONConfig::validateJSON(const nlohmann::json& jsonData, const ConfigSchema& schema) {
    auto errors = validateJSON(jsonData, schema, false);
    return errors.empty();
}

// Validate JSON against schema and return detailed errors
std::vector<std::string> JSONConfig::validateJSON(const nlohmann::json& jsonData, const ConfigSchema& schema, bool throwOnError) {
    std::vector<std::string> errors;
    
    for (const auto& pair : schema.required) {
        if (pair.second && !jsonData.contains(pair.first)) {
            std::string error = "Missing required field: " + pair.first;
            errors.push_back(error);
            if (throwOnError) {
                throw JSONConfigException(JSONConfigException::JSONErrorCode::MissingRequiredField, error);
            }
        }
    }
    
    for (const auto& pair : schema.types) {
        if (jsonData.contains(pair.first)) {
            const auto& jsonValue = jsonData[pair.first];
            const std::string& expectedType = pair.second;
            
            std::string actualType = jsonTypeToConfigType(jsonValue);
            
            if (!actualType.empty() && actualType != expectedType) {
                std::string error = "Type mismatch for field '" + pair.first + 
                                   "': expected '" + expectedType + 
                                   "', got '" + actualType + "'";
                errors.push_back(error);
                if (throwOnError) {
                    throw JSONConfigException(JSONConfigException::JSONErrorCode::TypeMismatch, error);
                }
            }
        }
    }
    
    return errors;
}

// Flatten nested JSON objects into flat key=value pairs
std::unordered_map<std::string, ConfigValue> JSONConfig::flattenJSON(const nlohmann::json& jsonData, const std::string& prefix) {
    std::unordered_map<std::string, ConfigValue> result;
    
    for (const auto& item : jsonData.items()) {
        std::string key = prefix.empty() ? item.key() : prefix + "." + item.key();
        
        if (item.value().is_object()) {
            auto nestedMap = flattenJSON(item.value(), key);
            for (const auto& nestedPair : nestedMap) {
                result[nestedPair.first] = nestedPair.second;
            }
        } else {
            try {
                result[key] = jsonToConfigValue(item.value());
            } catch (const JSONConfigException& e) {
                if (e.getJSONErrorCode() == JSONConfigException::JSONErrorCode::NestedObjectError) {
                    continue;
                }
                throw;
            }
        }
    }
    
    return result;
}

// Handle special JSON types
std::vector<ConfigValue> JSONConfig::jsonArrayToConfigVector(const nlohmann::json& array) {
    std::vector<ConfigValue> result;
    
    for (const auto& item : array) {
        result.push_back(jsonToConfigValue(item));
    }
    
    return result;
}

nlohmann::json JSONConfig::configVectorToJSONArray(const std::vector<ConfigValue>& vector) {
    nlohmann::json array = nlohmann::json::array();
    
    for (const auto& item : vector) {
        array.push_back(configValueToJSON(item));
    }
    
    return array;
}

// String conversion utilities
std::string JSONConfig::configValueToString(const ConfigValue& value) {
    std::stringstream ss;
    
    std::visit([&ss](auto&& arg) {
        using T = std::decay_t<decltype(arg)>;
        if constexpr (std::is_same_v<T, std::string>) {
            ss << '"' << arg << '"';
        } else if constexpr (std::is_same_v<T, std::vector<int>> ||
                           std::is_same_v<T, std::vector<double>> ||
                           std::is_same_v<T, std::vector<std::string>>) {
            ss << '[';
            bool first = true;
            for (const auto& item : arg) {
                if (!first) ss << ', ';
                ss << item;
                first = false;
            }
            ss << ']';
        } else {
            ss << arg;
        }
    }, value);
    
    return ss.str();
}

std::string JSONConfig::escapeJSONString(const std::string& str) {
    std::string result;
    result.reserve(str.size() * 2);
    
    for (char c : str) {
        switch (c) {
            case '\"': result += "\\\""; break;
            case '\\': result += "\\\\"; break;
            case '\b': result += "\\b"; break;
            case '\f': result += "\\f"; break;
            case '\n': result += "\\n"; break;
            case '\r': result += "\\r"; break;
            case '\t': result += "\\t"; break;
            default:
                if (static_cast<unsigned char>(c) < 32) {
                    result += "\\u" + std::to_string(static_cast<int>(c));
                } else {
                    result += c;
                }
                break;
        }
    }
    
    return result;
}

// Type checking utilities
bool JSONConfig::isNumeric(const nlohmann::json& value) {
    return value.is_number();
}

bool JSONConfig::isBoolean(const nlohmann::json& value) {
    return value.is_boolean();
}

bool JSONConfig::isString(const nlohmann::json& value) {
    return value.is_string();
}

bool JSONConfig::isArray(const nlohmann::json& value) {
    return value.is_array();
}

bool JSONConfig::isObject(const nlohmann::json& value) {
    return value.is_object();
}

// Schema validation helpers
std::string JSONConfig::jsonTypeToConfigType(const nlohmann::json& value) {
    if (value.is_string()) {
        return "string";
    } else if (value.is_number_integer()) {
        return "int";
    } else if (value.is_number_unsigned()) {
        return "uint64_t";
    } else if (value.is_number()) {
        return "double";
    } else if (value.is_boolean()) {
        return "bool";
    } else if (value.is_array()) {
        return "array";
    } else if (value.is_object()) {
        return "object";
    }
    
    return "";
}

bool JSONConfig::convertJSONValueToType(const nlohmann::json& jsonValue, const std::string& targetType, ConfigValue& outValue) {
    if (targetType == "string" && jsonValue.is_string()) {
        outValue = jsonValue.get<std::string>();
        return true;
    } else if ((targetType == "int" || targetType == "integer") && jsonValue.is_number_integer()) {
        outValue = jsonValue.get<int>();
        return true;
    } else if (targetType == "uint64_t" && jsonValue.is_number_unsigned()) {
        outValue = jsonValue.get<uint64_t>();
        return true;
    } else if (targetType == "double" && jsonValue.is_number()) {
        outValue = jsonValue.get<double>();
        return true;
    } else if (targetType == "bool" && jsonValue.is_boolean()) {
        outValue = jsonValue.get<bool>();
        return true;
    } else if (targetType == "array" && jsonValue.is_array()) {
        outValue = jsonArrayToConfigVector(jsonValue);
        return true;
    }
    
    return false;
}

// Error handling
std::string JSONConfig::getJSONErrorMessage(const std::exception& e) {
    return e.what();
}

} // namespace nlm