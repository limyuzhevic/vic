#include "core/AdvancedConfig.hpp"
#include "core/Config/Config.hpp"
#include "core/Logger/Logger.hpp"
#include <nlohmann/json.hpp>
#include <yaml-cpp/yaml.h>
#include <sstream>
#include <fstream>
#include <algorithm>

namespace nlm {

struct AdvancedConfig::Impl {
    std::unordered_map<std::string, ConfigValue> values;
    std::vector<ConfigSchema> schema;
    ConfigFormat detectedFormat;
    std::string currentFilepath;
    
    // Command implementations
    ConfigCommandLoader commandLoader;
    ConfigCommandSave commandSave;
    ConfigCommandExport commandExport;
    ConfigCommandValidate commandValidate;
    ConfigCommandMerge commandMerge;
    ConfigCommandInfo commandInfo;
};

AdvancedConfig::AdvancedConfig() : pImpl(std::make_unique<Impl>()) {}

AdvancedConfig::~AdvancedConfig() = default;

bool AdvancedConfig::loadFromFile(const std::string& filepath, ConfigFormat format) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        NLM_LOG_ERROR("Cannot open configuration file: " + filepath);
        return false;
    }
    
    std::stringstream buffer;
    buffer << file.rdbuf();
    std::string content = buffer.str();
    file.close();
    
    pImpl->currentFilepath = filepath;
    
    if (format == ConfigFormat::Auto) {
        // Auto-detect format
        if (content.find('{') != std::string::npos && content.find('}') != std::string::npos) {
            pImpl->detectedFormat = ConfigFormat::JSON;
        } else if (content.find("---") != std::string::npos) {
            pImpl->detectedFormat = ConfigFormat::YAML;
        } else {
            pImpl->detectedFormat = ConfigFormat::Text;
        }
    } else {
        pImpl->detectedFormat = format;
    }
    
    bool success = false;
    switch (pImpl->detectedFormat) {
        case ConfigFormat::JSON:
            success = loadFromJSON(content);
            break;
        case ConfigFormat::YAML:
            success = loadFromYAML(content);
            break;
        case ConfigFormat::Text:
            success = loadFromText(content);
            break;
        default:
            NLM_LOG_ERROR("Unsupported configuration format");
            break;
    }
    
    if (success) {
        NLM_LOG_INFO("Configuration loaded from: " + filepath + " (format: " + getFormatString(pImpl->detectedFormat) + ")");
    }
    
    return success;
}

bool AdvancedConfig::loadFromJSON(const std::string& jsonString) {
    try {
        auto json = nlohmann::json::parse(jsonString);
        
        // Convert JSON to ConfigValue
        convertJSONToConfig(json, values);
        
        return true;
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("JSON parsing error: " + std::string(e.what()));
        return false;
    }
}

bool AdvancedConfig::loadFromYAML(const std::string& yamlString) {
    try {
        auto yaml = YAML::Load(yamlString);
        
        // Convert YAML to ConfigValue
        convertYAMLToConfig(yaml, values);
        
        return true;
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("YAML parsing error: " + std::string(e.what()));
        return false;
    }
}

bool AdvancedConfig::loadFromText(const std::string& textString) {
    std::istringstream stream(textString);
    std::string line;
    
    while (std::getline(stream, line)) {
        // Skip comments and empty lines
        if (line.empty() || line[0] == '#') {
            continue;
        }
        
        // Parse key=value format
        size_t separatorPos = line.find('=');
        if (separatorPos == std::string::npos) {
            continue;
        }
        
        std::string key = line.substr(0, separatorPos);
        std::string value = line.substr(separatorPos + 1);
        
        // Trim whitespace
        key.erase(0, key.find_first_not_of(" \t\r\n"));
        key.erase(key.find_last_not_of(" \t\r\n") + 1);
        
        value.erase(0, value.find_first_not_of(" \t\r\n"));
        value.erase(value.find_last_not_of(" \t\r\n") + 1);
        
        // Try to infer value type
        if (value == "true" || value == "false") {
            values[key] = (value == "true");
        } else {
            try {
                // Try integer
                size_t pos;
                int64_t intValue = std::stoll(value, &pos);
                if (pos == value.size()) {
                    values[key] = intValue;
                    continue;
                }
            } catch (...) {}
            
            try {
                // Try double
                size_t pos;
                double doubleValue = std::stod(value, &pos);
                if (pos == value.size()) {
                    values[key] = doubleValue;
                    continue;
                }
            } catch (...) {}
            
            // Default to string
            values[key] = value;
        }
    }
    
    return true;
}

