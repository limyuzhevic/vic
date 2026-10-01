// PluginManager.cpp
// Main plugin loading and management system implementation

#include "PluginManager.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <chrono>
#include <filesystem>
#include <thread>

namespace nlm {

    // Implementation details for PluginRegistry
    struct PluginRegistry::Impl {
        std::map<std::string, RegistrationInfo> plugins;
        std::map<std::string, std::shared_ptr<PluginInterface>> loadedPlugins;
        std::map<std::string, PluginState> pluginStates;
        std::vector<DiscoveryCallback> discoveryCallbacks;
        std::vector<std::filesystem::path> searchPaths;
        mutable std::mutex mutex;
        std::map<std::string, std::vector<std::function<void(const std::string&, const std::any&)>>> eventListeners;
    };

    PluginRegistry::PluginRegistry() : pImpl(std::make_unique<Impl>()) {}
    PluginRegistry::~PluginRegistry() = default;

    bool PluginRegistry::discoverPlugins(const std::filesystem::path& directory) {
        std::lock_guard<std::mutex> lock(pImpl->mutex);
        scanDirectory(directory);
        return true;
    }

    bool PluginRegistry::discoverPlugins(const std::vector<std::filesystem::path>& directories) {
        std::lock_guard<std::mutex> lock(pImpl->mutex);
        for (const auto& dir : directories) {
            scanDirectory(dir);
        }
        return true;
    }

