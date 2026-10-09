#include "PluginManager.hpp"
#include <dlfcn.h>
#include <algorithm>
#include <filesystem>
#include <sstream>
#include <iomanip>

namespace nlm {

PluginManager::PluginManager() {
    NLM_LOG_INFO("PluginManager initialized");
}

PluginManager::~PluginManager() {
    shutdownPlugins();
    NLM_LOG_INFO("PluginManager shutdown");
}

bool PluginManager::registerPlugin(std::shared_ptr<PluginInterface> plugin) {
    if (!plugin) {
        NLM_LOG_ERROR("Cannot register null plugin");
        return false;
    }
    
    std::string name = plugin->getName();
    
    // Check if plugin already exists
    if (plugins_.find(name) != plugins_.end()) {
        NLM_LOG_WARN("Plugin already registered: " + name);
        return false;
    }
    
    // Validate plugin
    if (!validatePlugin(plugin)) {
        NLM_LOG_ERROR("Plugin validation failed: " + name);
        return false;
    }
    
    // Register plugin
    plugins_[name] = plugin;
    
    // Update type cache
    typeCache_[std::type_index(typeid(*plugin))].push_back(plugin);
    
    onPluginLoaded(plugin);
    
    NLM_LOG_INFO("Plugin registered: " + name + " (" + plugin->getDescription() + ")");
    return true;
}

bool PluginManager::unregisterPlugin(const std::string& name) {
    auto it = plugins_.find(name);
    if (it == plugins_.end()) {
        NLM_LOG_ERROR("Plugin not found: " + name);
        return false;
    }
    
    // Check dependencies
    if (dependencies_.find(name) != dependencies_.end() && !dependencies_.at(name).empty()) {
        NLM_LOG_ERROR("Cannot unregister plugin with dependencies: " + name);
        return false;
    }
    
    // Get plugin for callbacks
    auto plugin = it->second;
    
    // Remove from type cache
    auto typeIt = typeCache_.find(std::type_index(typeid(*plugin)));
    if (typeIt != typeCache_.end()) {
        auto& pluginList = typeIt->second;
        pluginList.erase(std::remove_if(pluginList.begin(), pluginList.end(),
                                        [&plugin](const std::shared_ptr<PluginInterface>& p) {
                                            return p.get() == plugin.get();
                                        }),
                        pluginList.end());
        if (pluginList.empty()) {
            typeCache_.erase(typeIt);
        }
    }
    
    // Remove from main registry
    plugins_.erase(it);
    
    // Remove dependencies
    dependencies_.erase(name);
    
    // Remove configuration
    pluginConfigs_.erase(name);
    
    // Remove statistics
    pluginStats_.erase(name);
    
    onPluginUnloaded(plugin);
    
    NLM_LOG_INFO("Plugin unregistered: " + name);
    return true;
}

std::shared_ptr<PluginInterface> PluginManager::getPlugin(const std::string& name) {
    auto it = plugins_.find(name);
    if (it == plugins_.end()) {
        return nullptr;
    }
    return it->second;
}

template<typename T>
std::shared_ptr<T> PluginManager::getPlugin() {
    auto typeIt = typeCache_.find(std::type_index(typeid(T)));
    if (typeIt == typeCache_.end() || typeIt->second.empty()) {
        return nullptr;
    }
    
    // Return first plugin of type T
    for (const auto& plugin : typeIt->second) {
        auto typedPlugin = std::dynamic_pointer_cast<T>(plugin);
        if (typedPlugin) {
            return typedPlugin;
        }
    }
    
    return nullptr;
}

template<typename T>
std::vector<std::shared_ptr<T>> PluginManager::getPluginsOfType() {
    std::vector<std::shared_ptr<T>> result;
    auto typeIt = typeCache_.find(std::type_index(typeid(T)));
    if (typeIt != typeCache_.end()) {
        for (const auto& plugin : typeIt->second) {
            auto typedPlugin = std::dynamic_pointer_cast<T>(plugin);
            if (typedPlugin) {
                result.push_back(typedPlugin);
            }
        }
    }
    return result;
}

bool PluginManager::initializePlugins(std::shared_ptr<Config> config) {
    if (!config) {
        NLM_LOG_ERROR("Cannot initialize plugins: null config");
        return false;
    }
    
    bool success = true;
    
    // Sort plugins by dependencies
    std::vector<std::string> pluginOrder;
    if (!sortPluginsByDependencies(pluginOrder)) {
        NLM_LOG_ERROR("Failed to sort plugins by dependencies");
        return false;
    }
    
    // Initialize plugins in dependency order
    for (const std::string& name : pluginOrder) {
        auto plugin = plugins_[name];
        
        if (!plugin->isEnabled()) {
            NLM_LOG_DEBUG("Skipping disabled plugin: " + name);
            continue;
        }
        
        // Set plugin config
        if (pluginConfigs_.find(name) != pluginConfigs_.end()) {
            for (const auto& pair : pluginConfigs_[name]) {
                config->set(pair.first, pair.second);
            }
        }
        
        // Initialize plugin
        auto startTime = std::chrono::high_resolution_clock::now();
        
        if (!plugin->initialize(config)) {
            NLM_LOG_ERROR("Failed to initialize plugin: " + name);
            success = false;
            continue;
        }
        
        auto endTime = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime).count();
        
        // Update plugin stats
        if (pluginStats_.find(name) == pluginStats_.end()) {
            pluginStats_[name] = PluginStats{name, duration, duration, 1, 0, 0.0};
        } else {
            pluginStats_[name].loadTime = duration;
            pluginStats_[name].callCount++;
        }
        
        onPluginEnabled(plugin);
        
        NLM_LOG_INFO("Plugin initialized: " + name);
    }
    