void AdvancedConfig::convertJSONToConfig(const nlohmann::json& json, std::unordered_map<std::string, ConfigValue>& target) {
    for (auto it = json.begin(); it != json.end(); ++it) {
        std::string key = it.key();
        
        if (it->is_null()) {
            target[key] = std::monostate();
        } else if (it->is_boolean()) {
            target[key] = it->get<bool>();
        } else if (it->is_number_integer()) {
            target[key] = it->get<int64_t>();
        } else if (it->is_number_unsigned()) {
            target[key] = static_cast<int64_t>(it->get<uint64_t>());
        } else if (it->is_number_float()) {
            target[key] = it->get<double>();
        } else if (it->is_string()) {
            target[key] = it->get<std::string>();
        } else if (it->is_array()) {
            // Convert array based on first element type
            std::vector<ConfigValue> array;
            for (const auto& element : *it) {
                if (element.is_boolean()) {
                    array.push_back(element.get<bool>());
                } else if (element.is_number_integer()) {
                    array.push_back(element.get<int64_t>());
                } else if (element.is_number_float()) {
                    array.push_back(element.get<double>());
                } else if (element.is_string()) {
                    array.push_back(element.get<std::string>());
                } else {
                    array.push_back(std::monostate());
                }
            }
            target[key] = array;
        } else if (it->is_object()) {
            // Convert object to map
            std::unordered_map<std::string, ConfigValue> object;
            convertJSONToConfig(*it, object);
            target[key] = object;
        }
    }
}

void AdvancedConfig::convertYAMLToConfig(const YAML::Node& yaml, std::unordered_map<std::string, ConfigValue>& target) {
    if (yaml.IsMap()) {
        for (const auto& it : yaml) {
            std::string key = it.first.as<std::string>();
            
            if (it.second.IsNull()) {
                target[key] = std::monostate();
            } else if (it.second.IsBoolean()) {
                target[key] = it.second.as<bool>();
            } else if (it.second.IsInteger()) {
                target[key] = it.second.as<int64_t>();
            } else if (it.second.IsFloat()) {
                target[key] = it.second.as<double>();
            } else if (it.second.IsScalar()) {
                target[key] = it.second.as<std::string>();
            } else if (it.second.IsSequence()) {
                // Convert sequence to array
                std::vector<ConfigValue> array;
                for (const auto& element : it.second) {
                    if (element.IsBoolean()) {
                        array.push_back(element.as<bool>());
                    } else if (element.IsInteger()) {
                        array.push_back(element.as<int64_t>());
                    } else if (element.IsFloat()) {
                        array.push_back(element.as<double>());
                    } else if (element.IsScalar()) {
                        array.push_back(element.as<std::string>());
                    } else {
                        array.push_back(std::monostate());
                    }
                }
                target[key] = array;
            }
        }
    }
}

std::string AdvancedConfig::exportToJSON() const {
    nlohmann::json json;
    
    for (const auto& pair : pImpl->values) {
        convertConfigToJSON(pair.second, json[pair.first]);
    }
    
    return json.dump(2); // Pretty print with 2 spaces
}

std::string AdvancedConfig::exportToYAML() const {
    YAML::Emitter emitter;
    emitter << YAML::BeginMap;
    
    for (const auto& pair : pImpl->values) {
        emitter << YAML::Key << pair.first;
        convertConfigToYAML(pair.second, emitter);
    }
    
    emitter << YAML::EndMap;
    return emitter.c_str();
}

std::string AdvancedConfig::exportToText() const {
    std::ostringstream stream;
    
    for (const auto& pair : pImpl->values) {
        stream << pair.first << " = ";
        
        if (std::holds_alternative<std::string>(pair.second)) {
            stream << '"' << std::get<std::string>(pair.second) << '"';
        } else if (std::holds_alternative<bool>(pair.second)) {
            stream << (std::get<bool>(pair.second) ? "true" : "false");
        } else if (std::holds_alternative<int64_t>(pair.second)) {
            stream << std::get<int64_t>(pair.second);
        } else if (std::holds_alternative<double>(pair.second)) {
            stream << std::get<double>(pair.second);
        } else {
            stream << "UNKNOWN";
        }
        
        stream << std::endl;
    }
    
    return stream.str();
}

