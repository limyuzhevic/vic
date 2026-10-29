// ConfigFormatManager.cpp - Implementation of multi-format configuration manager

#include "ConfigFormatManager.hpp"
#include <fstream>
#include <sstream>
#include <iostream>
#include <algorithm>
#include <filesystem>
#include <nlohmann/json.hpp>
#include <yaml-cpp/yaml.h>

// Use nlohmann::json for JSON parsing
using json = nlohmann::json;

namespace nlm {

struct ConfigFormatManager::Impl {
    std::vector<Config> configHistory;
    std::string lastError;
    bool cachingEnabled = true;
    mutable std::mutex mutex;
};

ConfigFormatManager::ConfigFormatManager() : pImpl(std::make_unique<Impl>()) {}

ConfigFormatManager::~ConfigFormatManager() = default;

ConfigFormatManager& ConfigFormatManager::getInstance() {
    static ConfigFormatManager instance;
    return instance;
}

bool ConfigFormatManager::loadConfig(Config& config, const std::string& filepath, Format format) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    std::ifstream file(filepath);
    if (!file.is_open()) {
        pImpl->lastError = "Cannot open file: " + filepath;
        return false;
    }
    
    std::stringstream buffer;
    buffer << file.rdbuf();
    std::string content = buffer.str();
    
    return loadConfig(config, content, format);
}

bool ConfigFormatManager::loadConfig(Config& config, const std::string& content, Format format) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    config.clear(); // Clear existing config
    
    switch (format) {
        case Format::INI:
            return loadFromINI(config, content);
        case Format::JSON:
            return loadFromJSON(config, content);
        case Format::YAML:
            return loadFromYAML(config, content);
        default:
            pImpl->lastError = "Unsupported format: " + std::to_string(static_cast<int>(format));
            return false;
    }
}

bool ConfigFormatManager::loadFromINI(Config& config, const std::string& content) {
    std::istringstream stream(content);
    std::string line;
    int lineNumber = 0;
    
    while (std::getline(stream, line)) {
        lineNumber++;
        
        // Skip empty lines and comments
        line.erase(0, line.find_first_not_of(" \t\r\n"));
        line.erase(line.find_last_not_of(" \t\r\n") + 1);
        
        if (line.empty() || line[0] == '#' || line[0] == ';') {
            continue;
        }
        
        // Parse key=value pairs
        size_t pos = line.find('=');
        if (pos == std::string::npos) {
            continue; // Invalid line
        }
        
        std::string key = line.substr(0, pos);
        std::string value = line.substr(pos + 1);
        
        // Trim whitespace
        key.erase(0, key.find_first_not_of(" \t"));
        key.erase(key.find_last_not_of(" \t") + 1);
        
        value.erase(0, value.find_first_not_of(" \t"));
        value.erase(value.find_last_not_of(" \t") + 1);
        
        // Remove quotes
        if (value.size() >= 2 && ((value.front() == '"' && value.back() == '"') ||
                                 (value.front() == '\'' && value.back() == '\''))) {
            value = value.substr(1, value.size() - 2);
        }
        
        config.set(key, value, ConfigSource::File);
    }
    
    return true;
}

bool ConfigFormatManager::loadFromJSON(Config& config, const std::string& content) {
    try {
        json jsonData = json::parse(content);
        
        // Process JSON object recursively
        processJSONObject(config, jsonData);
        
        return true;
    } catch (const json::exception& e) {
        pImpl->lastError = "JSON parsing error: " + std::string(e.what());
        return false;
    }
}

bool ConfigFormatManager::loadFromJSON(Config& config, const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        pImpl->lastError = "Cannot open JSON file: " + filepath;
        return false;
    }
    
    std::string content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
    
    return loadFromJSON(config, content);
}

bool ConfigFormatManager::loadFromYAML(Config& config, const std::string& content) {
    try {
        YAML::Node yamlData = YAML::Load(content);
        
        // Process YAML node recursively
        processYAMLNode(config, yamlData);
        
        return true;
    } catch (const YAML::Exception& e) {
        pImpl->lastError = "YAML parsing error: " + std::string(e.what());
        return false;
    }
}

bool ConfigFormatManager::loadFromYAML(Config& config, const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        pImpl->lastError = "Cannot open YAML file: " + filepath;
        return false;
    }
    
    std::string content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
    
    return loadFromYAML(config, content);
}

