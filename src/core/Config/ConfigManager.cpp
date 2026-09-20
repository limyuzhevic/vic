#include "ConfigManager.hpp"
#include "ConfigSchema.hpp"
#include "ConfigError.hpp"
#include <fstream>
#include <filesystem>
#include <algorithm>
#include <cctype>
#include <iomanip>
#include <sstream>
#include <tuple>
#include <map>

namespace nlm {

// Initialize static instances and functions
static bool isFile(const std::string& path) {
    return std::filesystem::is_regular_file(std::filesystem::path(path));
}

bool ConfigManager::loadFromFile(const std::string& filepath) {
    if (!validateKey("")) return false;
    
    std::string ext = "." + std::filesystem::path(filepath).extension().string();
    std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);
    
    if (ext == ".json") {
        loadFromJSON(filepath);
        return true;
    } else if (ext == ".yaml" || ext == ".yml") {
        loadFromYAML(filepath);
        return true;
    } else {
        throw ConfigFileError(filepath, "Unsupported file format");
    }
}

bool ConfigManager::loadFromJSON(const std::string& filepath) {
    if (!initialized) {
        initialize();
    }
    
    std::ifstream file(filepath);
    if (!file.is_open()) {
        throw ConfigFileError(filepath, "Failed to open file");
    }
    
    try {
        // Implementation would use nlohmann/json here
        // For now, fallback to old format
        std::ifstream file(filepath);
        std::string line;
        while (std::getline(file, line)) {
            line = trim(line);
            if (line.empty() || line[0] == '#') {
                continue;
            }
            
            size_t pos = line.find('=');
            if (pos != std::string::npos) {
                std::string key = trim(line.substr(0, pos));
                std::string value = trim(line.substr(pos + 1));
                
                if (value.size() >= 2 && 
                    ((value.front() == '"' && value.back() == '"') ||
                     (value.front() == '\'' && value.back() == '\''))) {
                    value = value.substr(1, value.size() - 2);
                }
                
                set(key, value, ConfigSource::File);
            }
        }
    } catch (const std::exception& e) {
        throw ConfigParseError(std::string("JSON parse error: ") + e.what());
    }
    
    return true;
}

bool ConfigManager::loadFromYAML(const std::string& filepath) {
    // Implementation would use YAML parser here
    throw ConfigFileError(filepath, "YAML parsing not yet implemented");
}

bool ConfigManager::loadFromArgs(int argc, char** argv) {
    if (!initialized) {
        initialize();
    }
    
    for (int i = 1; i < argc; ++i) {
        std::string arg(argv[i]);
        
        if (arg.substr(0, 2) == "--") {
            size_t pos = arg.find('=');
            if (pos != std::string::npos) {
                std::string key = arg.substr(2, pos - 2);
                std::string value = arg.substr(pos + 1);
                set(key, value, ConfigSource::CommandLine);
            }
        } else if (arg[0] == '-' && i + 1 < argc) {
            std::string key = arg.substr(1);
            std::string value = argv[++i];
            set(key, value, ConfigSource::CommandLine);
        }
    }
    return true;
}

bool ConfigManager::saveToFile(const std::string& filepath) const {
    std::string ext = "." + std::filesystem::path(filepath).extension().string();
    std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);
    
    if (ext == ".json") {
        return saveToJSON(filepath);
    } else if (ext == ".yaml" || ext == ".yml") {
        return saveToYAML(filepath);
    } else {
        throw ConfigFileError(filepath, "Unsupported file format");
    }
}

bool ConfigManager::saveToJSON(const std::string& filepath) const {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        throw ConfigFileError(filepath, "Failed to open file for writing");
    }
    
    file << "{\n";
    bool first = true;
    for (const auto& pair : configData) {
        if (!first) file << ",\n";
        first = false;
        
        file << "  \"" << pair.first << "\": ";
        file << configValueToString(pair.second);
    }
    file << "\n}\n";
    
    return true;
}

bool ConfigManager::saveToYAML(const std::string& filepath) const {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        throw ConfigFileError(filepath, "Failed to open file for writing");
    }
    
    file << "# Configuration\n";
    for (const auto& pair : configData) {
        file << pair.first << ": " << configValueToString(pair.second) << "\n";
    }
    
    return true;
}

template<typename T>
std::optional<T> ConfigManager::get(const std::string& key) const {
    if (!validateKey(key)) {
        return std::nullopt;
    }
    
    auto it = configData.find(key);
    if (it == configData.end()) {
        return std::nullopt;
    }
    
    try {
        return std::get<T>(it->second);
    } catch (const std::bad_variant_access&) {
        throw ConfigTypeError(key, paramTypeToString(stringToParamType(
            schema.getParameterRequired(key).typeToString())),
            paramTypeToString(stringToParamType(getParameterType(key))));
    }
}

template<typename T>
T ConfigManager::getOr(const std::string& key, const T& defaultValue) const {
    auto val = get<T>(key);
    return val.has_value() ? val.value() : defaultValue;
}

void ConfigManager::set(const std::string& key, const ConfigValue& value, ConfigSource source) {
    if (!validateKey(key)) {
        throw ConfigUnknownKeyError(key, "Key is not defined in schema");
    }
    
    if (!validateValue(key, value)) {
        throw ConfigValidationError("Value validation failed for key: " + key);
    }
    
    configData[key] = value;
    configSources[key] = source;
    schema.setParameterWasSet(key);
}

