#ifndef NLM_CONFIG_MANAGER_HPP
#define NLM_CONFIG_MANAGER_HPP

#include <string>
#include <unordered_map>
#include <optional>
#include <variant>
#include <memory>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <spdlog/spdlog.h>
#include "core/ErrorHandling/ErrorHandling.hpp"

namespace nlm {

// Configuration value types
using ConfigValue = std::variant<
    int64_t,
    double,
    float,
    std::string,
    bool,
    std::nullptr_t
>;

class ConfigManager {
public:
    ConfigManager() {
        logger_ = spdlog::get("nlm");
        if (!logger_) {
            logger_ = spdlog::stdout_color_mt("nlm");
        }
    }

    // Load configuration from file
    bool loadFromFile(const std::string& filepath) {
        std::ifstream file(filepath);
        if (!file.is_open()) {
            NLM_THROW_ERROR(NLMErrorCode::ConfigFileNotFound,
                          "Cannot open configuration file: " + filepath,
                          NLMErrorCategory::ConfigError);
            return false;
        }

        std::string line;
        int lineNumber = 0;
        bool inSection = false;
        std::string currentSection;
        
        while (std::getline(file, line)) {
            ++lineNumber;
            std::string trimmed = line;
            
            // Remove whitespace
            trimmed.erase(0, trimmed.find_first_not_of(" \t\r\n"));
            trimmed.erase(trimmed.find_last_not_of(" \t\r\n") + 1);
            
            // Skip empty lines and comments
            if (trimmed.empty() || trimmed[0] == '#') {
                continue;
            }
            
            // Check for section headers
            if (trimmed[0] == '[' && trimmed.back() == ']') {
                inSection = true;
                currentSection = trimmed.substr(1, trimmed.size() - 2);
                continue;
            }
            
            // Parse key = value pairs
            size_t equalsPos = trimmed.find('=');
            if (equalsPos == std::string::npos) {
                NLM_HANDLE_WARNING("Invalid configuration line " + std::to_string(lineNumber) + ": " + line,
                                 "config_file=" + filepath);
                continue;
            }
            
            std::string key = trimmed.substr(0, equalsPos);
            std::string value = trimmed.substr(equalsPos + 1);
            
            // Remove whitespace from key and value
            key.erase(0, key.find_first_not_of(" \t"));
            key.erase(key.find_last_not_of(" \t") + 1);
            
            value.erase(0, value.find_first_not_of(" \t"));
            value.erase(value.find_last_not_of(" \t") + 1);
            
            // Add section prefix if in a section
            if (!currentSection.empty()) {
                key = currentSection + "_" + key;
            }
            
            // Parse value based on type
            ConfigValue parsedValue;
            bool parsed = parseConfigValue(value, parsedValue);
            
            if (parsed) {
                config_[key] = parsedValue;
                NLM_HANDLE_INFO("Loaded configuration: " + key + " = " + value,
                              "config_file=" + filepath);
            } else {
                NLM_HANDLE_WARNING("Failed to parse configuration value for key " + key + ": " + value,
                                 "config_file=" + filepath);
            }
        }
        
        file.close();
        return true;
    }

    // Load configuration from string
    bool loadFromString(const std::string& configString) {
        std::istringstream stream(configString);
        std::string line;
        int lineNumber = 0;
        bool inSection = false;
        std::string currentSection;
        
        while (std::getline(stream, line)) {
            ++lineNumber;
            std::string trimmed = line;
            
            // Remove whitespace
            trimmed.erase(0, trimmed.find_first_not_of(" \t\r\n"));
            trimmed.erase(trimmed.find_last_not_of(" \t\r\n") + 1);
            
            // Skip empty lines and comments
            if (trimmed.empty() || trimmed[0] == '#') {
                continue;
            }
            
            // Check for section headers
            if (trimmed[0] == '[' && trimmed.back() == ']') {
                inSection = true;
                currentSection = trimmed.substr(1, trimmed.size() - 2);
                continue;
            }
            
            // Parse key = value pairs
            size_t equalsPos = trimmed.find('=');
            if (equalsPos == std::string::npos) {
                NLM_HANDLE_WARNING("Invalid configuration line " + std::to_string(lineNumber) + ": " + line,
                                 "config_string");
                continue;
            }
            
            std::string key = trimmed.substr(0, equalsPos);
            std::string value = trimmed.substr(equalsPos + 1);
            
            // Remove whitespace from key and value
            key.erase(0, key.find_first_not_of(" \t"));
            key.erase(key.find_last_not_of(" \t") + 1);
            
            value.erase(0, value.find_first_not_of(" \t"));
            value.erase(value.find_last_not_of(" \t") + 1);
            
            // Add section prefix if in a section
            if (!currentSection.empty()) {
                key = currentSection + "_" + key;
            }
            
            // Parse value based on type
            ConfigValue parsedValue;
            bool parsed = parseConfigValue(value, parsedValue);
            
            if (parsed) {
                config_[key] = parsedValue;
                NLM_HANDLE_INFO("Loaded configuration from string: " + key + " = " + value,
                              "config_string");
            } else {
                NLM_HANDLE_WARNING("Failed to parse configuration value for key " + key + ": " + value,
                                 "config_string");
            }
        }
        
        return true;
    }

    // Get configuration value with type-specific getter
    template<typename T>
    T getOr(const std::string& key, const T& defaultValue) const {
        try {
            auto it = config_.find(key);
            if (it == config_.end()) {
                NLM_HANDLE_WARNING("Configuration key not found: " + key + ", using default: " + std::to_string(defaultValue),
                                 "config_get=" + key);
                return defaultValue;
            }
            
            return std::get<T>(it->second);
        } catch (const std::bad_variant_access&) {
            NLM_HANDLE_WARNING("Configuration key " + key + " has wrong type for requested getter",
                             "config_get=" + key);
            return defaultValue;
        }
    }

    // Set configuration value
    template<typename T>
    void set(const std::string& key, const T& value) {
        config_[key] = value;
        NLM_HANDLE_INFO("Configuration set: " + key + " = " + valueToString(value),
                      "config_set=" + key);
    }

    // Check if configuration key exists
    bool hasKey(const std::string& key) const {
        return config_.find(key) != config_.end();
    }

    // Get all configuration keys
    std::vector<std::string> getKeys() const {
        std::vector<std::string> keys;
        keys.reserve(config_.size());
        for (const auto& pair : config_) {
            keys.push_back(pair.first);
        }
        return keys;
    }

    // Validate configuration values
    bool validate() const;

    // Print configuration summary
    void printConfig() const;

    // Merge with another configuration manager
    void merge(const ConfigManager& other);

    // Save configuration to file
    bool saveToFile(const std::string& filepath) const;

private:
    std::unordered_map<std::string, ConfigValue> config_;
    std::shared_ptr<spdlog::logger> logger_;

    // Helper functions
    std::string valueToString(const ConfigValue& value) const;
    bool parseConfigValue(const std::string& str, ConfigValue& value);
    
    // Configuration validation rules
    bool validateValueType(const std::string& key, const ConfigValue& value) const;
    bool validateValueRange(const std::string& key, const ConfigValue& value) const;
};

// Global configuration manager instance
inline ConfigManager& getGlobalConfig() {
    static ConfigManager globalConfig;
    return globalConfig;
}

} // namespace nlm

#endif // NLM_CONFIG_MANAGER_HPP