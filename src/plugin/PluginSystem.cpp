#pragma once

#include "PluginSystem.hpp"
#include <fstream>
#include <sstream>
#include <filesystem>
#include <iostream>
#include <algorithm>
#include <chrono>
#include <thread>
#include <mutex>
#include <unordered_map>

namespace nlm {

// Plugin manager implementation
struct PluginManager::Impl {
    std::map<std::string, std::shared_ptr<IPlugin>> plugins;
    std::map<std::string, PluginInfo> pluginInfos;
    std::map<std::string, std::unique_ptr<IPluginFactory>> factories;
    PluginConfig pluginConfig;
    std::map<std::string, std::string> loadErrors;
    bool initialized;
    mutable std::mutex mutex;
    
    Impl() : initialized(false) {}
};

PluginManager::PluginManager() : pImpl(std::make_unique<Impl>()) {}

PluginManager::~PluginManager() {
    shutdownAll();
}

bool PluginManager::loadPlugin(const std::string& pluginPath) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    if (pImpl->plugins.find(pluginPath) != pImpl->plugins.end()) {
        return true; // Already loaded
    }
    
    // Check if file exists and is a plugin
    if (!std::filesystem::exists(pluginPath) || !PluginUtils::isPluginFile(pluginPath)) {
        pImpl->loadErrors[pluginPath] = "File does not exist or is not a plugin";
        return false;
    }
    
    // Try to load the plugin using registered factories
    // For now, just add as unloaded plugin
    pImpl->plugins[pluginPath] = nullptr;
    
    return true;
}

bool PluginManager::loadPluginsFromDirectory(const std::string& directoryPath) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    if (!std::filesystem::exists(directoryPath) || !std::filesystem::is_directory(directoryPath)) {
        return false;
    }
    
    bool success = true;
    for (const auto& entry : std::filesystem::directory_iterator(directoryPath)) {
        if (entry.is_regular_file() && PluginUtils::isPluginFile(entry.path().string())) {
            if (!loadPlugin(entry.path().string())) {
                success = false;
            }
        }
    }
    
    return success;
}

bool PluginManager::unloadPlugin(const std::string& pluginName) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    auto it = pImpl->plugins.find(pluginName);
    if (it == pImpl->plugins.end()) {
        return false;
    }
    
    if (it->second) {
        it->second->shutdown();
    }
    
    pImpl->plugins.erase(it);
    pImpl->pluginInfos.erase(pluginName);
    
    return true;
}

bool PluginManager::enablePlugin(const std::string& pluginName) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    auto it = pImpl->plugins.find(pluginName);
    if (it == pImpl->plugins.end()) {
        return false;
    }
    
    if (it->second) {
        it->second->setEnabled(true);
    }
    
    return true;
}

bool PluginManager::disablePlugin(const std::string& pluginName) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    auto it = pImpl->plugins.find(pluginName);
    if (it == pImpl->plugins.end()) {
        return false;
    }
    
    if (it->second) {
        it->second->setEnabled(false);
    }
    
    return true;
}

std::shared_ptr<IPlugin> PluginManager::getPlugin(const std::string& pluginName) const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    auto it = pImpl->plugins.find(pluginName);
    if (it != pImpl->plugins.end()) {
        return it->second;
    }
    return nullptr;
}

std::vector<std::shared_ptr<IPlugin>> PluginManager::getLoadedPlugins() const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    std::vector<std::shared_ptr<IPlugin>> loadedPlugins;
    for (const auto& pair : pImpl->plugins) {
        if (pair.second && pair.second->isEnabled()) {
            loadedPlugins.push_back(pair.second);
        }
    }
    return loadedPlugins;
}

std::vector<std::shared_ptr<IPlugin>> PluginManager::getPluginsByType(PluginType type) const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    std::vector<std::shared_ptr<IPlugin>> plugins;
    for (const auto& pair : pImpl->plugins) {
        if (pair.second && pair.second->isEnabled()) {
            const PluginInfo& info = pair.second->getInfo();
            if (info.type == type) {
                plugins.push_back(pair.second);
            }
        }
    }
    return plugins;
}

bool PluginManager::isPluginLoaded(const std::string& pluginName) const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    return pImpl->plugins.find(pluginName) != pImpl->plugins.end();
}

std::optional<PluginInfo> PluginManager::getPluginInfo(const std::string& pluginName) const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    auto it = pImpl->pluginInfos.find(pluginName);
    if (it != pImpl->pluginInfos.end()) {
        return it->second;
    }
    return std::nullopt;
}