void ConfigManager::set(const std::string& key, const std::string& value, ConfigSource source) {
    set(key, ConfigValue(value), source);
}

void ConfigManager::set(const std::string& key, int value, ConfigSource source) {
    set(key, ConfigValue(value), source);
}

void ConfigManager::set(const std::string& key, double value, ConfigSource source) {
    set(key, ConfigValue(value), source);
}

void ConfigManager::set(const std::string& key, bool value, ConfigSource source) {
    set(key, ConfigValue(value), source);
}

bool ConfigManager::has(const std::string& key) const {
    return configData.find(key) != configData.end();
}

void ConfigManager::remove(const std::string& key) {
    if (validateKey(key)) {
        configData.erase(key);
        configSources.erase(key);
        schema.setParameterWasSet(key);
    }
}

std::vector<std::string> ConfigManager::getKeys() const {
    std::vector<std::string> keys;
    keys.reserve(configData.size());
    for (const auto& pair : configData) {
        keys.push_back(pair.first);
    }
    return keys;
}

void ConfigManager::clear() {
    configData.clear();
    configSources.clear();
}

std::string ConfigManager::summary() const {
    std::ostringstream oss;
    oss << "Configuration (" << configData.size() << " entries):\n";
    for (const auto& pair : configData) {
        oss << "  " << pair.first << " = [" << configValueToString(pair.second) << "] (";
        if (configSources.find(pair.first) != configSources.end()) {
            oss << static_cast<int>(configSources.at(pair.first));
        } else {
            oss << "-1";
        }
        oss << ")\n";
    }
    return oss.str();
}

bool ConfigManager::validate() const {
    return schema.validate(configData);
}

void ConfigManager::printHelp(std::ostream& os) const {
    os << "Configuration Help:" << std::endl;
    os << "===================" << std::endl;
    os << "\nAvailable parameters:" << std::endl;
    
    const auto& parameters = schema.getAllParameters();
    for (const auto& param : parameters) {
        os << "  --" << param.key << "=" << param.typeToString() << "\n";
        os << "      " << param.description << "\n";
        if (param.required) os << "      (required)\n";
        os << "      Default: ";
        std::visit([&os](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, std::string>) {
                os << "\"" << arg << "\"";
            } else {
                os << arg;
            }
        }, param.defaultValue);
        os << "\n\n";
    }
}

std::string ConfigManager::trim(const std::string& str) {
    size_t start = str.find_first_not_of(" \t\r\n");
    if (start == std::string::npos) return "";
    size_t end = str.find_last_not_of(" \t\r\n");
    return str.substr(start, end - start + 1);
}

ConfigValue ConfigManager::stringToConfigValue(const std::string& str, ParamType type) {
    // Implementation would handle type conversion here
    // For now, return as string
    return str;
}

std::string ConfigManager::configValueToString(const ConfigValue& value) const {
    std::ostringstream oss;
    std::visit([&oss](auto&& arg) {
        using T = std::decay_t<decltype(arg)>;
        if constexpr (std::is_same_v<T, std::string>) {
            oss << "\"" << arg << "\"";
        } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
            oss << "["; // Opening bracket for vector
            bool first = true;
            for (const auto& item : arg) {
                if (!first) oss << ", ";
                oss << "\"" << item << "\"";
                first = false;
            }
            oss << "]"; // Closing bracket
        } else if constexpr (std::is_same_v<T, std::vector<int>>) {
            oss << "[";
            bool first = true;
            for (const auto& item : arg) {
                if (!first) oss << ", ";
                oss << item;
                first = false;
            }
            oss << "]";
        } else if constexpr (std::is_same_v<T, std::vector<double>>) {
            oss << "[";
            bool first = true;
            for (const auto& item : arg) {
                if (!first) oss << ", ";
                oss << item;
                first = false;
            }
            oss << "]";
        } else {
            oss << arg;
        }
    }, value);
    return oss.str();
}

std::string ConfigManager::paramTypeToString(ParamType type) {
    switch (type) {
        case ParamType::Int: return "int";
        case ParamType::Int64: return "int64_t";
        case ParamType::Double: return "double";
        case ParamType::Bool: return "bool";
        case ParamType::String: return "string";
        case ParamType::StringVector: return "string_vector";
        case ParamType::IntVector: return "int_vector";
        case ParamType::DoubleVector: return "double_vector";
        default: return "unknown";
    }
}

ParamType ConfigManager::stringToParamType(const std::string& typeStr) {
    if (typeStr == "int") return ParamType::Int;
    if (typeStr == "int64_t") return ParamType::Int64;
    if (typeStr == "double") return ParamType::Double;
    if (typeStr == "bool") return ParamType::Bool;
    if (typeStr == "string") return ParamType::String;
    if (typeStr == "string_vector") return ParamType::StringVector;
    if (typeStr == "int_vector") return ParamType::IntVector;
    if (typeStr == "double_vector") return ParamType::DoubleVector;
    return ParamType::String;
}

bool ConfigManager::validateKey(const std::string& key) const {
    return key.empty() || schema.hasParameter(key);
}

bool ConfigManager::validateValue(const std::string& key, const ConfigValue& value) const {
    const ParameterInfo* param = schema.getParameter(key);
    if (!param) return false;
    
    return param->validate(value);
}

ParamType ConfigManager::getParameterType(const std::string& key) const {
    const ParameterInfo* param = schema.getParameter(key);
    if (!param) return ParamType::String;
    return param->type;
}

} // namespace nlm