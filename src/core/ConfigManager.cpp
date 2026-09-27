// NLM Configuration Manager Implementation
// Implementation of configuration manager with schema validation and hot-reloading

#include "core/ConfigManager.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <chrono>
#include <filesystem>
#include <cstring>

namespace nlm {

// ConfigManager implementation
ConfigManager::ConfigManager() : validationEnabled_(true) {}

ConfigManager::~ConfigManager() = default;

bool ConfigManager::loadFromFile(const std::string& filename) {
    if (!ConfigFormatUtils::validateFileSyntax(filename, ConfigFormatUtils::detectFormat(filename))) {
        return false;
    }
    
    if (!ConfigFormatUtils::parseFormat(filename, ConfigFormatUtils::detectFormat(filename), *this)) {
        return false;
    }
    
    lastLoadedFile_ = filename;
    lastFileCheck_ = std::chrono::system_clock::now();
    
    return true;
}

bool ConfigManager::loadFromString(const std::string& configStr) {
    // Simple INI-like format parsing for string
    std::istringstream stream(configStr);
    std::string line;
    
    while (std::getline(stream, line)) {
        std::string key, value;
        size_t equalsPos = line.find('=');
        if (equalsPos == std::string::npos) continue;
        
        key = StringUtils::trim(line.substr(0, equalsPos));
        value = StringUtils::trim(line.substr(equalsPos + 1));
        
        if (!key.empty()) {
            setValue<std::string>(key, value);
        }
    }
    
    return true;
}

bool ConfigManager::saveToFile(const std::string& filename) const {
    std::ofstream file(filename);
    if (!file.is_open()) return false;
    
    file << exportToString();
    file.close();
    
    return true;
}

void ConfigManager::addSchema(const ConfigSchema& schema) {
    configSchemas_[schema.name] = schema;
}

template<typename T>
bool ConfigManager::getValue(const std::string& key, T& value) const {
    auto it = configData_.find(key);
    if (it == configData_.end()) return false;
    
    // Convert based on type (simplified implementation)
    const ConfigValue& cv = it->second;
    
    // This is a simplified version - in practice you'd need proper type conversion
    try {
        if constexpr (std::is_same_v<T, std::string>) {
            value = cv.stringValue;
        } else if constexpr (std::is_same_v<T, int64_t>) {
            value = cv.intValue;
        } else if constexpr (std::is_same_v<T, float>) {
            value = static_cast<float>(cv.floatValue);
        } else if constexpr (std::is_same_v<T, bool>) {
            value = cv.boolValue;
        }
        return true;
    } catch (...) {
        return false;
    }
}

template<typename T>
bool ConfigManager::setValue(const std::string& key, const T& value) {
    std::string oldValue;
    bool existed = hasValue(key);
    if (existed) {
        getValue<std::string>(key, oldValue);
    }
    
    // Convert to string representation
    std::string newValueStr;
    if constexpr (std::is_same_v<T, std::string>) {
        newValueStr = value;
    } else if constexpr (std::is_same_v<T, int64_t>) {
        newValueStr = std::to_string(value);
    } else if constexpr (std::is_same_v<T, float>) {
        newValueStr = std::to_string(value);
    } else if constexpr (std::is_same_v<T, bool>) {
        newValueStr = value ? "true" : "false";
    }
    
    // Create or update ConfigValue
    ConfigValue cv;
    cv.stringValue = newValueStr;
    cv.lastModified = std::chrono::system_clock::now();
    
    if constexpr (std::is_same_v<T, std::string>) {
        cv.type = ConfigValueType::STRING;
    } else if constexpr (std::is_same_v<T, int64_t>) {
        cv.type = ConfigValueType::INTEGER;
        cv.intValue = value;
    } else if constexpr (std::is_same_v<T, float>) {
        cv.type = ConfigValueType::FLOAT;
        cv.floatValue = value;
    } else if constexpr (std::is_same_v<T, bool>) {
        cv.type = ConfigValueType::BOOLEAN;
        cv.boolValue = value;
    }
    
    configData_[key] = cv;
    
    // Notify callbacks
    if (existed) {
        notifyChange(key, oldValue, newValueStr);
    }
    
    return true;
}

bool ConfigManager::hasValue(const std::string& key) const {
    return configData_.find(key) != configData_.end();
}

std::vector<std::string> ConfigManager::getKeys() const {
    std::vector<std::string> keys;
    keys.reserve(configData_.size());
    for (const auto& pair : configData_) {
        keys.push_back(pair.first);
    }
    return keys;
}

ValidationResult ConfigManager::validate() const {
    if (!validationEnabled_) return ValidationResult();
    
    size_t invalidCount = 0;
    size_t missingCount = 0;
    
    for (const auto& schemaPair : configSchemas_) {
        const ConfigSchema& schema = schemaPair.second;
        
        if (schema.required && !hasValue(schema.name)) {
            missingCount++;
        } else if (hasValue(schema.name)) {
            std::string value;
            if (!getValue<std::string>(schema.name, value)) {
                invalidCount++;
                if (invalidCount == 0) {
                    return ValidationResult(false, "Invalid value for required field: " + schema.name);
                }
            }
        }
    }
    
    if (missingCount > 0) {
        return ValidationResult(false, "Missing " + std::to_string(missingCount) + " required configuration values");
    }
    
    if (invalidCount > 0) {
        return ValidationResult(false, "Invalid " + std::to_string(invalidCount) + " configuration values");
    }
    
    return ValidationResult();
}

const ConfigSchema* ConfigManager::getSchema(const std::string& key) const {
    auto it = configSchemas_.find(key);
    if (it != configSchemas_.end()) {
        return &it->second;
    }
    return nullptr;
}

void ConfigManager::clear() {
    configData_.clear();
}

void ConfigManager::merge(const ConfigManager& other) {
    for (const auto& pair : other.configData_) {
        const std::string& key = pair.first;
        const ConfigValue& value = pair.second;
        
        std::string oldValue;
        bool existed = hasValue(key);
        if (existed) {
            getValue<std::string>(key, oldValue);
        }
        
        configData_[key] = value;
        
        if (existed) {
            notifyChange(key, oldValue, convertToString(value));
        }
    }
}

std::string ConfigManager::exportToString() const {
    std::stringstream result;
    
    for (const auto& pair : configData_) {
        const std::string& key = pair.first;
        const ConfigValue& value = pair.second;
        
        result << key << " = " << value.stringValue << "\n";
    }
    
    return result.str();
}

bool ConfigManager::hotReload(const std::string& filename, double maxFileAge) {
    if (filename != lastLoadedFile_) return false;
    
    auto fileTime = std::filesystem::last_write_time(filename);
    auto fileTimePoint = std::chrono::time_point_cast<std::chrono::seconds>(fileTime);
    auto currentTime = std::chrono::system_clock::now();
    
    double age = std::chrono::duration<double>(currentTime - fileTimePoint).count();
    if (age <= maxFileAge) return false;
    
    // Reload configuration
    if (loadFromFile(filename)) {
        return true;
    }
    
    return false;
}

ConfigManager::Stats ConfigManager::getStats() const {
    Stats stats;
    stats.totalKeys = configData_.size();
    stats.requiredKeysMissing = 0;
    stats.invalidKeys = 0;
    
    for (const auto& schemaPair : configSchemas_) {
        const ConfigSchema& schema = schemaPair.second;
        
        if (schema.required && !hasValue(schema.name)) {
            stats.requiredKeysMissing++;
        } else if (hasValue(schema.name)) {
            std::string value;
            if (!getValue<std::string>(schema.name, value)) {
                stats.invalidKeys++;
            }
        }
    }
    
    // File age (simplified)
    if (lastLoadedFile_.empty()) {
        stats.fileAge = 0.0;
    } else {
        auto fileTime = std::filesystem::last_write_time(lastLoadedFile_);
        auto fileTimePoint = std::chrono::time_point_cast<std::chrono::seconds>(fileTime);
        auto currentTime = std::chrono::system_clock::now();
        stats.fileAge = std::chrono::duration<double>(currentTime - fileTimePoint).count();
    }
    
    stats.fileModified = !lastLoadedFile_.empty();
    
    return stats;
}

void ConfigManager::notifyChange(const std::string& key, const std::string& oldValue, const std::string& newValue) {
    for (const auto& callback : changeCallbacks_) {
        try {
            callback(key, oldValue, newValue);
        } catch (...) {
            // Ignore callback exceptions
        }
    }
}

std::string ConfigManager::convertToString(const ConfigValue& value) const {
    switch (value.type) {
        case ConfigValueType::STRING:
            return value.stringValue;
        case ConfigValueType::INTEGER:
            return std::to_string(value.intValue);
        case ConfigValueType::FLOAT:
            return std::to_string(value.floatValue);
        case ConfigValueType::BOOLEAN:
            return value.boolValue ? "true" : "false";
        default:
            return value.stringValue;
    }
}

bool ConfigManager::convertFromString(const std::string& str, ConfigValue& value, ConfigValueType targetType) const {
    switch (targetType) {
        case ConfigValueType::STRING:
            value.stringValue = str;
            break;
        case ConfigValueType::INTEGER:
            try {
                value.intValue = std::stoll(str);
            } catch (...) {
                return false;
            }
            break;
        case ConfigValueType::FLOAT:
            try {
                value.floatValue = std::stof(str);
            } catch (...) {
                return false;
            }
            break;
        case ConfigValueType::BOOLEAN:
            value.boolValue = (str == "true" || str == "1" || str == "yes");
            break;
        default:
            value.stringValue = str;
            break;
    }
    
    value.type = targetType;
    return true;
}

// ConfigPresets implementation
std::map<std::string, std::string> ConfigPresets::getPerformancePreset() {
    return {
        {"neuron_count", "2000"},
        {"region_count", "2"},
        {"connection_probability", "0.15"},
        {"simulation_timestep", "0.0005"},
        {"enable_checkpointing", "true"},
        {"enable_replay", "true"},
        {"enable_development", "true"}
    };
}

std::map<std::string, std::string> ConfigPresets::getQualityPreset() {
    return {
        {"neuron_count", "5000"},
        {"region_count", "3"},
        {"connection_probability", "0.1"},
        {"simulation_timestep", "0.0002"},
        {"enable_checkpointing", "true"},
        {"enable_replay", "true"},
        {"enable_development", "true"}
    };
}

std::map<std::string, std::string> ConfigPresets::getBalancedPreset() {
    return {
        {"neuron_count", "1000"},
        {"region_count", "1"},
        {"connection_probability", "0.1"},
        {"simulation_timestep", "0.001"},
        {"enable_checkpointing", "true"},
        {"enable_replay", "true"},
        {"enable_development", "true"}
    };
}

std::map<std::string, std::string> ConfigPresets::getMinimalPreset() {
    return {
        {"neuron_count", "100"},
        {"region_count", "1"},
        {"connection_probability", "0.05"},
        {"simulation_timestep", "0.001"},
        {"enable_checkpointing", "false"},
        {"enable_replay", "false"},
        {"enable_development", "false"}
    };
}

std::map<std::string, std::string> ConfigPresets::getResearchPreset() {
    return {
        {"neuron_count", "10000"},
        {"region_count", "4"},
        {"connection_probability", "0.08"},
        {"simulation_timestep", "0.0001"},
        {"enable_checkpointing", "true"},
        {"enable_replay", "true"},
        {"enable_development", "true"},
        {"stdp_ltp_weight", "0.015"},
        {"stdp_ltd_weight", "0.018"}
    };
}

std::map<std::string, std::string> ConfigPresets::getRealTimePreset() {
    return {
        {"neuron_count", "500"},
        {"region_count", "1"},
        {"connection_probability", "0.2"},
        {"simulation_timestep", "0.01"},
        {"enable_checkpointing", "false"},
        {"enable_replay", "false"},
        {"enable_development", "true"}
    };
}

std::vector<std::string> ConfigPresets::getPresetNames() {
    return {
        "performance",
        "quality",
        "balanced",
        "minimal",
        "research",
        "real_time"
    };
}

bool ConfigPresets::isPresetCompatible(const std::string& presetName) {
    // Check if preset name is valid
    std::vector<std::string> validPresets = getPresetNames();
    return std::find(validPresets.begin(), validPresets.end(), presetName) != validPresets.end();
}

// ConfigFormatUtils implementation
bool ConfigFormatUtils::parseFormat(const std::string& filename, Format format, ConfigManager& config) {
    switch (format) {
        case Format::INI:
            // Simple INI parsing
            {
                std::ifstream file(filename);
                if (!file.is_open()) return false;
                
                std::string line;
                while (std::getline(file, line)) {
                    std::string key, value;
                    size_t equalsPos = line.find('=');
                    if (equalsPos == std::string::npos) continue;
                    
                    key = StringUtils::trim(line.substr(0, equalsPos));
                    value = StringUtils::trim(line.substr(equalsPos + 1));
                    
                    if (!key.empty()) {
                        config.setValue<std::string>(key, value);
                    }
                }
            }
            return true;
        
        case Format::PLAIN:
            // Plain key=value format
            return config.loadFromFile(filename);
        
        default:
            // For now, default to plain text format
            return config.loadFromFile(filename);
    }
}

ConfigFormatUtils::Format ConfigFormatUtils::detectFormat(const std::string& filename) {
    std::string ext = StringUtils::toLower(StringUtils::trim(filename.substr(filename.find_last_of('.') + 1)));
    
    if (ext == "ini" || ext == "cfg" || ext == "conf") {
        return Format::INI;
    } else if (ext == "json") {
        return Format::JSON;
    } else if (ext == "yaml" || ext == "yml") {
        return Format::YAML;
    } else if (ext == "toml") {
        return Format::TOML;
    } else if (ext == "xml") {
        return Format::XML;
    } else {
        return Format::PLAIN;
    }
}

std::string ConfigFormatUtils::exportToFormat(const ConfigManager& config, Format format) {
    std::stringstream result;
    
    switch (format) {
        case Format::INI:
            for (const std::string& key : config.getKeys()) {
                std::string value;
                if (config.getValue<std::string>(key, value)) {
                    result << key << " = " << value << "\n";
                }
            }
            break;
        
        case Format::PLAIN:
            result << config.exportToString();
            break;
        
        default:
            // Default to INI format
            for (const std::string& key : config.getKeys()) {
                std::string value;
                if (config.getValue<std::string>(key, value)) {
                    result << key << " = " << value << "\n";
                }
            }
            break;
    }
    
    return result.str();
}

std::vector<std::string> ConfigFormatUtils::getSupportedExtensions() {
    return {"cfg", "ini", "conf", "txt", "config"};
}

bool ConfigFormatUtils::validateFileSyntax(const std::string& filename, Format format) {
    std::ifstream file(filename);
    if (!file.is_open()) return false;
    
    switch (format) {
        case Format::INI:
            {
                std::string line;
                while (std::getline(file, line)) {
                    std::string key, value;
                    size_t equalsPos = line.find('=');
                    if (equalsPos != std::string::npos) {
                        key = StringUtils::trim(line.substr(0, equalsPos));
                        value = StringUtils::trim(line.substr(equalsPos + 1));
                        if (key.empty()) {
                            return false;
                        }
                    } else if (!line.empty() && !StringUtils::startsWith(line, "#")) {
                        return false;
                    }
                }
            }
            return true;
        
        case Format::PLAIN:
            // Any non-empty file is considered valid for plain format
            return file.peek() != std::ifstream::traits_type::eof();
        
        default:
            // Default to INI validation
            return validateFileSyntax(filename, Format::INI);
    }
}

bool ConfigFormatUtils::mergeFiles(const std::vector<std::string>& filenames, ConfigManager& outputConfig) {
    for (const std::string& filename : filenames) {
        ConfigManager tempConfig;
        Format format = detectFormat(filename);
        
        if (!parseFormat(filename, format, tempConfig)) {
            return false;
        }
        
        outputConfig.merge(tempConfig);
    }
    
    return true;
}

} // namespace nlm