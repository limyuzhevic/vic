// Plugin System - Implementation File
#include "PluginSystem.hpp"
#include <fstream>
#include <sstream>
#include <regex>
#include <algorithm>
#include <filesystem>

namespace nlm {

// PluginManager implementation
bool PluginManager::loadPlugin(const std::string& path) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    std::string name = extractPluginName(path);
    if (plugins_.find(name) != plugins_.end()) {
        lastError_ = "Plugin already loaded: " + name;
        return false;
    }
    
    if (!loadPluginFile(path)) {
        return false;
    }
    
    plugins_[name] = PluginEntry{};
    plugins_[name].enabled = true;
    plugins_[name].filePath = path;
    plugins_[name].lastLoaded = "now"; // In real implementation, would use actual timestamp
    
    return true;
}

bool PluginManager::unloadPlugin(const std::string& name) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    auto it = plugins_.find(name);
    if (it == plugins_.end()) {
        lastError_ = "Plugin not found: " + name;
        return false;
    }
    
    // Shutdown the plugin if it has one
    if (it->second.neuralModel) {
        it->second.neuralModel->shutdown();
    }
    if (it->second.plasticityRule) {
        // In real implementation, would call shutdown on plasticity rule
    }
    
    plugins_.erase(it);
    return true;
}

bool PluginManager::reloadPlugin(const std::string& name) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    auto it = plugins_.find(name);
    if (it == plugins_.end()) {
        lastError_ = "Plugin not found: " + name;
        return false;
    }
    
    // Shutdown current plugin
    if (it->second.neuralModel) {
        it->second.neuralModel->shutdown();
    }
    
    // Unload and reload
    if (!unloadPlugin(name) || !loadPlugin(it->second.filePath)) {
        return false;
    }
    
    return true;
}

std::vector<std::string> PluginManager::getLoadedPlugins() const {
    std::lock_guard<std::mutex> lock(mutex_);
    std::vector<std::string> names;
    for (const auto& pair : plugins_) {
        names.push_back(pair.first);
    }
    return names;
}

template<typename T>
std::shared_ptr<T> PluginManager::getPlugin(const std::string& name) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    auto it = plugins_.find(name);
    if (it == plugins_.end()) {
        lastError_ = "Plugin not found: " + name;
        return nullptr;
    }
    
    // In real implementation, would type-check the returned plugin
    return nullptr; // Placeholder
}

bool PluginManager::registerPlugin(std::shared_ptr<NeuralModelPlugin> plugin) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    if (!plugin) {
        lastError_ = "Invalid plugin pointer";
        return false;
    }
    
    std::string name = plugin->getName();
    if (plugins_.find(name) != plugins_.end()) {
        lastError_ = "Plugin already registered: " + name;
        return false;
    }
    
    plugins_[name].neuralModel = plugin;
    plugins_[name].enabled = true;
    
    return true;
}

std::vector<std::shared_ptr<PluggableComponent>> PluginManager::getComponents() const {
    std::lock_guard<std::mutex> lock(mutex_);
    std::vector<std::shared_ptr<PluggableComponent>> components;
    for (const auto& pair : plugins_) {
        if (pair.second.neuralModel) {
            components.push_back(pair.second.neuralModel);
        }
        // In real implementation, would add other component types
    }
    return components;
}

PluginManager::PluginInfo PluginManager::getPluginInfo(const std::string& name) const {
    std::lock_guard<std::mutex> lock(mutex_);
    PluginInfo info;
    
    auto it = plugins_.find(name);
    if (it == plugins_.end()) {
        lastError_ = "Plugin not found: " + name;
        return info;
    }
    
    info.name = name;
    // In real implementation, would extract other info from plugin
    info.enabled = it->second.enabled;
    info.config = it->second.config;
    info.filePath = it->second.filePath;
    
    return info;
}

void PluginManager::enableAllPlugins() {
    std::lock_guard<std::mutex> lock(mutex_);
    for (auto& pair : plugins_) {
        pair.second.enabled = true;
    }
}