    return success;
}

void PluginManager::shutdownPlugins() {
    // Shutdown plugins in reverse dependency order
    std::vector<std::string> pluginOrder;
    if (sortPluginsByDependencies(pluginOrder)) {
        std::reverse(pluginOrder.begin(), pluginOrder.end());
        
        for (const std::string& name : pluginOrder) {
            auto plugin = plugins_[name];
            
            if (plugin->isEnabled()) {
                plugin->shutdown();
                onPluginDisabled(plugin);
                
                NLM_LOG_INFO("Plugin shutdown: " + name);
            }
        }
    }
    
    // Clear all plugin data
    plugins_.clear();
    dependencies_.clear();
    pluginConfigs_.clear();
    pluginStats_.clear();
    typeCache_.clear();
    pluginDiscoveryCache_.clear();
}

bool PluginManager::enablePlugin(const std::string& name) {
    auto plugin = getPlugin(name);
    if (!plugin) {
        NLM_LOG_ERROR("Plugin not found: " + name);
        return false;
    }
    
    plugin->setEnabled(true);
    NLM_LOG_INFO("Plugin enabled: " + name);
    return true;
}

bool PluginManager::disablePlugin(const std::string& name) {
    auto plugin = getPlugin(name);
    if (!plugin) {
        NLM_LOG_ERROR("Plugin not found: " + name);
        return false;
    }
    
    plugin->setEnabled(false);
    NLM_LOG_INFO("Plugin disabled: " + name);
    return true;
}

bool PluginManager::isPluginEnabled(const std::string& name) const {
    auto plugin = getPlugin(name);
    if (!plugin) {
        return false;
    }
    return plugin->isEnabled();
}

