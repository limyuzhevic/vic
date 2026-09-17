#include "ConfigCommand.hpp"
#include "CLIContext.hpp"
#include <iostream>
#include <fstream>
#include <sstream>
#include <nlohmann/json.hpp>
#include <filesystem>
#include <algorithm>
#include <iomanip>

using json = nlohmann::json;

namespace nlm {

bool ConfigCommand::executeList(const CLIContext& context, const std::map<std::string, std::string>& options) {
    std::cout << "=== Configuration Keys ===" << std::endl;
    auto keys = context.config->getKeys();
    
    if (keys.empty()) {
        std::cout << "No configuration keys found." << std::endl;
        return true;
    }
    
    // Sort keys for consistent output
    std::vector<std::string> sortedKeys = keys;
    std::sort(sortedKeys.begin(), sortedKeys.end());
    
    size_t maxKeyLength = 0;
    for (const auto& key : sortedKeys) {
        maxKeyLength = std::max(maxKeyLength, key.length());
    }
    
    for (const auto& key : sortedKeys) {
        auto value = context.config->get<std::string>(key);
        std::cout << std::left << "  " << std::setw(30) << key << ": " << 
            (value.has_value() ? *value : "<empty>") << std::endl;
    }
    
    return true;
}

bool ConfigCommand::executeShow(const CLIContext& context, const std::string& key, 
                               const std::map<std::string, std::string>& options) {
    std::cout << "=== Configuration Value: " << key << " ===" << std::endl;
    
    if (!context.config->has(key)) {
        std::cout << "Key not found: " << key << std::endl;
        return false;
    }
    
    auto value = context.config->get<std::string>(key);
    if (value.has_value()) {
        std::cout << key << " = " << *value << std::endl;
    }
    
    return true;
}

bool ConfigCommand::executeSet(CLIContext& context, const std::string& key, 
                              const std::string& valueStr, 
                              const std::map<std::string, std::string>& options) {
    std::cout << "=== Setting Configuration ===" << std::endl;
    std::cout << "Key: " << key << std::endl;
    std::cout << "Value: " << valueStr << std::endl;
    
    // Try to determine value type
    try {
        // Try int
        size_t pos = 0;
        int intVal = std::stoi(valueStr, &pos);
        if (pos == valueStr.length()) {
            context.config->set(key, intVal);
            std::cout << "Set as integer: " << intVal << std::endl;
            return true;
        }
    } catch (...) {}
    
    try {
        // Try double
        size_t pos = 0;
        double doubleVal = std::stod(valueStr, &pos);
        if (pos == valueStr.length()) {
            context.config->set(key, doubleVal);
            std::cout << "Set as double: " << doubleVal << std::endl;
            return true;
        }
    } catch (...) {}
    
    try {
        // Try bool
        if (valueStr == "true" || valueStr == "1") {
            context.config->set(key, true);
            std::cout << "Set as boolean: true" << std::endl;
            return true;
        } else if (valueStr == "false" || valueStr == "0") {
            context.config->set(key, false);
            std::cout << "Set as boolean: false" << std::endl;
            return true;
        }
    } catch (...) {}
    
    // Default to string
    context.config->set(key, valueStr);
    std::cout << "Set as string: " << valueStr << std::endl;
    
    return true;
}

bool ConfigCommand::executeCreate(const CLIContext& context, const std::string& filename,
                                 const std::map<std::string, std::string>& options) {
    std::cout << "=== Creating Configuration File ===" << std::endl;
    std::cout << "Filename: " << filename << std::endl;
    
    std::ofstream file(filename);
    if (!file.is_open()) {
        std::cout << "Failed to create file: " << filename << std::endl;
        return false;
    }
    
    // Get current config values
    auto keys = context.config->getKeys();
    json configJson;
    
    for (const auto& key : keys) {
        auto value = context.config->get<std::string>(key);
        if (value.has_value()) {
            configJson[key] = *value;
        }
    }
    
    file << configJson.dump(2) << std::endl;
    file.close();
    
    std::cout << "Configuration file created successfully: " << filename << std::endl;
    return true;
}

bool ConfigCommand::executeValidate(const CLIContext& context, const std::string& filename,
                                   const std::map<std::string, std::string>& options) {
    std::cout << "=== Validating Configuration File ===" << std::endl;
    std::cout << "Filename: " << filename << std::endl;
    
    if (!std::filesystem::exists(filename)) {
        std::cout << "File does not exist: " << filename << std::endl;
        return false;
    }
    
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cout << "Failed to read file: " << filename << std::endl;
        return false;
    }
    
    try {
        json configJson = json::parse(file);
        
        std::cout << "✓ File is valid JSON format" << std::endl;
        
        // Count key-value pairs
        std::cout << "  Found " << configJson.size() << " configuration entries" << std::endl;
        
        // Show first few keys
        int count = 0;
        for (auto it = configJson.begin(); it != configJson.end() && count < 5; ++it, ++count) {
            std::cout << "    - " << it.key() << std::endl;
        }
        
        if (configJson.size() > 5) {
            std::cout << "    ... and " << configJson.size() - 5 << " more entries" << std::endl;
        }
        
        file.close();
        return true;
        
    } catch (const json::parse_error& e) {
        std::cout << "✗ Invalid JSON format: " << e.what() << std::endl;
        file.close();
        return false;
    }
}

} // namespace nlm
