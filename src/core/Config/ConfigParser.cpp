#include "ConfigParser.hpp"
#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <cctype>

namespace nlm {

// Helper functions
static std::string trim(const std::string& str) {
    size_t start = str.find_first_not_of(" \t\r\n");
    if (start == std::string::npos) return "";
    size_t end = str.find_last_not_of(" \t\r\n");
    return str.substr(start, end - start + 1);
}

// Convert value to string
static std::string toString(const std::variant<int, int64_t, double, bool, std::string, std::vector<int>, std::vector<double>, std::vector<std::string>, std::unordered_map<std::string, std::variant<int, double, bool, std::string, std::vector<int>, std::vector<double>, std::vector<std::string>>>>& value) {
    std::ostringstream oss;
    std::visit([&oss](auto&& arg) {
        using T = std::decay_t<decltype(arg)>;
        if constexpr (std::is_same_v<T, int> || std::is_same_v<T, int64_t>) {
            oss << arg;
        } else if constexpr (std::is_same_v<T, double>) {
            oss << arg;
        } else if constexpr (std::is_same_v<T, bool>) {
            oss << (arg ? "true" : "false");
        } else if constexpr (std::is_same_v<T, std::string>) {
            oss << "\"" << arg << "\"";
        } else if constexpr (std::is_same_v<T, std::vector<int>> || std::is_same_v<T, std::vector<double>> || std::is_same_v<T, std::vector<std::string>>) {
            oss << "[";
            for (size_t i = 0; i < arg.size(); ++i) {
                if (i > 0) oss << ", ";
                if constexpr (std::is_same_v<T, std::vector<int>>) {
                    oss << arg[i];
                } else if constexpr (std::is_same_v<T, std::vector<double>>) {
                    oss << arg[i];
                } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
                    oss << "\"" << arg[i] << "\"";
                }
            }
            oss << "]";
        } else if constexpr (std::is_same_v<T, std::unordered_map<std::string, std::variant<int, double, bool, std::string, std::vector<int>, std::vector<double>, std::vector<std::string>, std::unordered_map<std::string, std::variant<int, double, bool, std::string, std::vector<int>, std::vector<double>, std::vector<std::string>>>>>) {
            oss << "{";
            for (auto it = arg.begin(); it != arg.end(); ++it) {
                if (it != arg.begin()) oss << ", ";
                oss << "\"" << it->first << "\": ";
                // Recursively convert nested map
                oss << toString(it->second);
            }
            oss << "}";
        }
    }, value);
    return oss.str();
}

// Parse JSON and convert to nested map
std::unordered_map<std::string, std::variant<int, double, bool, std::string, std::vector<int>, std::vector<double>, std::vector<std::string>, std::unordered_map<std::string, std::variant<int, double, bool, std::string, std::vector<int>, std::vector<double>, std::vector<std::string>, std::unordered_map<std::string, std::variant<int, double, bool, std::string, std::vector<int>, std::vector<double>, std::vector<std::string>>>>>
ConfigParser::parseJSON(const std::string& jsonStr) {
    try {
        auto json = nlohmann::json::parse(jsonStr);
        return jsonToNestedMap(json);
    } catch (const nlohmann::json::parse_error& e) {
        throw ParseError(std::string("JSON parse error: ") + e.what());
    }
}

bool ConfigParser::parseJSONFromFile(const std::string& filepath, std::unordered_map<std::string, std::variant<int, double, bool, std::string, std::vector<int>, std::vector<double>, std::vector<std::string>, std::unordered_map<std::string, std::variant<int, double, bool, std::string, std::vector<int>, std::vector<double>, std::vector<std::string>, std::unordered_map<std::string, std::variant<int, double, bool, std::string, std::vector<int>, std::vector<double>, std::vector<std::string>>>>>& config) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    std::stringstream buffer;
    buffer << file.rdbuf();
    
    try {
        auto json = nlohmann::json::parse(buffer.str());
        config = jsonToNestedMap(json);
        return true;
    } catch (const nlohmann::json::parse_error& e) {
        throw ParseError(std::string("JSON file parse error: ") + e.what());
    }
}

