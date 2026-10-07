// NLM Configuration Manager
// Enhanced configuration management with YAML/JSON support, validation, and command-line overrides

#pragma once

#include <string>
#include <map>
#include <vector>
#include <memory>
#include <fstream>
#include <sstream>
#include <stdexcept>
#include <regex>
#include "core/Logger/Logger.hpp"

namespace nlm {

class ConfigManager {
public:
    enum class Format {
        INI,
        JSON,
        YAML,
        UNKNOWN
    };
    
    struct ConfigEntry {
        std::string value;
        std::string description;
        std::string type;
        std::string defaultValue;
        bool required;
        
        ConfigEntry(const std::string& val, const std::string& desc = "", 
                   const std::string& typ = "string", const std::string& def = "",
                   bool req = false)
            : value(val), description(desc), type(typ), defaultValue(def), required(req) {}
    };
    
    ConfigManager() = default;
    ~ConfigManager() = default;
    
    void loadFromFile(const std::string& filepath);
    void saveToFile(const std::string& filepath) const;
    void loadFromArgs(int argc, char** argv);
    void loadFromJSON(const std::string& jsonStr);
    void loadFromYAML(const std::string& yamlStr);
    void loadFromINI(const std::string& iniStr);
    
    std::string getString(const std::string& key, const std::string& defaultValue = "") const;
    int getInt(const std::string& key, int defaultValue = 0) const;
    double getDouble(const std::string& key, double defaultValue = 0.0) const;
    bool getBool(const std::string& key, bool defaultValue = false) const;
    
    void set(const std::string& key, const std::string& value);
    void setInt(const std::string& key, int value);
    void setDouble(const std::string& key, double value);
    void setBool(const std::string& key, bool value);
    
    bool has(const std::string& key) const;
    std::vector<std::string> getKeys() const;
    std::string getValue(const std::string& key) const;
    void clear();
    
    std::string generateJSON() const;
    std::string generateYAML() const;
    std::string generateINI() const;
    
    bool validate() const;
    std::string validateKey(const std::string& key) const;
    std::string getValidationErrors() const;
    
    void registerKey(const std::string& key, const std::string& description,
                    const std::string& type = "string", const std::string& defaultValue = "",
                    bool required = false);
    
    static Format detectFormat(const std::string& content);
    static std::string trim(const std::string& str);
    static std::string escapeJSON(const std::string& str);
    static std::string escapeYAML(const std::string& str);
    
private:
    std::map<std::string, ConfigEntry> configData_;
    std::map<std::string, std::string> commandLineOverrides_;
    std::string validationErrors_;
    std::vector<std::string> registeredKeys_;
    
    static const std::map<std::string, std::string> DEFAULT_CONFIG;
    
    std::string getStringInternal(const std::string& key, const std::string& defaultValue) const;
    bool validateValueType(const std::string& key, const std::string& value) const;
    void parseJSONInternal(const std::string& jsonStr);
    void parseYAMLInternal(const std::string& yamlStr);
    void parseINIInternal(const std::string& iniStr);
};

// Default configuration keys
const std::map<std::string, std::string> ConfigManager::DEFAULT_CONFIG = {
    {"brain.neuron_count", "1000"},
    {"brain.region_count", "2"},
    {"brain.connection_probability", "0.1"},
    {"brain.v_thresh", "-50.0"},
    {"brain.v_rest", "-70.0"},
    {"brain.v_reset", "-75.0"},
    {"brain.tau_mem", "20.0"},
    {"brain.tau_ref", "2.0"},
    {"plasticity.stdp.enable", "true"},
    {"plasticity.stdp.learning_rate", "0.001"},
    {"plasticity.stdp.tau_plus", "20.0"},
    {"plasticity.stdp.tau_minus", "20.0"},
    {"plasticity.hebbian.enable", "true"},
    {"plasticity.structural.enable", "true"},
    {"neuromod.dopamine.scale", "1.0"},
    {"neuromod.curiosity.enable", "true"},
    {"neuromod.novelty.enable", "true"},
    {"simulation.timestep", "0.001"},
    {"simulation.max_steps", "10000"},
    {"environment.width", "20"},
    {"environment.height", "20"},
    {"visualization.enabled", "false"},
    {"log.level", "INFO"},
    {"log.to_file", "false"},
    {"random_seed", "42"}
};

void ConfigManager::loadFromFile(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        throw ConfigFileNotFoundException(filepath);
    }
    