void ConfigFormatManager::processJSONObject(Config& config, const json& jsonData, const std::string& prefix = "") {
    for (auto it = jsonData.begin(); it != jsonData.end(); ++it) {
        std::string key = it.key();
        std::string fullKey = prefix.empty() ? key : prefix + "." + key;
        
        if (it->is_object()) {
            // Recursively process nested object
            processJSONObject(config, *it, fullKey);
        } else if (it->is_array()) {
            // Convert array to string representation
            config.set(fullKey, it->dump(), ConfigSource::File);
        } else {
            // Process scalar value
            std::string value;
            
            if (it->is_string()) {
                value = "\"" + it->get<std::string>() + "\"";
            } else if (it->is_boolean()) {
                value = it->get<bool>() ? "true" : "false";
            } else if (it->is_number_integer()) {
                value = std::to_string(it->get<int64_t>());
            } else if (it->is_number_float()) {
                value = std::to_string(it->get<double>());
            } else {
                value = it->dump();
            }
            
            config.set(fullKey, value, ConfigSource::File);
        }
    }
}

void ConfigFormatManager::processYAMLNode(Config& config, const YAML::Node& yamlData, const std::string& prefix = "") {
    if (yamlData.IsMap()) {
        for (auto it = yamlData.begin(); it != yamlData.end(); ++it) {
            std::string key = it->first.as<std::string>();
            std::string fullKey = prefix.empty() ? key : prefix + "." + key;
            
            if (it->second.IsMap()) {
                // Recursively process nested map
                processYAMLNode(config, it->second, fullKey);
            } else if (it->second.IsSequence()) {
                // Convert sequence to string representation
                config.set(fullKey, YAML::Dump(it->second), ConfigSource::File);
            } else {
                // Process scalar value
                std::string value;
                
                if (it->second.IsScalar()) {
                    value = it->second.as<std::string>();
                } else {
                    value = YAML::Dump(it->second);
                }
                
                config.set(fullKey, value, ConfigSource::File);
            }
        }
    }
}

bool ConfigFormatManager::exportConfig(const Config& config, const std::string& filepath, Format format) {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        pImpl->lastError = "Cannot open file for writing: " + filepath;
        return false;
    }
    
    std::string content = exportConfig(config, format);
    
    if (!content.empty()) {
        file << content;
        return true;
    }
    
    return false;
}

std::string ConfigFormatManager::exportConfig(const Config& config, Format format) {
    std::ostringstream stream;
    
    switch (format) {
        case Format::INI:
            exportToINI(config, stream);
            break;
        case Format::JSON:
            exportToJSON(config, stream);
            break;
        case Format::YAML:
            exportToYAML(config, stream);
            break;
        default:
            pImpl->lastError = "Unsupported export format: " + std::to_string(static_cast<int>(format));
            return "";
    }
    
    return stream.str();
}

void ConfigFormatManager::exportToINI(const Config& config, std::ostream& stream) {
    auto keys = config.getKeys();
    for (const auto& key : keys) {
        stream << "# " << key << "\n";
        
        auto val = config.get<std::string>(key);
        if (val.has_value()) {
            stream << key << " = " << val.value() << "\n";
        } else {
            stream << key << " = <unknown>\n";
        }
        
        stream << "\n";
    }
}

void ConfigFormatManager::exportToJSON(const Config& config, std::ostream& stream) {
    json jsonData;
    
    auto keys = config.getKeys();
    for (const auto& key : keys) {
        auto val = config.get<std::string>(key);
        if (val.has_value()) {
            // Try to convert to appropriate JSON type
            std::string strVal = val.value();
            
            // Check if it's a boolean
            if (strVal == "true" || strVal == "false") {
                jsonData[key] = (strVal == "true");
            }
            // Check if it's a number
            else {
                try {
                    size_t pos;
                    double numVal = std::stod(strVal, &pos);
                    if (pos == strVal.size()) {
                        jsonData[key] = numVal;
                        continue;
                    }
                } catch (...) {
                    // Not a number, keep as string
                }
                
                // Remove quotes if present
                if (strVal.size() >= 2 && 
                    ((strVal.front() == '"' && strVal.back() == '"') ||
                     (strVal.front() == '\'' && strVal.back() == '\''))) {
                    strVal = strVal.substr(1, strVal.size() - 2);
                }
                
                jsonData[key] = strVal;
            }
        }
    }
    
    stream << jsonData.dump(4);
}

