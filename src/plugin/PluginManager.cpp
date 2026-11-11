#include "PluginManager.hpp"
#include <dlfcn.h>  // For dynamic library loading
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>

namespace nlm {

PluginManager::PluginManager() {}

PluginManager::~PluginManager() {
    clearPlugins();
}

Plugin* PluginManager::loadPlugin(const std::string& filepath) {
    std::lock_guard<std::mutex> lock(mutex);
    
    // Check if plugin is already loaded
    auto it = findPlugin(filepath);
    if (it != plugins.end()) {
        return it->plugin;
    }
    
    // Load plugin from file
    Plugin* plugin = loadPluginFromLibrary(filepath, "");
    
    if (plugin) {
        plugins.push_back({plugin, filepath, std::any()});
    }
    
    return plugin;
}

Plugin* PluginManager::loadPluginFromLibrary(const std::string& libraryPath, const std::string& pluginName) {
    // Open dynamic library
    void* handle = dlopen(libraryPath.c_str(), RTLD_LAZY | RTLD_GLOBAL);
    if (!handle) {
        return nullptr;
    }
    
    // Get create function
    void* createFunc = dlsym(handle, "createPlugin");
    if (!createFunc) {
        dlclose(handle);
        return nullptr;
    }
    
    // Convert function pointer
    using CreateFunc = Plugin*(*)(void);
    CreateFunc create = reinterpret_cast<CreateFunc>(createFunc);
    
    // Create plugin
    Plugin* plugin = create();
    if (!plugin) {
        dlclose(handle);
        return nullptr;
    }
    
    // Set library handle as plugin data
    plugin->setPluginHandle(handle);
    
    return plugin;
}

void PluginManager::unloadPlugin(Plugin* plugin) {
    if (!plugin) return;
    
    std::lock_guard<std::mutex> lock(mutex);
    
    // Find plugin
    auto it = std::find_if(plugins.begin(), plugins.end(),
        [plugin](const PluginInfo& info) { return info.plugin == plugin; });
    
    if (it != plugins.end()) {
        // Shutdown plugin
        it->plugin->shutdown();
        
        // Close library
        void* handle = it->plugin->getPluginHandle();
        if (handle) {
            dlclose(handle);
        }
        
        // Delete plugin
        delete it->plugin;
        
        // Remove from list
        plugins.erase(it);
    }
}

Plugin* PluginManager::getPlugin(const std::string& name) const {
    std::lock_guard<std::mutex> lock(mutex);
    
    auto it = findPlugin(name);
    if (it != plugins.end()) {
        return it->plugin;
    }
    
    return nullptr;
}

std::vector<Plugin*> PluginManager::getAllPlugins() const {
    std::lock_guard<std::mutex> lock(mutex);
    
    std::vector<Plugin*> pluginPtrs;
    for (const auto& info : plugins) {
        if (info.plugin && info.plugin->isEnabled()) {
            pluginPtrs.push_back(info.plugin);
        }
    }
    
    return pluginPtrs;
}

std::vector<Plugin*> PluginManager::getPluginsByType(const std::string& type) const {
    std::lock_guard<std::mutex> lock(mutex);
    
    std::vector<Plugin*> pluginPtrs;
    for (const auto& info : plugins) {
        if (info.plugin && info.plugin->isEnabled() && info.plugin->getType() == type) {
            pluginPtrs.push_back(info.plugin);
        }
    }
    
    return pluginPtrs;
}

void PluginManager::setPluginEnabled(const std::string& name, bool enabled) {
    std::lock_guard<std::mutex> lock(mutex);
    
    auto it = findPlugin(name);
    if (it != plugins.end()) {
        it->plugin->setEnabled(enabled);
    }
}

bool PluginManager::isPluginEnabled(const std::string& name) const {
    std::lock_guard<std::mutex> lock(mutex);
    
    auto it = findPlugin(name);
    if (it != plugins.end()) {
        return it->plugin->isEnabled();
    }
    
    return false;
}

void PluginManager::configurePlugin(const std::string& name, const std::any& config) {
    std::lock_guard<std::mutex> lock(mutex);
    
    auto it = findPlugin(name);
    if (it != plugins.end()) {
        it->plugin->initialize(config);
        it->config = config;
    }
}

void PluginManager::applyPluginsToBrain(class Brain* brain, SimulationStep timestep, Timestamp currentTime) {
    std::lock_guard<std::mutex> lock(mutex);
    
    // Get brain plugins
    auto brainPlugins = getPluginsByType("brain");
    
    // Apply all brain plugins
    for (Plugin* plugin : brainPlugins) {
        if (plugin->isEnabled()) {
            auto brainPlugin = static_cast<BrainPlugin*>(plugin);
            brainPlugin->applyToBrain(brain, timestep, currentTime);
        }
    }
}

void PluginManager::applyPluginsToMemory(class Brain* brain, SimulationStep timestep) {
    std::lock_guard<std::mutex> lock(mutex);
    
    // Get memory plugins
    auto memoryPlugins = getPluginsByType("memory");
    
    // Apply all memory plugins
    for (Plugin* plugin : memoryPlugins) {
        if (plugin->isEnabled()) {
            auto memoryPlugin = static_cast<MemoryPlugin*>(plugin);
            memoryPlugin->applyToMemory(brain, timestep);
        }
    }
}

void PluginManager::applyPluginsToPlasticity(class Brain* brain, SimulationStep timestep, Timestamp currentTime) {
    std::lock_guard<std::mutex> lock(mutex);
    
    // Get plasticity plugins
    auto plasticityPlugins = getPluginsByType("plasticity");
    
    // Apply all plasticity plugins
    for (Plugin* plugin : plasticityPlugins) {
        if (plugin->isEnabled()) {
            auto plasticityPlugin = static_cast<PlasticityPlugin*>(plugin);
            plasticityPlugin->applyToPlasticity(brain, timestep, currentTime);
        }
    }
}

void PluginManager::applyPluginsToNeuromodulation(class Brain* brain, SimulationStep timestep) {
    std::lock_guard<std::mutex> lock(mutex);
    
    // Get neuromodulation plugins
    auto neuromodulationPlugins = getPluginsByType("neuromodulation");
    
    // Apply all neuromodulation plugins
    for (Plugin* plugin : neuromodulationPlugins) {
        if (plugin->isEnabled()) {
            auto neuromodulationPlugin = static_cast<NeuromodulationPlugin*>(plugin);
            neuromodulationPlugin->applyToNeuromodulation(brain, timestep);
        }
    }
}

void PluginManager::savePluginState(const std::string& filepath) {
    std::lock_guard<std::mutex> lock(mutex);
    
    std::ofstream file(filepath);
    if (!file) {
        return;
    }
    
    file << "// Plugin state saved by NLM PluginManager" << std::endl;
    
    for (const auto& info : plugins) {
        file << "Plugin: " << info.plugin->getName() << std::endl;
        file << "Enabled: " << (info.plugin->isEnabled() ? "true" : "false") << std::endl;
        file << "Type: " << info.plugin->getType() << std::endl;
        file << std::endl;
    }
}

void PluginManager::loadPluginState(const std::string& filepath) {
    std::lock_guard<std::mutex> lock(mutex);
    
    std::ifstream file(filepath);
    if (!file) {
        return;
    }
    
    std::string line;
    std::string currentPlugin;
    bool inPlugin = false;
    
    while (std::getline(file, line)) {
        if (line.find("Plugin: ") == 0) {
            currentPlugin = line.substr(8);
            inPlugin = true;
        } else if (line.find("Enabled: ") == 0) {
            if (currentPlugin.empty()) continue;
            
            bool enabled = (line.substr(9) == "true");
            setPluginEnabled(currentPlugin, enabled);
            inPlugin = false;
        }
    }
}

void PluginManager::clearPlugins() {
    std::lock_guard<std::mutex> lock(mutex);
    
    for (auto& info : plugins) {
        if (info.plugin) {
            info.plugin->shutdown();
            delete info.plugin;
        }
    }
    
    plugins.clear();
}

std::vector<PluginManager::PluginInfo>::const_iterator PluginManager::findPlugin(const std::string& name) const {
    return std::find_if(plugins.begin(), plugins.end(),
        [&name](const PluginInfo& info) { return info.plugin->getName() == name; });
}

PluginManager::PluginManager() {}

PluginManager::~PluginManager() {
    clearPlugins();
}

{
        // Shutdown plugin
        it->plugin->shutdown();
        
        // Close library
        void* handle = it->plugin->getPluginHandle();
        if (handle) {
            dlclose(handle);
        }
        
        // Delete plugin
        delete it->plugin;
        
        // Remove from list
        plugins.erase(it);
    }
}

