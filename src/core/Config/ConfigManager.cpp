#pragma once

#include "ConfigManager.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <chrono>

namespace nlm {

struct ConfigManager::Impl {
    std::unique_ptr<ConfigAdvanced> activeConfig;
    std::unordered_map<std::string, ConfigAdvanced> configFiles;
    std::string defaultFormat = "json";
    bool autoSave = true;
    std::chrono::milliseconds autoSaveInterval = std::chrono::milliseconds(5000);
    std::chrono::system_clock::time_point lastSave;
    std::unordered_map<std::string, std::string> environmentConfigs;
};

ConfigManager::ConfigManager() : pImpl(std::make_unique<Impl>()) {
    pImpl->activeConfig = std::make_unique<ConfigAdvanced>();
}

ConfigManager::~ConfigManager() = default;

ConfigManager::ConfigManager(ConfigManager&& other) noexcept : 
    pImpl(std::move(other.pImpl)) {
}

ConfigManager& ConfigManager::operator=(ConfigManager&& other) noexcept {
    if (this != &other) {
        pImpl = std::move(other.pImpl);
    }
    return *this;
}

bool ConfigManager::loadConfig(const std::string& filepath, const std::string& format) {
    std::string actualFormat = format;
    if (actualFormat == "auto") {
        if (!ConfigFormatParser::detectFormat(filepath, actualFormat)) {
            return false;
        }
    }
    
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    std::stringstream buffer;
    buffer << file.rdbuf();
    std::string content = buffer.str();
    
    auto configData = ConfigFormatParser::parse(content, actualFormat);
    
    ConfigAdvanced config;
    for (const auto& pair : configData) {
        config.set(pair.first, pair.second, ConfigSource::File);
    }
    
    configFiles[filepath] = std::move(config);
    
    if (pImpl->activeConfig) {
        pImpl->activeConfig->merge(configFiles[filepath]);
    }
    
    if (pImpl->autoSave) {
        pImpl->lastSave = std::chrono::system_clock::now();
    }
    
    return true;
}

bool ConfigManager::saveConfig(const std::string& filepath, const std::string& format) {
    std::string actualFormat = format.empty() ? pImpl->defaultFormat : format;
    
    auto it = configFiles.find(filepath);
    if (it == configFiles.end()) {
        return false;
    }
    
    std::string content = ConfigFormatParser::serialize(it->second.getAll(), actualFormat);
    
    std::ofstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    file << content;
    
    if (pImpl->autoSave) {
        pImpl->lastSave = std::chrono::system_clock::now();
    }
    
    return true;
}

bool ConfigManager::validateConfig(const std::string& filepath, std::vector<ConfigValidationError>& errors) {
    auto it = configFiles.find(filepath);
    if (it == configFiles.end()) {
        errors.emplace_back(ConfigValidationError::ErrorType::SchemaViolation, "Config file not found: " + filepath);
        return false;
    }
    
    bool isValid = it->second.validate();
    if (!isValid) {
        errors = it->second.getValidationErrors();
    }
    
    return isValid;
}

bool ConfigManager::mergeConfigs(const std::string& source, const std::string& target, bool overrideExisting) {
    auto sourceIt = configFiles.find(source);
    auto targetIt = configFiles.find(target);
    
    if (sourceIt == configFiles.end() || targetIt == configFiles.end()) {
        return false;
    }
    
    targetIt->second.merge(sourceIt->second, overrideExisting);
    return true;
}

bool ConfigManager::inheritConfig(const std::string& child, const std::string& parent, bool overrideDefaults) {
    auto childIt = configFiles.find(child);
    auto parentIt = configFiles.find(parent);
    
    if (childIt == configFiles.end() || parentIt == configFiles.end()) {
        return false;
    }
    
    childIt->second.inheritFrom(*parentIt, overrideDefaults);
    return true;
}

bool ConfigManager::applyTemplate(const std::string& templateName, const std::string& target) {
    auto targetIt = configFiles.find(target);
    if (targetIt == configFiles.end()) {
        return false;
    }
    
    // Find template in active config or in templates directory
    auto templateIt = configFiles.find(templateName);
    if (templateIt == configFiles.end() && pImpl->activeConfig) {
        templateIt = configFiles.find(templateName);
    }
    
    if (templateIt == configFiles.end()) {
        return false;
    }
    
    targetIt->second.applyTemplate(templateName);
    return true;
}

bool ConfigManager::createProfile(const std::string& profileName, const std::string& configName, const std::string& description) {
    auto configIt = configFiles.find(configName);
    if (configIt == configFiles.end()) {
        return false;
    }
    
    if (pImpl->activeConfig) {
        pImpl->activeConfig->createProfile(profileName, description);
    }
    
    return true;
}

bool ConfigManager::applyProfile(const std::string& profileName, const std::string& configName) {
    auto configIt = configFiles.find(configName);
    if (configIt == configFiles.end()) {
        return false;
    }
    
    if (pImpl->activeConfig) {
        pImpl->activeConfig->applyProfile(profileName);
    }
    
    return true;
}

bool ConfigManager::removeProfile(const std::string& profileName, const std::string& configName) {
    auto configIt = configFiles.find(configName);
    if (configIt == configFiles.end()) {
        return false;
    }
    
    if (pImpl->activeConfig) {
        pImpl->activeConfig->removeProfile(profileName);
    }
    
    return true;
}

std::vector<std::string> ConfigManager::getProfileNames(const std::string& configName) const {
    auto configIt = configFiles.find(configName);
    if (configIt == configFiles.end()) {
        return std::vector<std::string>();
    }
    
    if (pImpl->activeConfig) {
        return pImpl->activeConfig->getProfileNames();
    }
    
    return std::vector<std::string>();
}

std::vector<ConfigEntry> ConfigManager::getModificationHistory(const std::string& configName, size_t maxEntries) const {
    auto configIt = configFiles.find(configName);
    if (configIt == configFiles.end()) {
        return std::vector<ConfigEntry>();
    }
    
    if (pImpl->activeConfig) {
        return pImpl->activeConfig->getModificationHistory(maxEntries);
    }
    
    return std::vector<ConfigEntry>();
}

bool ConfigManager::enableAutoSave(bool enable, std::chrono::milliseconds interval) {
    pImpl->autoSave = enable;
    if (enable && interval.count() > 0) {
        pImpl->autoSaveInterval = interval;
    }
    return true;
}

bool ConfigManager::needsAutoSave() const {
    if (!pImpl->autoSave) return false;
    
    auto now = std::chrono::system_clock::now();
    auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(now - pImpl->lastSave);
    
    return elapsed >= pImpl->autoSaveInterval;
}

void ConfigManager::setDefaultFormat(const std::string& format) {
    pImpl->defaultFormat = format;
}

const std::string& ConfigManager::getDefaultFormat() const {
    return pImpl->defaultFormat;
}

ConfigAdvanced* ConfigManager::getConfig(const std::string& name) {
    auto it = configFiles.find(name);
    if (it == configFiles.end()) {
        return nullptr;
    }
    return &it->second;
}

const ConfigAdvanced* ConfigManager::getConfig(const std::string& name) const {
    auto it = configFiles.find(name);
    if (it == configFiles.end()) {
        return nullptr;
    }
    return &it->second;
}

std::vector<std::string> ConfigManager::getConfigNames() const {
    std::vector<std::string> names;
    names.reserve(configFiles.size());
    for (const auto& pair : configFiles) {
        names.push_back(pair.first);
    }
    return names;
}

void ConfigManager::clear() {
    configFiles.clear();
    if (pImpl->activeConfig) {
        pImpl->activeConfig->clear();
    }
}

bool ConfigManager::removeConfig(const std::string& name) {
    auto it = configFiles.find(name);
    if (it == configFiles.end()) {
        return false;
    }
    
    configFiles.erase(it);
    return true;
}

bool ConfigManager::setEnvironmentConfig(const std::string& environment, const std::string& configName) {
    auto configIt = configFiles.find(configName);
    if (configIt == configFiles.end()) {
        return false;
    }
    
    environmentConfigs[environment] = configName;
    return true;
}

const std::string& ConfigManager::getEnvironmentConfig(const std::string& environment) const {
    static const std::string empty;
    
    auto it = environmentConfigs.find(environment);
    if (it == environmentConfigs.end()) {
        return empty;
    }
    
    return it->second;
}

std::vector<std::string> ConfigManager::getEnvironmentConfigs() const {
    std::vector<std::string> configs;
    configs.reserve(environmentConfigs.size());
    for (const auto& pair : environmentConfigs) {
        configs.push_back(pair.second);
    }
    return configs;
}

void ConfigManager::setActiveConfig(const std::string& name) {
    auto it = configFiles.find(name);
    if (it != configFiles.end()) {
        if (pImpl->activeConfig) {
            pImpl->activeConfig->merge(it->second);
        }
    }
}

} // namespace nlm