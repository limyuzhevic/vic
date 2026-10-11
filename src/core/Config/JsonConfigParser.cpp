#include "JsonConfigParser.hpp"
#include "core/Logger/Logger.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <mutex>
#include <nlohmann/json.hpp>

namespace nlm {

bool JsonConfigParser::parseFile(const std::string& filepath, Config& config) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        NLM_LOG_ERROR("Failed to open config file: " + filepath);
        return false;
    }
    
    std::string content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
    return parseString(content, config);
}

bool JsonConfigParser::parseString(const std::string& content, Config& config) {
    try {
        auto json = nlohmann::json::parse(content);
        convertJsonToConfig(json, config, ConfigSource::File);
        return true;
    } catch (const nlohmann::json::parse_error& e) {
        NLM_LOG_ERROR("JSON parse error: " + std::string(e.what()));
        return false;
    }
}

std::string JsonConfigParser::serializeToJson(const Config& config) {
    auto json = convertConfigToJson(config);
    return json.dump(2); // Pretty print with 2-space indentation
}

std::string JsonConfigParser::serializeToYaml(const Config& config) {
    // For now, return JSON as YAML since we don't have a YAML library
    // TODO: Add YAML support when a YAML library is available
    auto json = convertConfigToJson(config);
    return json.dump(2); // Pretty print with 2-space indentation
}

void JsonConfigParser::convertJsonToConfig(const nlohmann::json& json, Config& config, ConfigSource source) {
    if (!json.is_object()) {
        NLM_LOG_ERROR("JSON config must be an object");
        return;
    }
    
    for (const auto& [key, value] : json.items()) {
        // Recursively process nested objects
        if (value.is_object()) {
            // For now, skip nested objects as they require more complex handling
            continue;
        }
        
        ConfigValue configValue;
        
        // Convert JSON value to ConfigValue based on type
        if (value.is_boolean()) {
            configValue = value.get<bool>();
        } else if (value.is_number_integer()) {
            configValue = value.get<int64_t>();
        } else if (value.is_number_unsigned()) {
            // Convert unsigned int to int64_t
            configValue = static_cast<int64_t>(value.get<uint64_t>());
        } else if (value.is_number()) {
            configValue = value.get<double>();
        } else if (value.is_string()) {
            configValue = value.get<std::string>();
        } else if (value.is_array()) {
            // Convert array to vector
            std::vector<double> doubleArray;
            for (const auto& item : value) {
                doubleArray.push_back(item.get<double>());
            }
            configValue = doubleArray;
        }
        
        config.set(key, configValue, source);
    }
}

nlohmann::json JsonConfigParser::convertConfigToJson(const Config& config) {
    nlohmann::json json = nlohmann::json::object();
    
    for (const auto& key : config.getKeys()) {
        auto optValue = config.get<std::string>(key);
        if (optValue) {
            json[key] = *optValue;
        } else {
            auto optInt = config.get<int64_t>(key);
            if (optInt) {
                json[key] = *optInt;
            } else {
                auto optDouble = config.get<double>(key);
                if (optDouble) {
                    json[key] = *optDouble;
                } else {
                    auto optBool = config.get<bool>(key);
                    if (optBool) {
                        json[key] = *optBool;
                    }
                }
            }
        }
    }
    
    return json;
}

std::vector<std::string> JsonConfigParser::splitPath(const std::string& path) {
    std::vector<std::string> parts;
    std::stringstream ss(path);
    std::string part;
    
    while (std::getline(ss, part, '/')) {
        if (!part.empty()) {
            parts.push_back(part);
        }
    }
    
    return parts;
}

} // namespace nlm
