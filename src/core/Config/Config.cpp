#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <stdexcept>
#include <regex>

#ifdef HAS_NLOHMANN_JSON
#include <nlohmann/json.hpp>
#endif

namespace nlm {

// Simple YAML parser (for basic key-value pairs)
namespace yaml_parser {
    struct YamlValue {
        enum Type { STRING, INTEGER, FLOAT, BOOLEAN, NULL_VALUE } type;
        std::string stringValue;
        int64_t intValue;
        double floatValue;
        bool boolValue;
        
        YamlValue() : type(NULL_VALUE) {}
        explicit YamlValue(const std::string& v) : type(STRING), stringValue(v) {}
        explicit YamlValue(int64_t v) : type(INTEGER), intValue(v) {}
        explicit YamlValue(double v) : type(FLOAT), floatValue(v) {}
        explicit YamlValue(bool v) : type(BOOLEAN), boolValue(v) {}
    };
    
    std::vector<std::pair<std::string, YamlValue>> parseYamlFile(const std::string& filepath) {
        std::vector<std::pair<std::string, YamlValue>> result;
        std::ifstream file(filepath);
        if (!file.is_open()) {
            throw std::runtime_error("Cannot open YAML file: " + filepath);
        }
        
        std::string line;
        std::string currentKey;
        size_t indentLevel = 0;
        
        while (std::getline(file, line)) {
            // Trim whitespace
            size_t start = line.find_first_not_of(" \t\r\n");
            if (start == std::string::npos) continue;
            
            size_t end = line.find_last_not_of(" \t\r\n");
            line = line.substr(start, end - start + 1);
            
            // Skip comments
            if (line.substr(0, 2) == "# " || line.substr(0, 2) == "//") {
                continue;
            }
            
            // Determine indentation
            size_t currentIndent = line.find_first_not_of(" \t");
            
            // Handle nested structures (simplified - only handles simple key-value pairs)
            if (currentKey.empty() && currentIndent == 0 && !line.empty() && line.find(':') != std::string::npos) {
                size_t colonPos = line.find(':');
                std::string key = trim(line.substr(0, colonPos));
                std::string valueStr = trim(line.substr(colonPos + 1));
                
                // Parse value based on content
                YamlValue value;
                if (valueStr.empty() || valueStr == "null" || valueStr == "None") {
                    value = YamlValue();
                } else if (valueStr == "true" || valueStr == "false") {
                    value = YamlValue(valueStr == "true");
                } else if (std::all_of(valueStr.begin(), valueStr.end(), ::isdigit) ||
                           (valueStr[0] == '-' && std::all_of(valueStr.begin() + 1, valueStr.end(), ::isdigit))) {
                    value = YamlValue(std::stoll(valueStr));
                } else if (valueStr.find('.') != std::string::npos || 
                           (valueStr.find('.') != std::string::npos && valueStr[0] == '-')) {
                    try {
                        value = YamlValue(std::stod(valueStr));
                    } catch (...) {
                        value = YamlValue(valueStr);
                    }
                } else {
                    // Remove quotes if present
                    if (valueStr.size() >= 2 && 
                        ((valueStr.front() == '"' && valueStr.back() == '"') ||
                         (valueStr.front() == '\'' && valueStr.back() == '\''))) {
                        valueStr = valueStr.substr(1, valueStr.size() - 2);
                    }
                    value = YamlValue(valueStr);
                }
                
                result.emplace_back(key, value);
                currentKey = key;
                indentLevel = currentIndent;
            } else if (!currentKey.empty() && currentIndent > indentLevel) {
                // Nested value (simplified handling)
                if (currentKey == "parameters") {
                    // Handle parameter nested values
                    if (valueStr.find(':') != std::string::npos) {
                        size_t colonPos = valueStr.find(':');
                        std::string paramKey = trim(valueStr.substr(0, colonPos));
                        std::string paramValue = trim(valueStr.substr(colonPos + 1));
                        
                        YamlValue paramVal;
                        if (paramValue.empty() || paramValue == "null" || paramValue == "None") {
                            paramVal = YamlValue();
                        } else if (paramValue == "true" || paramValue == "false") {
                            paramVal = YamlValue(paramValue == "true");
                        } else if (std::all_of(paramValue.begin(), paramValue.end(), ::isdigit) ||
                                   (paramValue[0] == '-' && std::all_of(paramValue.begin() + 1, paramValue.end(), ::isdigit))) {
                            paramVal = YamlValue(std::stoll(paramValue));
                        } else if (paramValue.find('.') != std::string::npos || 
                                   (paramValue.find('.') != std::string::npos && paramValue[0] == '-')) {
                            try {
                                paramVal = YamlValue(std::stod(paramValue));
                            } catch (...) {
                                paramVal = YamlValue(paramValue);
                            }
                        } else {
                            paramVal = YamlValue(paramValue);
                        }
                        
                        result.emplace_back(paramKey, paramVal);
                    }
                }
            }
        }
        
        return result;
    }
}

struct Config::Impl {
    std::vector<ConfigEntry> entries;
};

Config::Config() : pImpl(std::make_unique<Impl>()) {}

Config::~Config() = default;

Config::Config(Config&&) noexcept = default;

Config& Config::operator=(Config&&) noexcept = default;

bool Config::loadFromFile(const std::string& filepath) {
    // Determine file format based on extension
    std::string extension = "";
    size_t dotPos = filepath.find_last_of('.');
    if (dotPos != std::string::npos) {
        extension = filepath.substr(dotPos + 1);
    }
    
    try {
        // Try JSON first
        if (extension == "json" || extension == "JSON" || extension.empty()) {
            return loadFromJsonFile(filepath);
        } 
        // Then try YAML
        else if (extension == "yaml" || extension == "yml" || extension == "YAML" || extension == "YML") {
            return loadFromYamlFile(filepath);
        }
        
        // Fallback to simple key=value format
        return loadFromSimpleFile(filepath);
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Failed to load config from " + filepath + ": " + std::string(e.what()));
        return false;
    }
}

bool Config::loadFromJsonFile(const std::string& filepath) {
#ifdef HAS_NLOHMANN_JSON
    std::ifstream file(filepath);
    if (!file.is_open()) {
        throw std::runtime_error("Cannot open JSON file: " + filepath);
    }
    
    try {
        auto json = nlohmann::json::parse(file);
        
        // Clear existing entries
        pImpl->entries.clear();
        
        // Parse JSON recursively
        parseJsonToConfig(json, "", ConfigSource::File);
        
        NLM_LOG_INFO("Loaded configuration from JSON file: " + filepath + " (" + 
                    std::to_string(pImpl->entries.size()) + " entries)");
        return true;
        
    } catch (const nlohmann::json::parse_error& e) {
        throw std::runtime_error("JSON parse error in " + filepath + ": " + std::string(e.what()));
    } catch (const std::exception& e) {
        throw std::runtime_error("Error reading JSON file " + filepath + ": " + std::string(e.what()));
    }
#else
    throw std::runtime_error("JSON support not available - nlohmann/json not compiled in");
#endif
}

bool Config::loadFromYamlFile(const std::string& filepath) {
    try {
        auto yamlData = yaml_parser::parseYamlFile(filepath);
        
        // Clear existing entries
        pImpl->entries.clear();
        
        // Convert YAML data to Config entries
        for (const auto& pair : yamlData) {
            ConfigValue configValue;
            
            switch (pair.second.type) {
                case yaml_parser::YamlValue::STRING:
                    configValue = pair.second.stringValue;
                    break;
                case yaml_parser::YamlValue::INTEGER:
                    configValue = pair.second.intValue;
                    break;
                case yaml_parser::YamlValue::FLOAT:
                    configValue = pair.second.floatValue;
                    break;
                case yaml_parser::YamlValue::BOOLEAN:
                    configValue = pair.second.boolValue;
                    break;
                case yaml_parser::YamlValue::NULL_VALUE:
                    configValue = std::string(""); // Empty string for null values
                    break;
            }
            
            set(pair.first, configValue, ConfigSource::File);
        }
        
        NLM_LOG_INFO("Loaded configuration from YAML file: " + filepath + " (" + 
                    std::to_string(pImpl->entries.size()) + " entries)");
        return true;
        
    } catch (const std::exception& e) {
        throw std::runtime_error("Error reading YAML file " + filepath + ": " + std::string(e.what()));
    }
}

bool Config::loadFromSimpleFile(const std::string& filepath) {
    // Original simple key=value format parsing (placeholder for backward compatibility)
    std::ifstream file(filepath);
    if (!file.is_open()) {
        throw std::runtime_error("Cannot open config file: " + filepath);
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
            
            set(key, value, ConfigSource::File);
        }
    }
    