void ConfigFormatManager::exportToYAML(const Config& config, std::ostream& stream) {
    YAML::Emitter emitter;
    
    emitter << YAML::BeginMap;
    
    auto keys = config.getKeys();
    for (const auto& key : keys) {
        emitter << YAML::Key << key;
        
        auto val = config.get<std::string>(key);
        if (val.has_value()) {
            std::string strVal = val.value();
            
            // Try to convert to appropriate YAML type
            if (strVal == "true" || strVal == "false") {
                emitter << (strVal == "true");
            } else {
                try {
                    size_t pos;
                    double numVal = std::stod(strVal, &pos);
                    if (pos == strVal.size()) {
                        emitter << numVal;
                        continue;
                    }
                } catch (...) {
                    // Not a number, keep as string
                }
                
                // Remove quotes if present
                if (strVal.size() >= 2 && 
                    ((strVal.front() == '"' && strVal.back() == '"') ||
                     (strVal.front() == '\'' && strVal.back() == '\''))) {
                    strVal = strVal.substr(1, strVal.size() - 2);
                }
                
                emitter << strVal;
            }
        }
    }
    
    emitter << YAML::EndMap;
    
    stream << emitter.c_str();
}

ConfigFormatManager::Format ConfigFormatManager::detectFormat(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return Format::INI; // Default to INI if file cannot be opened
    }
    
    std::string content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
    file.close();
    
    return detectFormatFromContent(content);
}

ConfigFormatManager::Format ConfigFormatManager::detectFormatFromContent(const std::string& content) {
    if (content.empty()) {
        return Format::INI;
    }
    
    // Check for JSON format
    if (content.find('{') != std::string::npos && content.find('}') != std::string::npos) {
        // Simple JSON detection
        try {
            json::parse(content);
            return Format::JSON;
        } catch (...) {
            // Not valid JSON
        }
    }
    
    // Check for YAML format
    if (content.find("key:") != std::string::npos && content.find("  ") != std::string::npos) {
        return Format::YAML;
    }
    
    // Check for INI format (key=value pattern)
    std::istringstream stream(content);
    std::string line;
    while (std::getline(stream, line)) {
        line.erase(0, line.find_first_not_of(" \t\r\n"));
        line.erase(line.find_last_not_of(" \t\r\n") + 1);
        
        if (!line.empty() && line.find('=') != std::string::npos) {
            return Format::INI;
        }
    }
    
    return Format::INI; // Default to INI
}

bool ConfigFormatManager::validateConfig(const Config& config, const std::map<std::string, std::pair<double, double>>& ranges) {
    auto keys = config.getKeys();
    
    for (const auto& key : keys) {
        auto val = config.get<std::string>(key);
        if (!val.has_value()) {
            continue;
        }
        
        auto it = ranges.find(key);
        if (it != ranges.end()) {
            if (!validateParameter(key, ConfigValue(val.value()), it->second)) {
                return false;
            }
        }
    }
    
    return true;
}

bool ConfigFormatManager::validateParameter(const std::string& key, const ConfigValue& value, 
                                            const std::pair<double, double>& range) {
    // Convert value to double for comparison
    double val = 0.0;
    
    try {
        if (std::holds_alternative<std::string>(value)) {
            val = std::stod(std::get<std::string>(value));
        } else if (std::holds_alternative<int64_t>(value)) {
            val = static_cast<double>(std::get<int64_t>(value));
        } else if (std::holds_alternative<double>(value)) {
            val = std::get<double>(value);
        } else if (std::holds_alternative<bool>(value)) {
            val = std::get<bool>() ? 1.0 : 0.0;
        } else {
            return false;
        }
    } catch (...) {
        return false;
    }
    
    return (val >= range.first && val <= range.second);
}

bool ConfigFormatManager::loadWithInheritance(Config& config, const std::string& baseFile, 
                                           const std::vector<std::string>& overrideFiles) {
    // Load base configuration
    if (!loadConfig(config, baseFile)) {
        return false;
    }
    
    // Apply overrides
    for (const auto& overrideFile : overrideFiles) {
        Format format = detectFormat(overrideFile);
        Config tempConfig;
        if (loadConfig(tempConfig, overrideFile, format)) {
            // Merge with override
            auto keys = tempConfig.getKeys();
            for (const auto& key : keys) {
                auto val = tempConfig.get<std::string>(key);
                if (val.has_value()) {
                    config.set(key, val.value(), ConfigSource::File);
                }
            }
        }
    }
    
    return true;
}