// Parse YAML and convert to nested map
std::unordered_map<std::string, std::variant<int, double, bool, std::string, std::vector<int>, std::vector<double>, std::vector<std::string>, std::unordered_map<std::string, std::variant<int, double, bool, std::string, std::vector<int>, std::vector<double>, std::vector<std::string>, std::unordered_map<std::string, std::variant<int, double, bool, std::string, std::vector<int>, std::vector<double>, std::vector<std::string>>>>> 
ConfigParser::parseYAML(const std::string& yamlStr) {
    try {
        auto yaml = YAML::Load(yamlStr);
        return yamlToNestedMap(yaml);
    } catch (const YAML::ParserException& e) {
        throw ParseError(std::string("YAML parse error: ") + e.what());
    }
}

bool ConfigParser::parseYAMLFromFile(const std::string& filepath, std::unordered_map<std::string, std::variant<int, double, bool, std::string, std::vector<int>, std::vector<double>, std::vector<std::string>, std::unordered_map<std::string, std::variant<int, double, bool, std::string, std::vector<int>, std::vector<double>, std::vector<std::string>, std::unordered_map<std::string, std::variant<int, double, bool, std::string, std::vector<int>, std::vector<double>, std::vector<std::string>>>>> config) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    try {
        auto yaml = YAML::LoadFile(filepath);
        config = yamlToNestedMap(yaml);
        return true;
    } catch (const YAML::ParserException& e) {
        throw ParseError(std::string("YAML file parse error: ") + e.what());
    }
}

// Convert JSON to nested map
std::unordered_map<std::string, std::variant<int, double, bool, std::string, std::vector<int>, std::vector<double>, std::vector<std::string>, std::unordered_map<std::string, std::variant<int, double, bool, std::string, std::vector<int>, std::vector<double>, std::vector<std::string>, std::unordered_map<std::string, std::variant<int, double, bool, std::string, std::vector<int>, std::vector<double>, std::vector<std::string>>>>> 
ConfigParser::jsonToNestedMap(const nlohmann::json& json) {
    std::unordered_map<std::string, std::variant<int, double, bool, std::string, std::vector<int>, std::vector<double>, std::vector<std::string>, std::unordered_map<std::string, std::variant<int, double, bool, std::string, std::vector<int>, std::vector<double>, std::vector<std::string>, std::unordered_map<std::string, std::variant<int, double, bool, std::string, std::vector<int>, std::vector<double>, std::vector<std::string>>>>> result;
    
    for (auto it = json.begin(); it != json.end(); ++it) {
        std::string key = it.key();
        result[key] = jsonValueToVariant(*it);
    }
    
    return result;
}

// Convert YAML to nested map
std::unordered_map<std::string, std::variant<int, double, bool, std::string, std::vector<int>, std::vector<double>, std::vector<std::string>, std::unordered_map<std::string, std::variant<int, double, bool, std::string, std::vector<int>, std::vector<double>, std::vector<std::string>, std::unordered_map<std::string, std::variant<int, double, bool, std::string, std::vector<int>, std::vector<double>, std::vector<std::string>>>>> 
ConfigParser::yamlToNestedMap(const YAML::Node& node) {
    std::unordered_map<std::string, std::variant<int, double, bool, std::string, std::vector<int>, std::vector<double>, std::vector<std::string>, std::unordered_map<std::string, std::variant<int, double, bool, std::string, std::vector<int>, std::vector<double>, std::vector<std::string>, std::unordered_map<std::string, std::variant<int, double, bool, std::string, std::vector<int>, std::vector<double>, std::vector<std::string>>>>> result;
    
    if (node.IsMap()) {
        for (auto it = node.begin(); it != node.end(); ++it) {
            std::string key = it->first.as<std::string>();
            result[key] = yamlValueToVariant(*it);
        }
    } else {
        throw ParseError("YAML node is not a map");
    }
    
    return result;
}