    NLM_LOG_INFO("Loaded configuration from simple file: " + filepath + " (" + 
                std::to_string(pImpl->entries.size()) + " entries)");
    return true;
}

void Config::parseJsonToConfig(const nlohmann::json& json, const std::string& prefix, ConfigSource source) {
#ifdef HAS_NLOHMANN_JSON
    if (json.is_object()) {
        for (auto it = json.begin(); it != json.end(); ++it) {
            std::string key = prefix.empty() ? it.key() : (prefix + "." + it.key());
            parseJsonToConfig(it.value(), key, source);
        }
    } else if (json.is_array()) {
        // Handle arrays - convert to vector of strings for simplicity
        std::vector<std::string> arrayData;
        for (const auto& item : json) {
            arrayData.push_back(item.dump());
        }
        set(prefix, arrayData, source);
    } else {
        // Handle primitive types
        ConfigValue value;
        
        if (json.is_string()) {
            value = json.get<std::string>();
        } else if (json.is_number_integer()) {
            value = json.get<int64_t>();
        } else if (json.is_number_float()) {
            value = json.get<double>();
        } else if (json.is_boolean()) {
            value = json.get<bool>();
        } else if (json.is_null()) {
            value = std::string(""); // Empty string for null values
        }
        
        if (!prefix.empty()) {
            set(prefix, value, source);
        }
    }
#endif
}
}