bool PluginManager::initializeAll(const Config& config) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    if (pImpl->initialized) {
        return true; // Already initialized
    }
    
    bool success = true;
    for (auto& pair : pImpl->plugins) {
        if (pair.second && pair.second->isEnabled()) {
            if (!pair.second->initialize(config)) {
                success = false;
            }
        }
    }
    
    if (success) {
        pImpl->initialized = true;
    }
    
    return success;
}

void PluginManager::shutdownAll() {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    if (!pImpl->initialized) {
        return;
    }
    
    for (auto& pair : pImpl->plugins) {
        if (pair.second) {
            pair.second->shutdown();
        }
    }
    
    pImpl->initialized = false;
}

bool PluginManager::validateDependencies(const PluginInfo& plugin, 
                                         const std::map<std::string, PluginInfo>& loadedPlugins) const {
    for (const auto& depName : plugin.dependencies) {
        if (loadedPlugins.find(depName) == loadedPlugins.end()) {
            return false;
        }
    }
    return true;
}

std::string PluginManager::exportPluginList() const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    std::ostringstream oss;
    oss << "Plugin List (" << pImpl->plugins.size() << " plugins):\n";
    
    for (const auto& pair : pImpl->plugins) {
        oss << "  " << pair.first << " - ";
        if (pair.second) {
            const PluginInfo& info = pair.second->getInfo();
            oss << info.name << " " << info.version << " (";
            switch (info.type) {
                case PluginType::World: oss << "World"; break;
                case PluginType::AgentBrain: oss << "AgentBrain"; break;
                case PluginType::NeuralArchitecture: oss << "NeuralArchitecture"; break;
                case PluginType::SensoryProcessor: oss << "SensoryProcessor"; break;
                case PluginType::MotorDecoder: oss << "MotorDecoder"; break;
                case PluginType::Neuromodulator: oss << "Neuromodulator"; break;
                case PluginType::MemorySystem: oss << "MemorySystem"; break;
                case PluginType::PlasticityRule: oss << "PlasticityRule"; break;
                case PluginType::DevelopmentSystem: oss << "DevelopmentSystem"; break;
                case PluginType::Utility: oss << "Utility"; break;
            }
            oss << ")";
        } else {
            oss << "(unloaded)";
        }
        oss << "\n";
    }
    
    return oss.str();
}

bool PluginManager::importPluginConfig(const std::string& configJson) {
    try {
        auto config = PluginUtils::jsonToConfig(configJson);
        // For now, just parse and validate
        return !config.empty();
    } catch (...) {
        return false;
    }
}

std::string PluginManager::exportPluginConfig(const std::string& pluginName) const {
    auto configOpt = pImpl->pluginConfig.getPluginConfig(pluginName);
    if (!configOpt) {
        return "{}";
    }
    
    return PluginUtils::configToJson(*configOpt);
}

bool PluginManager::reloadPlugin(const std::string& pluginName, const std::string& pluginPath) {
    unloadPlugin(pluginName);
    return loadPlugin(pluginPath);
}

std::map<std::string, std::string> PluginManager::getLoadErrors() const {
    return pImpl->loadErrors;
}

void PluginManager::clearLoadErrors() {
    pImpl->loadErrors.clear();
}

bool PluginManager::hasErrors() const {
    return !pImpl->loadErrors.empty();
}

// Plugin registry implementation
struct PluginRegistry::Impl {
    std::map<std::string, std::shared_ptr<IPluginFactory>> factories;
    std::map<std::string, PluginInfo> pluginInfos;
    std::map<PluginType, std::shared_ptr<IPluginFactory>> factoriesByType;
    
    Impl() {}
};

PluginRegistry::PluginRegistry() : pImpl(std::make_unique<Impl>()) {}

PluginRegistry::~PluginRegistry() = default;

bool PluginRegistry::registerFactory(std::unique_ptr<IPluginFactory> factory) {
    if (!factory) {
        return false;
    }
    
    PluginInfo info = factory->getInfo();
    
    // Check if factory name is valid
    if (info.name.empty()) {
        return false;
    }
    
    pImpl->factories[info.name] = factory;
    pImpl->factoriesByType[info.type] = factory;
    pImpl->pluginInfos[info.name] = info;
    
    return true;
}

std::shared_ptr<IPluginFactory> PluginRegistry::getFactory(const std::string& pluginName) const {
    auto it = pImpl->factories.find(pluginName);
    if (it != pImpl->factories.end()) {
        return it->second;
    }
    return nullptr;
}