void PluginManager::disableAllPlugins() {
    std::lock_guard<std::mutex> lock(mutex_);
    for (auto& pair : plugins_) {
        pair.second.enabled = false;
    }
}

void PluginManager::initializeAllPlugins() {
    std::lock_guard<std::mutex> lock(mutex_);
    for (auto& pair : plugins_) {
        if (pair.second.neuralModel && pair.second.enabled) {
            if (!pair.second.neuralModel->initialize(pair.second.config)) {
                lastError_ = "Failed to initialize plugin: " + pair.first;
            }
        }
    }
}

void PluginManager::shutdownAllPlugins() {
    std::lock_guard<std::mutex> lock(mutex_);
    for (auto& pair : plugins_) {
        if (pair.second.neuralModel) {
            pair.second.neuralModel->shutdown();
        }
    }
}

bool PluginManager::savePluginState(const std::string& filename) const {
    std::lock_guard<std::mutex> lock(mutex_);
    std::ofstream file(filename);
    if (!file.is_open()) {
        lastError_ = "Cannot open file for writing: " + filename;
        return false;
    }
    
    // In real implementation, would serialize plugin state
    file << "Plugin state saved successfully" << std::endl;
    return true;
}

bool PluginManager::loadPluginState(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        lastError_ = "Cannot open file for reading: " + filename;
        return false;
    }
    
    // In real implementation, would deserialize plugin state
    std::string line;
    while (std::getline(file, line)) {
        // Parse state (simplified)
    }
    return true;
}

std::string PluginManager::exportPluginState() const {
    std::lock_guard<std::mutex> lock(mutex_);
    std::ostringstream oss;
    oss << "Plugin State:" << std::endl;
    
    for (const auto& pair : plugins_) {
        oss << "Plugin: " << pair.first << std::endl;
        oss << "  Enabled: " << (pair.second.enabled ? "true" : "false") << std::endl;
        oss << "  Config: " << pair.second.config.size() << " entries" << std::endl;
    }
    
    return oss.str();
}

bool PluginManager::importPluginState(const std::string& state) {
    std::lock_guard<std::mutex> lock(mutex_);
    // In real implementation, would parse state and restore plugins
    return true;
}

bool PluginManager::enablePlugin(const std::string& name) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    auto it = plugins_.find(name);
    if (it == plugins_.end()) {
        lastError_ = "Plugin not found: " + name;
        return false;
    }
    
    it->second.enabled = true;
    return true;
}

bool PluginManager::disablePlugin(const std::string& name) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    auto it = plugins_.find(name);
    if (it == plugins_.end()) {
        lastError_ = "Plugin not found: " + name;
        return false;
    }
    
    it->second.enabled = false;
    return true;
}

bool PluginManager::isPluginEnabled(const std::string& name) const {
    std::lock_guard<std::mutex> lock(mutex_);
    
    auto it = plugins_.find(name);
    if (it == plugins_.end()) {
        return false;
    }
    
    return it->second.enabled;
}

bool PluginManager::configurePlugin(const std::string& name, const std::map<std::string, std::string>& config) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    auto it = plugins_.find(name);
    if (it == plugins_.end()) {
        lastError_ = "Plugin not found: " + name;
        return false;
    }
    
    it->second.config = config;
    return true;
}

std::map<std::string, std::string> PluginManager::getPluginConfig(const std::string& name) const {
    std::lock_guard<std::mutex> lock(mutex_);
    
    auto it = plugins_.find(name);
    if (it == plugins_.end()) {
        lastError_ = "Plugin not found: " + name;
        return std::map<std::string, std::string>();
    }
    
    return it->second.config;
}

size_t PluginManager::getPluginCount() const {
    std::lock_guard<std::mutex> lock(mutex_);
    return plugins_.size();
}

size_t PluginManager::getEnabledPluginCount() const {
    std::lock_guard<std::mutex> lock(mutex_);
    size_t count = 0;
    for (const auto& pair : plugins_) {
        if (pair.second.enabled) {
            count++;
        }
    }
    return count;
}

