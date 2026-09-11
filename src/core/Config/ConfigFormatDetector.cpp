#include "ConfigFormatDetector.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <cctype>
#include <nlohmann/json.hpp>
#include <yaml-cpp/yaml.h>

using json = nlohmann::json;

namespace nlm {

ConfigFormatDetector::Format ConfigFormatDetector::detectFormat(const std::string& filepath) {
    Format format = detectFormatFromExtension(filepath);
    
    // If extension-based detection is unknown, try content-based detection
    if (format == Format::Unknown) {
        std::ifstream file(filepath);
        if (!file.is_open()) {
            return Format::Unknown;
        }
        
        // Read first few lines to guess format
        std::string line;
        for (int i = 0; i < 5 && std::getline(file, line); ++i) {
            line = trim(line);
            if (line.empty()) continue;
            
            // Check for JSON array/object
            if (line.front() == '[' || (line.front() == '{' && line.back() == '}')) {
                return Format::JSON;
            }
            
            // Check for YAML-like structure (colon after key)
            if (line.find(':') != std::string::npos) {
                // Simple heuristic: if line contains colon and no equals, might be YAML
                if (line.find('=') == std::string::npos) {
                    return Format::YAML;
                }
            }
        }
    }
    
    return format;
}

ConfigFormatDetector::Format ConfigFormatDetector::detectFormatFromExtension(const std::string& filepath) {
    if (filepath.empty()) {
        return Format::Unknown;
    }
    
    std::string ext = "." + std::filesystem::path(filepath).extension().string();
    std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);
    
    if (ext == ".json") return Format::JSON;
    if (ext == ".yaml" || ext == ".yml") return Format::YAML;
    if (ext == ".toml") return Format::TOML;
    if (ext == ".cfg" || ext == ".txt") return Format::KeyValue;
    
    return Format::Unknown;
}

bool ConfigFormatDetector::isTextFormat(const Format format) {
    return format != Format::Unknown;
}

std::string ConfigFormatDetector::formatToString(const Format format) {
    switch (format) {
        case Format::KeyValue: return "key=value";
        case Format::JSON: return "JSON";
        case Format::YAML: return "YAML";
        case Format::TOML: return "TOML";
        case Format::Unknown: return "unknown";
        default: return "unknown";
    }
}

ConfigFormatDetector::Format ConfigFormatDetector::stringToFormat(const std::string& formatStr) {
    std::string lower = toLower(formatStr);
    if (lower == "key=value" || lower == "keyvalue") return Format::KeyValue;
    if (lower == "json") return Format::JSON;
    if (lower == "yaml") return Format::YAML;
    if (lower == "yml") return Format::YAML;
    if (lower == "toml") return Format::TOML;
    return Format::Unknown;
}

bool ConfigParser::parse(const std::string& filepath, ConfigMap& config) {
    Format format = ConfigFormatDetector::detectFormat(filepath);
    
    switch (format) {
        case Format::KeyValue:
            return parseKeyValue(filepath, config);
        case Format::JSON:
            return parseJSON(filepath, config);
        case Format::YAML:
            return parseYAML(filepath, config);
        case Format::TOML:
            // TODO: Implement TOML parsing
            return false;
        case Format::Unknown:
        default:
            return false;
    }
}

bool ConfigParser::parseKeyValue(const std::string& filepath, ConfigMap& config) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    return parseKeyValueFromStream(file, config);
}

bool ConfigParser::parseKeyValueFromStream(std::ifstream& file, ConfigMap& config) {
    std::string line;
    while (std::getline(file, line)) {
        // Skip empty lines and comments
        line = trim(line);
        if (line.empty() || line[0] == '#' || line[0] == '/') {
            continue;
        }
        
        // Parse key=value pairs
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
            
            config[key] = value;
        }
    }
    
    return true;
}

bool ConfigParser::parseJSON(const std::string& filepath, ConfigMap& config) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    try {
        json j;
        file >> j;
        
        return parseJSONObject(j.dump(), config);
    } catch (const std::exception&) {
        return false;
    }
}