bool PluginManager::loadPluginsFromDirectory(const std::string& directory) {
    if (!std::filesystem::exists(directory)) {
        NLM_LOG_ERROR("Plugin directory does not exist: " + directory);
        return false;
    }
    
    std::vector<std::string> pluginFiles = discoverPlugins(directory);
    
    bool success = true;
    for (const std::string& pluginFile : pluginFiles) {
        try {
            auto plugin = PluginLoader().loadPlugin(pluginFile, "");
            if (plugin) {
                if (registerPlugin(plugin)) {
                    NLM_LOG_INFO("Plugin loaded from: " + pluginFile);
                } else {
                    NLM_LOG_ERROR("Failed to register plugin from: " + pluginFile);
                    success = false;
                }
            } else {
                NLM_LOG_ERROR("Failed to load plugin: " + pluginFile);
                success = false;
            }
        } catch (const std::exception& e) {
            NLM_LOG_ERROR("Exception loading plugin " + pluginFile + ": " + std::string(e.what()));
            success = false;
        }
    }
    
    return success;
}

bool PluginManager::savePluginConfig(const std::string& filename) const {
    std::ofstream file(filename);
    if (!file.is_open()) {
        NLM_LOG_ERROR("Failed to open plugin config file for writing: " + filename);
        return false;
    }
    
    // Write plugin configuration in JSON format
    file << "{\n";
    file << "  \"plugin_manager_version\": \"1.0\",\n";
    file << "  \"plugins\": [\n";
    
    size_t pluginCount = 0;
    for (const auto& pair : plugins_) {
        const std::string& name = pair.first;
        const auto& plugin = pair.second;
        
        if (!plugin->isEnabled()) {
            continue; // Skip disabled plugins
        }
        
        file << "    {\n";
        file << "      \"name\": \"" << name << "\",\n";
        file << "      \"version\": \"" << plugin->getVersion() << "\",\n";
        file << "      \"enabled\": " << (plugin->isEnabled() ? "true" : "false") << ",\n";
        file << "      \"config\": ";
        
        if (pluginConfigs_.find(name) != pluginConfigs_.end()) {
            file << "{\n";
            for (size_t i = 0; i < pluginConfigs_.at(name).size(); ++i) {
                const auto& configPair = pluginConfigs_.at(name)[i];
                file << "        \"" << configPair.first << "\": ";
                
                // Write config value based on type
                const ConfigValue& value = configPair.second;
                std::visit([&file](auto&& arg) {
                    using T = std::decay_t<decltype(arg)>;
                    if constexpr (std::is_same_v<T, std::string>) {
                        file << "\"" << arg << "\"";
                    } else if constexpr (std::is_same_v<T, int>) {
                        file << arg;
                    } else if constexpr (std::is_same_v<T, double>) {
                        file << std::fixed << std::setprecision(6) << arg;
                    } else if constexpr (std::is_same_v<T, bool>) {
                        file << (arg ? "true" : "false");
                    } else {
                        file << "null";
                    }
                }, value);
                
                if (i < pluginConfigs_.at(name).size() - 1) {
                    file << ",\n";
                } else {
                    file << "\n";
                }
            }
            file << "      }\n";
        } else {
            file << "{}";
        }
        
        file << "    }";
        if (++pluginCount < plugins_.size()) {
            file << ",";
        }
        file << "\n";
    }
    
    file << "  ]\n";
    file << "}\n";
    
    NLM_LOG_INFO("Plugin configuration saved to: " + filename);
    return true;
}

bool PluginManager::loadPluginConfig(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        NLM_LOG_ERROR("Failed to open plugin config file for reading: " + filename);
        return false;
    }
    
    try {
        // Simple JSON parsing (could be enhanced with proper JSON library)
        std::string line;
        std::string content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
        
        // For now, just clear existing config and reload
        pluginConfigs_.clear();
        
        NLM_LOG_INFO("Plugin configuration loaded from: " + filename);
        return true;
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Exception during plugin config load: " + std::string(e.what()));
        return false;
    }
}

std::vector<std::string> PluginManager::getPluginNames() const {
    std::vector<std::string> names;
    names.reserve(plugins_.size());
    
    for (const auto& pair : plugins_) {
        names.push_back(pair.first);
    }
    
    return names;
}