void AdvancedConfig::convertConfigToJSON(const ConfigValue& value, nlohmann::json& json) {
    if (std::holds_alternative<std::monostate>(value)) {
        json = nullptr;
    } else if (std::holds_alternative<bool>(value)) {
        json = std::get<bool>(value);
    } else if (std::holds_alternative<int64_t>(value)) {
        json = std::get<int64_t>(value);
    } else if (std::holds_alternative<double>(value)) {
        json = std::get<double>(value);
    } else if (std::holds_alternative<std::string>(value)) {
        json = std::get<std::string>(value);
    } else if (std::holds_alternative<std::vector<bool>>(value)) {
        json = std::get<std::vector<bool>>(value);
    } else if (std::holds_alternative<std::vector<int64_t>>(value)) {
        json = std::get<std::vector<int64_t>>(value);
    } else if (std::holds_alternative<std::vector<double>>(value)) {
        json = std::get<std::vector<double>>(value);
    } else if (std::holds_alternative<std::vector<std::string>>(value)) {
        json = std::get<std::vector<std::string>>(value);
    } else if (std::holds_alternative<std::unordered_map<std::string, ConfigValue>>(value)) {
        nlohmann::json objectJson;
        convertConfigToJSON(std::get<std::unordered_map<std::string, ConfigValue>>(value), objectJson);
        json = objectJson;
    }
}

void AdvancedConfig::convertConfigToYAML(const ConfigValue& value, YAML::Emitter& emitter) {
    if (std::holds_alternative<std::monostate>(value)) {
        emitter << YAML::Null;
    } else if (std::holds_alternative<bool>(value)) {
        emitter << std::get<bool>(value);
    } else if (std::holds_alternative<int64_t>(value)) {
        emitter << std::get<int64_t>(value);
    } else if (std::holds_alternative<double>(value)) {
        emitter << std::get<double>(value);
    } else if (std::holds_alternative<std::string>(value)) {
        emitter << std::get<std::string>(value);
    } else if (std::holds_alternative<std::vector<bool>>(value)) {
        emitter << YAML::FlowSequenceStyle;
        emitter << YAML::BeginSeq;
        for (const bool& item : std::get<std::vector<bool>>(value)) {
            emitter << item;
        }
        emitter << YAML::EndSeq;
    } else if (std::holds_alternative<std::vector<int64_t>>(value)) {
        emitter << YAML::FlowSequenceStyle;
        emitter << YAML::BeginSeq;
        for (const int64_t& item : std::get<std::vector<int64_t>>(value)) {
            emitter << item;
        }
        emitter << YAML::EndSeq;
    } else if (std::holds_alternative<std::vector<double>>(value)) {
        emitter << YAML::FlowSequenceStyle;
        emitter << YAML::BeginSeq;
        for (const double& item : std::get<std::vector<double>>(value)) {
            emitter << item;
        }
        emitter << YAML::EndSeq;
    } else if (std::holds_alternative<std::vector<std::string>>(value)) {
        emitter << YAML::FlowSequenceStyle;
        emitter << YAML::BeginSeq;
        for (const std::string& item : std::get<std::vector<std::string>>(value)) {
            emitter << item;
        }
        emitter << YAML::EndSeq;
    } else if (std::holds_alternative<std::unordered_map<std::string, ConfigValue>>(value)) {
        emitter << YAML::BeginMap;
        for (const auto& pair : std::get<std::unordered_map<std::string, ConfigValue>>(value)) {
            emitter << YAML::Key << pair.first;
            convertConfigToYAML(pair.second, emitter);
        }
        emitter << YAML::EndMap;
    }
}

void AdvancedConfig::convertConfigToJSON(const std::unordered_map<std::string, ConfigValue>& object, nlohmann::json& json) {
    for (const auto& pair : object) {
        convertConfigToJSON(pair.second, json[pair.first]);
    }
}

std::string AdvancedConfig::getFormatString(ConfigFormat format) const {
    switch (format) {
        case ConfigFormat::Text: return "Text";
        case ConfigFormat::JSON: return "JSON";
        case ConfigFormat::YAML: return "YAML";
        case ConfigFormat::TOML: return "TOML";
        default: return "Unknown";
    }
}

} // namespace nlm