// Convert JSON value to variant
std::variant<int, double, bool, std::string, std::vector<int>, std::vector<double>, std::vector<std::string>, std::unordered_map<std::string, std::variant<int, double, bool, std::string, std::vector<int>, std::vector<double>, std::vector<std::string>, std::unordered_map<std::string, std::variant<int, double, bool, std::string, std::vector<int>, std::vector<double>, std::vector<std::string>>>>> 
ConfigParser::jsonValueToVariant(const nlohmann::json& json) {
    if (json.is_number_integer()) {
        return json.get<int>();
    } else if (json.is_number_float()) {
        return json.get<double>();
    } else if (json.is_boolean()) {
        return json.get<bool>();
    } else if (json.is_string()) {
        return json.get<std::string>();
    } else if (json.is_array()) {
        if (!json.empty()) {
            if (json[0].is_number_integer()) {
                std::vector<int> vec;
                vec.reserve(json.size());
                for (const auto& item : json) {
                    vec.push_back(item.get<int>());
                }
                return vec;
            } else if (json[0].is_number_float()) {
                std::vector<double> vec;
                vec.reserve(json.size());
                for (const auto& item : json) {
                    vec.push_back(item.get<double>());
                }
                return vec;
            } else if (json[0].is_string()) {
                std::vector<std::string> vec;
                vec.reserve(json.size());
                for (const auto& item : json) {
                    vec.push_back(item.get<std::string>());
                }
                return vec;
            }
        }
        return std::vector<int>{};
    } else if (json.is_object()) {
        std::unordered_map<std::string, std::variant<int, double, bool, std::string, std::vector<int>, std::vector<double>, std::vector<std::string>, std::unordered_map<std::string, std::variant<int, double, bool, std::string, std::vector<int>, std::vector<double>, std::vector<std::string>, std::unordered_map<std::string, std::variant<int, double, bool, std::string, std::vector<int>, std::vector<double>, std::vector<std::string>>>>> nestedMap;
        for (auto it = json.begin(); it != json.end(); ++it) {
            nestedMap[it.key()] = jsonValueToVariant(*it);
        }
        return nestedMap;
    }
    
    throw ParseError("Unsupported JSON value type");
}

// Convert YAML value to variant
std::variant<int, double, bool, std::string, std::vector<int>, std::vector<double>, std::vector<std::string>, std::unordered_map<std::string, std::variant<int, double, bool, std::string, std::vector<int>, std::vector<double>, std::vector<std::string>, std::unordered_map<std::string, std::variant<int, double, bool, std::string, std::vector<int>, std::vector<double>, std::vector<std::string>>>>> 
ConfigParser::yamlValueToVariant(const YAML::Node& node) {
    if (node.IsScalar()) {
        if (node.Tag() == "!!int" || (node.IsScalar() && node.as<std::string>().find('.') == std::string::npos)) {
            return node.as<int>();
        } else if (node.Tag() == "!!float" || node.Tag() == "!!double" || (node.IsScalar() && node.as<std::string>().find('.') != std::string::npos)) {
            return node.as<double>();
        } else if (node.Tag() == "!!bool" || node.as<std::string>() == "true" || node.as<std::string>() == "false") {
            return node.as<bool>();
        } else {
            return node.as<std::string>();
        }
    } else if (node.IsSequence()) {
        if (node.empty()) {
            return std::vector<int>{};
        }
        
        // Check type of first element
        if (node[0].IsScalar()) {
            if (node[0].Tag() == "!!int" || (node[0].IsScalar() && node[0].as<std::string>().find('.') == std::string::npos)) {
                std::vector<int> vec;
                vec.reserve(node.size());
                for (const auto& item : node) {
                    vec.push_back(item.as<int>());
                }
                return vec;
            } else if (node[0].Tag() == "!!float" || node[0].Tag() == "!!double" || (node[0].IsScalar() && node[0].as<std::string>().find('.') != std::string::npos)) {
                std::vector<double> vec;
                vec.reserve(node.size());
                for (const auto& item : node) {
                    vec.push_back(item.as<double>());
                }
                return vec;
            } else {
                std::vector<std::string> vec;
                vec.reserve(node.size());
                for (const auto& item : node) {
                    vec.push_back(item.as<std::string>());
                }
                return vec;
            }
        }
    } else if (node.IsMap()) {
        std::unordered_map<std::string, std::variant<int, double, bool, std::string, std::vector<int>, std::vector<double>, std::vector<std::string>, std::unordered_map<std::string, std::variant<int, double, bool, std::string, std::vector<int>, std::vector<double>, std::vector<std::string>, std::unordered_map<std::string, std::variant<int, double, bool, std::string, std::vector<int>, std::vector<double>, std::vector<std::string>>>>> nestedMap;
        for (auto it = node.begin(); it != node.end(); ++it) {
            nestedMap[it->first.as<std::string>()] = yamlValueToVariant(*it);
        }
        return nestedMap;
    }
    
    throw ParseError("Unsupported YAML value type");
}