Plugin* PluginManager::getPlugin(const std::string& name) const {
    std::lock_guard<std::mutex> lock(mutex);
    
    auto it = findPlugin(name);
    if (it != plugins.end()) {
        return it->plugin;
    }
    
    return nullptr;
}

std::vector<Plugin*> PluginManager::getAllPlugins() const {
    std::lock_guard<std::mutex> lock(mutex);
    
    std::vector<Plugin*> pluginPtrs;
    for (const auto& info : plugins) {
        if (info.plugin && info.plugin->isEnabled()) {
            pluginPtrs.push_back(info.plugin);
        }
    }
    
    return pluginPtrs;
}

std::vector<Plugin*> PluginManager::getPluginsByType(const std::string& type) const {
    std::lock_guard<std::mutex> lock(mutex);
    
    std::vector<Plugin*> pluginPtrs;
    for (const auto& info : plugins) {
        if (info.plugin && info.plugin->isEnabled() && info.plugin->getType() == type) {
            pluginPtrs.push_back(info.plugin);
        }
    }
    
    return pluginPtrs;
}

void PluginManager::setPluginEnabled(const std::string& name, bool enabled) {
    std::lock_guard<std::mutex> lock(mutex);
    
    auto it = findPlugin(name);
    if (it != plugins.end()) {
        it->plugin->setEnabled(enabled);
    }
}