std::shared_ptr<IPluginFactory> PluginRegistry::getFactory(PluginType type) const {
    auto it = pImpl->factoriesByType.find(type);
    if (it != pImpl->factoriesByType.end()) {
        return it->second;
    }
    return nullptr;
}

std::vector<std::shared_ptr<IPluginFactory>> PluginRegistry::getAllFactories() const {
    std::vector<std::shared_ptr<IPluginFactory>> factories;
    factories.reserve(pImpl->factories.size());
    for (const auto& pair : pImpl->factories) {
        factories.push_back(pair.second);
    }
    return factories;
}

std::optional<PluginInfo> PluginRegistry::getPluginInfo(const std::string& pluginName) const {
    auto it = pImpl->pluginInfos.find(pluginName);
    if (it != pImpl->pluginInfos.end()) {
        return it->second;
    }
    return std::nullopt;
}

std::vector<PluginInfo> PluginRegistry::getPluginsByType(PluginType type) const {
    std::vector<PluginInfo> plugins;
    for (const auto& pair : pImpl->pluginInfos) {
        if (pair.second.type == type) {
            plugins.push_back(pair.second);
        }
    }
    return plugins;
}

bool PluginRegistry::isPluginSupported(const std::string& pluginName) const {
    return pImpl->factories.find(pluginName) != pImpl->factories.end();
}

std::vector<PluginType> PluginRegistry::getSupportedTypes() const {
    std::vector<PluginType> types;
    for (const auto& pair : pImpl->factoriesByType) {
        types.push_back(pair.first);
    }
    return types;
}

std::vector<std::unique_ptr<IPluginFactory>> PluginRegistry::getDefaultFactories() {
    std::vector<std::unique_ptr<IPluginFactory>> factories;
    
    // TODO: Implement default factories
    // This would include built-in plugins for world, agent brain, etc.
    
    return factories;
}

void PluginRegistry::clear() {
    pImpl->factories.clear();
    pImpl->pluginInfos.clear();
    pImpl->factoriesByType.clear();
}

// Plugin configuration implementation
struct PluginConfig::Impl {
    std::map<std::string, std::map<std::string, std::variant<int, double, std::string, bool>>> configs;
    std::map<std::string, std::map<std::string, std::string>> metadata;
    
    Impl() {}
};

PluginConfig::PluginConfig() : pImpl(std::make_unique<Impl>()) {}

PluginConfig::~PluginConfig() = default;

bool PluginConfig::loadFromFile(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    try {
        std::string content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
        // TODO: Parse JSON/YAML content
        // For now, return false
        return false;
    } catch (...) {
        return false;
    }
}

bool PluginConfig::saveToFile(const std::string& filepath) const {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    try {
        // TODO: Write JSON/YAML content
        return true;
    } catch (...) {
        return false;
    }
}

std::optional<std::map<std::string, std::variant<int, double, std::string, bool>>> 
PluginConfig::getPluginConfig(const std::string& pluginName) const {
    auto it = pImpl->configs.find(pluginName);
    if (it != pImpl->configs.end()) {
        return it->second;
    }
    return std::nullopt;
}

bool PluginConfig::setPluginConfig(const std::string& pluginName, 
                                   const std::map<std::string, std::variant<int, double, std::string, bool>>& config) {
    pImpl->configs[pluginName] = config;
    return true;
}

bool PluginConfig::removePluginConfig(const std::string& pluginName) {
    auto it = pImpl->configs.find(pluginName);
    if (it != pImpl->configs.end()) {
        pImpl->configs.erase(it);
        return true;
    }
    return false;
}

bool PluginConfig::hasPluginConfig(const std::string& pluginName) const {
    return pImpl->configs.find(pluginName) != pImpl->configs.end();
}

std::map<std::string, std::map<std::string, std::variant<int, double, std::string, bool>>> 
PluginConfig::getAllPluginConfigs() const {
    return pImpl->configs;
}

void PluginConfig::clear() {
    pImpl->configs.clear();
    pImpl->metadata.clear();
}

std::map<std::string, std::string> PluginConfig::getPluginMetadata(const std::string& pluginName) const {
    auto it = pImpl->metadata.find(pluginName);
    if (it != pImpl->metadata.end()) {
        return it->second;
    }
    return std::map<std::string, std::string>();
}

bool PluginConfig::setPluginMetadata(const std::string& pluginName,
                                     const std::map<std::string, std::string>& metadata) {
    pImpl->metadata[pluginName] = metadata;
    return true;
}

// Plugin utilities namespace
namespace PluginUtils {
    bool isValidPluginName(const std::string& name) {
        if (name.empty()) return false;
        for (char c : name) {
            if (!std::isalnum(c) && c != '_' && c != '-') {
                return false;
            }
        }
        return true;
    }
    
