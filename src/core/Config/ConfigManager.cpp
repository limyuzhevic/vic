#include "ConfigManager.hpp"
#include "Config.hpp"
#include "Logger/Logger.hpp"
#include <fstream>
#include <sstream>
#include <filesystem>

namespace nlm {

bool ConfigManager::editConfig(const std::string& key, const std::string& value) {
    try {
        // Validate key format (alphanumeric, dots, underscores)
        if (key.empty() || !std::all_of(key.begin(), key.end(), [](char c) {
            return std::isalnum(c) || c == '.' || c == '_' || c == '-'; })) {
            lastError = "Invalid key format: " + key;
            NLM_LOG_ERROR(lastError);
            return false;
        }
        
        configValues[key] = value;
        NLM_LOG_INFO("Configuration updated: " + key + " = " + value);
        return true;
        
    } catch (const std::exception& e) {
        lastError = "Error editing config: " + std::string(e.what());
        NLM_LOG_ERROR(lastError);
        return false;
    }
}

bool ConfigManager::editConfig(const std::string& key, double value) {
    return editConfig(key, std::to_string(value));
}

bool ConfigManager::editConfig(const std::string& key, int value) {
    return editConfig(key, std::to_string(value));
}

bool ConfigManager::editConfig(const std::string& key, bool value) {
    return editConfig(key, value ? "true" : "false");
}

std::vector<std::string> ConfigManager::listConfigKeys() const {
    std::vector<std::string> keys;
    for (const auto& pair : configValues) {
        keys.push_back(pair.first);
    }
    std::sort(keys.begin(), keys.end());
    return keys;
}

std::vector<std::string> ConfigManager::listConfigKeys(const std::string& prefix) const {
    std::vector<std::string> keys;
    for (const auto& pair : configValues) {
        if (pair.first.find(prefix) == 0) {
            keys.push_back(pair.first);
        }
    }
    std::sort(keys.begin(), keys.end());
    return keys;
}

std::string ConfigManager::getConfigValue(const std::string& key) const {
    auto it = configValues.find(key);
    if (it != configValues.end()) {
        return it->second;
    }
    return "";
}

bool ConfigManager::saveConfig(const std::string& filename) const {
    try {
        // Create directory if it doesn't exist
        std::filesystem::path filePath(filename);
        if (!filePath.parent_path().empty()) {
            std::filesystem::create_directories(filePath.parent_path());
        }
        
        std::ofstream file(filename);
        if (!file.is_open()) {
            lastError = "Cannot open file for writing: " + filename;
            NLM_LOG_ERROR(lastError);
            return false;
        }
        
        // Simple key=value format for configuration
        for (const auto& pair : configValues) {
            file << pair.first << "=" << pair.second << "\n";
        }
        
        file.close();
        
        NLM_LOG_INFO("Configuration saved to: " + filename);
        return true;
        
    } catch (const std::exception& e) {
        lastError = "Error saving config: " + std::string(e.what());
        NLM_LOG_ERROR(lastError);
        return false;
    }
}

bool ConfigManager::loadConfig(const std::string& filename) {
    try {
        std::ifstream file(filename);
        if (!file.is_open()) {
            lastError = "Cannot open file for reading: " + filename;
            NLM_LOG_ERROR(lastError);
            return false;
        }
        
        configValues.clear();
        std::string line;
        while (std::getline(file, line)) {
            // Skip empty lines and comments
            if (line.empty() || line[0] == '#') {
                continue;
            }
            
            size_t equalsPos = line.find('=');
            if (equalsPos != std::string::npos && equalsPos > 0) {
                std::string key = line.substr(0, equalsPos);
                std::string value = line.substr(equalsPos + 1);
                // Trim whitespace
                key.erase(0, key.find_first_not_of(" \t\r\n"));
                key.erase(key.find_last_not_of(" \t\r\n") + 1);
                value.erase(0, value.find_first_not_of(" \t\r\n"));
                value.erase(value.find_last_not_of(" \t\r\n") + 1);
                
                if (!key.empty()) {
                    configValues[key] = value;
                }
            }
        }
        
        file.close();
        
        NLM_LOG_INFO("Configuration loaded from: " + filename);
        return true;
        
    } catch (const std::exception& e) {
        lastError = "Error loading config: " + std::string(e.what());
        NLM_LOG_ERROR(lastError);
        return false;
    }
}

bool ConfigManager::hasConfig(const std::string& key) const {
    return configValues.find(key) != configValues.end();
}

bool ConfigManager::isConfigValid() const {
    return !configValues.empty();
}

std::string ConfigManager::formatConfig(const std::map<std::string, std::string>& config) const {
    std::stringstream ss;
    ss << "{\n";
    for (const auto& pair : config) {
        ss << "  \"" << pair.first << "\": \"" << pair.second << "\",\n";
    }
    ss << "}";
    return ss.str();
}

std::map<std::string, std::string> ConfigManager::parseConfigFile(const std::string& filename) const {
    // This is a helper that delegates to the main Config class
    // For now, just return an empty map
    return std::map<std::string, std::string>();
}

std::string ConfigManager::configEditHelp() {
    return "config edit --key <key> --value <value> - Edit configuration value";
}

std::string ConfigManager::configListHelp() {
    return "config list [--key-prefix <prefix>] - List configuration keys";
}

std::string ConfigManager::configSaveHelp() {
    return "config save --file <filename> - Save configuration to file";
}

} // namespace nlm