bool PluginManager::isPluginEnabled(const std::string& name) const {
    std::lock_guard<std::mutex> lock(mutex);
    
    auto it = findPlugin(name);
    if (it != plugins.end()) {
        return it->plugin->isEnabled();
    }
    
    return false;
}

void PluginManager::configurePlugin(const std::string& name, const std::any& config) {
    std::lock_guard<std::mutex> lock(mutex);
    
    auto it = findPlugin(name);
    if (it != plugins.end()) {
        it->plugin->initialize(config);
        it->config = config;
    }
}

void PluginManager::applyPluginsToBrain(class Brain* brain, SimulationStep timestep, Timestamp currentTime) {
    std::lock_guard<std::mutex> lock(mutex);
    
    // Get brain plugins
    auto brainPlugins = getPluginsByType("brain");
    
    // Apply all brain plugins
    for (Plugin* plugin : brainPlugins) {
        if (plugin->isEnabled()) {
            auto brainPlugin = static_cast<BrainPlugin*>(plugin);
            brainPlugin->applyToBrain(brain, timestep, currentTime);
        }
    }
}

void PluginManager::applyPluginsToMemory(class Brain* brain, SimulationStep timestep) {
    std::lock_guard<std::mutex> lock(mutex);
    
    // Get memory plugins
    auto memoryPlugins = getPluginsByType("memory");
    
    // Apply all memory plugins
    for (Plugin* plugin : memoryPlugins) {
        if (plugin->isEnabled()) {
            auto memoryPlugin = static_cast<MemoryPlugin*>(plugin);
            memoryPlugin->applyToMemory(brain, timestep);
        }
    }
}

