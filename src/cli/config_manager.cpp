// Configuration manager for CLI
// Implementation of configuration handling

#include "config_manager.hpp"
#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <cctype>

using namespace nlm;

ConfigManager::ConfigManager(std::shared_ptr<Config> config) : config_(config) {
}

ConfigManager::~ConfigManager() {
}

bool ConfigManager::loadFromFile(const std::string& filepath) {
    // Try to load from different formats
    std::ifstream jsonFile(filepath + ".json");
    if (jsonFile.good()) {
        return loadFromJSON(filepath + ".json");
    }
    
    // Try simple key=value format
    std::ifstream textFile(filepath);
    if (textFile.good()) {
        return parseConfigFile(filepath);
    }
    
    return false;
}

bool ConfigManager::saveToFile(const std::string& filepath) const {
    // Try to save in multiple formats based on extension
    if (filepath.size() >= 5 && filepath.substr(filepath.size() - 5) == ".json") {
        return saveAsJSON(filepath);
    } else {
        // Save as simple key=value format
        std::ofstream file(filepath);
        if (!file.is_open()) {
            return false;
        }
        
        for (const auto& key : config_->getKeys()) {
            std::optional<ConfigValue> value = config_->get<ConfigValue>(key);
            if (value) {
                file << key << " = " << formatValue(*value) << std::endl;
            }
        }
        
        file.close();
        currentConfigFile_ = filepath;
        return true;
    }
}

bool ConfigManager::setParameter(const std::string& key, const std::string& value) {
    if (!config_.get()) {
        return false;
    }
    
    // Try to parse as different types
    // Check if it's a boolean
    if (value == "true" || value == "false") {
        bool boolValue = (value == "true");
        config_->set(key, boolValue, ConfigSource::CommandLine);
        return true;
    }
    
    // Check if it's an integer
    try {
        size_t pos;
        int intValue = std::stoi(value, &pos);
        if (pos == value.size()) {
            config_->set(key, intValue, ConfigSource::CommandLine);
            return true;
        }
    } catch (...) {
    }
    
    // Check if it's a double
    try {
        size_t pos;
        double doubleValue = std::stod(value, &pos);
        if (pos == value.size()) {
            config_->set(key, doubleValue, ConfigSource::CommandLine);
            return true;
        }
    } catch (...) {
    }
    
    // Default to string
    config_->set(key, value, ConfigSource::CommandLine);
    return true;
}

void ConfigManager::showConfig() const {
    std::cout << "=== NLM Configuration ===" << std::endl;
    std::cout << std::endl;
    
    for (const auto& key : config_->getKeys()) {
        std::optional<ConfigValue> value = config_->get<ConfigValue>(key);
        if (value) {
            std::cout << key << " = " << formatValue(*value);
            
            // Show source
            auto it = std::find_if(config_->getKeys().begin(), config_->getKeys().end(),
                                  [&key](const std::string& k) { return k == key; });
            if (it != config_->getKeys().end()) {
                // We would need access to get the source, but for now just show value
            }
            std::cout << std::endl;
        }
    }
    
    std::cout << std::endl;
    std::cout << "Configuration file: " << (currentConfigFile_.empty() ? "None" : currentConfigFile_) << std::endl;
}

std::string ConfigManager::getParameter(const std::string& key) const {
    std::optional<ConfigValue> value = config_->get<ConfigValue>(key);
    if (value) {
        return formatValue(*value);
    }
    return "";
}

bool ConfigManager::hasParameter(const std::string& key) const {
    return config_->has(key);
}

std::vector<std::string> ConfigManager::getAllKeys() const {
    return config_->getKeys();
}