std::vector<PluginManager::PluginInfo> PluginManager::getPluginInfo() const {
    std::vector<PluginInfo> infoList;
    
    for (const auto& pair : plugins_) {
        const std::string& name = pair.first;
        const auto& plugin = pair.second;
        
        PluginInfo info;
        info.name = name;
        info.version = plugin->getVersion();
        info.description = plugin->getDescription();
        info.type = "Unknown"; // Could be enhanced to detect plugin type
        info.enabled = plugin->isEnabled();
        info.filePath = "unknown"; // Would need to track file path
        
        infoList.push_back(info);
    }
    
    return infoList;
}

std::vector<std::string> PluginManager::discoverPlugins(const std::string& directory) const {
    std::vector<std::string> pluginFiles;
    
    try {
        for (const auto& entry : std::filesystem::directory_iterator(directory)) {
            if (entry.is_regular_file()) {
                std::string filename = entry.path().string();
                
                // Check if file is a shared library
                if (filename.ends_with(".so") || filename.ends_with(".dylib") || 
                    filename.ends_with(".dll") || filename.ends_with(".plugin")) {
                    pluginFiles.push_back(filename);
                }
            }
        }
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Exception during plugin discovery: " + std::string(e.what()));
    }
    
    return pluginFiles;
}

bool PluginManager::validatePlugin(std::shared_ptr<PluginInterface> plugin) const {
    if (!plugin) {
        return false;
    }
    
    std::string name = plugin->getName();
    std::string version = plugin->getVersion();
    std::string description = plugin->getDescription();
    
    // Basic validation
    if (name.empty() || version.empty()) {
        NLM_LOG_ERROR("Plugin validation failed: name or version is empty");
        return false;
    }
    
    // Check if name contains valid characters
    if (!std::all_of(name.begin(), name.end(),
                    [](char c) { return std::isalnum(c) || c == '_' || c == '-'; })) {
        NLM_LOG_ERROR("Plugin validation failed: invalid name format: " + name);
        return false;
    }
    
    return true;
}

bool PluginManager::addPluginDependency(const std::string& pluginName, const std::string& dependencyName) {
    if (!hasPlugin(pluginName) || !hasPlugin(dependencyName)) {
        NLM_LOG_ERROR("Plugin dependency failed: plugin not found");
        return false;
    }
    
    dependencies_[pluginName].push_back(dependencyName);
    NLM_LOG_DEBUG("Plugin dependency added: " + pluginName + " -> " + dependencyName);
    return true;
}

bool PluginManager::removePluginDependency(const std::string& pluginName, const std::string& dependencyName) {
    auto it = dependencies_.find(pluginName);
    if (it == dependencies_.end()) {
        return false;
    }
    
    auto& depList = it->second;
    auto depIt = std::find(depList.begin(), depList.end(), dependencyName);
    if (depIt == depList.end()) {
        return false;
    }
    
    depList.erase(depIt);
    
    if (depList.empty()) {
        dependencies_.erase(it);
    }
    
    NLM_LOG_DEBUG("Plugin dependency removed: " + pluginName + " -> " + dependencyName);
    return true;
}

std::vector<std::string> PluginManager::getPluginDependencies(const std::string& pluginName) const {
    auto it = dependencies_.find(pluginName);
    if (it == dependencies_.end()) {
        return std::vector<std::string>();
    }
    
    return it->second;
}

void PluginManager::setPluginConfig(const std::string& pluginName, const std::string& key, const ConfigValue& value) {
    pluginConfigs_[pluginName][key] = value;
    
    // Update enabled plugin config if plugin is loaded
    if (plugins_.find(pluginName) != plugins_.end() && plugins_[pluginName]->isEnabled()) {
        // Plugin config will be applied during initialization
        NLM_LOG_DEBUG("Plugin config updated: " + pluginName + " -> " + key);
    }
}