std::vector<std::string> ConfigFormatManager::diffConfigs(const Config& config1, const Config& config2) {
    std::vector<std::string> differences;
    
    auto keys1 = config1.getKeys();
    auto keys2 = config2.getKeys();
    
    // Check keys in config1 but not in config2
    for (const auto& key : keys1) {
        if (std::find(keys2.begin(), keys2.end(), key) == keys2.end()) {
            differences.push_back("Missing in config2: " + key);
        }
    }
    
    // Check keys in both but different values
    for (const auto& key : keys2) {
        auto val1 = config1.get<std::string>(key);
        auto val2 = config2.get<std::string>(key);
        
        if (val1.has_value() && val2.has_value()) {
            if (val1.value() != val2.value()) {
                differences.push_back("Different values for " + key + ": '" + val1.value() + "' vs '" + val2.value() + "'");
            }
        } else if (val1.has_value() != val2.has_value()) {
            differences.push_back("Missing value in config1 for key: " + key);
        }
    }
    
    return differences;
}

std::map<std::string, std::pair<double, double>> ConfigFormatManager::getDefaultParameterRanges() {
    return {
        {"random_seed", {0, 2147483647}},
        {"simulation_timestep", {0.0001, 1.0}},
        {"neuron_count", {1, 1000000}},
        {"region_count", {1, 100}},
        {"connection_probability", {0.0, 1.0}},
        {"plasticity_learning_rate", {0.0, 1.0}},
        {"stdp_ltp_weight", {0.0, 1.0}},
        {"stdp_ltd_weight", {0.0, 1.0}},
        {"stdp_time_constant", {1.0, 100.0}},
        {"dopamine_baseline", {0.0, 1.0}},
        {"reward_discount_factor", {0.0, 1.0}},
        {"environment_width", {1, 1000}},
        {"environment_height", {1, 1000}}
    };
}

bool ConfigFormatManager::saveConfigTemplate(const std::string& filepath, Format format) {
    Config config;
    
    // Set up a comprehensive template configuration
    config.set("random_seed", static_cast<int64_t>(42), ConfigSource::Default);
    config.set("simulation_timestep", 0.001, ConfigSource::Default);
    config.set("neuron_count", static_cast<int64_t>(1000), ConfigSource::Default);
    config.set("region_count", static_cast<int64_t>(2), ConfigSource::Default);
    config.set("connection_probability", 0.15f, ConfigSource::Default);
    config.set("plasticity_learning_rate", 0.01f, ConfigSource::Default);
    config.set("stdp_ltp_weight", 0.01f, ConfigSource::Default);
    config.set("stdp_ltd_weight", 0.012f, ConfigSource::Default);
    config.set("stdp_time_constant", 20.0f, ConfigSource::Default);
    config.set("dopamine_baseline", 0.1f, ConfigSource::Default);
    config.set("reward_discount_factor", 0.99f, ConfigSource::Default);
    config.set("environment_width", 10, ConfigSource::Default);
    config.set("environment_height", 10, ConfigSource::Default);
    config.set("log_level", "INFO", ConfigSource::Default);
    config.set("log_to_file", false, ConfigSource::Default);
    config.set("visualization_enabled", false, ConfigSource::Default);
    
    return exportConfig(config, filepath, format);
}

std::vector<std::string> ConfigFormatManager::getAllKeys(const Config& config) {
    return config.getKeys();
}

bool ConfigFormatManager::mergeConfigs(Config& config, const Config& other, bool overwrite) {
    auto keys = other.getKeys();
    
    for (const auto& key : keys) {
        auto val = other.get<std::string>(key);
        if (val.has_value()) {
            config.set(key, val.value(), ConfigSource::File);
        }
    }
    
    return true;
}

void ConfigFormatManager::addConfigToHistory(const Config& config) {
    pImpl->configHistory.push_back(config);
}

std::vector<Config> ConfigFormatManager::getConfigHistory() {
    return pImpl->configHistory;
}

void ConfigFormatManager::clearConfigHistory() {
    pImpl->configHistory.clear();
}