void PluginManager::applyPluginsToPlasticity(class Brain* brain, SimulationStep timestep, Timestamp currentTime) {
    std::lock_guard<std::mutex> lock(mutex);
    
    // Get plasticity plugins
    auto plasticityPlugins = getPluginsByType("plasticity");
    
    // Apply all plasticity plugins
    for (Plugin* plugin : plasticityPlugins) {
        if (plugin->isEnabled()) {
            auto plasticityPlugin = static_cast<PlasticityPlugin*>(plugin);
            plasticityPlugin->applyToPlasticity(brain, timestep, currentTime);
        }
    }
}

void PluginManager::applyPluginsToNeuromodulation(class Brain* brain, SimulationStep timestep) {
    std::lock_guard<std::mutex> lock(mutex);
    
    // Get neuromodulation plugins
    auto neuromodulationPlugins = getPluginsByType("neuromodulation");
    
    // Apply all neuromodulation plugins
    for (Plugin* plugin : neuromodulationPlugins) {
        if (plugin->isEnabled()) {
            auto neuromodulationPlugin = static_cast<NeuromodulationPlugin*>(plugin);
            neuromodulationPlugin->applyToNeuromodulation(brain, timestep);
        }
    }
}

void PluginManager::savePluginState(const std::string& filepath) {
    std::lock_guard<std::mutex> lock(mutex);
    
    std::ofstream file(filepath);
    if (!file) {
        return;
    }
    
    file << "// Plugin state saved by NLM PluginManager" << std::endl;
    
    for (const auto& info : plugins) {
        file << "Plugin: " << info.plugin->getName() << std::endl;
        file << "Enabled: " << (info.plugin->isEnabled() ? "true" : "false") << std::endl;
        file << "Type: " << info.plugin->getType() << std::endl;
        file << std::endl;
    }
}

void PluginManager::loadPluginState(const std::string& filepath) {
    std::lock_guard<std::mutex> lock(mutex);
    
    std::ifstream file(filepath);
    if (!file) {
        return;
    }
    
    std::string line;
    std::string currentPlugin;
    bool inPlugin = false;
    
    while (std::getline(file, line)) {
        if (line.find("Plugin: ") == 0) {
            currentPlugin = line.substr(8);
            inPlugin = true;
        } else if (line.find("Enabled: ") == 0) {
            if (currentPlugin.empty()) continue;
            
            bool enabled = (line.substr(9) == "true");
            setPluginEnabled(currentPlugin, enabled);
            inPlugin = false;
        }
    }
}

void PluginManager::clearPlugins() {
    std::lock_guard<std::mutex> lock(mutex);
    
    for (auto& info : plugins) {
        if (info.plugin) {
            info.plugin->shutdown();
            delete info.plugin;
        }
    }
    
    plugins.clear();
}

std::vector<PluginManager::PluginInfo>::const_iterator PluginManager::findPlugin(const std::string& name) const {
    return std::find_if(plugins.begin(), plugins.end(),
        [&name](const PluginInfo& info) { return info.plugin->getName() == name; });
}

PluginManager::PluginManager() {}

PluginManager::~PluginManager() {
    clearPlugins();
}

Plugin* PluginManager::loadPluginFromLibrary(const std::string& libraryPath, const std::string& pluginName) {
    // Open dynamic library
    void* handle = dlopen(libraryPath.c_str(), RTLD_LAZY | RTLD_GLOBAL);
    if (!handle) {
        return nullptr;
    }
    
    // Get create function
    void* createFunc = dlsym(handle, "createPlugin");
    if (!createFunc) {
        dlclose(handle);
        return nullptr;
    }
    
    // Convert function pointer
    using CreateFunc = Plugin*(*)(void);
    CreateFunc create = reinterpret_cast<CreateFunc>(createFunc);
    
    // Create plugin
    Plugin* plugin = create();
    if (!plugin) {
        dlclose(handle);
        return nullptr;
    }
    
    // Set library handle as plugin data
    plugin->setPluginHandle(handle);
    
    return plugin;
}

} // namespace nlm