bool ConfigParser::parseJSONObject(const std::string& content, ConfigMap& config) {
    // Simple JSON parsing - for production, use a proper JSON library
    // This is a simplified implementation
    config.clear();
    
    // TODO: Implement proper JSON parsing with nlohmann::json
    // For now, we'll just parse simple key-value pairs
    size_t pos = 0;
    while (pos < content.size()) {
        // Find next key (after { and comma or after comma)
        pos = content.find('"', pos);
        if (pos == std::string::npos) break;
        
        size_t keyEnd = content.find('"', pos + 1);
        if (keyEnd == std::string::npos) break;
        
        std::string key = content.substr(pos + 1, keyEnd - pos - 1);
        pos = keyEnd + 1;
        
        // Skip whitespace and colon
        while (pos < content.size() && (content[pos] == ' ' || content[pos] == '\t' || content[pos] == ':')) {
            pos++;
        }
        
        // Find value
        size_t valueStart = pos;
        
        // Simple value detection
        if (content[valueStart] == '"') {
            // String value
            size_t valueEnd = content.find('"', valueStart + 1);
            if (valueEnd == std::string::npos) break;
            std::string value = content.substr(valueStart + 1, valueEnd - valueStart - 1);
            config[key] = value;
            pos = valueEnd + 1;
        } else if (content[valueStart] == 't' || content[valueStart] == 'f') {
            // Boolean value
            if (content.substr(valueStart, 5) == "true") {
                config[key] = true;
                pos = valueStart + 4;
            } else if (content.substr(valueStart, 4) == "false") {
                config[key] = false;
                pos = valueStart + 5;
            }
        } else if (content[valueStart] >= '0' && content[valueStart] <= '9') {
            // Number value
            size_t valueEnd = valueStart;
            while (valueEnd < content.size() && (content[valueEnd] >= '0' && content[valueEnd] <= '9')) {
                valueEnd++;
            }
            if (valueEnd > valueStart) {
                std::string numStr = content.substr(valueStart, valueEnd - valueStart);
                config[key] = std::stoi(numStr);
                pos = valueEnd;
            }
        }
        
        // Skip whitespace
        while (pos < content.size() && (content[pos] == ' ' || content[pos] == '\t' || content[pos] == ',')) {
            pos++;
        }
    }
    
    return true;
}

bool ConfigParser::parseYAML(const std::string& filepath, ConfigMap& config) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    try {
        YAML::Node doc = YAML::LoadFile(filepath);
        return parseYAMLNode(doc, config);
    } catch (const std::exception&) {
        return false;
    }
}

bool ConfigParser::parseYAMLNode(const YAML::Node& node, ConfigMap& config) {
    if (node.IsScalar()) {
        // Leaf node - cannot be key-value
        return true;
    } else if (node.IsMap()) {
        for (const auto& it : node) {
            std::string key = it.first.as<std::string>();
            if (it.second.IsScalar()) {
                // Try to parse as different types
                std::string valueStr = it.second.as<std::string>();
                
                // Try bool
                if (valueStr == "true") {
                    config[key] = true;
                } else if (valueStr == "false") {
                    config[key] = false;
                } else {
                    // Try number
                    try {
                        if (valueStr.find('.') != std::string::npos) {
                            config[key] = std::stod(valueStr);
                        } else {
                            config[key] = std::stoll(valueStr);
                        }
                    } catch (...) {
                        config[key] = valueStr;
                    }
                }
            } else if (it.second.IsSequence()) {
                // Array
                std::vector<std::string> arr;
                for (const auto& item : it.second) {
                    arr.push_back(item.as<std::string>());
                }
                config[key] = arr;
            }
        }
    }
    return true;
}

std::string ConfigParser::serializeToKeyValue(const ConfigMap& config) {
    std::ostringstream oss;
    for (const auto& pair : config) {
        oss << pair.first << " = " << pair.second << "\n";
    }
    return oss.str();
}

std::string ConfigParser::serializeToJSON(const ConfigMap& config) {
    json j;
    
    for (const auto& pair : config) {
        std::visit([&j, &pair](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, std::string>) {
                j[pair.first] = arg;
            } else if constexpr (std::is_same_v<T, bool>) {
                j[pair.first] = arg;
            } else if constexpr (std::is_same_v<T, int> || std::is_same_v<T, int64_t>) {
                j[pair.first] = arg;
            } else if constexpr (std::is_same_v<T, double>) {
                j[pair.first] = arg;
            } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
                j[pair.first] = arg;
            }
        }, pair.second);
    }
    
    return j.dump(4);
}

std::string ConfigParser::serializeToYAML(const ConfigMap& config) {
    YAML::Emitter emitter;
    emitter << YAML::BeginMap;
    
    for (const auto& pair : config) {
        emitter << YAML::Key << pair.first;
        std::visit([&emitter](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, std::string>) {
                emitter << YAML::Value << arg;
            } else if constexpr (std::is_same_v<T, bool>) {
                emitter << YAML::Value << (arg ? "true" : "false");
            } else if constexpr (std::is_same_v<T, int> || std::is_same_v<T, int64_t>) {
                emitter << YAML::Value << arg;
            } else if constexpr (std::is_same_v<T, double>) {
                emitter << YAML::Value << arg;
            } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
                emitter << YAML::Value << YAML::BeginSeq;
                for (const auto& item : arg) {
                    emitter << item;
                }
                emitter << YAML::EndSeq;
            }
        }, pair.second);
    }
    
    emitter << YAML::EndMap;
    return emitter.c_str();
}

std::string ConfigParser::trim(const std::string& str) {
    size_t start = str.find_first_not_of(" \t\r\n");
    if (start == std::string::npos) return "";
    size_t end = str.find_last_not_of(" \t\r\n");
    return str.substr(start, end - start + 1);
}

std::string ConfigParser::toLower(const std::string& str) {
    std::string result = str;
    std::transform(result.begin(), result.end(), result.begin(), ::tolower);
    return result;
}

} // namespace nlm