std::optional<ConfigValue> PluginManager::getPluginConfig(const std::string& pluginName, const std::string& key) const {
    auto pluginIt = pluginConfigs_.find(pluginName);
    if (pluginIt == pluginConfigs_.end()) {
        return std::nullopt;
    }
    
    auto configIt = pluginIt->second.find(key);
    if (configIt == pluginIt->second.end()) {
        return std::nullopt;
    }
    
    return configIt->second;
}

std::vector<PluginManager::PluginStats> PluginManager::getPluginStats() const {
    std::vector<PluginStats> statsList;
    statsList.reserve(pluginStats_.size());
    
    for (const auto& pair : pluginStats_) {
        statsList.push_back(pair.second);
    }
    
    return statsList;
}

bool PluginManager::hasPlugin(const std::string& name) const {
    return plugins_.find(name) != plugins_.end();
}

bool PluginManager::sortPluginsByDependencies(std::vector<std::string>& pluginOrder) const {
    std::map<std::string, std::vector<std::string>> graph = dependencies_;
    std::unordered_map<std::string, int> inDegree;
    std::queue<std::string> zeroDegreeQueue;
    
    // Calculate in-degree for each node
    for (const auto& pair : graph) {
        const std::string& node = pair.first;
        inDegree[node] = 0; // Initialize
    }
    
    for (const auto& pair : graph) {
        const std::string& node = pair.first;
        const auto& edges = pair.second;
        
        for (const std::string& edge : edges) {
            if (inDegree.find(edge) != inDegree.end()) {
                inDegree[edge]++;
            } else {
                inDegree[edge] = 1;
            }
        }
    }
    
    // Add nodes with zero in-degree to queue
    for (const auto& pair : inDegree) {
        if (pair.second == 0) {
            zeroDegreeQueue.push(pair.first);
        }
    }
    
    // Topological sort
    while (!zeroDegreeQueue.empty()) {
        std::string current = zeroDegreeQueue.front();
        zeroDegreeQueue.pop();
        
        pluginOrder.push_back(current);
        
        // Decrease in-degree of neighbors
        if (graph.find(current) != graph.end()) {
            for (const std::string& neighbor : graph.at(current)) {
                inDegree[neighbor]--;
                if (inDegree[neighbor] == 0) {
                    zeroDegreeQueue.push(neighbor);
                }
            }
        }
    }
    
    // Check for cycles
    if (pluginOrder.size() != inDegree.size()) {
        NLM_LOG_ERROR("Plugin dependency cycle detected");
        return false;
    }
    
    return true;
}

// PluginLoader implementation
PluginLoader::PluginLoader() {
    // Initialize with default library paths
    libraryPaths_ = {
        "./plugins",
        "plugins",
        "./lib",
        "lib",
        "~/.local/lib/nlm/plugins"
    };
}

PluginLoader::~PluginLoader() {
    // Unload all plugins
    for (const auto& pair : pluginLibraries_) {
        dlclose(pair.second);
    }
}

std::shared_ptr<PluginInterface> PluginLoader::loadPlugin(const std::string& libraryPath,
                                                         const std::string& pluginName) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    // Check if plugin already loaded
    for (const auto& pair : pluginLibraries_) {
        if (pair.first == libraryPath) {
            NLM_LOG_ERROR("Plugin already loaded: " + libraryPath);
            return nullptr;
        }
    }
    
    // Open shared library
    void* handle = dlopen(libraryPath.c_str(), RTLD_LAZY | RTLD_LOCAL);
    if (!handle) {
        NLM_LOG_ERROR("Failed to load plugin library: " + libraryPath + " - " + dlerror());
        return nullptr;
    }
    
    // Get plugin factory function
    std::string symbolName = getPluginSymbolName(pluginName);
    void* factoryFunc = dlsym(handle, symbolName.c_str());
    if (!factoryFunc) {
        NLM_LOG_ERROR("Failed to find plugin factory symbol: " + symbolName + " - " + dlerror());
        dlclose(handle);
        return nullptr;
    }
    
    // Convert factory function to callable
    auto createPluginFunc = reinterpret_cast<PluginFactoryFunc>(factoryFunc);
    
    // Create plugin instance (requires Config)
    std::shared_ptr<PluginInterface> plugin = nullptr;
    try {
        plugin = createPluginFunc(nullptr); // Would need proper config in real implementation
        if (plugin) {
            pluginLibraries_[libraryPath] = handle;
            NLM_LOG_INFO("Plugin loaded successfully: " + libraryPath);
        } else {
            NLM_LOG_ERROR("Plugin factory returned null: " + libraryPath);
            dlclose(handle);
        }
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Exception loading plugin: " + std::string(e.what()));
        dlclose(handle);
    }
    
    return plugin;
}