bool PluginManager::loadPluginFile(const std::string& path) {
    // Check if file exists
    if (!std::filesystem::exists(path)) {
        lastError_ = "Plugin file not found: " + path;
        return false;
    }
    
    // Check if it's a valid plugin file
    if (!isPluginFile(path)) {
        lastError_ = "Invalid plugin file format: " + path;
        return false;
    }
    
    // In real implementation, would load the plugin dynamically
    // For now, just return true as a placeholder
    return true;
}

void PluginManager::discoverPlugins() {
    // In real implementation, would scan directories for plugins
}

std::vector<std::string> PluginManager::scanDirectory(const std::string& directory) const {
    std::vector<std::string> plugins;
    if (!std::filesystem::exists(directory)) {
        return plugins;
    }
    
    for (const auto& entry : std::filesystem::directory_iterator(directory)) {
        if (entry.is_regular_file() && isPluginFile(entry.path().string())) {
            plugins.push_back(entry.path().string());
        }
    }
    
    return plugins;
}

bool PluginManager::isPluginFile(const std::string& filename) const {
    // Check file extension
    std::string extension = plugin_utils::getPluginTypeString(filename);
    return (extension == "so" || extension == "dll" || extension == "dylib" ||
            extension == "nlm" || extension == "plugin");
}

bool PluginManager::validatePlugin(const PluginEntry& entry) const {
    if (!entry.neuralModel) {
        lastError_ = "Plugin has invalid neural model component";
        return false;
    }
    
    if (entry.neuralModel->getName().empty()) {
        lastError_ = "Plugin has empty name";
        return false;
    }
    
    if (entry.neuralModel->getVersion().empty()) {
        lastError_ = "Plugin has empty version";
        return false;
    }
    
    return true;
}

std::string PluginManager::extractPluginName(const std::string& path) const {
    std::filesystem::path fsPath(path);
    std::string stem = fsPath.stem().string();
    
    // Remove common extensions
    if (stem.size() > 3 && stem.substr(stem.size() - 3) == "_plugin") {
        stem = stem.substr(0, stem.size() - 7);
    }
    
    return stem;
}

std::string PluginManager::getPluginCategory(const std::string& path) const {
    // In real implementation, would read plugin manifest or header
    return "unknown";
}

// PluginNeuralFactory implementation
std::shared_ptr<NeuralModelPlugin> PluginNeuralFactory::createNeuralModel(const std::string& pluginName) {
    return pluginManager_.getPlugin<NeuralModelPlugin>(pluginName);
}

void PluginNeuralFactory::registerNeuralModel(const std::string& name, const std::string& pluginName) {
    ComponentRegistry registry;
    registry.pluginName = pluginName;
    registry.enabled = true;
    
    neuralModels_[name] = registry;
}

void PluginNeuralFactory::configureComponent(const std::string& name, const std::map<std::string, std::string>& config) {
    auto it = neuralModels_.find(name);
    if (it != neuralModels_.end()) {
        it->second.config = config;
    }
}

std::map<std::string, std::string> PluginNeuralFactory::getComponentConfig(const std::string& name) const {
    auto it = neuralModels_.find(name);
    if (it != neuralModels_.end()) {
        return it->second.config;
    }
    return std::map<std::string, std::string>();
}

bool PluginNeuralFactory::loadPlugin(const std::string& path) {
    return pluginManager_.loadPlugin(path);
}

bool PluginNeuralFactory::unloadPlugin(const std::string& name) {
    return pluginManager_.unloadPlugin(name);
}

std::vector<std::string> PluginNeuralFactory::getAvailablePlugins() const {
    return pluginManager_.getLoadedPlugins();
}

// PluginLoader implementation
std::map<std::string, std::shared_ptr<void>> PluginLoader::loadedLibraries_;