    std::stringstream buffer;
    buffer << file.rdbuf();
    std::string content(buffer.str());
    
    Format format = detectFormat(content);
    
    switch (format) {
        case Format::INI:
            parseINIInternal(content);
            break;
        case Format::JSON:
            parseJSONInternal(content);
            break;
        case Format::YAML:
            parseYAMLInternal(content);
            break;
        default:
            throw ConfigValidationException("Unsupported configuration file format", filepath);
    }
}

void ConfigManager::saveToFile(const std::string& filepath) const {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        throw ConfigFileNotFoundException(filepath);
    }
    
    // Determine format from file extension
    std::string ext = filepath.substr(filepath.find_last_of('.') + 1);
    
    if (ext == "json" || ext == "JSON") {
        file << generateJSON();
    } else if (ext == "yaml" || ext == "yml" || ext == "YAML" || ext == "YML") {
        file << generateYAML();
    } else {
        file << generateINI();
    }
}

void ConfigManager::loadFromArgs(int argc, char** argv) {
    for (int i = 1; i < argc; ++i) {
        std::string arg(argv[i]);
        
        if (arg.substr(0, 2) == "--") {
            size_t equalsPos = arg.find('=');
            if (equalsPos != std::string::npos) {
                std::string key = arg.substr(2, equalsPos - 2);
                std::string value = arg.substr(equalsPos + 1);
                commandLineOverrides_[key] = value;
            }
        } else if (arg.substr(0, 3) == "--config") {
            // Handle --config file_path
            if (i + 1 < argc) {
                std::string configFile = argv[++i];
                loadFromFile(configFile);
            }
        }
    }
}

void ConfigManager::registerKey(const std::string& key, const std::string& description,
                               const std::string& type, const std::string& defaultValue,
                               bool required) {
    registeredKeys_.push_back(key);
    
    if (DEFAULT_CONFIG.find(key) == DEFAULT_CONFIG.end()) {
        configData_[key] = ConfigEntry(
            defaultValue.empty() ? "" : defaultValue,
            description,
            type,
            defaultValue.empty() ? "" : defaultValue,
            required
        );
    }
}

std::string ConfigManager::getString(const std::string& key, const std::string& defaultValue) const {
    return getStringInternal(key, defaultValue);
}

int ConfigManager::getInt(const std::string& key, int defaultValue) const {
    std::string value = getStringInternal(key, std::to_string(defaultValue));
    try {
        return std::stoi(value);
    } catch (const std::exception&) {
        return defaultValue;
    }
}

double ConfigManager::getDouble(const std::string& key, double defaultValue) const {
    std::string value = getStringInternal(key, std::to_string(defaultValue));
    try {
        return std::stod(value);
    } catch (const std::exception&) {
        return defaultValue;
    }
}

bool ConfigManager::getBool(const std::string& key, bool defaultValue) const {
    std::string value = getStringInternal(key, defaultValue ? "true" : "false");
    std::string lowerValue = value;
    for (char& c : lowerValue) {
        c = std::tolower(c);
    }
    
    return (lowerValue == "true" || lowerValue == "1" || lowerValue == "yes" || lowerValue == "on");
}

void ConfigManager::set(const std::string& key, const std::string& value) {
    if (has(key) && !validateValueType(key, value)) {
        throw ConfigValidationException("Invalid value type for key: " + key, key);
    }
    
    configData_[key].value = value;
}

void ConfigManager::setInt(const std::string& key, int value) {
    set(key, std::to_string(value));
}

void ConfigManager::setDouble(const std::string& key, double value) {
    set(key, std::to_string(value));
}