    std::string sanitizePluginName(const std::string& name) {
        std::string result;
        for (char c : name) {
            if (std::isalnum(c)) {
                result += c;
            } else {
                result += '_';
            }
        }
        return result;
    }
    
    std::string parseVersion(const std::string& versionString) {
        // Simple version parsing - look for semantic version pattern
        std::regex versionPattern(R"((\d+)\.(\d+)\.(\d+))");
        std::smatch matches;
        if (std::regex_search(versionString, matches, versionPattern)) {
            return versionString;
        }
        return "0.0.0";
    }
    
    bool compareVersions(const std::string& version1, const std::string& version2) {
        // Simple version comparison - assume semantic versioning
        std::vector<int> v1, v2;
        
        std::regex versionPattern(R"((\d+))");
        auto words_begin1 = std::sregex_iterator(version1.begin(), version1.end(), versionPattern);
        auto words_end1 = std::sregex_iterator();
        for (std::sregex_iterator i = words_begin1; i != words_end1; ++i) {
            std::smatch match = *i;
            v1.push_back(std::stoi(match[1].str()));
        }
        
        auto words_begin2 = std::sregex_iterator(version2.begin(), version2.end(), versionPattern);
        auto words_end2 = std::sregex_iterator();
        for (std::sregex_iterator i = words_begin2; i != words_end2; ++i) {
            std::smatch match = *i;
            v2.push_back(std::stoi(match[1].str()));
        }
        
        for (size_t i = 0; i < std::max(v1.size(), v2.size()); ++i) {
            int val1 = (i < v1.size()) ? v1[i] : 0;
            int val2 = (i < v2.size()) ? v2[i] : 0;
            
            if (val1 < val2) return false;
            if (val1 > val2) return true;
        }
        
        return true;
    }
    
    std::string getCurrentPlatform() {
    #ifdef _WIN32
        return "windows";
    #elif __APPLE__ || __MACH__
        return "macos";
    #elif __linux__
        return "linux";
    #elif __unix__
        return "unix";
    #else
        return "unknown";
    #endif
    }
    
    std::string getPluginExtension() {
        std::string platform = getCurrentPlatform();
        if (platform == "windows") {
            return ".dll";
        } else if (platform == "linux") {
            return ".so";
        } else if (platform == "macos") {
            return ".dylib";
        }
        return ".plugin";
    }
    
    bool isPluginFile(const std::string& filepath) {
        if (!std::filesystem::exists(filepath)) {
            return false;
        }
        
        std::string extension = std::filesystem::path(filepath).extension().string();
        std::transform(extension.begin(), extension.end(), extension.begin(), ::tolower);
        
        return extension == ".dll" || extension == ".so" || extension == ".dylib" || extension == ".plugin";
    }
    
    std::unique_ptr<IPlugin> loadPluginFromMemory(const std::vector<uint8_t>& data) {
        // TODO: Implement plugin loading from memory
        // This would typically involve deserializing the plugin from the memory buffer
        return nullptr;
    }
    
    std::string configToJson(const std::map<std::string, std::variant<int, double, std::string, bool>>& config) {
        // TODO: Implement JSON conversion
        // This would use a JSON library like nlohmann::json
        return "{}";
    }
    
    std::map<std::string, std::variant<int, double, std::string, bool>> 
    jsonToConfig(const std::string& jsonString) {
        // TODO: Implement JSON parsing
        // This would use a JSON library like nlohmann::json
        return std::map<std::string, std::variant<int, double, std::string, bool>>();
    }
}

} // namespace nlm

// Explicit template instantiations for PluginManager
template std::shared_ptr<IPlugin> PluginManager::getPlugin(const std::string&) const;
template std::vector<std::shared_ptr<IPlugin>> PluginManager::getPluginsByType(PluginType) const;
template bool PluginManager::isPluginLoaded(const std::string&) const;

// Explicit template instantiations for PluginRegistry
template std::shared_ptr<IPluginFactory> PluginRegistry::getFactory(const std::string&) const;
template std::shared_ptr<IPluginFactory> PluginRegistry::getFactory(PluginType) const;

// Explicit template instantiations for PluginConfig
template std::optional<std::map<std::string, std::variant<int, double, std::string, bool>>> 
PluginConfig::getPluginConfig(const std::string&) const;
template bool PluginConfig::setPluginConfig(const std::string&, 
                                          const std::map<std::string, std::variant<int, double, std::string, bool>>&);