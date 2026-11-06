#include "JsonConfigParser.hpp"
#include "../core/Logger/Logger.hpp"
#include <fstream>
#include <sstream>

namespace nlm {

bool JsonConfigParser::parseFromString(const std::string& jsonString,
                                        std::unordered_map<std::string, std::string>& outConfig) {
    try {
        nlohmann::json jsonData = nlohmann::json::parse(jsonString);
        
        if (!jsonData.is_object()) {
            NLM_LOG_ERROR("JSON data is not an object");
            return false;
        }
        
        for (auto it = jsonData.begin(); it != jsonData.end(); ++it) {
            std::string key = it.key();
            std::string value;
            
            // Convert JSON value to string representation
            if (it.value().is_string()) {
                value = it.value();
            } else if (it.value().is_number_integer()) {
                value = std::to_string(it.value().get<int64_t>());
            } else if (it.value().is_number_float()) {
                value = std::to_string(it.value().get<double>());
            } else if (it.value().is_boolean()) {
                value = it.value().get<bool>() ? "true" : "false";
            } else if (it.value().is_array()) {
                // For arrays, create JSON array representation
                std::stringstream ss;
                ss << "[";
                for (size_t i = 0; i < it.value().size(); ++i) {
                    if (i > 0) ss << ", ";
                    if (it.value()[i].is_string()) {
                        ss << "\"" << it.value()[i].get<std::string>() << "\"";
                    } else {
                        ss << it.value()[i].dump();
                    }
                }
                ss << "]";
                value = ss.str();
            } else {
                NLM_LOG_ERROR("Unsupported JSON type for key: " + key);
                return false;
            }
            
            if (validateConfigValue(value)) {
                outConfig[key] = value;
            } else {
                NLM_LOG_ERROR("Invalid config value for key: " + key);
                return false;
            }
        }
        
        return true;
        
    } catch (const nlohmann::json::parse_error& e) {
        NLM_LOG_ERROR("JSON parse error: " + std::string(e.what()));
        return false;
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("JSON parsing error: " + std::string(e.what()));
        return false;
    }
}

bool JsonConfigParser::parseFromFile(const std::string& filepath,
                                     std::unordered_map<std::string, std::string>& outConfig) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        NLM_LOG_ERROR("Cannot open config file: " + filepath);
        return false;
    }
    
    std::stringstream buffer;
    buffer << file.rdbuf();
    std::string jsonString = buffer.str();
    
    return parseFromString(jsonString, outConfig);
}

std::string JsonConfigParser::toJson(const std::unordered_map<std::string, std::string>& config) {
    nlohmann::json jsonData;
    
    for (const auto& pair : config) {
        jsonData[pair.first] = pair.second;
    }
    
    return jsonData.dump(2); // Pretty print with 2-space indentation
}

std::vector<std::string> JsonConfigParser::getSupportedExtensions() {
    return {".json", ".cfg", ".toml"};
}

bool JsonConfigParser::validateConfigValue(const std::string& value) {
    if (value.empty()) {
        return false;
    }
    
    // Basic validation - could be enhanced
    return true;
}

std::string JsonConfigParser::escapeJsonString(const std::string& str) {
    nlohmann::json j = str;
    return j.dump();
}

} // namespace nlm