bool Config::loadFromArgs(int argc, char** argv) {
    for (int i = 1; i < argc; ++i) {
        std::string arg(argv[i]);
        
        // Handle --key=value format
        if (arg.substr(0, 2) == "--") {
            size_t pos = arg.find('=');
            if (pos != std::string::npos) {
                std::string key = arg.substr(2, pos - 2);
                std::string value = arg.substr(pos + 1);
                set(key, value, ConfigSource::CommandLine);
            }
        }
        // Handle -key value format
        else if (arg[0] == '-' && i + 1 < argc) {
            std::string key = arg.substr(1);
            std::string value = argv[++i];
            set(key, value, ConfigSource::CommandLine);
        }
    }
    return true;
}

bool Config::saveToFile(const std::string& filepath) const {
    // Determine file format based on extension
    std::string extension = "";
    size_t dotPos = filepath.find_last_of('.');
    if (dotPos != std::string::npos) {
        extension = filepath.substr(dotPos + 1);
    }
    
    try {
        // Try JSON first
        if (extension == "json" || extension == "JSON" || extension.empty()) {
            return saveToJsonFile(filepath);
        } 
        // Then try YAML
        else if (extension == "yaml" || extension == "yml" || extension == "YAML" || extension == "YML") {
            return saveToYamlFile(filepath);
        }
        
        // Fallback to simple key=value format
        return saveToSimpleFile(filepath);
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Failed to save config to " + filepath + ": " + std::string(e.what()));
        return false;
    }
}