// Flatten nested map to flat key-value pairs
void ConfigParser::flattenConfig(
    const std::unordered_map<std::string, std::variant<int, double, bool, std::string, std::vector<int>, std::vector<double>, std::vector<std::string>, std::unordered_map<std::string, std::variant<int, double, bool, std::string, std::vector<int>, std::vector<double>, std::vector<std::string>, std::unordered_map<std::string, std::variant<int, double, bool, std::string, std::vector<int>, std::vector<double>, std::vector<std::string>>>>> &nestedConfig,
    std::vector<std::pair<std::string, std::variant<int, double, bool, std::string, std::vector<int>, std::vector<double>, std::vector<std::string>>>> &flatConfig,
    const std::string &prefix) {
    
    for (const auto& pair : nestedConfig) {
        std::string fullKey = prefix.empty() ? pair.first : prefix + "." + pair.first;
        
        if (std::holds_alternative<std::unordered_map<std::string, std::variant<int, double, bool, std::string, std::vector<int>, std::vector<double>, std::vector<std::string>, std::unordered_map<std::string, std::variant<int, double, bool, std::string, std::vector<int>, std::vector<double>, std::vector<std::string>>>>> (pair.second)) {
            // Recursively flatten nested maps
            flattenConfig(std::get<std::unordered_map<std::string, std::variant<int, double, bool, std::string, std::vector<int>, std::vector<double>, std::vector<std::string>, std::unordered_map<std::string, std::variant<int, double, bool, std::string, std::vector<int>, std::vector<double>, std::vector<std::string>, std::unordered_map<std::string, std::variant<int, double, bool, std::string, std::vector<int>, std::vector<double>, std::vector<std::string>>>>> (pair.second), flatConfig, fullKey);
        } else {
            flatConfig.emplace_back(fullKey, pair.second);
        }
    }
}

// Validate configuration
bool ConfigParser::validateConfig(const std::unordered_map<std::string, std::variant<int, double, bool, std::string, std::vector<int>, std::vector<double>, std::vector<std::string>, std::unordered_map<std::string, std::variant<int, double, bool, std::string, std::vector<int>, std::vector<double>, std::vector<std::string>, std::unordered_map<std::string, std::variant<int, double, bool, std::string, std::vector<int>, std::vector<double>, std::vector<std::string>>>>> &config) {
    // Basic validation - can be extended with specific validation rules
    for (const auto& pair : config) {
        // Check for empty key
        if (pair.first.empty()) {
            throw ValidationError("Configuration key cannot be empty");
        }
        
        // Check for valid keys (basic alphanumeric and underscore)
        if (!std::all_of(pair.first.begin(), pair.first.end(), [](char c) {
            return std::isalnum(c) || c == '_' || c == '.';
        })) {
            throw ValidationError("Invalid configuration key format: " + pair.first);
        }
        
        // Check value type (basic)
        if (std::holds_alternative<std::unordered_map<std::string, std::variant<int, double, bool, std::string, std::vector<int>, std::vector<double>, std::vector<std::string>, std::unordered_map<std::string, std::variant<int, double, bool, std::string, std::vector<int>, std::vector<double>, std::vector<std::string>, std::unordered_map<std::string, std::variant<int, double, bool, std::string, std::vector<int>, std::vector<double>, std::vector<std::string>>>>> (pair.second)) {
            // Nested map - recursively validate
            validateConfig(std::get<std::unordered_map<std::string, std::variant<int, double, bool, std::string, std::vector<int>, std::vector<double>, std::vector<std::string>, std::unordered_map<std::string, std::variant<int, double, bool, std::string, std::vector<int>, std::vector<double>, std::vector<std::string>, std::unordered_map<std::string, std::variant<int, double, bool, std::string, std::vector<int>, std::vector<double>, std::vector<std::string>>>>> (pair.second));
        }
    }
    
    return true;
}