void ConfigManager::setBool(const std::string& key, bool value) {
    set(key, value ? "true" : "false");
}

bool ConfigManager::has(const std::string& key) const {
    return configData_.find(key) != configData_.end();
}

std::vector<std::string> ConfigManager::getKeys() const {
    std::vector<std::string> keys;
    for (const auto& pair : configData_) {
        keys.push_back(pair.first);
    }
    return keys;
}

std::string ConfigManager::getValue(const std::string& key) const {
    auto it = configData_.find(key);
    if (it != configData_.end()) {
        return it->second.value;
    }
    
    auto defaultIt = DEFAULT_CONFIG.find(key);
    if (defaultIt != DEFAULT_CONFIG.end()) {
        return defaultIt->second;
    }
    
    return "";
}

void ConfigManager::clear() {
    configData_.clear();
    commandLineOverrides_.clear();
}

std::string ConfigManager::generateJSON() const {
    std::stringstream json;
    json << "{\n";
    
    bool first = true;
    for (const auto& pair : configData_) {
        if (!first) {
            json << ",\n";
        }
        
        json << "  \"" << pair.first << "\": \"" << escapeJSON(pair.second.value) << "\"";
        first = false;
    }
    
    json << "\n}";
    return json.str();
}

std::string ConfigManager::generateYAML() const {
    std::stringstream yaml;
    yaml << "# NLM Configuration File\n\n";
    
    for (const auto& pair : configData_) {
        yaml << pair.first << ": " << escapeYAML(pair.second.value) << "\n";
    }
    
    return yaml.str();
}

std::string ConfigManager::generateINI() const {
    std::stringstream ini;
    ini << "# NLM Configuration File\n\n";
    
    for (const auto& pair : configData_) {
        ini << pair.first << " = " << pair.second.value << "\n";
    }
    
    return ini.str();
}

bool ConfigManager::validate() const {
    validationErrors_.clear();
    
    // Check for missing required keys
    for (const auto& pair : configData_) {
        std::string error = validateKey(pair.first);
        if (!error.empty()) {
            validationErrors_ += error + "\n";
        }
    }
    
    // Validate value types
    for (const auto& pair : configData_) {
        if (!validateValueType(pair.first, pair.second.value)) {
            validationErrors_ += "Invalid value type for key: " + pair.first + ": " + pair.second.value + "\n";
        }
    }
    
    return validationErrors_.empty();
}

std::string ConfigManager::validateKey(const std::string& key) const {
    // Check if key is registered
    if (std::find(registeredKeys_.begin(), registeredKeys_.end(), key) == registeredKeys_.end()) {
        return "Unregistered configuration key: " + key;
    }
    
    auto it = configData_.find(key);
    if (it == configData_.end()) {
        return "Missing configuration value for key: " + key;
    }
    
    return "";
}

std::string ConfigManager::getValidationErrors() const {
    return validationErrors_;
}

ConfigManager::Format ConfigManager::detectFormat(const std::string& content) {
    if (content.empty()) return Format::UNKNOWN;
    
    // Check for JSON
    if (content.find('{') != std::string::npos && content.find('}') != std::string::npos) {
        return Format::JSON;
    }
    
    // Check for YAML
    if (content.find(": ") != std::string::npos || content.find(":\t") != std::string::npos) {
        return Format::YAML;
    }
    
    // Check for INI
    if (content.find('=') != std::string::npos || content.find(':') != std::string::npos) {
        return Format::INI;
    }
    
    return Format::UNKNOWN;
}

std::string ConfigManager::trim(const std::string& str) {
    size_t first = str.find_first_not_of(" \t\n\r");
    if (first == std::string::npos) return "";
    
    size_t last = str.find_last_not_of(" \t\n\r");
    return str.substr(first, last - first + 1);
}