bool Config::saveToJsonFile(const std::string& filepath) const {
#ifdef HAS_NLOHMANN_JSON
    std::ofstream file(filepath);
    if (!file.is_open()) {
        NLM_LOG_ERROR("Failed to open JSON file for writing: " + filepath);
        return false;
    }
    
    try {
        nlohmann::json jsonConfig;
        
        for (const auto& entry : pImpl->entries) {
            // Handle different value types
            std::visit([&](const auto& val) {
                using T = std::decay_t<decltype(val)>;
                
                if constexpr (std::is_same_v<T, std::string>) {
                    jsonConfig[entry.key] = val;
                } else if constexpr (std::is_same_v<T, int64_t>) {
                    jsonConfig[entry.key] = val;
                } else if constexpr (std::is_same_v<T, double>) {
                    jsonConfig[entry.key] = val;
                } else if constexpr (std::is_same_v<T, bool>) {
                    jsonConfig[entry.key] = val;
                } else if constexpr (std::is_same_v<T, std::vector<int>>) {
                    // Convert to array of numbers
                    nlohmann::json arrayJson = nlohmann::json::array();
                    for (int item : val) {
                        arrayJson.push_back(item);
                    }
                    jsonConfig[entry.key] = arrayJson;
                } else if constexpr (std::is_same_v<T, std::vector<double>>) {
                    // Convert to array of numbers
                    nlohmann::json arrayJson = nlohmann::json::array();
                    for (double item : val) {
                        arrayJson.push_back(item);
                    }
                    jsonConfig[entry.key] = arrayJson;
                } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
                    // Convert to array of strings
                    nlohmann::json arrayJson = nlohmann::json::array();
                    for (const std::string& item : val) {
                        arrayJson.push_back(item);
                    }
                    jsonConfig[entry.key] = arrayJson;
                }
            }, entry.value);
        }
        
        // Write JSON to file with indentation
        file << std::setw(2) << jsonConfig;
        
        NLM_LOG_INFO("Configuration saved to JSON file: " + filepath + " (" + 
                    std::to_string(pImpl->entries.size()) + " entries)");
        return true;
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Error writing JSON file " + filepath + ": " + std::string(e.what()));
        return false;
    }
#else
    NLM_LOG_ERROR("JSON support not available - nlohmann/json not compiled in");
    return false;
#endif
}

bool Config::saveToYamlFile(const std::string& filepath) const {
    try {
        std::ofstream file(filepath);
        if (!file.is_open()) {
            NLM_LOG_ERROR("Failed to open YAML file for writing: " + filepath);
            return false;
        }
        
        file << "# NLM Configuration (YAML format)\n";
        file << "# Generated by NLM Configuration System\n\n";
        
        for (const auto& entry : pImpl->entries) {
            file << "# " << entry.description << "\n";
            file << entry.key << ": ";
            
            std::visit([&](const auto& val) {
                using T = std::decay_t<decltype(val)>;
                
                if constexpr (std::is_same_v<T, std::string>) {
                    file << "\"" << val << "\"";
                } else if constexpr (std::is_same_v<T, int64_t> || 
                                     std::is_same_v<T, double> ||
                                     std::is_same_v<T, bool>) {
                    file << val;
                } else if constexpr (std::is_same_v<T, std::vector<int>> ||
                                     std::is_same_v<T, std::vector<double>> ||
                                     std::is_same_v<T, std::vector<std::string>>) {
                    file << "["; // Simple array representation
                    file << "]";
                }
            }, entry.value);
            
            file << "\n\n";
        }
        
        NLM_LOG_INFO("Configuration saved to YAML file: " + filepath + " (" + 
                    std::to_string(pImpl->entries.size()) + " entries)");
        return true;
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Error writing YAML file " + filepath + ": " + std::string(e.what()));
        return false;
    }
}

bool Config::saveToSimpleFile(const std::string& filepath) const {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        NLM_LOG_ERROR("Failed to open config file for writing: " + filepath);
        return false;
    }
    
    for (const auto& entry : pImpl->entries) {
        file << "# " << entry.description << "\n";
        file << entry.key << " = ";
        
        std::visit([&](const auto& val) {
            using T = std::decay_t<decltype(val)>;
            
            if constexpr (std::is_same_v<T, std::string>) {
                file << "\"" << val << "\"";
            } else if constexpr (std::is_same_v<T, int64_t> || 
                                 std::is_same_v<T, double> ||
                                 std::is_same_v<T, bool>) {
                file << val;
            } else {
                file << "[COMPLEX_VALUE]";
            }
        }, entry.value);
        
        file << "\n\n";
    }
    
    NLM_LOG_INFO("Configuration saved to simple file: " + filepath + " (" + 
                std::to_string(pImpl->entries.size()) + " entries)");
    return true;
}