// Convert ConfigEntry to JSON
nlohmann::json ConfigParser::configEntryToJSON(const ConfigEntry& entry) {
    nlohmann::json json;
    json["key"] = entry.key;
    json["value"] = variantToJSON(entry.value);
    json["source"] = static_cast<int>(entry.source);
    json["description"] = entry.description;
    return json;
}

// Convert JSON to ConfigEntry
ConfigEntry ConfigParser::JSONToConfigEntry(const nlohmann::json& json, ConfigSource source) {
    ConfigEntry entry;
    entry.key = json["key"].get<std::string>();
    entry.value = jsonValueFromJSON(json["value"]);
    entry.source = source; // Use provided source or existing source if present
    if (json.contains("description")) {
        entry.description = json["description"].get<std::string>();
    }
    return entry;
}

// Convert ConfigEntry to YAML
YAML::Node ConfigParser::configEntryToYAML(const ConfigEntry& entry) {
    YAML::Node node;
    node["key"] = entry.key;
    node["value"] = variantToYAML(entry.value);
    node["source"] = static_cast<int>(entry.source);
    node["description"] = entry.description;
    return node;
}

// Convert YAML to ConfigEntry
ConfigEntry ConfigParser::YAMLToConfigEntry(const YAML::Node& node, ConfigSource source) {
    ConfigEntry entry;
    entry.key = node["key"].as<std::string>();
    entry.value = yamlValueToVariant(node["value"]);
    entry.source = source; // Use provided source or existing source if present
    if (node["description"]) {
        entry.description = node["description"].as<std::string>();
    }
    return entry;
}

// Convert variant to JSON
nlohmann::json ConfigParser::variantToJSON(const std::variant<int, int64_t, double, bool, std::string, std::vector<int>, std::vector<double>, std::vector<std::string>, std::unordered_map<std::string, std::variant<int, double, bool, std::string, std::vector<int>, std::vector<double>, std::vector<std::string>, std::unordered_map<std::string, std::variant<int, double, bool, std::string, std::vector<int>, std::vector<double>, std::vector<std::string>>>>> &value) {
    if (std::holds_alternative<int>(value) || std::holds_alternative<int64_t>(value)) {
        return std::get<int>(value);
    } else if (std::holds_alternative<double>(value)) {
        return std::get<double>(value);
    } else if (std::holds_alternative<bool>(value)) {
        return std::get<bool>(value);
    } else if (std::holds_alternative<std::string>(value)) {
        return std::get<std::string>(value);
    } else if (std::holds_alternative<std::vector<int>>(value)) {
        nlohmann::json jsonArray = nlohmann::json::array();
        for (const auto& item : std::get<std::vector<int>>(value)) {
            jsonArray.push_back(item);
        }
        return jsonArray;
    } else if (std::holds_alternative<std::vector<double>>(value)) {
        nlohmann::json jsonArray = nlohmann::json::array();
        for (const auto& item : std::get<std::vector<double>>(value)) {
            jsonArray.push_back(item);
        }
        return jsonArray;
    } else if (std::holds_alternative<std::vector<std::string>>(value)) {
        nlohmann::json jsonArray = nlohmann::json::array();
        for (const auto& item : std::get<std::vector<std::string>>(value)) {
            jsonArray.push_back(item);
        }
        return jsonArray;
    } else if (std::holds_alternative<std::unordered_map<std::string, std::variant<int, double, bool, std::string, std::vector<int>, std::vector<double>, std::vector<std::string>, std::unordered_map<std::string, std::variant<int, double, bool, std::string, std::vector<int>, std::vector<double>, std::vector<std::string>>>>> (value)) {
        nlohmann::json jsonObject;
        for (const auto& pair : std::get<std::unordered_map<std::string, std::variant<int, double, bool, std::string, std::vector<int>, std::vector<double>, std::vector<std::string>, std::unordered_map<std::string, std::variant<int, double, bool, std::string, std::vector<int>, std::vector<double>, std::vector<std::string>>>>>(value)) {
            jsonObject[pair.first] = variantToJSON(pair.second);
        }
        return jsonObject;
    }
    
    throw ParseError("Unsupported variant type");
}