bool PluginLoader::loadPluginLibrary(const std::string& path) {
    if (loadedLibraries_.find(path) != loadedLibraries_.end()) {
        return true; // Already loaded
    }
    
    // In real implementation, would load the dynamic library
    // For now, just create a placeholder
    loadedLibraries_[path] = std::shared_ptr<void>(nullptr, [path](void*) {
        // Destructor - cleanup
    });
    
    return true;
}

bool PluginLoader::unloadPluginLibrary(const std::string& path) {
    auto it = loadedLibraries_.find(path);
    if (it == loadedLibraries_.end()) {
        return true; // Not loaded
    }
    
    loadedLibraries_.erase(it);
    return true;
}

std::vector<std::string> PluginLoader::getPluginLibraries() const {
    std::vector<std::string> libraries;
    for (const auto& pair : loadedLibraries_) {
        libraries.push_back(pair.first);
    }
    return libraries;
}

std::shared_ptr<void> PluginLoader::loadSymbol(const std::string& libraryPath, const std::string& symbolName) {
    // In real implementation, would load symbol from dynamic library
    return nullptr;
}

// Plugin utility functions namespace
bool plugin_utils::validatePluginName(const std::string& name) {
    if (name.empty()) return false;
    if (name.find(' ') != std::string::npos) return false;
    if (name.find('/') != std::string::npos) return false;
    if (name.find('\\') != std::string::npos) return false;
    if (name.find('*') != std::string::npos) return false;
    if (name.find('?') != std::string::npos) return false;
    
    return true;
}

bool plugin_utils::validatePluginVersion(const std::string& version) {
    if (version.empty()) return false;
    
    // Simple version format validation (semantic versioning)
    std::regex versionPattern(R"(\d+\.\d+\.\d+(-[a-zA-Z0-9]+)?(\+[a-zA-Z0-9]+)?)");
    return std::regex_match(version, versionPattern);
}

std::map<std::string, std::string> plugin_utils::parseConfigString(const std::string& configStr) {
    std::map<std::string, std::string> config;
    
    if (configStr.empty()) return config;
    
    // Simple key=value parsing
    size_t pos = 0;
    while (pos < configStr.size()) {
        size_t equalPos = configStr.find('=', pos);
        if (equalPos == std::string::npos) break;
        
        std::string key = trim(configStr.substr(pos, equalPos - pos));
        size_t nextPos = equalPos + 1;
        
        // Find next delimiter (comma or end)
        size_t commaPos = configStr.find(',', nextPos);
        if (commaPos == std::string::npos) {
            commaPos = configStr.size();
        }
        
        std::string value = trim(configStr.substr(nextPos, commaPos - nextPos));
        
        config[key] = value;
        pos = commaPos + 1;
    }
    
    return config;
}

std::string plugin_utils::formatConfigString(const std::map<std::string, std::string>& config) {
    std::ostringstream oss;
    
    bool first = true;
    for (const auto& pair : config) {
        if (!first) oss << ", ";
        oss << pair.first << "=" << pair.second;
        first = false;
    }
    
    return oss.str();
}

std::string plugin_utils::getPluginTypeString(const std::string& pluginPath) {
    std::filesystem::path path(pluginPath);
    std::string extension = path.extension().string();
    
    if (extension.empty()) return "";
    
    // Remove the dot
    extension = extension.substr(1);
    
    // Normalize extension
    std::transform(extension.begin(), extension.end(), extension.begin(),
                   [](unsigned char c) { return std::tolower(c); });
    
    return extension;
}

std::string plugin_utils::getPluginVersion(const std::string& pluginPath) {
    // In real implementation, would read plugin manifest or header
    return "1.0.0";
}

bool plugin_utils::isPluginCompatible(const std::string& pluginVersion, const std::string& requiredVersion) {
    // Simple version compatibility check
    return pluginVersion >= requiredVersion;
}

std::vector<std::string> plugin_utils::findPlugins(const std::string& directory, const std::string& extension) {
    return PluginManager().scanDirectory(directory);
}

bool plugin_utils::isValidPluginFile(const std::string& filename) {
    std::filesystem::path path(filename);
    return PluginManager().isPluginFile(filename);
}

} // namespace nlm