template<typename T>
std::optional<T> Config::get(const std::string& key) const {
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    
    if (it == pImpl->entries.end()) {
        return std::nullopt;
    }
    
    try {
        return std::get<T>(it->value);
    } catch (const std::bad_variant_access&) {
        return std::nullopt;
    }
}

template<typename T>
T Config::getOr(const std::string& key, const T& defaultValue) const {
    auto val = get<T>(key);
    return val.has_value() ? val.value() : defaultValue;
}

void Config::set(const std::string& key, const ConfigValue& value, ConfigSource source) {
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    
    if (it != pImpl->entries.end()) {
        it->value = value;
        it->source = source;
    } else {
        pImpl->entries.emplace_back(key, value, source);
    }
}

void Config::set(const std::string& key, const std::string& value, ConfigSource source) {
    set(key, ConfigValue(value), source);
}

void Config::set(const std::string& key, int value, ConfigSource source) {
    set(key, ConfigValue(value), source);
}

void Config::set(const std::string& key, double value, ConfigSource source) {
    set(key, ConfigValue(value), source);
}

void Config::set(const std::string& key, bool value, ConfigSource source) {
    set(key, ConfigValue(value), source);
}

bool Config::has(const std::string& key) const {
    return std::any_of(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
}

void Config::remove(const std::string& key) {
    pImpl->entries.erase(
        std::remove_if(pImpl->entries.begin(), pImpl->entries.end(),
            [&key](const ConfigEntry& e) { return e.key == key; }),
        pImpl->entries.end()
    );
}

std::vector<std::string> Config::getKeys() const {
    std::vector<std::string> keys;
    keys.reserve(pImpl->entries.size());
    for (const auto& entry : pImpl->entries) {
        keys.push_back(entry.key);
    }
    return keys;
}

void Config::clear() {
    pImpl->entries.clear();
}

std::string Config::summary() const {
    std::ostringstream oss;
    oss << "Configuration (" << pImpl->entries.size() << " entries):\n";
    for (const auto& entry : pImpl->entries) {
        oss << "  " << entry.key << " = [";
        std::visit([&oss](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, std::string>) {
                oss << "\"" << arg << "\"";
            } else {
                oss << arg;
            }
        }, entry.value);
        oss << "] (" << static_cast<int>(entry.source) << ")\n";
    }
    return oss.str();
}

std::string Config::trim(const std::string& str) {
    size_t start = str.find_first_not_of(" \t\r\n");
    if (start == std::string::npos) return "";
    size_t end = str.find_last_not_of(" \t\r\n");
    return str.substr(start, end - start + 1);
}

std::string Config::toLower(const std::string& str) {
    std::string result = str;
    std::transform(result.begin(), result.end(), result.begin(), ::tolower);
    return result;
}

// Explicit template instantiations
template std::optional<int> Config::get<int>(const std::string&) const;
template std::optional<int64_t> Config::get<int64_t>(const std::string&) const;
template std::optional<double> Config::get<double>(const std::string&) const;
template std::optional<bool> Config::get<bool>(const std::string&) const;
template std::optional<std::string> Config::get<std::string>(const std::string&) const;

template int Config::getOr<int>(const std::string&, const int&) const;
template int64_t Config::getOr<int64_t>(const std::string&, const int64_t&) const;
template double Config::getOr<double>(const std::string&, const double&) const;
template bool Config::getOr<bool>(const std::string&, const bool&) const;
template std::string Config::getOr<std::string>(const std::string&, const std::string&) const;

} // namespace nlm