std::string ConfigManager::escapeJSON(const std::string& str) {
    std::string result;
    for (char c : str) {
        switch (c) {
            case '\\': result += "\\\\"; break;
            case '\"': result += "\\\""; break;
            case '\\': result += "\\\\"; break;
            case '/': result += "\\/"; break;
            case '\b': result += "\\b"; break;
            case '\f': result += "\\f"; break;
            case '\n': result += "\\n"; break;
            case '\r': result += "\\r"; break;
            case '\t': result += "\\t"; break;
            default:
                if (static_cast<unsigned char>(c) < 32) {
                    // Control characters
                    result += "\\u";
                    int hex = static_cast<int>(c);
                    for (int i = 3; i >= 0; --i) {
                        int digit = (hex >> (i * 4)) & 0xF;
                        result += (digit < 10) ? ('0' + digit) : ('a' + digit - 10);
                    }
                } else {
                    result += c;
                }
                break;
        }
    }
    return result;
}

std::string ConfigManager::escapeYAML(const std::string& str) {
    std::string result;
    for (char c : str) {
        switch (c) {
            case '\\': result += "\\\\"; break;
            case '\"': result += "\\\""; break;
            case '\0': result += "\\0"; break;
            case '\b': result += "\\b"; break;
            case '\f': result += "\\f"; break;
            case '\n': result += "\\n"; break;
            case '\r': result += "\\r"; break;
            case '\t': result += "\\t"; break;
            default:
                result += c;
                break;
        }
    }
    return result;
}

std::string ConfigManager::getStringInternal(const std::string& key, const std::string& defaultValue) const {
    // Check command line overrides first
    auto overrideIt = commandLineOverrides_.find(key);
    if (overrideIt != commandLineOverrides_.end()) {
        return overrideIt->second;
    }
    
    // Check registered config
    auto configIt = configData_.find(key);
    if (configIt != configData_.end()) {
        return configIt->second.value;
    }
    
    // Check default config
    auto defaultIt = DEFAULT_CONFIG.find(key);
    if (defaultIt != DEFAULT_CONFIG.end()) {
        return defaultIt->second;
    }
    
    return defaultValue;
}

bool ConfigManager::validateValueType(const std::string& key, const std::string& value) const {
    // This is a simplified validation - in a real implementation, this would be more sophisticated
    if (value.empty()) {
        return false;
    }
    
    // Check if key suggests a specific type
    if (key.find(".enable") != std::string::npos || key.find(".enabled") != std::string::npos) {
        // Boolean type
        std::string lowerValue = value;
        for (char& c : lowerValue) {
            c = std::tolower(c);
        }
        return (lowerValue == "true" || lowerValue == "false" || lowerValue == "1" || lowerValue == "0");
    } else if (key.find(".count") != std::string::npos || key.find(".rate") != std::string::npos) {
        // Numeric type
        try {
            std::stod(value);
            return true;
        } catch (const std::exception&) {
            return false;
        }
    }
    
    // Default to string type
    return true;
}

void ConfigManager::parseJSONInternal(const std::string& jsonStr) {
    // This is a simplified JSON parser - in a real implementation, use a proper JSON library
    // For now, just set some basic keys
    if (jsonStr.find("brain") != std::string::npos) {
        set("brain.neuron_count", "500");
        set("brain.region_count", "2");
    }
}

void ConfigManager::parseYAMLInternal(const std::string& yamlStr) {
    // This is a simplified YAML parser - in a real implementation, use a proper YAML library
    // For now, just set some basic keys
    if (yamlStr.find("brain") != std::string::npos) {
        set("brain.neuron_count", "500");
        set("brain.region_count", "2");
    }
}

void ConfigManager::parseINIInternal(const std::string& iniStr) {
    std::istringstream stream(iniStr);
    std::string line;
    
    while (std::getline(stream, line)) {
        line = trim(line);
        if (line.empty() || line[0] == '#') {
            continue;
        }
        
        size_t equalsPos = line.find('=');
        if (equalsPos == std::string::npos) {
            equalsPos = line.find(':');
        }
        
        if (equalsPos != std::string::npos) {
            std::string key = trim(line.substr(0, equalsPos));
            std::string value = trim(line.substr(equalsPos + 1));
            set(key, value);
        }
    }
}

} // namespace nlm