bool PluginLoader::unloadPlugin(const std::string& pluginName) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    // Find library path for plugin
    for (auto it = pluginLibraries_.begin(); it != pluginLibraries_.end(); ++it) {
        // This is a simplified implementation - real implementation would need
        // to track which library contains which plugin
        if (pluginName.empty()) { // If pluginName is empty, unload last loaded
            dlclose(it->second);
            pluginLibraries_.erase(it);
            NLM_LOG_INFO("Plugin unloaded: " + it->first);
            return true;
        }
    }
    
    NLM_LOG_ERROR("Plugin not found for unloading: " + pluginName);
    return false;
}

std::vector<std::string> PluginLoader::getLoadedPluginNames() const {
    std::vector<std::string> names;
    names.reserve(pluginLibraries_.size());
    
    for (const auto& pair : pluginLibraries_) {
        names.push_back(pair.first);
    }
    
    return names;
}

const std::string PluginLoader::getPluginSymbolName(const std::string& pluginName) {
    return "create_plugin_" + pluginName;
}

} // namespace nlm

// PluginFactory implementation (example)
namespace nlm {
    
    class DefaultPluginFactory : public PluginFactory {
    public:
        std::shared_ptr<PluginInterface> createPlugin(const std::string& name,
                                                     std::shared_ptr<Config> config) override {
            // Create default plugin based on name
            if (name == "neuron_model_lif") {
                // Create LIF neuron model plugin
                return nullptr; // Placeholder
            } else if (name == "plasticity_stdp") {
                // Create STDP plasticity plugin
                return nullptr; // Placeholder
            }
            return nullptr;
        }
        
        std::vector<std::string> getSupportedPluginTypes() const override {
            return {"neuron_model_lif", "plasticity_stdp", "neuromodulator_dopamine",
                   "sensor_vision", "actuator_motors", "memory_episodic"};
        }
        
        bool validatePluginConfig(const std::string& pluginName,
                                 const std::map<std::string, ConfigValue>& config) const override {
            // Basic validation
            if (pluginName.empty()) {
                return false;
            }
            
            // Check required configuration keys
            if (pluginName == "neuron_model_lif") {
                return config.find("neuron_params") != config.end();
            } else if (pluginName == "plasticity_stdp") {
                return config.find("learning_rate") != config.end();
            }
            
            return true;
        }
    };

} // namespace nlm

// Plugin registration macro
#define REGISTER_PLUGIN(PluginClass) \
    extern "C" std::shared_ptr<nlm::PluginInterface> create_plugin(const std::string& name) { \
        return std::make_shared<PluginClass>(); \
    }

// Plugin base class template
class BasePlugin : public nlm::PluginInterface {
public:
    std::string getName() const override { return "BasePlugin"; }
    std::string getVersion() const override { return "1.0.0"; }
    std::string getDescription() const override { return "Base plugin class"; }
    bool initialize(std::shared_ptr<Config> config) override { return true; }
    void shutdown() override {}
    bool isEnabled() const override { return enabled_; }
    void setEnabled(bool enabled) override { enabled_ = enabled; }
    
protected:
    bool enabled_ = true;
};