    void PluginRegistry::scanDirectory(const std::filesystem::path& directory, bool recursive) {
        if (!std::filesystem::exists(directory) || !std::filesystem::is_directory(directory)) {
            return;
        }

        for (const auto& entry : std::filesystem::directory_iterator(directory)) {
            if (entry.is_regular_file()) {
                const auto& path = entry.path();
                
                // Check file extension
                std::string ext = path.extension().string();
                std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);
                
                if (ext == ".so" || ext == ".dll" || ext == ".dylib") {
                    registerPlugin(path);
                }
                else if (ext == ".py") {
                    // Try to import as Python module
                    std::string moduleName = path.stem().string();
                    registerPythonPlugin(moduleName);
                }
            }
            else if (recursive && entry.is_directory()) {
                scanDirectory(entry.path(), recursive);
            }
        }
    }

    bool PluginRegistry::registerPlugin(const std::filesystem::path& pluginPath) {
        if (!validatePluginPath(pluginPath)) {
            return false;
        }

        std::lock_guard<std::mutex> lock(pImpl->mutex);
        
        if (checkDuplicateRegistration(generatePluginName(pluginPath))) {
            return false;
        }

        RegistrationInfo info;
        info.path = pluginPath;
        info.name = generatePluginName(pluginPath);

        if (!loadDynamicLibrary(pluginPath, info)) {
            return false;
        }

        if (!validatePlugin(info)) {
            return false;
        }

        pImpl->plugins[info.name] = info;
        return true;
    }

    bool PluginRegistry::registerPythonPlugin(const std::string& moduleName) {
        std::lock_guard<std::mutex> lock(pImpl->mutex);
        
        if (checkDuplicateRegistration(moduleName)) {
            return false;
        }

        RegistrationInfo info;
        info.name = moduleName;
        info.path = moduleName + ".py";
        info.isPythonPlugin = true;

        if (!loadPythonPlugin(moduleName, info)) {
            return false;
        }

        if (!validatePlugin(info)) {
            return false;
        }

        pImpl->plugins[info.name] = info;
        return true;
    }

    bool PluginRegistry::unregisterPlugin(const std::string& name) {
        std::lock_guard<std::mutex> lock(pImpl->mutex);
        
        auto it = pImpl->plugins.find(name);
        if (it == pImpl->plugins.end()) {
            return false;
        }

        if (pImpl->loadedPlugins.find(name) != pImpl->loadedPlugins.end()) {
            return false; // Cannot unregister loaded plugins
        }

        pImpl->plugins.erase(it);
        return true;
    }

    std::shared_ptr<PluginInterface> PluginRegistry::getPlugin(const std::string& name) const {
        std::lock_guard<std::mutex> lock(pImpl->mutex);
        
        auto it = pImpl->loadedPlugins.find(name);
        if (it != pImpl->loadedPlugins.end()) {
            return it->second;
        }
        return nullptr;
    }

    std::shared_ptr<PluginInterface> PluginRegistry::getPluginByCapability(PluginCapability capability) const {
        std::lock_guard<std::mutex> lock(pImpl->mutex);
        
        for (const auto& pair : pImpl->loadedPlugins) {
            const auto& plugin = pair.second;
            const auto& info = pImpl->plugins.at(pair.first);
            
            if (std::find(info.capabilities.begin(), info.capabilities.end(), capability) != info.capabilities.end()) {
                return plugin;
            }
        }
        return nullptr;
    }

    std::vector<std::shared_ptr<PluginInterface>> PluginRegistry::getPluginsByCategory(const std::string& category) const {
        std::lock_guard<std::mutex> lock(pImpl->mutex);
        std::vector<std::shared_ptr<PluginInterface>> result;
        
        for (const auto& pair : pImpl->loadedPlugins) {
            const auto& info = pImpl->plugins.at(pair.first);
            if (info.category == category) {
                result.push_back(pair.second);
            }
        }
        return result;
    }

    std::vector<std::string> PluginRegistry::getPluginNames() const {
        std::lock_guard<std::mutex> lock(pImpl->mutex);
        std::vector<std::string> result;
        result.reserve(pImpl->plugins.size());
        
        for (const auto& pair : pImpl->plugins) {
            result.push_back(pair.first);
        }
        return result;
    }

    std::vector<std::string> PluginRegistry::getPluginNamesByCapability(PluginCapability capability) const {
        std::lock_guard<std::mutex> lock(pImpl->mutex);
        std::vector<std::string> result;
        
        for (const auto& pair : pImpl->plugins) {
            const auto& info = pair.second;
            if (std::find(info.capabilities.begin(), info.capabilities.end(), capability) != info.capabilities.end()) {
                result.push_back(pair.first);
            }
        }
        return result;
    }

    const RegistrationInfo* PluginRegistry::getPluginInfo(const std::string& name) const {
        std::lock_guard<std::mutex> lock(pImpl->mutex);
        
        auto it = pImpl->plugins.find(name);
        if (it != pImpl->plugins.end()) {
            return &it->second;
        }
        return nullptr;
    }

    std::vector<RegistrationInfo> PluginRegistry::getAllPluginInfo() const {
        std::lock_guard<std::mutex> lock(pImpl->mutex);
        std::vector<RegistrationInfo> result;
        result.reserve(pImpl->plugins.size());
        
        for (const auto& pair : pImpl->plugins) {
            result.push_back(pair.second);
        }
        return result;
    }

    bool PluginRegistry::isPluginRegistered(const std::string& name) const {
        std::lock_guard<std::mutex> lock(pImpl->mutex);
        return pImpl->plugins.find(name) != pImpl->plugins.end();
    }

    bool PluginRegistry::validatePlugin(const RegistrationInfo& info) {
        if (!validatePluginDependencies(info)) {
            return false;
        }
        
        if (!checkVersionCompatibility(info)) {
            return false;
        }
        
        if (!validatePluginSecurity(info)) {
            return false;
        }
        
        // Try to load the plugin to validate
        if (info.isPythonPlugin) {
            // Python plugin validation would go here
            return true;
        } else {
            // C++ plugin validation would go here
            return true;
        }
    }

    bool PluginRegistry::validatePluginDependencies(const RegistrationInfo& info) const {
        for (const auto& dep : info.dependencies) {
            if (pImpl->plugins.find(dep) == pImpl->plugins.end()) {
                return false;
            }
        }
        return true;
    }

    bool PluginRegistry::checkVersionCompatibility(const RegistrationInfo& info) const {
        CompatibilityInfo compatInfo;
        // Compatibility checking would go here
        return compatInfo.isCompatible("0.1.0"); // Current NLM version
    }

    bool PluginRegistry::loadPlugin(const std::string& name) {
        std::lock_guard<std::mutex> lock(pImpl->mutex);
        
        auto infoIt = pImpl->plugins.find(name);
        if (infoIt == pImpl->plugins.end()) {
            return false;
        }
        
        auto stateIt = pImpl->pluginStates.find(name);
        if (stateIt != pImpl->pluginStates.end() && stateIt->second == PluginState::LOADED) {
            return true; // Already loaded
        }

        auto plugin = createPluginInstance(infoIt->second);
        if (!plugin) {
            return false;
        }

        pImpl->loadedPlugins[name] = plugin;
        updatePluginState(name, PluginState::LOADED);
        notifyPluginLoaded(name);
        
        return true;
    }

    bool PluginRegistry::unloadPlugin(const std::string& name) {
        std::lock_guard<std::mutex> lock(pImpl->mutex);
        
        auto it = pImpl->loadedPlugins.find(name);
        if (it == pImpl->loadedPlugins.end()) {
            return false;
        }

        auto plugin = it->second;
        
        if (plugin->getState() == PluginState::RUNNING) {
            plugin->stop();
        }

        cleanupPluginResources(name);
        pImpl->loadedPlugins.erase(it);
        updatePluginState(name, PluginState::UNLOADED);
        notifyPluginUnloaded(name);
        
        return true;
    }

    bool PluginRegistry::configurePlugin(const std::string& name, const std::map<std::string, std::any>& config) {
        std::lock_guard<std::mutex> lock(pImpl->mutex);
        
        auto it = pImpl->loadedPlugins.find(name);
        if (it == pImpl->loadedPlugins.end()) {
            return false;
        }

        return it->second->configure(config);
    }

    bool PluginRegistry::initializePlugin(const std::string& name) {
        std::lock_guard<std::mutex> lock(pImpl->mutex);
        
        auto it = pImpl->loadedPlugins.find(name);
        if (it == pImpl->loadedPlugins.end()) {
            return false;
        }

        return it->second->initialize();
    }

    bool PluginRegistry::startPlugin(const std::string& name) {
        std::lock_guard<std::mutex> lock(pImpl->mutex);
        
        auto it = pImpl->loadedPlugins.find(name);
        if (it == pImpl->loadedPlugins.end()) {
            return false;
        }

        return it->second->start();
    }

    bool PluginRegistry::stopPlugin(const std::string& name) {
        std::lock_guard<std::mutex> lock(pImpl->mutex);
        
        auto it = pImpl->loadedPlugins.find(name);
        if (it == pImpl->loadedPlugins.end()) {
            return false;
        }

        return it->second->stop();
    }

    bool PluginRegistry::unloadPluginCompletely(const std::string& name) {
        std::lock_guard<std::mutex> lock(pImpl->mutex);
        
        auto it = pImpl->plugins.find(name);
        if (it == pImpl->plugins.end()) {
            return false;
        }

        unloadPlugin(name);
        pImpl->plugins.erase(it);
        return true;
    }

    PluginState PluginRegistry::getPluginState(const std::string& name) const {
        std::lock_guard<std::mutex> lock(pImpl->mutex);
        
        auto it = pImpl->pluginStates.find(name);
        if (it != pImpl->pluginStates.end()) {
            return it->second;
        }
        return PluginState::UNLOADED;
    }

    std::map<std::string, PluginState> PluginRegistry::getAllPluginStates() const {
        std::lock_guard<std::mutex> lock(pImpl->mutex);
        return pImpl->pluginStates;
    }

    void PluginRegistry::addDiscoveryCallback(DiscoveryCallback callback) {
        std::lock_guard<std::mutex> lock(pImpl->mutex);
        pImpl->discoveryCallbacks.push_back(callback);
    }

    void PluginRegistry::removeDiscoveryCallback(const DiscoveryCallback& callback) {
        std::lock_guard<std::mutex> lock(pImpl->mutex);
        pImpl->discoveryCallbacks.erase(
            std::remove(pImpl->discoveryCallbacks.begin(), pImpl->discoveryCallbacks.end(), callback),
            pImpl->discoveryCallbacks.end()
        );
    }

    void PluginRegistry::notifyPluginLoaded(const std::string& name) {
        std::lock_guard<std::mutex> lock(pImpl->mutex);
        
        for (const auto& callback : pImpl->discoveryCallbacks) {
            const auto& info = pImpl->plugins.at(name);
            callback(info);
        }
    }

    void PluginRegistry::notifyPluginUnloaded(const std::string& name) {
        std::lock_guard<std::mutex> lock(pImpl->mutex);
        // Implementation would go here
    }

    bool PluginRegistry::saveRegistry(const std::filesystem::path& filePath) const {
        std::lock_guard<std::mutex> lock(pImpl->mutex);
        // Serialization would go here
        return true;
    }

    bool PluginRegistry::loadRegistry(const std::filesystem::path& filePath) {
        std::lock_guard<std::mutex> lock(pImpl->mutex);
        // Deserialization would go here
        return true;
    }

    void PluginRegistry::clear() {
        std::lock_guard<std::mutex> lock(pImpl->mutex);
        pImpl->plugins.clear();
        pImpl->loadedPlugins.clear();
        pImpl->pluginStates.clear();
    }

    std::vector<std::string> PluginRegistry::searchPlugins(const std::string& query) const {
        std::lock_guard<std::mutex> lock(pImpl->mutex);
        std::vector<std::string> result;
        
        for (const auto& pair : pImpl->plugins) {
            const auto& info = pair.second;
            if (info.name.find(query) != std::string::npos || 
                info.description.find(query) != std::string::npos) {
                result.push_back(pair.first);
            }
        }
        return result;
    }

    std::vector<std::string> PluginRegistry::findPluginsByCategory(const std::string& category) const {
        std::lock_guard<std::mutex> lock(pImpl->mutex);
        std::vector<std::string> result;
        
        for (const auto& pair : pImpl->plugins) {
            const auto& info = pair.second;
            if (info.category == category) {
                result.push_back(pair.first);
            }
        }
        return result;
    }

    std::vector<std::string> PluginRegistry::findPluginsByDependency(const std::string& dependency) const {
        std::lock_guard<std::mutex> lock(pImpl->mutex);
        std::vector<std::string> result;
        
        for (const auto& pair : pImpl->plugins) {
            const auto& info = pair.second;
            if (std::find(info.dependencies.begin(), info.dependencies.end(), dependency) != info.dependencies.end()) {
                result.push_back(pair.first);
            }
        }
        return result;
    }

    size_t PluginRegistry::getPluginCount() const {
        std::lock_guard<std::mutex> lock(pImpl->mutex);
        return pImpl->plugins.size();
    }

    size_t PluginRegistry::getLoadedPluginCount() const {
        std::lock_guard<std::mutex> lock(pImpl->mutex);
        return pImpl->loadedPlugins.size();
    }

    std::map<std::string, std::string> PluginRegistry::getPluginStats() const {
        std::lock_guard<std::mutex> lock(pImpl->mutex);
        std::map<std::string, std::string> stats;
        
        for (const auto& pair : pImpl->plugins) {
            const auto& info = pair.second;
            stats[pair.first] = "Version: " + info.version.toString() + ", Category: " + info.category;
        }
        return stats;
    }

    void PluginRegistry::setPluginSearchPath(const std::vector<std::filesystem::path>& paths) {
        std::lock_guard<std::mutex> lock(pImpl->mutex);
        pImpl->searchPaths = paths;
    }

    const std::vector<std::filesystem::path>& PluginRegistry::getPluginSearchPath() const {
        std::lock_guard<std::mutex> lock(pImpl->mutex);
        return pImpl->searchPaths;
    }

    bool PluginRegistry::validatePluginPath(const std::filesystem::path& path) const {
        if (!std::filesystem::exists(path)) {
            return false;
        }
        
        // Check if it's a shared library
        std::string ext = path.extension().string();
        std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);
        
        return ext == ".so" || ext == ".dll" || ext == ".dylib" || ext == ".py";
    }

    bool PluginRegistry::validatePluginSecurity(const RegistrationInfo& info) const {
        // Basic security validation
        if (info.name.empty()) {
            return false;
        }
        
        // Check for potentially dangerous patterns in name/path
        if (info.name.find("..") != std::string::npos || info.name.find("/") != std::string::npos) {
            return false;
        }
        
        return true;
    }

    void PluginRegistry::lock() const {
        pImpl->mutex.lock();
    }

    void PluginRegistry::unlock() const {
        pImpl->mutex.unlock();
    }

    // Private helper methods implementation
    bool PluginRegistry::loadDynamicLibrary(const std::filesystem::path& path, RegistrationInfo& info) {
        // Implementation would go here - load the shared library and extract symbols
        return true;
    }

    bool PluginRegistry::loadPythonPlugin(const std::string& moduleName, RegistrationInfo& info) {
        // Implementation would go here - load the Python module
        return true;
    }

    void PluginRegistry::extractPluginMetadata(void* handle, RegistrationInfo& info) {
        // Implementation would go here - extract metadata from the loaded library
    }

    void PluginRegistry::extractPluginVersion(void* handle, Version& version) {
        // Implementation would go here - extract version from the loaded library
    }

    void PluginRegistry::extractPluginCapabilities(void* handle, std::vector<PluginCapability>& capabilities) {
        // Implementation would go here - extract capabilities from the loaded library
    }

    void PluginRegistry::extractCompatibilityInfo(void* handle, CompatibilityInfo& compatibility) {
        // Implementation would go here - extract compatibility info from the loaded library
    }

    std::shared_ptr<PluginInterface> PluginRegistry::createPluginInstance(const RegistrationInfo& info) {
        // Implementation would go here - create plugin instance using factory function
        return nullptr;
    }

    void PluginRegistry::cleanupPlugin(void* handle) {
        // Implementation would go here - unload the shared library
    }

    std::string PluginRegistry::generatePluginName(const std::string& path) const {
        std::filesystem::path p(path);
        std::string name = p.stem().string();
        
        // Remove any extension suffix
        if (name.find('.') != std::string::npos) {
            name = name.substr(0, name.find('.'));
        }
        
        return name;
    }

    bool PluginRegistry::checkDuplicateRegistration(const std::string& name) const {
        return pImpl->plugins.find(name) != pImpl->plugins.end();
    }

    void PluginRegistry::updatePluginState(const std::string& name, PluginState state) {
        pImpl->pluginStates[name] = state;
    }

    void PluginRegistry::triggerEvent(const std::string& eventType, const std::string& pluginName, const std::any& data) {
        // Implementation would go here
    }

} // namespace nlm