// PluginInterface.cpp implementation
#include "PluginInterface.hpp"
#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>

namespace nlm {

// DefaultPluginFactory::Impl implementation
struct DefaultPluginFactory::Impl {
    std::string pluginDirectory;
    std::unordered_map<std::string, std::shared_ptr<NLMPlugin>> plugins;
    std::vector<std::string> loadedPluginNames;
    std::mutex mutex;
    
    Impl() : pluginDirectory(".") {}
};

DefaultPluginFactory::DefaultPluginFactory() : pImpl(std::make_unique<Impl>()) {}

DefaultPluginFactory::~DefaultPluginFactory() = default;

bool DefaultPluginFactory::loadPlugin(const std::string& filename) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    if (pImpl->plugins.find(filename) != pImpl->plugins.end()) {
        return false; // Already loaded
    }
    
    try {
        // For now, create a dummy plugin
        // In a real implementation, this would load from a shared library
        auto plugin = std::make_shared<BasicPlugin<NLMPlugin>>(
            "test_plugin", "1.0.0", "Test plugin", "Test Author");
        
        // Initialize plugin
        std::unordered_map<std::string, std::string> config;
        if (!plugin->initialize(config)) {
            std::cerr << "Failed to initialize plugin: " << filename << std::endl;
            return false;
        }
        
        pImpl->plugins[filename] = plugin;
        pImpl->loadedPluginNames.push_back(filename);
        
    } catch (const std::exception& e) {
        std::cerr << "Failed to load plugin " << filename << ": " << e.what() << std::endl;
        return false;
    }
    
    return true;
}

std::shared_ptr<NLMPlugin> DefaultPluginFactory::createPlugin(const std::string& name) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    auto it = pImpl->plugins.find(name);
    if (it != pImpl->plugins.end()) {
        return it->second;
    }
    
    return nullptr;
}

bool DefaultPluginFactory::unloadPlugin(const std::string& name) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    auto it = pImpl->plugins.find(name);
    if (it == pImpl->plugins.end()) {
        return false;
    }
    
    it->second->shutdown();
    pImpl->plugins.erase(it);
    
    auto it2 = std::find(pImpl->loadedPluginNames.begin(), 
                        pImpl->loadedPluginNames.end(), name);
    if (it2 != pImpl->loadedPluginNames.end()) {
        pImpl->loadedPluginNames.erase(it2);
    }
    
    return true;
}

bool DefaultPluginFactory::hasPlugin(const std::string& name) const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    return pImpl->plugins.find(name) != pImpl->plugins.end();
}

std::vector<std::string> DefaultPluginFactory::getLoadedPlugins() const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    return pImpl->loadedPluginNames;
}

void DefaultPluginFactory::setPluginDirectory(const std::string& directory) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->pluginDirectory = directory;
}

std::string DefaultPluginFactory::getPluginDirectory() const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    return pImpl->pluginDirectory;
}

std::vector<std::string> DefaultPluginFactory::scanPlugins(const std::string& directory) const {
    std::vector<std::string> plugins;
    
    if (!std::filesystem::exists(directory) || !std::filesystem::is_directory(directory)) {
        return plugins;
    }
    
    for (const auto& entry : std::filesystem::directory_iterator(directory)) {
        if (entry.is_regular_file() && isPluginFile(entry.path().string())) {
            plugins.push_back(entry.path().string());
        }
    }
    
    return plugins;
}

bool DefaultPluginFactory::isPluginFile(const std::string& filename) const {
    std::string extension = std::filesystem::path(filename).extension().string();
    std::transform(extension.begin(), extension.end(), extension.begin(), ::tolower);
    
    // Check for common plugin extensions
    static const std::set<std::string> pluginExtensions = {
        ".so", ".dll", ".dylib", ".plugin", ".nlm", ".p", ".plugin"
    };
    
    return pluginExtensions.find(extension) != pluginExtensions.end();
}

// PluginManager::Impl implementation
struct PluginManager::Impl {
    std::shared_ptr<PluginFactory> factory;
    std::string pluginDirectory;
    std::mutex mutex;
    
    Impl() {
        factory = std::make_shared<DefaultPluginFactory>();
    }
};

PluginManager::PluginManager() : pImpl(std::make_unique<Impl>()) {}

PluginManager::~PluginManager() {
    shutdownAll();
}

PluginManager::PluginManager(PluginManager&& other) noexcept
    : pImpl(std::move(other.pImpl)) {}

PluginManager& PluginManager::operator=(PluginManager&& other)&& {
    if (this != &other) {
        pImpl = std::move(other.pImpl);
    }
    return *this;
}

void PluginManager::setPluginFactory(std::shared_ptr<PluginFactory> factory) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->factory = factory;
}

bool PluginManager::loadPlugin(const std::string& filename) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    if (!pImpl->factory) {
        return false;
    }
    
    return pImpl->factory->loadPlugin(filename);
}

bool PluginManager::loadPluginByName(const std::string& name) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    if (!pImpl->factory) {
        return false;
    }
    
    // Try to load from plugin directory
    std::string fullPath = pImpl->pluginDirectory + "/" + name;
    
    if (pImpl->factory->loadPlugin(fullPath)) {
        return true;
    }
    
    // Try direct load
    return pImpl->factory->loadPlugin(name);
}

