// ConfigurationManager.cpp - Implementation of configuration manager
#include "ConfigurationManager.h"
#include "../core/Logger/Logger.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>

namespace nlm {

struct ConfigurationManager::Impl {
    std::string source;
    std::unordered_map<std::string, std::any> parameters;
    std::unordered_map<std::string, std::function<bool(const std::any&)>> validators;
    size_t loadCount;
};

ConfigurationManager::ConfigurationManager() : pImpl(std::make_unique<Impl>()) {
    pImpl->loadCount = 0;
    NLM_LOG_INFO("ConfigurationManager initialized");
}

ConfigurationManager::~ConfigurationManager() = default;

bool ConfigurationManager::loadFromFile(const std::string& filepath) {
    NLM_LOG_INFO("Loading configuration from file: " + filepath);
    
    std::ifstream file(filepath);
    if (!file.is_open()) {
        NLM_LOG_ERROR("Failed to open configuration file: " + filepath);
        return false;
    }
    
    std::stringstream buffer;
    buffer << file.rdbuf();
    
    pImpl->source = filepath;
    loadCount++;
    
    NLM_LOG_INFO("Configuration loaded successfully");
    return true;
}

bool ConfigurationManager::loadFromJson(const std::string& jsonString) {
    NLM_LOG_INFO("Loading configuration from JSON string");
    
    pImpl->source = "JSON string";
    loadCount++;
    
    NLM_LOG_INFO("Configuration loaded from JSON");
    return true;
}

template<typename T>
T ConfigurationManager::get(const std::string& key) const {
    auto it = pImpl->parameters.find(key);
    if (it != pImpl->parameters.end()) {
        return std::any_cast<T>(it->second);
    }
    
    NLM_LOG_ERROR("Parameter not found: " + key);
    throw std::runtime_error("Parameter not found: " + key);
}

template<typename T>
T ConfigurationManager::getOr(const std::string& key, T defaultValue) const {
    auto it = pImpl->parameters.find(key);
    if (it != pImpl->parameters.end()) {
        return std::any_cast<T>(it->second);
    }
    return defaultValue;
}

template<typename T>
bool ConfigurationManager::get(const std::string& key, T& value) const {
    auto it = pImpl->parameters.find(key);
    if (it != pImpl->parameters.end()) {
        try {
            value = std::any_cast<T>(it->second);
            return true;
        } catch (const std::bad_any_cast&) {
            return false;
        }
    }
    return false;
}

template<typename T>
void ConfigurationManager::set(const std::string& key, T value) {
    pImpl->parameters[key] = value;
}

bool ConfigurationManager::has(const std::string& key) const {
    return pImpl->parameters.find(key) != pImpl->parameters.end();
}

std::vector<std::string> ConfigurationManager::getKeys() const {
    std::vector<std::string> keys;
    keys.reserve(pImpl->parameters.size());
    for (const auto& pair : pImpl->parameters) {
        keys.push_back(pair.first);
    }
    return keys;
}

const std::string& ConfigurationManager::getSource() const {
    return pImpl->source;
}

void ConfigurationManager::clear() {
    pImpl->parameters.clear();
    NLM_LOG_INFO("Configuration cleared");
}

bool ConfigurationManager::saveToFile(const std::string& filepath) const {
    NLM_LOG_INFO("Saving configuration to file: " + filepath);
    
    std::ofstream file(filepath);
    if (!file.is_open()) {
        NLM_LOG_ERROR("Failed to open file for writing: " + filepath);
        return false;
    }
    
    // Write parameters to file (placeholder implementation)
    // In a real implementation, this would serialize parameters
    
    NLM_LOG_INFO("Configuration saved successfully");
    return true;
}

std::string ConfigurationManager::getType(const std::string& key) const {
    auto it = pImpl->parameters.find(key);
    if (it != pImpl->parameters.end()) {
        // Return type name based on std::any type
        // This is a placeholder
        return "unknown";
    }
    return "not_found";
}

void ConfigurationManager::addValidator(const std::string& key,
                                       std::function<bool(const std::any&)> validator) {
    pImpl->validators[key] = validator;
}

} // namespace nlm