std::string ConfigManager::formatValue(const ConfigValue& value) const {
    std::stringstream stream;
    
    std::visit([&](const auto& val) {
        using T = std::decay_t<decltype(val)>;
        if constexpr (std::is_same_v<T, int>) {
            stream << val;
        } else if constexpr (std::is_same_v<T, int64_t>) {
            stream << val;
        } else if constexpr (std::is_same_v<T, double>) {
            stream << std::fixed << std::setprecision(6) << val;
        } else if constexpr (std::is_same_v<T, bool>) {
            stream << (val ? "true" : "false");
        } else if constexpr (std::is_same_v<T, std::string>) {
            stream << val;
        } else if constexpr (std::is_same_v<T, std::vector<int>>) {
            stream << "[";
            for (size_t i = 0; i < val.size(); ++i) {
                if (i > 0) stream << ", ";
                stream << val[i];
            }
            stream << "]";
        } else if constexpr (std::is_same_v<T, std::vector<double>>) {
            stream << "[";
            for (size_t i = 0; i < val.size(); ++i) {
                if (i > 0) stream << ", ";
                stream << std::fixed << std::setprecision(6) << val[i];
            }
            stream << "]";
        } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
            stream << "[";
            for (size_t i = 0; i < val.size(); ++i) {
                if (i > 0) stream << ", ";
                stream << val[i];
            }
            stream << "]";
        } else {
            stream << "<unknown>";
        }
    }, value);
    
    return stream.str();
}

bool ConfigManager::parseConfigFile(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    std::string line;
    int lineNum = 0;
    
    while (std::getline(file, line)) {
        lineNum++;
        // Skip empty lines and comments
        size_t commentPos = line.find('#');
        if (commentPos != std::string::npos) {
            line = line.substr(0, commentPos);
        }
        
        line = trim(line);
        if (line.empty()) {
            continue;
        }
        
        size_t equalsPos = line.find('=');
        if (equalsPos == std::string::npos) {
            continue; // Invalid line format
        }
        
        std::string key = trim(line.substr(0, equalsPos));
        std::string value = trim(line.substr(equalsPos + 1));
        
        if (!key.empty()) {
            setParameter(key, value);
        }
    }
    
    file.close();
    currentConfigFile_ = filepath;
    return true;
}

bool ConfigManager::saveAsJSON(const std::string& filepath) const {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    file << "{\n";
    
    size_t keyCount = config_->getKeys().size();
    size_t keyIndex = 0;
    
    for (const auto& key : config_->getKeys()) {
        std::optional<ConfigValue> value = config_->get<ConfigValue>(key);
        if (value) {
            file << "  \"" << key << "\": " << formatValue(*value);
            
            if (keyIndex < keyCount - 1) {
                file << ",";
            }
            file << "\n";
            
            keyIndex++;
        }
    }
    
    file << "}\n";
    file.close();
    currentConfigFile_ = filepath;
    return true;
}

bool ConfigManager::loadFromJSON(const std::string& filepath) {
    // Simplified JSON parsing - in production would use a proper JSON library
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    std::string line;
    bool inObject = false;
    std::string currentKey;
    
    while (std::getline(file, line)) {
        // Simple JSON parsing - remove braces, quotes, and parse key:value pairs
        // This is a simplified implementation for demonstration
        size_t colonPos = line.find(':');
        if (colonPos != std::string::npos) {
            std::string key = line.substr(0, colonPos);
            std::string value = line.substr(colonPos + 1);
            
            // Remove quotes and whitespace
            key = trim(key);
            value = trim(value);
            
            if (!key.empty() && key.front() == '\"' && key.back() == '\"') {
                key = key.substr(1, key.size() - 2);
            }
            
            if (!value.empty() && value.front() == '\"' && value.back() == '\"') {
                value = value.substr(1, value.size() - 2);
            }
            
            setParameter(key, value);
        }
    }
    
    file.close();
    currentConfigFile_ = filepath;
    return true;
}

std::string ConfigManager::trim(const std::string& str) {
    size_t first = str.find_first_not_of(" \t\r\n");
    if (first == std::string::npos) {
        return "";
    }
    
    size_t last = str.find_last_not_of(" \t\r\n");
    return str.substr(first, last - first + 1);
}