bool PluginManager::unloadPlugin(const std::string& name) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    if (!pImpl->factory) {
        return false;
    }
    
    return pImpl->factory->unloadPlugin(name);
}

std::vector<std::string> PluginManager::getLoadedPlugins() const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    if (!pImpl->factory) {
        return std::vector<std::string>();
    }
    
    return pImpl->factory->getLoadedPlugins();
}

std::shared_ptr<NLMPlugin> PluginManager::getPlugin(const std::string& name) const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    if (!pImpl->factory) {
        return nullptr;
    }
    
    return pImpl->factory->createPlugin(name);
}

bool PluginManager::initializeAll(const std::unordered_map<std::string, std::unordered_map<std::string, std::string>>& configs) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    if (!pImpl->factory) {
        return false;
    }
    
    for (const auto& pair : pImpl->factory->getLoadedPlugins()) {
        auto plugin = pImpl->factory->createPlugin(pair);
        if (!plugin) {
            continue;
        }
        
        // Get configuration for this plugin
        auto configIt = configs.find(pair);
        std::unordered_map<std::string, std::string> config;
        if (configIt != configs.end()) {
            config = configIt->second;
        }
        
        if (!plugin->initialize(config)) {
            std::cerr << "Failed to initialize plugin: " << pair << std::endl;
            return false;
        }
    }
    
    return true;
}

void PluginManager::shutdownAll() {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    if (!pImpl->factory) {
        return;
    }
    
    for (const auto& name : pImpl->factory->getLoadedPlugins()) {
        pImpl->factory->unloadPlugin(name);
    }
}

bool PluginManager::executePluginAction(const std::string& pluginName,
                                       const std::string& action,
                                       const std::vector<std::string>& args) {
    auto plugin = getPlugin(pluginName);
    if (!plugin) {
        return false;
    }
    
    return plugin->execute(action, args);
}

bool PluginManager::isPluginInitialized(const std::string& name) const {
    auto plugin = getPlugin(name);
    if (!plugin) {
        return false;
    }
    
    return plugin->isInitialized();
}

std::string PluginManager::getPluginInfo(const std::string& name) const {
    auto plugin = getPlugin(name);
    if (!plugin) {
        return "Plugin not found: " + name;
    }
    
    std::stringstream ss;
    ss << "Plugin: " << plugin->getName() << std::endl;
    ss << "Version: " << plugin->getVersion() << std::endl;
    ss << "Description: " << plugin->getDescription() << std::endl;
    ss << "Author: " << plugin->getAuthor() << std::endl;
    ss << "Dependencies: ";
    bool first = true;
    for (const auto& dep : plugin->getDependencies()) {
        if (!first) ss << ", ";
        ss << dep;
        first = false;
    }
    ss << std::endl;
    ss << "Capabilities: ";
    first = true;
    for (const auto& cap : plugin->getCapabilities()) {
        if (!first) ss << ", ";
        ss << cap;
        first = false;
    }
    ss << std::endl;
    ss << "Initialized: " << (plugin->isInitialized() ? "Yes" : "No") << std::endl;
    
    return ss.str();
}

std::string PluginManager::getAllPluginsInfo() const {
    std::stringstream ss;
    ss << "=== Loaded Plugins ===" << std::endl;
    
    for (const auto& name : getLoadedPlugins()) {
        ss << getPluginInfo(name) << std::endl;
        ss << "---" << std::endl;
    }
    
    return ss.str();
}

void PluginManager::setPluginDirectory(const std::string& directory) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->pluginDirectory = directory;
    
    if (pImpl->factory) {
        std::dynamic_pointer_cast<DefaultPluginFactory>(pImpl->factory)->setPluginDirectory(directory);
    }
}

std::string PluginManager::getPluginDirectory() const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    return pImpl->pluginDirectory;
}

// BasicPlugin implementation
template<typename T>
BasicPlugin<T>::BasicPlugin(const std::string& name, const std::string& version,
                          const std::string& description, const std::string& author)
    : name_(name), version_(version), description_(description), author_(author),
      initialized_(false) {}

template<typename T>
bool BasicPlugin<T>::initialize(const std::unordered_map<std::string, std::string>& config) {
    if (initialized_) {
        return true;
    }
    
    config_ = config;
    
    if (!initializeImpl(config)) {
        return false;
    }
    
    initialized_ = true;
    return true;
}

template<typename T>
void BasicPlugin<T>::shutdown() {
    if (!initialized_) {
        return;
    }
    
    shutdownImpl();
    initialized_ = false;
    config_.clear();
}

template<typename T>
bool BasicPlugin<T>::execute(const std::string& action, const std::vector<std::string>& args) {
    if (!initialized_) {
        return false;
    }
    
    return executeImpl(action, args);
}

template<typename T>
std::unordered_map<std::string, std::string> BasicPlugin<T>::getConfigSchema() const {
    return getConfigSchemaImpl();
}

template<typename T>
bool BasicPlugin<T>::validateConfig(const std::unordered_map<std::string, std::string>& config) const {
    return validateConfigImpl(config);
}

template<typename T>
void BasicPlugin<T>::addCapability(const std::string& capability) {
    capabilities_.push_back(capability);
}

template<typename T>
void BasicPlugin<T>::addDependency(const std::string& dependency) {
    dependencies_.push_back(dependency);
}

} // namespace nlm