// Convert variant to YAML
YAML::Node ConfigParser::variantToYAML(const std::variant<int, int64_t, double, bool, std::string, std::vector<int>, std::vector<double>, std::vector<std::string>, std::unordered_map<std::string, std::variant<int, double, bool, std::string, std::vector<int>, std::vector<double>, std::vector<std::string>, std::unordered_map<std::string, std::variant<int, double, bool, std::string, std::vector<int>, std::vector<double>, std::vector<std::string>>>>> &value) {
    if (std::holds_alternative<int>(value) || std::holds_alternative<int64_t>(value)) {
        return std::get<int>(value);
    } else if (std::holds_alternative<double>(value)) {
        return std::get<double>(value);
    } else if (std::holds_alternative<bool>(value)) {
        return std::get<bool>(value);
    } else if (std::holds_alternative<std::string>(value)) {
        return std::get<std::string>(value);
    } else if (std::holds_alternative<std::vector<int>>(value)) {
        YAML::Node node = YAML::Node(YAML::NodeType::Sequence);
        for (const auto& item : std::get<std::vector<int>>(value)) {
            node.push_back(item);
        }
        return node;
    } else if (std::holds_alternative<std::vector<double>>(value)) {
        YAML::Node node = YAML::Node(YAML::NodeType::Sequence);
        for (const auto& item : std::get<std::vector<double>>(value)) {
            node.push_back(item);
        }
        return node;
    } else if (std::holds_alternative<std::vector<std::string>>(value)) {
        YAML::Node node = YAML::Node(YAML::NodeType::Sequence);
        for (const auto& item : std::get<std::vector<std::string>>(value)) {
            node.push_back(item);
        }
        return node;
    } else if (std::holds_alternative<std::unordered_map<std::string, std::variant<int, double, bool, std::string, std::vector<int>, std::vector<double>, std::vector<std::string>, std::unordered_map<std::string, std::variant<int, double, bool, std::string, std::vector<int>, std::vector<double>, std::vector<std::string>>>>> (value)) {
        YAML::Node node = YAML::Node(YAML::NodeType::Map);
        for (const auto& pair : std::get<std::unordered_map<std::string, std::variant<int, double, bool, std::string, std::vector<int>, std::vector<double>, std::vector<std::string>, std::unordered_map<std::string, std::variant<int, double, bool, std::string, std::vector<int>, std::vector<double>, std::vector<std::string>>>>>(value)) {
            node[pair.first] = variantToYAML(pair.second);
        }
        return node;
    }
    
    throw ParseError("Unsupported variant type");
}

