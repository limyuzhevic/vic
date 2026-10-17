#pragma once

#include "EnvironmentConfig.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <unordered_set>

namespace nlm {

struct EnvironmentConfig::Impl {
    std::unordered_map<std::string, ConfigAdvanced> environmentConfigs;
    std::unordered_map<std::string, std::vector<std::string>> environmentInheritance;
    std::string currentEnvironment;
    std::unordered_set<std::string> activeEnvironments;
    std::unordered_set<std::string> disabledEnvironments;
    std::chrono::system_clock::time_point lastEnvironmentUpdate;
};

EnvironmentConfig::EnvironmentConfig() : pImpl(std::make_unique<Impl>()) {
}

EnvironmentConfig::~EnvironmentConfig() = default;

EnvironmentConfig::EnvironmentConfig(EnvironmentConfig&& other) noexcept : 
    pImpl(std::move(other.pImpl)) {
}

EnvironmentConfig& EnvironmentConfig::operator=(EnvironmentConfig&& other) noexcept {
    if (this != &other) {
        pImpl = std::move(other.pImpl);
    }
    return *this;
}

bool EnvironmentConfig::createEnvironment(const std::string& name, const ConfigAdvanced& config, 
                                         bool inheritFromDefault) {
    std::lock_guard<std::mutex> lock(mutex);
    
    if (pImpl->environmentConfigs.find(name) != pImpl->environmentConfigs.end()) {
        return false;
    }
    
    pImpl->environmentConfigs[name] = config;
    pImpl->activeEnvironments.insert(name);
    
    if (inheritFromDefault && name != "default") {
        auto defaultIt = pImpl->environmentConfigs.find("default");
        if (defaultIt != pImpl->environmentConfigs.end()) {
            pImpl->environmentConfigs[name].inheritFrom(defaultIt->second);
        }
    }
    
    pImpl->lastEnvironmentUpdate = std::chrono::system_clock::now();
    return true;
}

bool EnvironmentConfig::applyEnvironment(const std::string& name) {
    std::lock_guard<std::mutex> lock(mutex);
    
    auto it = pImpl->environmentConfigs.find(name);
    if (it == pImpl->environmentConfigs.end() || pImpl->activeEnvironments.find(name) == pImpl->activeEnvironments.end()) {
        return false;
    }
    
    pImpl->currentEnvironment = name;
    return true;
}

bool EnvironmentConfig::removeEnvironment(const std::string& name) {
    std::lock_guard<std::mutex> lock(mutex);
    
    auto it = pImpl->environmentConfigs.find(name);
    if (it == pImpl->environmentConfigs.end()) {
        return false;
    }
    
    pImpl->environmentConfigs.erase(it);
    pImpl->activeEnvironments.erase(name);
    pImpl->disabledEnvironments.erase(name);
    
    if (pImpl->currentEnvironment == name) {
        pImpl->currentEnvironment.clear();
    }
    
    pImpl->lastEnvironmentUpdate = std::chrono::system_clock::now();
    return true;
}

bool EnvironmentConfig::setInheritance(const std::string& child, const std::string& parent) {
    std::lock_guard<std::mutex> lock(mutex);
    
    if (pImpl->environmentConfigs.find(child) == pImpl->environmentConfigs.end() || 
        pImpl->environmentConfigs.find(parent) == pImpl->environmentConfigs.end()) {
        return false;
    }
    
    pImpl->environmentInheritance[child].push_back(parent);
    return true;
}

const ConfigAdvanced* EnvironmentConfig::getEnvironment(const std::string& name) const {
    std::lock_guard<std::mutex> lock(mutex);
    
    auto it = pImpl->environmentConfigs.find(name);
    if (it == pImpl->environmentConfigs.end()) {
        return nullptr;
    }
    
    return &it->second;
}

ConfigAdvanced* EnvironmentConfig::getEnvironment(const std::string& name) {
    std::lock_guard<std::mutex> lock(mutex);
    
    auto it = pImpl->environmentConfigs.find(name);
    if (it == pImpl->environmentConfigs.end()) {
        return nullptr;
    }
    
    return &it->second;
}

std::vector<std::string> EnvironmentConfig::getEnvironments() const {
    std::lock_guard<std::mutex> lock(mutex);
    std::vector<std::string> environments;
    environments.reserve(pImpl->environmentConfigs.size());
    for (const auto& pair : pImpl->environmentConfigs) {
        environments.push_back(pair.first);
    }
    return environments;
}

std::vector<std::string> EnvironmentConfig::getActiveEnvironments() const {
    std::lock_guard<std::mutex> lock(mutex);
    std::vector<std::string> activeEnvs;
    activeEnvs.reserve(pImpl->activeEnvironments.size());
    for (const auto& env : pImpl->activeEnvironments) {
        activeEnvs.push_back(env);
    }
    return activeEnvs;
}

std::vector<std::string> EnvironmentConfig::getDisabledEnvironments() const {
    std::lock_guard<std::mutex> lock(mutex);
    std::vector<std::string> disabledEnvs;
    disabledEnvs.reserve(pImpl->disabledEnvironments.size());
    for (const auto& env : pImpl->disabledEnvironments) {
        disabledEnvs.push_back(env);
    }
    return disabledEnvs;
}

bool EnvironmentConfig::enableEnvironment(const std::string& name) {
    std::lock_guard<std::mutex> lock(mutex);
    
    auto it = pImpl->environmentConfigs.find(name);
    if (it == pImpl->environmentConfigs.end()) {
        return false;
    }
    
    pImpl->activeEnvironments.insert(name);
    pImpl->disabledEnvironments.erase(name);
    pImpl->lastEnvironmentUpdate = std::chrono::system_clock::now();
    return true;
}

bool EnvironmentConfig::disableEnvironment(const std::string& name) {
    std::lock_guard<std::mutex> lock(mutex);
    
    auto it = pImpl->environmentConfigs.find(name);
    if (it == pImpl->environmentConfigs.end()) {
        return false;
    }
    
    pImpl->activeEnvironments.erase(name);
    pImpl->disabledEnvironments.insert(name);
    
    if (pImpl->currentEnvironment == name) {
        pImpl->currentEnvironment.clear();
    }
    
    pImpl->lastEnvironmentUpdate = std::chrono::system_clock::now();
    return true;
}

const std::string& EnvironmentConfig::getCurrentEnvironment() const {
    return pImpl->currentEnvironment;
}

std::vector<std::string> EnvironmentConfig::getParentEnvironments(const std::string& child) const {
    std::lock_guard<std::mutex> lock(mutex);
    
    auto it = pImpl->environmentInheritance.find(child);
    if (it == pImpl->environmentInheritance.end()) {
        return std::vector<std::string>();
    }
    
    return it->second;
}

bool EnvironmentConfig::loadEnvironmentsFromFile(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    std::stringstream buffer;
    buffer << file.rdbuf();
    std::string content = buffer.str();
    
    // Parse JSON content
    auto data = ConfigFormatParser::parse(content, "json");
    
    // Process environments (simplified - in real implementation would be more complex)
    for (const auto& pair : data) {
        // Create environment config from data
        ConfigAdvanced envConfig;
        envConfig.set(pair.first, pair.second, ConfigSource::Default);
        
        // Find environment name in key or use default
        std::string envName = "env_" + std::to_string(pImpl->environmentConfigs.size());
        createEnvironment(envName, envConfig);
    }
    
    return true;
}

bool EnvironmentConfig::saveEnvironmentsToFile(const std::string& filepath) {
    std::lock_guard<std::mutex> lock(mutex);
    
    std::unordered_map<std::string, ConfigValue> allData;
    
    for (const auto& pair : pImpl->environmentConfigs) {
        // In real implementation, would serialize each environment individually
        // For now, create a combined representation
        for (const auto& entry : pair.second.getAll()) {
            allData[entry.first] = entry.second;
        }
    }
    
    std::string content = ConfigFormatParser::serialize(allData, "json");
    
    std::ofstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    file << content;
    return true;
}

std::chrono::system_clock::time_point EnvironmentConfig::getLastUpdateTime() const {
    return pImpl->lastEnvironmentUpdate;
}

} // namespace nlm