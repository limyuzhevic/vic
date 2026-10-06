#include "core/Config/ConfigManager.hpp"
#include <sstream>
#include <limits>

namespace nlm {

bool ConfigManager::loadFromFile(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        logError("Cannot open configuration file: " + filepath);
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
            logWarning("Invalid configuration line " + std::to_string(lineNumber) + " in " + filepath + ": " + line);
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
            logDebug("Loaded configuration: " + key + " = " + value);
        } else {
            logWarning("Failed to parse configuration value for key " + key + ": " + value);
        }
    }
    
    file.close();
    return true;
}

bool ConfigManager::loadFromString(const std::string& configString) {
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
            logWarning("Invalid configuration line " + std::to_string(lineNumber) + ": " + line);
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
            logDebug("Loaded configuration from string: " + key + " = " + value);
        } else {
            logWarning("Failed to parse configuration value for key " + key + ": " + value);
        }
    }
    
    return true;
}

bool ConfigManager::parseConfigValue(const std::string& str, ConfigValue& value) {
    // Try to parse as boolean
    if (str == "true" || str == "1" || str == "yes" || str == "on") {
        value = true;
        return true;
    }
    if (str == "false" || str == "0" || str == "no" || str == "off") {
        value = false;
        return true;
    }
    
    // Try to parse as integer
    try {
        size_t pos;
        int64_t intVal = std::stoll(str, &pos);
        if (pos == str.size()) {
            value = intVal;
            return true;
        }
    } catch (...) {}
    
    // Try to parse as double
    try {
        size_t pos;
        double doubleVal = std::stod(str, &pos);
        if (pos == str.size()) {
            value = doubleVal;
            return true;
        }
    } catch (...) {}
    
    // Parse as string
    value = str;
    return true;
}

std::string ConfigManager::valueToString(const ConfigValue& value) const {
    std::ostringstream oss;
    std::visit([&oss](const auto& val) {
        if constexpr (std::is_same_v<decltype(val), int64_t>) {
            oss << val;
        } else if constexpr (std::is_same_v<decltype(val), double>) {
            oss << val;
        } else if constexpr (std::is_same_v<decltype(val), float>) {
            oss << val;
        } else if constexpr (std::is_same_v<decltype(val), std::string>) {
            oss << "\"" << val << "\"";
        } else if constexpr (std::is_same_v<decltype(val), bool>) {
            oss << (val ? "true" : "false");
        } else {
            oss << "null";
        }
    }, value);
    return oss.str();
}

bool ConfigManager::validate() const {
    bool allValid = true;
    
    for (const auto& pair : config_) {
        if (!validateValueType(pair.first, pair.second)) {
            allValid = false;
        }
        if (!validateValueRange(pair.first, pair.second)) {
            allValid = false;
        }
    }
    
    return allValid;
}

bool ConfigManager::validateValueType(const std::string& key, const ConfigValue& value) const {
    // Example validation rules
    if (key == "random_seed" || key == "neuron_count" || key == "region_count") {
        if (!std::holds_alternative<int64_t>(value)) {
            logError("Configuration key " + key + " must be int64_t, got: " + valueToString(value));
            return false;
        }
    } else if (key == "simulation_timestep" || key == "plasticity_learning_rate" || 
               key == "stdp_ltp_weight" || key == "stdp_ltd_weight" || 
               key == "stdp_time_constant") {
        if (!std::holds_alternative<double>(value) && !std::holds_alternative<float>(value)) {
            logError("Configuration key " + key + " must be numeric, got: " + valueToString(value));
            return false;
        }
    } else if (key == "connection_probability" || key == "development_synaptogenesis_rate" || 
               key == "development_pruning_rate") {
        if (!std::holds_alternative<double>(value) && !std::holds_alternative<float>(value)) {
            logError("Configuration key " + key + " must be numeric, got: " + valueToString(value));
            return false;
        }
        
        // Check range
        double val = std::holds_alternative<double>(value) ? 
                     std::get<double>(value) : std::get<float>(value);
        if (val < 0.0 || val > 1.0) {
            logWarning("Configuration key " + key + " value " + valueToString(value) + " is outside recommended range [0.0, 1.0]");
        }
    } else if (key == "log_level" || key == "log_to_file") {
        if (!std::holds_alternative<bool>(value)) {
            logError("Configuration key " + key + " must be bool, got: " + valueToString(value));
            return false;
        }
    }
    
    return true;
}

bool ConfigManager::validateValueRange(const std::string& key, const ConfigValue& value) const {
    // Check specific key ranges
    if (key == "neuron_count") {
        if (std::holds_alternative<int64_t>(value)) {
            int64_t val = std::get<int64_t>(value);
            if (val <= 0 || val > 100000) {
                logError("Configuration key " + key + " value " + valueToString(value) + " is outside valid range (1-100000)");
                return false;
            }
        }
    } else if (key == "region_count") {
        if (std::holds_alternative<int64_t>(value)) {
            int64_t val = std::get<int64_t>(value);
            if (val <= 0 || val > 1000) {
                logError("Configuration key " + key + " value " + valueToString(value) + " is outside valid range (1-1000)");
                return false;
            }
        }
    }
    
    return true;
}

void ConfigManager::printConfig() const {
    logInfo("=== NLM Configuration Summary ===");
    for (const auto& pair : config_) {
        logInfo(pair.first + " = " + valueToString(pair.second));
    }
    logInfo("==================================");
}

void ConfigManager::merge(const ConfigManager& other) {
    for (const auto& pair : other.config_) {
        config_[pair.first] = pair.second;
    }
}

bool ConfigManager::saveToFile(const std::string& filepath) const {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        logError("Cannot open configuration file for writing: " + filepath);
        return false;
    }
    
    file << "# NLM Configuration File\n";
    file << "# Generated on " << getCurrentTimestamp() << "\n\n";
    
    // Write configuration with some organization
    for (const auto& pair : config_) {
        file << pair.first << " = " << valueToString(pair.second) << "\n";
    }
    
    file.close();
    return true;
}

std::string ConfigManager::getCurrentTimestamp() const {
    auto now = std::chrono::system_clock::now();
    auto time_t = std::chrono::system_clock::to_time_t(now);
    std::stringstream ss;
    ss << std::put_time(std::localtime(&time_t), "%Y-%m-%d %H:%M:%S");
    return ss.str();
}

void ConfigManager::logError(const std::string& message) const {
    if (logger_) {
        logger_->error(message);
    }
}

void ConfigManager::logWarning(const std::string& message) const {
    if (logger_) {
        logger_->warn(message);
    }
}

void ConfigManager::logDebug(const std::string& message) const {
    if (logger_) {
        logger_->debug(message);
    }
}

void ConfigManager::logInfo(const std::string& message) const {
    if (logger_) {
        logger_->info(message);
    }
}

} // namespace nlm