// Convert JSON value to variant
std::variant<int, int64_t, double, bool, std::string, std::vector<int>, std::vector<double>, std::vector<std::string>, std::unordered_map<std::string, std::variant<int, double, bool, std::string, std::vector<int>, std::vector<double>, std::vector<std::string>, std::unordered_map<std::string, std::variant<int, double, bool, std::string, std::vector<int>, std::vector<double>, std::vector<std::string>>>>> 
ConfigParser::jsonValueFromJSON(const nlohmann::json& json) {
    if (json.is_number_integer()) {
        return json.get<int>();
    } else if (json.is_number_float()) {
        return json.get<double>();
    } else if (json.is_boolean()) {
        return json.get<bool>();
    } else if (json.is_string()) {
        return json.get<std::string>();
    } else if (json.is_array()) {
        if (!json.empty()) {
            if (json[0].is_number_integer()) {
                std::vector<int> vec;
                vec.reserve(json.size());
                for (const auto& item : json) {
                    vec.push_back(item.get<int>());
                }
                return vec;
            } else if (json[0].is_number_float()) {
                std::vector<double> vec;
                vec.reserve(json.size());
                for (const auto& item : json) {
                    vec.push_back(item.get<double>());
                }
                return vec;
            } else if (json[0].is_string()) {
                std::vector<std::string> vec;
                vec.reserve(json.size());
                for (const auto& item : json) {
                    vec.push_back(item.get<std::string>());
                }
                return vec;
            }
        }
        return std::vector<int>{};
    } else if (json.is_object()) {
        std::unordered_map<std::string, std::variant<int, double, bool, std::string, std::vector<int>, std::vector<double>, std::vector<std::string>, std::unordered_map<std::string, std::variant<int, double, bool, std::string, std::vector<int>, std::vector<double>, std::vector<std::string>, std::unordered_map<std::string, std::variant<int, double, bool, std::string, std::vector<int>, std::vector<double>, std::vector<std::string>>>>> nestedMap;
        for (auto it = json.begin(); it != json.end(); ++it) {
            nestedMap[it.key()] = jsonValueFromJSON(*it);
        }
        return nestedMap;
    }
    
    throw ParseError("Unsupported JSON value type");
}

// Check if a file is JSON or YAML based on extension and content
bool ConfigParser::isJSONFile(const std::string& filepath) {
    // Check extension
    if (filepath.size() >= 5 && filepath.substr(filepath.size() - 5) == ".json") {
        return true;
    }
    if (filepath.size() >= 4 && filepath.substr(filepath.size() - 4) == ".yml") {
        return false;
    }
    if (filepath.size() >= 4 && filepath.substr(filepath.size() - 4) == ".yaml") {
        return false;
    }
    
    // Fallback: try to parse as JSON first, then YAML
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    std::stringstream buffer;
    buffer << file.rdbuf();
    std::string content = buffer.str();
    
    // Try to parse as JSON
    try {
        auto json = nlohmann::json::parse(content);
        return true;
    } catch (const nlohmann::json::parse_error&) {
        // If JSON parsing fails, try YAML
        try {
            auto yaml = YAML::Load(content);
            return false;
        } catch (const YAML::ParserException&) {
            return false;
        }
    }
}

bool ConfigParser::isYAMLFile(const std::string& filepath) {
    // Check extension
    if (filepath.size() >= 5 && filepath.substr(filepath.size() - 5) == ".json") {
        return false;
    }
    if (filepath.size() >= 4 && filepath.substr(filepath.size() - 4) == ".yml") {
        return true;
    }
    if (filepath.size() >= 4 && filepath.substr(filepath.size() - 4) == ".yaml") {
        return true;
    }
    
    // Fallback: try to parse as YAML first, then JSON
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    std::stringstream buffer;
    buffer << file.rdbuf();
    std::string content = buffer.str();
    
    // Try to parse as YAML
    try {
        auto yaml = YAML::Load(content);
        return true;
    } catch (const YAML::ParserException&) {
        // If YAML parsing fails, try JSON
        try {
            auto json = nlohmann::json::parse(content);
            return false;
        } catch (const nlohmann::json::parse_error&) {
            return false;
        }
    }
}

} // namespace nlm