std::string ConfigFormatManager::formatValue(const ConfigValue& value, Format format) {
    std::ostringstream stream;
    
    if (std::holds_alternative<std::string>(value)) {
        stream << std::get<std::string>(value);
    } else if (std::holds_alternative<int64_t>(value)) {
        stream << std::get<int64_t>(value);
    } else if (std::holds_alternative<double>(value)) {
        stream << std::get<double>(value);
    } else if (std::holds_alternative<bool>(value)) {
        stream << (std::get<bool>(value) ? "true" : "false");
    }
    
    return stream.str();
}

std::optional<ConfigValue> ConfigFormatManager::parseValue(const std::string& str, Format format) {
    // Simple parsing - in practice would need more sophisticated parsing
    try {
        if (str == "true" || str == "false") {
            return (str == "true");
        }
        
        size_t pos;
        double numVal = std::stod(str, &pos);
        if (pos == str.size()) {
            // Check if it's an integer
            try {
                int64_t intVal = std::stoll(str);
                return intVal;
            } catch (...) {
                return numVal;
            }
        }
        
        return str;
    } catch (...) {
        return std::nullopt;
    }
}

void ConfigFormatManager::enableCaching(bool enable) {
    pImpl->cachingEnabled = enable;
}

bool ConfigFormatManager::isCachingEnabled() const {
    return pImpl->cachingEnabled;
}

std::string ConfigFormatManager::getLastError() const {
    return pImpl->lastError;
}

void ConfigFormatManager::clearError() {
    pImpl->lastError.clear();
}

std::vector<ConfigFormatManager::ParameterInfo> ConfigFormatManager::getParameterSchema() {
    return {
        {"random_seed", "int64", "Random seed for reproducible experiments", {0, 2147483647}, 42, false},
        {"simulation_timestep", "double", "Simulation timestep in seconds", {0.0001, 1.0}, 0.001, false},
        {"neuron_count", "int64", "Number of neurons in the brain", {1, 1000000}, static_cast<int64_t>(1000), false},
        {"region_count", "int64", "Number of brain regions", {1, 100}, static_cast<int64_t>(2), false},
        {"connection_probability", "double", "Connection probability for random connectivity", {0.0, 1.0}, 0.1, false},
        {"plasticity_learning_rate", "double", "Plasticity learning rate", {0.0, 1.0}, 0.01, false},
        {"stdp_ltp_weight", "double", "STDP LTP weight", {0.0, 1.0}, 0.01, false},
        {"stdp_ltd_weight", "double", "STDP LTD weight", {0.0, 1.0}, 0.012, false},
        {"stdp_time_constant", "double", "STDP time constant (ms)", {1.0, 100.0}, 20.0, false},
        {"dopamine_baseline", "double", "Dopamine baseline value", {0.0, 1.0}, 0.1, false},
        {"reward_discount_factor", "double", "Reward discount factor", {0.0, 1.0}, 0.99, false},
        {"environment_width", "int64", "Environment width", {1, 1000}, 10, false},
        {"environment_height", "int64", "Environment height", {1, 1000}, 10, false},
        {"log_level", "string", "Log level (DEBUG, INFO, WARNING, ERROR, CRITICAL)", {}, "INFO", false},
        {"log_to_file", "bool", "Whether to log to file", {}, false, false},
        {"visualization_enabled", "bool", "Whether visualization is enabled", {}, false, false}
    };
}

// Convenience functions
bool loadConfigFromJSON(Config& config, const std::string& filepath) {
    ConfigFormatManager manager;
    return manager.loadConfig(config, filepath, ConfigFormatManager::Format::JSON);
}

bool loadConfigFromYAML(Config& config, const std::string& filepath) {
    ConfigFormatManager manager;
    return manager.loadConfig(config, filepath, ConfigFormatManager::Format::YAML);
}

bool loadConfigFromINI(Config& config, const std::string& filepath) {
    ConfigFormatManager manager;
    return manager.loadConfig(config, filepath, ConfigFormatManager::Format::INI);
}

bool exportConfigToJSON(const Config& config, const std::string& filepath) {
    ConfigFormatManager manager;
    return manager.exportConfig(config, filepath, ConfigFormatManager::Format::JSON);
}

bool exportConfigToYAML(const Config& config, const std::string& filepath) {
    ConfigFormatManager manager;
    return manager.exportConfig(config, filepath, ConfigFormatManager::Format::YAML);
}

bool exportConfigToINI(const Config& config, const std::string& filepath) {
    ConfigFormatManager manager;
    return manager.exportConfig(config, filepath, ConfigFormatManager::Format::INI);
}

} // namespace nlm